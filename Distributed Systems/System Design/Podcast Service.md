# 3. Podcast Service (Spotify/Apple Podcasts/Feedly)

## Step 1: Requirements Clarification

**Functional Requirements:**

- Users can browse/search podcasts and episodes
- Users can subscribe/unsubscribe to podcast channels
- Users can play audio episodes (streaming)
- Fetch top N episodes from subscribed channels (personalized feed)
- Download episodes for offline listening
- Track listening progress (resume playback)
- Trending/recommended podcasts
- Create and share playlists

**Non-Functional Requirements:**

- High availability (99.9%)
- Low latency for audio streaming (<200ms initial load)
- Scale: 10M users, 100K podcasts, 10M episodes
- Read-heavy: 100:1 read-to-write ratio
- Global distribution (CDN for audio files)
- Support 1M concurrent streams
- Eventual consistency acceptable

**Out of Scope:**

- Podcast creation/hosting
- Live streaming
- Video podcasts
- Comments/community features
- Monetization/ads insertion


## Step 2: Capacity Estimation

```
User Statistics:
Total users: 10M
Daily Active Users (DAU): 3M
Average subscriptions per user: 20 podcasts
Average listening time: 45 min/day

Podcast Statistics:
Total podcasts: 100K
Total episodes: 10M (avg 100 episodes/podcast)
New episodes per day: 50K
Average episode size: 50 MB (1 hour at 128 kbps)
Average episode metadata: 5 KB

Traffic Estimation:
Episode plays per day: 3M users × 2 episodes = 6M plays
Streaming QPS: 6M / 86,400 ≈ 70 QPS
Peak streaming QPS: 70 × 5 = 350 QPS (commute hours)

Browse/Search QPS: 3M × 10 page views = 30M
Browse QPS: 30M / 86,400 ≈ 350 QPS

Subscription changes per day: 3M × 0.5 = 1.5M
Write QPS: 1.5M / 86,400 ≈ 18 QPS

Storage Estimation:
Audio storage: 10M episodes × 50 MB = 500 TB
Metadata: 10M × 5 KB = 50 GB
User data: 10M users × 10 KB = 100 GB
Total: ~500 TB (primarily audio)

With replication (3x): 1.5 PB

Bandwidth:
Audio streaming: 70 QPS × 128 kbps = 8.96 Mbps avg
Peak: 350 QPS × 128 kbps = 44.8 Mbps
CDN bandwidth: 10x higher = 448 Mbps

Feed Generation:
User fetches feed: Get 20 podcasts × 5 latest = 100 episodes
Query time: Need to fetch 100 episodes and sort
Cache hit ratio: 80% (20% cold cache)

RSS Polling:
Podcasts to poll: 100K
Poll frequency: Every 1 hour
Poll QPS: 100K / 3,600 ≈ 28 QPS
```


## Step 3: API Design

**GET /v1/podcasts/search**

```json
GET /v1/podcasts/search?q=system+design&limit=20&offset=0

Response: 200 OK
{
  "podcasts": [
    {
      "podcast_id": "podcast_123",
      "title": "System Design Podcast",
      "author": "Tech Talks",
      "description": "Deep dives into system architecture",
      "cover_image_url": "https://cdn.example.com/covers/podcast_123.jpg",
      "rss_feed_url": "https://feeds.example.com/system-design.xml",
      "category": ["Technology", "Education"],
      "subscriber_count": 125000,
      "episode_count": 87,
      "last_updated": "2025-10-02T15:00:00Z"
    }
  ],
  "total": 45,
  "has_more": true
}
```

**POST /v1/users/{user_id}/subscriptions**

```json
POST /v1/users/user_789/subscriptions
Authorization: Bearer <token>
Content-Type: application/json

Request:
{
  "podcast_id": "podcast_123"
}

Response: 201 Created
{
  "subscription_id": "sub_456",
  "user_id": "user_789",
  "podcast_id": "podcast_123",
  "subscribed_at": "2025-10-03T06:05:00Z"
}
```

