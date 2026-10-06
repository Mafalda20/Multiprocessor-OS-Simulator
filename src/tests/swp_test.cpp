#include "somm25nm.h"
#include "exception.h"
#include <cstdio>
#include <cstdint>
#include <unistd.h>

// External variables
extern SwpNode *swpHead;
extern SwpNode *swpTail;
extern SwpSwappingPolicy swpPolicy;

static int testsPassed = 0;
static int testsFailed = 0;

static void banner(const char *msg)
{
    std::printf("\n");
    std::printf("════════════════════════════════════════════════════════════════\n");
    std::printf("  %s\n", msg);
    std::printf("════════════════════════════════════════════════════════════════\n\n");
}

static void testHeader(const char *name)
{
    std::printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    std::printf("│ %s\n", name);
    std::printf("└─────────────────────────────────────────────────────────────┘\n");
}

static void printList(const char *label)
{
    std::printf("  %s: ", label);
    if (swpHead == nullptr) {
        std::printf("EMPTY\n");
        return;
    }
    
    SwpNode *cur = swpHead;
    int count = 0;
    while (cur != nullptr) {
        if (count > 0) std::printf(" → ");
        std::printf("[PID:%hu, Size:%u, %s]", 
                    cur->pid, cur->size, cur->blocked ? "BLOCKED" : "READY");
        cur = cur->next;
        count++;
    }
    std::printf("\n");
}

static int countNodes()
{
    int count = 0;
    SwpNode *cur = swpHead;
    while (cur != nullptr) {
        count++;
        cur = cur->next;
    }
    return count;
}

static bool verifyNode(uint16_t pid, uint32_t expectedSize, bool expectedBlocked)
{
    SwpNode *cur = swpHead;
    while (cur != nullptr) {
        if (cur->pid == pid) {
            return (cur->size == expectedSize && cur->blocked == expectedBlocked);
        }
        cur = cur->next;
    }
    return false;
}

static void PASS(const char *msg)
{
    std::printf("  ✓ PASS: %s\n", msg);
    testsPassed++;
}

static void FAIL(const char *msg)
{
    std::printf("  ✗ FAIL: %s\n", msg);
    testsFailed++;
}

// ============================================================================
// TEST 1: Open and Close Module
// ============================================================================
static void test01_OpenClose()
{
    testHeader("TEST 1: swpOpen and swpClose");
    
    try {
        soBinSetIDs(0, 0);
        
        std::printf("\n  [1.1] Testing swpOpen with FirstFit policy\n");
        swpOpen(FirstFit);
        
        if (swpHead == nullptr && swpTail == nullptr) {
            PASS("List initialized as empty (head=NULL, tail=NULL)");
        } else {
            FAIL("List not properly initialized");
        }
        
        if (swpPolicy == FirstFit) {
            PASS("Policy correctly set to FirstFit");
        } else {
            FAIL("Policy not set correctly");
        }
        
        std::printf("\n  [1.2] Testing swpClose\n");
        swpClose();
        PASS("Module closed successfully");
        
        std::printf("\n  [1.3] Testing swpOpen with FirstBest policy\n");
        swpOpen(FirstBest);
        
        if (swpPolicy == FirstBest) {
            PASS("Policy correctly set to FirstBest");
        } else {
            FAIL("Policy not set correctly");
        }
        
        swpClose();
        PASS("Module closed successfully");
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s (errno=%d)\n", e.what(), e.en);
    }
}

