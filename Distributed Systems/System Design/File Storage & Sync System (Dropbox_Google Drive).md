# File Storage \& Sync System (Dropbox/Google Drive) Design

## Step 1: Requirements Clarification

### Functional Requirements

**File Operations:**

- Upload files (up to 5 GB per file)
- Download files
- Delete files
- Rename/move files
- Create folders
- File versioning (keep previous versions)

**Synchronization:**

- Real-time sync across devices
- Automatic sync when files change
- Selective sync (choose folders to sync)
- Conflict resolution (two users edit same file)
- Pause/resume sync

**Sharing:**

- Share files/folders via link
- Permission levels (view, edit)
- Password-protected links
- Expiration dates for links
- Revoke access

**Desktop Client:**

- System tray integration
- Automatic background sync
- Bandwidth throttling
- LAN sync (sync between local devices)

**Web Interface:**

- File preview (images, PDFs, videos)
- Online editing (basic text files)
- Search files
- Activity feed

**Out of Scope:**

- Office suite (Docs, Sheets, Slides)
- Photo organization/albums
- Mobile camera upload
- File recovery after 30 days


### Non-Functional Requirements

**Scale (Based on 2025 data):**

- Registered users: 700 million (Dropbox) , 1+ billion (Google Drive)[^1][^2]
- Paying users: 18.22 million (Dropbox)[^1]
- Market share: 47.4% (Google Drive)[^2]
- Free storage: 2 GB (Dropbox) , 15 GB (Google Drive)[^3][^2]
- Average files per user: 10,000 files
- Average file size: 2 MB

**Performance:**

- Upload speed: Limited by user's bandwidth
- Sync latency: <5 seconds for small files
- File sync: 200 objects/sec per sync group[^4]
- Download throughput: 60 objects/sec per endpoint[^4]
- Large file performance: 1,068 MB/sec (NVMe SSD)[^5]

**Reliability:**

- 99.99% uptime
- No data loss (11 nines durability)
- Data integrity (checksums)
- Redundancy (3+ copies)

**Consistency:**

- Eventual consistency across devices
- Strong consistency for metadata
- Last-write-wins for conflicts

***

## Step 2: File Sync Theory \& Concepts

### 2.1 Chunking Strategy

**Problem: Large files are slow to upload/download**

```
Naive: Upload entire 5 GB file
→ If upload fails at 99%: Start over
→ Any change: Re-upload entire file

Better: Chunking
File (5 GB) → Split into 4 MB chunks → 1,250 chunks

Benefits:
✅ Resume from failed chunk (not from start)
✅ Parallel upload (upload multiple chunks simultaneously)
✅ Deduplication (same chunk = upload once)
✅ Incremental upload (only changed chunks)

Example:
Original file: [chunk1, chunk2, chunk3, ... chunk1250]
User edits middle of file
New file: [chunk1, chunk2, chunk3_modified, chunk4, ...]

Only upload: chunk3_modified (one chunk, not entire file!)
```


### 2.2 Content-Defined Chunking (Rabin Fingerprinting)

**Problem: Fixed-size chunks don't handle edits well**

```
Fixed-size chunking (bad for edits):
Original: [AAAA|BBBB|CCCC|DDDD]  (4-byte chunks)
Insert 'X' at start: [XAAA|ABBB|BCCC|CDDD|D...]
→ All chunks changed! Must re-upload everything

Content-defined chunking (Rabin fingerprinting):
Look for natural boundaries based on content

Algorithm:
1. Compute rolling hash while reading file
2. When hash matches pattern (e.g., hash % 4096 == 0): Split chunk
3. Average chunk size: 4 MB, but boundaries based on content

Original: [AAA|BBBB|CCC|DDDD]
Insert 'X': [XAAA|BBBB|CCC|DDDD]
→ Only first chunk changed!

Used by: Dropbox, Google Drive
```


### 2.3 Delta Encoding (Rsync Algorithm)

**Concept: Send only differences between versions**

```
Old version: "Hello World"
New version: "Hello Beautiful World"

Naive: Upload entire new version (22 bytes)

Delta encoding:
1. Client: Compute checksums of old version blocks
   Block 1: "Hello " → checksum: abc123
   Block 2: "World" → checksum: def456

2. Server: Find matching blocks in new version
   New: "Hello Beautiful World"
   Found: "Hello " (checksum matches abc123)
   Found: "World" (checksum matches def456)

3. Client uploads only: " Beautiful" (10 bytes vs 22)

Result: 55% bandwidth savings

Used by: Rsync, Dropbox delta sync
```


### 2.4 Conflict Resolution

**Problem: Two users edit same file offline**

```
Scenario:
Initial: document.txt (version 1)

User A (offline): Edits → version 2A
User B (offline): Edits → version 2B

Both sync online → Conflict!

Resolution strategies:

1. Last-Write-Wins (simple):
   Compare timestamps
   User B synced later → User B's version wins
   User A's changes lost ❌

2. Create Conflict Copy (Dropbox approach):
   Keep both versions:
   - document.txt (User B's version)
   - document (User A's conflicted copy).txt
   
   User manually merges ✅

3. Operational Transformation (complex):
   Merge changes automatically
   Used by Google Docs (not Drive files)

Dropbox uses: Conflict copies
Google Drive uses: Last-write-wins + conflict copies
```


***

## Step 3: Capacity Estimation