**GET /v1/users/{user_id}/feed**

```json
GET /v1/users/user_789/feed?limit=20&cursor=xyz123
Authorization: Bearer <token>

Response: 200 OK
{
  "episodes": [
    {
      "episode_id": "ep_456",
      "podcast_id": "podcast_123",
      "podcast_title": "System Design Podcast",
      "podcast_cover": "https://cdn.example.com/covers/podcast_123.jpg",
      "title": "Designing a News Feed System",
      "description": "How Facebook and Twitter build scalable feeds",
      "duration": 3600,
      "published_at": "2025-10-02T12:00:00Z",
      "audio_url": "https://cdn.example.com/episodes/ep_456.mp3",
      "file_size": 52428800,
      "is_played": false,
      "progress": 0
    }
  ],
  "next_cursor": "abc789",
  "generated_at": "2025-10-03T06:05:00Z"
}
```

**GET /v1/episodes/{episode_id}/stream**

```json
GET /v1/episodes/ep_456/stream
Authorization: Bearer <token>
Range: bytes=0-

Response: 206 Partial Content
Content-Type: audio/mpeg
Content-Length: 52428800
Content-Range: bytes 0-52428799/52428800
Accept-Ranges: bytes
Cache-Control: public, max-age=3600

<binary audio data>
```

**POST /v1/users/{user_id}/progress**

```json
POST /v1/users/user_789/progress
Authorization: Bearer <token>

Request:
{
  "episode_id": "ep_456",
  "position": 1234,  // seconds
  "duration": 3600
}

Response: 200 OK
{
  "episode_id": "ep_456",
  "position": 1234,
  "percentage": 34.3,
  "updated_at": "2025-10-03T06:10:00Z"
}
```

**GET /v1/podcasts/{podcast_id}/episodes**

```json
GET /v1/podcasts/podcast_123/episodes?limit=50&sort=desc

Response: 200 OK
{
  "podcast_id": "podcast_123",
  "episodes": [
    {
      "episode_id": "ep_456",
      "title": "Designing a News Feed System",
      "description": "...",
      "duration": 3600,
      "published_at": "2025-10-02T12:00:00Z",
      "audio_url": "https://cdn.example.com/episodes/ep_456.mp3",
      "file_size": 52428800
    }
  ],
  "total": 87
}
```


## Step 4: Database Design

**SQL Schema (PostgreSQL):**

