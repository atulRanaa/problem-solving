# 2. News Feed System (Facebook/Twitter)

## Step 1: Requirements Clarification

**Functional Requirements:**

- User can create posts (text, images, videos)
- User can follow/unfollow other users
- User can view personalized news feed
- User can like, comment, share posts
- Real-time updates for new posts
- Feed pagination and infinite scroll

**Non-Functional Requirements:**

- High availability (99.99%)
- Low latency for feed generation (<500ms)
- Eventual consistency acceptable
- Scale: 1B users, 500M DAU
- Read-heavy: 1000:1 read-to-write ratio
- Personalized ranking based on engagement

**Out of Scope:**

- Direct messaging
- Stories/ephemeral content
- Video streaming infrastructure
- Content moderation


## Step 2: Capacity Estimation

```
User Statistics:
Total users: 1 Billion
Daily Active Users (DAU): 500M
Average follows per user: 200
Average posts per user per day: 2

Traffic Estimation:
Posts created per day: 500M × 2 = 1B posts
Write QPS: 1B / 86,400 ≈ 11,600 QPS
Peak write QPS: 11,600 × 3 ≈ 35,000 QPS

Feed reads per user per day: 10 views
Feed reads per day: 500M × 10 = 5B
Read QPS: 5B / 86,400 ≈ 58,000 QPS
Peak read QPS: 58,000 × 3 ≈ 174,000 QPS

Storage Estimation (5 years):
Posts per year: 1B × 365 = 365B posts
Total posts (5 years): 365B × 5 = 1.825 Trillion posts

Storage per post:
  - Post ID: 8 bytes
  - User ID: 8 bytes
  - Content: 1 KB (avg text + metadata)
  - Media URL: 100 bytes
  - Timestamps: 16 bytes
  - Engagement counters: 20 bytes
  Total: ~1.2 KB per post

Total storage: 1.825T × 1.2 KB = 2,190 TB = 2.2 PB
With replication (3x): 6.6 PB

Fan-out Estimation (Push model):
Average followers per user: 200
Fan-out writes per post: 200 writes
Fan-out QPS: 11,600 × 200 = 2.32M writes/sec

Feed Cache Size (per user):
Feed items per user: 1000 posts
Cache size per user: 1000 × 1.2 KB = 1.2 MB
Total cache (500M active): 500M × 1.2 MB = 600 TB

Bandwidth:
Write: 11,600 QPS × 1.2 KB = 14 MB/s
Read: 58,000 QPS × 1.2 KB = 70 MB/s
Fan-out: 2.32M QPS × 200 bytes = 464 MB/s
```


## Step 3: API Design

**POST /v1/posts**

```json
POST /v1/posts
Authorization: Bearer <token>
Content-Type: application/json

Request:
{
  "user_id": "user_123",
  "content": "Just finished designing a news feed system!",
  "media_urls": ["https://cdn.example.com/image1.jpg"],
  "post_type": "text",
  "privacy": "public",  // public, friends, private
  "location": {
    "lat": 37.7749,
    "lng": -122.4194,
    "name": "San Francisco"
  }
}

Response: 201 Created
{
  "post_id": "post_789xyz",
  "user_id": "user_123",
  "content": "Just finished designing a news feed system!",
  "created_at": "2025-10-03T05:49:00Z",
  "likes_count": 0,
  "comments_count": 0,
  "shares_count": 0
}
```

**GET /v1/feed**

```json
GET /v1/feed?user_id=user_123&limit=20&cursor=abc123
Authorization: Bearer <token>

Response: 200 OK
{
  "posts": [
    {
      "post_id": "post_789",
      "user": {
        "user_id": "user_456",
        "username": "john_doe",
        "avatar_url": "https://cdn.example.com/avatars/user456.jpg"
      },
      "content": "Amazing sunset today!",
      "media_urls": ["https://cdn.example.com/images/sunset.jpg"],
      "created_at": "2025-10-03T05:30:00Z",
      "likes_count": 1523,
      "comments_count": 89,
      "shares_count": 45,
      "is_liked": false,
      "relevance_score": 0.87
    }
  ],
  "next_cursor": "xyz789",
  "has_more": true,
  "generated_at": "2025-10-03T05:49:00Z"
}
```

**POST /v1/posts/{post_id}/like**

```json
POST /v1/posts/post_789/like
Authorization: Bearer <token>

Response: 200 OK
{
  "post_id": "post_789",
  "likes_count": 1524,
  "is_liked": true
}
```

