# WhatsApp System Design

## Step 1: Requirements Clarification

### Functional Requirements

**Core Messaging:**

- Send one-to-one text messages
- Send group messages (up to 256 members)
- Send media (images, videos, audio, documents)
- Voice/video calls (out of scope for this design - separate system)
- Message status (sent ✓, delivered ✓✓, read ✓✓)
- Real-time message delivery
- Offline message queue
- Last seen / online status
- Typing indicators

**Message Features:**

- Message editing (within 15 minutes)
- Message deletion (delete for me, delete for everyone)
- Reply to messages
- Forward messages
- Star/favorite messages
- Message search

**Group Features:**

- Create group
- Add/remove members
- Group admin controls
- Group info/description
- Exit group

**Profile \& Contacts:**

- User profile (name, photo, status)
- Contact sync
- Block/unblock users

**End-to-End Encryption:**

- All messages encrypted
- Signal Protocol

**Out of Scope:**

- Voice/video calls
- Status/Stories feature
- Payment features
- WhatsApp Business features


### Non-Functional Requirements

**Scale (Based on 2025 data):**

- 3 billion monthly active users[^1]
- 2 billion daily active users
- 140 billion messages per day[^1]
- 1.6 million messages per second (average)[^2]
- 5 million messages per second (peak)
- Average message size: 100 bytes (text), 1 MB (media)

**Performance:**

- Message delivery latency: <200ms (P95)
- WebSocket connection establishment: <100ms
- Media upload/download: Based on bandwidth
- Support 50K concurrent connections per server

**Reliability:**

- 99.99% uptime
- At-least-once message delivery
- Message persistence (never lose messages)
- Offline message queue (store until delivered)

**Availability:**

- Messages available from any device
- Multi-device support
- Cross-platform (iOS, Android, Web)

***

## Step 2: WhatsApp Theory \& Concepts

### 2.1 Real-Time Communication - WebSocket

**Why WebSocket Over HTTP?**

```
HTTP Polling (Bad):
Client → Server: "Any new messages?" (every 5 seconds)
Server → Client: "No"
...repeat forever

Problems:
❌ Latency (5 second delay)
❌ Wasted bandwidth (constant polling)
❌ Server load (millions of polls)

HTTP Long Polling (Better):
Client → Server: "Any new messages?" (waits)
Server: (holds connection open for 30s)
Server → Client: "Yes! Here's a message"
Client → Server: "Any more?" (immediately)

Problems:
❌ Still inefficient
❌ Reconnection overhead
❌ Not true bidirectional

WebSocket (Best):
Client ←→ Server: Persistent TCP connection
Server → Client: Push messages instantly
Client → Server: Send messages instantly

Advantages:
✅ True bidirectional
✅ Low latency (<100ms)
✅ Efficient (one connection)
✅ Real-time push
```

**WebSocket Handshake:**

```
1. Client → Server: HTTP Upgrade request
GET /chat HTTP/1.1
Host: whatsapp.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==
Sec-WebSocket-Version: 13

2. Server → Client: Switch to WebSocket
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=

3. Now persistent connection established
Client ←→ Server: Binary frames (messages)
```


### 2.2 Message Delivery Guarantees

**At-Least-Once Delivery:**

```
Problem: Network failures can lose messages

Solution: Acknowledgment + Retry

Client → Server: Message (ID: 123)
Server → Client: ACK (ID: 123)
Client: ✅ Message delivered, delete from local queue

If no ACK:
Client: Retry after timeout
Server: Detect duplicate (by ID), send ACK but don't duplicate

Result: At-least-once (message delivered 1+ times)
```

**Exactly-Once Semantics (Illusion):**

```
Reality: Exactly-once is impossible in distributed systems

WhatsApp's approach:
1. At-least-once delivery (guarantee delivery)
2. Idempotent processing (detect duplicates by message ID)
3. User sees: Exactly-once (deduplication on UI)

Message ID = hash(sender_id + timestamp + random)
Server stores: processed_message_ids (last 7 days)
If duplicate: Send ACK but don't store again
```


### 2.3 Message Status System

**Single Check (✓):**

```
Message sent from client to server
Server ACKs receipt
Client shows: Sent (✓)
```

**Double Check (✓✓):**

```
Message delivered from server to recipient's device
Recipient's device sends delivery ACK to server
Server forwards ACK to sender
Sender shows: Delivered (✓✓)
```

**Blue Double Check (✓✓):**

```
Recipient opens chat and reads message
Recipient's device sends read receipt to server
Server forwards to sender
Sender shows: Read (blue ✓✓)
```

**Sequence Diagram:**

```
Sender          Server          Recipient
   |──Message──→|                |
   |            |──Store in DB──→|
   |←─ACK (✓)───|                |
   |            |──Push msg──────→|
   |            |←──Delivered ACK─|
   |←─Delivered─|                |
   |   (✓✓)     |                |
   |            |                | (User opens chat)
   |            |←──Read ACK─────|
   |←─Read (✓✓)─|                |
```


### 2.4 Offline Message Queue

**Challenge:** Recipient is offline, how to deliver messages?

```
Solution: Server-Side Queue

1. User A sends message to User B (offline)
2. Server stores in B's message queue (database)
3. Server sends push notification to B's phone
4. User B comes online (opens WhatsApp)
5. B's device → Server: "I'm online, any messages?"
6. Server → B: Here are 50 queued messages
7. B's device: Display messages, send delivery ACKs
8. Server: Clear B's queue, forward ACKs to A
```

**Queue Structure:**

<details>
<summary>MessageQueue Struct</summary>

```cpp
struct MessageQueue {
    user_id: 123
    messages: [
        {msg_id: "abc", from: 456, timestamp: 1728048000, content: "Hello"},
        {msg_id: "def", from: 789, timestamp: 1728048010, content: "Hi"},
        ...
    ]
    max_size: 10,000 messages
    retention: 30 days
}
```

</details>


### 2.5 Last Seen \& Online Status

**Privacy-Aware Status:**

```
User Online Status:
- Online: Currently has active WebSocket connection
- Last seen: Timestamp of last activity
- Privacy settings: Everyone / Contacts / Nobody

Implementation:
WebSocket connection: User marked "online"
Disconnect: Record timestamp → "Last seen"
Heartbeat: Every 30s to detect zombie connections

Optimization: Don't broadcast every status change
- Batch updates every 30 seconds
- Only notify contacts who have chat with user
```


### 2.6 Group Messaging

**Fan-Out Pattern:**