```sql
-- Podcasts table
CREATE TABLE podcasts (
    podcast_id BIGSERIAL PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    author VARCHAR(255),
    description TEXT,
    cover_image_url TEXT,
    rss_feed_url TEXT UNIQUE NOT NULL,
    website_url TEXT,
    category TEXT[],
    language VARCHAR(10),
    subscriber_count INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT NOW(),
    last_updated TIMESTAMP DEFAULT NOW(),
    last_fetched TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    
    INDEX idx_title (title),
    INDEX idx_category USING GIN(category),
    INDEX idx_last_fetched (last_fetched)
);

-- Episodes table (partitioned by published_at)
CREATE TABLE episodes (
    episode_id BIGSERIAL,
    podcast_id BIGINT NOT NULL,
    title VARCHAR(500) NOT NULL,
    description TEXT,
    audio_url TEXT NOT NULL,
    file_size BIGINT,
    duration INT,  -- seconds
    published_at TIMESTAMP NOT NULL,
    guid VARCHAR(255) UNIQUE,  -- RSS GUID for deduplication
    play_count BIGINT DEFAULT 0,
    
    PRIMARY KEY (episode_id, published_at),
    FOREIGN KEY (podcast_id) REFERENCES podcasts(podcast_id)
) PARTITION BY RANGE (published_at);

-- Monthly partitions
CREATE TABLE episodes_2025_10 PARTITION OF episodes
    FOR VALUES FROM ('2025-10-01') TO ('2025-11-01');

-- Indexes
CREATE INDEX idx_episodes_podcast_time 
    ON episodes(podcast_id, published_at DESC);
CREATE INDEX idx_episodes_guid ON episodes(guid);

-- User subscriptions
CREATE TABLE user_subscriptions (
    subscription_id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL,
    podcast_id BIGINT NOT NULL,
    subscribed_at TIMESTAMP DEFAULT NOW(),
    notification_enabled BOOLEAN DEFAULT TRUE,
    
    UNIQUE(user_id, podcast_id),
    INDEX idx_user_subscriptions (user_id, subscribed_at DESC),
    INDEX idx_podcast_subscribers (podcast_id)
);

-- User listening progress
CREATE TABLE user_progress (
    user_id BIGINT NOT NULL,
    episode_id BIGINT NOT NULL,
    position INT,  -- seconds
    duration INT,
    updated_at TIMESTAMP DEFAULT NOW(),
    completed BOOLEAN DEFAULT FALSE,
    
    PRIMARY KEY (user_id, episode_id),
    INDEX idx_updated (updated_at)
);

-- Trending/Popular (materialized view)
CREATE MATERIALIZED VIEW trending_podcasts AS
SELECT 
    p.podcast_id,
    p.title,
    p.cover_image_url,
    COUNT(DISTINCT us.user_id) as subscriber_count,
    COUNT(DISTINCT up.user_id) as listener_count,
    SUM(e.play_count) as total_plays
FROM podcasts p
LEFT JOIN user_subscriptions us ON p.podcast_id = us.podcast_id
LEFT JOIN episodes e ON p.podcast_id = e.podcast_id
LEFT JOIN user_progress up ON e.episode_id = up.episode_id
WHERE p.last_updated > NOW() - INTERVAL '30 days'
GROUP BY p.podcast_id
ORDER BY listener_count DESC, subscriber_count DESC
LIMIT 100;

REFRESH MATERIALIZED VIEW trending_podcasts;
```

**NoSQL Schema (Cassandra for User Feed):**

```sql
-- Pre-computed user feed (fan-out on write)
CREATE TABLE user_feed (
    user_id BIGINT,
    published_at TIMESTAMP,
    episode_id BIGINT,
    podcast_id BIGINT,
    podcast_title TEXT,
    episode_title TEXT,
    audio_url TEXT,
    duration INT,
    
    PRIMARY KEY (user_id, published_at, episode_id)
) WITH CLUSTERING ORDER BY (published_at DESC);

-- Query: Get latest episodes for user
-- SELECT * FROM user_feed WHERE user_id = ? LIMIT 20;
```

**Object Storage (S3) for Audio Files:**

```
Bucket structure:
s3://podcast-audio/
  ├── {podcast_id}/
  │   ├── {episode_id}.mp3
  │   ├── {episode_id}_128k.mp3  (transcoded)
  │   └── {episode_id}_64k.mp3   (low quality)
  └── covers/
      └── {podcast_id}.jpg
```


## Step 5: High-Level Design

