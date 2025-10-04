# Video Streaming Platform (YouTube/Netflix) System Design

## Step 1: Requirements Clarification

### Functional Requirements

**Video Upload:**

- Upload videos (various formats: MP4, AVI, MOV, etc.)
- Support large files (up to 256 GB per video)
- Video metadata (title, description, tags, thumbnail)
- Privacy settings (public, private, unlisted)
- Video processing (transcoding, thumbnail generation)
- Upload progress tracking
- Resume interrupted uploads

**Video Playback:**

- Stream videos in multiple resolutions (240p, 360p, 480p, 720p, 1080p, 4K)
- Adaptive bitrate streaming (adjust quality based on bandwidth)
- Seek to any position in video
- Playback controls (play, pause, volume, speed)
- Subtitles/closed captions
- Picture-in-picture mode

**Video Discovery:**

- Search videos by keyword
- Browse by category
- Trending videos
- Recommended videos (personalized)
- Related videos
- Video thumbnail previews

**User Interactions:**

- Like/dislike videos
- Comments \& replies
- Subscribe to channels
- Playlists (create, edit, share)
- Watch history
- Watch later queue

**Channel Management:**

- Create channel
- Upload profile picture \& banner
- Channel description
- Video library management
- Analytics dashboard

**Out of Scope:**

- Live streaming (separate design)
- Monetization/ads
- Copyright detection
- Community posts


### Non-Functional Requirements

**Scale (Based on YouTube 2025 data):**

- Total videos: 5.1 billion videos[^1]
- Daily uploads: 1.1 million videos/day[^2]
- Upload rate: 360 hours/minute = 21,600 hours/hour[^1]
- Daily watch time: 1 billion hours[^3]
- Concurrent viewers: 100 million (estimate)
- Storage: ~1 exabyte (1000 PB)

**Performance:**

- Video start time: <2 seconds
- Seek latency: <1 second
- Upload processing time: <1 hour for 1-hour video
- Search latency: <500ms
- Recommendation generation: <1 second

**Reliability:**

- 99.99% uptime
- No video loss after upload
- Durable storage (99.999999999% - 11 nines)

**Availability:**

- Videos available globally
- CDN with <50ms latency to 95% of users

***

## Step 2: Video Streaming Theory \& Concepts

### 2.1 Video Encoding \& Transcoding

**Why Transcoding is Essential:**

```
Problem: User uploads 4K video at 100 Mbps
- User on mobile 4G: Only 5 Mbps available → Cannot play!
- User on old device: Doesn't support HEVC codec → Cannot decode!
- Storage: 100 Mbps × 3600 sec = 45 GB for 1 hour → Too expensive!

Solution: Transcode into multiple formats
```

**Video Codecs:**

```
H.264 (AVC) - Most Compatible:
- Supported by 99% of devices
- Good compression (1 hour 1080p = ~3 GB)
- Moderate CPU for encoding

H.265 (HEVC) - Better Compression:
- 50% smaller than H.264 for same quality
- 1 hour 1080p = ~1.5 GB
- Higher CPU for encoding/decoding
- Licensing issues

VP9 (Google) - Royalty-Free:
- Similar to H.265 compression
- Used by YouTube
- Open source

AV1 - Future:
- 30% better than H.265
- Royalty-free
- Very high CPU requirements
```

**Transcoding Pipeline:**

```
Input: raw_video.mp4 (4K, 100 Mbps, H.264)

Transcode to multiple resolutions & bitrates:
1. 2160p (4K) - 20 Mbps - H.264
2. 1080p (Full HD) - 8 Mbps - H.264
3. 720p (HD) - 5 Mbps - H.264
4. 480p (SD) - 2.5 Mbps - H.264
5. 360p - 1 Mbps - H.264
6. 240p - 0.5 Mbps - H.264

Also transcode to VP9 for supported browsers:
7. 1080p - 5 Mbps - VP9
8. 720p - 3 Mbps - VP9
...

Total: 12-15 versions of same video

FFmpeg command:
ffmpeg -i input.mp4 \
  -c:v libx264 -b:v 8M -s 1920x1080 output_1080p.mp4 \
  -c:v libx264 -b:v 5M -s 1280x720 output_720p.mp4 \
  ...
```


### 2.2 Adaptive Bitrate Streaming (ABR)

**Goal:** Deliver best quality without buffering

**How It Works:**

```
1. Video segmented into small chunks (2-10 seconds each)
2. Each chunk available in multiple qualities
3. Player measures bandwidth every few seconds
4. Player selects appropriate quality for next chunk

Example:
Video: 10 minutes = 600 seconds
Segment size: 6 seconds
Total segments: 100 segments

Segment 1:
  - 1080p (15 MB)
  - 720p (8 MB)
  - 480p (4 MB)
  - 360p (2 MB)

User starts watching:
- Bandwidth: 10 Mbps → Player chooses 1080p
- Bandwidth drops to 3 Mbps → Player switches to 720p (seamless!)
- Bandwidth increases to 8 Mbps → Player switches back to 1080p
```

**Streaming Protocols:**

**HLS (HTTP Live Streaming) - Apple:**

```
Master Playlist (playlist.m3u8):
#EXTM3U
#EXT-X-STREAM-INF:BANDWIDTH=8000000,RESOLUTION=1920x1080
1080p.m3u8
#EXT-X-STREAM-INF:BANDWIDTH=5000000,RESOLUTION=1280x720
720p.m3u8
#EXT-X-STREAM-INF:BANDWIDTH=2500000,RESOLUTION=854x480
480p.m3u8

Quality Playlist (1080p.m3u8):
#EXTM3U
#EXT-X-TARGETDURATION:6
#EXTINF:6.0,
segment_0_1080p.ts
#EXTINF:6.0,
segment_1_1080p.ts
#EXTINF:6.0,
segment_2_1080p.ts
...

Player:
1. Fetch master playlist
2. Measure bandwidth
3. Select appropriate quality playlist
4. Fetch segments sequentially
5. Adjust quality based on real-time bandwidth
```

**DASH (Dynamic Adaptive Streaming over HTTP) - Industry Standard:**

```
Manifest (MPD - Media Presentation Description):
<MPD>
  <Period>
    <AdaptationSet>
      <Representation bandwidth="8000000" width="1920" height="1080">
        <SegmentTemplate media="1080p_$Number$.m4s" />
      </Representation>
      <Representation bandwidth="5000000" width="1280" height="720">
        <SegmentTemplate media="720p_$Number$.m4s" />
      </Representation>
    </AdaptationSet>
  </Period>
</MPD>

Similar to HLS but:
- Codec agnostic (supports multiple)
- More flexible
- Better for live streaming
```


### 2.3 Content Delivery Network (CDN)

**Why CDN is Critical:**

```
Without CDN:
User in Tokyo → Origin Server in California → 150ms latency
Video buffering time: 10+ seconds
Origin server bandwidth: 100K concurrent users × 5 Mbps = 500 Gbps (impossible!)

With CDN:
User in Tokyo → CDN Edge (Tokyo) → 5ms latency
Video start time: <2 seconds
Origin bandwidth: Only cache misses (~5% of traffic)
```

**CDN Architecture for Video:**

```
                    Users
                      ↓
              ┌───────┴───────┐
         Edge Cache        Edge Cache
         (Tokyo)           (London)
              ↓                 ↓
         ┌────┴─────┐      ┌───┴────┐
    Mid-Tier       Mid-Tier      Mid-Tier
    (Asia)         (Europe)      (Americas)
         └─────┬──────┴───────┬─────┘
              Origin Servers
           (California, Virginia)

Cache Hierarchy:
- Edge: Serves 95% of requests, small cache (hot videos)
- Mid-Tier: Serves 4% of requests, larger cache
- Origin: Serves 1% of requests, all videos

Video segments cached at each level
Cache TTL: 7 days (videos don't change)
```