```
Sender → Server: Message for Group (100 members)

Option 1: Immediate Fan-Out (WhatsApp uses this)
Server: For each member:
  - Add message to member's inbox
  - If online: Push via WebSocket
  - If offline: Queue + push notification

Latency: ~100ms (parallel delivery)

Option 2: Pull-Based
Server: Store message in group table
Members: Pull from group table when online
Latency: Higher (wait for pull)

WhatsApp Choice: Push-based for real-time experience
```

**Group Size Limit: 256 members**

```
Why 256?
- Fan-out to 256 users is manageable
- Beyond this, overhead increases exponentially
- Larger groups → Use broadcast lists or channels
```


### 2.7 End-to-End Encryption (E2EE)

**Signal Protocol:**

```
Key Exchange (Simplified):
1. Alice generates key pair: (PublicKeyA, PrivateKeyA)
2. Bob generates key pair: (PublicKeyB, PrivateKeyB)
3. Alice fetches Bob's PublicKeyB from server
4. Bob fetches Alice's PublicKeyA from server
5. Both derive shared secret using Diffie-Hellman

Message Encryption:
Alice: plaintext → encrypt(plaintext, shared_secret) → ciphertext
Server: Relay ciphertext (cannot decrypt!)
Bob: decrypt(ciphertext, shared_secret) → plaintext

Server sees: Only encrypted blobs
End-to-End: Only Alice and Bob can decrypt
```

**Double Ratchet Algorithm:**

```
For each message, generate new encryption key
Previous keys destroyed (forward secrecy)
Even if current key compromised, past messages safe

Message 1: Key1 → encrypt → destroy Key1
Message 2: Key2 → encrypt → destroy Key2
...
```


***

## Step 3: Capacity Estimation

```
Users & Activity:
Monthly active users: 3 billion [web:275]
Daily active users: 2 billion
Peak concurrent users: 500 million (25% of DAU)

Messages:
Total per day: 140 billion [web:275]
Messages per second (avg): 140B / 86,400 = 1.62 million/sec
Messages per second (peak): 5 million/sec (3x average)

Message Breakdown:
- Text (80%): 112B messages/day × 100 bytes = 11.2 TB/day
- Images (15%): 21B messages/day × 500 KB = 10.5 PB/day
- Videos (4%): 5.6B messages/day × 5 MB = 28 PB/day
- Audio (1%): 1.4B messages/day × 100 KB = 140 TB/day
Total data: ~38.6 PB/day

Compressed (3:1): 12.9 PB/day

Storage (30-day retention):
Text messages: 11.2 TB × 30 = 336 TB
Media (compressed): 12.9 PB × 30 = 387 PB
Total: ~390 PB

WebSocket Connections:
Concurrent connections: 500 million
Connections per server: 50K (using Erlang/BEAM can handle 2M [web:277])
Servers needed: 500M / 50K = 10,000 servers
With redundancy (3x): 30,000 connection servers

Message Routing:
Message routing lookups: 1.62M/sec
Database reads: 1.62M × 2 (sender + recipient lookup) = 3.24M reads/sec
Database writes: 1.62M writes/sec

Chat Server Load:
Per chat server: 1.62M / 1000 servers = 1,620 messages/sec
With WebSocket forwarding: ~50-100 MB/sec per server

Database Writes:
Messages table: 1.62M writes/sec
With replication (3x): 4.86M writes/sec

Cassandra/ScyllaDB capacity:
Per node: 10K writes/sec
Nodes needed: 4.86M / 10K = 486 nodes
With headroom: 1,000 nodes

Media Storage:
Media files: 38.6 PB/day
Deduplicated (common images/videos): 30% savings → 27 PB/day
With S3/Blob storage: ~$600K/day ($0.023/GB/month × 27 PB)

Push Notifications:
Offline users: 1.5 billion (75% of DAU)
Notifications per day: 140B messages × 30% offline rate = 42B notifications/day
Per second: 42B / 86,400 = 486K notifications/sec

Network Bandwidth:
Ingress (messages): 1.62M msgs/sec × 100 bytes = 162 MB/sec
Media uploads: 38.6 PB/day / 86,400 = 446 GB/sec
Egress (deliver to recipients): 162 MB/sec × 2 (sender + recipient) = 324 MB/sec
Media downloads: 446 GB/sec
Total: ~900 GB/sec (with CDN for media)

Group Messages:
Group messages: 20% of 140B = 28B group messages/day
Average group size: 10 members
Fan-out: 28B × 10 = 280B deliveries/day
Additional load: 2x normal message delivery

Online Status Updates:
Status change events: 500M users × 10 changes/day = 5B events/day
Broadcast to contacts: 5B × 50 contacts avg = 250B status updates/day
Batched (every 30s): 250B / 2,880 = 87M updates/batch
```


***

## Step 4: API Design

### WebSocket Protocol Messages

```json
// Client → Server: Send Message
{
  "type": "message",
  "msg_id": "abc123def456",  // Client-generated UUID
  "to": "user_789",
  "content": "Hello, how are you?",
  "timestamp": 1728048000000,
  "encryption": {
    "version": "signal_v3",
    "ciphertext": "encrypted_blob_base64"
  }
}

// Server → Client: Acknowledge
{
  "type": "ack",
  "msg_id": "abc123def456",
  "status": "sent",
  "server_timestamp": 1728048001234
}

// Server → Recipient: Deliver Message
{
  "type": "message",
  "msg_id": "abc123def456",
  "from": "user_123",
  "content": "Hello, how are you?",
  "timestamp": 1728048000000,
  "encryption": {
    "version": "signal_v3",
    "ciphertext": "encrypted_blob_base64"
  }
}

// Recipient → Server: Delivery Acknowledgment
{
  "type": "delivery_ack",
  "msg_id": "abc123def456",
  "timestamp": 1728048002000
}

// Server → Sender: Update Status
{
  "type": "status_update",
  "msg_id": "abc123def456",
  "status": "delivered",
  "delivered_at": 1728048002000
}

// Recipient → Server: Read Receipt
{
  "type": "read_receipt",
  "msg_id": "abc123def456",
  "timestamp": 1728048120000
}
```


### REST API (for initial sync, media)