```
┌────────────────────────────────────────────────────────────┐
│                   Global CDN (CloudFront/Akamai)           │
│              Cache audio files at edge locations           │
└────────────────────────────┬───────────────────────────────┘
                             │
┌────────────────────────────▼───────────────────────────────┐
│                   DNS Load Balancer                        │
│               Route to nearest region                      │
└────────────────────────────┬───────────────────────────────┘
                             │
        ┌────────────────────┴────────────────────┐
        │                                         │
┌───────▼──────────┐                   ┌──────────▼─────────┐
│  API Gateway     │                   │  WebSocket Server  │
│  Rate Limiting   │                   │  (Real-time sync)  │
└───────┬──────────┘                   └────────────────────┘
        │
┌───────▼────────────────────────────────────────────────────┐
│                 Application Services Layer                 │
│                                                            │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │
│  │ Search   │  │ Feed     │  │ Stream   │  │ User     │  │
│  │ Service  │  │ Service  │  │ Service  │  │ Service  │  │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘  │
└───────┼─────────────┼─────────────┼─────────────┼────────┘
        │             │             │             │
        │             │             │             │
┌───────▼─────────────▼─────────────▼─────────────▼────────┐
│                   Redis Cluster                           │
│  - Podcast metadata cache                                 │
│  - Episode metadata cache                                 │
│  - User feed cache (sorted sets)                          │
│  - Session/progress tracking                              │
└───────────────────────────────┬───────────────────────────┘
                                │
┌───────────────────────────────▼───────────────────────────┐
│                    Message Queue (Kafka)                  │
│  Topics:                                                  │
│  - rss_updates (new episodes detected)                    │
│  - feed_generation (fan-out to subscribers)               │
│  - analytics_events (play/pause/complete)                 │
└────────────┬──────────────────────────────┬───────────────┘
             │                              │
    ┌────────▼─────────┐         ┌─────────▼──────────┐
    │  RSS Fetcher     │         │  Feed Generator    │
    │  Workers         │         │  Workers           │
    │  (Poll feeds)    │         │  (Fan-out)         │
    └────────┬─────────┘         └──────────┬─────────┘
             │                              │
┌────────────▼──────────────────────────────▼───────────────┐
│              Primary Database Layer                       │
│                                                           │
│  ┌──────────────┐    ┌──────────────┐   ┌─────────────┐ │
│  │ PostgreSQL   │    │ Cassandra    │   │ Elasticsearch│ │
│  │ (Metadata)   │    │ (User Feed)  │   │ (Search)     │ │
│  │ Sharded      │    │ Distributed  │   │ Inverted idx │ │
│  └──────┬───────┘    └──────┬───────┘   └──────┬──────┘ │
└─────────┼────────────────────┼──────────────────┼────────┘
          │                    │                  │
    ┌─────▼────────┐     ┌─────▼────────┐       │
    │  Read        │     │  Read        │       │
    │  Replicas    │     │  Replicas    │       │
    └──────────────┘     └──────────────┘       │
                                                 │
┌────────────────────────────────────────────────▼──────────┐
│              Object Storage (S3/GCS)                      │
│  - Audio files (.mp3, .m4a)                               │
│  - Podcast covers                                         │
│  - Transcoded versions (different bitrates)               │
└───────────────────────────────────────────────────────────┘
                                │
┌───────────────────────────────▼───────────────────────────┐
│          Background Processing Services                   │
│                                                           │
│  ┌──────────────┐    ┌──────────────┐   ┌─────────────┐ │
│  │ Transcoding  │    │ Analytics    │   │ ML Recommend │ │
│  │ Service      │    │ Aggregation  │   │ Service      │ │
│  └──────────────┘    └──────────────┘   └──────────────┘ │
└───────────────────────────────────────────────────────────┘
```


## Step 6: Deep Dive - RSS Feed Polling \& Feed Generation

### RSS Feed Polling Strategy[^1][^2]

<details>
<summary>RSSFeedPoller Class</summary>