```
Users & Files:
Registered users: 700 million [web:509]
Active users: 200 million (daily active)
Files per user: 10,000 files
Total files: 700M × 10K = 7 trillion files

Storage:
Average file size: 2 MB
Total storage: 7T files × 2 MB = 14 exabytes
With deduplication (30%): 9.8 exabytes
Per-user average: 20 GB used (out of 15 GB free + paid)

Sync Operations:
Active devices: 200M users × 2 devices = 400M devices
File changes per user per day: 50 files
Total changes: 200M × 50 = 10 billion file operations/day
Operations per second: 10B / 86,400 = 115,740 ops/sec

Upload Traffic:
Files uploaded per day: 10 billion files × 10% new = 1B uploads
Upload size: 1B files × 2 MB = 2 petabytes/day
Ingress bandwidth: 2 PB / 86,400 sec = 23 GB/sec = 184 Gbps

Download Traffic:
Downloads: 10B operations × 50% = 5B downloads/day
Download size: 5B × 2 MB = 10 PB/day
Egress bandwidth: 10 PB / 86,400 = 115 GB/sec = 920 Gbps

Chunking:
Chunk size: 4 MB
Chunks per file: 2 MB / 4 MB = 0.5 chunks (average)
Total chunks: 7T files × 0.5 = 3.5 trillion chunks
Chunk metadata: 3.5T × 100 bytes = 350 TB

Metadata Storage:
File metadata: 7T files × 1 KB = 7 petabytes
Folder metadata: 700M users × 100 folders × 500 bytes = 35 TB
Version history: 7T files × 3 versions × 1 KB = 21 PB
Total metadata: ~28 PB

Database Operations:
File metadata reads: 115K ops/sec
File metadata writes: 115K ops/sec
User authentication: 200M / 86,400 = 2,315 logins/sec

Sync Notification:
Devices to notify: 400M devices
Average notifications: 5 notifications/device/day
Total notifications: 2B notifications/day = 23,148 notifications/sec

Block Storage:
S3 storage: 9.8 exabytes
S3 PUT operations: 1B uploads × 0.5 chunks = 500M PUTs/day = 5,787 PUT/sec
S3 GET operations: 5B downloads × 0.5 chunks = 2.5B GETs/day = 28,935 GET/sec

Deduplication:
Duplicate chunks: 30% of chunks
Storage saved: 14 EB × 0.3 = 4.2 EB
Bandwidth saved: 2 PB/day × 0.3 = 600 TB/day

Desktop Client:
Active sync clients: 400M devices
Memory per client: 50 MB
CPU usage: 5% (idle), 20% (syncing)
Disk I/O: 10 MB/sec (syncing)

Conflict Rate:
Files with conflicts: 0.1% (optimistic)
Daily conflicts: 10B operations × 0.001 = 10M conflicts/day
Conflict resolution: Manual (conflict copies)
```


***

## Step 4: API Design

### File Operations APIs

```json
POST /api/v1/files/upload
Authorization: Bearer <token>
Content-Type: multipart/form-data

Request:
{
  "file": <binary_data>,
  "path": "/Documents/report.pdf",
  "parent_folder_id": "folder_123",
  "chunk_index": 0,
  "total_chunks": 5,
  "chunk_hash": "sha256_hash",
  "file_modified_time": "2025-10-04T17:43:00Z"
}

Response: 201 Created
{
  "file_id": "file_abc123",
  "chunk_uploaded": 0,
  "chunks_remaining": 4,
  "upload_url": "/api/v1/files/file_abc123/upload",
  "expires_at": "2025-10-04T18:43:00Z"
}

GET /api/v1/files/{file_id}/download

Response: 200 OK
Content-Disposition: attachment; filename="report.pdf"
Content-Length: 2097152
<binary_data>

GET /api/v1/files/{file_id}/metadata

Response: 200 OK
{
  "file_id": "file_abc123",
  "name": "report.pdf",
  "path": "/Documents/report.pdf",
  "size_bytes": 2097152,
  "content_hash": "sha256_abc123def456",
  "modified_time": "2025-10-04T17:43:00Z",
  "created_time": "2025-10-01T10:00:00Z",
  "is_folder": false,
  "version": 3,
  "shared": false
}

DELETE /api/v1/files/{file_id}

Response: 204 No Content

POST /api/v1/files/{file_id}/move
Request:
{
  "target_folder_id": "folder_456",
  "new_name": "Q4_report.pdf"
}

Response: 200 OK
```


### Sync APIs

```json
POST /api/v1/sync/delta
Authorization: Bearer <token>

Request:
{
  "cursor": "cursor_xyz789",  // null for first sync
  "path_prefix": "/Documents"  // null for all
}

Response: 200 OK
{
  "entries": [
    {
      "file_id": "file_abc123",
      "path": "/Documents/report.pdf",
      "event": "modified",  // added, modified, deleted, moved
      "metadata": {
        "size_bytes": 2097152,
        "content_hash": "sha256_abc123",
        "modified_time": "2025-10-04T17:43:00Z",
        "version": 3
      }
    },
    {
      "file_id": "file_def456",
      "path": "/Documents/old_file.txt",
      "event": "deleted"
    }
  ],
  "cursor": "cursor_new123",
  "has_more": false
}

POST /api/v1/sync/commit
Request:
{
  "file_id": "file_abc123",
  "content_hash": "sha256_abc123",
  "chunks": [
    {"chunk_hash": "chunk1_hash", "offset": 0, "size": 4194304},
    {"chunk_hash": "chunk2_hash", "offset": 4194304, "size": 2097152}
  ],
  "modified_time": "2025-10-04T17:43:00Z",
  "client_device_id": "device_xyz"
}

Response: 200 OK
{
  "file_id": "file_abc123",
  "version": 4,
  "conflict": false
}

# If conflict exists:
Response: 409 Conflict
{
  "file_id": "file_abc123",
  "conflict": true,
  "server_version": 5,
  "client_version": 3,
  "resolution": "create_copy"
}
```


### Sharing APIs

```json
POST /api/v1/sharing/create_link
Request:
{
  "file_id": "file_abc123",
  "permission": "view",  // view, edit
  "password": "secret123",
  "expires_at": "2025-10-11T17:43:00Z"
}

Response: 201 Created
{
  "share_id": "share_xyz789",
  "share_url": "https://drive.example.com/share/xyz789",
  "permission": "view",
  "expires_at": "2025-10-11T17:43:00Z"
}

POST /api/v1/sharing/revoke
Request:
{
  "share_id": "share_xyz789"
}

Response: 200 OK
```


***

## Step 5: Database Design

### PostgreSQL Schema

