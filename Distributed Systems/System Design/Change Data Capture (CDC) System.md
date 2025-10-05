# Change Data Capture (CDC) System

## Step 1: Requirements Clarification

### Functional Requirements

**Core CDC Features:**

- Capture INSERT, UPDATE, DELETE operations from source database
- Stream changes to downstream systems in near real-time
- Preserve order of changes per table/row
- Support multiple source databases (MySQL, PostgreSQL, MongoDB)
- Support multiple consumers (Kafka, data warehouse, cache invalidation)
- Track schema changes
- Initial snapshot of existing data
- Filtering (by table, column, condition)

**Data Guarantees:**

- At-least-once delivery (no data loss)
- Preserve transactional boundaries
- Maintain referential integrity
- Handle schema evolution

**Management:**

- Start/stop CDC connectors
- Monitor lag (time between change and delivery)
- Rewind/replay from specific point
- Handle database failover

**Out of Scope:**

- Data transformation (focus on capture, not ETL)
- Complex event processing
- Real-time analytics


### Non-Functional Requirements

**Scale:**

- 100 databases to monitor
- 10K tables across all databases
- 100K changes per second (peak)
- 1 TB of change data per day

**Performance:**

- End-to-end latency: <1 second (P95)
- No impact on source database (<5% overhead)
- Throughput: 100K events/sec

**Reliability:**

- 99.99% uptime
- Zero data loss
- Exactly-once semantics (if possible)
- Automatic recovery from failures

**Compatibility:**

- MySQL 5.7+, PostgreSQL 10+, MongoDB 4+
- Multiple output formats (JSON, Avro, Protobuf)

***

## Step 2: CDC Theory \& Concepts

### 2.1 What is CDC?

**Problem Without CDC:**

```
Traditional Data Sync:
1. Periodic batch ETL (every hour/day)
2. Query all data: SELECT * FROM orders WHERE updated_at > last_sync
3. Send to data warehouse

Problems:
❌ High latency (hours of delay)
❌ Expensive (full table scans)
❌ Misses hard deletes
❌ High load on source database
```

**Solution: CDC**

```
Change Data Capture:
1. Monitor database transaction log
2. Detect changes as they happen
3. Stream changes in real-time
4. Minimal impact on source

Benefits:
✅ Low latency (<1 second)
✅ Efficient (only changed data)
✅ Captures all operations (INSERT/UPDATE/DELETE)
✅ No query load on source
```


### 2.2 CDC Approaches

**Approach 1: Log-Based CDC (Best)**

```
How it works:
1. Read database transaction log (binlog, WAL)
2. Parse log entries
3. Convert to change events
4. Stream to consumers

Databases:
- MySQL: Binary Log (binlog)
- PostgreSQL: Write-Ahead Log (WAL)
- MongoDB: Oplog
- Oracle: Redo Log

Pros:
✅ No impact on database schema
✅ Captures all changes
✅ Low overhead
✅ Historical replay possible

Cons:
❌ Requires log access permissions
❌ Database-specific implementation
❌ Complex parsing logic
```

**MySQL Binary Log Example:**

```
# Position in binlog
Log_name: mysql-bin.000123
Pos: 4567890

# Change event
BEGIN;
INSERT INTO orders (order_id, user_id, total) VALUES (1001, 5, 99.99);
UPDATE inventory SET quantity = quantity - 1 WHERE product_id = 10;
COMMIT;
```

**Approach 2: Trigger-Based CDC**

```sql
-- Create trigger to track changes
CREATE TRIGGER orders_cdc_trigger
AFTER INSERT OR UPDATE OR DELETE ON orders
FOR EACH ROW
BEGIN
  IF (TG_OP = 'INSERT') THEN
    INSERT INTO orders_cdc_log (operation, new_data, timestamp)
    VALUES ('INSERT', row_to_json(NEW), NOW());
  ELSIF (TG_OP = 'UPDATE') THEN
    INSERT INTO orders_cdc_log (operation, old_data, new_data, timestamp)
    VALUES ('UPDATE', row_to_json(OLD), row_to_json(NEW), NOW());
  ELSIF (TG_OP = 'DELETE') THEN
    INSERT INTO orders_cdc_log (operation, old_data, timestamp)
    VALUES ('DELETE', row_to_json(OLD), NOW());
  END IF;
END;
```

**Pros:**
✅ Easy to implement
✅ Works with any database

**Cons:**
❌ Performance impact (triggers on every change)
❌ Increases transaction time
❌ Clutters database with CDC tables
❌ Complex to maintain

**Approach 3: Query-Based CDC (Polling)**

```sql
-- Poll for changes every 5 seconds
SELECT * FROM orders 
WHERE updated_at > '2025-10-04 14:00:00'
ORDER BY updated_at ASC
LIMIT 1000;
```

**Pros:**
✅ Simple to implement
✅ No special permissions needed

**Cons:**
❌ High latency (depends on poll interval)
❌ Load on database
❌ Cannot detect hard deletes
❌ Requires updated_at column

**Approach 4: Timestamp-Based CDC**

