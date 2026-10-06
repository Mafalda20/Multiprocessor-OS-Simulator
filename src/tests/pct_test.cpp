/*
 *  PCT Module Test
 *
 *  \author Rodrigo Lopes, nmec: 113811
 * Correções : José Francisco Teixeira Mota - 113985
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <termios.h>

#include <string>
#include <iostream>

#include "somm25nm.h"


/* ******************************************** */
/* print help message */
static void printUsage(const char *cmd_name)
{
    printf("Sinopsis: %s [OPTIONS]\n"
           "  OPTIONS:\n"
           "  -o outfile     --- set log file (default: stdout)\n"
           "  -O outfile     --- set probbing file (default: stdout)\n"
           "  -P num-num     --- set probe ID range (default: 0-0)\n"
           "  -A num-num     --- add range of IDs to probe configuration\n"
           "  -R num-num     --- remove range of IDs from probe configuration\n"
           "  -b             --- set bin selection map to 100-799\n"
           "  -g             --- set bin selection map to 0-0 (default)\n"
           "  -a num-num     --- add range of IDs to bin selection map\n"
           "  -r num-num     --- remove range of IDs from bin selection map\n"
           "  -n             --- run without pause (default: pause)\n"
           "  -h             --- print this help\n", cmd_name);
}

bool noPause()
{
   return true;
}

bool termPause()
{
    static bool firstTime = true;
    static struct termios prev, cur;
    if (firstTime)
    {
        firstTime = false;
        tcgetattr(STDIN_FILENO, &prev);
        cur = prev;
        cur.c_lflag &= (~ICANON);
        tcsetattr(STDIN_FILENO, TCSANOW, &cur);
    }

    printf("Continue (Y/n)? "); fflush(stdout);
    while (true)
    {
        int res = getchar();
        if (res == '\n') break;
        printf("\n");
        if (res == 'n' or res == 'N') return false;
        if (res == 'y' or res == 'Y') break;
        printf("Bad option! Continue (Y/n)? "); fflush(stdout);
    }
    return true;
}

bool (*pauseSim)(void) = termPause;

void banner(const char *msg)
{
    fprintf(stdout, "\n\e[33;1m%s\e[0m\n\n", msg);
}

