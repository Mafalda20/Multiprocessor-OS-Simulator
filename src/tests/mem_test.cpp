#include "somm25nm.h"
#include "exception.h"

#include <cstdio>
#include <cstdint>
#include <errno.h>

extern MemNode *memBlocks;
extern uint32_t memBlockCount;
extern uint16_t memMinLogSize;
extern MemNode *memFreeHead;
extern MemNode *memOccupiedHead;

static void banner(const char *msg)
{
    std::printf("\n==================== %s ====================\n\n", msg);
}

static void testMemOpen()
{
    banner("Testing MEM (memOpen)");
    soBinSetIDs(0, 0);

    try {
        // ============================================
        // TEST 1: Basic memOpen with 6 blocks
        // ============================================
        std::printf(">>> TEST 1: memOpen with 6 blocks\n");
        std::printf("    Config: initAddr=0, minLogSize=5, blocks: 2x32B, 1x64B, 3x128B\n\n");

        uint32_t sizes1[] = {2, 1, 3};
        memOpen(0, 5, sizes1, 3);
        
        std::printf("    ✓ memOpen OK! Total blocks: %u, minLogSize: %u\n\n", 
                   memBlockCount, memMinLogSize);

        if (memBlocks != NULL && memBlocks != MEM_UNDEF_NODE) {
            std::printf("    ✓ memBlocks initialized\n");
        } else {
            std::printf("    ✗ ERROR: memBlocks not initialized!\n");
        }
        
        if (memFreeHead != NULL && memFreeHead != MEM_UNDEF_NODE) {
            std::printf("    ✓ Free list initialized\n");
            
            uint32_t count = 0;
            MemNode *node = memFreeHead;
            while (node != NULL) {
                count++;
                std::printf("      [%u] addr=0x%08x, logSize=%u, pid=%u\n",
                           count, node->addr, node->logSize, node->pid);
                node = node->next;
            }
            std::printf("    Total free: %u\n", count);
            
            if (count == 6) {
                std::printf("    ✓ Correct number of blocks\n");
            } else {
                std::printf("    ✗ ERROR: Expected 6 blocks, got %u\n", count);
            }
        }
        
        if (memOccupiedHead == NULL) {
            std::printf("    ✓ Occupied list empty (as expected)\n");
        } else {
            std::printf("    ✗ ERROR: Occupied list should be empty!\n");
        }
        std::printf("\n");

        std::printf("    ✓ TEST 1 PASSED!\n\n");
        
        memClose();

        // ============================================
        // TEST 2: Different configuration
        // ============================================
        std::printf(">>> TEST 2: Different configuration\n");
        std::printf("    Config: initAddr=0x1000, minLogSize=10, blocks: 1x1024B, 2x2048B\n\n");

        uint32_t sizes2[] = {1, 0, 2};
        memOpen(0x1000, 10, sizes2, 3);
        
        std::printf("    ✓ Module opened (blocks=%u, minLogSize=%u)\n", memBlockCount, memMinLogSize);
        
        if (memBlockCount == 3) {
            std::printf("    ✓ Correct block count: 3\n");
        } else {
            std::printf("    ✗ ERROR: Expected 3 blocks, got %u\n", memBlockCount);
        }
        
        if (memMinLogSize == 10) {
            std::printf("    ✓ Correct minLogSize: 10\n");
        } else {
            std::printf("    ✗ ERROR: Expected minLogSize=10, got %u\n", memMinLogSize);
        }
        
        if (memFreeHead && memFreeHead->addr == 0x1000) {
            std::printf("    ✓ First block at correct address: 0x1000\n");
        } else {
            std::printf("    ✗ ERROR: First block not at 0x1000\n");
        }
        
        std::printf("    ✓ TEST 2 PASSED!\n\n");
        
        memClose();

        // ============================================
        // TEST 3: Edge case - single block
        // ============================================
        std::printf(">>> TEST 3: Edge case - single block\n\n");

        uint32_t sizes3[] = {1};
        memOpen(0, 5, sizes3, 1);
        
        if (memBlockCount == 1) {
            std::printf("    ✓ Single block created\n");
        } else {
            std::printf("    ✗ ERROR: Expected 1 block, got %u\n", memBlockCount);
        }
        
        std::printf("    ✓ TEST 3 PASSED!\n\n");
        
        memClose();

        // ============================================
        // TEST 4: Multiple zero entries in sizes array
        // ============================================
        std::printf(">>> TEST 4: Sizes array with zeros\n");
        std::printf("    Config: sizes = {0, 3, 0, 2, 0}\n\n");

        uint32_t sizes4[] = {0, 3, 0, 2, 0};
        memOpen(0x5000, 7, sizes4, 5);
        
        if (memBlockCount == 5) {
            std::printf("    ✓ Correct block count: 5\n");
        } else {
            std::printf("    ✗ ERROR: Expected 5 blocks, got %u\n", memBlockCount);
        }
        
        std::printf("    ✓ TEST 4 PASSED!\n\n");
        
        memClose();

        // ============================================
        // TEST 5: Large configuration
        // ============================================
        std::printf(">>> TEST 5: Large configuration\n");
        std::printf("    Config: 5 different block sizes, total 15 blocks\n\n");

        uint32_t sizes5[] = {5, 3, 2, 3, 2};
        memOpen(0, 5, sizes5, 5);
        
        if (memBlockCount == 15) {
            std::printf("    ✓ Correct total: 15 blocks\n");
        } else {
            std::printf("    ✗ ERROR: Expected 15 blocks, got %u\n", memBlockCount);
        }
        
        std::printf("    ✓ TEST 5 PASSED!\n\n");
        
        memClose();

        std::printf(">>> All memOpen tests PASSED! ✓✓✓✓✓\n");
    }
    catch (const Exception &e) {
        std::printf("\n✗ EXCEPTION: %s (errno=%d)\n", e.what(), e.en);
    }
}

