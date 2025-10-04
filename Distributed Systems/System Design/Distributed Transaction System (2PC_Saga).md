# Distributed Transaction System (2PC/Saga)

## Step 1: Requirements Clarification

### Functional Requirements

**E-commerce Order Placement Scenario:**

- Create order (Order Service)
- Reserve inventory (Inventory Service)
- Process payment (Payment Service)
- Update loyalty points (Loyalty Service)
- Send notification (Notification Service)

**Transaction Requirements:**

- **Atomicity**: All steps succeed or all rollback
- **Consistency**: System remains in valid state
- **Isolation**: Concurrent transactions don't interfere
- **Durability**: Committed changes persist

**Out of Scope:**

- Read-only queries (focus on write transactions)
- Single database transactions (trivial with local ACID)


### Non-Functional Requirements

**Performance:**

- Transaction throughput: 1000 TPS
- Latency: <500ms for 2PC, <1s for Saga (P99)
- Coordinator availability: 99.99%

**Consistency:**

- Strong consistency (2PC)
- Eventual consistency acceptable (Saga)

**Fault Tolerance:**

- Survive participant failures
- Survive coordinator failures
- Handle network partitions

***

## Step 2: Distributed Transaction Theory

### 2.1 ACID Properties in Distributed Systems

**Single Database (Easy):**

```sql
BEGIN TRANSACTION;
  INSERT INTO orders VALUES (...);
  UPDATE inventory SET quantity = quantity - 1;
  INSERT INTO payments VALUES (...);
COMMIT;  -- All or nothing (atomicity guaranteed by DB)
```

**Distributed System (Hard):**

```
Service 1 (Orders):      INSERT INTO orders ...
Service 2 (Inventory):   UPDATE inventory ...
Service 3 (Payment):     INSERT INTO payments ...

Problem: What if Payment fails? How to rollback Orders and Inventory?
```


### 2.2 CAP Theorem

**Fundamental Theorem:**

- **Consistency (C)**: All nodes see same data
- **Availability (A)**: Every request gets response (success/failure)
- **Partition Tolerance (P)**: System works despite network failures

**You can only have 2 out of 3!**

```
        Consistency
           /  \
          /    \
         /      \
    CA /        \ CP
      /          \
     /    AP      \
Availability ---- Partition Tolerance

CA: Single datacenter (no partition tolerance)
CP: Distributed databases (sacrifice availability during partition)
AP: NoSQL systems (eventual consistency)
```

**Real-World Trade-offs:**


| System | Choice | Reason |
| :-- | :-- | :-- |
| Banking | CP | Consistency critical, can sacrifice availability |
| Social Media | AP | Availability critical, eventual consistency OK |
| E-commerce | Depends | Product catalog (AP), Payments (CP) |

### 2.3 Two Generals Problem

**Fundamental Problem in Distributed Systems:**

```
General A (on hill 1)  -->  [UNRELIABLE CHANNEL]  -->  General B (on hill 2)
                                                         
Goal: Coordinate attack at dawn
Problem: How to ensure both attack simultaneously?

Message sequence:
1. A → B: "Attack at dawn"  (might get lost)
2. B → A: "ACK received"     (might get lost)
3. A → B: "ACK of ACK"       (might get lost)
...infinite ACKs needed!

Conclusion: Perfect consensus impossible with unreliable communication
```

**Implications:**

- Cannot guarantee distributed transactions with 100% certainty
- Must choose between consistency and availability
- Need timeout mechanisms


### 2.4 Byzantine Generals Problem

**More Complex Scenario:**

```
Multiple generals need to coordinate
Some generals might be traitors (send conflicting messages)

General 1: "Attack"
General 2: "Attack"
General 3 (traitor): Tells G1 "Attack", tells G2 "Retreat"

How to reach consensus despite malicious actors?
```

**Solutions:**

- Byzantine Fault Tolerance (BFT) algorithms
- Blockchain consensus (Proof of Work, Proof of Stake)
- 2PC/Saga assume non-Byzantine faults (crashes, not malice)

***

## Step 3: Two-Phase Commit (2PC) Protocol

### 3.1 Theory \& Algorithm

**Participants:**

- **Transaction Coordinator (TC)**: Orchestrates the transaction
- **Participants**: Services involved in transaction (Order, Inventory, Payment)

**Phases:**

**Phase 1: PREPARE (Voting Phase)**

```
Coordinator → Participants: "Can you commit transaction X?"

Participants respond:
- YES (vote to commit) - resources locked, ready to commit
- NO (vote to abort) - cannot complete transaction
```

**Phase 2: COMMIT/ABORT (Decision Phase)**

```
If ALL participants vote YES:
  Coordinator → Participants: "COMMIT"
  Participants: Execute commit, release locks
  
If ANY participant votes NO:
  Coordinator → Participants: "ABORT"
  Participants: Rollback changes, release locks
```


### 3.2 State Diagram

```
Coordinator States:
INIT → WAIT → COMMIT/ABORT → END

Participant States:
INIT → READY → COMMIT/ABORT → END

State Transitions:
┌─────────┐  Prepare   ┌─────────┐
│  INIT   │ ────────→  │  READY  │
└─────────┘            └─────────┘
                            ↓
                   ┌────────┴────────┐
                   ↓                 ↓
              ┌─────────┐      ┌─────────┐
              │ COMMIT  │      │  ABORT  │
              └─────────┘      └─────────┘
```


### 3.3 Message Flow