### 2.4 Video Segmentation

**Why Segment Videos?**

```
Problem: 2-hour movie as single file
- User seeks to 1:30:00 → Must download 1.5 hours of video!
- Buffering if network slow
- Cannot switch quality mid-stream

Solution: Segment into chunks
2-hour movie → 1200 segments (6 seconds each)
User seeks to 1:30:00 → Fetch segment 900 only (6 seconds = 10 MB)
Fast seeking!
```

**Segmentation Process:**

```
Input: movie.mp4 (2 hours, 1080p)

1. Split into 6-second segments:
   segment_0.mp4 (0-6 sec)
   segment_1.mp4 (6-12 sec)
   segment_2.mp4 (12-18 sec)
   ...
   segment_1199.mp4 (1:59:54 - 2:00:00)

2. Transcode each segment to multiple qualities:
   segment_0_1080p.mp4
   segment_0_720p.mp4
   segment_0_480p.mp4
   ...

3. Store in CDN/S3

Total files: 1200 segments × 6 qualities = 7,200 files per video
```


### 2.5 Thumbnail Generation

**Purpose:** Video preview images

```
Generate thumbnails at key moments:
- Automatic: Every 10 seconds
- Machine learning: Detect interesting frames (faces, action)

For 10-minute video:
60 thumbnails (1 per 10 seconds)

Process:
ffmpeg -i video.mp4 -vf "fps=1/10" -s 320x180 thumb_%03d.jpg

Output:
thumb_001.jpg (0:00)
thumb_002.jpg (0:10)
thumb_003.jpg (0:20)
...

Store in S3
Display on hover (like Netflix)
```


***

## Step 3: Capacity Estimation

```
Upload Volume:
Videos uploaded per day: 1.1 million [web:293]
Videos uploaded per second: 1.1M / 86,400 = 12.7 videos/sec
Hours uploaded per minute: 360 hours [web:291]
Hours per video (avg): 360 × 60 / 1.1M = 20 minutes avg

Video Size (Original):
Average video: 20 minutes at 1080p
Bitrate: 10 Mbps
Size: 10 Mbps × 20 min × 60 sec / 8 = 1.5 GB per video
Daily upload (raw): 1.1M × 1.5 GB = 1.65 PB/day

Transcoded Storage (per video):
Original: 1.5 GB
Transcoded versions:
  - 4K (20 Mbps, 10% of videos): 3 GB
  - 1080p (8 Mbps): 1.2 GB
  - 720p (5 Mbps): 750 MB
  - 480p (2.5 Mbps): 375 MB
  - 360p (1 Mbps): 150 MB
  - 240p (0.5 Mbps): 75 MB

Total per video: ~6 GB (after transcoding)

With VP9 codec (additional):
  - 1080p VP9 (5 Mbps): 750 MB
  - 720p VP9 (3 Mbps): 450 MB
  Total with VP9: ~7.2 GB per video

Daily storage (transcoded): 1.1M × 7.2 GB = 7.92 PB/day
With compression: 7.92 PB/day

Yearly storage: 7.92 PB × 365 = 2.9 EB/year (exabyte)
Total storage (5.1B videos): 5.1B × 7.2 GB = 36.7 EB

CDN Storage:
Hot videos (10% watched 90% of time): 3.67 EB
CDN cache per PoP: 3.67 EB / 200 PoPs = 18 PB per PoP
Realistic: 100 TB per PoP (most popular videos)

Streaming Traffic:
Daily watch time: 1 billion hours [web:290]
Average bitrate: 3 Mbps (mix of qualities)
Daily bandwidth: 1B hours × 3 Mbps / 8 = 375 PB/day
Per second: 375 PB / 86,400 = 4.3 TB/sec = 34.4 Tbps

With CDN:
Origin bandwidth (5% cache miss): 1.72 Tbps
Edge bandwidth: 32.7 Tbps (handled by CDN)

Transcoding:
Videos per day: 1.1 million
Transcoding time: 20 min video → 2x realtime = 40 min
With parallelization: 40 min / 10 parallel = 4 min
Transcoding workers: 1.1M / (24 × 60 / 4) = 3,056 workers
With GPU: 500 workers

Storage Breakdown:
Video files: 36.7 EB
Thumbnails: 5.1B videos × 60 thumbnails × 50 KB = 15.3 PB
Metadata: 5.1B videos × 10 KB = 51 TB
Total: ~36.7 EB

Database Writes:
New videos: 12.7/sec
Video views: 1B hours / 20 min avg = 3B views/day = 34.7K/sec
Interactions (like, comment): 10% of views = 3.5K/sec
Total writes: 38.2K writes/sec

Database Reads:
Video metadata (search, browse): 100K reads/sec
User data (profile, history): 50K reads/sec
Recommendation engine: 100K reads/sec
Total reads: 250K reads/sec

Search Index:
Videos to index: 5.1 billion
Index size: 5.1B × 5 KB (title, description, tags) = 25.5 TB
Elasticsearch cluster: 255 nodes (100 GB per node)

Recommendation System:
Users: 2 billion
User-video interactions: 100B (views, likes)
Collaborative filtering matrix: 2B × 5.1B = 10^19 (too sparse!)
Use matrix factorization: 2B × 100 dimensions = 800 GB
```


***

## Step 4: API Design

### Video Upload API

```json
POST /api/v1/videos/upload/init
Authorization: Bearer <token>
Content-Type: application/json

Request:
{
  "title": "My Awesome Video",
  "description": "Learn system design",
  "category": "education",
  "privacy": "public",
  "file_size": 1610612736,  // 1.5 GB in bytes
  "duration": 1200,  // 20 minutes in seconds
  "file_name": "system_design.mp4"
}

Response: 201 Created
{
  "video_id": "video_abc123",
  "upload_url": "https://upload-server-5.youtube.com/upload",
  "chunk_size": 5242880,  // 5 MB chunks
  "total_chunks": 307,
  "session_id": "upload_session_xyz"
}

// Upload video in chunks (resumable)
PUT /api/v1/videos/upload/chunk
Content-Type: application/octet-stream
X-Session-Id: upload_session_xyz
X-Chunk-Number: 1
X-Chunk-Hash: sha256_hash

Request Body: <binary chunk data>

Response: 200 OK
{
  "chunk_number": 1,
  "status": "received",
  "uploaded_bytes": 5242880,
  "total_bytes": 1610612736,
  "percentage": 0.32
}

// Finalize upload
POST /api/v1/videos/upload/finalize
Request:
{
  "video_id": "video_abc123",
  "session_id": "upload_session_xyz",
  "total_chunks": 307
}

Response: 200 OK
{
  "video_id": "video_abc123",
  "status": "processing",
  "estimated_time": 240  // seconds
}

// Check processing status
GET /api/v1/videos/{video_id}/status

Response: 200 OK
{
  "video_id": "video_abc123",
  "status": "ready",  // processing, ready, failed
  "available_qualities": ["240p", "360p", "480p", "720p", "1080p"],
  "thumbnail_url": "https://cdn.youtube.com/thumbs/abc123/default.jpg",
  "duration": 1200,
  "processed_at": "2025-10-04T16:09:00Z"
}
```


### Video Playback API