```json
POST /api/v1/auth/login
Request:
{
  "phone": "+919876543210",
  "verification_code": "123456"
}

Response: 200 OK
{
  "user_id": "user_123",
  "auth_token": "jwt_token_here",
  "session_id": "session_abc",
  "ws_endpoint": "wss://chat-server-5.whatsapp.com/ws"
}

GET /api/v1/messages/sync?since=1728000000

Response: 200 OK
{
  "messages": [
    {
      "msg_id": "abc123",
      "from": "user_456",
      "to": "user_123",
      "content": "encrypted_blob",
      "timestamp": 1728001000,
      "status": "delivered"
    }
  ],
  "cursor": "1728048000",
  "has_more": true
}

POST /api/v1/media/upload
Content-Type: multipart/form-data

Request:
{
  "file": <binary>,
  "msg_id": "abc123",
  "mime_type": "image/jpeg"
}

Response: 201 Created
{
  "media_id": "media_xyz789",
  "url": "https://cdn.whatsapp.com/media/xyz789",
  "thumbnail_url": "https://cdn.whatsapp.com/media/xyz789/thumb"
}

GET /api/v1/users/{user_id}/status

Response: 200 OK
{
  "user_id": "user_789",
  "status": "online",
  "last_seen": 1728048000
}
```


***

## Step 5: Database Design

### Cassandra Schema (Message Storage)

```sql
-- Messages table (partitioned by user_id for inbox)
CREATE TABLE messages (
    user_id UUID,                    -- Partition key (recipient)
    timestamp TIMESTAMP,              -- Clustering key (sorted)
    msg_id UUID,                      -- Message UUID
    from_user_id UUID,                -- Sender
    to_user_id UUID,                  -- Recipient (for 1-1) or group_id
    content BLOB,                     -- Encrypted content
    media_url TEXT,                   -- S3 URL if media
    status TEXT,                      -- sent, delivered, read
    delivered_at TIMESTAMP,
    read_at TIMESTAMP,
    PRIMARY KEY (user_id, timestamp, msg_id)
) WITH CLUSTERING ORDER BY (timestamp DESC);

-- Group messages table
CREATE TABLE group_messages (
    group_id UUID,                    -- Partition key
    timestamp TIMESTAMP,              -- Clustering key
    msg_id UUID,
    from_user_id UUID,
    content BLOB,
    media_url TEXT,
    PRIMARY KEY (group_id, timestamp, msg_id)
) WITH CLUSTERING ORDER BY (timestamp DESC);

-- User inbox (pointer to messages)
CREATE TABLE user_inbox (
    user_id UUID,                     -- Partition key
    chat_id UUID,                     -- Other user or group
    last_msg_id UUID,
    last_msg_timestamp TIMESTAMP,
    unread_count INT,
    PRIMARY KEY (user_id, chat_id)
);

-- Message status tracking
CREATE TABLE message_status (
    msg_id UUID PRIMARY KEY,
    sender_id UUID,
    recipient_id UUID,
    status TEXT,                      -- sent, delivered, read
    sent_at TIMESTAMP,
    delivered_at TIMESTAMP,
    read_at TIMESTAMP
);
```


### Redis Schema (Real-Time State)

```
# Online users (sorted set by last activity)
ZADD online_users 1728048000 "user_123"
ZADD online_users 1728048010 "user_456"

# User's WebSocket connection mapping
HSET user_connections "user_123" "ws_server_5"
HSET user_connections "user_456" "ws_server_12"

# Offline message queue (list)
LPUSH offline_queue:user_789 "{msg_id: 'abc', from: 'user_123', ...}"
LLEN offline_queue:user_789  # Check queue size

# Typing indicators (TTL 5 seconds)
SETEX typing:chat_abc "user_123" 5

# Delivery acknowledgment tracking
SADD pending_ack:user_123 "msg_id_abc123"
SREM pending_ack:user_123 "msg_id_abc123"  # After ACK received

# Rate limiting (per user)
INCR rate_limit:user_123:messages
EXPIRE rate_limit:user_123:messages 60  # 60 seconds window
```


### PostgreSQL Schema (User Data, Groups)

```sql
-- Users table
CREATE TABLE users (
    user_id UUID PRIMARY KEY,
    phone_number VARCHAR(20) UNIQUE NOT NULL,
    username VARCHAR(50),
    profile_photo_url TEXT,
    status_message TEXT,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    last_seen TIMESTAMPTZ,

    INDEX idx_phone (phone_number)
);

-- Contacts/Friends
CREATE TABLE contacts (
    user_id UUID REFERENCES users(user_id),
    contact_user_id UUID REFERENCES users(user_id),
    contact_name VARCHAR(100),  -- User's custom name for contact
    added_at TIMESTAMPTZ DEFAULT NOW(),
    blocked BOOLEAN DEFAULT FALSE,

    PRIMARY KEY (user_id, contact_user_id),
    INDEX idx_user_contacts (user_id)
);

-- Groups
CREATE TABLE groups (
    group_id UUID PRIMARY KEY,
    group_name VARCHAR(100),
    group_photo_url TEXT,
    created_by UUID REFERENCES users(user_id),
    created_at TIMESTAMPTZ DEFAULT NOW(),
    description TEXT,
    max_members INT DEFAULT 256
);

-- Group members
CREATE TABLE group_members (
    group_id UUID REFERENCES groups(group_id),
    user_id UUID REFERENCES users(user_id),
    role VARCHAR(20) DEFAULT 'member',  -- admin, member
    joined_at TIMESTAMPTZ DEFAULT NOW(),
    left_at TIMESTAMPTZ,  -- NULL if still member

    PRIMARY KEY (group_id, user_id),
    INDEX idx_user_groups (user_id)
);

-- Encryption keys (public keys for E2EE)
CREATE TABLE user_keys (
    user_id UUID REFERENCES users(user_id),
    key_id UUID PRIMARY KEY,
    public_key TEXT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW(),

    INDEX idx_user_keys (user_id)
);
```


***

## Step 6: High-Level Architecture