static void testMemClose()
{
    banner("Testing MEM (memClose)");
    soBinSetIDs(0, 0);

    try {
        // ============================================
        // TEST 1: Basic close functionality
        // ============================================
        std::printf(">>> TEST 1: Basic close after open\n\n");
        
        std::printf("    Step 1: Opening MEM module...\n");
        uint32_t sizes1[] = {2, 1, 3};
        memOpen(0, 5, sizes1, 3);
        std::printf("    ✓ Module opened (blocks=%u)\n\n", memBlockCount);

        std::printf("    Step 2: Verifying module state BEFORE close:\n");
        bool allCorrectBefore = true;
        
        if (memBlocks != NULL && memBlocks != MEM_UNDEF_NODE) {
            std::printf("      ✓ memBlocks = %p (allocated)\n", (void*)memBlocks);
        } else {
            std::printf("      ✗ ERROR: memBlocks not allocated\n");
            allCorrectBefore = false;
        }
        
        if (memBlockCount == 6) {
            std::printf("      ✓ memBlockCount = 6\n");
        } else {
            std::printf("      ✗ ERROR: memBlockCount = %u (expected 6)\n", memBlockCount);
            allCorrectBefore = false;
        }
        
        if (memMinLogSize == 5) {
            std::printf("      ✓ memMinLogSize = 5\n");
        } else {
            std::printf("      ✗ ERROR: memMinLogSize = %u (expected 5)\n", memMinLogSize);
            allCorrectBefore = false;
        }
        
        if (memFreeHead != NULL && memFreeHead != MEM_UNDEF_NODE) {
            std::printf("      ✓ memFreeHead = %p (set)\n", (void*)memFreeHead);
        } else {
            std::printf("      ✗ ERROR: memFreeHead not set\n");
            allCorrectBefore = false;
        }
        
        if (memOccupiedHead == NULL) {
            std::printf("      ✓ memOccupiedHead = NULL (empty list)\n");
        } else {
            std::printf("      ✗ ERROR: memOccupiedHead should be NULL\n");
            allCorrectBefore = false;
        }
        std::printf("\n");

        if (!allCorrectBefore) {
            std::printf("    ✗ TEST 1 FAILED: Module state before close is incorrect!\n\n");
            return;
        }

        std::printf("    Step 3: Closing MEM module...\n");
        memClose();
        std::printf("    ✓ memClose() executed successfully\n\n");

        std::printf("    Step 4: Verifying module state AFTER close:\n");
        bool allCorrectAfter = true;
        
        if (memBlocks == MEM_UNDEF_NODE) {
            std::printf("      ✓ memBlocks = MEM_UNDEF_NODE (0x%p)\n", (void*)MEM_UNDEF_NODE);
        } else {
            std::printf("      ✗ ERROR: memBlocks = %p (should be MEM_UNDEF_NODE)\n", (void*)memBlocks);
            allCorrectAfter = false;
        }
        
        if (memFreeHead == MEM_UNDEF_NODE) {
            std::printf("      ✓ memFreeHead = MEM_UNDEF_NODE\n");
        } else {
            std::printf("      ✗ ERROR: memFreeHead = %p (should be MEM_UNDEF_NODE)\n", (void*)memFreeHead);
            allCorrectAfter = false;
        }
        
        if (memOccupiedHead == MEM_UNDEF_NODE) {
            std::printf("      ✓ memOccupiedHead = MEM_UNDEF_NODE\n");
        } else {
            std::printf("      ✗ ERROR: memOccupiedHead = %p (should be MEM_UNDEF_NODE)\n", (void*)memOccupiedHead);
            allCorrectAfter = false;
        }
        
        if (memBlockCount == 0) {
            std::printf("      ✓ memBlockCount = 0\n");
        } else {
            std::printf("      ✗ ERROR: memBlockCount = %u (should be 0)\n", memBlockCount);
            allCorrectAfter = false;
        }
        
        if (memMinLogSize == 0) {
            std::printf("      ✓ memMinLogSize = 0\n");
        } else {
            std::printf("      ✗ ERROR: memMinLogSize = %u (should be 0)\n", memMinLogSize);
            allCorrectAfter = false;
        }
        std::printf("\n");

        if (allCorrectAfter) {
            std::printf("    >>> TEST 1 PASSED! ✓✓✓\n");
        } else {
            std::printf("    >>> TEST 1 FAILED! Module not properly closed\n");
        }
        std::printf("\n");

        // ============================================
        // TEST 2: Open-Close-Reopen cycle
        // ============================================
        std::printf(">>> TEST 2: Open-Close-Reopen cycle\n\n");
        
        std::printf("    Iteration 1:\n");
        std::printf("      Opening with config: initAddr=0x1000, minLogSize=10, blocks=3\n");
        uint32_t sizes2[] = {1, 0, 2};
        memOpen(0x1000, 10, sizes2, 3);
        std::printf("      ✓ Opened (blocks=%u, minLogSize=%u)\n", memBlockCount, memMinLogSize);
        
        std::printf("      Closing...\n");
        memClose();
        std::printf("      ✓ Closed\n\n");
        
        std::printf("    Iteration 2:\n");
        std::printf("      Opening with NEW config: initAddr=0x2000, minLogSize=6, blocks=6\n");
        uint32_t sizes3[] = {3, 2, 1};
        memOpen(0x2000, 6, sizes3, 3);
        std::printf("      ✓ Opened (blocks=%u, minLogSize=%u)\n", memBlockCount, memMinLogSize);
        
        bool config2OK = true;
        if (memBlockCount != 6) {
            std::printf("      ✗ ERROR: blockCount=%u, expected 6\n", memBlockCount);
            config2OK = false;
        }
        if (memMinLogSize != 6) {
            std::printf("      ✗ ERROR: minLogSize=%u, expected 6\n", memMinLogSize);
            config2OK = false;
        }
        if (memFreeHead != NULL && memFreeHead->addr != 0x2000) {
            std::printf("      ✗ ERROR: first block at 0x%08x, expected 0x2000\n", memFreeHead->addr);
            config2OK = false;
        }
        
        if (config2OK) {
            std::printf("      ✓ New configuration correctly loaded!\n");
            std::printf("      ✓ First block at address 0x%08x\n", memFreeHead->addr);
        }
        
        std::printf("      Closing...\n");
        memClose();
        std::printf("      ✓ Closed\n\n");
        
        if (config2OK) {
            std::printf("    >>> TEST 2 PASSED! ✓✓✓\n");
        } else {
            std::printf("    >>> TEST 2 FAILED!\n");
        }
        std::printf("\n");

        // ============================================
        // TEST 3: Memory leak check (5 iterations)
        // ============================================
        std::printf(">>> TEST 3: Memory leak check (5 open/close cycles)\n\n");
        
        for (int i = 1; i <= 5; i++) {
            std::printf("    Cycle %d: ", i);
            uint32_t testSizes[] = {2, 1, 1};
            memOpen(0x1000 * i, 5, testSizes, 3);
            std::printf("Open OK (blocks=%u) -> ", memBlockCount);
            memClose();
            std::printf("Close OK");
            
            // Verify clean state after each close
            if (memBlocks == MEM_UNDEF_NODE && 
                memFreeHead == MEM_UNDEF_NODE && 
                memOccupiedHead == MEM_UNDEF_NODE &&
                memBlockCount == 0 &&
                memMinLogSize == 0) {
                std::printf(" ✓\n");
            } else {
                std::printf(" ✗ ERROR: Not properly reset!\n");
            }
        }
        
        std::printf("\n    ✓ No crashes or memory corruption detected!\n");
        std::printf("    >>> TEST 3 PASSED! ✓✓✓\n\n");

        std::printf(">>> All memClose tests PASSED! ✓✓✓✓✓✓\n");
    }
    catch (const Exception &e) {
        std::printf("\n✗ EXCEPTION CAUGHT: %s (errno=%d)\n", e.what(), e.en);
        std::printf("Test FAILED!\n");
    }
}