```
Timeline:

T0: Client → Coordinator: Begin transaction
T1: Coordinator → All participants: PREPARE
T2: Participant 1 → Coordinator: YES
T3: Participant 2 → Coordinator: YES
T4: Participant 3 → Coordinator: YES
T5: Coordinator → All participants: COMMIT
T6: Participant 1 → Coordinator: COMMITTED
T7: Participant 2 → Coordinator: COMMITTED
T8: Participant 3 → Coordinator: COMMITTED
T9: Coordinator → Client: Transaction complete

Latency: 2 round trips (PREPARE + COMMIT)
```


### 3.4 Failure Scenarios

**Scenario 1: Participant Crashes After PREPARE**

```
T0: Coordinator → Participants: PREPARE
T1: P1 → Coordinator: YES
T2: P2 → Coordinator: YES
T3: P3 CRASHES (no response)

Timeout:
T4: Coordinator decides to ABORT
T5: Coordinator → P1, P2: ABORT

Result: Transaction aborted (safe)
```

**Scenario 2: Coordinator Crashes After PREPARE**

```
T0: Coordinator → Participants: PREPARE
T1: All participants: YES (in READY state, locks held)
T2: Coordinator CRASHES before sending COMMIT

Problem: Participants blocked indefinitely!

Solution: Timeout + Query other participants
- If any participant aborted → ABORT
- If any participant committed → COMMIT
- If all uncertain → Still blocked (worst case)
```

**Scenario 3: Network Partition**

```
         Coordinator
          /    |    \
         /     |     \
        P1    P2     P3
              |
         PARTITION HERE

P2 cannot communicate with Coordinator
P2 is blocked in READY state

Result: Reduced availability (CP system)
```


### 3.5 Problems with 2PC

**1. Blocking Protocol:**

- Participants locked during PREPARE phase
- If coordinator crashes, participants blocked indefinitely

**2. Single Point of Failure:**

- Coordinator crash → entire transaction blocked

**3. Performance:**

- Synchronous, high latency (2 round trips)
- Locks held for long time (reduced concurrency)

**4. Not Suitable for:**

- Long-running transactions
- Cross-organization transactions
- High-latency networks

***

## Step 4: Three-Phase Commit (3PC) Protocol

### 4.1 Improvement Over 2PC

**Goal:** Eliminate blocking in 2PC

**Three Phases:**

**Phase 1: CAN-COMMIT (Voting)**

```
Coordinator → Participants: "Can you commit?"
Participants → Coordinator: YES/NO
```

**Phase 2: PRE-COMMIT (Prepare)**

```
If all YES:
  Coordinator → Participants: "PRE-COMMIT"
  Participants: Lock resources, acknowledge
  
This phase makes decision known before final commit
```

**Phase 3: DO-COMMIT (Commit)**

```
If all PRE-COMMIT acknowledged:
  Coordinator → Participants: "DO-COMMIT"
  Participants: Actually commit and release locks
```


### 4.2 Advantage

```
Key difference: PRE-COMMIT phase allows recovery

If coordinator crashes after PRE-COMMIT:
- Participants know decision was COMMIT
- New coordinator can complete transaction
- No indefinite blocking!

Timeline:
CAN-COMMIT → PRE-COMMIT → DO-COMMIT
   ↓            ↓            ↓
Can abort   Must commit  Committed
```

**Trade-off:**

- ✅ Non-blocking (better availability)
- ❌ 3 round trips (worse latency)
- ❌ Not partition-tolerant (can commit on both sides of partition)

***

## Step 5: Saga Pattern

### 5.1 Theory \& Motivation

**Problems with 2PC:**

- Locks held too long
- Not suitable for microservices
- Coordinator SPOF
- Poor performance

**Saga Solution:**

- Split transaction into sequence of local transactions
- Each step has compensating transaction (undo)
- Eventual consistency (not immediate)

**Key Idea:**

```
Instead of:
  BEGIN
    Step 1
    Step 2
    Step 3
  COMMIT or ROLLBACK all

Use:
  Execute Step 1 → Commit locally
  Execute Step 2 → Commit locally
  Execute Step 3 → Commit locally
  
  If Step 3 fails:
    Compensate Step 2 (undo)
    Compensate Step 1 (undo)
```


### 5.2 Saga Execution Patterns

**Forward Recovery (Retry Until Success):**

```
T1 → T2 → T3 (fails) → Retry T3 → T3 (success)
```

**Backward Recovery (Compensate):**

```
T1 → T2 → T3 (fails) → C2 (compensate T2) → C1 (compensate T1)
```


### 5.3 Compensating Transactions

**Requirements:**

- **Idempotent**: Can be executed multiple times safely
- **Retryable**: Will eventually succeed
- **Semantically meaningful**: Actually undoes the action

**Examples:**

```
Action: Reserve inventory (qty - 1)
Compensation: Release inventory (qty + 1)

Action: Debit account ($100)
Compensation: Credit account ($100)

Action: Send email
Compensation: Cannot undo! (Saga limitation)
           Workaround: Send "cancellation" email
```


### 5.4 Saga Coordination Patterns

**A) Choreography (Event-Driven)**

```
Each service listens to events and knows what to do next

Order Service:
  1. Create order
  2. Emit "OrderCreated" event

Inventory Service:
  1. Listen to "OrderCreated"
  2. Reserve inventory
  3. Emit "InventoryReserved" or "InventoryFailed"

Payment Service:
  1. Listen to "InventoryReserved"
  2. Process payment
  3. Emit "PaymentProcessed" or "PaymentFailed"

If "PaymentFailed":
  Inventory Service: Listen and compensate (release inventory)
  Order Service: Listen and compensate (cancel order)
```

**Pros:**

- ✅ No central coordinator (no SPOF)
- ✅ Services loosely coupled
- ✅ Good for simple workflows