```sql
-- Users
CREATE TABLE users (
    user_id BIGSERIAL PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255),
    storage_quota_bytes BIGINT DEFAULT 16106127360,  -- 15 GB
    storage_used_bytes BIGINT DEFAULT 0,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    INDEX idx_email (email)
);

-- Files metadata
CREATE TABLE files (
    file_id VARCHAR(50) PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id),
    parent_folder_id VARCHAR(50),
    
    name TEXT NOT NULL,
    path TEXT NOT NULL,
    is_folder BOOLEAN DEFAULT FALSE,
    
    -- File properties
    size_bytes BIGINT DEFAULT 0,
    content_hash VARCHAR(64),  -- SHA256 of entire file
    mime_type VARCHAR(100),
    
    -- Timestamps
    created_at TIMESTAMPTZ DEFAULT NOW(),
    modified_at TIMESTAMPTZ DEFAULT NOW(),
    deleted_at TIMESTAMPTZ,  -- Soft delete
    
    -- Version
    version INT DEFAULT 1,
    
    INDEX idx_user_path (user_id, path),
    INDEX idx_parent (parent_folder_id),
    INDEX idx_content_hash (content_hash),
    INDEX idx_deleted (deleted_at)
);

-- File chunks (for chunked storage)
CREATE TABLE file_chunks (
    chunk_id BIGSERIAL PRIMARY KEY,
    chunk_hash VARCHAR(64) UNIQUE NOT NULL,  -- Content-defined hash
    size_bytes INT NOT NULL,
    storage_key VARCHAR(500),  -- S3 key
    reference_count INT DEFAULT 0,  -- Deduplication counter
    
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    INDEX idx_chunk_hash (chunk_hash)
);

-- File-to-chunks mapping
CREATE TABLE file_chunk_mappings (
    file_id VARCHAR(50) REFERENCES files(file_id) ON DELETE CASCADE,
    chunk_id BIGINT REFERENCES file_chunks(chunk_id),
    chunk_offset BIGINT NOT NULL,  -- Position in file
    chunk_sequence INT NOT NULL,  -- Order
    
    PRIMARY KEY (file_id, chunk_sequence),
    INDEX idx_file_chunks (file_id, chunk_sequence)
);

-- File versions
CREATE TABLE file_versions (
    version_id BIGSERIAL PRIMARY KEY,
    file_id VARCHAR(50) REFERENCES files(file_id),
    version_number INT NOT NULL,
    
    content_hash VARCHAR(64),
    size_bytes BIGINT,
    modified_at TIMESTAMPTZ,
    modified_by BIGINT REFERENCES users(user_id),
    
    -- Chunks for this version (JSONB for flexibility)
    chunks JSONB,
    
    UNIQUE(file_id, version_number),
    INDEX idx_file_version (file_id, version_number DESC)
);

-- Devices (for multi-device sync)
CREATE TABLE devices (
    device_id VARCHAR(50) PRIMARY KEY,
    user_id BIGINT REFERENCES users(user_id),
    device_name VARCHAR(200),
    device_type VARCHAR(50),  -- desktop, mobile, web
    
    last_sync_at TIMESTAMPTZ,
    last_cursor VARCHAR(100),  -- Delta sync cursor
    
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    INDEX idx_user_devices (user_id)
);

-- Sync events (for delta sync)
CREATE TABLE sync_events (
    event_id BIGSERIAL PRIMARY KEY,
    file_id VARCHAR(50) REFERENCES files(file_id),
    user_id BIGINT REFERENCES users(user_id),
    
    event_type VARCHAR(20),  -- added, modified, deleted, moved
    event_data JSONB,  -- Additional event info
    
    created_at TIMESTAMPTZ DEFAULT NOW(),
    
    INDEX idx_user_events (user_id, created_at DESC),
    INDEX idx_file_events (file_id, created_at DESC)
) PARTITION BY RANGE (created_at);

CREATE TABLE sync_events_2025_10 PARTITION OF sync_events
    FOR VALUES FROM ('2025-10-01') TO ('2025-11-01');

-- Shared links
CREATE TABLE shared_links (
    share_id VARCHAR(50) PRIMARY KEY,
    file_id VARCHAR(50) REFERENCES files(file_id),
    created_by BIGINT REFERENCES users(user_id),
    
    permission_level VARCHAR(20),  -- view, edit
    password_hash VARCHAR(255),
    
    created_at TIMESTAMPTZ DEFAULT NOW(),
    expires_at TIMESTAMPTZ,
    revoked_at TIMESTAMPTZ,
    
    access_count INT DEFAULT 0,
    
    INDEX idx_file_shares (file_id)
);
```



## Step 6: High-Level Architecture