```json
GET /api/v1/videos/{video_id}/stream

Response: 200 OK
{
  "video_id": "video_abc123",
  "title": "My Awesome Video",
  "description": "Learn system design",
  "views": 10523,
  "likes": 432,
  "duration": 1200,
  
  "streams": {
    "hls": {
      "master_playlist": "https://cdn.youtube.com/hls/abc123/master.m3u8",
      "qualities": [
        {
          "resolution": "1080p",
          "bitrate": 8000000,
          "playlist": "https://cdn.youtube.com/hls/abc123/1080p.m3u8"
        },
        {
          "resolution": "720p",
          "bitrate": 5000000,
          "playlist": "https://cdn.youtube.com/hls/abc123/720p.m3u8"
        }
      ]
    },
    "dash": {
      "manifest": "https://cdn.youtube.com/dash/abc123/manifest.mpd"
    }
  },
  
  "thumbnails": {
    "default": "https://cdn.youtube.com/thumbs/abc123/default.jpg",
    "medium": "https://cdn.youtube.com/thumbs/abc123/mqdefault.jpg",
    "high": "https://cdn.youtube.com/thumbs/abc123/hqdefault.jpg",
    "sprites": "https://cdn.youtube.com/thumbs/abc123/storyboard.jpg"
  }
}

// Record view event
POST /api/v1/videos/{video_id}/view
Request:
{
  "watch_time": 120,  // seconds watched
  "quality": "720p",
  "device": "mobile"
}

Response: 204 No Content
```


### Search \& Discovery API

```json
GET /api/v1/search?q=system+design&page=1&limit=20

Response: 200 OK
{
  "query": "system design",
  "total_results": 15234,
  "results": [
    {
      "video_id": "video_abc123",
      "title": "System Design Interview Guide",
      "description": "Complete guide to system design...",
      "thumbnail": "https://cdn.youtube.com/thumbs/abc123/default.jpg",
      "duration": 1200,
      "views": 50000,
      "upload_date": "2025-09-15T10:00:00Z",
      "channel": {
        "id": "channel_xyz",
        "name": "Tech Channel",
        "subscribers": 100000
      }
    }
  ]
}

// Get recommendations
GET /api/v1/videos/{video_id}/recommendations?limit=10

Response: 200 OK
{
  "video_id": "video_abc123",
  "recommendations": [
    {
      "video_id": "video_def456",
      "title": "Advanced System Design",
      "score": 0.95,
      "reason": "based_on_watch_history"
    }
  ]
}

// Trending videos
GET /api/v1/trending?category=education&region=US

Response: 200 OK
{
  "category": "education",
  "region": "US",
  "trending": [
    {
      "video_id": "video_trending1",
      "title": "Trending Video",
      "views_24h": 500000,
      "growth_rate": 2.5
    }
  ]
}
```


***

## Step 5: Database Design

### PostgreSQL Schema (Metadata)

```sql
-- Users/Channels
CREATE TABLE users (
    user_id BIGSERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    display_name VARCHAR(100),
    profile_picture_url TEXT,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    subscriber_count INT DEFAULT 0,
    
    INDEX idx_username (username),
    INDEX idx_email (email)
);

-- Videos metadata
CREATE TABLE videos (
    video_id VARCHAR(50) PRIMARY KEY,  -- e.g., "abc123xyz"
    user_id BIGINT REFERENCES users(user_id),
    title VARCHAR(200) NOT NULL,
    description TEXT,
    duration INT NOT NULL,  -- seconds
    category VARCHAR(50),
    privacy VARCHAR(20) DEFAULT 'public',  -- public, private, unlisted
    status VARCHAR(20) DEFAULT 'processing',  -- processing, ready, failed
    upload_date TIMESTAMPTZ DEFAULT NOW(),
    
    -- Statistics
    view_count BIGINT DEFAULT 0,
    like_count INT DEFAULT 0,
    dislike_count INT DEFAULT 0,
    comment_count INT DEFAULT 0,
    
    -- Storage
    original_file_path TEXT,
    thumbnail_url TEXT,
    
    INDEX idx_user_videos (user_id, upload_date DESC),
    INDEX idx_status (status),
    INDEX idx_upload_date (upload_date DESC),
    FULLTEXT INDEX idx_search (title, description)
);

-- Video qualities/streams
CREATE TABLE video_streams (
    stream_id BIGSERIAL PRIMARY KEY,
    video_id VARCHAR(50) REFERENCES videos(video_id),
    quality VARCHAR(20),  -- 240p, 360p, 480p, 720p, 1080p, 4k
    codec VARCHAR(20),  -- h264, vp9, av1
    bitrate INT,  -- bits per second
    file_path TEXT,
    file_size BIGINT,  -- bytes
    hls_playlist_url TEXT,
    dash_manifest_url TEXT,
    
    UNIQUE(video_id, quality, codec),
    INDEX idx_video_streams (video_id)
);

-- View history (for recommendations)
CREATE TABLE view_history (
    view_id BIGSERIAL PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id),
    video_id VARCHAR(50) REFERENCES videos(video_id),
    watched_at TIMESTAMPTZ DEFAULT NOW(),
    watch_duration INT,  -- seconds watched
    completion_rate DECIMAL(3,2),  -- 0.75 = 75%
    quality VARCHAR(20),
    device_type VARCHAR(50),
    
    INDEX idx_user_history (user_id, watched_at DESC),
    INDEX idx_video_views (video_id, watched_at DESC)
) PARTITION BY RANGE (watched_at);

-- Partitions by month
CREATE TABLE view_history_2025_10 PARTITION OF view_history
    FOR VALUES FROM ('2025-10-01') TO ('2025-11-01');

-- User interactions
CREATE TABLE video_likes (
    user_id BIGINT REFERENCES users(user_id),
    video_id VARCHAR(50) REFERENCES videos(video_id),
    liked BOOLEAN,  -- true = like, false = dislike
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    PRIMARY KEY (user_id, video_id),
    INDEX idx_video_likes (video_id, created_at DESC)
);

-- Comments
CREATE TABLE comments (
    comment_id BIGSERIAL PRIMARY KEY,
    video_id VARCHAR(50) REFERENCES videos(video_id),
    user_id BIGINT REFERENCES users(user_id),
    parent_comment_id BIGINT REFERENCES comments(comment_id),  -- for replies
    content TEXT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    like_count INT DEFAULT 0,
    
    INDEX idx_video_comments (video_id, created_at DESC),
    INDEX idx_user_comments (user_id, created_at DESC)
);

-- Subscriptions
CREATE TABLE subscriptions (
    subscriber_id BIGINT REFERENCES users(user_id),
    channel_id BIGINT REFERENCES users(user_id),
    subscribed_at TIMESTAMPTZ DEFAULT NOW(),
    notifications BOOLEAN DEFAULT TRUE,
    
    PRIMARY KEY (subscriber_id, channel_id),
    INDEX idx_channel_subscribers (channel_id)
);

-- Playlists
CREATE TABLE playlists (
    playlist_id BIGSERIAL PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id),
    title VARCHAR(200) NOT NULL,
    description TEXT,
    privacy VARCHAR(20) DEFAULT 'public',
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    INDEX idx_user_playlists (user_id)
);

CREATE TABLE playlist_videos (
    playlist_id BIGINT REFERENCES playlists(playlist_id),
    video_id VARCHAR(50) REFERENCES videos(video_id),
    position INT NOT NULL,
    added_at TIMESTAMPTZ DEFAULT NOW(),
    
    PRIMARY KEY (playlist_id, video_id),
    UNIQUE(playlist_id, position)
);
```


### Cassandra Schema (Time-Series Data)