/* ******************************************** */
/* The main function */
int main(int argc, char *argv[])
{
    const char *progName = basename(argv[0]); 

    /* by default, send probing to stdout */
    FILE *fout = stdout;
    soProbeOpen(stdout, 0, 0);

    /* process command line options */
    int opt;
    while ((opt = getopt(argc, argv, "o:O:P:A:R:nbga:r:h")) != -1)
    {
        switch (opt)
        {
            case 'o':          // set output file
            {
                if ((fout = fopen(optarg, "w")) == NULL)
                {
                    fprintf(stderr, "%s: Bad argument (\"%s\"): fail opening file.\n", progName, optarg);
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'O':          /* set probbing file */
            {
                soProbeFile(optarg);
                break;
            }
            case 'P':          /* set ID range to probing system */
            {
                uint32_t lower, upper;
                uint32_t cnt = 0;
                if ( (sscanf(optarg, "%d%*[,-]%d %n", &lower, &upper, &cnt) != 2) 
                        or (cnt != strlen(optarg)) )
                {
                    fprintf(stderr, "%s: Bad argument to '-P' option.\n", progName);
                    printUsage(progName);
                    return EXIT_FAILURE;
                }
                soProbeSetIDs(lower, upper);
                break;
            }
            case 'A':          /* add IDs to probe conf */
            {
                uint32_t lower, upper;
                uint32_t cnt = 0;
                if ( (sscanf(optarg, "%d%*[,-]%d %n", &lower, &upper, &cnt) != 2) 
                        or (cnt != strlen(optarg)) )
                {
                    fprintf(stderr, "%s: Bad argument to '-A' option.\n", progName);
                    printUsage(progName);
                    return EXIT_FAILURE;
                }
                soProbeAddIDs(lower, upper);
                break;
            }
            case 'R':          /* remove IDs from probe conf */
            {
                uint32_t lower, upper;
                uint32_t cnt = 0;
                if ( (sscanf(optarg, "%d-%d %n", &lower, &upper, &cnt) != 2) 
                        or (cnt != strlen(optarg)) )
                {
                    fprintf(stderr, "%s: Bad argument to '-R' option.\n", progName);
                    printUsage(progName);
                    return EXIT_FAILURE;
                }
                soProbeRemoveIDs(lower, upper);
                break;
            }
            case 'n':    // set no pause mode
            {
                pauseSim = noPause;
                break;
            }
            case 'b':  // set binary mode
            {
                soBinSetIDs(0, 999);
                break;
            }
            case 'g':  // set binary mode
            {
                soBinSetIDs(0, 0);
                break;
            }
            case 'a':          /* add IDs to bin conf */
            {
                uint32_t lower, upper;
                uint32_t cnt = 0;
                if ( (sscanf(optarg, "%d%*[,-]%d %n", &lower, &upper, &cnt) != 2) 
                        or (cnt != strlen(optarg)) )
                {
                    fprintf(stderr, "%s: Bad argument to '-a' option.\n", progName);
                    printUsage(progName);
                    return EXIT_FAILURE;
                }
                soBinAddIDs(lower, upper);
                break;
            }
            case 'r':          /* remove IDs from bin conf */
            {
                uint32_t lower, upper;
                uint32_t cnt = 0;
                if ( (sscanf(optarg, "%d-%d %n", &lower, &upper, &cnt) != 2) 
                        or (cnt != strlen(optarg)) )
                {
                    fprintf(stderr, "%s: Bad argument to '-r' option.\n", progName);
                    printUsage(progName);
                    return EXIT_FAILURE;
                }
                soBinRemoveIDs(lower, upper);
                break;
            }
            case 'h':
            {
                printUsage(progName);
                return 0;
            }
            default:
            {
                fprintf(stderr, "%s: Wrong option (\"-%c\".\n", progName, opt);
                printUsage(progName);
                return EXIT_FAILURE;
            }
        }
    }

    /* set fout stream as no buffered */
    setvbuf(fout, NULL, _IONBF, 0);

    try {
        /* init test */
        banner("Starting the PCT module");
        pctOpen(1000, 10);  // Base PID = 1000, Count = 10 processes

        /* test pctPrint when PCT is empty */
        banner("Printing PCT in CSV mode (empty)");
        pctPrint(stdout, true);
        banner("Printing PCT in normal mode (empty)");
        pctPrint(stdout);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* test pctNew - create new processes */
        banner("Creating new processes");
        uint16_t pid1 = pctNew(0x12345678);
        fprintf(fout, "Created process with PID: %u (JID: 0x12345678)\n", pid1);
        
        uint16_t pid2 = pctNew(0xABCDEF00);
        fprintf(fout, "Created process with PID: %u (JID: 0xABCDEF00)\n", pid2);
        
        uint16_t pid3 = pctNew(0xDEADBEEF);
        fprintf(fout, "Created process with PID: %u (JID: 0xDEADBEEF)\n", pid3);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* test pctPrint with some processes */
        banner("Printing PCT in CSV mode (with 3 processes)");
        pctPrint(stdout, true);
        banner("Printing PCT in normal mode (with 3 processes)");
        pctPrint(stdout);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* test pctGet - getting process information */
        banner("Testing pctGet - retrieving process information");
        uint32_t jid;
        pctGet(pid1, PctJid, &jid);
        fprintf(fout, "PID %u has JID: 0x%08X\n", pid1, jid);
        
        uint32_t memAddr;
        pctGet(pid1, PctMemAddr, &memAddr);
        if (memAddr == PCT_UNDEF_ADDRESS)
            fprintf(fout, "PID %u has memAddr: UNDEF\n", pid1);
        else
            fprintf(fout, "PID %u has memAddr: 0x%08X\n", pid1, memAddr);
        
        PctProcessState state;
        pctGet(pid2, PctState, &state);
        fprintf(fout, "PID %u has state: %d (NEW=%d)\n", pid2, state, NEW);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* test pctSet - setting process information */
        banner("Testing pctSet - changing process state and memory address");
        PctProcessState newState = READY;
        pctSet(pid1, PctState, &newState);
        fprintf(fout, "Changed PID %u state to READY\n", pid1);
        
        uint32_t newMemAddr = 0x1000;
        pctSet(pid1, PctMemAddr, &newMemAddr);
        fprintf(fout, "Changed PID %u memAddr to 0x%08X\n", pid1, newMemAddr);
        
        newState = RUNNING;
        pctSet(pid2, PctState, &newState);
        fprintf(fout, "Changed PID %u state to RUNNING\n", pid2);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* print PCT after modifications */
        banner("Printing PCT after modifications");
        pctPrint(stdout, true);
        pctPrint(stdout);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* test pctDelete - delete a process */
        banner("Testing pctDelete - removing process");
        fprintf(fout, "Deleting process with PID %u\n", pid2);
        pctDelete(pid2);
        
        banner("Printing PCT after deletion");
        pctPrint(stdout);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
        }

        /* test pctNew after deletion - reusing PID */
        banner("Creating new process to test PID reuse");
        uint16_t pid4 = pctNew(0xCAFEBABE);
        fprintf(fout, "Created process with PID: %u (JID: 0xCAFEBABE)\n", pid4);
        fprintf(fout, "Notice: PID %u was reused (deleted PID was %u)\n", pid4, pid2);
        
        banner("Printing PCT after PID reuse");
        pctPrint(stdout);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* test filling the PCT table */
        banner("Filling the PCT table (10 processes max)");
        uint16_t pids[10];
        int count = 3; // Already have 3 processes (pid1, pid3, pid4)
        pids[0] = pid1;
        pids[1] = pid3;
        pids[2] = pid4;
        
        for (int i = count; i < 10; i++)
        {
            uint32_t testJid = 0x10000000 + i;
            pids[i] = pctNew(testJid);
            fprintf(fout, "Created process %d with PID: %u (JID: 0x%08X)\n", i+1, pids[i], testJid);
        }
        
        banner("Printing full PCT table");
        pctPrint(stdout);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* test pctNew when table is full - should throw exception */
        banner("Testing pctNew when table is full (should fail)");
        try {
            uint16_t pidFail = pctNew(0xFFFFFFFF);
            fprintf(fout, "ERROR: Should have thrown exception but created PID %u\n", pidFail);
        } catch (Exception &e) {
            fprintf(fout, "SUCCESS: Exception caught as expected: %s\n", e.what());
        }
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* test changing states of multiple processes */
        banner("Testing state transitions");
        newState = BLOCKED;
        pctSet(pids[0], PctState, &newState);
        fprintf(fout, "Changed PID %u to BLOCKED\n", pids[0]);
        
        newState = S_READY;
        pctSet(pids[1], PctState, &newState);
        fprintf(fout, "Changed PID %u to S_READY\n", pids[1]);
        
        newState = ENDED;
        pctSet(pids[2], PctState, &newState);
        fprintf(fout, "Changed PID %u to ENDED\n", pids[2]);
        
        banner("Printing PCT after state changes");
        pctPrint(stdout);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        /* end simulation */
        banner("Testing pctClose - closing the PCT module");
        pctClose();
        fprintf(fout, "PCT module closed successfully\n");
        
        /* verify the module is closed */
        banner("Verifying PCT is closed");
        if (pctTable == PCT_UNDEF_TABLE)
            fprintf(fout, "SUCCESS: pctTable is PCT_UNDEF_TABLE\n");
        else
            fprintf(fout, "ERROR: pctTable is not PCT_UNDEF_TABLE\n");
            
        if (pctPidCount == 0)
            fprintf(fout, "SUCCESS: pctPidCount is 0\n");
        else
            fprintf(fout, "ERROR: pctPidCount is %u (should be 0)\n", pctPidCount);

        /* test reopening the module */
        banner("Testing pctOpen again (should work after close)");
        pctOpen(2000, 5);  // Base PID = 2000, Count = 5 processes
        fprintf(fout, "PCT module reopened with base=%u, count=%u\n", pctPidBase, pctPidCount);
        
        banner("Printing reopened PCT (should be empty)");
        pctPrint(stdout);
        if (!pauseSim()) {
            banner("PCT Module Test Complete!");
            return EXIT_SUCCESS;
}

        banner("Closing PCT module again");
        pctClose();

        banner("PCT Module Test Complete!");
        
    } catch (Exception &e) {
        fprintf(stderr, "Unexpected exception: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