```cpp
class RSSFeedPoller {
private:
    struct PodcastFeed {
        int64_t podcast_id;
        std::string rss_url;
        time_t last_fetched;
        int poll_frequency;  // seconds
    };
    
public:
    // Distributed polling using consistent hashing
    void pollFeeds() {
        // Get podcasts assigned to this worker
        vector<PodcastFeed> assigned_feeds = getAssignedFeeds();
        
        for (const auto& feed : assigned_feeds) {
            if (shouldPoll(feed)) {
                pollSingleFeed(feed);
            }
        }
    }
    
private:
    bool shouldPoll(const PodcastFeed& feed) {
        time_t now = time(nullptr);
        return (now - feed.last_fetched) >= feed.poll_frequency;
    }
    
    void pollSingleFeed(const PodcastFeed& feed) {
        try {
            // 1. Fetch RSS feed
            std::string xml_content = httpClient.get(feed.rss_url);
            
            // 2. Parse XML
            RSSParser parser;
            vector<Episode> episodes = parser.parse(xml_content);
            
            // 3. Filter new episodes (using GUID)
            vector<Episode> new_episodes = 
                filterNewEpisodes(feed.podcast_id, episodes);
            
            if (!new_episodes.empty()) {
                // 4. Store episodes in database
                for (const auto& episode : new_episodes) {
                    storeEpisode(episode);
                    
                    // 5. Trigger feed generation for subscribers
                    kafka.publish("rss_updates", {
                        "podcast_id": feed.podcast_id,
                        "episode_id": episode.episode_id,
                        "published_at": episode.published_at
                    });
                }
            }
            
            // 6. Update last_fetched timestamp
            updatePodcastFetchTime(feed.podcast_id, time(nullptr));
            
        } catch (const std::exception& e) {
            logError("Failed to poll feed", feed.podcast_id, e.what());
            // Exponential backoff on failure
            increasePollInterval(feed.podcast_id);
        }
    }
    
    vector<Episode> filterNewEpisodes(
        int64_t podcast_id, 
        const vector<Episode>& episodes
    ) {
        // Get existing GUIDs from database
        unordered_set<string> existing_guids = 
            getExistingGUIDs(podcast_id);
        
        vector<Episode> new_episodes;
        for (const auto& episode : episodes) {
            if (existing_guids.find(episode.guid) == existing_guids.end()) {
                new_episodes.push_back(episode);
            }
        }
        
        return new_episodes;
    }
};

// RSS XML Parser
class RSSParser {
public:
    vector<Episode> parse(const std::string& xml) {
        vector<Episode> episodes;
        
        // Parse XML (use library like libxml2 or pugixml)
        pugi::xml_document doc;
        doc.load_string(xml.c_str());
        
        // Navigate to items
        auto channel = doc.child("rss").child("channel");
        
        for (auto item : channel.children("item")) {
            Episode ep;
            ep.title = item.child_value("title");
            ep.description = item.child_value("description");
            ep.guid = item.child_value("guid");
            ep.published_at = parseDate(item.child_value("pubDate"));
            
            // Get enclosure (audio file)
            auto enclosure = item.child("enclosure");
            ep.audio_url = enclosure.attribute("url").value();
            ep.file_size = enclosure.attribute("length").as_llong();
            ep.duration = parseDuration(item.child_value("itunes:duration"));
            
            episodes.push_back(ep);
        }
        
        return episodes;
    }
    
private:
    time_t parseDate(const std::string& date_str) {
        // Parse RFC 2822 format: "Mon, 02 Oct 2025 12:00:00 GMT"
        struct tm tm = {};
        strptime(date_str.c_str(), "%a, %d %b %Y %H:%M:%S %Z", &tm);
        return mktime(&tm);
    }
    
    int parseDuration(const std::string& duration) {
        // Format: "HH:MM:SS" or "MM:SS" or seconds
        int hours = 0, minutes = 0, seconds = 0;
        
        if (sscanf(duration.c_str(), "%d:%d:%d", &hours, &minutes, &seconds) == 3) {
            return hours * 3600 + minutes * 60 + seconds;
        } else if (sscanf(duration.c_str(), "%d:%d", &minutes, &seconds) == 2) {
            return minutes * 60 + seconds;
        } else {
            return std::stoi(duration);
        }
    }
};
```

</details>


### Feed Generation (Hybrid Push-Pull)[^3]

<details>
<summary>FeedGenerator Class</summary>

