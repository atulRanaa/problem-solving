# Ticket Booking System — focused on race conditions, concurrency control, and data consistency

Below is a compact but thorough system-design guide that emphasizes how to prevent race conditions, guarantee correct concurrency behavior, and keep data consistent in a high-throughput distributed ticket booking system (concerts, flights, cinemas, etc.). I include architecture, concrete control options, tradeoffs, sample algorithms/SQL/pseudocode, failure/recovery patterns, and testing/observability suggestions.

---

# 1 — High level architecture (components)

* **API Gateway / Edge** — rate limiting, auth, routing.
* **Booking Service (write path)** — accepts booking requests, enforces business rules.
* **Inventory / Seats Service** — authoritative seat store; responsible for lock/hold/commit semantics.
* **Payment Service** — handles payments asynchronously; ideally an external provider.
* **Read/View Service (Query)** — fast read-only seat maps, availability (can be eventually consistent).
* **Cache (Redis / Memcached)** — for hot reads and seat-hold state.
* **Event Bus (Kafka / Pulsar)** — to publish booking events, decouple services, enable eventual consistency.
* **Relational DB (Postgres / MySQL) or NewSQL** — authoritative transactional store for critical operations.
* **Worker / Scheduler** — clears expired holds, processes retries, runs comp actions.
* **Monitoring / Tracing / Alerting** — metrics, logs, distributed tracing.

Design pattern: **CQRS** (separate read/write). Writes go to booking/inventory services; reads use denormalized read models updated via events.

---

# 2 — Booking flow (typical)

1. User requests to reserve seats.
2. Booking Service checks availability and *places a hold* on seats (short TTL).
3. Service returns a `reservation_token` to client; UI proceeds to payment.
4. Client calls Payment Service with `reservation_token`.
5. On successful payment, Booking Service **confirms** (commits) the reservation: seat status → `booked`.
6. If payment fails or TTL expires, hold is released; seats go back to `available`.

Critical points for race conditions: steps 2 (placing hold) and 5 (confirming) are the race-prone operations.

---

# 3 — Concurrency-control options (practical strategies)

### 3.1 Optimistic Concurrency Control (OCC)

* Store a `version` (or `last_updated_at`) per row.
* Update with conditional clause:
  `UPDATE seats SET status='booked', version = version + 1 WHERE seat_id = ? AND version = ?;`
  If `rows_affected == 0` → conflict, retry or fail.
* Pros: high throughput, no long locks.
* Cons: high conflict/retry cost under contention.

### 3.2 Pessimistic Locking (DB-level, SELECT FOR UPDATE)

* `SELECT * FROM seats WHERE seat_id IN (...) FOR UPDATE;` inside a DB transaction.
* Guarantees serialization for that transaction; prevents concurrent updates until commit.
* Pros: simple, strong correctness.
* Cons: reduces concurrency, risk of deadlocks and blocking; not great for extreme scale.

### 3.3 Distributed Locks (Redis / Zookeeper / etcd)

* Acquire lock per seat/row/partition using `SETNX` or RedLock pattern.
* Use short TTL and renewals. On lock success, change seat state.
* Pros: can coordinate across services/nodes.
* Cons: complexity, failure modes (clock drift, split-brain), performance overhead.

### 3.4 Application-level Queuing / Single-writer per partition

* Partition seats (e.g., by row, screen, flight) and run a single writer for each partition (actor model). Use a queue (Kafka partition, or in-memory single-threaded worker).
* Pros: serializes access without DB locks; high throughput if partitioning is good.
* Cons: hotspots if one partition gets heavy traffic.

### 3.5 Serializable Isolation (DB)

* Use strict serializable transactions (Postgres `SERIALIZABLE`) to let DB detect anomalies.
* Pros: strong guarantees.
* Cons: higher abort/retry rate and latency.

---

# 4 — Recommended hybrid strategy (practical, robust)

For most production ticket systems:

1. **Fast path (reads / UI seat map):** serve from a read model (cache / read DB) updated asynchronously.
2. **Reserve/Hold seats:** use **optimistic locking** with a short-lived *hold* in Redis and a DB record:

   * Write a `seat_hold` entry in Redis (SET with TTL) and attempt DB OCC update:

     ```sql
     UPDATE seats
     SET status='hold', hold_token=$token, version = version + 1
     WHERE seat_id = $id AND status='available' AND version = $v;
     ```
   * If DB update succeeds, you have a hold; else release Redis hold and inform client.