```sql
-- Video views (time-series analytics)
CREATE TABLE video_views_by_day (
    video_id TEXT,
    date DATE,
    hour INT,
    view_count COUNTER,
    total_watch_time COUNTER,  -- seconds
    
    PRIMARY KEY (video_id, date, hour)
) WITH CLUSTERING ORDER BY (date DESC, hour DESC);

-- User watch history (for quick lookups)
CREATE TABLE user_watch_history (
    user_id BIGINT,
    watched_at TIMESTAMP,
    video_id TEXT,
    watch_duration INT,
    
    PRIMARY KEY (user_id, watched_at)
) WITH CLUSTERING ORDER BY (watched_at DESC);
```


### Redis Cache

```
# Video metadata cache (hot videos)
HSET video:abc123 "title" "My Video" "views" "10523" "duration" "1200"
EXPIRE video:abc123 3600  # 1 hour

# Trending videos (sorted set by views)
ZADD trending:global 10523 "video_abc123"
ZADD trending:global 25000 "video_def456"

# View count (real-time increment)
INCR views:abc123
INCRBY watch_time:abc123 120  # 2 minutes watched

# Recommendation cache
SET recommendations:user_123 "[video_1, video_2, video_3]" EX 1800

# User session
HSET session:xyz "user_id" "123" "last_video" "abc123"
EXPIRE session:xyz 7200  # 2 hours
```


### Elasticsearch (Search Index)

```json
PUT /videos
{
  "mappings": {
    "properties": {
      "video_id": {"type": "keyword"},
      "title": {
        "type": "text",
        "analyzer": "standard",
        "fields": {
          "keyword": {"type": "keyword"}
        }
      },
      "description": {"type": "text"},
      "tags": {"type": "keyword"},
      "category": {"type": "keyword"},
      "upload_date": {"type": "date"},
      "view_count": {"type": "long"},
      "duration": {"type": "integer"},
      "channel": {
        "properties": {
          "id": {"type": "keyword"},
          "name": {"type": "text"}
        }
      }
    }
  }
}
```


***

## Step 6: High-Level Architecture

```mermaid
graph TB
    subgraph "Clients"
        WEB[Web Browser]
        MOBILE[Mobile App]
        TV[Smart TV]
    end
    
    subgraph "Load Balancer"
        LB[Load Balancer<br/>Nginx/CloudFront]
    end
    
    subgraph "API Gateway"
        GATEWAY[API Gateway<br/>Rate limiting<br/>Authentication]
    end
    
    subgraph "Upload Service"
        UPLOAD1[Upload Server 1<br/>Chunked upload<br/>Resumable]
        UPLOAD2[Upload Server 2]
    end
    
    subgraph "Video Processing Pipeline"
        QUEUE[Message Queue<br/>Kafka/RabbitMQ<br/>Processing jobs]
        
        TRANSCODE1[Transcoding Worker 1<br/>FFmpeg + GPU<br/>Multiple qualities]
        TRANSCODE2[Transcoding Worker 2]
        TRANSCODE3[Transcoding Worker N<br/>500 workers]
        
        THUMB[Thumbnail Generator<br/>Extract frames<br/>ML-based selection]
    end
    
    subgraph "Storage Layer"
        S3_RAW[S3 - Raw Videos<br/>Original uploads<br/>Temporary]
        
        S3_PROCESSED[S3 - Processed Videos<br/>Multiple qualities<br/>Segmented<br/>36.7 EB]
        
        S3_THUMBS[S3 - Thumbnails<br/>15.3 PB]
    end
    
    subgraph "CDN (CloudFront/Akamai)"
        CDN_EDGE[Edge Servers<br/>200+ PoPs<br/>100 TB cache each]
        CDN_MID[Mid-Tier Cache<br/>Larger cache]
    end
    
    subgraph "API Services"
        VIDEO_SVC[Video Service<br/>Metadata CRUD<br/>Status tracking]
        
        STREAM_SVC[Streaming Service<br/>HLS/DASH manifests<br/>Adaptive bitrate]
        
        SEARCH_SVC[Search Service<br/>Elasticsearch<br/>Video discovery]
        
        RECOMMEND_SVC[Recommendation<br/>ML models<br/>Personalization]
        
        INTERACT_SVC[Interaction Service<br/>Likes, comments<br/>Subscriptions]
    end
    
    subgraph "Databases"
        PG_MASTER[(PostgreSQL Master<br/>Video metadata<br/>Users, comments)]
        PG_REPLICA[(PostgreSQL Replicas<br/>Read scaling)]
        
        CASSANDRA[(Cassandra Cluster<br/>Time-series views<br/>Watch history<br/>1000 nodes)]
        
        REDIS[Redis Cluster<br/>Cache<br/>Real-time counters]
        
        ES[Elasticsearch<br/>Search index<br/>5.1B videos)]
    end
    
    subgraph "Analytics & ML"
        ANALYTICS[Analytics Service<br/>View counting<br/>Trending calculation]
        
        ML_TRAIN[ML Training Pipeline<br/>Recommendation models<br/>Batch processing]
        
        ML_SERVE[ML Inference<br/>Real-time recommendations]
    end
    
    subgraph "Monitoring"
        METRICS[Prometheus + Grafana<br/>System metrics]
        LOGS[ELK Stack<br/>Centralized logging]
    end
    
    WEB & MOBILE & TV -->|Upload| LB
    LB --> GATEWAY
    
    GATEWAY --> UPLOAD1 & UPLOAD2
    UPLOAD1 & UPLOAD2 -->|Store raw| S3_RAW
    UPLOAD1 & UPLOAD2 -->|Enqueue job| QUEUE
    
    QUEUE --> TRANSCODE1 & TRANSCODE2 & TRANSCODE3
    QUEUE --> THUMB
    
    TRANSCODE1 & TRANSCODE2 & TRANSCODE3 -->|Store processed| S3_PROCESSED
    THUMB -->|Store thumbs| S3_THUMBS
    
    TRANSCODE1 -->|Update status| VIDEO_SVC
    
    S3_PROCESSED --> CDN_MID --> CDN_EDGE
    
    WEB & MOBILE & TV -->|Stream video| CDN_EDGE
    CDN_EDGE -->|Cache miss| CDN_MID
    CDN_MID -->|Cache miss| S3_PROCESSED
    
    GATEWAY --> VIDEO_SVC
    GATEWAY --> STREAM_SVC
    GATEWAY --> SEARCH_SVC
    GATEWAY --> RECOMMEND_SVC
    GATEWAY --> INTERACT_SVC
    
    VIDEO_SVC --> PG_MASTER
    VIDEO_SVC --> PG_REPLICA
    VIDEO_SVC --> REDIS
    
    STREAM_SVC --> S3_PROCESSED
    STREAM_SVC --> REDIS
    
    SEARCH_SVC --> ES
    
    RECOMMEND_SVC --> ML_SERVE
    RECOMMEND_SVC --> CASSANDRA
    
    INTERACT_SVC --> PG_MASTER
    INTERACT_SVC --> REDIS
    
    VIDEO_SVC --> ANALYTICS
    ANALYTICS --> CASSANDRA
    
    CASSANDRA --> ML_TRAIN
    ML_TRAIN --> ML_SERVE
    
    UPLOAD1 & VIDEO_SVC & TRANSCODE1 --> METRICS
    UPLOAD1 & VIDEO_SVC --> LOGS
    
    style TRANSCODE1 fill:#90EE90
    style CDN_EDGE fill:#87CEEB
    style S3_PROCESSED fill:#ffa500
    style QUEUE fill:#ff9900
    style REDIS fill:#dc382d
```


***