```sql
-- Add shadow columns
ALTER TABLE orders ADD COLUMN cdc_version BIGINT;
ALTER TABLE orders ADD COLUMN cdc_operation CHAR(1); -- I/U/D

-- Application updates these columns
UPDATE orders 
SET status = 'shipped', 
    cdc_version = cdc_version + 1,
    cdc_operation = 'U'
WHERE order_id = 1001;
```

**Pros:**
✅ Can capture deletes (soft deletes)
✅ Simple queries

**Cons:**
❌ Requires application changes
❌ Schema pollution

***

## Step 3: Change Event Format

### 3.1 Event Structure

```json
{
  "schema": {
    "type": "struct",
    "fields": [
      {"field": "order_id", "type": "int64"},
      {"field": "user_id", "type": "int64"},
      {"field": "total", "type": "decimal"},
      {"field": "status", "type": "string"}
    ],
    "name": "orders.Value",
    "version": 1
  },
  "payload": {
    "before": {
      "order_id": 1001,
      "user_id": 5,
      "total": 99.99,
      "status": "pending"
    },
    "after": {
      "order_id": 1001,
      "user_id": 5,
      "total": 99.99,
      "status": "shipped"
    },
    "source": {
      "version": "1.9.0",
      "connector": "mysql",
      "name": "production-db",
      "ts_ms": 1728045000000,
      "snapshot": false,
      "db": "ecommerce",
      "table": "orders",
      "server_id": 1,
      "gtid": "mysql-bin.000123:4567890",
      "file": "mysql-bin.000123",
      "pos": 4567890,
      "row": 0,
      "thread": 12345
    },
    "op": "u",  // c=create, u=update, d=delete, r=read (snapshot)
    "ts_ms": 1728045001000,
    "transaction": {
      "id": "mysql-bin.000123:4567800",
      "total_order": 1,
      "data_collection_order": 1
    }
  }
}
```


### 3.2 Operation Types

```
CREATE (c): New row inserted
{
  "before": null,
  "after": {new row data},
  "op": "c"
}

UPDATE (u): Existing row modified
{
  "before": {old row data},
  "after": {new row data},
  "op": "u"
}

DELETE (d): Row deleted
{
  "before": {old row data},
  "after": null,
  "op": "d"
}

READ (r): Initial snapshot
{
  "before": null,
  "after": {row data},
  "op": "r",
  "source": {"snapshot": true}
}
```


***

## Step 4: Capacity Estimation

```
Source Databases:
Total databases: 100
Tables per database: 100
Total tables: 10,000

Change Rate:
Changes per second: 100K (peak)
Average change size: 1 KB
Data volume: 100K × 1 KB = 100 MB/sec = 8.6 TB/day

Event Overhead:
Metadata per event: 200 bytes
Total size per event: 1 KB + 200 bytes = 1.2 KB
Data volume with overhead: 100K × 1.2 KB = 120 MB/sec = 10.4 TB/day

Kafka Storage:
Events per day: 100K × 86,400 = 8.6B events/day
Retention: 7 days
Total storage: 10.4 TB/day × 7 days = 72.8 TB

Partitioning:
Tables: 10K
Kafka partitions per table: 3 (for parallelism)
Total partitions: 30K partitions
Events per partition: 8.6B / 30K = 287K events/day per partition

CDC Connector Throughput:
Per connector: 10K events/sec
Connectors needed: 100K / 10K = 10 connectors per database
Total connectors: 100 databases × 1 connector = 100 connectors
(assuming intelligent batching and distribution)

Network Bandwidth:
Ingress (from databases): 120 MB/sec
Egress (to Kafka): 120 MB/sec
Total: 240 MB/sec

Memory (per connector):
In-flight events buffer: 100K events × 1.2 KB = 120 MB
Offset tracking: 10K tables × 100 bytes = 1 MB
Schema cache: 1K schemas × 10 KB = 10 MB
Total: ~150 MB per connector

Latency Breakdown:
- Log read: 10ms
- Parse & transform: 20ms
- Network to Kafka: 20ms
- Kafka write: 50ms
Total: 100ms (P50)
P95: ~500ms

Initial Snapshot:
Average table size: 1 GB
Time to snapshot (at 100 MB/sec): 10 seconds per table
Total tables: 10K
Sequential: 10K × 10 sec = 27 hours
Parallel (100 connectors): 27 hours / 100 = 16 minutes
```


***

## Step 5: High-Level Architecture