**Cons:**

- ❌ Hard to track transaction state
- ❌ Cyclic dependencies possible
- ❌ Testing is complex

**B) Orchestration (Centralized Coordinator)**

```
Saga Orchestrator (central coordinator):

execute():
  1. Call Order Service: CreateOrder()
  2. If success: Call Inventory Service: ReserveInventory()
  3. If success: Call Payment Service: ProcessPayment()
  4. If success: Call Notification Service: SendEmail()
  
compensate():
  If any step fails:
    Call previous services with compensation
    Payment Service: RefundPayment()
    Inventory Service: ReleaseInventory()
    Order Service: CancelOrder()
```

**Pros:**

- ✅ Centralized state management
- ✅ Easy to track and monitor
- ✅ Clear workflow definition

**Cons:**

- ❌ Single point of failure (orchestrator)
- ❌ Orchestrator can become complex


### 5.5 Saga vs 2PC Comparison

| Aspect | Two-Phase Commit (2PC) | Saga Pattern |
| :-- | :-- | :-- |
| **Consistency** | Strong (ACID) | Eventual |
| **Latency** | High (blocking) | Low (async) |
| **Isolation** | Full (locks) | None (dirty reads possible) |
| **Availability** | Low (blocked on failure) | High (continues on failure) |
| **Complexity** | Simple protocol | Complex compensation |
| **Coupling** | Tight (coordinator) | Loose (events) |
| **Use Case** | Banking, payments | E-commerce, workflows |
| **Failure Handling** | Automatic rollback | Manual compensation |
| **Long Transactions** | Not suitable | Suitable |


***

## Step 6: Implementation - C++

### 6.1 Two-Phase Commit Implementation

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>

using namespace std::chrono;

// Transaction context
struct Transaction {
    std::string tx_id;
    std::unordered_map<std::string, std::string> data;
    system_clock::time_point started_at;
    
    Transaction(const std::string& id) 
        : tx_id(id), started_at(system_clock::now()) {}
};

// Vote from participant
enum class Vote {
    YES,    // Ready to commit
    NO      // Cannot commit, must abort
};

// Transaction state
enum class TxState {
    INIT,
    PREPARING,
    PREPARED,
    COMMITTING,
    COMMITTED,
    ABORTING,
    ABORTED
};

// Participant interface
class TransactionParticipant {
public:
    virtual ~TransactionParticipant() = default;
    
    // Phase 1: Prepare
    virtual Vote prepare(const Transaction& tx) = 0;
    
    // Phase 2: Commit or Abort
    virtual void commit(const std::string& tx_id) = 0;
    virtual void abort(const std::string& tx_id) = 0;
    
    // Get participant name
    virtual std::string getName() const = 0;
};

// Example: Order Service Participant
class OrderServiceParticipant : public TransactionParticipant {
private:
    std::unordered_map<std::string, Transaction> prepared_txs;
    std::mutex mtx;
    
public:
    Vote prepare(const Transaction& tx) override {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::cout << "[OrderService] PREPARE " << tx.tx_id << std::endl;
        
        // Simulate validation
        std::this_thread::sleep_for(milliseconds(50));
        
        // Check if order can be created
        std::string order_data = tx.data.at("order");
        
        if (order_data.empty()) {
            std::cout << "[OrderService] VOTE NO (invalid order)" << std::endl;
            return Vote::NO;
        }
        
        // Lock resources and store in prepared state
        prepared_txs[tx.tx_id] = tx;
        
        std::cout << "[OrderService] VOTE YES (ready to commit)" << std::endl;
        return Vote::YES;
    }
    
    void commit(const std::string& tx_id) override {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::cout << "[OrderService] COMMIT " << tx_id << std::endl;
        
        auto it = prepared_txs.find(tx_id);
        if (it == prepared_txs.end()) {
            std::cerr << "[OrderService] ERROR: Unknown transaction " << tx_id << std::endl;
            return;
        }
        
        // Actually create the order
        std::cout << "[OrderService] Order created: " << it->second.data.at("order") << std::endl;
        
        // Release locks
        prepared_txs.erase(it);
    }
    
    void abort(const std::string& tx_id) override {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::cout << "[OrderService] ABORT " << tx_id << std::endl;
        
        // Rollback and release locks
        prepared_txs.erase(tx_id);
    }
    
    std::string getName() const override {
        return "OrderService";
    }
};

// Example: Inventory Service Participant
class InventoryServiceParticipant : public TransactionParticipant {
private:
    std::unordered_map<std::string, int> inventory = {
        {"product_1", 100},
        {"product_2", 50}
    };
    std::unordered_map<std::string, std::string> reserved;
    std::mutex mtx;
    
public:
    Vote prepare(const Transaction& tx) override {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::cout << "[InventoryService] PREPARE " << tx.tx_id << std::endl;
        
        std::this_thread::sleep_for(milliseconds(30));
        
        // Check if inventory available
        std::string product_id = tx.data.at("product_id");
        int quantity = std::stoi(tx.data.at("quantity"));
        
        if (inventory[product_id] < quantity) {
            std::cout << "[InventoryService] VOTE NO (insufficient inventory)" << std::endl;
            return Vote::NO;
        }
        
        // Reserve inventory (pessimistic lock)
        inventory[product_id] -= quantity;
        reserved[tx.tx_id] = product_id;
        
        std::cout << "[InventoryService] VOTE YES (inventory reserved)" << std::endl;
        return Vote::YES;
    }
    