3. **Confirm booking (after payment):** use a transaction to move `hold` → `booked`. Use DB ACID transaction on authoritative DB. Publish events.
4. **Hold expiry:** a worker watches Redis TTL expirations or scans DB for expired holds and releases seats.
5. **Idempotency:** every client operation must include an `idempotency_key` to prevent duplicate bookings.

Why hybrid? Redis for speed + TTL + workers for expiry, DB for authoritative linearizable updates. OCC reduces lock contention; Redis avoids heavy DB reads under load.

---

# 5 — Preventing double-booking — concrete techniques

### A — Per-seat conditional update (SQL OCC)

```sql
-- Attempt to hold seat
UPDATE seats
SET status='hold', hold_token=$token, hold_expires_at=$t, version = version + 1
WHERE seat_id = $id AND status = 'available' AND version = $v;
```

Check affected rows.

### B — SELECT FOR UPDATE (when consistency required)

```sql
BEGIN;
SELECT status FROM seats WHERE seat_id = $id FOR UPDATE;
-- check status
UPDATE seats SET status='hold', hold_token=..., hold_expires_at=... WHERE seat_id = $id;
COMMIT;
```

### C — Redis SETNX with DB confirmation

1. `SETNX seat:{seat_id} {token} PX 120000`  — acquire temporary lock in Redis.
2. If success, attempt DB update; if DB update fails, delete Redis key.
3. On commit, extend TTL or delete Redis lock and mark booked in DB.

Note: Do **not** rely solely on Redis; it’s cache/lock, DB must be source of truth.

---

# 6 — Atomicity between booking and payment

Payments may take time. Avoid committing seats until payment is confirmed — use a **reservation_token**:

* Steps:

  1. Create hold and issue `reservation_token` atomically.
  2. Payment calls reference token; Payment Service returns success via webhook/callback.
  3. Booking Service commits booking on payment success.
  4. If payment does not arrive before TTL, auto-release hold.

For higher guarantees, use SAGA (see below).

---

# 7 — Distributed transactions and SAGA

* **Two-phase commit (2PC)** is fragile at scale (blocking, single coordinator). **Avoid** for cross-service heavy paths.
* **Use SAGA** (choreography or orchestration):

  * Each step is a local transaction. On failure, run compensating actions.
  * Example: hold seat (local commit), attempt payment (local commit). If payment fails, compensating transaction: release hold.
* Record SAGA state (persistent log) to make process resumable and idempotent.

---

# 8 — Data models (minimal)

Seats table (Postgres):

```sql
CREATE TABLE seats (
  seat_id UUID PRIMARY KEY,
  event_id UUID,
  seat_label TEXT,
  status TEXT CHECK (status IN ('available','hold','booked')),
  hold_token UUID NULL,
  hold_expires_at TIMESTAMP NULL,
  version INT DEFAULT 0,
  last_updated TIMESTAMP DEFAULT now()
);
```

Reservations table:

```sql
CREATE TABLE reservations (
  reservation_id UUID PRIMARY KEY,
  user_id UUID,
  event_id UUID,
  seats JSONB, -- or normalized reserved_seats table
  status TEXT CHECK (status IN ('pending','confirmed','cancelled')),
  idempotency_key TEXT,
  created_at TIMESTAMP DEFAULT now()
);
```

---

# 9 — Edge cases, failure modes & how to handle them

### 9.1 Payment succeeds but commit fails (e.g., DB down)

* Use durable event log: record payment success event and retry commit when DB recovers.
* Notify user if final status is delayed; ensure automatic retries and idempotency.

### 9.2 DB commit succeeds but webhook to payment fails

* Claim: seat is booked in DB. If user doesn't receive confirmation, they can query reservation by idempotency key. Don't reverse.

### 9.3 Multiple concurrent clients selecting same seats

* Use OCC + retries (with exponential backoff) and clear UI updates to show seats taken.

### 9.4 Expired holds not released (worker crash)

* Periodic sweep job: `UPDATE seats SET status='available' WHERE status='hold' AND hold_expires_at < now();`
* Reconciliation: run daily/real-time audits comparing DB vs cache vs events.

### 9.5 Clock skew and TTLs

* Use consistent time source (DB server time or NTP) when evaluating expiry. Prefer DB timestamps for authoritative expiry.

---

# 10 — Performance & scaling strategies

* **Partition seats by event**: each event independent; shard DB/tables by `event_id`.
* **Hot partitions**: for extremely popular events, further partition by seat blocks or use per-partition single-writer service.
* **Read scaling**: use read replicas and denormalized read models (materialized views) refreshed from event stream.
* **Cache**: cache availability but always validate on write.
* **Backpressure**: rate-limit booking attempts per event to avoid overload during flash sales.