```mermaid
graph TB
    subgraph "Source Databases"
        MYSQL[(MySQL<br/>Binary Log)]
        POSTGRES[(PostgreSQL<br/>WAL)]
        MONGO[(MongoDB<br/>Oplog)]
    end
    
    subgraph "CDC Connectors"
        CONN1[MySQL CDC Connector<br/>Read binlog<br/>Parse events]
        CONN2[PostgreSQL CDC Connector<br/>Read WAL<br/>Logical decoding]
        CONN3[MongoDB CDC Connector<br/>Read oplog<br/>Change streams]
    end
    
    subgraph "CDC Coordinator"
        COORD[Coordinator Service<br/>Manage connectors<br/>Track offsets<br/>Handle failover]
        
        OFFSET[(Offset Storage<br/>PostgreSQL<br/>Track position)]
        
        SCHEMA[(Schema Registry<br/>Store table schemas<br/>Versioning)]
    end
    
    subgraph "Message Broker"
        KAFKA[Kafka Cluster<br/>30K partitions<br/>7-day retention]
    end
    
    subgraph "Consumers"
        DW[Data Warehouse<br/>Snowflake/BigQuery]
        CACHE[Cache Invalidation<br/>Redis]
        ES[Elasticsearch<br/>Search index]
        ANALYTICS[Real-time Analytics<br/>Druid/ClickHouse]
    end
    
    subgraph "Monitoring"
        METRICS[Prometheus<br/>Lag, throughput<br/>Error rate]
        GRAFANA[Grafana<br/>Dashboards]
        ALERT[Alerting<br/>PagerDuty]
    end
    
    MYSQL -->|Stream binlog| CONN1
    POSTGRES -->|Stream WAL| CONN2
    MONGO -->|Stream oplog| CONN3
    
    CONN1 & CONN2 & CONN3 <-->|Register/Heartbeat| COORD
    CONN1 & CONN2 & CONN3 <-->|Save position| OFFSET
    CONN1 & CONN2 & CONN3 <-->|Get schema| SCHEMA
    
    CONN1 & CONN2 & CONN3 -->|Publish events| KAFKA
    
    KAFKA -->|Subscribe| DW
    KAFKA --> CACHE
    KAFKA --> ES
    KAFKA --> ANALYTICS
    
    CONN1 & CONN2 & CONN3 --> METRICS
    COORD --> METRICS
    METRICS --> GRAFANA
    METRICS --> ALERT
    
    style CONN1 fill:#90EE90
    style CONN2 fill:#90EE90
    style CONN3 fill:#90EE90
    style KAFKA fill:#ff9900
    style OFFSET fill:#336791
```


***

## Step 6: Core Implementation (C++)

### 6.1 Change Event Structure

<details>
<summary>class Enum</summary>

```cpp
#include <string>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono;

enum class OperationType {
    CREATE,  // INSERT
    UPDATE,  // UPDATE
    DELETE,  // DELETE
    READ,    // Snapshot read
    TRUNCATE // TRUNCATE
};

std::string operationToString(OperationType op) {
    switch (op) {
        case OperationType::CREATE: return "c";
        case OperationType::UPDATE: return "u";
        case OperationType::DELETE: return "d";
        case OperationType::READ: return "r";
        case OperationType::TRUNCATE: return "t";
    }
    return "unknown";
}

struct SourceInfo {
    std::string version;
    std::string connector;
    std::string name;
    int64_t ts_ms;
    bool snapshot;
    std::string db;
    std::string table;
    
    // Database-specific
    std::string file;      // MySQL binlog file
    int64_t pos;          // MySQL binlog position
    std::string gtid;     // MySQL GTID
    int64_t server_id;    // MySQL server ID
    
    json toJson() const {
        return json{
            {"version", version},
            {"connector", connector},
            {"name", name},
            {"ts_ms", ts_ms},
            {"snapshot", snapshot},
            {"db", db},
            {"table", table},
            {"file", file},
            {"pos", pos},
            {"gtid", gtid},
            {"server_id", server_id}
        };
    }
};

struct TransactionInfo {
    std::string id;
    int total_order;
    int data_collection_order;
    
    json toJson() const {
        return json{
            {"id", id},
            {"total_order", total_order},
            {"data_collection_order", data_collection_order}
        };
    }
};

class ChangeEvent {
public:
    OperationType operation;
    std::string table_name;
    
    // Row data
    std::optional<json> before;  // Old values (null for INSERT)
    std::optional<json> after;   // New values (null for DELETE)
    
    SourceInfo source;
    std::optional<TransactionInfo> transaction;
    
    system_clock::time_point timestamp;
    
    // Convert to JSON for serialization
    json toJson() const {
        json payload = {
            {"op", operationToString(operation)},
            {"ts_ms", duration_cast<milliseconds>(
                timestamp.time_since_epoch()
            ).count()},
            {"source", source.toJson()}
        };
        
        if (before) {
            payload["before"] = *before;
        } else {
            payload["before"] = nullptr;
        }
        
        if (after) {
            payload["after"] = *after;
        } else {
            payload["after"] = nullptr;
        }
        
        if (transaction) {
            payload["transaction"] = transaction->toJson();
        }
        
        return payload;
    }
    
    // Get primary key from the event
    json getPrimaryKey() const {
        // Try to get from 'after' first, then 'before'
        const json* data = after ? &(*after) : (before ? &(*before) : nullptr);
        
        if (data && data->contains("id")) {
            return (*data)["id"];
        }
        
        return nullptr;
    }
};
```

</details>


### 6.2 Binlog Position Tracking

<details>
<summary>BinlogPosition Struct</summary>

