# 1. URL Shortener (TinyURL)

### Step 1: Requirements Clarification

**Functional Requirements:**

- Generate short URL from long URL
- Redirect short URL to original URL
- Custom aliases (optional)
- Expiration time for URLs
- Analytics/click tracking

**Non-Functional Requirements:**

- High availability (99.9%)
- Low latency (<100ms for redirects)
- Read-heavy: 100:1 read-to-write ratio
- URL uniqueness guaranteed
- Scale: 100M new URLs per month


### Step 2: Capacity Estimation

```
Traffic Estimation:
DAU: 100M users
URLs created per day: 100M / 30 = 3.3M
Write QPS: 3.3M / 86400 ≈ 40 QPS
Read QPS: 40 × 100 = 4,000 QPS
Peak QPS: 4,000 × 3 = 12,000 QPS

Storage Estimation (5 years):
Total URLs: 3.3M × 365 × 5 = 6B URLs
Storage per URL:
  - short_code: 7 bytes
  - long_url: 500 bytes (avg)
  - created_at: 8 bytes
  - user_id: 8 bytes
  - metadata: 20 bytes
  Total: ~550 bytes/URL

Total Storage: 6B × 550 bytes = 3.3 TB
With replication (3x): 10 TB

Base62 Encoding:
62^7 = 3.5 trillion possible URLs
At 3.3M/day: 3.5T / 3.3M = ~1M days = 2,900 years

Bandwidth:
Write: 40 QPS × 550 bytes = 22 KB/s
Read: 4,000 QPS × 550 bytes = 2.2 MB/s

Cache Size (80/20 rule - 20% URLs get 80% traffic):
Daily hot URLs: 4,000 × 86,400 = 345M reads/day
Unique hot URLs: 345M × 0.2 = 69M URLs
Cache size: 69M × 550 bytes ≈ 38 GB
```


### Step 3: API Design

```
POST /v1/urls
Content-Type: application/json
Authorization: Bearer <token>

Request:
{
  "long_url": "https://www.example.com/very/long/path?param=value",
  "custom_alias": "my-link",           // optional
  "expiration_date": "2025-12-31",     // optional
  "user_id": "user_123"
}

Response: 201 Created
{
  "short_url": "https://tinyurl.com/abc123x",
  "short_code": "abc123x",
  "long_url": "https://www.example.com/very/long/path?param=value",
  "created_at": "2025-10-03T05:00:00Z",
  "expires_at": "2025-12-31T23:59:59Z"
}

GET /v1/urls/{short_code}
Response: 302 Found
Location: <long_url>
X-Cache: HIT

GET /v1/urls/{short_code}/analytics
Response: 200 OK
{
  "short_code": "abc123x",
  "total_clicks": 15234,
  "unique_visitors": 8921,
  "last_24h_clicks": 234,
  "top_countries": ["US", "IN", "UK"],
  "click_timeline": [...]
}

DELETE /v1/urls/{short_code}
Authorization: Bearer <token>
Response: 204 No Content
```


### Step 4: Database Design

**Schema (PostgreSQL with partitioning):**

```sql
-- Primary URLs table
CREATE TABLE urls (
    id BIGSERIAL PRIMARY KEY,
    short_code VARCHAR(10) UNIQUE NOT NULL,
    long_url TEXT NOT NULL,
    user_id BIGINT,
    created_at TIMESTAMP DEFAULT NOW(),
    expires_at TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    
    INDEX idx_short_code (short_code),
    INDEX idx_user_created (user_id, created_at DESC),
    INDEX idx_expires (expires_at) WHERE expires_at IS NOT NULL
);

-- Analytics table (high write volume, partitioned by time)
CREATE TABLE url_analytics (
    id BIGSERIAL,
    short_code VARCHAR(10) NOT NULL,
    clicked_at TIMESTAMP DEFAULT NOW(),
    ip_address INET,
    country_code CHAR(2),
    user_agent TEXT,
    referer TEXT,
    
    PRIMARY KEY (id, clicked_at)
) PARTITION BY RANGE (clicked_at);

-- Create monthly partitions
CREATE TABLE url_analytics_2025_10 
    PARTITION OF url_analytics 
    FOR VALUES FROM ('2025-10-01') TO ('2025-11-01');

-- Materialized view for analytics (refresh periodically)
CREATE MATERIALIZED VIEW url_analytics_summary AS
SELECT 
    short_code,
    COUNT(*) as total_clicks,
    COUNT(DISTINCT ip_address) as unique_visitors,
    COUNT(*) FILTER (WHERE clicked_at > NOW() - INTERVAL '24 hours') as clicks_24h
FROM url_analytics
GROUP BY short_code;

CREATE UNIQUE INDEX ON url_analytics_summary (short_code);
```