**GET /v1/users/{user_id}/followers**

```json
GET /v1/users/user_123/followers?limit=100&offset=0

Response: 200 OK
{
  "followers": [
    {
      "user_id": "user_456",
      "username": "jane_smith",
      "followed_at": "2025-09-01T10:00:00Z"
    }
  ],
  "total_count": 5420,
  "has_more": true
}
```


## Step 4: Database Design

**SQL Schema (PostgreSQL):**

```sql
-- Users table
CREATE TABLE users (
    user_id BIGSERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    avatar_url TEXT,
    bio TEXT,
    created_at TIMESTAMP DEFAULT NOW(),
    last_active TIMESTAMP,
    
    INDEX idx_username (username),
    INDEX idx_email (email)
);

-- Posts table (partitioned by created_at)
CREATE TABLE posts (
    post_id BIGSERIAL,
    user_id BIGINT NOT NULL,
    content TEXT,
    media_urls TEXT[],
    post_type VARCHAR(20),  -- text, image, video, link
    privacy VARCHAR(20) DEFAULT 'public',
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP,
    likes_count INT DEFAULT 0,
    comments_count INT DEFAULT 0,
    shares_count INT DEFAULT 0,
    
    PRIMARY KEY (post_id, created_at)
) PARTITION BY RANGE (created_at);

-- Create monthly partitions
CREATE TABLE posts_2025_10 PARTITION OF posts 
    FOR VALUES FROM ('2025-10-01') TO ('2025-11-01');

-- Indexes
CREATE INDEX idx_posts_user_time ON posts(user_id, created_at DESC);
CREATE INDEX idx_posts_created ON posts(created_at DESC);

-- Followers/Following relationship (graph)
CREATE TABLE user_relationships (
    follower_id BIGINT NOT NULL,
    followee_id BIGINT NOT NULL,
    followed_at TIMESTAMP DEFAULT NOW(),
    
    PRIMARY KEY (follower_id, followee_id),
    FOREIGN KEY (follower_id) REFERENCES users(user_id),
    FOREIGN KEY (followee_id) REFERENCES users(user_id)
);

CREATE INDEX idx_followee ON user_relationships(followee_id);
CREATE INDEX idx_follower ON user_relationships(follower_id);

-- Likes
CREATE TABLE post_likes (
    user_id BIGINT NOT NULL,
    post_id BIGINT NOT NULL,
    liked_at TIMESTAMP DEFAULT NOW(),
    
    PRIMARY KEY (user_id, post_id)
) PARTITION BY HASH (post_id);

CREATE INDEX idx_post_likes ON post_likes(post_id, liked_at DESC);

-- Comments
CREATE TABLE comments (
    comment_id BIGSERIAL PRIMARY KEY,
    post_id BIGINT NOT NULL,
    user_id BIGINT NOT NULL,
    content TEXT NOT NULL,
    parent_comment_id BIGINT,  -- for nested comments
    created_at TIMESTAMP DEFAULT NOW(),
    likes_count INT DEFAULT 0,
    
    INDEX idx_post_comments (post_id, created_at DESC)
);
```

**NoSQL Schema (Cassandra for Feed Storage):**

```sql
-- Pre-computed feed storage
CREATE TABLE user_feeds (
    user_id BIGINT,
    post_id BIGINT,
    created_at TIMESTAMP,
    relevance_score FLOAT,
    
    PRIMARY KEY (user_id, created_at, post_id)
) WITH CLUSTERING ORDER BY (created_at DESC, post_id DESC);

-- Post denormalized data for fast read
CREATE TABLE post_cache (
    post_id BIGINT PRIMARY KEY,
    user_id BIGINT,
    username TEXT,
    avatar_url TEXT,
    content TEXT,
    media_urls LIST<TEXT>,
    created_at TIMESTAMP,
    likes_count INT,
    comments_count INT,
    shares_count INT
);
```

**Graph Database (Neo4j for Social Graph):**

```cypher
// Users
CREATE (u:User {
    user_id: 123,
    username: 'john_doe'
})

// Follow relationship
MATCH (follower:User {user_id: 123})
MATCH (followee:User {user_id: 456})
CREATE (follower)-[:FOLLOWS {since: datetime()}]->(followee)

// Query: Get all followers
MATCH (follower:User)-[:FOLLOWS]->(u:User {user_id: 123})
RETURN follower
```


## Step 5: High-Level Design