## Step 7: Core Implementation (C++)

### 7.1 Video Upload Handler

```cpp
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <openssl/sha.h>

namespace fs = std::filesystem;

struct VideoUploadSession {
    std::string session_id;
    std::string video_id;
    std::string user_id;
    size_t total_size;
    size_t chunk_size;
    int total_chunks;
    std::vector<bool> received_chunks;
    fs::path temp_directory;
    std::chrono::system_clock::time_point created_at;
    
    VideoUploadSession(const std::string& vid_id, const std::string& uid,
                      size_t total_sz, size_t chunk_sz)
        : video_id(vid_id), user_id(uid), total_size(total_sz), chunk_size(chunk_sz) {
        
        session_id = generateSessionId();
        total_chunks = (total_size + chunk_size - 1) / chunk_size;
        received_chunks.resize(total_chunks, false);
        
        // Create temporary directory for chunks
        temp_directory = fs::temp_directory_path() / session_id;
        fs::create_directories(temp_directory);
        
        created_at = std::chrono::system_clock::now();
    }
    
    bool isComplete() const {
        for (bool received : received_chunks) {
            if (!received) return false;
        }
        return true;
    }
    
    double getProgress() const {
        int received_count = 0;
        for (bool received : received_chunks) {
            if (received) received_count++;
        }
        return (double)received_count / total_chunks;
    }
    
private:
    std::string generateSessionId() {
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        return "upload_" + std::to_string(now) + "_" + std::to_string(rand());
    }
};

class VideoUploadHandler {
private:
    std::unordered_map<std::string, std::shared_ptr<VideoUploadSession>> sessions_;
    std::mutex sessions_mtx_;
    
    fs::path upload_storage_path_;
    
public:
    VideoUploadHandler(const std::string& storage_path)
        : upload_storage_path_(storage_path) {
        fs::create_directories(upload_storage_path_);
    }
    
    // Initialize upload session
    std::shared_ptr<VideoUploadSession> initializeUpload(
        const std::string& video_id,
        const std::string& user_id,
        size_t file_size,
        size_t chunk_size = 5 * 1024 * 1024  // 5 MB chunks
    ) {
        std::lock_guard<std::mutex> lock(sessions_mtx_);
        
        auto session = std::make_shared<VideoUploadSession>(
            video_id, user_id, file_size, chunk_size
        );
        
        sessions_[session->session_id] = session;
        
        std::cout << "Initialized upload session: " << session->session_id
                 << " for video: " << video_id
                 << " (" << session->total_chunks << " chunks)" << std::endl;
        
        return session;
    }
    
    // Upload single chunk
    bool uploadChunk(const std::string& session_id,
                    int chunk_number,
                    const std::vector<uint8_t>& chunk_data,
                    const std::string& chunk_hash) {
        std::lock_guard<std::mutex> lock(sessions_mtx_);
        
        auto it = sessions_.find(session_id);
        if (it == sessions_.end()) {
            std::cerr << "Session not found: " << session_id << std::endl;
            return false;
        }
        
        auto session = it->second;
        
        if (chunk_number < 0 || chunk_number >= session->total_chunks) {
            std::cerr << "Invalid chunk number: " << chunk_number << std::endl;
            return false;
        }
        
        // Verify chunk hash
        if (!verifyChunkHash(chunk_data, chunk_hash)) {
            std::cerr << "Chunk hash mismatch for chunk " << chunk_number << std::endl;
            return false;
        }
        
        // Save chunk to temporary file
        fs::path chunk_path = session->temp_directory / 
                             ("chunk_" + std::to_string(chunk_number));
        
        std::ofstream chunk_file(chunk_path, std::ios::binary);
        chunk_file.write(reinterpret_cast<const char*>(chunk_data.data()), 
                        chunk_data.size());
        chunk_file.close();
        
        session->received_chunks[chunk_number] = true;
        
        std::cout << "Received chunk " << chunk_number << " of " 
                 << session->total_chunks 
                 << " (" << (session->getProgress() * 100) << "% complete)" << std::endl;
        
        return true;
    }
    
    // Finalize upload (merge chunks)
    bool finalizeUpload(const std::string& session_id) {
        std::lock_guard<std::mutex> lock(sessions_mtx_);
        
        auto it = sessions_.find(session_id);
        if (it == sessions_.end()) {
            return false;
        }
        
        auto session = it->second;
        
        if (!session->isComplete()) {
            std::cerr << "Upload incomplete: " << (session->getProgress() * 100) 
                     << "%" << std::endl;
            return false;
        }
        
        // Merge all chunks into final file
        fs::path final_path = upload_storage_path_ / 
                             (session->video_id + "_original.mp4");
        
        std::ofstream final_file(final_path, std::ios::binary);
        
        for (int i = 0; i < session->total_chunks; ++i) {
            fs::path chunk_path = session->temp_directory / 
                                 ("chunk_" + std::to_string(i));
            
            std::ifstream chunk_file(chunk_path, std::ios::binary);
            
            // Copy chunk to final file
            std::vector<uint8_t> buffer(session->chunk_size);
            while (chunk_file.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) ||
                   chunk_file.gcount() > 0) {
                final_file.write(reinterpret_cast<char*>(buffer.data()), chunk_file.gcount());
            }
            
            chunk_file.close();
        }
        
        final_file.close();
        
        std::cout << "Upload finalized: " << final_path << std::endl;
        
        // Cleanup temporary files
        fs::remove_all(session->temp_directory);
        
        // Remove session
        sessions_.erase(it);
        
        // Trigger video processing
        enqueueVideoProcessing(session->video_id, final_path.string());
        
        return true;
    }
    
private:
    bool verifyChunkHash(const std::vector<uint8_t>& data, const std::string& expected_hash) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(data.data(), data.size(), hash);
        
        // Convert to hex string
        char hex_hash[SHA256_DIGEST_LENGTH * 2 + 1];
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            sprintf(hex_hash + (i * 2), "%02x", hash[i]);
        }
        hex_hash[SHA256_DIGEST_LENGTH * 2] = 0;
        
        return std::string(hex_hash) == expected_hash;
    }
    
    void enqueueVideoProcessing(const std::string& video_id, const std::string& file_path) {
        // Send message to processing queue (Kafka/RabbitMQ)
        json processing_job = {
            {"video_id", video_id},
            {"file_path", file_path},
            {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
        };
        
        std::cout << "Enqueued processing job for video: " << video_id << std::endl;
        // kafka_producer.send("video-processing", processing_job.dump());
    }
};
```


### 7.2 Video Transcoding Worker