// ============================================================================
// TEST 2: Single Insertion
// ============================================================================
static void test02_InsertSingle()
{
    testHeader("TEST 2: swpInsert - Single Process");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [2.1] Inserting first process\n");
        std::printf("      → swpInsert(100, 1024, false)\n");
        swpInsert(100, 1024, false);
        printList("List state");
        
        if (swpHead != nullptr && swpTail != nullptr) {
            PASS("Head and tail are not NULL");
        } else {
            FAIL("Head or tail is NULL after insertion");
        }
        
        if (swpHead == swpTail) {
            PASS("Head equals tail (single node)");
        } else {
            FAIL("Head should equal tail for single node");
        }
        
        if (swpHead != nullptr && swpHead->pid == 100 && 
            swpHead->size == 1024 && swpHead->blocked == false) {
            PASS("Node data correctly stored");
        } else {
            FAIL("Node data incorrect");
        }
        
        if (swpHead != nullptr && swpHead->next == nullptr) {
            PASS("Next pointer is NULL");
        } else {
            FAIL("Next pointer should be NULL");
        }
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 3: Multiple Insertions
// ============================================================================
static void test03_InsertMultiple()
{
    testHeader("TEST 3: swpInsert - Multiple Processes");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [3.1] Inserting 5 processes\n");
        swpInsert(101, 512, false);
        std::printf("      → Inserted PID 101\n");
        swpInsert(102, 2048, true);
        std::printf("      → Inserted PID 102 (blocked)\n");
        swpInsert(103, 1024, false);
        std::printf("      → Inserted PID 103\n");
        swpInsert(104, 4096, true);
        std::printf("      → Inserted PID 104 (blocked)\n");
        swpInsert(105, 768, false);
        std::printf("      → Inserted PID 105\n");
        
        printList("Final list");
        
        int count = countNodes();
        std::printf("\n  [3.2] Verifying list structure\n");
        if (count == 5) {
            PASS("List contains 5 nodes");
        } else {
            FAIL("List should contain 5 nodes");
            std::printf("      Found: %d nodes\n", count);
        }
        
        std::printf("\n  [3.3] Verifying insertion order (FIFO)\n");
        SwpNode *cur = swpHead;
        uint16_t expectedPids[] = {101, 102, 103, 104, 105};
        bool orderCorrect = true;
        for (int i = 0; i < 5 && cur != nullptr; i++) {
            if (cur->pid != expectedPids[i]) {
                orderCorrect = false;
                break;
            }
            cur = cur->next;
        }
        
        if (orderCorrect) {
            PASS("Processes in correct FIFO order");
        } else {
            FAIL("Processes not in FIFO order");
        }
        
        std::printf("\n  [3.4] Verifying tail pointer\n");
        if (swpTail != nullptr && swpTail->pid == 105) {
            PASS("Tail points to last inserted process");
        } else {
            FAIL("Tail pointer incorrect");
        }
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 4: Retrieve from Empty List
// ============================================================================
static void test04_RetrieveEmpty()
{
    testHeader("TEST 4: swpRetrieve - Empty List");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [4.1] Retrieving from empty list\n");
        std::printf("      → swpRetrieve(1000, false)\n");
        uint16_t pid = swpRetrieve(1000, false);
        
        std::printf("      Returned PID: %hu\n", pid);
        
        if (pid == 0) {
            PASS("Correctly returned 0 for empty list");
        } else {
            FAIL("Should return 0 for empty list");
        }
        
        if (swpHead == nullptr && swpTail == nullptr) {
            PASS("List remains empty");
        } else {
            FAIL("List should remain empty");
        }
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 5: Retrieve - FirstFit Policy
// ============================================================================
static void test05_RetrieveFirstFit()
{
    testHeader("TEST 5: swpRetrieve - FirstFit Policy");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [5.1] Setting up test scenario\n");
        swpInsert(201, 500, false);   // Fits
        swpInsert(202, 1500, false);  // Too big
        swpInsert(203, 800, false);   // Fits
        swpInsert(204, 2000, false);  // Too big
        
        printList("Initial list");
        
        std::printf("\n  [5.2] First retrieve: sizeAvailable=1000\n");
        std::printf("      → Expected: PID 201 (first that fits)\n");
        uint16_t pid = swpRetrieve(1000, false);
        std::printf("      → Retrieved: PID %hu\n", pid);
        
        if (pid == 201) {
            PASS("Retrieved correct process (FirstFit)");
        } else {
            FAIL("Should retrieve PID 201");
        }
        
        printList("After first retrieve");
        
        std::printf("\n  [5.3] Second retrieve: sizeAvailable=1000\n");
        std::printf("      → Expected: PID 203 (next that fits)\n");
        pid = swpRetrieve(1000, false);
        std::printf("      → Retrieved: PID %hu\n", pid);
        
        if (pid == 203) {
            PASS("Retrieved correct process");
        } else {
            FAIL("Should retrieve PID 203");
        }
        
        printList("After second retrieve");
        
        std::printf("\n  [5.4] Third retrieve: sizeAvailable=1000\n");
        std::printf("      → Expected: 0 (no process fits)\n");
        pid = swpRetrieve(1000, false);
        std::printf("      → Retrieved: PID %hu\n", pid);
        
        if (pid == 0) {
            PASS("Correctly returned 0 (no fit)");
        } else {
            FAIL("Should return 0");
        }
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 6: Retrieve - No Fit Scenario
// ============================================================================
static void test06_RetrieveNoFit()
{
    testHeader("TEST 6: swpRetrieve - No Process Fits");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [6.1] Inserting processes with large memory requirements\n");
        swpInsert(301, 2000, false);
        swpInsert(302, 3000, false);
        swpInsert(303, 2500, false);
        
        printList("Initial list");
        
        std::printf("\n  [6.2] Attempting retrieve with small size\n");
        std::printf("      → swpRetrieve(1000, false)\n");
        uint16_t pid = swpRetrieve(1000, false);
        std::printf("      → Retrieved: PID %hu\n", pid);
        
        if (pid == 0) {
            PASS("Correctly returned 0 (no fit)");
        } else {
            FAIL("Should return 0 when no process fits");
        }
        
        int countBefore = countNodes();
        printList("List after retrieve attempt");
        int countAfter = countNodes();
        
        if (countBefore == countAfter && countAfter == 3) {
            PASS("List unchanged (all processes remain)");
        } else {
            FAIL("List should remain unchanged");
        }
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 7: Retrieve - Blocked Process Filtering
// ============================================================================
static void test07_RetrieveBlocked()
{
    testHeader("TEST 7: swpRetrieve - Blocked Process Handling");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [7.1] Mixed blocked/unblocked processes\n");
        swpInsert(401, 500, true);    // Blocked, fits
        swpInsert(402, 600, true);    // Blocked, fits
        swpInsert(403, 1000, false);  // Unblocked, fits
        swpInsert(404, 700, true);    // Blocked, fits
        
        printList("Initial list");
        
        std::printf("\n  [7.2] Retrieve with canBeBlocked=false\n");
        std::printf("      → Should skip blocked processes\n");
        std::printf("      → swpRetrieve(1500, false)\n");
        uint16_t pid = swpRetrieve(1500, false);
        std::printf("      → Retrieved: PID %hu (expected 403)\n", pid);
        
        if (pid == 403) {
            PASS("Correctly skipped blocked processes");
        } else {
            FAIL("Should retrieve first unblocked process (403)");
        }
        
        printList("After retrieve");
        
        std::printf("\n  [7.3] Retrieve with canBeBlocked=true\n");
        std::printf("      → Should consider blocked processes\n");
        std::printf("      → swpRetrieve(1500, true)\n");
        pid = swpRetrieve(1500, true);
        std::printf("      → Retrieved: PID %hu (expected 401)\n", pid);
        
        if (pid == 401) {
            PASS("Correctly retrieved blocked process");
        } else {
            FAIL("Should retrieve first fitting process (401)");
        }
        
        printList("After second retrieve");
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 8: Retrieve - FirstBest Policy
// ============================================================================
static void test08_RetrieveFirstBest()
{
    testHeader("TEST 8: swpRetrieve - FirstBest Policy");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstBest);
        
        std::printf("\n  [8.1] Setting up test scenario\n");
        swpInsert(501, 400, false);   // Size 400
        swpInsert(502, 1000, false);  // Size 1000
        swpInsert(503, 800, false);   // Size 800 (best fit for 900)
        swpInsert(504, 600, false);   // Size 600
        
        printList("Initial list");
        
        std::printf("\n  [8.2] Retrieve with FirstBest, sizeAvailable=900\n");
        std::printf("      → Best fit: PID 503 (size 800)\n");
        uint16_t pid = swpRetrieve(900, false);
        std::printf("      → Retrieved: PID %hu\n", pid);
        
        if (pid == 503) {
            PASS("Retrieved best fitting process");
        } else {
            FAIL("Should retrieve PID 503 (best fit)");
        }
        
        printList("After retrieve");
        
        std::printf("\n  [8.3] Second retrieve with sizeAvailable=1200\n");
        std::printf("      → Best fit: PID 502 (size 1000)\n");
        pid = swpRetrieve(1200, false);
        std::printf("      → Retrieved: PID %hu\n", pid);
        
        if (pid == 502) {
            PASS("Retrieved best fitting process");
        } else {
            FAIL("Should retrieve PID 502");
        }
        
        printList("After second retrieve");
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 9: swpUnblock
// ============================================================================
static void test09_Unblock()
{
    testHeader("TEST 9: swpUnblock - Unblocking Processes");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [9.1] Creating list with blocked processes\n");
        swpInsert(601, 500, true);
        swpInsert(602, 1000, false);
        swpInsert(603, 750, true);
        swpInsert(604, 1200, true);
        
        printList("Initial list");
        
        std::printf("\n  [9.2] Unblocking PID 601\n");
        swpUnblock(601);
        printList("After unblock(601)");
        
        if (verifyNode(601, 500, false)) {
            PASS("PID 601 correctly unblocked");
        } else {
            FAIL("PID 601 not properly unblocked");
        }
        
        std::printf("\n  [9.3] Unblocking PID 604\n");
        swpUnblock(604);
        printList("After unblock(604)");
        
        if (verifyNode(604, 1200, false)) {
            PASS("PID 604 correctly unblocked");
        } else {
            FAIL("PID 604 not properly unblocked");
        }
        
        std::printf("\n  [9.4] Verifying list integrity\n");
        int count = countNodes();
        if (count == 4) {
            PASS("List maintains correct node count");
        } else {
            FAIL("Node count changed unexpectedly");
        }
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 10: swpPrint
// ============================================================================
static void test10_Print()
{
    testHeader("TEST 10: swpPrint - Output Formatting");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [10.1] Testing print with empty list\n");
        std::printf("      Normal format:\n");
        swpPrint(stdout, false);
        std::printf("\n      CSV format:\n");
        swpPrint(stdout, true);
        PASS("Print works with empty list");
        
        std::printf("\n  [10.2] Adding processes and printing\n");
        swpInsert(701, 512, false);
        swpInsert(702, 1024, true);
        swpInsert(703, 2048, false);
        
        std::printf("      Normal format:\n");
        swpPrint(stdout, false);
        
        std::printf("\n      CSV format:\n");
        swpPrint(stdout, true);
        
        PASS("Print works with populated list");
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// TEST 11: Complex Real-World Scenario
// ============================================================================
static void test11_ComplexScenario()
{
    testHeader("TEST 11: Complex Real-World Scenario");
    
    try {
        soBinSetIDs(0, 0);
        swpOpen(FirstFit);
        
        std::printf("\n  [11.1] Initial system state\n");
        std::printf("      Simulating 10 processes arriving...\n");
        
        swpInsert(1001, 256, false);
        swpInsert(1002, 512, true);
        swpInsert(1003, 1024, false);
        swpInsert(1004, 768, true);
        swpInsert(1005, 2048, false);
        swpInsert(1006, 384, false);
        swpInsert(1007, 1536, true);
        swpInsert(1008, 896, false);
        swpInsert(1009, 640, true);
        swpInsert(1010, 1280, false);
        
        printList("All processes swapped out");
        int initialCount = countNodes();
        std::printf("      Total processes: %d\n", initialCount);
        
        std::printf("\n  [11.2] Memory becomes available (1000 bytes)\n");
        std::printf("      Looking for unblocked process...\n");
        uint16_t pid = swpRetrieve(1000, false);
        std::printf("      → Swapped in: PID %hu\n", pid);
        printList("After first swap-in");
        
        std::printf("\n  [11.3] Unblocking some processes\n");
        swpUnblock(1002);
        swpUnblock(1009);
        printList("After unblocking");
        
        std::printf("\n  [11.4] More memory available (800 bytes)\n");
        pid = swpRetrieve(800, false);
        std::printf("      → Swapped in: PID %hu\n", pid);
        printList("After second swap-in");
        
        std::printf("\n  [11.5] Large memory block available (2500 bytes)\n");
        pid = swpRetrieve(2500, true);
        std::printf("      → Swapped in: PID %hu\n", pid);
        printList("After third swap-in");
        
        std::printf("\n  [11.6] Final system statistics\n");
        int finalCount = countNodes();
        std::printf("      Remaining swapped processes: %d\n", finalCount);
        std::printf("      Processes swapped in: %d\n", initialCount - finalCount);
        
        if (finalCount < initialCount) {
            PASS("Successfully managed complex scenario");
        } else {
            FAIL("No processes were swapped in");
        }
        
        swpClose();
        
    } catch (const Exception &e) {
        FAIL("Exception thrown");
        std::printf("      Exception: %s\n", e.what());
    }
}

// ============================================================================
// MAIN
// ============================================================================
int main()
{
    soProbeOpen(stdout, 0, 0);
    
    banner("SWP MODULE COMPREHENSIVE TEST SUITE");
    std::printf("Testing all swappperson management functions...\n");
    sleep(1);
    
    test01_OpenClose();
    sleep(1);
    test02_InsertSingle();
    sleep(1);
    test03_InsertMultiple();
    sleep(1);
    test04_RetrieveEmpty();
    sleep(1);
    test05_RetrieveFirstFit();
    sleep(1);
    test06_RetrieveNoFit();
    sleep(1);
    test07_RetrieveBlocked();
    sleep(1);
    test08_RetrieveFirstBest();
    sleep(1);
    test09_Unblock();
    sleep(1);
    test10_Print();
    sleep(1);
    test11_ComplexScenario();
    
    banner("TEST SUMMARY");
    std::printf("\n");
    std::printf("  ╔══════════════════════════════════════╗\n");
    std::printf("  ║         TEST RESULTS                 ║\n");
    std::printf("  ╠══════════════════════════════════════╣\n");
    std::printf("  ║  Tests Passed:  %3d                  ║\n", testsPassed);
    std::printf("  ║  Tests Failed:  %3d                  ║\n", testsFailed);
    std::printf("  ║  Total Tests:   %3d                  ║\n", testsPassed + testsFailed);
    std::printf("  ╚══════════════════════════════════════╝\n");
    std::printf("\n");
    
    if (testsFailed == 0) {
        std::printf("   ALL TESTS PASSED! \n\n");
        return 0;
    } else {
        std::printf("    SOME TESTS FAILED \n\n");
        return 1;
    }
}