```
┌────────────────────────────────────────────────────────────────┐
│                         CDN (Media Files)                      │
└────────────────────────────────────────────────────────────────┘
                                  │
┌─────────────────────────────────▼──────────────────────────────┐
│                    Global Load Balancer                        │
│            (GeoDNS, Health Checks, SSL Termination)            │
└─────────────────────────────────┬──────────────────────────────┘
                                  │
        ┌─────────────────────────┼─────────────────────────┐
        │                         │                         │
┌───────▼─────────┐    ┌──────────▼──────────┐   ┌─────────▼─────────┐
│  Web Servers    │    │   Mobile API        │   │   WebSocket       │
│  (Read/Write)   │    │   Gateway           │   │   Server          │
└───────┬─────────┘    └──────────┬──────────┘   └─────────┬─────────┘
        │                         │                         │
        └─────────────────────────┼─────────────────────────┘
                                  │
┌─────────────────────────────────▼──────────────────────────────┐
│                      API Gateway Layer                         │
│           (Auth, Rate Limiting, Request Routing)               │
└──────────┬──────────────────────────────┬──────────────────────┘
           │                              │
    ┌──────▼──────┐            ┌──────────▼──────────┐
    │   Post      │            │   Feed Generation   │
    │   Service   │            │   Service           │
    └──────┬──────┘            └──────────┬──────────┘
           │                              │
           │                              │
    ┌──────▼───────────────────────┐      │
    │   Fan-out Service            │      │
    │   (Push to followers)        │      │
    └──────┬───────────────────────┘      │
           │                              │
           │                              │
    ┌──────▼───────────────────────────┐  │
    │   Kafka Message Queue            │  │
    │   Topics:                        │  │ 
    │   - post_created                 │  │
    │   - feed_updates                 │  │
    │   - engagement_events            │  │
    └──────┬───────────────────────────┘  │
           │                              │
    ┌──────▼────────────┬─────────────────▼──────────┐
    │  Fan-out Workers  │   Feed Cache Workers       │
    │  (Async process)  │   (Precompute & cache)     │
    └──────┬────────────┴─────────────────┬──────────┘
           │                              │
    ┌──────▼──────────────────────────────▼──────────┐
    │         Redis Cluster (Feed Cache)             │
    │   Key: user_id → List of post_ids (scored)     │
    │   Sorted Set: score = relevance + timestamp    │
    └────────────────────────┬───────────────────────┘
                             │
    ┌────────────────────────┴───────────────────────┐
    │                                                │
┌───▼─────────────┐    ┌───────────────┐   ┌───────▼────────┐
│  PostgreSQL     │    │  Cassandra    │   │   Neo4j        │
│  (Users, Posts) │    │  (Feed Store) │   │  (Social Graph)│
│  Sharded        │    │  Wide-column  │   │ (Relationships)│
└─────────────────┘    └───────────────┘   └────────────────┘
         │                     │                    │
    ┌────▼─────────────────────▼────────────────────▼────┐
    │         Elasticsearch (Search Service)             │
    │         Full-text search for posts, users          │
    └────────────────────────────────────────────────────┘
                             │
    ┌────────────────────────▼────────────────────────────┐
    │   ML Service (Ranking & Recommendation)             │
    │   - Feature extraction                              │
    │   - Relevance scoring                               │
    │   - Personalized ranking                            │
    └─────────────────────────────────────────────────────┘
```


## Step 6: Deep Dive - Fan-out Strategies

### Fan-out Models Comparison[^1][^2][^3]

**1. Pull Model (Fan-out-on-Read)**

<details>
<summary>C++ Code</summary>

```cpp
// User requests feed - generate on-demand
vector<Post> generateFeed(int64_t user_id) {
    // 1. Get list of followees
    vector<int64_t> followees = getFollowees(user_id);
    
    // 2. Fetch recent posts from each followee
    vector<Post> posts;
    for (int64_t followee : followees) {
        vector<Post> followee_posts = 
            getRecentPosts(followee, limit=100);
        posts.insert(posts.end(), 
                    followee_posts.begin(), 
                    followee_posts.end());
    }
    
    // 3. Merge and sort by timestamp
    sort(posts.begin(), posts.end(), 
         [](const Post& a, const Post& b) {
             return a.created_at > b.created_at;
         });
    
    // 4. Apply ranking algorithm
    vector<Post> ranked = rankPosts(posts, user_id);
    
    return ranked;
}
```

</details>

**Pros:**

- Low write latency (simple post creation)
- No wasted computation for inactive users
- Always shows fresh content

**Cons:**

- High read latency (O(followees) queries)
- Expensive for users following many accounts
- Database load spikes on feed requests