**Sharding Strategy:**

```
Shard by short_code using consistent hashing
Shard key: hash(short_code) % num_shards

Benefits:
- Even distribution
- Easy to add shards
- No hot spots

Alternative: Range-based sharding by ID
- Shard 1: IDs 1 - 1B
- Shard 2: IDs 1B - 2B
- Easier range queries but potential hot spots
```


### Step 5: High-Level Design

```
┌──────────────────────────────────────────────────────────┐
│                    CDN (Static Assets)                   │
└──────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────▼────────────────────────────┐
│              Global Load Balancer (DNS)                  │
│         Route to nearest region (latency-based)          │
└─────────────────────────────┬────────────────────────────┘
                              │
        ┌─────────────────────┴───────────────────┐
        │                                         │
┌───────▼────────┐                      ┌─────────▼────────┐
│   Region US    │                      │   Region EU      │
│                │                      │                  │
│  ┌──────────┐  │                      │  ┌──────────┐    │
│  │   ALB    │  │                      │  │   ALB    │    │
│  └────┬─────┘  │                      │  └────┬─────┘    │
│       │        │                      │       │          │
│  ┌────▼─────┐  │                      │  ┌────▼─────┐    │
│  │ API GW + │  │                      │  │ API GW + │    │
│  │Rate Limit│  │                      │  │Rate Limit│    │
│  └────┬─────┘  │                      │  └────┬─────┘    │
│       │        │                      │       │          │
│  ┌────▼──────────────┐                │  ┌────▼──────┐   │
│  │  App Servers      │                │  │ App Svrs  │   │
│  │  (Auto-scaling)   │                │  └────┬──────┘   │
│  └────┬──────┬───────┘                │       │          │
│       │      │                        └───────┼──────────┘
│       │      │                                │
│  ┌────▼──────▼────────┐              ┌───────▼──────────┐
│  │  Redis Cluster     │              │ Redis Cluster    │
│  │  (Read Cache)      │◄─────────────►   (Replication)  │
│  └────┬───────────────┘              └──────────────────┘
│       │                                        
│  ┌────▼────────────────────────────────────────────────┐
│  │           PostgreSQL (Primary)                      │
│  │              Sharded by short_code                  │
│  ├──────────┬──────────┬──────────┬──────────┐         │
│  │ Shard 1  │ Shard 2  │ Shard 3  │ Shard 4  │         │
│  └────┬─────┴────┬─────┴────┬─────┴────┬─────┘         │
│       │          │          │          │                │
│  ┌────▼─────┬────▼─────┬────▼─────┬────▼─────┐         │
│  │Replica 1 │Replica 2 │Replica 3 │Replica 4 │         │
│  └──────────┴──────────┴──────────┴──────────┘         │
│                                                          │
│  ┌───────────────────────────────────────────────────┐  │
│  │    Kafka (Analytics Events Stream)                │  │
│  └────────────────┬──────────────────────────────────┘  │
│                   │                                      │
│  ┌────────────────▼──────────────────────────────────┐  │
│  │    Spark/Flink (Stream Processing)                │  │
│  │    Aggregate analytics, detect anomalies          │  │
│  └────────────────┬──────────────────────────────────┘  │
│                   │                                      │
│  ┌────────────────▼──────────────────────────────────┐  │
│  │    ClickHouse / Cassandra (Analytics Storage)     │  │
│  │    Time-series optimized for analytics queries    │  │
│  └───────────────────────────────────────────────────┘  │
│                                                          │
│  ┌───────────────────────────────────────────────────┐  │
│  │    Zookeeper / etcd (Distributed Coordination)    │  │
│  │    ID generation, leader election, config         │  │
│  └───────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────┘
```


### Step 6: Deep Dive - ID Generation Strategy

**Option 1: Base62 Encoding of Auto-increment ID**[^1][^2][^3]