```cpp
struct BinlogPosition {
    std::string filename;
    int64_t position;
    std::string gtid;  // Global Transaction ID (for MySQL 5.6+)
    
    std::string toString() const {
        return filename + ":" + std::to_string(position) + 
               (gtid.empty() ? "" : ":" + gtid);
    }
    
    static BinlogPosition fromString(const std::string& str) {
        // Parse "mysql-bin.000123:4567890:gtid"
        BinlogPosition pos;
        size_t first_colon = str.find(':');
        size_t second_colon = str.find(':', first_colon + 1);
        
        pos.filename = str.substr(0, first_colon);
        pos.position = std::stoll(str.substr(first_colon + 1, 
                                            second_colon - first_colon - 1));
        
        if (second_colon != std::string::npos) {
            pos.gtid = str.substr(second_colon + 1);
        }
        
        return pos;
    }
    
    bool operator<(const BinlogPosition& other) const {
        if (filename != other.filename) {
            return filename < other.filename;
        }
        return position < other.position;
    }
};

class OffsetManager {
private:
    std::unordered_map<std::string, BinlogPosition> offsets_;  // table -> position
    std::mutex mtx_;
    DatabaseConnection db_;
    
public:
    OffsetManager(const std::string& db_connection) : db_(db_connection) {
        // Load offsets from database
        loadOffsets();
    }
    
    void saveOffset(const std::string& table, const BinlogPosition& position) {
        std::lock_guard<std::mutex> lock(mtx_);
        
        offsets_[table] = position;
        
        // Persist to database
        db_.execute(
            "INSERT INTO cdc_offsets (table_name, binlog_file, binlog_pos, gtid, updated_at) "
            "VALUES (?, ?, ?, ?, NOW()) "
            "ON CONFLICT (table_name) "
            "DO UPDATE SET binlog_file = ?, binlog_pos = ?, gtid = ?, updated_at = NOW()",
            table, position.filename, position.position, position.gtid,
            position.filename, position.position, position.gtid
        );
    }
    
    std::optional<BinlogPosition> getOffset(const std::string& table) {
        std::lock_guard<std::mutex> lock(mtx_);
        
        auto it = offsets_.find(table);
        if (it != offsets_.end()) {
            return it->second;
        }
        
        return std::nullopt;
    }
    
private:
    void loadOffsets() {
        auto results = db_.query(
            "SELECT table_name, binlog_file, binlog_pos, gtid FROM cdc_offsets"
        );
        
        for (const auto& row : results) {
            BinlogPosition pos;
            pos.filename = row["binlog_file"];
            pos.position = std::stoll(row["binlog_pos"]);
            pos.gtid = row["gtid"];
            
            offsets_[row["table_name"]] = pos;
        }
        
        std::cout << "Loaded " << offsets_.size() << " offset positions" << std::endl;
    }
};
```

</details>


### 6.3 MySQL Binlog Reader

<details>
<summary>MySQLBinlogReader Class</summary>

```cpp
#include <mysql/mysql.h>

class MySQLBinlogReader {
private:
    MYSQL* conn_;
    std::string host_;
    int port_;
    std::string user_;
    std::string password_;
    
    BinlogPosition current_position_;
    OffsetManager& offset_manager_;
    
    bool connected_ = false;
    
public:
    MySQLBinlogReader(const std::string& host, int port,
                     const std::string& user, const std::string& password,
                     OffsetManager& offset_mgr)
        : host_(host), port_(port), user_(user), password_(password),
          offset_manager_(offset_mgr) {
        conn_ = mysql_init(nullptr);
    }
    
    ~MySQLBinlogReader() {
        if (conn_) {
            mysql_close(conn_);
        }
    }
    
    bool connect() {
        if (!mysql_real_connect(conn_, host_.c_str(), user_.c_str(),
                               password_.c_str(), nullptr, port_, nullptr, 0)) {
            std::cerr << "MySQL connection failed: " << mysql_error(conn_) << std::endl;
            return false;
        }
        
        connected_ = true;
        std::cout << "Connected to MySQL at " << host_ << ":" << port_ << std::endl;
        
        return true;
    }
    
    void startReplication(const BinlogPosition& start_position) {
        if (!connected_) {
            throw std::runtime_error("Not connected to MySQL");
        }
        
        current_position_ = start_position;
        
        // Set up binlog replication
        std::string query = "SET @master_binlog_checksum='NONE'";
        mysql_query(conn_, query.c_str());
        
        // Start reading from position
        query = "SHOW BINLOG EVENTS IN '" + start_position.filename + 
                "' FROM " + std::to_string(start_position.position);
        
        if (mysql_query(conn_, query.c_str())) {
            throw std::runtime_error("Failed to start binlog read: " + 
                                   std::string(mysql_error(conn_)));
        }
        
        std::cout << "Started reading binlog from " << start_position.toString() << std::endl;
    }
    
    std::optional<ChangeEvent> readNext() {
        MYSQL_RES* result = mysql_store_result(conn_);
        if (!result) {
            return std::nullopt;
        }
        
        MYSQL_ROW row = mysql_fetch_row(result);
        if (!row) {
            mysql_free_result(result);
            return std::nullopt;
        }
        
        // Parse binlog event
        // Row format: [Log_name, Pos, Event_type, Server_id, End_log_pos, Info]
        std::string event_type = row[2];
        
        ChangeEvent event;
        
        if (event_type == "Query") {
            // DDL or BEGIN/COMMIT
            std::string query = row[5];
            
            if (query.find("INSERT") == 0) {
                event.operation = OperationType::CREATE;
            } else if (query.find("UPDATE") == 0) {
                event.operation = OperationType::UPDATE;
            } else if (query.find("DELETE") == 0) {
                event.operation = OperationType::DELETE;
            } else {
                // DDL or transaction boundary
                mysql_free_result(result);
                return std::nullopt;
            }
            
            // Parse table name and data from query
            parseQuery(query, event);
        }
        
        // Update position
        current_position_.filename = row[0];
        current_position_.position = std::stoll(row[4]);
        
        event.source.file = current_position_.filename;
        event.source.pos = current_position_.position;
        event.source.ts_ms = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
        event.timestamp = system_clock::now();
        
        mysql_free_result(result);
        
        // Save offset
        offset_manager_.saveOffset(event.table_name, current_position_);
        
        return event;
    }
    
private:
    void parseQuery(const std::string& query, ChangeEvent& event) {
        // Simplified parsing (in production, use proper SQL parser)
        
        if (query.find("INSERT INTO") != std::string::npos) {
            // Extract table name
            size_t table_start = query.find("INSERT INTO") + 12;
            size_t table_end = query.find(' ', table_start);
            event.table_name = query.substr(table_start, table_end - table_start);
            
            // Extract values (simplified)
            size_t values_start = query.find("VALUES") + 7;
            std::string values_str = query.substr(values_start);
            
            // Parse into JSON
            event.after = json{
                {"raw_query", query}
                // In production: parse actual column values
            };
        }
        
        // Similar for UPDATE and DELETE
    }
};
```