**2. Push Model (Fan-out-on-Write)**[^4][^5]

<details>
<summary>C++ Code</summary>

```cpp
// When user creates post - push to all followers
void publishPost(const Post& post) {
    // 1. Store post in database
    storePost(post);
    
    // 2. Get all followers
    vector<int64_t> followers = getFollowers(post.user_id);
    
    // 3. Fan-out: Write to each follower's feed
    for (int64_t follower_id : followers) {
        // Add post to follower's pre-computed feed cache
        redis.zadd(
            "feed:" + to_string(follower_id),
            post.created_at,  // score
            post.post_id      // member
        );
        
        // Keep only top 1000 posts in cache
        redis.zremrangebyrank(
            "feed:" + to_string(follower_id),
            0, -1001
        );
    }
    
    // 4. Publish real-time notification
    publishToWebSocket(followers, post);
}

// User requests feed - read from cache
vector<Post> generateFeed(int64_t user_id) {
    // 1. Get post IDs from cache (already sorted)
    vector<int64_t> post_ids = 
        redis.zrevrange("feed:" + to_string(user_id), 0, 19);
    
    // 2. Fetch post details (batch query)
    vector<Post> posts = batchGetPosts(post_ids);
    
    return posts;
}
```

</details>

**Pros:**

- Very fast feed reads (O(1) cache lookup)
- Reduced database load at read time
- Pre-computed and cached

**Cons:**

- Expensive writes for users with millions of followers
- Wasted computation for inactive users
- Write amplification (1 post → millions of writes)

**3. Hybrid Model (Recommended)**[^3][^4]

<details>
<summary>HybridFeedGenerator Class</summary>

```cpp
class HybridFeedGenerator {
private:
    static constexpr int CELEBRITY_THRESHOLD = 10000;
    static constexpr int ACTIVE_USER_THRESHOLD_DAYS = 7;
    
public:
    void publishPost(const Post& post) {
        storePost(post);
        
        int64_t follower_count = getFollowerCount(post.user_id);
        
        if (follower_count < CELEBRITY_THRESHOLD) {
            // Small follower count: Push to all
            fanOutToAll(post);
        } else {
            // Celebrity: Hybrid approach
            fanOutToActiveFollowers(post);
            markForPullRetrieval(post);
        }
    }
    
private:
    void fanOutToAll(const Post& post) {
        vector<int64_t> followers = getFollowers(post.user_id);
        
        // Async fan-out via Kafka
        for (int64_t follower : followers) {
            kafka.publish("feed_updates", {
                "follower_id": follower,
                "post_id": post.post_id,
                "timestamp": post.created_at
            });
        }
    }
    
    void fanOutToActiveFollowers(const Post& post) {
        // Only push to users active in last 7 days
        vector<int64_t> active_followers = 
            getActiveFollowers(
                post.user_id, 
                ACTIVE_USER_THRESHOLD_DAYS
            );
        
        for (int64_t follower : active_followers) {
            kafka.publish("feed_updates", {
                "follower_id": follower,
                "post_id": post.post_id,
                "timestamp": post.created_at
            });
        }
    }
    
    void markForPullRetrieval(const Post& post) {
        // Store in celebrity posts cache
        redis.zadd(
            "celebrity_posts:" + to_string(post.user_id),
            post.created_at,
            post.post_id
        );
    }
    
public:
    vector<Post> generateFeed(int64_t user_id) {
        vector<Post> feed_posts;
        
        // 1. Get posts from pre-computed feed (push)
        vector<int64_t> pushed_post_ids = 
            redis.zrevrange("feed:" + to_string(user_id), 0, 999);
        
        // 2. Get posts from celebrities (pull)
        vector<int64_t> celebrity_followees = 
            getCelebrityFollowees(user_id);
        
        for (int64_t celebrity : celebrity_followees) {
            vector<int64_t> celeb_posts = 
                redis.zrevrange(
                    "celebrity_posts:" + to_string(celebrity),
                    0, 99
                );
            pushed_post_ids.insert(
                pushed_post_ids.end(),
                celeb_posts.begin(),
                celeb_posts.end()
            );
        }
        
        // 3. Merge, deduplicate, and rank
        sort(pushed_post_ids.begin(), pushed_post_ids.end());
        pushed_post_ids.erase(
            unique(pushed_post_ids.begin(), pushed_post_ids.end()),
            pushed_post_ids.end()
        );
        
        // 4. Fetch post details and apply ML ranking
        vector<Post> posts = batchGetPosts(pushed_post_ids);
        return rankWithML(posts, user_id);
    }
};
```