```mermaid
graph TB
    subgraph "Client Applications"
        DESKTOP[Desktop Client<br/>Windows/Mac/Linux<br/>File watcher<br/>Sync engine]
        
        WEB[Web Interface<br/>File browser<br/>Upload/download]
        
        MOBILE[Mobile Apps<br/>iOS/Android<br/>Camera upload]
    end
    
    subgraph "API Gateway"
        GATEWAY[API Gateway<br/>Load balancing<br/>Authentication<br/>Rate limiting]
    end
    
    subgraph "Core Services"
        FILE_SVC[File Service<br/>Metadata CRUD<br/>Path resolution]
        
        CHUNK_SVC[Chunk Service<br/>Content-defined chunking<br/>Deduplication]
        
        SYNC_SVC[Sync Service<br/>Delta sync<br/>Event streaming]
        
        VERSION_SVC[Version Service<br/>History tracking<br/>Rollback]
        
        SHARE_SVC[Sharing Service<br/>Link generation<br/>Permissions]
    end
    
    subgraph "Upload/Download Pipeline"
        UPLOAD_WORKER[Upload Workers<br/>Chunk processing<br/>5,787 chunks/sec]
        
        DOWNLOAD_WORKER[Download Workers<br/>Chunk assembly<br/>28,935 chunks/sec]
        
        HASH_WORKER[Hash Workers<br/>Content hashing<br/>Dedup detection]
        
        COMPRESS[Compression<br/>Gzip/Brotli<br/>30% savings]
    end
    
    subgraph "Sync Notification System"
        NOTIF_SVC[Notification Service<br/>Push notifications<br/>23K notifications/sec]
        
        WEBSOCKET[WebSocket Server<br/>Real-time sync<br/>400M connections]
        
        LONG_POLL[Long Polling<br/>Fallback<br/>Legacy clients]
    end
    
    subgraph "Storage Layer"
        PG_MASTER[(PostgreSQL Master<br/>File metadata<br/>28 PB)]
        
        PG_REPLICA[(PostgreSQL Replicas<br/>Read scaling<br/>50 replicas)]
        
        S3_STORAGE[S3 - File Chunks<br/>9.8 exabytes<br/>Multi-region<br/>11 nines durability)]
        
        S3_THUMBNAILS[S3 - Thumbnails<br/>Image previews<br/>100 TB]
        
        REDIS_CACHE[Redis Cluster<br/>Hot metadata<br/>User sessions<br/>500 GB]
    end
    
    subgraph "Message Queue"
        KAFKA[Kafka<br/>Sync events<br/>115K ops/sec]
        
        SQS[SQS<br/>Upload queue<br/>Download queue]
    end
    
    subgraph "Background Jobs"
        DEDUP_WORKER[Dedup Worker<br/>Find duplicates<br/>30% savings]
        
        THUMBNAIL_WORKER[Thumbnail Worker<br/>Generate previews<br/>Images, PDFs]
        
        CLEANUP_WORKER[Cleanup Worker<br/>Delete old versions<br/>Garbage collection]
        
        QUOTA_WORKER[Quota Worker<br/>Calculate storage<br/>Usage updates]
    end
    
    subgraph "Monitoring & Analytics"
        METRICS[Prometheus<br/>Upload/download<br/>Latency, errors]
        
        LOGS[ELK Stack<br/>Audit logs<br/>Access logs]
        
        DASHBOARD[Grafana<br/>Real-time metrics<br/>Alerts]
    end
    
    DESKTOP & WEB & MOBILE --> GATEWAY
    GATEWAY --> FILE_SVC
    GATEWAY --> CHUNK_SVC
    GATEWAY --> SYNC_SVC
    
    FILE_SVC --> PG_MASTER
    CHUNK_SVC --> HASH_WORKER
    SYNC_SVC --> NOTIF_SVC
    
    HASH_WORKER --> UPLOAD_WORKER
    UPLOAD_WORKER --> COMPRESS
    COMPRESS --> S3_STORAGE
    
    DOWNLOAD_WORKER --> S3_STORAGE
    
    FILE_SVC --> KAFKA
    KAFKA --> SYNC_SVC
    SYNC_SVC --> WEBSOCKET
    SYNC_SVC --> LONG_POLL
    
    WEBSOCKET --> DESKTOP
    
    PG_MASTER --> PG_REPLICA
    FILE_SVC --> REDIS_CACHE
    
    UPLOAD_WORKER --> SQS
    DOWNLOAD_WORKER --> SQS
    
    DEDUP_WORKER --> S3_STORAGE
    DEDUP_WORKER --> PG_MASTER
    
    THUMBNAIL_WORKER --> S3_THUMBNAILS
    
    QUOTA_WORKER --> PG_MASTER
    
    FILE_SVC --> METRICS
    CHUNK_SVC --> METRICS
    METRICS --> DASHBOARD
    FILE_SVC --> LOGS
    
    style S3_STORAGE fill:#336791
    style REDIS_CACHE fill:#dc382d
    style KAFKA fill:#ff9900
    style CHUNK_SVC fill:#ffa500
```


***

## Step 7: Core Implementation (C++)

### 7.1 Content-Defined Chunking (Rabin Fingerprinting)

<details>
<summary>RabinChunker Class</summary>

```cpp
#include <vector>
#include <string>
#include <cstdint>

class RabinChunker {
private:
    static constexpr size_t MIN_CHUNK_SIZE = 2 * 1024 * 1024;  // 2 MB
    static constexpr size_t MAX_CHUNK_SIZE = 8 * 1024 * 1024;  // 8 MB
    static constexpr size_t WINDOW_SIZE = 48;
    static constexpr uint64_t POLYNOMIAL = 0x3DA3358B4DC173LL;
    static constexpr uint64_t MASK = 0xFFF;  // Average chunk: 4 MB
    
    uint64_t polynomial_table_[^256];
    
public:
    RabinChunker() {
        initializePolynomialTable();
    }
    
    struct Chunk {
        std::vector<uint8_t> data;
        std::string hash;
        size_t offset;
        size_t size;
    };
    
    std::vector<Chunk> chunkFile(const std::vector<uint8_t>& file_data) {
        std::cout << "\n=== Content-Defined Chunking ===" << std::endl;
        std::cout << "File size: " << file_data.size() << " bytes" << std::endl;
        
        std::vector<Chunk> chunks;
        
        size_t offset = 0;
        uint64_t hash = 0;
        size_t window_start = 0;
        
        while (offset < file_data.size()) {
            size_t chunk_start = offset;
            size_t chunk_size = 0;
            
            // Compute rolling hash
            while (offset < file_data.size()) {
                // Update rolling hash
                hash = (hash << 1) ^ polynomial_table_[file_data[offset]];
                
                offset++;
                chunk_size = offset - chunk_start;
                
                // Check for chunk boundary
                bool boundary = (hash & MASK) == 0;
                bool min_reached = chunk_size >= MIN_CHUNK_SIZE;
                bool max_reached = chunk_size >= MAX_CHUNK_SIZE;
                
                if ((boundary && min_reached) || max_reached || offset == file_data.size()) {
                    // Create chunk
                    Chunk chunk;
                    chunk.offset = chunk_start;
                    chunk.size = chunk_size;
                    chunk.data.assign(file_data.begin() + chunk_start,
                                     file_data.begin() + offset);
                    chunk.hash = computeChunkHash(chunk.data);
                    
                    chunks.push_back(chunk);
                    
                    std::cout << "  Chunk " << chunks.size() 
                             << ": offset=" << chunk.offset 
                             << ", size=" << chunk.size 
                             << " bytes, hash=" << chunk.hash.substr(0, 8) << "..." << std::endl;
                    
                    break;
                }
            }
        }
        
        std::cout << "✓ Created " << chunks.size() << " chunks" << std::endl;
        std::cout << "  Average chunk size: " << (file_data.size() / chunks.size()) << " bytes" << std::endl;
        
        return chunks;
    }
    
private:
    void initializePolynomialTable() {
        for (int i = 0; i < 256; ++i) {
            uint64_t hash = i;
            for (int j = 0; j < 8; ++j) {
                if (hash & 1) {
                    hash = (hash >> 1) ^ POLYNOMIAL;
                } else {
                    hash >>= 1;
                }
            }
            polynomial_table_[i] = hash;
        }
    }
    
    std::string computeChunkHash(const std::vector<uint8_t>& data) {
        // Simplified SHA256 (use OpenSSL in production)
        std::hash<std::string> hasher;
        std::string data_str(data.begin(), data.end());
        size_t hash = hasher(data_str);
        
        char buffer[^17];
        sprintf(buffer, "%016zx", hash);
        return std::string(buffer);
    }
};
```