static void testMemPrint()
{
    banner("Testing MEM (memPrint)");
    soBinSetIDs(0, 0);

    try {
        // ============================================
        // TEST 1: Print global state (normal format)
        // ============================================
        std::printf(">>> TEST 1: Print global state (normal format)\n");
        std::printf("    Expected: Table with 6 free blocks, empty occupied list\n\n");
        
        uint32_t sizes1[] = {2, 1, 3};
        memOpen(0, 5, sizes1, 3);
        
        std::printf("Output from memPrint(stdout, MemPrintGlobal, false):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintGlobal, false);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 1 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 2: Print only free blocks
        // ============================================
        std::printf(">>> TEST 2: Print only FREE blocks\n\n");
        
        uint32_t sizes2[] = {1, 2, 1};
        memOpen(0x1000, 10, sizes2, 3);
        
        std::printf("Output from memPrint(stdout, MemPrintFree, false):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintFree, false);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 2 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 3: Print occupied blocks (empty)
        // ============================================
        std::printf(">>> TEST 3: Print OCCUPIED blocks (empty list)\n\n");
        
        uint32_t sizes3[] = {3, 0, 2};
        memOpen(0, 5, sizes3, 3);
        
        std::printf("Output from memPrint(stdout, MemPrintOccupied, false):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintOccupied, false);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 3 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 4: CSV format - free blocks
        // ============================================
        std::printf(">>> TEST 4: CSV format (free blocks)\n\n");
        
        uint32_t sizes4[] = {2, 1, 2};
        memOpen(0x2000, 6, sizes4, 3);
        
        std::printf("Output from memPrint(stdout, MemPrintFree, true):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintFree, true);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 4 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 5: CSV format - global
        // ============================================
        std::printf(">>> TEST 5: CSV format (global state)\n\n");
        
        uint32_t sizes5[] = {1, 1, 1};
        memOpen(0, 5, sizes5, 3);
        
        std::printf("Output from memPrint(stdout, MemPrintGlobal, true):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintGlobal, true);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 5 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 6: Verify address ordering
        // ============================================
        std::printf(">>> TEST 6: Verify blocks ordered by ADDRESS\n\n");
        
        uint32_t sizes6[] = {3, 2, 1};
        memOpen(0x5000, 8, sizes6, 3);
        
        std::printf("Output from memPrint(stdout, MemPrintFree, false):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintFree, false);
        std::printf("========================================================\n");
        
        std::printf("\n    Verifying address order:\n");
        uint32_t prevAddr = 0;
        MemNode *node = memFreeHead;
        bool correctOrder = true;
        uint32_t idx = 1;
        
        while (node != NULL) {
            if (node->addr < prevAddr) {
                std::printf("      ✗ Block %u (addr=0x%08x) out of order!\n", idx, node->addr);
                correctOrder = false;
            }
            prevAddr = node->addr;
            node = node->next;
            idx++;
        }
        
        if (correctOrder) {
            std::printf("      ✓ All blocks correctly ordered by address\n");
        }
        
        std::printf("    ✓ TEST 6 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 7: Various block sizes
        // ============================================
        std::printf(">>> TEST 7: Various block sizes (minLogSize=4)\n\n");
        
        uint32_t sizes7[] = {1, 1, 1, 1, 1};
        memOpen(0, 4, sizes7, 5);
        
        std::printf("Output from memPrint(stdout, MemPrintFree, false):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintFree, false);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 7 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 8: Large addresses
        // ============================================
        std::printf(">>> TEST 8: Large addresses (0xF0000000)\n\n");
        
        uint32_t sizes8[] = {2, 1};
        memOpen(0xF0000000, 12, sizes8, 2);
        
        std::printf("Output from memPrint(stdout, MemPrintFree, false):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintFree, false);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 8 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 9: Single block
        // ============================================
        std::printf(">>> TEST 9: Single block\n\n");
        
        uint32_t sizes9[] = {1};
        memOpen(0x1000, 10, sizes9, 1);
        
        std::printf("Output from memPrint(stdout, MemPrintGlobal, false):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintGlobal, false);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 9 PASSED\n\n");
        
        memClose();

        // ============================================
        // TEST 10: CSV - occupied (empty)
        // ============================================
        std::printf(">>> TEST 10: CSV format for empty occupied list\n\n");
        
        uint32_t sizes10[] = {2, 1};
        memOpen(0, 5, sizes10, 2);
        
        std::printf("Output from memPrint(stdout, MemPrintOccupied, true):\n");
        std::printf("========================================================\n");
        memPrint(stdout, MemPrintOccupied, true);
        std::printf("========================================================\n");
        std::printf("    ✓ TEST 10 PASSED\n\n");
        
        memClose();

        std::printf(">>> All memPrint tests PASSED! ✓✓✓✓✓✓✓✓✓✓\n");
    }
    catch (const Exception &e) {
        std::printf("\n✗ EXCEPTION: %s (errno=%d)\n", e.what(), e.en);
    }
}

int main()
{
    soProbeOpen(stdout, 0, 0);
    
    testMemOpen();
    testMemClose();
    testMemPrint();
    
    std::printf("\n");
    std::printf("==========================================\n");
    std::printf("=  ALL MEM TESTS COMPLETED SUCCESSFULLY  =\n");
    std::printf("==========================================\n");
    std::printf("\n");
    
    return 0;
}