```cpp
class FeedGenerator {
public:
    // When new episode published - fan out to active subscribers
    void onNewEpisode(int64_t podcast_id, int64_t episode_id) {
        // Get subscribers
        vector<int64_t> subscribers = getSubscribers(podcast_id);
        
        // Filter active users (listened in last 7 days)
        vector<int64_t> active_subscribers = 
            filterActiveUsers(subscribers, 7);
        
        // Batch fan-out to active users
        for (int64_t user_id : active_subscribers) {
            // Add to user's feed cache (sorted set by published_at)
            redis.zadd(
                "user_feed:" + to_string(user_id),
                episode.published_at,  // score
                episode_id             // member
            );
            
            // Trim to keep only latest 1000 episodes
            redis.zremrangebyrank(
                "user_feed:" + to_string(user_id),
                0, -1001
            );
        }
        
        // For inactive users, mark podcast as having new content
        redis.sadd("updated_podcasts:" + to_string(podcast_id), "1");
    }
    
    // When user requests feed
    vector<Episode> generateFeed(int64_t user_id, int limit) {
        // 1. Try cache first
        vector<int64_t> cached_episode_ids = 
            redis.zrevrange(
                "user_feed:" + to_string(user_id),
                0, limit - 1
            );
        
        if (!cached_episode_ids.empty()) {
            return batchGetEpisodes(cached_episode_ids);
        }
        
        // 2. Cache miss - generate feed on-demand (pull)
        vector<int64_t> subscribed_podcasts = 
            getUserSubscriptions(user_id);
        
        vector<Episode> episodes;
        for (int64_t podcast_id : subscribed_podcasts) {
            // Get latest 5 episodes per podcast
            vector<Episode> podcast_episodes = 
                getLatestEpisodes(podcast_id, 5);
            
            episodes.insert(
                episodes.end(),
                podcast_episodes.begin(),
                podcast_episodes.end()
            );
        }
        
        // 3. Sort by published_at (descending)
        sort(episodes.begin(), episodes.end(),
             [](const Episode& a, const Episode& b) {
                 return a.published_at > b.published_at;
             });
        
        // 4. Cache for next time
        for (const auto& ep : episodes) {
            redis.zadd(
                "user_feed:" + to_string(user_id),
                ep.published_at,
                ep.episode_id
            );
        }
        
        // 5. Return top N
        episodes.resize(min(episodes.size(), (size_t)limit));
        return episodes;
    }
};
```

</details>


### Audio Streaming with Range Requests

<details>
<summary>AudioStreamingService Class</summary>

```cpp
class AudioStreamingService {
public:
    // Handle HTTP range request for audio streaming
    HttpResponse streamAudio(
        int64_t episode_id,
        const std::string& range_header
    ) {
        // 1. Get episode metadata
        Episode episode = getEpisode(episode_id);
        
        // 2. Parse range header: "bytes=0-1023" or "bytes=1024-"
        auto [start, end] = parseRange(range_header, episode.file_size);
        
        // 3. Generate signed CDN URL (S3 presigned URL)
        std::string cdn_url = generateCDNUrl(
            episode.audio_url,
            3600  // expires in 1 hour
        );
        
        // 4. Return 302 redirect to CDN (or proxy stream)
        HttpResponse response;
        response.status = 302;
        response.headers["Location"] = cdn_url;
        response.headers["Cache-Control"] = "public, max-age=3600";
        
        // Alternative: Proxy stream from S3
        // response.body = s3Client.getObject(
        //     episode.audio_url,
        //     start,
        //     end - start + 1
        // );
        // response.status = 206;  // Partial Content
        // response.headers["Content-Range"] = 
        //     "bytes " + to_string(start) + "-" + 
        //     to_string(end) + "/" + to_string(episode.file_size);
        
        return response;
    }
    
private:
    std::pair<int64_t, int64_t> parseRange(
        const std::string& range_header,
        int64_t total_size
    ) {
        // Range: bytes=start-end
        int64_t start = 0, end = total_size - 1;
        
        if (!range_header.empty()) {
            std::string bytes_part = range_header.substr(6);  // Skip "bytes="
            size_t dash_pos = bytes_part.find('-');
            
            if (dash_pos != std::string::npos) {
                std::string start_str = bytes_part.substr(0, dash_pos);
                std::string end_str = bytes_part.substr(dash_pos + 1);
                
                if (!start_str.empty()) {
                    start = std::stoll(start_str);
                }
                
                if (!end_str.empty()) {
                    end = std::stoll(end_str);
                } else {
                    end = total_size - 1;
                }
            }
        }
        
        return {start, end};
    }
    
    std::string generateCDNUrl(
        const std::string& s3_path,
        int expiry_seconds
    ) {
        // Generate CloudFront signed URL
        time_t expiration = time(nullptr) + expiry_seconds;
        
        // Create policy
        std::string policy = createPolicy(s3_path, expiration);
        
        // Sign with private key
        std::string signature = signPolicy(policy);
        
        // Build URL
        return "https://cdn.example.com/" + s3_path +
               "?Expires=" + std::to_string(expiration) +
               "&Signature=" + signature +
               "&Key-Pair-Id=" + keyPairId;
    }
};
```