    void commit(const std::string& tx_id) override {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::cout << "[InventoryService] COMMIT " << tx_id << std::endl;
        
        // Inventory already deducted in prepare phase
        // Just clear reservation
        reserved.erase(tx_id);
    }
    
    void abort(const std::string& tx_id) override {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::cout << "[InventoryService] ABORT " << tx_id << std::endl;
        
        // Rollback: return reserved inventory
        auto it = reserved.find(tx_id);
        if (it != reserved.end()) {
            std::string product_id = it->second;
            int quantity = 1;  // Simplified
            inventory[product_id] += quantity;
            reserved.erase(it);
            
            std::cout << "[InventoryService] Inventory released for " << product_id << std::endl;
        }
    }
    
    std::string getName() const override {
        return "InventoryService";
    }
};

// Transaction Coordinator (TC)
class TwoPhaseCommitCoordinator {
private:
    std::vector<std::shared_ptr<TransactionParticipant>> participants;
    std::unordered_map<std::string, TxState> transaction_states;
    std::mutex mtx;
    
    const int PREPARE_TIMEOUT_MS = 5000;
    const int COMMIT_TIMEOUT_MS = 5000;
    
public:
    void addParticipant(std::shared_ptr<TransactionParticipant> participant) {
        participants.push_back(participant);
    }
    
    // Execute transaction with 2PC
    bool executeTransaction(Transaction& tx) {
        std::cout << "\n=== Starting 2PC Transaction " << tx.tx_id << " ===" << std::endl;
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            transaction_states[tx.tx_id] = TxState::PREPARING;
        }
        
        // PHASE 1: PREPARE
        std::cout << "\n--- PHASE 1: PREPARE ---" << std::endl;
        
        std::vector<std::future<Vote>> votes;
        
        // Send PREPARE to all participants in parallel
        for (auto& participant : participants) {
            votes.push_back(
                std::async(std::launch::async, [&participant, &tx]() {
                    return participant->prepare(tx);
                })
            );
        }
        
        // Collect votes with timeout
        std::vector<Vote> collected_votes;
        bool all_yes = true;
        
        for (size_t i = 0; i < votes.size(); ++i) {
            auto status = votes[i].wait_for(milliseconds(PREPARE_TIMEOUT_MS));
            
            if (status == std::future_status::timeout) {
                std::cout << "\n[Coordinator] TIMEOUT waiting for " 
                         << participants[i]->getName() << std::endl;
                all_yes = false;
                break;
            }
            
            Vote vote = votes[i].get();
            collected_votes.push_back(vote);
            
            if (vote == Vote::NO) {
                std::cout << "\n[Coordinator] Received NO vote from " 
                         << participants[i]->getName() << std::endl;
                all_yes = false;
                break;
            }
        }
        
        // PHASE 2: COMMIT or ABORT
        std::cout << "\n--- PHASE 2: ";
        
        if (all_yes) {
            std::cout << "COMMIT ---" << std::endl;
            
            {
                std::lock_guard<std::mutex> lock(mtx);
                transaction_states[tx.tx_id] = TxState::COMMITTING;
            }
            
            // Send COMMIT to all participants
            std::vector<std::future<void>> commit_futures;
            
            for (auto& participant : participants) {
                commit_futures.push_back(
                    std::async(std::launch::async, [&participant, &tx]() {
                        participant->commit(tx.tx_id);
                    })
                );
            }
            
            // Wait for all commits
            for (auto& fut : commit_futures) {
                fut.wait();
            }
            
            {
                std::lock_guard<std::mutex> lock(mtx);
                transaction_states[tx.tx_id] = TxState::COMMITTED;
            }
            
            std::cout << "\n[Coordinator] Transaction COMMITTED" << std::endl;
            return true;
            
        } else {
            std::cout << "ABORT ---" << std::endl;
            
            {
                std::lock_guard<std::mutex> lock(mtx);
                transaction_states[tx.tx_id] = TxState::ABORTING;
            }
            
            // Send ABORT to all participants
            std::vector<std::future<void>> abort_futures;
            
            for (auto& participant : participants) {
                abort_futures.push_back(
                    std::async(std::launch::async, [&participant, &tx]() {
                        participant->abort(tx.tx_id);
                    })
                );
            }
            
            // Wait for all aborts
            for (auto& fut : abort_futures) {
                fut.wait();
            }
            
            {
                std::lock_guard<std::mutex> lock(mtx);
                transaction_states[tx.tx_id] = TxState::ABORTED;
            }
            
            std::cout << "\n[Coordinator] Transaction ABORTED" << std::endl;
            return false;
        }
    }
};