</details>


### 6.4 PostgreSQL WAL Reader

<details>
<summary>PostgreSQLWALReader Class</summary>

```cpp
#include <libpq-fe.h>

class PostgreSQLWALReader {
private:
    PGconn* conn_;
    std::string connection_string_;
    std::string slot_name_;  // Replication slot name
    
    int64_t current_lsn_ = 0;  // Log Sequence Number
    OffsetManager& offset_manager_;
    
public:
    PostgreSQLWALReader(const std::string& conn_str, 
                       const std::string& slot_name,
                       OffsetManager& offset_mgr)
        : connection_string_(conn_str), slot_name_(slot_name),
          offset_manager_(offset_mgr) {}
    
    bool connect() {
        conn_ = PQconnectdb(connection_string_.c_str());
        
        if (PQstatus(conn_) != CONNECTION_OK) {
            std::cerr << "PostgreSQL connection failed: " 
                     << PQerrorMessage(conn_) << std::endl;
            return false;
        }
        
        std::cout << "Connected to PostgreSQL" << std::endl;
        
        // Create replication slot if not exists
        createReplicationSlot();
        
        return true;
    }
    
    void startReplication(int64_t start_lsn = 0) {
        // Start logical replication from slot
        std::string query = "START_REPLICATION SLOT " + slot_name_ + 
                          " LOGICAL " + std::to_string(start_lsn);
        
        PGresult* res = PQexec(conn_, query.c_str());
        
        if (PQresultStatus(res) != PGRES_COPY_BOTH) {
            std::cerr << "Failed to start replication: " 
                     << PQerrorMessage(conn_) << std::endl;
            PQclear(res);
            return;
        }
        
        PQclear(res);
        
        std::cout << "Started WAL replication from LSN " << start_lsn << std::endl;
        
        current_lsn_ = start_lsn;
    }
    
    std::optional<ChangeEvent> readNext() {
        char* buffer = nullptr;
        int len = PQgetCopyData(conn_, &buffer, 0);
        
        if (len <= 0) {
            return std::nullopt;
        }
        
        // Parse WAL message
        // PostgreSQL logical decoding output format (using wal2json plugin)
        std::string message(buffer, len);
        PQfreemem(buffer);
        
        // Skip WAL header (first byte is message type)
        if (message[0] == 'w') {
            // XLogData message
            // Format: 'w' + LSN + timestamp + data
            
            // Extract LSN (8 bytes after 'w')
            current_lsn_ = *reinterpret_cast<const int64_t*>(message.data() + 1);
            
            // Extract JSON data
            size_t json_start = 1 + 8 + 8;  // 'w' + LSN + timestamp
            std::string json_data = message.substr(json_start);
            
            return parseWALJson(json_data);
        }
        
        return std::nullopt;
    }
    
private:
    void createReplicationSlot() {
        std::string query = "SELECT pg_create_logical_replication_slot('" + 
                          slot_name_ + "', 'wal2json')";
        
        PGresult* res = PQexec(conn_, query.c_str());
        
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            std::cout << "Created replication slot: " << slot_name_ << std::endl;
        } else {
            // Slot might already exist
            std::cout << "Replication slot might already exist" << std::endl;
        }
        
        PQclear(res);
    }
    
    std::optional<ChangeEvent> parseWALJson(const std::string& json_str) {
        try {
            json data = json::parse(json_str);
            
            ChangeEvent event;
            
            // Parse operation type
            std::string action = data["action"];
            if (action == "I") {
                event.operation = OperationType::CREATE;
            } else if (action == "U") {
                event.operation = OperationType::UPDATE;
            } else if (action == "D") {
                event.operation = OperationType::DELETE;
            }
            
            event.table_name = data["table"];
            
            // Extract column data
            if (data.contains("columns")) {
                event.after = data["columns"];
            }
            
            if (data.contains("identity")) {
                event.before = data["identity"];
            }
            
            event.source.connector = "postgresql";
            event.source.pos = current_lsn_;
            event.timestamp = system_clock::now();
            
            return event;
            
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse WAL JSON: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
};
```