<details>
<summary>Base62Encoder Class</summary>

```cpp
class Base62Encoder {
private:
    static constexpr char CHARSET[] = 
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
public:
    static std::string encode(uint64_t num) {
        if (num == 0) return "0";
        
        std::string result;
        while (num > 0) {
            result = CHARSET[num % 62] + result;
            num /= 62;
        }
        return result;
    }
    
    static uint64_t decode(const std::string& encoded) {
        uint64_t result = 0;
        for (char c : encoded) {
            result = result * 62 + charToValue(c);
        }
        return result;
    }
    
private:
    static int charToValue(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'z') return c - 'a' + 10;
        if (c >= 'A' && c <= 'Z') return c - 'A' + 36;
        return 0;
    }
};

// Usage
uint64_t id = 123456789;
std::string short_code = Base62Encoder::encode(id);  // "8M0kX"
```

</details>

**Pros:** Simple, guaranteed unique, predictable length
**Cons:** Sequential (security concern), single point of failure for ID generation

**Option 2: Distributed ID Generation (Twitter Snowflake)**

```
64-bit ID structure:
┌────────────────────────────────────────────────────────────┐
│ 1 bit  │  41 bits     │ 10 bits    │   12 bits             │
│unused  │  timestamp   │ machine ID │   sequence number     │
└────────────────────────────────────────────────────────────┘

- Timestamp: milliseconds since epoch (69 years)
- Machine ID: 1024 unique machines
- Sequence: 4096 IDs per millisecond per machine
```

<details>
<summary>SnowflakeIDGenerator Class</summary>

```cpp
class SnowflakeIDGenerator {
private:
    const uint64_t epoch = 1609459200000;  // 2021-01-01
    const uint64_t machine_id;
    uint64_t sequence = 0;
    uint64_t last_timestamp = 0;
    
    std::mutex mtx;
    
    uint64_t currentTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
public:
    explicit SnowflakeIDGenerator(uint64_t machine) 
        : machine_id(machine & 0x3FF) {}  // 10 bits
    
    uint64_t generateID() {
        std::lock_guard<std::mutex> lock(mtx);
        
        uint64_t timestamp = currentTimestamp();
        
        if (timestamp < last_timestamp) {
            throw std::runtime_error("Clock moved backwards");
        }
        
        if (timestamp == last_timestamp) {
            sequence = (sequence + 1) & 0xFFF;  // 12 bits
            if (sequence == 0) {
                // Sequence overflow, wait for next millisecond
                while (timestamp <= last_timestamp) {
                    timestamp = currentTimestamp();
                }
            }
        } else {
            sequence = 0;
        }
        
        last_timestamp = timestamp;
        
        // Combine components
        return ((timestamp - epoch) << 22) | 
               (machine_id << 12) | 
               sequence;
    }
};
```

</details>

**Recommended: Range Allocation + Base62**

<details>
<summary>RangeBasedIDGenerator Class</summary>

```cpp
// Central ID allocator assigns ranges to each service
class RangeBasedIDGenerator {
private:
    uint64_t current_id;
    uint64_t max_id;
    std::mutex mtx;
    
    // Function to request new range from coordinator
    std::pair<uint64_t, uint64_t> requestRange() {
        // Call distributed coordinator (etcd/ZooKeeper)
        // Atomically increment and return range [start, end]
        return {1000000, 1100000};  // 100K IDs
    }
    
public:
    RangeBasedIDGenerator() {
        auto [start, end] = requestRange();
        current_id = start;
        max_id = end;
    }
    
    uint64_t generateID() {
        std::lock_guard<std::mutex> lock(mtx);
        
        if (current_id >= max_id) {
            auto [start, end] = requestRange();
            current_id = start;
            max_id = end;
        }
        
        return current_id++;
    }
    
    std::string generateShortCode() {
        uint64_t id = generateID();
        return Base62Encoder::encode(id);
    }
};
```

</details>

**Collision Handling for Custom Aliases:**

<details>
<summary>C++ Code</summary>