---

# 11 — Testing & validation (to uncover race conditions)

* **Concurrency/stress tests:** run thousands of concurrent booking attempts for same seats—assert no double-booking.
* **Chaos testing:** kill services, simulate network partitions, DB failover.
* **Property-based tests:** model seat states and invariants (a seat cannot be `booked` by two reservations).
* **Integration tests with payment emulator** and delayed responses.
* **Load testing on holds & sweeper** — ensure sweeper can keep up.

---

# 12 — Observability & metrics (must-have)

* Metrics: `holds_created`, `holds_expired`, `bookings_confirmed`, `booking_conflicts`, `db_transaction_retries`, `lock_acquires`, `lock_timeouts`.
* Tracing: trace transaction from API → Booking Service → Payment → DB commit.
* Alerts: high `booking_conflicts` spike, long transaction latency, sweeper backlog.
* Logs: include `reservation_token`, `idempotency_key` for debugging.

---

# 13 — Example pseudocode: optimistic hold + confirm

```python
# Attempt to hold seats (client supplies seat_ids and idempotency_key)
def try_hold(user_id, seat_ids, idempotency_key):
    token = uuid4()
    now = db.now()
    expires = now + HOLD_TTL
    # read current versions for seats (fast SELECT)
    seats = db.query("SELECT seat_id, status, version FROM seats WHERE seat_id IN (...)")
    for s in seats:
        if s.status != 'available':
            return {"ok": False, "reason": "not_available"}
    # try conditional update per seat (batch is better)
    for s in seats:
        rows = db.execute("""
            UPDATE seats
            SET status='hold', hold_token=%s, hold_expires_at=%s, version = version + 1
            WHERE seat_id=%s AND status='available' AND version=%s
        """, (token, expires, s.seat_id, s.version))
        if rows == 0:
            # conflict; rollback changes we made for prior seats
            db.execute("DELETE FROM seats WHERE hold_token=%s", (token,))
            return {"ok": False, "reason": "conflict"}
    # create reservation record
    db.insert("INSERT INTO reservations (...) VALUES (...)", ...)
    cache.set(f"hold:{token}", ..., ttl=HOLD_TTL)
    return {"ok": True, "reservation_token": token}
```

Confirm (post payment):

```python
def confirm(reservation_token, payment_info):
    # idempotent: check reservation status first
    reservation = db.get_reservation_by_token(reservation_token)
    if reservation.status == 'confirmed':
        return {"ok": True}
    # attempt to charge payment, then inside DB transaction
    if charge(payment_info):
        db.begin()
        # change seats with hold_token -> booked
        db.execute("""
          UPDATE seats SET status='booked', hold_token=NULL, hold_expires_at=NULL
          WHERE hold_token=%s
        """, (reservation_token,))
        db.execute("UPDATE reservations SET status='confirmed' WHERE token=%s", (reservation_token,))
        db.commit()
        event_bus.publish("reservation_confirmed", {...})
        return {"ok": True}
    else:
        # payment failed -> release hold
        release_hold(reservation_token)
        return {"ok": False, "reason": "payment_failed"}
```

---

# 14 — Tradeoffs summary (short)

* **Strict correctness**: Pessimistic locks / SERIALIZABLE → correct but low throughput and higher latency.
* **High throughput**: OCC + Redis holds + SAGA → fast but requires careful retries and handling of edge cases.
* **Cross-service strong transactions**: 2PC → avoid at scale; use SAGA choreography with durable logs.
* **Read UX**: show "likely available" based on cached read model but always confirm before payment.

---

# 15 — Quick checklist to implement safely

* [ ] Use idempotency keys for create/confirm operations.
* [ ] Use hold with TTL, persisted in both cache and DB.
* [ ] Use conditional DB updates (OCC) or FOR UPDATE for critical sections.
* [ ] Use Redis locks cautiously (RedLock) if needed.
* [ ] Implement a reliable sweeper for expired holds.
* [ ] Ensure payment + booking commit is durable and idempotent (SAGA pattern).
* [ ] Add thorough concurrency/stress tests and chaos experiments.
* [ ] Monitor conflict metrics and tune lock TTLs, retry policies.
* [ ] Partition hot events to reduce contention.

---

If you want, I can now:

* produce a **sequence diagram (ASCII)** for the full flow,
* generate **sample SQL + stored-procedure** implementations for holds and confirms,
* or produce a **detailed design** tuned for a specific scale (e.g., 10k TPS vs 100 TPS) including caching and partitioning strategy.

Which of those would you like next?