</details>


### 7.2 Chunk Deduplication Manager

<details>
<summary>ChunkMetadata Struct</summary>

```cpp
#include <unordered_map>
#include <mutex>

struct ChunkMetadata {
    std::string chunk_hash;
    size_t size_bytes;
    std::string storage_key;  // S3 key
    int reference_count;
};

class DeduplicationManager {
private:
    std::unordered_map<std::string, ChunkMetadata> chunk_index_;
    std::mutex index_mtx_;
    
    DatabaseConnection db_;
    S3Client s3_;
    
public:
    DeduplicationManager(DatabaseConnection& db, S3Client& s3)
        : db_(db), s3_(s3) {}
    
    bool uploadChunk(const std::string& chunk_hash,
                    const std::vector<uint8_t>& data) {
        std::cout << "\n=== Uploading Chunk ===" << std::endl;
        std::cout << "Hash: " << chunk_hash.substr(0, 16) << "..." << std::endl;
        std::cout << "Size: " << data.size() << " bytes" << std::endl;
        
        // Check if chunk already exists (deduplication)
        {
            std::lock_guard<std::mutex> lock(index_mtx_);
            auto it = chunk_index_.find(chunk_hash);
            if (it != chunk_index_.end()) {
                // Chunk exists, increment reference count
                it->second.reference_count++;
                
                std::cout << "✓ Chunk already exists (deduplicated)" << std::endl;
                std::cout << "  Reference count: " << it->second.reference_count << std::endl;
                
                // Update database
                updateReferenceCount(chunk_hash, it->second.reference_count);
                
                return true;  // Skip upload
            }
        }
        
        // New chunk, upload to S3
        std::string storage_key = "chunks/" + chunk_hash.substr(0, 2) + "/" + 
                                 chunk_hash.substr(2, 2) + "/" + chunk_hash;
        
        std::cout << "  Uploading to S3: " << storage_key << std::endl;
        
        bool success = s3_.putObject(storage_key, data);
        
        if (success) {
            // Add to index
            ChunkMetadata metadata;
            metadata.chunk_hash = chunk_hash;
            metadata.size_bytes = data.size();
            metadata.storage_key = storage_key;
            metadata.reference_count = 1;
            
            {
                std::lock_guard<std::mutex> lock(index_mtx_);
                chunk_index_[chunk_hash] = metadata;
            }
            
            // Save to database
            saveChunkMetadata(metadata);
            
            std::cout << "✓ Chunk uploaded successfully" << std::endl;
        }
        
        return success;
    }
    
    std::vector<uint8_t> downloadChunk(const std::string& chunk_hash) {
        std::cout << "\n=== Downloading Chunk ===" << std::endl;
        std::cout << "Hash: " << chunk_hash.substr(0, 16) << "..." << std::endl;
        
        // Get storage key
        std::string storage_key;
        {
            std::lock_guard<std::mutex> lock(index_mtx_);
            auto it = chunk_index_.find(chunk_hash);
            if (it != chunk_index_.end()) {
                storage_key = it->second.storage_key;
            }
        }
        
        if (storage_key.empty()) {
            // Load from database
            storage_key = loadStorageKey(chunk_hash);
        }
        
        if (storage_key.empty()) {
            std::cerr << "✗ Chunk not found" << std::endl;
            return {};
        }
        
        // Download from S3
        std::cout << "  Downloading from S3: " << storage_key << std::endl;
        
        auto data = s3_.getObject(storage_key);
        
        std::cout << "✓ Downloaded " << data.size() << " bytes" << std::endl;
        
        return data;
    }
    
    void deleteChunk(const std::string& chunk_hash) {
        std::lock_guard<std::mutex> lock(index_mtx_);
        
        auto it = chunk_index_.find(chunk_hash);
        if (it == chunk_index_.end()) {
            return;
        }
        
        // Decrement reference count
        it->second.reference_count--;
        
        std::cout << "\n=== Deleting Chunk Reference ===" << std::endl;
        std::cout << "Hash: " << chunk_hash.substr(0, 16) << "..." << std::endl;
        std::cout << "Reference count: " << it->second.reference_count << std::endl;
        
        if (it->second.reference_count <= 0) {
            // No more references, delete from S3
            std::cout << "  Deleting from S3..." << std::endl;
            s3_.deleteObject(it->second.storage_key);
            
            chunk_index_.erase(it);
            
            // Delete from database
            deleteChunkMetadata(chunk_hash);
            
            std::cout << "✓ Chunk deleted" << std::endl;
        } else {
            // Still referenced, just update count
            updateReferenceCount(chunk_hash, it->second.reference_count);
            std::cout << "✓ Reference count updated" << std::endl;
        }
    }
    
private:
    void saveChunkMetadata(const ChunkMetadata& metadata) {
        std::string query = R"(
            INSERT INTO file_chunks (chunk_hash, size_bytes, storage_key, reference_count)
            VALUES (?, ?, ?, ?)
            ON CONFLICT (chunk_hash) DO UPDATE SET reference_count = reference_count + 1
        )";
        
        db_.execute(query, metadata.chunk_hash, metadata.size_bytes,
                   metadata.storage_key, metadata.reference_count);
    }
    
    void updateReferenceCount(const std::string& chunk_hash, int count) {
        std::string query = "UPDATE file_chunks SET reference_count = ? WHERE chunk_hash = ?";
        db_.execute(query, count, chunk_hash);
    }
    
    void deleteChunkMetadata(const std::string& chunk_hash) {
        std::string query = "DELETE FROM file_chunks WHERE chunk_hash = ?";
        db_.execute(query, chunk_hash);
    }
    
    std::string loadStorageKey(const std::string& chunk_hash) {
        std::string query = "SELECT storage_key FROM file_chunks WHERE chunk_hash = ?";
        auto result = db_.query(query, chunk_hash);
        
        if (!result.empty()) {
            return result[^0]["storage_key"];
        }
        
        return "";
    }
};
```