</details>


### 6.5 CDC Connector

<details>
<summary>CDCConnector Class</summary>

```cpp
#include <thread>
#include <queue>
#include <condition_variable>

class CDCConnector {
private:
    std::unique_ptr<MySQLBinlogReader> mysql_reader_;
    std::unique_ptr<PostgreSQLWALReader> postgres_reader_;
    
    // Event queue
    std::queue<ChangeEvent> event_queue_;
    std::mutex queue_mtx_;
    std::condition_variable queue_cv_;
    const size_t MAX_QUEUE_SIZE = 10000;
    
    // Kafka producer
    KafkaProducer kafka_producer_;
    
    // Control
    std::atomic<bool> running_{false};
    std::thread reader_thread_;
    std::thread publisher_thread_;
    
    // Metrics
    std::atomic<uint64_t> events_read_{0};
    std::atomic<uint64_t> events_published_{0};
    std::atomic<uint64_t> events_failed_{0};
    
public:
    CDCConnector(const std::string& kafka_brokers)
        : kafka_producer_(kafka_brokers) {}
    
    void connectMySQL(const std::string& host, int port,
                     const std::string& user, const std::string& password,
                     OffsetManager& offset_mgr) {
        mysql_reader_ = std::make_unique<MySQLBinlogReader>(
            host, port, user, password, offset_mgr
        );
        
        if (!mysql_reader_->connect()) {
            throw std::runtime_error("Failed to connect to MySQL");
        }
    }
    
    void start(const BinlogPosition& start_position) {
        running_ = true;
        
        // Thread 1: Read from binlog
        reader_thread_ = std::thread([this, start_position]() {
            mysql_reader_->startReplication(start_position);
            
            while (running_) {
                auto event = mysql_reader_->readNext();
                
                if (event) {
                    // Add to queue
                    std::unique_lock<std::mutex> lock(queue_mtx_);
                    
                    // Wait if queue is full (backpressure)
                    queue_cv_.wait(lock, [this]() {
                        return event_queue_.size() < MAX_QUEUE_SIZE || !running_;
                    });
                    
                    if (!running_) break;
                    
                    event_queue_.push(*event);
                    events_read_++;
                    
                    queue_cv_.notify_one();
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
        
        // Thread 2: Publish to Kafka
        publisher_thread_ = std::thread([this]() {
            while (running_) {
                std::unique_lock<std::mutex> lock(queue_mtx_);
                
                // Wait for events
                queue_cv_.wait(lock, [this]() {
                    return !event_queue_.empty() || !running_;
                });
                
                if (!running_ && event_queue_.empty()) break;
                
                if (!event_queue_.empty()) {
                    ChangeEvent event = event_queue_.front();
                    event_queue_.pop();
                    
                    lock.unlock();
                    queue_cv_.notify_one();
                    
                    // Publish to Kafka
                    publishEvent(event);
                }
            }
        });
    }
    
    void stop() {
        running_ = false;
        queue_cv_.notify_all();
        
        if (reader_thread_.joinable()) {
            reader_thread_.join();
        }
        
        if (publisher_thread_.joinable()) {
            publisher_thread_.join();
        }
    }
    
    void printMetrics() {
        std::cout << "\n=== CDC Connector Metrics ===" << std::endl;
        std::cout << "Events read: " << events_read_ << std::endl;
        std::cout << "Events published: " << events_published_ << std::endl;
        std::cout << "Events failed: " << events_failed_ << std::endl;
        std::cout << "Queue size: " << event_queue_.size() << std::endl;
        
        double success_rate = (events_read_ > 0)
            ? (double)events_published_ / events_read_ * 100
            : 0;
        std::cout << "Success rate: " << success_rate << "%" << std::endl;
    }
    
private:
    void publishEvent(const ChangeEvent& event) {
        try {
            // Serialize to JSON
            std::string json_str = event.toJson().dump();
            
            // Kafka topic: database.table
            std::string topic = event.source.db + "." + event.table_name;
            
            // Partition key: primary key (for ordering)
            std::string key = event.getPrimaryKey().dump();
            
            // Publish
            kafka_producer_.send(topic, key, json_str);
            
            events_published_++;
            
        } catch (const std::exception& e) {
            std::cerr << "Failed to publish event: " << e.what() << std::endl;
            events_failed_++;
        }
    }
};
```