</details>


### Feed Ranking Algorithm[^6][^7][^8]

**EdgeRank-inspired Scoring (Simplified):**

<details>
<summary>FeedRanker Class</summary>

```cpp
class FeedRanker {
public:
    struct RankingFeatures {
        double affinity_score;      // User-creator relationship
        double edge_weight;         // Post type weight
        double time_decay;          // Recency factor
        double engagement_score;    // Likes, comments, shares
        double content_quality;     // ML-predicted quality
    };
    
    double calculateRelevanceScore(
        const Post& post,
        int64_t user_id,
        const RankingFeatures& features
    ) {
        // EdgeRank formula: Score = Affinity × Weight × Decay
        // Modern: Add engagement and ML scores
        
        double score = 
            features.affinity_score * 0.3 +
            features.edge_weight * 0.15 +
            features.time_decay * 0.15 +
            features.engagement_score * 0.25 +
            features.content_quality * 0.15;
        
        return score;
    }
    
    double calculateAffinityScore(int64_t user_id, int64_t creator_id) {
        // Based on interaction history
        int likes = countLikes(user_id, creator_id);
        int comments = countComments(user_id, creator_id);
        int shares = countShares(user_id, creator_id);
        int profile_visits = countProfileVisits(user_id, creator_id);
        
        // Weighted sum
        double affinity = 
            likes * 1.0 +
            comments * 2.0 +      // Comments worth more
            shares * 3.0 +        // Shares worth most
            profile_visits * 0.5;
        
        // Normalize to [0, 1]
        return tanh(affinity / 100.0);
    }
    
    double calculateEdgeWeight(const Post& post) {
        // Different post types have different weights
        if (post.post_type == "video") return 1.5;
        if (post.post_type == "image") return 1.2;
        if (post.post_type == "link") return 1.0;
        if (post.post_type == "text") return 0.8;
        return 1.0;
    }
    
    double calculateTimeDecay(time_t post_time) {
        // Exponential decay
        time_t now = time(nullptr);
        double hours_old = (now - post_time) / 3600.0;
        
        // Half-life of 6 hours
        return exp(-0.1155 * hours_old);
    }
    
    double calculateEngagementScore(const Post& post) {
        // Viral coefficient
        double total_engagement = 
            post.likes_count * 1.0 +
            post.comments_count * 2.0 +
            post.shares_count * 3.0;
        
        // Normalize by follower count to avoid celebrity bias
        int64_t follower_count = getFollowerCount(post.user_id);
        double engagement_rate = total_engagement / 
                                 max(follower_count, 1L);
        
        return min(engagement_rate * 100, 1.0);
    }
    
    vector<Post> rankPosts(vector<Post> posts, int64_t user_id) {
        // Calculate scores for all posts
        for (Post& post : posts) {
            RankingFeatures features;
            features.affinity_score = 
                calculateAffinityScore(user_id, post.user_id);
            features.edge_weight = calculateEdgeWeight(post);
            features.time_decay = calculateTimeDecay(post.created_at);
            features.engagement_score = 
                calculateEngagementScore(post);
            features.content_quality = 
                mlService.predictQuality(post);
            
            post.relevance_score = 
                calculateRelevanceScore(post, user_id, features);
        }
        
        // Sort by relevance score (descending)
        sort(posts.begin(), posts.end(),
             [](const Post& a, const Post& b) {
                 return a.relevance_score > b.relevance_score;
             });
        
        return posts;
    }
};
```

</details>


## Step 7: Bottlenecks \& Optimizations

### Performance Optimizations

**1. Caching Strategy:**

```
L1: Application-level cache (in-memory)
    - Recently accessed posts: 5 min TTL
    - User profiles: 10 min TTL

L2: Redis Cluster (distributed cache)
    - Pre-computed feeds: 30 min TTL
    - Post metadata: 1 hour TTL
    - Social graph subset: 1 hour TTL

L3: CDN (edge caching)
    - Static assets (images, videos)
    - Profile avatars
    - Public posts
```

**2. Database Optimizations:**

```sql
-- Denormalize frequently accessed data
CREATE MATERIALIZED VIEW user_feed_summary AS
SELECT 
    f.follower_id,
    COUNT(p.post_id) as post_count,
    MAX(p.created_at) as last_post_time
FROM user_relationships f
JOIN posts p ON f.followee_id = p.user_id
WHERE p.created_at > NOW() - INTERVAL '7 days'
GROUP BY f.follower_id;

-- Refresh periodically
REFRESH MATERIALIZED VIEW user_feed_summary;
```