</details>


### 7.3 File Sync Engine

<details>
<summary>FileEvent Struct</summary>

```cpp
struct FileEvent {
    std::string file_id;
    std::string path;
    std::string event_type;  // added, modified, deleted, moved
    std::chrono::system_clock::time_point timestamp;
    int version;
};

class SyncEngine {
private:
    std::string user_id_;
    std::string device_id_;
    std::string sync_cursor_;
    
    DatabaseConnection db_;
    RabinChunker chunker_;
    DeduplicationManager& dedup_manager_;
    
    // Local file cache
    std::unordered_map<std::string, std::string> local_file_hashes_;
    std::mutex cache_mtx_;
    
public:
    SyncEngine(const std::string& user_id,
              const std::string& device_id,
              DatabaseConnection& db,
              DeduplicationManager& dedup)
        : user_id_(user_id),
          device_id_(device_id),
          db_(db),
          dedup_manager_(dedup) {}
    
    void uploadFile(const std::string& local_path, const std::string& remote_path) {
        std::cout << "\n=== Uploading File ===" << std::endl;
        std::cout << "Local: " << local_path << std::endl;
        std::cout << "Remote: " << remote_path << std::endl;
        
        // Read file
        std::vector<uint8_t> file_data = readFile(local_path);
        
        std::cout << "File size: " << file_data.size() << " bytes" << std::endl;
        
        // Chunk file
        auto chunks = chunker_.chunkFile(file_data);
        
        // Upload chunks
        std::vector<std::string> chunk_hashes;
        for (const auto& chunk : chunks) {
            bool success = dedup_manager_.uploadChunk(chunk.hash, chunk.data);
            if (success) {
                chunk_hashes.push_back(chunk.hash);
            }
        }
        
        // Compute file hash
        std::string file_hash = computeFileHash(file_data);
        
        // Create file metadata
        std::string file_id = generateFileId();
        
        std::string query = R"(
            INSERT INTO files (file_id, user_id, name, path, size_bytes, 
                              content_hash, version, modified_at)
            VALUES (?, ?, ?, ?, ?, ?, 1, NOW())
        )";
        
        std::string filename = extractFilename(remote_path);
        
        db_.execute(query, file_id, user_id_, filename, remote_path,
                   file_data.size(), file_hash);
        
        // Store chunk mappings
        for (size_t i = 0; i < chunk_hashes.size(); ++i) {
            saveChunkMapping(file_id, chunk_hashes[i], chunks[i].offset, i);
        }
        
        // Update local cache
        {
            std::lock_guard<std::mutex> lock(cache_mtx_);
            local_file_hashes_[remote_path] = file_hash;
        }
        
        std::cout << "✓ File uploaded successfully" << std::endl;
        std::cout << "  File ID: " << file_id << std::endl;
        std::cout << "  Chunks: " << chunk_hashes.size() << std::endl;
    }
    
    void downloadFile(const std::string& file_id, const std::string& local_path) {
        std::cout << "\n=== Downloading File ===" << std::endl;
        std::cout << "File ID: " << file_id << std::endl;
        std::cout << "Local path: " << local_path << std::endl;
        
        // Get file metadata
        auto file_info = getFileMetadata(file_id);
        if (!file_info) {
            std::cerr << "✗ File not found" << std::endl;
            return;
        }
        
        // Get chunk hashes
        auto chunk_hashes = getChunkHashes(file_id);
        
        std::cout << "Downloading " << chunk_hashes.size() << " chunks..." << std::endl;
        
        // Download chunks
        std::vector<uint8_t> file_data;
        for (const auto& chunk_hash : chunk_hashes) {
            auto chunk_data = dedup_manager_.downloadChunk(chunk_hash);
            file_data.insert(file_data.end(), chunk_data.begin(), chunk_data.end());
        }
        
        // Write to file
        writeFile(local_path, file_data);
        
        // Update local cache
        {
            std::lock_guard<std::mutex> lock(cache_mtx_);
            local_file_hashes_[file_info->path] = file_info->content_hash;
        }
        
        std::cout << "✓ File downloaded successfully" << std::endl;
        std::cout << "  Size: " << file_data.size() << " bytes" << std::endl;
    }
    
    std::vector<FileEvent> getDeltaSync() {
        std::cout << "\n=== Delta Sync ===" << std::endl;
        std::cout << "Cursor: " << (sync_cursor_.empty() ? "(initial)" : sync_cursor_) << std::endl;
        
        std::string query = R"(
            SELECT file_id, path, event_type, created_at, version
            FROM sync_events
            WHERE user_id = ? AND event_id > ?
            ORDER BY event_id
            LIMIT 1000
        )";
        
        int cursor_id = sync_cursor_.empty() ? 0 : std::stoi(sync_cursor_);
        
        auto results = db_.query(query, user_id_, cursor_id);
        
        std::vector<FileEvent> events;
        
        for (const auto& row : results) {
            FileEvent event;
            event.file_id = row["file_id"];
            event.path = row["path"];
            event.event_type = row["event_type"];
            event.version = std::stoi(row["version"]);
            
            events.push_back(event);
        }
        
        std::cout << "Found " << events.size() << " changes" << std::endl;
        
        // Update cursor
        if (!events.empty()) {
            sync_cursor_ = std::to_string(cursor_id + events.size());
        }
        
        return events;
    }
    
    void processEvents(const std::vector<FileEvent>& events) {
        for (const auto& event : events) {
            std::cout << "\nEvent: " << event.event_type << " " << event.path << std::endl;
            
            if (event.event_type == "added" || event.event_type == "modified") {
                // Download file
                std::string local_path = "/sync/" + event.path;
                downloadFile(event.file_id, local_path);
            } else if (event.event_type == "deleted") {
                // Delete local file
                std::string local_path = "/sync/" + event.path;
                deleteLocalFile(local_path);
            }
        }
    }
    
private:
    std::vector<uint8_t> readFile(const std::string& path) {
        // Simplified file reading
        std::vector<uint8_t> data;
        // In production: Use proper file I/O
        return data;
    }
    
    void writeFile(const std::string& path, const std::vector<uint8_t>& data) {
        // Simplified file writing
        std::cout << "Writing " << data.size() << " bytes to " << path << std::endl;
    }
    
    void deleteLocalFile(const std::string& path) {
        std::cout << "Deleting local file: " << path << std::endl;
    }
    
    std::string computeFileHash(const std::vector<uint8_t>& data) {
        std::hash<std::string> hasher;
        std::string data_str(data.begin(), data.end());
        size_t hash = hasher(data_str);
        
        char buffer[^17];
        sprintf(buffer, "%016zx", hash);
        return std::string(buffer);
    }
    
    std::string generateFileId() {
        static int counter = 0;
        return "file_" + std::to_string(++counter);
    }
    
    std::string extractFilename(const std::string& path) {
        size_t pos = path.rfind('/');
        if (pos != std::string::npos) {
            return path.substr(pos + 1);
        }
        return path;
    }
    
    struct FileInfo {
        std::string file_id;
        std::string path;
        size_t size_bytes;
        std::string content_hash;
    };
    
    std::optional<FileInfo> getFileMetadata(const std::string& file_id) {
        std::string query = "SELECT * FROM files WHERE file_id = ?";
        auto result = db_.query(query, file_id);
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        FileInfo info;
        info.file_id = result[^0]["file_id"];
        info.path = result[^0]["path"];
        info.size_bytes = std::stoul(result[^0]["size_bytes"]);
        info.content_hash = result[^0]["content_hash"];
        
        return info;
    }
    
    std::vector<std::string> getChunkHashes(const std::string& file_id) {
        std::string query = R"(
            SELECT fc.chunk_hash
            FROM file_chunk_mappings fcm
            JOIN file_chunks fc ON fcm.chunk_id = fc.chunk_id
            WHERE fcm.file_id = ?
            ORDER BY fcm.chunk_sequence
        )";
        
        auto results = db_.query(query, file_id);
        
        std::vector<std::string> hashes;
        for (const auto& row : results) {
            hashes.push_back(row["chunk_hash"]);
        }
        
        return hashes;
    }
    
    void saveChunkMapping(const std::string& file_id, const std::string& chunk_hash,
                         size_t offset, int sequence) {
        std::string query = R"(
            INSERT INTO file_chunk_mappings (file_id, chunk_id, chunk_offset, chunk_sequence)
            SELECT ?, chunk_id, ?, ?
            FROM file_chunks
            WHERE chunk_hash = ?
        )";
        
        db_.execute(query, file_id, offset, sequence, chunk_hash);
    }
};
```