```cpp
std::string createShortURL(const std::string& long_url, 
                          const std::string& custom_alias = "") {
    std::string short_code;
    
    if (!custom_alias.empty()) {
        // Check if custom alias available
        if (isShortCodeAvailable(custom_alias)) {
            short_code = custom_alias;
        } else {
            throw std::runtime_error("Custom alias already taken");
        }
    } else {
        // Generate unique short code
        int attempts = 0;
        do {
            uint64_t id = id_generator.generateID();
            short_code = Base62Encoder::encode(id);
            ++attempts;
        } while (!isShortCodeAvailable(short_code) && attempts < 3);
        
        if (attempts >= 3) {
            throw std::runtime_error("Failed to generate unique short code");
        }
    }
    
    // Store mapping
    storeMapping(short_code, long_url);
    return "https://tinyurl.com/" + short_code;
}
```

</details>


### Step 7: Bottlenecks \& Optimizations

**Caching Strategy:**

```
Cache-Aside Pattern with Redis:
1. Read path:
   - Check Redis cache
   - On miss: Query DB → Write to cache → Return
   - On hit: Return from cache

2. Write path:
   - Write to DB
   - Invalidate cache (or write-through)

3. Cache eviction: LRU with TTL
   - TTL: 24 hours for active URLs
   - Max memory: 100GB per cluster
```

**Read Optimization:**

```
CDN for popular URLs:
- Cache 302 redirects at edge locations
- Reduces latency to <50ms globally
- Set cache-control headers properly

Database read replicas:
- Master: Writes
- Replicas: Reads (analytics, lookups)
- Lag monitoring: Alert if >5 seconds
```

**Analytics Optimization:**

```
Async event processing:
1. Redirect request arrives
2. Respond immediately with 302
3. Async: Publish click event to Kafka
4. Stream processor: Aggregate in batches
5. Write to analytics DB (ClickHouse)

Benefits:
- No latency impact on redirects
- Handle spike traffic
- Batch writes reduce DB load
```

**Rate Limiting:**

```
Token bucket per user/IP:
- 1000 requests per hour per user
- 10 URLs created per hour per user

Implementation:
Redis INCR with expiry:
SETEX rate_limit:user_123:create 3600 10
DECR rate_limit:user_123:create
```


***

Let me continue with the next major design problem. Would you like me to proceed with:

1. **News Feed System (Facebook/Twitter)**
2. **Stock Price Monitoring with Notifications**
3. **Podcast Service (Feedly-like)**

Which one would you like me to solve next in detail?
<span style="display:none">[^10][^11][^12][^13][^14][^15][^16][^17][^18][^19][^20][^4][^5][^6][^7][^8][^9]</span>

<div align="center">⁂</div>

[^1]: https://blog.algomaster.io/p/design-a-url-shortener

[^2]: https://www.geeksforgeeks.org/system-design/system-design-url-shortening-service/

[^3]: https://www.linkedin.com/pulse/building-url-shortener-using-hash-functions-base62-conversion-singh-y01oc

[^4]: https://bytebytego.com/courses/system-design-interview/design-a-url-shortener

[^5]: https://www.hellointerview.com/learn/system-design/problem-breakdowns/bitly

[^6]: https://www.designgurus.io/blog/design-social-media-news-feed

[^7]: https://www.marketcalls.in/python/websocket-tutorial-for-traders-and-python-developers.html

[^8]: https://systemdesignschool.io/problems/url-shortener/solution

[^9]: https://www.geeksforgeeks.org/blogs/facebook-news-feed-algorithm/

[^10]: https://polygon.io/docs/websocket/stocks/overview

[^11]: https://systemdesign.one/url-shortening-system-design/

[^12]: https://engineering.fb.com/2021/01/26/core-infra/news-feed-ranking/

[^13]: https://finnhub.io/docs/api/websocket-trades

[^14]: https://dev.to/zeeshanali0704/design-a-url-shortner-tiny-url-4cb5

[^15]: https://javatechonline.com/feed-ranking-algorithms-in-system-design/

[^16]: https://blog.algomaster.io/p/websocket-use-cases-system-design

[^17]: https://javatechonline.com/url-shortening-system-design-tiny-url/

[^18]: https://bytebytego.com/courses/machine-learning-system-design-interview/personalized-news-feed

[^19]: https://aws.amazon.com/blogs/mobile/building-a-real-time-stock-monitoring-dashboard-with-aws-appsync/

[^20]: https://blog.algomaster.io/p/designing-a-scalable-news-feed-system