```mermaid
graph TB
    subgraph "Clients"
        IOS[iOS App]
        ANDROID[Android App]
        WEB[Web Client]
    end

    subgraph "Load Balancer & Gateway"
        LB[Load Balancer<br/>Nginx/HAProxy<br/>SSL Termination]

        GATEWAY[API Gateway<br/>REST endpoints<br/>Auth/Rate limiting]
    end

    subgraph "WebSocket Connection Layer (Erlang)"
        WS1[Chat Server 1<br/>50K connections<br/>Erlang/BEAM]
        WS2[Chat Server 2<br/>50K connections]
        WS3[Chat Server N<br/>10K total servers]
    end

    subgraph "Message Routing & Delivery"
        ROUTER[Message Router<br/>Route to recipient<br/>Fan-out for groups]

        QUEUE[Message Queue<br/>Kafka<br/>Buffering & replay]
    end

    subgraph "Real-Time State (Redis)"
        REDIS1[Redis Cluster 1<br/>Online users<br/>WS mapping]
        REDIS2[Redis Cluster 2<br/>Typing status<br/>Presence]
    end

    subgraph "Message Storage (Cassandra)"
        CASS1[Cassandra Node 1<br/>Messages<br/>Time-series]
        CASS2[Cassandra Node 2]
        CASS3[Cassandra Node N<br/>1000 nodes]
    end

    subgraph "User & Group Data (PostgreSQL)"
        PG_MASTER[(PostgreSQL Master<br/>Users, Groups<br/>Contacts)]
        PG_REPLICA[(PostgreSQL Replicas<br/>Read scaling)]
    end

    subgraph "Media Storage"
        S3[S3/Blob Storage<br/>Images, Videos<br/>Audio files<br/>390 PB]

        CDN[CDN<br/>CloudFront/Akamai<br/>Media delivery]
    end

    subgraph "Push Notifications"
        PUSH[Push Service<br/>APNs (iOS)<br/>FCM (Android)<br/>486K/sec]
    end

    subgraph "Supporting Services"
        AUTH[Auth Service<br/>Phone verification<br/>JWT tokens]

        PRESENCE[Presence Service<br/>Last seen<br/>Online status]

        ENCRYPTION[Key Service<br/>Public key distribution<br/>Signal protocol]
    end

    subgraph "Monitoring"
        METRICS[Metrics<br/>Prometheus<br/>Connection count<br/>Message latency]

        LOGS[Logs<br/>ELK Stack<br/>Error tracking]
    end

    IOS & ANDROID & WEB -->|HTTPS| LB
    LB --> GATEWAY

    IOS & ANDROID & WEB -->|WSS upgrade| LB
    LB --> WS1 & WS2 & WS3

    GATEWAY --> AUTH
    GATEWAY --> PG_MASTER

    WS1 & WS2 & WS3 <-->|Check online status| REDIS1 & REDIS2
    WS1 & WS2 & WS3 -->|Route message| ROUTER

    ROUTER -->|Buffer| QUEUE
    ROUTER -->|Fan-out| WS1 & WS2 & WS3

    ROUTER -->|Store| CASS1 & CASS2 & CASS3
    ROUTER -->|User/Group lookup| PG_REPLICA

    WS1 & WS2 & WS3 -->|Offline users| PUSH

    GATEWAY -->|Upload media| S3
    S3 --> CDN
    WEB -->|Download media| CDN

    WS1 & WS2 & WS3 --> PRESENCE
    PRESENCE --> REDIS1

    AUTH <--> PG_MASTER
    ENCRYPTION <--> PG_MASTER

    WS1 & ROUTER & CASS1 --> METRICS
    WS1 & ROUTER --> LOGS

    style WS1 fill:#90EE90
    style WS2 fill:#90EE90
    style REDIS1 fill:#dc382d
    style CASS1 fill:#4169E1
    style S3 fill:#ffa500
```


***

## Step 7: Core Implementation (C++)

### 7.1 Message Structure

<details>
<summary>Message Structure</summary>

<details>
<summary>class Enum</summary>

```cpp
#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono;

enum class MessageType {
    TEXT,
    IMAGE,
    VIDEO,
    AUDIO,
    DOCUMENT
};

enum class MessageStatus {
    PENDING,    // Not yet sent
    SENT,       // Sent to server (✓)
    DELIVERED,  // Delivered to recipient (✓✓)
    READ        // Read by recipient (blue ✓✓)
};

struct Message {
    std::string msg_id;                // UUID
    std::string from_user_id;
    std::string to_user_id;            // User ID or Group ID
    bool is_group;

    MessageType type;
    std::string content;               // Text or encrypted blob
    std::string media_url;             // S3 URL if media

    system_clock::time_point timestamp;
    MessageStatus status;

    system_clock::time_point delivered_at;
    system_clock::time_point read_at;

    // Encryption
    std::string encryption_version;    // "signal_v3"
    std::string ciphertext;

    // Generate unique message ID
    static std::string generateMessageId() {
        // UUID v4 generation (simplified)
        auto now = system_clock::now().time_since_epoch().count();
        auto random = std::rand();
        return "msg_" + std::to_string(now) + "_" + std::to_string(random);
    }

    json toJson() const {
        json j = {
            {"msg_id", msg_id},
            {"from", from_user_id},
            {"to", to_user_id},
            {"is_group", is_group},
            {"type", static_cast<int>(type)},
            {"content", content},
            {"timestamp", duration_cast<milliseconds>(
                timestamp.time_since_epoch()
            ).count()},
            {"status", static_cast<int>(status)}
        };

        if (!media_url.empty()) {
            j["media_url"] = media_url;
        }

        if (!ciphertext.empty()) {
            j["encryption"] = {
                {"version", encryption_version},
                {"ciphertext", ciphertext}
            };
        }

        return j;
    }

    static Message fromJson(const json& j) {
        Message msg;
        msg.msg_id = j["msg_id"];
        msg.from_user_id = j["from"];
        msg.to_user_id = j["to"];
        msg.is_group = j.value("is_group", false);
        msg.type = static_cast<MessageType>(j["type"]);
        msg.content = j["content"];
        msg.timestamp = system_clock::time_point(
            milliseconds(j["timestamp"].get<int64_t>())
        );
        msg.status = static_cast<MessageStatus>(j["status"]);

        if (j.contains("media_url")) {
            msg.media_url = j["media_url"];
        }

        if (j.contains("encryption")) {
            msg.encryption_version = j["encryption"]["version"];
            msg.ciphertext = j["encryption"]["ciphertext"];
        }

        return msg;
    }
};
```

</details>

</details>


### 7.2 WebSocket Connection Manager

<details>
<summary>WebSocket Connection Manager</summary>

<details>
<summary>WebSocketConnectionManager Class</summary>