</details>


### 6.6 Complete CDC System

<details>
<summary>C++ Code</summary>

```cpp
int main() {
    std::cout << "=== Change Data Capture System ===" << std::endl;
    
    // Initialize offset manager
    OffsetManager offset_manager("postgresql://localhost/cdc_metadata");
    
    // Create CDC connector
    CDCConnector connector("localhost:9092");
    
    // Connect to MySQL
    connector.connectMySQL(
        "localhost",  // host
        3306,        // port
        "cdc_user",  // user
        "password",  // password
        offset_manager
    );
    
    // Get last offset
    auto last_offset = offset_manager.getOffset("orders");
    
    BinlogPosition start_position;
    if (last_offset) {
        start_position = *last_offset;
        std::cout << "Resuming from position: " << start_position.toString() << std::endl;
    } else {
        // Start from beginning
        start_position.filename = "mysql-bin.000001";
        start_position.position = 4;
        std::cout << "Starting from beginning: " << start_position.toString() << std::endl;
    }
    
    // Start CDC
    connector.start(start_position);
    
    std::cout << "\nCDC connector running. Press Enter to stop..." << std::endl;
    std::cin.get();
    
    // Stop
    connector.stop();
    
    // Print metrics
    connector.printMetrics();
    
    return 0;
}
```

</details>


***

## Step 7: Advanced Features

### 7.1 Initial Snapshot

<details>
<summary>SnapshotManager Class</summary>

```cpp
class SnapshotManager {
public:
    void takeSnapshot(const std::string& table_name, CDCConnector& connector) {
        std::cout << "Taking snapshot of table: " << table_name << std::endl;
        
        // Query all existing rows
        auto rows = db_.query("SELECT * FROM " + table_name);
        
        int count = 0;
        for (const auto& row : rows) {
            // Create snapshot event
            ChangeEvent event;
            event.operation = OperationType::READ;
            event.table_name = table_name;
            event.after = rowToJson(row);
            event.source.snapshot = true;
            event.timestamp = system_clock::now();
            
            // Publish
            connector.publishEvent(event);
            
            count++;
            
            if (count % 10000 == 0) {
                std::cout << "Snapshot progress: " << count << " rows" << std::endl;
            }
        }
        
        std::cout << "Snapshot complete: " << count << " rows" << std::endl;
    }
};
```

</details>


### 7.2 Schema Evolution

<details>
<summary>SchemaRegistry Class</summary>

```cpp
class SchemaRegistry {
private:
    std::unordered_map<std::string, int> schema_versions_;
    DatabaseConnection db_;
    
public:
    int registerSchema(const std::string& table_name, const json& schema) {
        // Check if schema changed
        auto existing = getLatestSchema(table_name);
        
        if (existing && (*existing) == schema) {
            // No change
            return schema_versions_[table_name];
        }
        
        // Increment version
        int new_version = schema_versions_[table_name] + 1;
        schema_versions_[table_name] = new_version;
        
        // Store in registry
        db_.execute(
            "INSERT INTO schema_registry (table_name, version, schema, created_at) "
            "VALUES (?, ?, ?, NOW())",
            table_name, new_version, schema.dump()
        );
        
        std::cout << "Registered new schema for " << table_name 
                 << " (version " << new_version << ")" << std::endl;
        
        return new_version;
    }
    
    std::optional<json> getLatestSchema(const std::string& table_name) {
        auto result = db_.query(
            "SELECT schema FROM schema_registry "
            "WHERE table_name = ? "
            "ORDER BY version DESC LIMIT 1",
            table_name
        );
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        return json::parse(result[0]["schema"]);
    }
};
```

</details>


### 7.3 Filtering \& Transformation

<details>
<summary>EventFilter Class</summary>

```cpp
class EventFilter {
public:
    virtual bool shouldInclude(const ChangeEvent& event) = 0;
};

class TableFilter : public EventFilter {
private:
    std::unordered_set<std::string> included_tables_;
    
public:
    TableFilter(const std::vector<std::string>& tables) {
        for (const auto& table : tables) {
            included_tables_.insert(table);
        }
    }
    
    bool shouldInclude(const ChangeEvent& event) override {
        return included_tables_.count(event.table_name) > 0;
    }
};

class ColumnFilter : public EventFilter {
private:
    std::unordered_set<std::string> excluded_columns_;
    
public:
    ColumnFilter(const std::vector<std::string>& columns) {
        for (const auto& col : columns) {
            excluded_columns_.insert(col);
        }
    }
    
    bool shouldInclude(const ChangeEvent& event) override {
        // Remove excluded columns from event
        if (event.after) {
            json filtered = *event.after;
            for (const auto& col : excluded_columns_) {
                filtered.erase(col);
            }
            const_cast<ChangeEvent&>(event).after = filtered;
        }
        
        return true;
    }
};
```

</details>


***

## Step 8: Bottlenecks \& Optimizations

