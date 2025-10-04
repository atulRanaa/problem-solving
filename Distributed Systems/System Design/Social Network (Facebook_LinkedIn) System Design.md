<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

# Social Network (Facebook/LinkedIn) System Design

## Step 1: Requirements Clarification

### Functional Requirements

**User Profile:**

- Create/update profile (name, bio, photo, cover photo)
- Privacy settings (public, friends-only, private)
- Education, work experience, skills (LinkedIn)
- Profile views tracking

**Connections/Friendships:**

- Send/accept/reject friend requests
- Follow users (one-way)
- Block/unblock users
- Connection suggestions
- Mutual friends

**Posts \& Feed:**

- Create post (text, images, videos, links)
- Like, comment, share posts
- Tag users in posts
- News feed algorithm (personalized)
- Trending posts/topics

**Groups:**

- Create/join groups
- Group posts and discussions
- Group admins \& moderation

**Notifications:**

- Real-time notifications (likes, comments, friend requests)
- Unread count
- Push notifications

**Search:**

- Search users
- Search posts
- Search groups
- Typeahead suggestions

**Messaging:**

- Send/receive messages (out of scope - separate system)

**Out of Scope:**

- Live video streaming
- Stories/ephemeral content
- Marketplace
- Events
- Dating features


### Non-Functional Requirements

**Scale (Based on 2025 data):**

- Monthly active users: 3.07 billion (Facebook)[^1]
- Daily active users: 2.11 billion (Facebook)[^1]
- LinkedIn: 1.20 billion members, 310M monthly active[^2][^3]
- Posts per day: ~500 million (estimate)
- Post interactions: 4 billion/day (likes, comments, shares)

**Performance:**

- News feed generation: <1 second
- Post creation: <500ms
- Search: <200ms
- Notification delivery: <5 seconds

**Reliability:**

- 99.99% uptime
- No data loss (posts, connections)
- Eventual consistency for feeds
- Strong consistency for connections

**Availability:**

- Multi-region deployment
- Graceful degradation
- Offline mode support

***

## Step 2: Social Network Theory \& Concepts

### 2.1 Graph Storage - Social Connections

**Problem: Storing Friend Relationships**

```
Relational Database (Naive):
CREATE TABLE friendships (
    user_id1 INT,
    user_id2 INT,
    PRIMARY KEY (user_id1, user_id2)
);

Query: Find Alice's friends
SELECT user_id2 FROM friendships WHERE user_id1 = 'alice';

Query: Find friends of friends (2nd degree)
SELECT DISTINCT f2.user_id2 
FROM friendships f1
JOIN friendships f2 ON f1.user_id2 = f2.user_id1
WHERE f1.user_id1 = 'alice';

Problems:
❌ Slow for multi-hop queries (friends of friends of friends)
❌ Not optimized for graph traversal
❌ Difficult to scale
```

**Better: Adjacency List (Graph Database)**

```
Graph Representation:

Alice → [Bob, Charlie, David]
Bob → [Alice, Eve, Frank]
Charlie → [Alice, David]
David → [Alice, Charlie, Frank]

Storage:
User: Alice
Connections: Bob, Charlie, David (adjacency list)

Query: Friends of friends
1. Get Alice's friends: [Bob, Charlie, David]
2. For each friend, get their friends
3. Union all sets, exclude Alice and direct friends
Result: [Eve, Frank]

Neo4j Cypher Query:
MATCH (alice:User {id: 'alice'})-[:FRIEND]-(friend)-[:FRIEND]-(fof)
WHERE fof.id <> 'alice' AND NOT (alice)-[:FRIEND]-(fof)
RETURN DISTINCT fof

Time: O(k) where k = friends + friends-of-friends
Much faster than SQL joins!
```


### 2.2 News Feed Generation - Fan-Out Strategies

**Challenge: Alice posts → Show to 5,000 friends**

**Approach 1: Fan-Out on Write (Push Model)**

```
When Alice creates post:
1. Get Alice's 5,000 friends
2. For each friend:
   - Insert post into friend's feed table
   - Feed_Bob: [Alice's post, ...]
   - Feed_Charlie: [Alice's post, ...]
   
Pros:
✅ Fast reads (feed pre-computed)
✅ Simple query: SELECT * FROM feed WHERE user_id = ?

Cons:
❌ Slow writes (5,000 inserts per post!)
❌ Storage explosion (duplicate posts)
❌ Celebrity problem (1M followers = 1M writes)

When to use: Small networks, equal popularity
```

**Approach 2: Fan-Out on Read (Pull Model)**

```
When Alice creates post:
1. Store post in Alice's posts table
2. Done! (instant)

When Bob loads feed:
1. Get Bob's friends: [Alice, Charlie, David, ...]
2. For each friend, fetch recent posts
3. Merge-sort by timestamp
4. Return top 50

Pros:
✅ Fast writes (single INSERT)
✅ No duplication
✅ Handles celebrities well

Cons:
❌ Slow reads (query 5,000 friends' posts!)
❌ Complex merge logic
❌ Database load on read

When to use: Large networks, varied popularity
```