```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <unordered_map>
#include <shared_mutex>

typedef websocketpp::server<websocketpp::config::asio> WebSocketServer;
typedef websocketpp::connection_hdl ConnectionHandle;

class WebSocketConnectionManager {
private:
    WebSocketServer server_;

    // User ID → WebSocket connection
    std::unordered_map<std::string, ConnectionHandle> user_connections_;

    // Connection → User ID (reverse mapping)
    std::unordered_map<ConnectionHandle, std::string,
                      std::owner_less<ConnectionHandle>> connection_users_;

    mutable std::shared_mutex connections_mtx_;

    // Callback for message router
    std::function<void(const Message&)> message_callback_;

public:
    WebSocketConnectionManager(int port) {
        // Initialize WebSocket server
        server_.init_asio();
        server_.set_reuse_addr(true);

        // Set handlers
        server_.set_open_handler([this](ConnectionHandle hdl) {
            onConnect(hdl);
        });

        server_.set_close_handler([this](ConnectionHandle hdl) {
            onDisconnect(hdl);
        });

        server_.set_message_handler([this](ConnectionHandle hdl,
                                          WebSocketServer::message_ptr msg) {
            onMessage(hdl, msg);
        });

        server_.listen(port);
        server_.start_accept();
    }

    void run() {
        std::cout << "WebSocket server running..." << std::endl;
        server_.run();
    }

    void stop() {
        server_.stop();
    }

    // Register message callback
    void setMessageCallback(std::function<void(const Message&)> callback) {
        message_callback_ = callback;
    }

    // Send message to specific user
    bool sendToUser(const std::string& user_id, const Message& msg) {
        std::shared_lock<std::shared_mutex> lock(connections_mtx_);

        auto it = user_connections_.find(user_id);
        if (it == user_connections_.end()) {
            std::cout << "User " << user_id << " not connected" << std::endl;
            return false;  // User offline
        }

        try {
            std::string json_msg = msg.toJson().dump();
            server_.send(it->second, json_msg, websocketpp::frame::opcode::text);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to send message: " << e.what() << std::endl;
            return false;
        }
    }

    // Check if user is online
    bool isUserOnline(const std::string& user_id) const {
        std::shared_lock<std::shared_mutex> lock(connections_mtx_);
        return user_connections_.count(user_id) > 0;
    }

    // Get connection statistics
    size_t getConnectionCount() const {
        std::shared_lock<std::shared_mutex> lock(connections_mtx_);
        return user_connections_.size();
    }

private:
    void onConnect(ConnectionHandle hdl) {
        std::cout << "New WebSocket connection" << std::endl;

        // Note: User authentication happens via first message
        // Connection is temporarily stored without user ID
    }

    void onDisconnect(ConnectionHandle hdl) {
        std::unique_lock<std::shared_mutex> lock(connections_mtx_);

        auto it = connection_users_.find(hdl);
        if (it != connection_users_.end()) {
            std::string user_id = it->second;
            std::cout << "User " << user_id << " disconnected" << std::endl;

            user_connections_.erase(user_id);
            connection_users_.erase(it);

            // Update last seen timestamp
            updateLastSeen(user_id);
        }
    }

    void onMessage(ConnectionHandle hdl, WebSocketServer::message_ptr msg) {
        try {
            std::string payload = msg->get_payload();
            json j = json::parse(payload);

            std::string msg_type = j["type"];

            if (msg_type == "auth") {
                // Authenticate connection
                handleAuth(hdl, j);
            } else if (msg_type == "message") {
                // Forward message
                Message message = Message::fromJson(j);

                if (message_callback_) {
                    message_callback_(message);
                }
            } else if (msg_type == "delivery_ack") {
                // Handle delivery acknowledgment
                handleDeliveryAck(j);
            } else if (msg_type == "read_receipt") {
                // Handle read receipt
                handleReadReceipt(j);
            }

        } catch (const std::exception& e) {
            std::cerr << "Error processing message: " << e.what() << std::endl;
        }
    }

    void handleAuth(ConnectionHandle hdl, const json& auth_data) {
        std::string user_id = auth_data["user_id"];
        std::string auth_token = auth_data["token"];

        // Validate token (check with auth service)
        if (validateAuthToken(user_id, auth_token)) {
            std::unique_lock<std::shared_mutex> lock(connections_mtx_);

            // Remove old connection if exists
            auto old_it = user_connections_.find(user_id);
            if (old_it != user_connections_.end()) {
                connection_users_.erase(old_it->second);
            }

            // Register new connection
            user_connections_[user_id] = hdl;
            connection_users_[hdl] = user_id;

            std::cout << "User " << user_id << " authenticated and connected" << std::endl;

            // Send auth success
            json response = {
                {"type", "auth_success"},
                {"user_id", user_id}
            };

            server_.send(hdl, response.dump(), websocketpp::frame::opcode::text);

            // Mark user as online
            markUserOnline(user_id);

            // Deliver queued messages
            deliverQueuedMessages(user_id);
        } else {
            std::cout << "Authentication failed for user " << user_id << std::endl;
            server_.close(hdl, websocketpp::close::status::policy_violation,
                         "Authentication failed");
        }
    }

    void handleDeliveryAck(const json& ack_data) {
        std::string msg_id = ack_data["msg_id"];
        // Update message status to DELIVERED
        // Forward to sender
    }

    void handleReadReceipt(const json& receipt_data) {
        std::string msg_id = receipt_data["msg_id"];
        // Update message status to READ
        // Forward to sender
    }

    bool validateAuthToken(const std::string& user_id, const std::string& token) {
        // Verify JWT token with auth service
        return true;  // Simplified
    }

    void markUserOnline(const std::string& user_id) {
        // Update Redis: ZADD online_users <timestamp> <user_id>
    }

    void updateLastSeen(const std::string& user_id) {
        // Update Redis: last_seen:<user_id> = <timestamp>
    }

    void deliverQueuedMessages(const std::string& user_id) {
        // Fetch from Redis: LRANGE offline_queue:<user_id>
        // Send each message via WebSocket
        // Delete queue after delivery
    }
};
```

</details>

</details>


### 7.3 Message Router

<details>
<summary>Message Router</summary>

<details>
<summary>MessageRouter Class</summary>