// Example usage
int main() {
    // Create coordinator
    TwoPhaseCommitCoordinator coordinator;
    
    // Add participants
    coordinator.addParticipant(std::make_shared<OrderServiceParticipant>());
    coordinator.addParticipant(std::make_shared<InventoryServiceParticipant>());
    
    // Test case 1: Successful transaction
    {
        Transaction tx("tx_001");
        tx.data["order"] = "Order #123";
        tx.data["product_id"] = "product_1";
        tx.data["quantity"] = "5";
        
        bool success = coordinator.executeTransaction(tx);
        std::cout << "\nResult: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    }
    
    std::cout << "\n\n";
    
    // Test case 2: Failed transaction (insufficient inventory)
    {
        Transaction tx("tx_002");
        tx.data["order"] = "Order #124";
        tx.data["product_id"] = "product_2";
        tx.data["quantity"] = "1000";  // More than available
        
        bool success = coordinator.executeTransaction(tx);
        std::cout << "\nResult: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    }
    
    return 0;
}
```


### 6.2 Saga Pattern Implementation (Orchestration)

```cpp
#include <functional>
#include <stack>

// Saga step definition
struct SagaStep {
    std::string name;
    std::function<bool()> execute;           // Forward action
    std::function<void()> compensate;        // Backward action (undo)
};

// Saga execution state
enum class SagaState {
    EXECUTING,
    COMPENSATING,
    COMPLETED,
    FAILED
};

// Saga orchestrator
class SagaOrchestrator {
private:
    std::vector<SagaStep> steps;
    std::stack<int> completed_steps;  // Track for compensation
    SagaState state;
    std::string saga_id;
    
public:
    SagaOrchestrator(const std::string& id) : saga_id(id), state(SagaState::EXECUTING) {}
    
    void addStep(const SagaStep& step) {
        steps.push_back(step);
    }
    
    bool execute() {
        std::cout << "\n=== Executing Saga " << saga_id << " ===" << std::endl;
        
        // Execute steps sequentially
        for (size_t i = 0; i < steps.size(); ++i) {
            std::cout << "\n[Saga] Executing step " << (i + 1) << ": " 
                     << steps[i].name << std::endl;
            
            try {
                bool success = steps[i].execute();
                
                if (!success) {
                    std::cout << "[Saga] Step " << steps[i].name << " failed!" << std::endl;
                    compensate();
                    return false;
                }
                
                std::cout << "[Saga] Step " << steps[i].name << " completed" << std::endl;
                completed_steps.push(i);
                
            } catch (const std::exception& e) {
                std::cout << "[Saga] Step " << steps[i].name << " threw exception: " 
                         << e.what() << std::endl;
                compensate();
                return false;
            }
        }
        
        state = SagaState::COMPLETED;
        std::cout << "\n[Saga] All steps completed successfully!" << std::endl;
        return true;
    }
    
private:
    void compensate() {
        std::cout << "\n[Saga] Starting compensation..." << std::endl;
        state = SagaState::COMPENSATING;
        
        // Compensate in reverse order
        while (!completed_steps.empty()) {
            int step_idx = completed_steps.top();
            completed_steps.pop();
            
            std::cout << "[Saga] Compensating step: " << steps[step_idx].name << std::endl;
            
            try {
                steps[step_idx].compensate();
                std::cout << "[Saga] Compensation successful for " 
                         << steps[step_idx].name << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[Saga] Compensation failed for " << steps[step_idx].name 
                         << ": " << e.what() << std::endl;
                // Continue compensating other steps even if one fails
            }
        }
        
        state = SagaState::FAILED;
        std::cout << "[Saga] Compensation completed" << std::endl;
    }
};

// Example services for Saga
class OrderService {
public:
    static std::string createOrder(const std::string& order_data) {
        std::cout << "  [OrderService] Creating order: " << order_data << std::endl;
        std::this_thread::sleep_for(milliseconds(100));
        return "ORDER_123";
    }
    
    static void cancelOrder(const std::string& order_id) {
        std::cout << "  [OrderService] Cancelling order: " << order_id << std::endl;
        std::this_thread::sleep_for(milliseconds(50));
    }
};

class InventoryService {
public:
    static bool reserveInventory(const std::string& product_id, int quantity) {
        std::cout << "  [InventoryService] Reserving " << quantity 
                 << " units of " << product_id << std::endl;
        std::this_thread::sleep_for(milliseconds(100));
        
        // Simulate insufficient inventory
        if (quantity > 100) {
            return false;
        }
        return true;
    }
    
    static void releaseInventory(const std::string& product_id, int quantity) {
        std::cout << "  [InventoryService] Releasing " << quantity 
                 << " units of " << product_id << std::endl;
        std::this_thread::sleep_for(milliseconds(50));
    }
};

class PaymentService {
public:
    static std::string processPayment(double amount) {
        std::cout << "  [PaymentService] Processing payment: $" << amount << std::endl;
        std::this_thread::sleep_for(milliseconds(150));
        return "PAYMENT_456";
    }
    
    static void refundPayment(const std::string& payment_id) {
        std::cout << "  [PaymentService] Refunding payment: " << payment_id << std::endl;
        std::this_thread::sleep_for(milliseconds(100));
    }
};

class NotificationService {
public:
    static void sendNotification(const std::string& user_id, const std::string& message) {
        std::cout << "  [NotificationService] Sending to " << user_id 
                 << ": " << message << std::endl;
        std::this_thread::sleep_for(milliseconds(50));
    }
};

// Example usage
int main() {
    // Test case 1: Successful saga
    {
        std::cout << "===== TEST CASE 1: Successful Saga =====" << std::endl;
        
        SagaOrchestrator saga("saga_001");
        
        // Shared state across steps
        std::string order_id;
        std::string payment_id;
        
        // Step 1: Create Order
        saga.addStep({
            "CreateOrder",
            [&order_id]() {
                order_id = OrderService::createOrder("Order for user_123");
                return true;
            },
            [&order_id]() {
                OrderService::cancelOrder(order_id);
            }
        });
        
        // Step 2: Reserve Inventory
        saga.addStep({
            "ReserveInventory",
            []() {
                return InventoryService::reserveInventory("product_1", 5);
            },
            []() {
                InventoryService::releaseInventory("product_1", 5);
            }
        });
        
        // Step 3: Process Payment
        saga.addStep({
            "ProcessPayment",
            [&payment_id]() {
                payment_id = PaymentService::processPayment(99.99);
                return true;
            },
            [&payment_id]() {
                PaymentService::refundPayment(payment_id);
            }
        });
        
        // Step 4: Send Notification (no compensation)
        saga.addStep({
            "SendNotification",
            []() {
                NotificationService::sendNotification("user_123", "Order confirmed!");
                return true;
            },
            []() {
                // Cannot "unsend" email, but can send cancellation notice
                NotificationService::sendNotification("user_123", "Order cancelled");
            }
        });
        
        bool success = saga.execute();
        std::cout << "\nSaga result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    }
    
    std::cout << "\n\n";
    
    // Test case 2: Failed saga (triggers compensation)
    {
        std::cout << "===== TEST CASE 2: Failed Saga (Insufficient Inventory) =====" << std::endl;
        
        SagaOrchestrator saga("saga_002");
        
        std::string order_id;
        std::string payment_id;
        
        // Step 1: Create Order
        saga.addStep({
            "CreateOrder",
            [&order_id]() {
                order_id = OrderService::createOrder("Order for user_456");
                return true;
            },
            [&order_id]() {
                OrderService::cancelOrder(order_id);
            }
        });
        
        // Step 2: Reserve Inventory (will fail)
        saga.addStep({
            "ReserveInventory",
            []() {
                return InventoryService::reserveInventory("product_2", 500);  // Too many
            },
            []() {
                InventoryService::releaseInventory("product_2", 500);
            }
        });
        
        // Step 3: Process Payment (won't be reached)
        saga.addStep({
            "ProcessPayment",
            [&payment_id]() {
                payment_id = PaymentService::processPayment(199.99);
                return true;
            },
            [&payment_id]() {
                PaymentService::refundPayment(payment_id);
            }
        });
        
        bool success = saga.execute();
        std::cout << "\nSaga result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    }
    
    return 0;
}
```


### 6.3 Saga Pattern - Choreography (Event-Driven)

```cpp
#include <queue>

// Event types
enum class EventType {
    ORDER_CREATED,
    ORDER_CANCELLED,
    INVENTORY_RESERVED,
    INVENTORY_FAILED,
    PAYMENT_PROCESSED,
    PAYMENT_FAILED,
    NOTIFICATION_SENT
};

// Event structure
struct Event {
    EventType type;
    std::string saga_id;
    std::unordered_map<std::string, std::string> data;
    system_clock::time_point timestamp;
};

// Event bus (simplified message broker)
class EventBus {
private:
    std::queue<Event> event_queue;
    std::mutex mtx;
    std::condition_variable cv;
    
public:
    void publish(const Event& event) {
        std::lock_guard<std::mutex> lock(mtx);
        event_queue.push(event);
        cv.notify_all();
    }
    
    Event consume(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx);
        
        if (!cv.wait_for(lock, timeout, [this]() { return !event_queue.empty(); })) {
            throw std::runtime_error("Timeout waiting for event");
        }
        
        Event event = event_queue.front();
        event_queue.pop();
        return event;
    }
};

// Service that participates in saga via events
class EventDrivenOrderService {
private:
    EventBus& event_bus;
    std::unordered_map<std::string, std::string> orders;
    
public:
    EventDrivenOrderService(EventBus& bus) : event_bus(bus) {}
    
    void handleCreateOrderRequest(const std::string& saga_id, const std::string& order_data) {
        std::cout << "[OrderService] Creating order for saga " << saga_id << std::endl;
        
        // Create order
        orders[saga_id] = "ORDER_" + saga_id;
        
        // Publish event
        Event event{
            EventType::ORDER_CREATED,
            saga_id,
            {{"order_id", orders[saga_id]}, {"order_data", order_data}},
            system_clock::now()
        };
        event_bus.publish(event);
        
        std::cout << "[OrderService] Published ORDER_CREATED" << std::endl;
    }
    
    void handleInventoryFailed(const Event& event) {
        std::string saga_id = event.saga_id;
        
        std::cout << "[OrderService] Compensating: Cancelling order for saga " 
                 << saga_id << std::endl;
        
        // Cancel order
        orders.erase(saga_id);
        
        // Publish compensation event
        Event comp_event{
            EventType::ORDER_CANCELLED,
            saga_id,
            {},
            system_clock::now()
        };
        event_bus.publish(comp_event);
    }
};

class EventDrivenInventoryService {
private:
    EventBus& event_bus;
    
public:
    EventDrivenInventoryService(EventBus& bus) : event_bus(bus) {}
    
    void handleOrderCreated(const Event& event) {
        std::string saga_id = event.saga_id;
        
        std::cout << "[InventoryService] Reserving inventory for saga " 
                 << saga_id << std::endl;
        
        // Simulate inventory check
        bool success = (rand() % 100) < 80;  // 80% success rate
        
        Event result_event;
        if (success) {
            result_event = {
                EventType::INVENTORY_RESERVED,
                saga_id,
                {{"product_id", "product_1"}, {"quantity", "5"}},
                system_clock::now()
            };
            std::cout << "[InventoryService] Published INVENTORY_RESERVED" << std::endl;
        } else {
            result_event = {
                EventType::INVENTORY_FAILED,
                saga_id,
                {{"reason", "insufficient_stock"}},
                system_clock::now()
            };
            std::cout << "[InventoryService] Published INVENTORY_FAILED" << std::endl;
        }
        
        event_bus.publish(result_event);
    }
};

// Choreography example
void runChoreographySaga() {
    EventBus event_bus;
    
    EventDrivenOrderService order_service(event_bus);
    EventDrivenInventoryService inventory_service(event_bus);
    
    // Start saga
    std::string saga_id = "saga_choreography_001";
    order_service.handleCreateOrderRequest(saga_id, "Order data");
    
    // Event loop
    for (int i = 0; i < 5; ++i) {
        try {
            Event event = event_bus.consume(milliseconds(1000));
            
            switch (event.type) {
                case EventType::ORDER_CREATED:
                    inventory_service.handleOrderCreated(event);
                    break;
                    
                case EventType::INVENTORY_FAILED:
                    order_service.handleInventoryFailed(event);
                    break;
                    
                // Other event handlers...
                
                default:
                    break;
            }
        } catch (const std::runtime_error& e) {
            std::cout << "Event loop timeout" << std::endl;
            break;
        }
    }
}
```


***

## Step 7: Advanced Concepts

### 7.1 Isolation Issues in Sagas

**Problem: Dirty Reads**

```
Timeline:

T0: Saga 1: Reserve 10 units of product_A (balance: 100 → 90)
T1: Saga 2: Reads balance (sees 90 units)
T2: Saga 2: Tries to reserve 50 units (success, thinks 40 left)
T3: Saga 1: FAILS, compensate (balance: 90 → 100)
T4: Now Saga 2 has incorrect view!

Problem: Saga 2 saw intermediate state of Saga 1
```

**Solutions:**

**1. Semantic Lock**

```cpp
struct SemanticLock {
    std::string resource_id;
    std::string locked_by_saga;
    system_clock::time_point locked_at;
};

// Other sagas can read but know resource is "dirty"
bool canRead(const std::string& resource_id) {
    if (isLocked(resource_id)) {
        // Return data but mark as "tentative"
        return true;
    }
    return true;
}
```

**2. Commutative Updates**

```cpp
// Instead of: SET balance = 90
// Use: ADD balance = -10

// This way, order doesn't matter
// Saga 1: ADD -10
// Saga 2: ADD -50
// Saga 1 compensate: ADD +10
// Final: -10 - 50 + 10 = -50 (correct)
```

**3. Pessimistic View**

```cpp
// Mark resources as "tentative" during saga
struct ResourceVersion {
    int committed_value;
    std::vector<int> tentative_changes;
};

// Readers see only committed value
// Writers see committed + tentative
```

**4. Reread Value**

```cpp
// Before committing, reread to ensure nothing changed
bool commitWithValidation() {
    int value_at_start = readValue();
    
    // ... saga steps ...
    
    int value_now = readValue();
    
    if (value_now != value_at_start) {
        // Someone else modified, abort
        return false;
    }
    
    commit();
    return true;
}
```


### 7.2 Saga Execution Guarantees

**At-Least-Once Execution:**

```cpp
class SagaStepWithRetry {
    bool executeWithRetry(const SagaStep& step, int max_retries = 3) {
        for (int attempt = 0; attempt < max_retries; ++attempt) {
            try {
                if (step.execute()) {
                    return true;
                }
            } catch (const std::exception& e) {
                std::cerr << "Attempt " << (attempt + 1) << " failed: " 
                         << e.what() << std::endl;
                
                // Exponential backoff
                std::this_thread::sleep_for(
                    milliseconds(100 * (1 << attempt))
                );
            }
        }
        return false;
    }
};

// Implication: Steps must be idempotent!
// Example: 
//   - reserveInventory(product_id, quantity) - use SET not INCREMENT
//   - createOrder(order_id, data) - use INSERT ... ON CONFLICT DO NOTHING
```

**Idempotency Pattern:**

```cpp
class IdempotentService {
private:
    std::unordered_set<std::string> processed_requests;
    
public:
    bool processRequest(const std::string& request_id, std::function<void()> action) {
        // Check if already processed
        if (processed_requests.find(request_id) != processed_requests.end()) {
            std::cout << "Request " << request_id << " already processed (idempotent)" << std::endl;
            return true;  // Return success without re-executing
        }
        
        // Execute action
        action();
        
        // Mark as processed
        processed_requests.insert(request_id);
        
        return true;
    }
};
```


### 7.3 Saga Recovery Patterns

**Forward Recovery (Retry Until Success):**

```cpp
class ForwardRecoverySaga {
    bool execute() {
        for (size_t i = 0; i < steps.size(); ++i) {
            bool success = false;
            int retry_count = 0;
            
            while (!success) {
                success = steps[i].execute();
                
                if (!success) {
                    retry_count++;
                    std::cout << "Step " << i << " failed, retry " 
                             << retry_count << std::endl;
                    
                    // Exponential backoff
                    std::this_thread::sleep_for(seconds(1 << std::min(retry_count, 5)));
                    
                    // Could also add max retries and switch to compensation
                }
            }
        }
        return true;
    }
};
```

**Backward Recovery (Compensate on Failure):**

```cpp
// Already shown in previous implementation
// Key: Must be prepared to compensate at any point
```

**Mixed Strategy:**

```cpp
class HybridRecoverySaga {
    bool execute() {
        for (size_t i = 0; i < steps.size(); ++i) {
            bool success = retryWithLimit(steps[i], 3);
            
            if (!success) {
                // Forward recovery failed, switch to backward recovery
                if (steps[i].retriable) {
                    // Keep retrying forever (idempotent operation)
                    retryForever(steps[i]);
                } else {
                    // Not retriable, must compensate
                    compensate();
                    return false;
                }
            }
        }
        return true;
    }
};
```


***

## Step 8: Real-World Considerations

### 8.1 When to Use 2PC vs Saga

**Use Two-Phase Commit When:**

- ✅ Strong consistency required (financial transactions)
- ✅ Short-lived transactions (<100ms)
- ✅ All participants within same datacenter
- ✅ Can tolerate blocking
- ✅ Traditional databases (support XA protocol)

**Use Saga When:**

- ✅ Long-running workflows (minutes/hours)
- ✅ Microservices architecture
- ✅ Cross-datacenter/cloud transactions
- ✅ High availability required
- ✅ Eventual consistency acceptable
- ✅ NoSQL databases involved

**Real Examples:**

```
Banking Wire Transfer: 2PC
- Strong consistency critical
- Cannot tolerate "money lost"
- Acceptable to wait

E-commerce Order: Saga
- Multiple independent services
- User can wait for confirmation
- Can handle "order pending" state

Hotel + Flight Booking: Saga
- Long-running (user browses)
- External APIs involved
- Compensate if one fails
```


### 8.2 Monitoring \& Observability

```cpp
class SagaMonitor {
private:
    struct SagaMetrics {
        int total_executions;
        int successful;
        int compensated;
        std::chrono::milliseconds avg_duration;
        std::unordered_map<std::string, int> step_failures;
    };
    
    std::unordered_map<std::string, SagaMetrics> metrics;
    
public:
    void recordExecution(const std::string& saga_type, bool success, 
                        std::chrono::milliseconds duration) {
        auto& m = metrics[saga_type];
        m.total_executions++;
        
        if (success) {
            m.successful++;
        } else {
            m.compensated++;
        }
        
        // Update running average
        m.avg_duration = (m.avg_duration * (m.total_executions - 1) + duration) 
                        / m.total_executions;
    }
    
    void recordStepFailure(const std::string& saga_type, const std::string& step_name) {
        metrics[saga_type].step_failures[step_name]++;
    }
    
    void printMetrics() {
        for (const auto& [saga_type, m] : metrics) {
            std::cout << "\n=== Saga Type: " << saga_type << " ===" << std::endl;
            std::cout << "Total executions: " << m.total_executions << std::endl;
            std::cout << "Successful: " << m.successful << " ("
                     << (m.successful * 100.0 / m.total_executions) << "%)" << std::endl;
            std::cout << "Compensated: " << m.compensated << " ("
                     << (m.compensated * 100.0 / m.total_executions) << "%)" << std::endl;
            std::cout << "Avg duration: " << m.avg_duration.count() << "ms" << std::endl;
            
            std::cout << "\nStep failures:" << std::endl;
            for (const auto& [step, count] : m.step_failures) {
                std::cout << "  " << step << ": " << count << std::endl;
            }
        }
    }
};
```


### 8.3 Saga State Persistence

```cpp
class SagaStatePersistence {
private:
    DatabaseConnection db;
    
    struct PersistedSagaState {
        std::string saga_id;
        std::string saga_type;
        int current_step;
        SagaState state;
        std::string execution_data;  // JSON
        system_clock::time_point created_at;
        system_clock::time_point updated_at;
    };
    
public:
    void saveSagaState(const PersistedSagaState& state) {
        std::string sql = R"(
            INSERT INTO saga_state (saga_id, saga_type, current_step, state, 
                                   execution_data, created_at, updated_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
            ON CONFLICT (saga_id) 
            DO UPDATE SET 
                current_step = ?,
                state = ?,
                execution_data = ?,
                updated_at = ?
        )";
        
        // Execute SQL (simplified)
    }
    
    std::optional<PersistedSagaState> loadSagaState(const std::string& saga_id) {
        std::string sql = "SELECT * FROM saga_state WHERE saga_id = ?";
        // Execute and return state
        return std::nullopt;  // Simplified
    }
    
    // Recovery: Find incomplete sagas and resume
    std::vector<PersistedSagaState> findIncompleteSagas() {
        std::string sql = R"(
            SELECT * FROM saga_state 
            WHERE state IN ('EXECUTING', 'COMPENSATING')
              AND updated_at < NOW() - INTERVAL '5 minutes'
        )";
        
        // These sagas might have crashed, need manual intervention or retry
        return {};  // Simplified
    }
};
```


***

## Summary: Key Takeaways

### Protocol Comparison

| Feature | 2PC | 3PC | Saga (Orchestration) | Saga (Choreography) |
| :-- | :-- | :-- | :-- | :-- |
| **Consistency** | Strong | Strong | Eventual | Eventual |
| **Availability** | Low (blocking) | Medium | High | High |
| **Complexity** | Low | Medium | Medium | High |
| **Latency** | 2 RTT | 3 RTT | Async | Async |
| **Isolation** | Full | Full | None | None |
| **Long Transactions** | ❌ No | ❌ No | ✅ Yes | ✅ Yes |
| **Failure Recovery** | Automatic | Automatic | Manual compensation | Distributed |
| **SPOF** | Coordinator | Coordinator | Orchestrator | None |
| **Testing** | Easy | Easy | Medium | Hard |

### Decision Matrix

```
Choose 2PC if:
  ✅ Need ACID guarantees
  ✅ Short transactions (<100ms)
  ✅ All systems support XA
  ✅ Same datacenter
  
Choose Saga if:
  ✅ Microservices architecture
  ✅ Long-running workflows
  ✅ High availability required
  ✅ Can model compensations
  
Choose Orchestration if:
  ✅ Complex workflow logic
  ✅ Need centralized monitoring
  ✅ Clear step dependencies
  
Choose Choreography if:
  ✅ Simple workflows
  ✅ Services loosely coupled
  ✅ Event-driven architecture
```


### Performance Characteristics

```
Two-Phase Commit:
- Throughput: 1K TPS (low due to blocking)
- Latency: 50-100ms (P99)
- Scalability: Limited (coordinator bottleneck)

Saga Pattern:
- Throughput: 10K TPS (async processing)
- Latency: 500ms-5s (P99, depends on steps)
- Scalability: High (distributed)
```

This comprehensive guide covers the theory, implementation, and practical considerations for distributed transaction systems!