**3. Async Processing:**

```
Write Path (Non-blocking):
1. User creates post → Return 201 immediately
2. Background: Publish to Kafka
3. Workers: Process fan-out async
4. Eventual consistency: Feeds updated within seconds

Benefits:
- Low write latency (<100ms)
- Handles traffic spikes
- Fault tolerance (retry failed fan-outs)
```

**4. Hot Spot Mitigation:**

```
Celebrity problem:
- Shard celebrity's followers across multiple workers
- Rate limit fan-out (batch updates every 5 min)
- Use pull model for mega-celebrities (>10M followers)

Thundering herd:
- Request coalescing for popular posts
- Stale-while-revalidate cache pattern
- Circuit breaker on DB queries
```

**5. Real-time Updates:**

<details>
<summary>FeedUpdateNotifier Class</summary>

```cpp
// WebSocket for live feed updates
class FeedUpdateNotifier {
public:
    void notifyFollowers(const Post& post) {
        vector<int64_t> online_followers = 
            getOnlineFollowers(post.user_id);
        
        for (int64_t follower : online_followers) {
            if (isWebSocketConnected(follower)) {
                sendWebSocketMessage(follower, {
                    "type": "new_post",
                    "post_id": post.post_id,
                    "preview": post.content.substr(0, 100)
                });
            }
        }
    }
};
```

</details>


### Trade-offs

| Aspect | Pull Model | Push Model | Hybrid |
| :-- | :-- | :-- | :-- |
| **Read Latency** | High (slow) | Low (fast) | Low |
| **Write Latency** | Low (fast) | High (slow) | Medium |
| **Storage** | Low | High | Medium |
| **Consistency** | Strong | Eventual | Eventual |
| **Celebrity Support** | Good | Poor | Excellent |
| **Best For** | Write-heavy | Read-heavy | Production |

### Monitoring \& Alerts

```
Key Metrics:
- Feed generation latency (P50, P95, P99)
- Fan-out lag (time from post to feed appearance)
- Cache hit rate (target: >90%)
- Post creation rate (QPS)
- Feed read rate (QPS)
- Kafka consumer lag

Alerts:
- Feed latency > 1s (P99)
- Cache hit rate < 80%
- Kafka lag > 1 million messages
- Database connection pool exhaustion
- Fan-out worker failures
```

This design handles 500M DAU with <500ms feed generation by combining push/pull strategies, aggressive caching, and async processing.[^9][^2][^5][^1][^3]
<span style="display:none">[^10][^11][^12][^13][^14][^15][^16][^17][^18]</span>

<div align="center">⁂</div>

[^1]: https://bytebytego.com/courses/system-design-interview/design-a-news-feed-system

[^2]: https://dev.to/sgchris/designing-a-news-feed-system-facebook-and-twitter-architecture-5292

[^3]: https://liuzhenglaichn.gitbook.io/system-design/news-feed/design-a-news-feed-system

[^4]: https://www.designgurus.io/blog/design-social-media-news-feed

[^5]: https://algodaily.com/lessons/dive-into-facebook-newsfeed-architecture

[^6]: https://www.amraandelma.com/top-facebook-algorithm-statistics/

[^7]: https://en.wikipedia.org/wiki/EdgeRank

[^8]: https://www.geeksforgeeks.org/blogs/edgerank-algorithm-facebook-news-feed/

[^9]: https://www.hellointerview.com/learn/system-design/problem-breakdowns/fb-news-feed

[^10]: https://stackoverflow.com/questions/50236862/push-vs-pull-when-designing-social-networks-twitter-fb-news-feed-etc

[^11]: https://www.0xkishan.com/blogs/designing-a-scalable-news-feed-architecture

[^12]: https://www.linkedin.com/pulse/system-design-newsfeed-momen-negm-cm3nf

[^13]: https://www.greatfrontend.com/questions/system-design/news-feed-facebook

[^14]: https://blog.algomaster.io/p/designing-a-scalable-news-feed-system

[^15]: https://buffer.com/resources/facebook-algorithm/

[^16]: https://www.newamerica.org/oti/reports/rising-through-ranks/news-feed-ranking/

[^17]: https://en.ryte.com/wiki/Edge_Rank

[^18]: https://www.smartinsights.com/social-media-marketing/facebook-marketing/how-to-use-facebook-edgerank/