```cpp
#include <queue>
#include <thread>
#include <condition_variable>

class MessageRouter {
private:
    WebSocketConnectionManager& ws_manager_;
    DatabaseConnection& db_;
    RedisClient& redis_;

    // Message queue for async processing
    std::queue<Message> message_queue_;
    std::mutex queue_mtx_;
    std::condition_variable queue_cv_;

    std::vector<std::thread> worker_threads_;
    std::atomic<bool> running_{false};

    // Statistics
    std::atomic<uint64_t> messages_routed_{0};
    std::atomic<uint64_t> messages_delivered_{0};
    std::atomic<uint64_t> messages_queued_{0};

public:
    MessageRouter(WebSocketConnectionManager& ws_mgr,
                 DatabaseConnection& db,
                 RedisClient& redis)
        : ws_manager_(ws_mgr), db_(db), redis_(redis) {}

    void start(int num_workers = 10) {
        running_ = true;

        // Start worker threads
        for (int i = 0; i < num_workers; ++i) {
            worker_threads_.emplace_back([this]() {
                processMessages();
            });
        }

        std::cout << "Message router started with " << num_workers
                 << " workers" << std::endl;
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

    // Enqueue message for routing
    void routeMessage(const Message& msg) {
        {
            std::lock_guard<std::mutex> lock(queue_mtx_);
            message_queue_.push(msg);
            queue_cv_.notify_one();
        }

        messages_routed_++;
    }

    void printStats() {
        std::cout << "\n=== Message Router Stats ===" << std::endl;
        std::cout << "Messages routed: " << messages_routed_ << std::endl;
        std::cout << "Messages delivered: " << messages_delivered_ << std::endl;
        std::cout << "Messages queued (offline): " << messages_queued_ << std::endl;
        std::cout << "Queue size: " << message_queue_.size() << std::endl;
    }

private:
    void processMessages() {
        while (running_) {
            Message msg;

            {
                std::unique_lock<std::mutex> lock(queue_mtx_);

                queue_cv_.wait(lock, [this]() {
                    return !message_queue_.empty() || !running_;
                });

                if (!running_ && message_queue_.empty()) break;

                if (message_queue_.empty()) continue;

                msg = message_queue_.front();
                message_queue_.pop();
            }

            // Process message
            if (msg.is_group) {
                routeGroupMessage(msg);
            } else {
                routeOneToOneMessage(msg);
            }
        }
    }

    void routeOneToOneMessage(Message& msg) {
        // 1. Store message in database
        storeMessage(msg);

        // 2. Send ACK to sender (message sent ✓)
        sendAckToSender(msg);

        // 3. Deliver to recipient
        if (ws_manager_.isUserOnline(msg.to_user_id)) {
            // Recipient online - deliver via WebSocket
            bool delivered = ws_manager_.sendToUser(msg.to_user_id, msg);

            if (delivered) {
                messages_delivered_++;
            } else {
                // Failed to deliver, queue for later
                queueMessage(msg);
            }
        } else {
            // Recipient offline - queue message
            queueMessage(msg);

            // Send push notification
            sendPushNotification(msg);
        }
    }

    void routeGroupMessage(const Message& msg) {
        // 1. Store message in database
        storeMessage(msg);

        // 2. Send ACK to sender
        sendAckToSender(msg);

        // 3. Get group members
        auto members = getGroupMembers(msg.to_user_id);  // to_user_id is group_id

        // 4. Fan-out to all members (except sender)
        for (const auto& member_id : members) {
            if (member_id == msg.from_user_id) continue;  // Skip sender

            Message member_msg = msg;
            member_msg.to_user_id = member_id;

            if (ws_manager_.isUserOnline(member_id)) {
                ws_manager_.sendToUser(member_id, member_msg);
            } else {
                queueMessage(member_msg);
                sendPushNotification(member_msg);
            }
        }
    }

    void storeMessage(const Message& msg) {
        // Store in Cassandra
        std::string query = R"(
            INSERT INTO messages (user_id, timestamp, msg_id, from_user_id,
                                 to_user_id, content, media_url, status)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        )";

        // Store for recipient
        db_.execute(query,
            msg.to_user_id,
            msg.timestamp,
            msg.msg_id,
            msg.from_user_id,
            msg.to_user_id,
            msg.content,
            msg.media_url,
            static_cast<int>(msg.status)
        );

        // Also store for sender (for multi-device sync)
        db_.execute(query,
            msg.from_user_id,
            msg.timestamp,
            msg.msg_id,
            msg.from_user_id,
            msg.to_user_id,
            msg.content,
            msg.media_url,
            static_cast<int>(msg.status)
        );
    }

    void sendAckToSender(const Message& msg) {
        if (ws_manager_.isUserOnline(msg.from_user_id)) {
            json ack = {
                {"type", "ack"},
                {"msg_id", msg.msg_id},
                {"status", "sent"},
                {"server_timestamp", duration_cast<milliseconds>(
                    system_clock::now().time_since_epoch()
                ).count()}
            };

            Message ack_msg;
            ack_msg.msg_id = msg.msg_id;
            ack_msg.content = ack.dump();
            ack_msg.to_user_id = msg.from_user_id;

            ws_manager_.sendToUser(msg.from_user_id, ack_msg);
        }
    }

    void queueMessage(const Message& msg) {
        // Store in Redis offline queue
        redis_.lpush("offline_queue:" + msg.to_user_id, msg.toJson().dump());

        messages_queued_++;

        std::cout << "Queued message for offline user: " << msg.to_user_id << std::endl;
    }

    void sendPushNotification(const Message& msg) {
        // Send to APNs (iOS) or FCM (Android)
        json notification = {
            {"user_id", msg.to_user_id},
            {"title", "New message from " + msg.from_user_id},
            {"body", msg.content.substr(0, 50)},  // Preview
            {"badge", 1}
        };

        // Push to notification service (async)
        std::cout << "Sent push notification to " << msg.to_user_id << std::endl;
    }

    std::vector<std::string> getGroupMembers(const std::string& group_id) {
        // Query PostgreSQL for group members
        std::string query = R"(
            SELECT user_id FROM group_members
            WHERE group_id = ? AND left_at IS NULL
        )";

        auto results = db_.query(query, group_id);

        std::vector<std::string> members;
        for (const auto& row : results) {
            members.push_back(row["user_id"]);
        }

        return members;
    }
};
```

</details>

</details>


### 7.4 Complete WhatsApp System

<details>
<summary>Complete WhatsApp System</summary>

<details>
<summary>WhatsAppSystem Class</summary>

```cpp
class WhatsAppSystem {
private:
    WebSocketConnectionManager ws_manager_;
    MessageRouter message_router_;
    DatabaseConnection db_;
    RedisClient redis_;

public:
    WhatsAppSystem(int ws_port)
        : ws_manager_(ws_port),
          db_("cassandra://localhost:9042"),
          redis_("redis://localhost:6379"),
          message_router_(ws_manager_, db_, redis_) {}

    void start() {
        std::cout << "=== Starting WhatsApp System ===" << std::endl;

        // Start message router
        message_router_.start(10);  // 10 worker threads

        // Register message callback
        ws_manager_.setMessageCallback([this](const Message& msg) {
            // Route incoming messages
            message_router_.routeMessage(msg);
        });

        // Start WebSocket server (blocking)
        ws_manager_.run();
    }

    void stop() {
        message_router_.stop();
        ws_manager_.stop();
    }

    void printStats() {
        std::cout << "\n=== WhatsApp System Stats ===" << std::endl;
        std::cout << "Active connections: " << ws_manager_.getConnectionCount() << std::endl;
        message_router_.printStats();
    }
};

// Example usage
int main() {
    WhatsAppSystem whatsapp(9090);

    // Start in separate thread to allow stats printing
    std::thread server_thread([&whatsapp]() {
        whatsapp.start();
    });

    // Run for demo
    std::this_thread::sleep_for(std::chrono::seconds(60));

    whatsapp.printStats();
    whatsapp.stop();

    if (server_thread.joinable()) {
        server_thread.join();
    }

    return 0;
}
```