</details>


## Step 7: Bottlenecks \& Optimizations

### Performance Optimizations

**1. CDN Strategy:**

```
Multi-tier caching:
- Edge: Popular episodes (80/20 rule)
- Regional: Medium popularity
- Origin: S3 (all episodes)

Cache invalidation:
- 30 day TTL for audio files (immutable)
- 1 hour TTL for covers
- Aggressive prefetching for trending podcasts
```

**2. Feed Generation Optimization:**

```
Hybrid approach:
- Push: Fan-out to active users (<10K subscribers/podcast)
- Pull: On-demand for inactive users or mega-podcasts

Cache warming:
- Pre-compute feeds during off-peak hours
- Refresh top 10K active users every hour
```

**3. Search Optimization:**

```
Elasticsearch setup:
- Shard by podcast category
- Replicas in each region
- Autocomplete using edge n-grams
- Fuzzy matching for typos

Index structure:
{
  "podcast_id": 123,
  "title": "System Design Podcast",
  "description": "...",
  "category": ["Technology"],
  "suggest": {
    "input": ["system", "design", "podcast"],
    "weight": 125000  // subscriber count
  }
}
```

**4. Database Optimizations:**

```sql
-- Partition episodes by month
-- Each partition on separate disk for parallel I/O

-- Materialized view for user feed (refresh hourly)
CREATE MATERIALIZED VIEW user_feed_cache AS
SELECT 
    us.user_id,
    e.episode_id,
    e.published_at,
    e.title,
    p.title as podcast_title
FROM user_subscriptions us
JOIN podcasts p ON us.podcast_id = p.podcast_id
JOIN episodes e ON p.podcast_id = e.podcast_id
WHERE e.published_at > NOW() - INTERVAL '30 days'
ORDER BY us.user_id, e.published_at DESC;

-- Index for fast lookups
CREATE INDEX idx_user_feed ON user_feed_cache(user_id, published_at DESC);
```

**5. Audio Transcoding:**

```
Multiple bitrates:
- 320 kbps: High quality (75 MB/hour)
- 128 kbps: Standard (15 MB/hour)
- 64 kbps: Low bandwidth (7.5 MB/hour)

Adaptive streaming:
- Detect user's bandwidth
- Switch quality dynamically
- Buffer 30 seconds ahead
```


### Monitoring \& SLAs

```
Key Metrics:
- Feed generation latency (P95 < 500ms)
- Audio stream start time (P95 < 200ms)
- CDN cache hit rate (target > 90%)
- RSS poll success rate (> 99%)
- Database query latency (P99 < 100ms)

Alerts:
- Feed generation failures > 1%
- CDN origin requests spike (cache miss)
- RSS fetcher lag > 2 hours
- Database replica lag > 5 seconds
- Audio stream errors > 0.1%
```


### Trade-offs

| Aspect | Decision | Trade-off |
| :-- | :-- | :-- |
| **Feed Model** | Hybrid push-pull | Balance freshness vs cost |
| **Caching** | Aggressive CDN | Storage cost vs latency |
| **RSS Polling** | Hourly for most | Freshness vs API costs |
| **Audio Storage** | Multi-bitrate | Storage 3x vs UX |
| **Search** | Elasticsearch | Cost vs speed |