```cpp
#include <thread>
#include <queue>

struct TranscodingJob {
    std::string video_id;
    std::string input_file;
    std::vector<std::string> target_qualities;  // "1080p", "720p", "480p", etc.
    std::string output_directory;
};

struct TranscodingQuality {
    std::string name;         // "1080p"
    int width;                // 1920
    int height;               // 1080
    int bitrate;              // 8000000 (8 Mbps)
    std::string preset;       // "medium", "fast", "slow"
};

class VideoTranscoder {
private:
    std::map<std::string, TranscodingQuality> quality_presets_;
    
public:
    VideoTranscoder() {
        // Define quality presets
        quality_presets_["4k"] = {"4k", 3840, 2160, 20000000, "medium"};
        quality_presets_["1080p"] = {"1080p", 1920, 1080, 8000000, "medium"};
        quality_presets_["720p"] = {"720p", 1280, 720, 5000000, "medium"};
        quality_presets_["480p"] = {"480p", 854, 480, 2500000, "medium"};
        quality_presets_["360p"] = {"360p", 640, 360, 1000000, "fast"};
        quality_presets_["240p"] = {"240p", 426, 240, 500000, "fast"};
    }
    
    bool transcodeVideo(const TranscodingJob& job) {
        std::cout << "\n=== Transcoding Video: " << job.video_id << " ===" << std::endl;
        
        fs::create_directories(job.output_directory);
        
        // Transcode to each quality
        for (const auto& quality_name : job.target_qualities) {
            auto it = quality_presets_.find(quality_name);
            if (it == quality_presets_.end()) {
                std::cerr << "Unknown quality preset: " << quality_name << std::endl;
                continue;
            }
            
            const auto& quality = it->second;
            
            // Output file path
            std::string output_file = job.output_directory + "/" + 
                                     job.video_id + "_" + quality.name + ".mp4";
            
            // Build FFmpeg command
            std::string ffmpeg_cmd = buildFFmpegCommand(
                job.input_file,
                output_file,
                quality
            );
            
            std::cout << "Transcoding to " << quality.name << "..." << std::endl;
            
            // Execute FFmpeg (in production, use libav API for better control)
            int result = system(ffmpeg_cmd.c_str());
            
            if (result == 0) {
                std::cout << "✓ Transcoded to " << quality.name << std::endl;
                
                // Generate HLS segments
                generateHLSSegments(output_file, job.output_directory, quality.name);
            } else {
                std::cerr << "✗ Failed to transcode to " << quality.name << std::endl;
            }
        }
        
        // Generate master HLS playlist
        generateMasterPlaylist(job.output_directory, job.target_qualities);
        
        std::cout << "=== Transcoding Complete ===" << std::endl;
        
        return true;
    }
    
private:
    std::string buildFFmpegCommand(const std::string& input,
                                  const std::string& output,
                                  const TranscodingQuality& quality) {
        std::stringstream cmd;
        
        cmd << "ffmpeg -i " << input
            << " -c:v libx264"                          // H.264 codec
            << " -preset " << quality.preset             // Encoding speed/quality
            << " -b:v " << quality.bitrate               // Bitrate
            << " -maxrate " << (quality.bitrate * 1.5)   // Max bitrate
            << " -bufsize " << (quality.bitrate * 2)     // Buffer size
            << " -vf scale=" << quality.width << ":" << quality.height  // Resolution
            << " -c:a aac"                               // Audio codec
            << " -b:a 128k"                              // Audio bitrate
            << " -movflags +faststart"                   // Enable streaming
            << " -y " << output                          // Overwrite output
            << " 2>&1";                                  // Redirect stderr
        
        return cmd.str();
    }
    
    void generateHLSSegments(const std::string& input_video,
                           const std::string& output_dir,
                           const std::string& quality) {
        // Generate HLS segments (6-second chunks)
        std::string segment_dir = output_dir + "/hls/" + quality;
        fs::create_directories(segment_dir);
        
        std::string playlist_file = segment_dir + "/" + quality + ".m3u8";
        std::string segment_pattern = segment_dir + "/segment_%03d.ts";
        
        std::stringstream cmd;
        cmd << "ffmpeg -i " << input_video
            << " -codec copy"                            // Copy without re-encoding
            << " -start_number 0"
            << " -hls_time 6"                            // 6-second segments
            << " -hls_list_size 0"                       // Include all segments
            << " -f hls"
            << " -hls_segment_filename " << segment_pattern
            << " " << playlist_file
            << " 2>&1";
        
        std::cout << "Generating HLS segments for " << quality << "..." << std::endl;
        system(cmd.str().c_str());
    }
    
    void generateMasterPlaylist(const std::string& output_dir,
                               const std::vector<std::string>& qualities) {
        std::string master_playlist = output_dir + "/hls/master.m3u8";
        
        std::ofstream playlist(master_playlist);
        
        playlist << "#EXTM3U\n";
        playlist << "#EXT-X-VERSION:3\n\n";
        
        for (const auto& quality_name : qualities) {
            auto it = quality_presets_.find(quality_name);
            if (it == quality_presets_.end()) continue;
            
            const auto& quality = it->second;
            
            playlist << "#EXT-X-STREAM-INF:";
            playlist << "BANDWIDTH=" << quality.bitrate << ",";
            playlist << "RESOLUTION=" << quality.width << "x" << quality.height << "\n";
            playlist << quality_name << "/" << quality_name << ".m3u8\n\n";
        }
        
        playlist.close();
        
        std::cout << "Generated master playlist: " << master_playlist << std::endl;
    }
};
```


### 7.3 Thumbnail Generator

```cpp
class ThumbnailGenerator {
public:
    struct Thumbnail {
        int timestamp_sec;
        std::string file_path;
    };
    
    std::vector<Thumbnail> generateThumbnails(const std::string& video_file,
                                              const std::string& output_dir,
                                              int interval_sec = 10) {
        fs::create_directories(output_dir);
        
        // Get video duration
        int duration = getVideoDuration(video_file);
        
        std::vector<Thumbnail> thumbnails;
        
        // Generate thumbnail every N seconds
        for (int time = 0; time < duration; time += interval_sec) {
            std::string output_file = output_dir + "/thumb_" + 
                                     std::to_string(time) + ".jpg";
            
            // Extract frame at specific timestamp
            std::stringstream cmd;
            cmd << "ffmpeg -ss " << time                  // Seek to timestamp
                << " -i " << video_file
                << " -vframes 1"                          // Extract 1 frame
                << " -vf scale=320:180"                   // Thumbnail size
                << " -q:v 2"                              // Quality (2 = high)
                << " " << output_file
                << " 2>&1";
            
            system(cmd.str().c_str());
            
            thumbnails.push_back({time, output_file});
        }
        
        std::cout << "Generated " << thumbnails.size() << " thumbnails" << std::endl;
        
        // Generate sprite sheet (single image with all thumbnails)
        generateSpriteSheet(thumbnails, output_dir + "/sprites.jpg");
        
        return thumbnails;
    }
    
private:
    int getVideoDuration(const std::string& video_file) {
        // Use ffprobe to get duration
        std::string cmd = "ffprobe -v error -show_entries format=duration "
                         "-of default=noprint_wrappers=1:nokey=1 " + video_file;
        
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return 0;
        
        char buffer[^128];
        std::string result = "";
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
        pclose(pipe);
        
        return static_cast<int>(std::stof(result));
    }
    
    void generateSpriteSheet(const std::vector<Thumbnail>& thumbnails,
                           const std::string& output_file) {
        // Create sprite sheet for hover preview (like Netflix)
        // Use ImageMagick montage command
        
        std::stringstream cmd;
        cmd << "montage";
        
        for (const auto& thumb : thumbnails) {
            cmd << " " << thumb.file_path;
        }
        
        cmd << " -tile 10x"                               // 10 thumbnails per row
            << " -geometry +0+0"                          // No spacing
            << " " << output_file;
        
        system(cmd.str().c_str());
        
        std::cout << "Generated sprite sheet: " << output_file << std::endl;
    }
};
```


### 7.4 Complete Video Processing Pipeline