</details>

</details>


***

## Step 8: Advanced Features

### 8.1 Message Retry \& Acknowledgment

<details>
<summary>Message Retry Manager</summary>

<details>
<summary>MessageRetryManager Class</summary>

```cpp
class MessageRetryManager {
private:
    struct PendingMessage {
        Message msg;
        system_clock::time_point sent_at;
        int retry_count;
    };

    std::unordered_map<std::string, PendingMessage> pending_messages_;
    std::mutex mtx_;

    const int MAX_RETRIES = 3;
    const seconds RETRY_INTERVAL{5};

    std::thread retry_thread_;
    std::atomic<bool> running_{false};

public:
    void start() {
        running_ = true;

        retry_thread_ = std::thread([this]() {
            retryLoop();
        });
    }

    void stop() {
        running_ = false;
        if (retry_thread_.joinable()) {
            retry_thread_.join();
        }
    }

    void addPendingMessage(const Message& msg) {
        std::lock_guard<std::mutex> lock(mtx_);

        pending_messages_[msg.msg_id] = {
            msg,
            system_clock::now(),
            0
        };
    }

    void markDelivered(const std::string& msg_id) {
        std::lock_guard<std::mutex> lock(mtx_);
        pending_messages_.erase(msg_id);
    }

private:
    void retryLoop() {
        while (running_) {
            std::this_thread::sleep_for(RETRY_INTERVAL);

            std::lock_guard<std::mutex> lock(mtx_);

            auto now = system_clock::now();
            std::vector<std::string> to_remove;

            for (auto& [msg_id, pending] : pending_messages_) {
                auto elapsed = duration_cast<seconds>(now - pending.sent_at);

                if (elapsed >= RETRY_INTERVAL) {
                    if (pending.retry_count < MAX_RETRIES) {
                        // Retry sending
                        std::cout << "Retrying message " << msg_id
                                 << " (attempt " << (pending.retry_count + 1) << ")" << std::endl;

                        // Re-send message (would call router)
                        pending.sent_at = now;
                        pending.retry_count++;
                    } else {
                        // Max retries exceeded
                        std::cout << "Message " << msg_id << " failed after "
                                 << MAX_RETRIES << " retries" << std::endl;
                        to_remove.push_back(msg_id);
                    }
                }
            }

            for (const auto& msg_id : to_remove) {
                pending_messages_.erase(msg_id);
            }
        }
    }
};
```

</details>

</details>


### 8.2 Typing Indicator

<details>
<summary>Typing Indicator Manager</summary>

<details>
<summary>TypingIndicatorManager Class</summary>

```cpp
class TypingIndicatorManager {
private:
    RedisClient& redis_;

    const seconds TYPING_TTL{5};  // Typing status expires after 5 seconds

public:
    TypingIndicatorManager(RedisClient& redis) : redis_(redis) {}

    void setTyping(const std::string& chat_id, const std::string& user_id) {
        std::string key = "typing:" + chat_id;

        // Add user to typing set with TTL
        redis_.setex(key + ":" + user_id, "1", TYPING_TTL);

        // Notify other participants in chat
        broadcastTypingStatus(chat_id, user_id, true);
    }

    void clearTyping(const std::string& chat_id, const std::string& user_id) {
        std::string key = "typing:" + chat_id;

        redis_.del(key + ":" + user_id);

        broadcastTypingStatus(chat_id, user_id, false);
    }

    std::vector<std::string> getTypingUsers(const std::string& chat_id) {
        std::string pattern = "typing:" + chat_id + ":*";

        auto keys = redis_.keys(pattern);

        std::vector<std::string> typing_users;
        for (const auto& key : keys) {
            // Extract user_id from key
            size_t pos = key.rfind(':');
            if (pos != std::string::npos) {
                typing_users.push_back(key.substr(pos + 1));
            }
        }

        return typing_users;
    }

private:
    void broadcastTypingStatus(const std::string& chat_id,
                              const std::string& user_id,
                              bool is_typing) {
        json notification = {
            {"type", "typing"},
            {"chat_id", chat_id},
            {"user_id", user_id},
            {"is_typing", is_typing}
        };

        // Broadcast to all chat participants
        // (Would integrate with WebSocket manager)
    }
};
```

</details>

</details>


***

## Step 9: Bottlenecks \& Optimizations

### Bottleneck 1: Group Message Fan-Out

**Problem:** 256-member group = 256 deliveries per message

**Solution: Lazy Fan-Out**

<details>
<summary>Optimized Group Router</summary>

<details>
<summary>OptimizedGroupRouter Class</summary>

```cpp
class OptimizedGroupRouter {
public:
    void routeGroupMessage(const Message& msg, const std::vector<std::string>& members) {
        // Store message once in group table
        storeInGroupTable(msg);

        // For online members: Send pointer to message
        for (const auto& member_id : members) {
            if (isOnline(member_id)) {
                sendMessagePointer(member_id, msg.msg_id);
            } else {
                // Offline: Queue notification only
                queueNotification(member_id, msg.msg_id);
            }
        }
    }

private:
    void sendMessagePointer(const std::string& user_id, const std::string& msg_id) {
        json pointer = {
            {"type", "message_pointer"},
            {"msg_id", msg_id},
            {"fetch_from", "group_messages_table"}
        };

        // Client fetches actual message when needed
    }
};

// Result: 256 small pointers instead of 256 full messages
// Bandwidth: 256 × 50 bytes = 12.8 KB (vs 256 × 1 KB = 256 KB)
// 20x improvement!
```

</details>

</details>


### Bottleneck 2: Hot Shards (Celebrity Accounts)

**Problem:** One user with 100M followers creates hot shard

**Solution: Consistent Hashing with Virtual Nodes**

<details>
<summary>Consistent Hash Router</summary>