This design supports 10M users with 100K podcasts, delivering <200ms audio start time through CDN caching, hybrid feed generation, and distributed RSS polling.[^4][^5][^6][^3]
<span style="display:none">[^10][^11][^12][^13][^14][^15][^16][^17][^18][^19][^20][^21][^22][^23][^24][^25][^26][^27][^28][^29][^30][^31][^32][^33][^34][^35][^36][^37][^7][^8][^9]</span>

<div align="center">⁂</div>

[^1]: https://moldstud.com/articles/p-key-strategies-for-developing-a-highly-functional-rss-feed-parser

[^2]: https://nooptoday.com/building-an-rss-feed-aggregator-with-go-and-chatgpt

[^3]: https://developer.spotify.com/blog/2020-03-20-introducing-podcasts-api

[^4]: https://www.fastpix.io/blog/system-design-and-site-architecture-for-an-audio-streaming-app-like-spotify

[^5]: https://taddy.org/developers/podcast-api

[^6]: https://www.cachefly.com/news/perfect-harmony-using-cdns-for-seamless-high-resolution-audio-streaming/

[^7]: https://thectoclub.com/career/system-design-podcast/

[^8]: https://open.spotify.com/show/2wnpdaxTbMaKQVVmUxb9QW

[^9]: https://www.reddit.com/r/computerscience/comments/yadw43/are_there_existing_podcasts_or_videos_that_talk/

[^10]: https://music.amazon.in/podcasts/691f34e8-7e5a-42eb-bee7-15c02d52c677/system-design

[^11]: https://podcast.feedspot.com/system_design_podcasts/

[^12]: https://www.linkedin.com/posts/rmn-52012_systemdesign-newsaggregator-realtimedata-activity-7269372264498761729-hfQt

[^13]: https://softvelum.com/2025/08/nimble-multicdn-streaming/

[^14]: https://www.redhat.com/en/blog/podcasts-architects

[^15]: https://nextbigtechnology.com/how-to-build-a-news-aggregator-app-like-flipboard-key-features/

[^16]: https://friends.cs.purdue.edu/pubs/MMCN03-enhanced.pdf

[^17]: https://www.infoq.com/architecture-design/podcasts/

[^18]: https://www.hellointerview.com/learn/system-design/problem-breakdowns/fb-news-feed

[^19]: https://www.cachefly.com/news/leveraging-cdns-for-targeted-ad-insertion-in-podcast-streams/

[^20]: https://www.knapsack.cloud/blog-categories/podcast

[^21]: https://bytebytego.com/courses/system-design-interview/design-a-news-feed-system

[^22]: https://enginebogie.com/public/question/news-aggregator-system-design-a-high-scale-event-news-feed-system/442

[^23]: https://www.zype.com/blog/what-is-multi-cdn-multi-cdn-architecture-for-video-streaming

[^24]: https://interviewing.io/mocks/faang-system-design-rss-news-feed

[^25]: https://www.cachefly.com/news/podcast-distribution-with-cdns-shaping-the-trends-of-2024/

[^26]: https://codemia.io/system-design/design-a-podcast-hosting-platform/submissions/spptrq

[^27]: https://ai.google.dev/competition/projects/reers-ai-podcast-platform

[^28]: https://autocontentapi.com

[^29]: https://podcasters.apple.com/support/3956-publish-subscriptions-with-hosting-provider

[^30]: https://podcastpage.io

[^31]: https://www.youtube.com/watch?v=hVMGtfaiM9Q

[^32]: https://github.com/PodcastAPI

[^33]: https://www.reddit.com/r/googlecloud/comments/ghsmzz/any_suggestions_for_an_audio_streaming_backend/

[^34]: https://tyk.io/all-about-apis-podcast/

[^35]: https://www.tothenew.com/blog/rss-feed-parsing-using-pyspark/

[^36]: https://stackoverflow.com/questions/63315674/basic-architecture-to-serve-stream-and-consume-large-audio-files-to-minimize-cl

[^37]: https://www.smashingmagazine.com/2024/10/build-static-rss-reader-fight-fomo/