```cpp
class VideoProcessingPipeline {
private:
    VideoTranscoder transcoder_;
    ThumbnailGenerator thumbnail_generator_;
    
    std::queue<TranscodingJob> job_queue_;
    std::mutex queue_mtx_;
    std::condition_variable queue_cv_;
    
    std::vector<std::thread> worker_threads_;
    std::atomic<bool> running_{false};
    
    S3Client s3_client_;
    DatabaseConnection db_;
    
public:
    VideoProcessingPipeline(int num_workers = 4)
        : s3_client_("s3://video-bucket"),
          db_("postgresql://localhost/videos") {}
    
    void start() {
        running_ = true;
        
        // Start worker threads
        for (int i = 0; i < 4; ++i) {
            worker_threads_.emplace_back([this, i]() {
                processJobs(i);
            });
        }
        
        std::cout << "Video processing pipeline started with " 
                 << worker_threads_.size() << " workers" << std::endl;
    }
    
    void stop() {
        running_ = false;
        queue_cv_.notify_all();
        
        for (auto& thread : worker_threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    
    void enqueueJob(const TranscodingJob& job) {
        std::lock_guard<std::mutex> lock(queue_mtx_);
        job_queue_.push(job);
        queue_cv_.notify_one();
    }
    
private:
    void processJobs(int worker_id) {
        std::cout << "Worker " << worker_id << " started" << std::endl;
        
        while (running_) {
            TranscodingJob job;
            
            {
                std::unique_lock<std::mutex> lock(queue_mtx_);
                
                queue_cv_.wait(lock, [this]() {
                    return !job_queue_.empty() || !running_;
                });
                
                if (!running_ && job_queue_.empty()) break;
                
                if (job_queue_.empty()) continue;
                
                job = job_queue_.front();
                job_queue_.pop();
            }
            
            std::cout << "[Worker " << worker_id << "] Processing video: " 
                     << job.video_id << std::endl;
            
            // Update status to "processing"
            updateVideoStatus(job.video_id, "processing");
            
            // Step 1: Transcode video
            bool transcode_success = transcoder_.transcodeVideo(job);
            
            if (!transcode_success) {
                updateVideoStatus(job.video_id, "failed");
                continue;
            }
            
            // Step 2: Generate thumbnails
            auto thumbnails = thumbnail_generator_.generateThumbnails(
                job.input_file,
                job.output_directory + "/thumbnails"
            );
            
            // Step 3: Upload to S3
            uploadToS3(job);
            
            // Step 4: Update database
            updateVideoStatus(job.video_id, "ready");
            updateVideoStreams(job);
            updateVideoThumbnails(job, thumbnails);
            
            std::cout << "[Worker " << worker_id << "] Completed: " 
                     << job.video_id << std::endl;
        }
    }
    
    void updateVideoStatus(const std::string& video_id, const std::string& status) {
        std::string query = "UPDATE videos SET status = ? WHERE video_id = ?";
        db_.execute(query, status, video_id);
    }
    
    void updateVideoStreams(const TranscodingJob& job) {
        // Insert stream info into database
        for (const auto& quality : job.target_qualities) {
            std::string hls_url = "https://cdn.youtube.com/hls/" + 
                                 job.video_id + "/" + quality + "/" + quality + ".m3u8";
            
            std::string query = R"(
                INSERT INTO video_streams (video_id, quality, codec, hls_playlist_url)
                VALUES (?, ?, ?, ?)
            )";
            
            db_.execute(query, job.video_id, quality, "h264", hls_url);
        }
    }
    
    void updateVideoThumbnails(const TranscodingJob& job,
                              const std::vector<ThumbnailGenerator::Thumbnail>& thumbnails) {
        if (thumbnails.empty()) return;
        
        // Use first thumbnail as default
        std::string default_thumb = "https://cdn.youtube.com/thumbs/" + 
                                   job.video_id + "/default.jpg";
        
        std::string query = "UPDATE videos SET thumbnail_url = ? WHERE video_id = ?";
        db_.execute(query, default_thumb, job.video_id);
    }
    
    void uploadToS3(const TranscodingJob& job) {
        // Upload all transcoded files to S3
        std::cout << "Uploading to S3..." << std::endl;
        
        // In production: use AWS SDK
        // s3_client_.uploadDirectory(job.output_directory, job.video_id);
    }
};

// Example usage
int main() {
    std::cout << "=== Video Processing Pipeline ===" << std::endl;
    
    VideoProcessingPipeline pipeline(4);  // 4 workers
    pipeline.start();
    
    // Simulate job
    TranscodingJob job;
    job.video_id = "video_abc123";
    job.input_file = "/uploads/video_abc123_original.mp4";
    job.output_directory = "/processed/video_abc123";
    job.target_qualities = {"1080p", "720p", "480p", "360p", "240p"};
    
    pipeline.enqueueJob(job);
    
    // Wait for processing
    std::this_thread::sleep_for(std::chrono::seconds(300));  // 5 minutes
    
    pipeline.stop();
    
    return 0;
}
```


***

## Step 8: Advanced Features (Continued in next part due to length...)

### 8.1 Adaptive Bitrate Player

```cpp
class AdaptiveBitratePlayer {
private:
    struct Quality {
        std::string name;
        int bitrate;
        std::string playlist_url;
    };
    
    std::vector<Quality> available_qualities_;
    int current_quality_index_ = 0;
    
    // Bandwidth estimation
    double estimated_bandwidth_ = 5000000;  // Start at 5 Mbps
    std::deque<double> bandwidth_samples_;
    const size_t MAX_SAMPLES = 10;
    
public:
    void loadManifest(const std::string& master_playlist_url) {
        // Parse master playlist
        // Extract available qualities
        available_qualities_ = {
            {"1080p", 8000000, "1080p.m3u8"},
            {"720p", 5000000, "720p.m3u8"},
            {"480p", 2500000, "480p.m3u8"}
        };
        
        // Start with lowest quality
        current_quality_index_ = available_qualities_.size() - 1;
    }
    
    void downloadSegment(int segment_number) {
        auto start = std::chrono::steady_clock::now();
        
        // Download segment
        const auto& quality = available_qualities_[current_quality_index_];
        std::string segment_url = getSegmentUrl(quality, segment_number);
        
        size_t bytes_downloaded = downloadFile(segment_url);
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Calculate bandwidth
        double bandwidth = (bytes_downloaded * 8.0) / (duration.count() / 1000.0);  // bits per second
        
        updateBandwidthEstimate(bandwidth);
        
        // Decide quality for next segment
        selectQuality();
    }
    
private:
    void updateBandwidthEstimate(double bandwidth) {
        bandwidth_samples_.push_back(bandwidth);
        
        if (bandwidth_samples_.size() > MAX_SAMPLES) {
            bandwidth_samples_.pop_front();
        }
        
        // Exponential moving average
        double sum = 0;
        double weight = 1.0;
        double total_weight = 0;
        
        for (auto it = bandwidth_samples_.rbegin(); it != bandwidth_samples_.rend(); ++it) {
            sum += (*it) * weight;
            total_weight += weight;
            weight *= 0.8;  // Decay factor
        }
        
        estimated_bandwidth_ = sum / total_weight;
    }
    
    void selectQuality() {
        // Select quality based on bandwidth with safety margin
        double target_bandwidth = estimated_bandwidth_ * 0.85;  // 15% safety margin
        
        int best_quality = 0;
        for (size_t i = 0; i < available_qualities_.size(); ++i) {
            if (available_qualities_[i].bitrate <= target_bandwidth) {
                best_quality = i;
                break;
            }
        }
        
        if (best_quality != current_quality_index_) {
            std::cout << "Switching quality: " 
                     << available_qualities_[current_quality_index_].name
                     << " → " << available_qualities_[best_quality].name
                     << " (bandwidth: " << (estimated_bandwidth_ / 1000000) << " Mbps)"
                     << std::endl;
            
            current_quality_index_ = best_quality;
        }
    }
    
    std::string getSegmentUrl(const Quality& quality, int segment_number) {
        return "https://cdn.youtube.com/segments/" + quality.name + 
               "/segment_" + std::to_string(segment_number) + ".ts";
    }
    
    size_t downloadFile(const std::string& url) {
        // Simulate download
        return 1024 * 1024;  // 1 MB
    }
};
```