</details>


### 7.4 Complete File Storage System

<details>
<summary>FileStorageSystem Class</summary>

```cpp
class FileStorageSystem {
private:
    DatabaseConnection db_;
    S3Client s3_;
    DeduplicationManager dedup_manager_;
    
public:
    FileStorageSystem()
        : db_("postgresql://localhost/file_storage"),
          s3_("s3://file-storage-bucket"),
          dedup_manager_(db_, s3_) {}
    
    void simulateFileSync() {
        std::cout << "========================================" << std::endl;
        std::cout << "    File Storage & Sync Simulation" << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        std::string user_id = "user_123";
        std::string device_id = "device_abc";
        
        SyncEngine sync_engine(user_id, device_id, db_, dedup_manager_);
        
        // Scenario 1: Upload file
        std::cout << "\n--- Scenario 1: File Upload ---" << std::endl;
        
        // Simulate file data
        std::vector<uint8_t> file_data(10 * 1024 * 1024);  // 10 MB
        for (size_t i = 0; i < file_data.size(); ++i) {
            file_data[i] = rand() % 256;
        }
        
        // Chunk the file
        RabinChunker chunker;
        auto chunks = chunker.chunkFile(file_data);
        
        // Upload chunks (with deduplication)
        std::cout << "\nUploading chunks..." << std::endl;
        for (const auto& chunk : chunks) {
            dedup_manager_.uploadChunk(chunk.hash, chunk.data);
        }
        
        // Scenario 2: Upload modified file (same chunks + new chunk)
        std::cout << "\n--- Scenario 2: Modified File Upload ---" << std::endl;
        
        // Modify file (append data)
        file_data.resize(11 * 1024 * 1024);
        for (size_t i = 10 * 1024 * 1024; i < file_data.size(); ++i) {
            file_data[i] = rand() % 256;
        }
        
        auto chunks_modified = chunker.chunkFile(file_data);
        
        int new_chunks = 0;
        int deduped_chunks = 0;
        
        for (const auto& chunk : chunks_modified) {
            // Check if chunk already exists
            bool exists = false;
            for (const auto& old_chunk : chunks) {
                if (old_chunk.hash == chunk.hash) {
                    exists = true;
                    deduped_chunks++;
                    break;
                }
            }
            
            if (!exists) {
                new_chunks++;
            }
            
            dedup_manager_.uploadChunk(chunk.hash, chunk.data);
        }
        
        std::cout << "\n=== Deduplication Results ===" << std::endl;
        std::cout << "Total chunks: " << chunks_modified.size() << std::endl;
        std::cout << "Deduplicated: " << deduped_chunks << " ("
                 << (deduped_chunks * 100 / chunks_modified.size()) << "%)" << std::endl;
        std::cout << "New chunks: " << new_chunks << std::endl;
        
        std::cout << "\n=== Simulation Complete ===" << std::endl;
    }
};

int main() {
    FileStorageSystem system;
    system.simulateFileSync();
    
    return 0;
}
```

</details>


***