**Approach 3: Hybrid (Facebook's Approach)**

```
Strategy: Use both!

For normal users (< 10K followers):
  → Fan-out on write (push to followers' feeds)

For celebrities (> 10K followers):
  → Fan-out on read (query when viewing feed)

When Bob loads feed:
1. Fetch pre-computed feed (normal friends)
   SELECT * FROM feed WHERE user_id = 'Bob' LIMIT 50
   
2. Fetch celebrity posts on-demand
   SELECT * FROM posts WHERE user_id IN (celebrity_ids)
   LIMIT 10
   
3. Merge both lists by timestamp
4. Return top 50

Result: Best of both worlds!
Fast for most users, scales for celebrities
```


### 2.3 Feed Ranking Algorithm

**Simple: Chronological**

```
Problem: Information overload
If Alice has 5,000 friends posting 10 times/day = 50,000 posts!

Solution: Show "top" posts based on engagement

EdgeRank Algorithm (Simplified):
Score = Affinity × Weight × Time_Decay

Affinity: How close are you?
- Direct messages: 10
- Comments: 5
- Likes: 1
- No interaction: 0

Weight: Type of content
- Video: 10
- Photo: 5
- Status: 2
- Link: 1

Time Decay: Recency
Score = Score × e^(-λt)
λ = decay constant
t = hours since post

Example:
Alice posts video (Weight=10)
Bob frequently interacts with Alice (Affinity=8)
Posted 2 hours ago (Decay=0.8)
Score = 8 × 10 × 0.8 = 64

Charlie posts status (Weight=2)
Bob rarely interacts (Affinity=2)
Posted 1 hour ago (Decay=0.9)
Score = 2 × 2 × 0.9 = 3.6

Result: Alice's post ranks higher!
```

**Modern: Machine Learning**

```
Features (100s of features):
- User interactions (likes, comments, shares)
- Post type (video, photo, link)
- Time of day
- Device type
- User demographics
- Explicit feedback (hide post, report)

Model: Gradient Boosted Trees / Neural Network
Objective: Maximize engagement (time spent, interactions)

Training:
Positive examples: Posts user engaged with
Negative examples: Posts user scrolled past

Inference:
For each candidate post → Predict engagement score
Sort by score → Return top N

Facebook uses thousands of ML models!
```


### 2.4 Connection Suggestions (People You May Know)

**Algorithms:**

```
1. Mutual Friends (Most Common)
Alice → [Bob, Charlie, David]
Bob → [Alice, Eve, Charlie]

Suggestions for Alice:
Eve (1 mutual friend: Bob)
Score: 1 / sqrt(Alice_friends × Eve_friends)

2. Graph Distance
Find users 2-3 hops away
Closer = higher score

3. Common Groups
Alice in groups: [Tech, Hiking]
Bob in groups: [Tech, Cooking]
Common: 1 group → Suggest Bob

4. Work/School
Same company/university → High score

5. Contact Upload
Alice's phone has Bob's number → Suggest Bob

Combined Score:
Final_Score = w1×mutual_friends + w2×distance + w3×groups + w4×workplace

Top 10 suggestions shown
```


***

## Step 3: Capacity Estimation

```
Users & Activity:
Monthly active users: 3.07 billion [web:370]
Daily active users: 2.11 billion [web:370]
Average session time: 30 minutes/day
Sessions per day: 2-3

Posts:
Daily posts: 500 million (estimate)
Posts per second: 500M / 86,400 = 5,787 posts/sec
Average post size: 500 bytes (text) to 5 MB (video)
Post types: Text (40%), Images (50%), Videos (10%)

Post Interactions:
Daily likes: 4 billion
Daily comments: 500 million
Daily shares: 100 million
Total interactions: 4.6 billion/day = 53,240 interactions/sec

Connections:
Average friends per user: 338 (Facebook average)
Total connections: 3.07B × 338 / 2 = 519 billion edges
Connection graph size: 519B × 16 bytes = 8.3 TB

News Feed:
Feed generations: 2.11B DAU × 5 sessions = 10.55 billion feeds/day
Feed generations per second: 10.55B / 86,400 = 122K feeds/sec
Posts per feed: 50-100 posts
Feed computation: 122K × 50 = 6.1M post evaluations/sec

Notifications:
Daily notifications: 4.6B interactions → 4.6B notifications
Notifications per second: 53,240/sec
Notification types: Likes (87%), Comments (11%), Friend requests (2%)

Search:
Daily searches: 500 million
Searches per second: 5,787 QPS
Typeahead queries: 5× more = 28,935 QPS

Storage:
Users: 3.07B × 5 KB = 15.35 TB
Posts (1 year): 500M/day × 365 days × 5 KB = 912 TB
Images: 250M/day × 365 days × 500 KB = 45.6 PB
Videos: 50M/day × 365 days × 10 MB = 182 PB
Total media: ~228 PB/year
Connections graph: 8.3 TB
Total: ~230 PB

Database Operations:
Post writes: 5,787 writes/sec
Interaction writes: 53,240 writes/sec
Total writes: 59,027 writes/sec

Profile reads: 2.11B DAU × 10 profile views/day = 244,212 reads/sec
Feed reads: 122K feeds/sec × 50 posts = 6.1M reads/sec (with cache: 610K)
Total reads: 854K reads/sec (with 90% cache hit)

Cache (Redis):
Hot users: 100M users × 5 KB = 500 GB
Recent posts: 500M posts × 1 KB = 500 GB
Feed cache: 100M users × 50 posts × 1 KB = 5 TB
Total cache: ~6 TB

Network Bandwidth:
Feed loads: 122K/sec × 500 KB = 61 GB/sec
Image loads: 250M/day / 86,400 × 500 KB = 1.4 GB/sec
Video streams: 50M/day / 86,400 × 1 MB/sec bitrate = 578 GB/sec
Total: ~640 GB/sec (with CDN: 32 GB/sec to origin)
```


***

## Step 4: API Design

### User Profile APIs

```json
GET /api/v1/users/{user_id}

Response: 200 OK
{
  "user_id": "user_123",
  "username": "john_doe",
  "full_name": "John Doe",
  "bio": "Software Engineer at Tech Corp",
  "profile_photo": "https://cdn.facebook.com/photos/user_123.jpg",
  "cover_photo": "https://cdn.facebook.com/photos/cover_123.jpg",
  "location": "San Francisco, CA",
  "friend_count": 542,
  "follower_count": 1234,
  "created_at": "2015-03-15T10:00:00Z",
  "mutual_friends": 15,  // If requester is authenticated
  "connection_status": "friends"  // friends, pending, not_connected
}

POST /api/v1/users/{user_id}/profile
Request:
{
  "bio": "Updated bio",
  "location": "New York, NY",
  "work": {
    "company": "Tech Corp",
    "position": "Senior Engineer",
    "start_date": "2020-01-01"
  }
}

Response: 200 OK
```


### Connection APIs

```json
POST /api/v1/connections/request
Request:
{
  "to_user_id": "user_456",
  "message": "Hi! We met at the conference."
}

Response: 201 Created
{
  "request_id": "req_abc123",
  "status": "pending",
  "sent_at": "2025-10-04T16:37:00Z"
}

POST /api/v1/connections/accept
Request:
{
  "request_id": "req_abc123"
}

Response: 200 OK
{
  "connection_id": "conn_xyz789",
  "status": "connected",
  "connected_at": "2025-10-04T16:38:00Z"
}

GET /api/v1/users/{user_id}/friends?page=1&limit=50

Response: 200 OK
{
  "total_count": 542,
  "friends": [
    {
      "user_id": "user_456",
      "username": "jane_smith",
      "full_name": "Jane Smith",
      "profile_photo": "...",
      "mutual_friends": 23,
      "connected_since": "2018-05-20T00:00:00Z"
    }
  ],
  "next_page": 2,
  "has_more": true
}

GET /api/v1/users/{user_id}/suggestions?limit=10

Response: 200 OK
{
  "suggestions": [
    {
      "user_id": "user_789",
      "full_name": "Bob Johnson",
      "profile_photo": "...",
      "reason": "15 mutual friends",
      "mutual_friends": 15,
      "score": 0.87
    }
  ]
}
```


### Post \& Feed APIs

```json
POST /api/v1/posts
Request:
{
  "content": "Just finished an amazing project!",
  "media": [
    {
      "type": "image",
      "url": "https://upload.facebook.com/img123.jpg"
    }
  ],
  "tags": ["user_456", "user_789"],
  "privacy": "friends"  // public, friends, private
}

Response: 201 Created
{
  "post_id": "post_abc123",
  "user_id": "user_123",
  "content": "Just finished an amazing project!",
  "media": [...],
  "created_at": "2025-10-04T16:37:00Z",
  "like_count": 0,
  "comment_count": 0,
  "share_count": 0
}

GET /api/v1/feed?limit=50&cursor=abc123

Response: 200 OK
{
  "feed": [
    {
      "post_id": "post_xyz789",
      "author": {
        "user_id": "user_456",
        "username": "jane_smith",
        "profile_photo": "..."
      },
      "content": "Great day at the beach!",
      "media": [...],
      "created_at": "2025-10-04T14:30:00Z",
      "like_count": 42,
      "comment_count": 8,
      "share_count": 3,
      "liked_by_user": false,
      "ranking_score": 87.5
    }
  ],
  "next_cursor": "def456",
  "has_more": true
}

POST /api/v1/posts/{post_id}/like

Response: 201 Created
{
  "post_id": "post_abc123",
  "like_count": 43,
  "liked_by_user": true
}

POST /api/v1/posts/{post_id}/comments
Request:
{
  "content": "Amazing work!",
  "parent_comment_id": null  // For replies
}

Response: 201 Created
{
  "comment_id": "comment_xyz",
  "post_id": "post_abc123",
  "user_id": "user_123",
  "content": "Amazing work!",
  "created_at": "2025-10-04T16:40:00Z",
  "like_count": 0
}
```


### Notification APIs

```json
GET /api/v1/notifications?limit=20&unread_only=false

Response: 200 OK
{
  "unread_count": 5,
  "notifications": [
    {
      "notification_id": "notif_123",
      "type": "like",
      "actor": {
        "user_id": "user_456",
        "username": "jane_smith",
        "profile_photo": "..."
      },
      "target": {
        "post_id": "post_abc123",
        "content_preview": "Just finished an amazing..."
      },
      "message": "Jane Smith liked your post",
      "created_at": "2025-10-04T16:35:00Z",
      "read": false
    },
    {
      "notification_id": "notif_124",
      "type": "comment",
      "actor": {...},
      "target": {...},
      "message": "Bob Johnson commented on your post",
      "created_at": "2025-10-04T16:30:00Z",
      "read": false
    },
    {
      "notification_id": "notif_125",
      "type": "friend_request",
      "actor": {...},
      "message": "Alice wants to be friends",
      "created_at": "2025-10-04T15:00:00Z",
      "read": true
    }
  ]
}

POST /api/v1/notifications/{notification_id}/read

Response: 204 No Content

// WebSocket for real-time notifications
WS /api/v1/notifications/stream

Server → Client:
{
  "type": "new_notification",
  "notification": {...}
}
```


***

## Step 5: Database Design

### PostgreSQL Schema

```sql
-- Users
CREATE TABLE users (
    user_id BIGSERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255),
    full_name VARCHAR(200),
    bio TEXT,
    profile_photo_url TEXT,
    cover_photo_url TEXT,
    location VARCHAR(200),
    birthday DATE,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    last_active_at TIMESTAMPTZ,
    
    INDEX idx_username (username),
    INDEX idx_email (email),
    INDEX idx_created (created_at DESC)
);

-- Connections/Friendships (undirected graph)
CREATE TABLE connections (
    connection_id BIGSERIAL PRIMARY KEY,
    user_id1 BIGINT REFERENCES users(user_id),
    user_id2 BIGINT REFERENCES users(user_id),
    status VARCHAR(20) DEFAULT 'pending',  -- pending, accepted, blocked
    created_at TIMESTAMPTZ DEFAULT NOW(),
    accepted_at TIMESTAMPTZ,
    
    UNIQUE(user_id1, user_id2),
    CHECK (user_id1 < user_id2),  -- Ensure consistent ordering
    INDEX idx_user1 (user_id1, status),
    INDEX idx_user2 (user_id2, status)
);

-- Follows (directed, for LinkedIn/Twitter-style)
CREATE TABLE follows (
    follower_id BIGINT REFERENCES users(user_id),
    followee_id BIGINT REFERENCES users(user_id),
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    PRIMARY KEY (follower_id, followee_id),
    INDEX idx_follower (follower_id),
    INDEX idx_followee (followee_id)
);

-- Posts
CREATE TABLE posts (
    post_id BIGSERIAL PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id),
    content TEXT,
    privacy VARCHAR(20) DEFAULT 'friends',  -- public, friends, private
    post_type VARCHAR(20),  -- status, photo, video, link
    
    -- Counters (cached)
    like_count INT DEFAULT 0,
    comment_count INT DEFAULT 0,
    share_count INT DEFAULT 0,
    
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW(),
    
    INDEX idx_user_posts (user_id, created_at DESC),
    INDEX idx_created (created_at DESC)
) PARTITION BY RANGE (created_at);

-- Partition by month
CREATE TABLE posts_2025_10 PARTITION OF posts
    FOR VALUES FROM ('2025-10-01') TO ('2025-11-01');

-- Post media
CREATE TABLE post_media (
    media_id BIGSERIAL PRIMARY KEY,
    post_id BIGINT REFERENCES posts(post_id) ON DELETE CASCADE,
    media_type VARCHAR(20),  -- image, video
    media_url TEXT NOT NULL,
    thumbnail_url TEXT,
    width INT,
    height INT,
    position INT DEFAULT 0,
    
    INDEX idx_post_media (post_id, position)
);

-- Likes
CREATE TABLE post_likes (
    user_id BIGINT REFERENCES users(user_id),
    post_id BIGINT REFERENCES posts(post_id),
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    PRIMARY KEY (user_id, post_id),
    INDEX idx_post_likes (post_id, created_at DESC)
);

-- Comments
CREATE TABLE comments (
    comment_id BIGSERIAL PRIMARY KEY,
    post_id BIGINT REFERENCES posts(post_id),
    user_id BIGINT REFERENCES users(user_id),
    parent_comment_id BIGINT REFERENCES comments(comment_id),  -- For replies
    content TEXT NOT NULL,
    like_count INT DEFAULT 0,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    INDEX idx_post_comments (post_id, created_at DESC),
    INDEX idx_user_comments (user_id, created_at DESC)
);

-- News feed (pre-computed, fan-out on write)
CREATE TABLE news_feed (
    feed_id BIGSERIAL PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id),
    post_id BIGINT REFERENCES posts(post_id),
    ranking_score FLOAT DEFAULT 0,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    UNIQUE(user_id, post_id),
    INDEX idx_user_feed (user_id, ranking_score DESC, created_at DESC)
) PARTITION BY HASH (user_id);

-- 16 partitions for horizontal scaling
CREATE TABLE news_feed_0 PARTITION OF news_feed FOR VALUES WITH (MODULUS 16, REMAINDER 0);
CREATE TABLE news_feed_1 PARTITION OF news_feed FOR VALUES WITH (MODULUS 16, REMAINDER 1);
-- ... up to 15

-- Notifications
CREATE TABLE notifications (
    notification_id BIGSERIAL PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id),
    actor_id BIGINT REFERENCES users(user_id),
    type VARCHAR(50),  -- like, comment, friend_request, tag
    target_type VARCHAR(50),  -- post, comment, user
    target_id BIGINT,
    message TEXT,
    read BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    INDEX idx_user_notifications (user_id, read, created_at DESC)
);

-- Groups
CREATE TABLE groups (
    group_id BIGSERIAL PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    privacy VARCHAR(20) DEFAULT 'public',  -- public, private
    created_by BIGINT REFERENCES users(user_id),
    created_at TIMESTAMPTZ DEFAULT NOW(),
    member_count INT DEFAULT 0,
    
    INDEX idx_created (created_at DESC)
);

CREATE TABLE group_members (
    group_id BIGINT REFERENCES groups(group_id),
    user_id BIGINT REFERENCES users(user_id),
    role VARCHAR(20) DEFAULT 'member',  -- admin, member
    joined_at TIMESTAMPTZ DEFAULT NOW(),
    
    PRIMARY KEY (group_id, user_id),
    INDEX idx_user_groups (user_id)
);
```


### Graph Database (Neo4j) - For Connections

```cypher
// User node
CREATE (u:User {
  user_id: 123,
  username: 'john_doe',
  created_at: datetime()
})

// Friendship relationship
MATCH (a:User {user_id: 123}), (b:User {user_id: 456})
CREATE (a)-[:FRIEND {since: datetime()}]->(b)

// Find friends
MATCH (u:User {user_id: 123})-[:FRIEND]-(friend)
RETURN friend

// Find friends of friends
MATCH (u:User {user_id: 123})-[:FRIEND]-(friend)-[:FRIEND]-(fof)
WHERE fof.user_id <> 123 AND NOT (u)-[:FRIEND]-(fof)
RETURN DISTINCT fof

// Find shortest path
MATCH path = shortestPath(
  (a:User {user_id: 123})-[:FRIEND*]-(b:User {user_id: 789})
)
RETURN path

// Connection suggestions (mutual friends)
MATCH (u:User {user_id: 123})-[:FRIEND]-(friend)-[:FRIEND]-(suggestion)
WHERE NOT (u)-[:FRIEND]-(suggestion) AND suggestion.user_id <> 123
WITH suggestion, COUNT(friend) AS mutual_count
ORDER BY mutual_count DESC
RETURN suggestion, mutual_count
LIMIT 10
```


### Redis Cache

```redis
# User profile cache
HSET user:123 "username" "john_doe" "full_name" "John Doe" "friend_count" "542"
EXPIRE user:123 3600  # 1 hour

# Friend list (sorted set by recency)
ZADD friends:123 1728048000 "456"
ZADD friends:123 1728048100 "789"

# News feed cache (top 50 posts)
ZADD feed:123 0.95 "post_1"  # Score = ranking score
ZADD feed:123 0.87 "post_2"
ZREVRANGE feed:123 0 49 WITHSCORES

# Post cache
HSET post:abc123 "content" "Great day!" "like_count" "42" "user_id" "123"
EXPIRE post:abc123 3600

# Online users (for real-time features)
SADD online_users "123"
EXPIRE online_users:123 300  # 5 minutes

# Notification count
INCR notif_count:123
```


### Elasticsearch (Search)

```json
PUT /users
{
  "mappings": {
    "properties": {
      "user_id": {"type": "keyword"},
      "username": {"type": "text"},
      "full_name": {
        "type": "text",
        "fields": {
          "keyword": {"type": "keyword"}
        }
      },
      "bio": {"type": "text"},
      "location": {"type": "text"},
      "friend_count": {"type": "integer"}
    }
  }
}

PUT /posts
{
  "mappings": {
    "properties": {
      "post_id": {"type": "keyword"},
      "user_id": {"type": "keyword"},
      "content": {"type": "text"},
      "created_at": {"type": "date"},
      "like_count": {"type": "integer"}
    }
  }
}
```


## Step 6: High-Level Architecture

```mermaid
graph TB
    subgraph "Clients"
        WEB[Web Browser]
        MOBILE[Mobile App]
        TABLET[Tablet App]
    end
    
    subgraph "CDN & Edge"
        CDN[CDN<br/>CloudFront<br/>Static assets<br/>Images, videos]
        
        EDGE[Edge Computing<br/>Real-time features<br/>Notifications]
    end
    
    subgraph "Load Balancer"
        LB[Load Balancer<br/>Nginx/HAProxy<br/>SSL termination]
    end
    
    subgraph "API Gateway"
        GATEWAY[API Gateway<br/>Auth, rate limiting<br/>Request routing]
        
        WS_GATEWAY[WebSocket Gateway<br/>Real-time connections<br/>2M concurrent]
    end
    
    subgraph "Core Services"
        USER_SVC[User Service<br/>Profile CRUD<br/>Authentication]
        
        CONNECTION_SVC[Connection Service<br/>Friend requests<br/>Graph operations]
        
        POST_SVC[Post Service<br/>Create/edit posts<br/>Media upload]
        
        FEED_SVC[Feed Service<br/>News feed generation<br/>Ranking algorithm]
        
        INTERACTION_SVC[Interaction Service<br/>Likes, comments<br/>Shares]
        
        NOTIFICATION_SVC[Notification Service<br/>Real-time alerts<br/>Push notifications]
        
        SEARCH_SVC[Search Service<br/>Users, posts<br/>Typeahead]
        
        GROUP_SVC[Group Service<br/>Create/join groups<br/>Group posts]
    end
    
    subgraph "Feed Generation Pipeline"
        FEED_WRITER[Feed Writer<br/>Fan-out on write<br/>Push to followers]
        
        FEED_RANKER[Feed Ranker<br/>ML-based ranking<br/>EdgeRank algorithm]
        
        FEED_CACHE[Feed Cache<br/>Pre-computed feeds<br/>Hot users]
    end
    
    subgraph "Databases"
        PG_MASTER[(PostgreSQL Master<br/>Users, posts<br/>Strong consistency)]
        
        PG_REPLICA[(PostgreSQL Replicas<br/>Read scaling<br/>20 replicas)]
        
        NEO4J[Neo4j Graph DB<br/>Social connections<br/>Graph traversal<br/>519B edges)]
        
        CASSANDRA[(Cassandra<br/>News feed storage<br/>Time-series data)]
        
        REDIS_CACHE[Redis Cluster<br/>User cache<br/>Feed cache<br/>6 TB)]
        
        REDIS_COUNTER[Redis<br/>Counters<br/>Likes, comments]
    end
    
    subgraph "Storage"
        S3_PHOTOS[S3 - Photos<br/>45.6 PB<br/>Compressed]
        
        S3_VIDEOS[S3 - Videos<br/>182 PB<br/>Multiple qualities]
    end
    
    subgraph "Message Queue"
        KAFKA[Kafka<br/>Post events<br/>Notification events<br/>Feed updates]
    end
    
    subgraph "Background Workers"
        FEED_WORKER[Feed Generator<br/>Batch feed updates<br/>Offline users]
        
        NOTIF_WORKER[Notification Worker<br/>Process notifications<br/>Batch delivery]
        
        MEDIA_WORKER[Media Processor<br/>Image resize<br/>Video transcoding]
        
        SUGGEST_WORKER[Suggestion Worker<br/>Friend suggestions<br/>Graph algorithms]
    end
    
    subgraph "Search & Analytics"
        ES[Elasticsearch<br/>User search<br/>Post search<br/>Typeahead]
        
        ML_SERVICE[ML Service<br/>Feed ranking<br/>Content moderation<br/>Recommendations]
        
        ANALYTICS[Analytics Service<br/>User behavior<br/>Engagement metrics]
        
        DATAWAREHOUSE[(Data Warehouse<br/>Redshift<br/>Historical analysis)]
    end
    
    subgraph "External Services"
        PUSH[Push Service<br/>APNs, FCM<br/>Mobile notifications]
        
        EMAIL[SendGrid<br/>Email notifications<br/>Weekly digests]
        
        VISION_API[Vision API<br/>Image recognition<br/>Content moderation]
    end
    
    WEB & MOBILE & TABLET --> CDN
    CDN --> LB
    LB --> GATEWAY
    LB --> WS_GATEWAY
    
    GATEWAY --> USER_SVC
    GATEWAY --> CONNECTION_SVC
    GATEWAY --> POST_SVC
    GATEWAY --> FEED_SVC
    GATEWAY --> SEARCH_SVC
    
    POST_SVC --> KAFKA
    INTERACTION_SVC --> KAFKA
    
    KAFKA --> FEED_WRITER
    KAFKA --> NOTIF_WORKER
    KAFKA --> MEDIA_WORKER
    
    FEED_WRITER --> CASSANDRA
    FEED_WRITER --> REDIS_CACHE
    
    FEED_SVC --> FEED_RANKER
    FEED_RANKER --> ML_SERVICE
    FEED_SVC --> FEED_CACHE
    FEED_CACHE --> REDIS_CACHE
    
    CONNECTION_SVC --> NEO4J
    CONNECTION_SVC --> PG_MASTER
    
    USER_SVC --> PG_MASTER
    POST_SVC --> PG_MASTER
    INTERACTION_SVC --> PG_MASTER
    
    PG_MASTER --> PG_REPLICA
    
    USER_SVC --> REDIS_CACHE
    POST_SVC --> REDIS_COUNTER
    INTERACTION_SVC --> REDIS_COUNTER
    
    POST_SVC --> S3_PHOTOS
    POST_SVC --> S3_VIDEOS
    
    MEDIA_WORKER --> S3_PHOTOS
    MEDIA_WORKER --> S3_VIDEOS
    MEDIA_WORKER --> VISION_API
    
    SEARCH_SVC --> ES
    
    NOTIFICATION_SVC --> WS_GATEWAY
    NOTIFICATION_SVC --> PUSH
    NOTIFICATION_SVC --> EMAIL
    
    SUGGEST_WORKER --> NEO4J
    
    POST_SVC --> ANALYTICS
    FEED_SVC --> ANALYTICS
    ANALYTICS --> DATAWAREHOUSE
    
    CDN --> S3_PHOTOS
    CDN --> S3_VIDEOS
    
    style FEED_SVC fill:#90EE90
    style NEO4J fill:#4169E1
    style KAFKA fill:#ff9900
    style REDIS_CACHE fill:#dc382d
    style ML_SERVICE fill:#ffa500
```


***

## Step 7: Core Implementation (C++)

### 7.1 Connection Service (Friend Graph)

```cpp
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>

struct Connection {
    std::string user_id1;
    std::string user_id2;
    std::string status;  // pending, accepted, blocked
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point accepted_at;
};

struct User {
    std::string user_id;
    std::string username;
    std::string full_name;
    std::string profile_photo_url;
};

class ConnectionService {
private:
    DatabaseConnection db_;
    Neo4jClient neo4j_;
    RedisClient redis_;
    
    // In-memory graph for hot users (cache)
    std::unordered_map<std::string, std::unordered_set<std::string>> adjacency_list_;
    std::mutex graph_mtx_;
    
public:
    ConnectionService(DatabaseConnection& db, Neo4jClient& neo4j, RedisClient& redis)
        : db_(db), neo4j_(neo4j), redis_(redis) {}
    
    // Send friend request
    bool sendFriendRequest(const std::string& from_user, 
                          const std::string& to_user) {
        std::cout << "\n=== Sending Friend Request ===" << std::endl;
        std::cout << "From: " << from_user << " → To: " << to_user << std::endl;
        
        // Check if already connected
        if (areConnected(from_user, to_user)) {
            std::cerr << "Already connected" << std::endl;
            return false;
        }
        
        // Ensure consistent ordering (smaller ID first)
        std::string user1 = (from_user < to_user) ? from_user : to_user;
        std::string user2 = (from_user < to_user) ? to_user : from_user;
        
        // Insert into database
        std::string query = R"(
            INSERT INTO connections (user_id1, user_id2, status, created_at)
            VALUES (?, ?, 'pending', NOW())
            ON CONFLICT (user_id1, user_id2) 
            DO UPDATE SET status = 'pending'
        )";
        
        db_.execute(query, user1, user2);
        
        // Create notification
        createNotification(to_user, from_user, "friend_request");
        
        std::cout << "✓ Friend request sent" << std::endl;
        
        return true;
    }
    
    // Accept friend request
    bool acceptFriendRequest(const std::string& from_user,
                            const std::string& to_user) {
        std::cout << "\n=== Accepting Friend Request ===" << std::endl;
        
        std::string user1 = (from_user < to_user) ? from_user : to_user;
        std::string user2 = (from_user < to_user) ? to_user : from_user;
        
        // Update database
        std::string query = R"(
            UPDATE connections
            SET status = 'accepted', accepted_at = NOW()
            WHERE user_id1 = ? AND user_id2 = ?
            AND status = 'pending'
        )";
        
        int rows = db_.execute(query, user1, user2);
        
        if (rows == 0) {
            std::cerr << "No pending request found" << std::endl;
            return false;
        }
        
        // Add to graph database (for fast graph queries)
        neo4j_.execute(R"(
            MATCH (a:User {user_id: $user1}), (b:User {user_id: $user2})
            CREATE (a)-[:FRIEND {since: datetime()}]->(b)
        )", {{"user1", user1}, {"user2", user2}});
        
        // Update in-memory graph
        {
            std::lock_guard<std::mutex> lock(graph_mtx_);
            adjacency_list_[user1].insert(user2);
            adjacency_list_[user2].insert(user1);
        }
        
        // Invalidate friend list cache
        redis_.del("friends:" + user1);
        redis_.del("friends:" + user2);
        
        // Create notification
        createNotification(from_user, to_user, "friend_request_accepted");
        
        std::cout << "✓ Friend request accepted" << std::endl;
        std::cout << user1 << " and " << user2 << " are now friends" << std::endl;
        
        return true;
    }
    
    // Get friends list
    std::vector<User> getFriends(const std::string& user_id, int limit = 50) {
        // Try cache first
        auto cached = redis_.smembers("friends:" + user_id);
        if (!cached.empty()) {
            std::cout << "✓ Friends loaded from cache" << std::endl;
            return fetchUserDetails(cached);
        }
        
        // Query database
        std::string query = R"(
            SELECT 
                CASE 
                    WHEN user_id1 = ? THEN user_id2 
                    ELSE user_id1 
                END as friend_id
            FROM connections
            WHERE (user_id1 = ? OR user_id2 = ?)
            AND status = 'accepted'
            LIMIT ?
        )";
        
        auto results = db_.query(query, user_id, user_id, user_id, limit);
        
        std::vector<std::string> friend_ids;
        for (const auto& row : results) {
            friend_ids.push_back(row["friend_id"]);
            
            // Cache friend ID
            redis_.sadd("friends:" + user_id, row["friend_id"]);
        }
        
        redis_.expire("friends:" + user_id, 3600);  // 1 hour
        
        return fetchUserDetails(friend_ids);
    }
    
    // Find friends of friends (2nd degree connections)
    std::vector<User> findFriendsOfFriends(const std::string& user_id, int limit = 10) {
        std::cout << "\n=== Finding Friends of Friends ===" << std::endl;
        
        // Use Neo4j for efficient graph traversal
        std::string cypher = R"(
            MATCH (u:User {user_id: $user_id})-[:FRIEND]-(friend)-[:FRIEND]-(fof)
            WHERE fof.user_id <> $user_id 
            AND NOT (u)-[:FRIEND]-(fof)
            WITH fof, COUNT(friend) AS mutual_count
            ORDER BY mutual_count DESC
            LIMIT $limit
            RETURN fof.user_id AS user_id, mutual_count
        )";
        
        auto results = neo4j_.execute(cypher, {
            {"user_id", user_id},
            {"limit", limit}
        });
        
        std::vector<User> suggestions;
        for (const auto& row : results) {
            User user;
            user.user_id = row["user_id"];
            // Fetch full user details
            auto user_info = getUserInfo(user.user_id);
            if (user_info) {
                suggestions.push_back(*user_info);
            }
        }
        
        std::cout << "Found " << suggestions.size() << " suggestions" << std::endl;
        
        return suggestions;
    }
    
    // BFS to find shortest path between users
    std::vector<std::string> findShortestPath(const std::string& from_user,
                                               const std::string& to_user) {
        // Use Neo4j for graph algorithms
        std::string cypher = R"(
            MATCH path = shortestPath(
                (a:User {user_id: $from})-[:FRIEND*]-(b:User {user_id: $to})
            )
            RETURN [node IN nodes(path) | node.user_id] AS path
        )";
        
        auto result = neo4j_.execute(cypher, {
            {"from", from_user},
            {"to", to_user}
        });
        
        if (result.empty()) {
            return {};  // No path found
        }
        
        return result[^0]["path"];
    }
    
    // Calculate mutual friends count
    int getMutualFriendsCount(const std::string& user1, const std::string& user2) {
        // Use Neo4j for efficient calculation
        std::string cypher = R"(
            MATCH (a:User {user_id: $user1})-[:FRIEND]-(mutual)-[:FRIEND]-(b:User {user_id: $user2})
            RETURN COUNT(mutual) AS count
        )";
        
        auto result = neo4j_.execute(cypher, {
            {"user1", user1},
            {"user2", user2}
        });
        
        return std::stoi(result[^0]["count"]);
    }
    
private:
    bool areConnected(const std::string& user1, const std::string& user2) {
        // Check in-memory graph first
        {
            std::lock_guard<std::mutex> lock(graph_mtx_);
            auto it = adjacency_list_.find(user1);
            if (it != adjacency_list_.end()) {
                return it->second.count(user2) > 0;
            }
        }
        
        // Check database
        std::string u1 = (user1 < user2) ? user1 : user2;
        std::string u2 = (user1 < user2) ? user2 : user1;
        
        std::string query = R"(
            SELECT 1 FROM connections
            WHERE user_id1 = ? AND user_id2 = ?
            AND status = 'accepted'
        )";
        
        auto result = db_.query(query, u1, u2);
        return !result.empty();
    }
    
    std::vector<User> fetchUserDetails(const std::vector<std::string>& user_ids) {
        std::vector<User> users;
        
        for (const auto& id : user_ids) {
            auto user = getUserInfo(id);
            if (user) {
                users.push_back(*user);
            }
        }
        
        return users;
    }
    
    std::optional<User> getUserInfo(const std::string& user_id) {
        // Try cache
        auto cached = redis_.hgetall("user:" + user_id);
        if (!cached.empty()) {
            User user;
            user.user_id = user_id;
            user.username = cached["username"];
            user.full_name = cached["full_name"];
            user.profile_photo_url = cached["profile_photo_url"];
            return user;
        }
        
        // Query database
        std::string query = "SELECT * FROM users WHERE user_id = ?";
        auto result = db_.query(query, user_id);
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        User user;
        user.user_id = result[^0]["user_id"];
        user.username = result[^0]["username"];
        user.full_name = result[^0]["full_name"];
        user.profile_photo_url = result[^0]["profile_photo_url"];
        
        // Cache for future
        redis_.hset("user:" + user_id, {
            {"username", user.username},
            {"full_name", user.full_name},
            {"profile_photo_url", user.profile_photo_url}
        });
        redis_.expire("user:" + user_id, 3600);
        
        return user;
    }
    
    void createNotification(const std::string& user_id,
                           const std::string& actor_id,
                           const std::string& type) {
        // Queue notification for async processing
        json notif = {
            {"user_id", user_id},
            {"actor_id", actor_id},
            {"type", type},
            {"timestamp", std::time(nullptr)}
        };
        
        // Send to Kafka
        // kafka_.send("notifications", user_id, notif.dump());
    }
};
```


### 7.2 Feed Generation Service

```cpp
struct Post {
    std::string post_id;
    std::string user_id;
    std::string content;
    std::vector<std::string> media_urls;
    int like_count;
    int comment_count;
    int share_count;
    std::chrono::system_clock::time_point created_at;
    double ranking_score;
};

struct FeedItem {
    Post post;
    User author;
    double score;
};

class FeedService {
private:
    DatabaseConnection db_;
    RedisClient redis_;
    ConnectionService& connection_service_;
    MLRankingService& ranking_service_;
    
public:
    FeedService(DatabaseConnection& db,
               RedisClient& redis,
               ConnectionService& conn_svc,
               MLRankingService& ranking)
        : db_(db), redis_(redis), 
          connection_service_(conn_svc),
          ranking_service_(ranking) {}
    
    // Generate personalized feed
    std::vector<FeedItem> generateFeed(const std::string& user_id, 
                                       int limit = 50) {
        std::cout << "\n=== Generating Feed ===" << std::endl;
        std::cout << "User: " << user_id << std::endl;
        
        // Try cache first (hot path)
        auto cached_feed = loadCachedFeed(user_id, limit);
        if (!cached_feed.empty()) {
            std::cout << "✓ Loaded " << cached_feed.size() << " posts from cache" << std::endl;
            return cached_feed;
        }
        
        // Cold path: Generate feed
        std::cout << "Cache miss, generating fresh feed..." << std::endl;
        
        // Step 1: Get user's friends
        auto friends = connection_service_.getFriends(user_id, 5000);
        std::cout << "[1/4] Found " << friends.size() << " friends" << std::endl;
        
        // Step 2: Fetch recent posts from friends
        std::vector<Post> candidate_posts;
        
        for (const auto& friend_user : friends) {
            auto posts = getRecentPosts(friend_user.user_id, 10);  // Last 10 posts
            candidate_posts.insert(candidate_posts.end(), posts.begin(), posts.end());
        }
        
        std::cout << "[2/4] Collected " << candidate_posts.size() << " candidate posts" << std::endl;
        
        // Step 3: Rank posts using ML model
        std::vector<FeedItem> ranked_feed;
        
        for (auto& post : candidate_posts) {
            double score = ranking_service_.scorePost(user_id, post);
            
            FeedItem item;
            item.post = post;
            item.author = *connection_service_.getUserInfo(post.user_id);
            item.score = score;
            
            ranked_feed.push_back(item);
        }
        
        // Sort by score
        std::sort(ranked_feed.begin(), ranked_feed.end(),
                 [](const FeedItem& a, const FeedItem& b) {
                     return a.score > b.score;
                 });
        
        std::cout << "[3/4] Ranked posts" << std::endl;
        
        // Step 4: Cache result
        if (ranked_feed.size() > limit) {
            ranked_feed.resize(limit);
        }
        
        cacheFeed(user_id, ranked_feed);
        
        std::cout << "[4/4] Cached feed" << std::endl;
        std::cout << "✓ Generated feed with " << ranked_feed.size() << " posts" << std::endl;
        
        return ranked_feed;
    }
    
    // Fan-out post to followers (write path)
    void fanOutPost(const Post& post) {
        std::cout << "\n=== Fanning Out Post ===" << std::endl;
        std::cout << "Post: " << post.post_id << " by " << post.user_id << std::endl;
        
        // Get followers
        auto followers = connection_service_.getFriends(post.user_id);
        
        std::cout << "Fanning out to " << followers.size() << " followers" << std::endl;
        
        // Insert into each follower's feed (fan-out on write)
        for (const auto& follower : followers) {
            insertIntoFeed(follower.user_id, post);
        }
        
        // Invalidate cached feeds
        for (const auto& follower : followers) {
            redis_.del("feed:" + follower.user_id);
        }
        
        std::cout << "✓ Fan-out complete" << std::endl;
    }
    
private:
    std::vector<FeedItem> loadCachedFeed(const std::string& user_id, int limit) {
        // Load from Redis sorted set (by score)
        auto cached = redis_.zrevrange("feed:" + user_id, 0, limit - 1, true);
        
        if (cached.empty()) {
            return {};
        }
        
        std::vector<FeedItem> feed;
        
        for (const auto& [post_id, score_str] : cached) {
            auto post = getPost(post_id);
            if (post) {
                FeedItem item;
                item.post = *post;
                item.author = *connection_service_.getUserInfo(post->user_id);
                item.score = std::stod(score_str);
                feed.push_back(item);
            }
        }
        
        return feed;
    }
    
    void cacheFeed(const std::string& user_id, const std::vector<FeedItem>& feed) {
        // Store in Redis sorted set
        for (const auto& item : feed) {
            redis_.zadd("feed:" + user_id, item.score, item.post.post_id);
        }
        
        redis_.expire("feed:" + user_id, 600);  // 10 minutes
    }
    
    std::vector<Post> getRecentPosts(const std::string& user_id, int limit) {
        std::string query = R"(
            SELECT * FROM posts
            WHERE user_id = ?
            ORDER BY created_at DESC
            LIMIT ?
        )";
        
        auto results = db_.query(query, user_id, limit);
        
        std::vector<Post> posts;
        for (const auto& row : results) {
            Post post;
            post.post_id = row["post_id"];
            post.user_id = row["user_id"];
            post.content = row["content"];
            post.like_count = std::stoi(row["like_count"]);
            post.comment_count = std::stoi(row["comment_count"]);
            posts.push_back(post);
        }
        
        return posts;
    }
    
    std::optional<Post> getPost(const std::string& post_id) {
        // Try cache
        auto cached = redis_.hgetall("post:" + post_id);
        if (!cached.empty()) {
            Post post;
            post.post_id = post_id;
            post.content = cached["content"];
            post.like_count = std::stoi(cached["like_count"]);
            return post;
        }
        
        // Query database
        std::string query = "SELECT * FROM posts WHERE post_id = ?";
        auto result = db_.query(query, post_id);
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        Post post;
        post.post_id = result[^0]["post_id"];
        post.user_id = result[^0]["user_id"];
        post.content = result[^0]["content"];
        post.like_count = std::stoi(result[^0]["like_count"]);
        
        return post;
    }
    
    void insertIntoFeed(const std::string& user_id, const Post& post) {
        // Insert into Cassandra (time-series)
        std::string query = R"(
            INSERT INTO news_feed (user_id, post_id, ranking_score, created_at)
            VALUES (?, ?, ?, NOW())
        )";
        
        db_.execute(query, user_id, post.post_id, post.ranking_score);
    }
};
```


### 7.3 ML-Based Feed Ranking

```cpp
class MLRankingService {
private:
    struct UserFeatures {
        std::unordered_map<std::string, double> interactions;  // user_id -> affinity
        std::vector<std::string> interests;
        int avg_session_time;
    };
    
    struct PostFeatures {
        std::string post_type;  // text, photo, video
        int engagement_count;
        double virality_score;
        int age_hours;
    };
    
    // Simplified ML model (in production: TensorFlow/PyTorch)
    struct RankingModel {
        double affinity_weight = 0.4;
        double engagement_weight = 0.3;
        double recency_weight = 0.2;
        double type_weight = 0.1;
    };
    
    RankingModel model_;
    std::unordered_map<std::string, UserFeatures> user_cache_;
    
public:
    // Score a post for a specific user
    double scorePost(const std::string& user_id, const Post& post) {
        // Feature 1: User-Author Affinity
        double affinity = calculateAffinity(user_id, post.user_id);
        
        // Feature 2: Post Engagement (likes, comments, shares)
        double engagement = calculateEngagement(post);
        
        // Feature 3: Recency (time decay)
        double recency = calculateRecency(post.created_at);
        
        // Feature 4: Content Type
        double content_type_score = getContentTypeScore(post);
        
        // Weighted sum
        double score = (affinity * model_.affinity_weight) +
                      (engagement * model_.engagement_weight) +
                      (recency * model_.recency_weight) +
                      (content_type_score * model_.type_weight);
        
        return score;
    }
    
private:
    double calculateAffinity(const std::string& user_id, const std::string& author_id) {
        // Based on past interactions
        auto it = user_cache_.find(user_id);
        if (it != user_cache_.end()) {
            auto& interactions = it->second.interactions;
            auto author_it = interactions.find(author_id);
            if (author_it != interactions.end()) {
                return author_it->second;  // 0.0 - 1.0
            }
        }
        
        // Default: moderate affinity for friends
        return 0.5;
    }
    
    double calculateEngagement(const Post& post) {
        // Normalize engagement metrics
        int total_engagement = post.like_count + 
                              (post.comment_count * 3) +  // Comments worth more
                              (post.share_count * 5);      // Shares worth most
        
        // Sigmoid normalization
        return 1.0 / (1.0 + exp(-total_engagement / 100.0));
    }
    
    double calculateRecency(const std::chrono::system_clock::time_point& created_at) {
        auto now = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::hours>(now - created_at);
        
        // Exponential decay: e^(-λt)
        double lambda = 0.05;  // Decay constant
        double hours = age.count();
        
        return exp(-lambda * hours);
    }
    
    double getContentTypeScore(const Post& post) {
        // Video > Photo > Text
        if (!post.media_urls.empty()) {
            // Assuming first media determines type
            return 0.9;  // Photo/video
        }
        return 0.5;  // Text only
    }
};
```


### 7.4 Notification Service

```cpp
class NotificationService {
private:
    DatabaseConnection db_;
    RedisClient redis_;
    WebSocketManager& ws_manager_;
    PushNotificationService& push_service_;
    
public:
    NotificationService(DatabaseConnection& db,
                       RedisClient& redis,
                       WebSocketManager& ws_mgr,
                       PushNotificationService& push)
        : db_(db), redis_(redis), ws_manager_(ws_mgr), push_service_(push) {}
    
    // Create notification
    void createNotification(const std::string& user_id,
                           const std::string& actor_id,
                           const std::string& type,
                           const std::string& target_type,
                           const std::string& target_id) {
        std::cout << "\n=== Creating Notification ===" << std::endl;
        std::cout << "User: " << user_id << ", Type: " << type << std::endl;
        
        // Insert into database
        std::string query = R"(
            INSERT INTO notifications (user_id, actor_id, type, target_type, 
                                      target_id, message, created_at)
            VALUES (?, ?, ?, ?, ?, ?, NOW())
            RETURNING notification_id
        )";
        
        std::string message = generateMessage(type, actor_id);
        
        auto result = db_.query(query, user_id, actor_id, type, 
                               target_type, target_id, message);
        
        std::string notif_id = result[^0]["notification_id"];
        
        // Increment unread count
        redis_.incr("notif_count:" + user_id);
        
        // Send real-time notification
        if (ws_manager_.isUserOnline(user_id)) {
            // Send via WebSocket
            json notif_data = {
                {"notification_id", notif_id},
                {"type", type},
                {"actor_id", actor_id},
                {"message", message},
                {"timestamp", std::time(nullptr)}
            };
            
            ws_manager_.sendToUser(user_id, "notification", notif_data);
            
            std::cout << "✓ Sent via WebSocket (real-time)" << std::endl;
        } else {
            // Send push notification
            push_service_.sendPush(user_id, message);
            
            std::cout << "✓ Sent push notification (user offline)" << std::endl;
        }
    }
    
    // Get notifications for user
    std::vector<json> getNotifications(const std::string& user_id,
                                      bool unread_only = false,
                                      int limit = 20) {
        std::string query = R"(
            SELECT n.*, u.username, u.profile_photo_url
            FROM notifications n
            JOIN users u ON n.actor_id = u.user_id
            WHERE n.user_id = ?
        )";
        
        if (unread_only) {
            query += " AND n.read = FALSE";
        }
        
        query += " ORDER BY n.created_at DESC LIMIT ?";
        
        auto results = db_.query(query, user_id, limit);
        
        std::vector<json> notifications;
        for (const auto& row : results) {
            json notif = {
                {"notification_id", row["notification_id"]},
                {"type", row["type"]},
                {"actor", {
                    {"user_id", row["actor_id"]},
                    {"username", row["username"]},
                    {"profile_photo", row["profile_photo_url"]}
                }},
                {"message", row["message"]},
                {"read", row["read"] == "true"},
                {"created_at", row["created_at"]}
            };
            notifications.push_back(notif);
        }
        
        return notifications;
    }
    
    // Mark as read
    void markAsRead(const std::string& notification_id) {
        std::string query = "UPDATE notifications SET read = TRUE WHERE notification_id = ?";
        db_.execute(query, notification_id);
    }
    
private:
    std::string generateMessage(const std::string& type, const std::string& actor_id) {
        // In production: Fetch actor name and format message
        if (type == "like") {
            return "liked your post";
        } else if (type == "comment") {
            return "commented on your post";
        } else if (type == "friend_request") {
            return "sent you a friend request";
        }
        return "interacted with your content";
    }
};
```


### 7.5 Complete Social Network System

```cpp
class SocialNetwork {
private:
    DatabaseConnection db_;
    RedisClient redis_;
    Neo4jClient neo4j_;
    
    ConnectionService connection_service_;
    MLRankingService ranking_service_;
    FeedService feed_service_;
    NotificationService notification_service_;
    WebSocketManager ws_manager_;
    PushNotificationService push_service_;
    
public:
    SocialNetwork()
        : db_("postgresql://localhost/social_network"),
          redis_("redis://localhost:6379"),
          neo4j_("bolt://localhost:7687"),
          connection_service_(db_, neo4j_, redis_),
          ranking_service_(),
          ws_manager_(9090),
          push_service_(),
          notification_service_(db_, redis_, ws_manager_, push_service_),
          feed_service_(db_, redis_, connection_service_, ranking_service_) {}
    
    void start() {
        std::cout << "=== Starting Social Network ===" << std::endl;
        ws_manager_.start();
        std::cout << "System ready!" << std::endl;
    }
    
    void simulateUserJourney() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "    Social Network Simulation" << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        std::string alice = "user_alice";
        std::string bob = "user_bob";
        std::string charlie = "user_charlie";
        
        // Scenario 1: Alice sends friend request to Bob
        std::cout << "\n--- Scenario 1: Friend Request ---" << std::endl;
        connection_service_.sendFriendRequest(alice, bob);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Scenario 2: Bob accepts
        std::cout << "\n--- Scenario 2: Accept Request ---" << std::endl;
        connection_service_.acceptFriendRequest(alice, bob);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Scenario 3: Bob connects with Charlie
        connection_service_.sendFriendRequest(bob, charlie);
        connection_service_.acceptFriendRequest(bob, charlie);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Scenario 4: Find suggestions for Alice (should include Charlie)
        std::cout << "\n--- Scenario 4: Friend Suggestions ---" << std::endl;
        auto suggestions = connection_service_.findFriendsOfFriends(alice, 5);
        std::cout << "Suggestions for Alice:" << std::endl;
        for (const auto& user : suggestions) {
            std::cout << "  - " << user.user_id << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Scenario 5: Generate news feed for Alice
        std::cout << "\n--- Scenario 5: News Feed Generation ---" << std::endl;
        auto feed = feed_service_.generateFeed(alice, 10);
        std::cout << "Alice's feed has " << feed.size() << " posts" << std::endl;
        
        // Scenario 6: Show mutual friends
        std::cout << "\n--- Scenario 6: Mutual Friends ---" << std::endl;
        int mutual = connection_service_.getMutualFriendsCount(alice, charlie);
        std::cout << "Alice and Charlie have " << mutual << " mutual friend(s)" << std::endl;
    }
};

int main() {
    SocialNetwork network;
    network.start();
    
    network.simulateUserJourney();
    
    std::cout << "\nPress Enter to stop..." << std::endl;
    std::cin.get();
    
    return 0;
}
```


***

## Step 8: Bottlenecks \& Optimizations

### Bottleneck 1: Feed Generation Latency

**Problem:** Generating feed for user with 5,000 friends = query 5,000 users' posts

**Solution: Hybrid Fan-Out**

```cpp
class OptimizedFeedService {
public:
    std::vector<Post> generateFeed(const std::string& user_id) {
        // Pre-computed feed (normal users)
        auto precomputed = loadPrecomputedFeed(user_id);
        
        // Celebrity posts (on-demand)
        auto celebrity_posts = fetchCelebrityPosts(user_id);
        
        // Merge and sort
        return mergeAndRank(precomputed, celebrity_posts);
    }
    
private:
    bool isCelebrity(const std::string& user_id) {
        int follower_count = getFollowerCount(user_id);
        return follower_count > 10000;  // Threshold
    }
};

// Result: 
// Normal feed: <100ms (cached)
// Celebrity mixed: <300ms (partial cache)
```


### Bottleneck 2: Graph Query Performance

**Problem:** Friends-of-friends query on 519B edges

**Solution: GraphDB + Sharding**

```
Neo4j Causal Cluster:
- 3 core servers (write)
- 5 read replicas (read)

Sharding by user_id:
- Shard 1: users 0-999M
- Shard 2: users 1B-2B
- Shard 3: users 2B-3.07B

Query routing:
getUserFriends(alice) → Route to alice's shard

Result:
Single shard query: 10ms
Cross-shard query: 50ms (rare)
```


### Bottleneck 3: Notification Storm

**Problem:** Popular post gets 1M likes = 1M notifications

**Solution: Batch \& Aggregate**

```cpp
class BatchedNotificationService {
public:
    void createLikeNotification(const std::string& post_author,
                               const std::string& liker) {
        // Don't create individual notification
        // Instead, aggregate in Redis
        
        redis_.zadd("notif_likes:" + post_author, 
                   std::time(nullptr), liker);
        
        // Batch delivery every 1 minute
    }
    
    void deliverBatchedNotifications() {
        // "Alice and 42 others liked your post"
        auto recent_likes = redis_.zrange("notif_likes:" + post_author, 0, -1);
        
        if (recent_likes.size() > 0) {
            std::string message = recent_likes[^0] + " and " + 
                                 std::to_string(recent_likes.size() - 1) + 
                                 " others liked your post";
            
            sendNotification(post_author, message);
        }
    }
};

// Result: 1M individual notifs → 1 aggregated notif
```


***

## Summary: Key Decisions

| Aspect | Decision | Rationale |
| :-- | :-- | :-- |
| **Graph Storage** | Neo4j + PostgreSQL | Fast traversal + ACID |
| **Feed Generation** | Hybrid fan-out | Balance latency \& freshness |
| **Feed Ranking** | ML-based scoring | Personalization, engagement |
| **Notifications** | WebSocket + Push | Real-time delivery |
| **Search** | Elasticsearch | Full-text, typeahead |
| **Media** | S3 + CDN | Scalable, global delivery |
| **Cache** | Redis (6 TB) | Hot data, fast access |

**Performance Characteristics:**

```
Scale (Facebook 2025):
- Monthly active users: 3.07 billion [web:370]
- Daily active users: 2.11 billion [web:370]
- Daily posts: 500 million

Latency:
- Feed generation: <1 second (cached: 100ms)
- Post creation: <500ms
- Search: <200ms
- Friend suggestions: 50ms (Neo4j)

Database:
- Writes: 59K/sec
- Reads: 854K/sec (with cache)
- Graph edges: 519 billion

Storage:
- Users: 15 TB
- Posts: 912 TB/year
- Media: 228 PB/year
- Graph: 8.3 TB
- Total: ~230 PB/year

Cache:
- Redis: 6 TB
- Hit rate: 90%
```

**Facebook vs Competitors:**


| Feature | Facebook | LinkedIn | Twitter | Instagram |
| :-- | :-- | :-- | :-- | :-- |
| **MAU** | 3.07B [^1] | 1.2B [^2] | 550M | 2B |
| **Model** | Social graph | Professional | Interest | Photo-first |
| **Feed** | Algorithmic | Chronological + Algo | Reverse chrono | Algorithmic |
| **Connections** | Bidirectional | Bidirectional | Follow (1-way) | Follow |
| **Posts/day** | 500M | 50M | 500M | 100M |

This design handles **2.11 billion DAU** with **500M posts/day** using hybrid fan-out, Neo4j graph queries, ML-based ranking, and 90% cache hit rate! 📱👥

<span style="display:none">[^10][^11][^12][^13][^14][^15][^16][^17][^18][^19][^20][^4][^5][^6][^7][^8][^9]</span>

<div align="center">⁂</div>

[^1]: https://backlinko.com/facebook-users

[^2]: https://datareportal.com/essential-linkedin-stats

[^3]: https://www.linkedin.com/pulse/100-essential-linkedin-statistics-facts-2025-your-guide-dilawar-malik-pog9f

[^4]: https://www.statista.com/statistics/272014/global-social-networks-ranked-by-number-of-users/

[^5]: https://www.statista.com/statistics/268136/top-15-countries-based-on-number-of-facebook-users/

[^6]: https://cropink.com/fb-statistics

[^7]: https://buffer.com/resources/facebook-statistics/

[^8]: https://www.linkedin.com/posts/neilkpatel_heres-the-ideal-posting-frequency-for-each-activity-7178769232388628480-TVVk

[^9]: https://thesocialshepherd.com/blog/facebook-statistics

[^10]: https://buffer.com/resources/linkedin-statistics/

[^11]: https://blog.hootsuite.com/how-often-to-post-on-social-media/

[^12]: https://meetanshi.com/blog/facebook-statistics/

[^13]: https://www.sprinklr.com/blog/how-often-to-post-on-social-media/

[^14]: https://www.cognism.com/blog/linkedin-statistics

[^15]: https://www.practina.com/how-often-to-post-on-social-media/

[^16]: https://www.linkedin.com/pulse/70-most-important-linkedin-statistics-data-trends-2025-bint-e-jamil-o0kwf

[^17]: https://sproutsocial.com/insights/how-often-to-post-on-social-media/

[^18]: https://www.sprinklr.com/blog/linkedin-demographics/

[^19]: https://oksocial.co.uk/insight/how-many-social-media-posts

[^20]: https://www.statista.com/forecasts/1147197/linkedin-users-in-the-world