***

## Step 9: Bottlenecks \& Optimizations

### Bottleneck 1: Transcoding Cost

**Problem:** Transcoding 1.1M videos/day is CPU-intensive

**Solution: GPU Acceleration + Smart Transcoding**

```cpp
class OptimizedTranscoder {
public:
    // Use NVIDIA GPU for transcoding (10x faster)
    std::string buildGPUFFmpegCommand() {
        return "ffmpeg -hwaccel cuda -i input.mp4 "
               "-c:v h264_nvenc "  // NVIDIA hardware encoder
               "-preset fast "
               "-b:v 5M "
               "output_720p.mp4";
    }
    
    // Smart transcoding: Only transcode popular videos to all qualities
    void smartTranscode(const std::string& video_id, int predicted_views) {
        std::vector<std::string> qualities;
        
        if (predicted_views > 100000) {
            // High-view prediction: All qualities
            qualities = {"4k", "1080p", "720p", "480p", "360p", "240p"};
        } else if (predicted_views > 10000) {
            // Medium: Skip 4K
            qualities = {"1080p", "720p", "480p", "360p"};
        } else {
            // Low: Only HD and below
            qualities = {"720p", "480p", "360p"};
        }
        
        // Transcode on-demand for missing qualities if video becomes popular
    }
};

// Result: 50% cost reduction by avoiding unnecessary transcoding
```


### Bottleneck 2: Storage Costs

**Problem:** 36.7 EB storage costs millions per month

**Solution: Intelligent Tiering + Compression**

```
Hot tier (SSD): Recent uploads (30 days) → 7.92 PB × 30 = 238 PB
Warm tier (HDD): 31-365 days → 2.9 EB
Cold tier (Glacier): 1+ years, rarely watched → 34 EB

Compression:
- Use HEVC (H.265) for new videos: 50% smaller
- Aggressive compression for old, low-view videos
- Delete very low-view videos after 2 years (with user notification)

Cost:
Hot (SSD): 238 PB × $0.023/GB/month = $5.5M/month
Warm (HDD): 2.9 EB × $0.004/GB/month = $11.6M/month
Cold (Glacier): 34 EB × $0.001/GB/month = $34M/month
Total: $51.1M/month → With optimizations: $25M/month

YouTube's actual approach:
- Delete unused qualities (e.g., if no one watches 4K, delete it)
- Compress old videos more aggressively
- Use custom codec optimizations
```


### Bottleneck 3: CDN Bandwidth Costs

**Problem:** 34.4 Tbps bandwidth costs \$50M+/month

**Solution: P2P + Smart Caching**

```cpp
// P2P delivery (like BitTorrent)
class P2PVideoDelivery {
public:
    // Popular videos delivered via P2P
    // Viewers help deliver to other viewers
    // Reduces CDN load by 30-50%
    
    void enableP2P(const std::string& video_id, bool is_popular) {
        if (is_popular && video_age_days < 7) {
            // Recent popular video: Enable WebRTC P2P
            enable_webrtc_p2p = true;
            
            // Each viewer uploads to 2-3 other viewers
            // CDN only provides "seed"
        }
    }
};

// Result: 40% CDN cost reduction for popular videos
```


***

## Summary: Key Decisions

| Aspect | Decision | Rationale |
| :-- | :-- | :-- |
| **Upload** | Chunked + Resumable | Handle large files, network failures |
| **Encoding** | H.264 (primary), VP9 (secondary) | Compatibility vs compression |
| **Streaming** | HLS + DASH | Industry standard, adaptive |
| **Segmentation** | 6-second chunks | Balance seek speed \& overhead |
| **Storage** | S3 + Tiered | Durability, cost optimization |
| **CDN** | CloudFront/Akamai | Global reach, low latency |
| **Processing** | Async queue (Kafka) | Scalability, fault tolerance |

**Performance Characteristics:**

```
Scale (YouTube 2025):
- Total videos: 5.1 billion [web:291]
- Daily uploads: 1.1 million [web:293]
- Daily watch time: 1 billion hours [web:290]

Upload:
- Chunk size: 5 MB
- Max file size: 256 GB
- Upload bandwidth: 100 Mbps (user)
- Time to upload 1 GB: ~90 seconds

Processing:
- Transcoding: 2x realtime (10 min video → 20 min)
- With GPU: 0.5x realtime (10 min video → 5 min)
- Thumbnail generation: <30 seconds

Playback:
- Video start time: <2 seconds
- Seek latency: <1 second
- Quality switch: Seamless (1-2 seconds)
- Buffering: <1% of watch time

Storage:
- Total: 36.7 EB
- Per video: ~7.2 GB (all qualities)
- CDN cache: 100 TB per PoP

Cost (Estimated):
- Storage: $25M/month (with optimizations)
- CDN: $30M/month (with P2P)
- Transcoding: $5M/month (with GPU)
- Total: ~$60M/month for YouTube-scale
```

**YouTube vs Netflix:**


| Feature | YouTube | Netflix |
| :-- | :-- | :-- |
| **Content** | User-generated (UGC) | Professional (licensed) |
| **Upload** | Anyone can upload | Netflix employees only |
| **Scale** | 5.1B videos [^1] | ~15K titles |
| **Processing** | On-demand transcoding | Pre-transcoded library |
| **Recommendation** | Collaborative filtering | Heavy personalization + ML |
| **Cost Model** | Ad-supported (free) | Subscription |
| **Infrastructure** | Google (owned) | AWS + Open Connect CDN |

This design handles **5.1 billion videos** with **1.1 million daily uploads** and **1 billion hours watched daily** using chunked upload, GPU transcoding, HLS/DASH streaming, and global CDN! 🎬🚀
<span style="display:none">[^10][^11][^12][^13][^14][^15][^16][^17][^18][^19][^20][^4][^5][^6][^7][^8][^9]</span>

<div align="center">⁂</div>

[^1]: https://seo.ai/blog/how-many-videos-are-on-youtube

[^2]: https://mconverter.eu/blog/youtube-upload-statistics/

[^3]: https://www.globalmediainsight.com/blog/youtube-users-statistics/

[^4]: https://blog.youtube/press/

[^5]: https://analyzify.com/statsup/youtube

[^6]: https://about.netflix.com/news/what-we-watched-the-first-half-of-2025

[^7]: https://trtc.io/learning/the-architecture-of-modern-live

[^8]: https://www.socialvideoplaza.com/en/articles/youtube-upload-schedule

[^9]: https://99firms.com/research/netflix-statistics/

[^10]: https://www.0xkishan.com/blogs/designing-youtube

[^11]: https://thesocialshepherd.com/blog/youtube-statistics

[^12]: https://www.investing.com/academy/statistics/netflix-facts-and-statistics/

[^13]: https://hygraph.com/blog/video-streaming-architecture

[^14]: https://www.oberlo.com/blog/youtube-statistics

[^15]: https://www.statista.com/topics/842/netflix/

[^16]: https://www.mindbowser.com/how-to-build-a-scalable-video-streaming-app-architecture/

[^17]: https://moodive.com/youtube-stats-everything-you-need-to-know-in-2025/

[^18]: https://flixpatrol.com/top10/netflix/

[^19]: https://builder.aws.com/content/2wAcezE0LwAoIbaR1xA2eTEOYym/video-streaming-architectures

[^20]: https://explodingtopics.com/blog/youtube-creator-stats