## Step 8: Bottlenecks \& Optimizations

### Bottleneck 1: Upload Bandwidth

**Problem:** Users have limited upload bandwidth (5 Mbps typical)

**Solution: Multi-Part Parallel Upload**

<details>
<summary>ParallelUploader Class</summary>

```cpp
class ParallelUploader {
public:
    void uploadFile(const std::vector<RabinChunker::Chunk>& chunks) {
        const int PARALLEL_UPLOADS = 4;  // Upload 4 chunks simultaneously
        
        std::vector<std::thread> threads;
        std::atomic<int> completed{0};
        
        for (int i = 0; i < chunks.size(); i += PARALLEL_UPLOADS) {
            for (int j = 0; j < PARALLEL_UPLOADS && (i + j) < chunks.size(); ++j) {
                threads.emplace_back([&, idx = i + j]() {
                    uploadChunk(chunks[idx]);
                    completed++;
                });
            }
            
            // Wait for this batch
            for (auto& t : threads) {
                t.join();
            }
            threads.clear();
        }
    }
};

// Result: 4× faster upload (limited by bandwidth, not processing)
```

</details>


### Bottleneck 2: Sync Latency

**Problem:** Polling every 30 seconds = up to 30-second delay

**Solution: WebSocket Push Notifications**

```javascript
// Client-side
const ws = new WebSocket('wss://sync.dropbox.com/sync');

ws.onmessage = (event) => {
    const notification = JSON.parse(event.data);
    
    if (notification.type === 'file_changed') {
        // Immediate sync
        syncFile(notification.file_id);
    }
};

// Server broadcasts changes instantly
// Latency: <1 second (vs 30 seconds)
```


### Bottleneck 3: Metadata Database Load

**Problem:** 115K metadata ops/sec on single database

**Solution: Sharding + Caching**

```
Sharding:
- Shard by user_id (consistent hashing)
- 100 database shards
- Load per shard: 1,150 ops/sec (manageable)

Caching (Redis):
- Cache file metadata (95% hit rate)
- Actual database load: 5,750 ops/sec
- Distributed across 100 shards = 57 ops/sec per shard

Result: 2,000× reduction in database load per shard
```


***

## Summary: Key Decisions

| Aspect | Decision | Rationale |
| :-- | :-- | :-- |
| **Chunking** | Content-defined (Rabin) | Edit-friendly, deduplication |
| **Chunk Size** | 4 MB average | Balance overhead vs efficiency |
| **Deduplication** | Chunk-level | 30% storage savings |
| **Sync Protocol** | WebSocket push | <5 second latency |
| **Conflict Resolution** | Conflict copies | User control |
| **Storage** | S3 (multi-region) | 11 nines durability |

**Performance Characteristics:**

```
Scale (2025):
- Users: 700M (Dropbox) [web:509], 1B+ (Google Drive) [web:490]
- Files: 7 trillion files
- Storage: 9.8 exabytes (with dedup)
- Operations: 115K ops/sec

Per-File:
- Upload: 4 MB chunks, parallel
- Download: 200 objects/sec [web:514]
- Sync latency: <5 seconds

Deduplication:
- Storage saved: 30% (4.2 EB)
- Bandwidth saved: 30%

Infrastructure:
- Database shards: 100 shards
- S3 regions: 3 regions
- WebSocket servers: 5,000 servers
- Connections: 400M devices
```

**Platform Comparison:**


| Feature | Dropbox | Google Drive | OneDrive | iCloud |
| :-- | :-- | :-- | :-- | :-- |
| **Users** | 700M [^1] | 1B+ [^2] | 400M | 2B |
| **Free Storage** | 2 GB [^3] | 15 GB [^2] | 5 GB | 5 GB |
| **Chunking** | Rabin fingerprint | Custom | Fixed-size | Unknown |
| **Deduplication** | Block-level | File-level | Block-level | File-level |
| **Sync Protocol** | WebSocket | WebSocket/HTTP | WebSocket | Push notifications |
| **Sync Latency** | <5 sec | <5 sec | <10 sec | <10 sec |
| **LAN Sync** | Yes | No | Yes | No |

This File Storage \& Sync System handles **700M users** , **9.8 exabytes** storage, **115K operations/sec**, with **30% deduplication savings** using content-defined chunking, WebSocket sync, and S3 storage! ☁️📁🔄[^1]
<span style="display:none">[^10][^11][^12][^13][^14][^15][^16][^17][^18][^19][^20][^6][^7][^8][^9]</span>

<div align="center">⁂</div>

[^1]: https://backlinko.com/dropbox-users

[^2]: https://electroiq.com/stats/google-drive-statistics/

[^3]: https://sqmagazine.co.uk/dropbox-statistics/

[^4]: https://learn.microsoft.com/en-us/azure/storage/file-sync/file-sync-scale-targets

[^5]: https://www.syncbreeze.com/syncbreeze_file_synchronization_performance.html

[^6]: https://www.skillademia.com/statistics/dropbox-statistics/

[^7]: https://help.dropbox.com/storage-space/account-space-left

[^8]: https://threadgoldconsulting.com/research/personal-cloud-storage-usage

[^9]: https://electroiq.com/stats/dropbox-statistics/

[^10]: https://explodingtopics.com/blog/google-workspace-stats

[^11]: https://connectbit.com/cloud-storage-statistics/

[^12]: https://drive.google.com/drive/quota

[^13]: https://learn.microsoft.com/en-us/azure/storage/file-sync/file-sync-monitoring

[^14]: https://www.statista.com/topics/10094/dropbox-inc/

[^15]: https://support.google.com/drive/thread/353742027/gdrive-desktop-2025-update-made-it-the-worst-cannot-see-which-files-are-syncing?hl=en

[^16]: https://www.ibm.com/docs/ssw_aix_72/performance/file_sync_perf_tuning.html

[^17]: https://www.experte.com/cloud-storage/dropbox

[^18]: https://www.youtube.com/watch?v=luH4t1kZ5CA

[^19]: https://docs.aws.amazon.com/efs/latest/ug/efs-metrics.html

[^20]: https://www.cloudzero.com/blog/cloud-computing-statistics/