<details>
<summary>ConsistentHashRouter Class</summary>

```cpp
class ConsistentHashRouter {
private:
    const int VIRTUAL_NODES = 150;
    std::map<uint64_t, std::string> hash_ring_;

public:
    void addServer(const std::string& server_id) {
        for (int i = 0; i < VIRTUAL_NODES; ++i) {
            std::string virtual_id = server_id + "#" + std::to_string(i);
            uint64_t hash = hashFunction(virtual_id);
            hash_ring_[hash] = server_id;
        }
    }

    std::string getServer(const std::string& user_id) {
        uint64_t hash = hashFunction(user_id);

        // Find next server on ring
        auto it = hash_ring_.lower_bound(hash);

        if (it == hash_ring_.end()) {
            return hash_ring_.begin()->second;
        }

        return it->second;
    }
};

// Celebrity account distributed across multiple virtual nodes
// Load spread more evenly
```

</details>

</details>


### Bottleneck 3: Database Write Hotspots

**Problem:** 1.62M writes/sec overwhelms database

**Solution: Write-Behind Cache + Batching**

<details>
<summary>Write Behind Cache</summary>

<details>
<summary>WriteBehindCache Class</summary>

```cpp
class WriteBehindCache {
private:
    std::vector<Message> write_buffer_;
    std::mutex buffer_mtx_;
    const size_t BATCH_SIZE = 1000;
    const seconds FLUSH_INTERVAL{5};

    std::thread flush_thread_;
    std::atomic<bool> running_{false};

public:
    void write(const Message& msg) {
        std::lock_guard<std::mutex> lock(buffer_mtx_);

        write_buffer_.push_back(msg);

        if (write_buffer_.size() >= BATCH_SIZE) {
            flush();
        }
    }

    void start() {
        running_ = true;

        flush_thread_ = std::thread([this]() {
            while (running_) {
                std::this_thread::sleep_for(FLUSH_INTERVAL);
                flush();
            }
        });
    }

private:
    void flush() {
        std::vector<Message> to_flush;

        {
            std::lock_guard<std::mutex> lock(buffer_mtx_);
            to_flush = std::move(write_buffer_);
            write_buffer_.clear();
        }

        if (to_flush.empty()) return;

        // Batch insert to database
        batchInsert(to_flush);

        std::cout << "Flushed " << to_flush.size() << " messages to DB" << std::endl;
    }

    void batchInsert(const std::vector<Message>& messages) {
        // Single batch INSERT with multiple rows
        // Much faster than individual INSERTs
    }
};

// Result: 1.62M individual writes → 1,620 batch writes (1000x reduction)
```

</details>

</details>


***

## Summary: Key Decisions

| Aspect | Decision | Rationale |
| :-- | :-- | :-- |
| **Real-time Protocol** | WebSocket | Bidirectional, low latency |
| **Backend Language** | Erlang/BEAM (real WhatsApp) | Lightweight processes, 2M connections/server [^3] |
| **Message Storage** | Cassandra | Time-series, horizontal scaling |
| **Real-time State** | Redis | Online status, typing, fast lookups |
| **Media Storage** | S3 + CDN | Cost-effective, global delivery |
| **Delivery Guarantee** | At-least-once | Prevent message loss |
| **Group Fan-Out** | Push-based | Real-time experience |
| **Encryption** | Signal Protocol | End-to-end security |

**Performance Characteristics:**

```
Scale (2025):
- Users: 3 billion monthly [web:275]
- Messages: 140 billion/day [web:275]
- Peak: 5M messages/sec

Latency:
- Message delivery: <200ms (P95)
- WebSocket connection: <100ms
- Group message (256 members): <500ms

Connections:
- Per server: 50K (C++), 2M (Erlang) [web:277]
- Total servers: 10K-30K

Storage:
- Messages (30 days): 336 TB
- Media (30 days): 387 PB
- Total: ~390 PB

Availability: 99.99%
Message delivery success: 99.9%
```

**WhatsApp vs Alternatives:**


| Feature | WhatsApp | Telegram | Signal | Slack |
| :-- | :-- | :-- | :-- | :-- |
| **E2EE** | ✅ Default | ❌ Optional | ✅ Default | ❌ Enterprise only |
| **Group Size** | 256 | 200K | 1000 | Unlimited |
| **Media Sharing** | ✅ Rich | ✅ Rich | ✅ Basic | ✅ Rich |
| **Message Sync** | ✅ Multi-device | ✅ Cloud | ✅ Limited | ✅ Full |
| **Backend** | Erlang | C++ | Java | PHP + Go |

This design handles **3 billion users** and **140 billion messages/day** with **<200ms delivery latency** using WebSocket, Erlang, Cassandra, and distributed architecture! 🚀
<span style="display:none">[^10][^11][^12][^13][^14][^15][^16][^17][^18][^19][^4][^5][^6][^7][^8][^9]</span>

<div align="center">⁂</div>

[^1]: https://meetanshi.com/blog/whatsapp-statistics/

[^2]: https://wanotifier.com/whatsapp-statistics/

[^3]: https://getstream.io/blog/whatsapp-works/

[^4]: https://backlinko.com/whatsapp-users

[^5]: https://www.verloop.io/blog/whatsapp-statistics-2025/

[^6]: https://www.statista.com/statistics/272014/global-social-networks-ranked-by-number-of-users/

[^7]: https://worldpopulationreview.com/country-rankings/whatsapp-users-by-country

[^8]: https://developers.facebook.com/docs/whatsapp/messaging-limits/

[^9]: https://www.interakt.shop/blog/increase-whatsapp-messaging-sending-limit/

[^10]: https://www.geeksforgeeks.org/system-design/how-whatsapp-handles-50-billion-messages-a-day/

[^11]: https://explodingtopics.com/blog/messaging-apps-stats

[^12]: https://restaurant.eatapp.co/blog/whatsapp-quality-rating-howto

[^13]: https://blog.bytebytego.com/p/how-whatsapp-handles-40-billion-messages

[^14]: https://analyzify.com/statsup/whatsapp

[^15]: https://chatarmin.com/en/blog/whatsapp-message-count

[^16]: https://highscalability.com/designing-whatsapp/

[^17]: https://www.cometchat.com/blog/whatsapps-architecture-and-system-design

[^18]: https://www.geeksforgeeks.org/system-design/designing-whatsapp-messenger-system-design/

[^19]: https://www.linkedin.com/posts/xianxu_how-whatsapp-works-activity-7373117106411909120-lWIo