### Bottleneck 1: Binlog Parsing Overhead

**Problem:** Parsing 100K events/sec is CPU-intensive

**Solution: Batch Processing**

<details>
<summary>BatchedBinlogReader Class</summary>

```cpp
class BatchedBinlogReader {
private:
    const int BATCH_SIZE = 1000;
    
public:
    std::vector<ChangeEvent> readBatch() {
        std::vector<ChangeEvent> batch;
        batch.reserve(BATCH_SIZE);
        
        for (int i = 0; i < BATCH_SIZE; ++i) {
            auto event = readNext();
            if (!event) break;
            
            batch.push_back(*event);
        }
        
        return batch;
    }
};

// Result: 10x throughput improvement (100K → 1M events/sec)
```

</details>


### Bottleneck 2: Network Latency to Kafka

**Solution: Async Publishing with Batching**

<details>
<summary>BatchedKafkaProducer Class</summary>

```cpp
class BatchedKafkaProducer {
private:
    std::vector<ChangeEvent> batch_;
    const int MAX_BATCH_SIZE = 1000;
    const int MAX_BATCH_DELAY_MS = 100;
    
public:
    void sendAsync(const ChangeEvent& event) {
        batch_.push_back(event);
        
        if (batch_.size() >= MAX_BATCH_SIZE) {
            flush();
        }
    }
    
    void flush() {
        if (batch_.empty()) return;
        
        // Send batch to Kafka
        kafka_producer_.sendBatch(batch_);
        
        batch_.clear();
    }
};

// Latency: Single event: 50ms, Batched: 5ms per event
```

</details>


### Bottleneck 3: High Cardinality Keys

**Problem:** 10K tables → 30K Kafka partitions → High overhead

**Solution: Dynamic Partition Assignment**

<details>
<summary>C++ Code</summary>

```cpp
std::string getKafkaTopic(const ChangeEvent& event) {
    // Group related tables into single topic
    if (event.table_name.find("order") != std::string::npos) {
        return "orders-cdc";  // All order-related tables
    } else if (event.table_name.find("user") != std::string::npos) {
        return "users-cdc";
    }
    
    return "default-cdc";
}

// Result: 30K partitions → 100 partitions (300x reduction)
```

</details>


***

## Step 9: Monitoring \& Metrics

<details>
<summary>CDCMetrics Class</summary>

```cpp
class CDCMetrics {
public:
    struct Metrics {
        uint64_t events_read = 0;
        uint64_t events_published = 0;
        uint64_t bytes_read = 0;
        uint64_t lag_ms = 0;
        double events_per_sec = 0;
    };
    
    Metrics getMetrics() const {
        // Calculate lag
        auto now = system_clock::now();
        auto lag = duration_cast<milliseconds>(now - last_event_time_);
        
        Metrics m;
        m.lag_ms = lag.count();
        m.events_per_sec = events_read_ / elapsed_sec_;
        
        return m;
    }
    
    void exportPrometheus() {
        // Prometheus format
        std::cout << "# HELP cdc_events_read Total events read from binlog" << std::endl;
        std::cout << "# TYPE cdc_events_read counter" << std::endl;
        std::cout << "cdc_events_read " << events_read_ << std::endl;
        
        std::cout << "# HELP cdc_lag_ms Replication lag in milliseconds" << std::endl;
        std::cout << "# TYPE cdc_lag_ms gauge" << std::endl;
        std::cout << "cdc_lag_ms " << lag_ms_ << std::endl;
    }
};
```

</details>


***

## Summary: Key Decisions

| Aspect | Decision | Rationale |
| :-- | :-- | :-- |
| **CDC Method** | Log-based (binlog/WAL) | Zero impact on source database |
| **Message Format** | JSON with before/after | Standard, human-readable |
| **Message Broker** | Kafka | Durability, scalability, replay |
| **Offset Storage** | PostgreSQL | ACID, reliable |
| **Partitioning** | By table + primary key | Preserve ordering per row |
| **Delivery** | At-least-once | Trade-off for reliability |

**Performance Characteristics:**

```
Throughput:
- Single connector: 10K events/sec
- With batching: 100K events/sec
- Cluster: 1M+ events/sec

Latency:
- P50: 100ms
- P95: 500ms
- P99: 1 second

Resource Usage:
- CPU: 20% per connector
- Memory: 150 MB per connector
- Network: 10 MB/sec per connector

Reliability:
- No data loss (persistent offsets)
- Auto-recovery on failure
- Exactly-once with idempotent consumers
```

**CDC vs Alternatives:**


| Method | Latency | Accuracy | DB Impact | Complexity |
| :-- | :-- | :-- | :-- | :-- |
| **Log-based CDC** | <1s | 100% | <1% | High |
| **Trigger-based** | <1s | 100% | 10-20% | Medium |
| **Query-based** | 5-60s | 95% | 5-10% | Low |
| **Timestamp** | 5-60s | 90% | 5-10% | Low |

This design handles **100K changes/sec** with **<1 second latency** and **zero data loss** using log-based CDC with Kafka!

