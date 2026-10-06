/*
 *  \author Mafalda
 */

 #include "somm25nm.h"
 #include "exception.h"
 #include "dbc.h"
 
 #include <stdio.h>
 #include <stdint.h>
 #include <errno.h>
 
 namespace group 
 {
     void simPrint(FILE *fout, uint32_t which, bool csv)
     {
         require(fout != nullptr && fileno(fout) != -1, "simPrint: invalid output stream");
         require(simTime != SIM_UNDEF_TIME, "simPrint: SIM module must be open");
         require(simProcessorState != SIM_UNDEF_POINTER, "simPrint: processor table not initialized");
         require(simProcessorCount > 0, "simPrint: invalid processor count");
         require(simIdleHead != SIM_UNDEF_INDEX && simIdleTail != SIM_UNDEF_INDEX,
                  "simPrint: idle list not initialized");
 
         bool *isIdle = new (std::nothrow) bool[simProcessorCount];
         if (isIdle == nullptr)
         {
             errno = ENOMEM;
             throw Exception(errno, __func__);
         }
         for (uint32_t i = 0; i < simProcessorCount; i++)
             isIdle[i] = false;
 
         uint16_t idx = simIdleHead;
         while (idx < simProcessorCount)
         {
             isIdle[idx] = true;
             uint16_t next = simProcessorState[idx].next;
             if (next == simProcessorCount)
                 break;
             idx = next;
         }
 
         if (csv)
         {
             fprintf(fout, "simTime;processorCount;idleHead;idleTail\n");
             fprintf(fout, "%.6f;%u;%hu;%hu\n",
                     simTime, simProcessorCount, simIdleHead, simIdleTail);
 
             fprintf(fout, "index;status;value\n");
             for (uint32_t i = 0; i < simProcessorCount; i++)
             {
                 if (isIdle[i])
                 {
                     fprintf(fout, "%u;IDLE;%hu\n", i, simProcessorState[i].next);
                 }
                 else
                 {
                     fprintf(fout, "%u;RUNNING;%hu\n", i, simProcessorState[i].pid);
                 }
             }
         }
         else
         {
             fprintf(fout, "SIM - Simulation state\n");
             fprintf(fout, "  Time              : %.6f\n", simTime);
             fprintf(fout, "  Processor count   : %u\n", simProcessorCount);
             fprintf(fout, "  Idle list head    : %hu\n", simIdleHead);
             fprintf(fout, "  Idle list tail    : %hu\n", simIdleTail);
             fprintf(fout, "\n");
             fprintf(fout, "Processors:\n");
             for (uint32_t i = 0; i < simProcessorCount; i++)
             {
                 if (isIdle[i])
                 {
                     fprintf(fout, "  #%u: IDLE (next=%hu)\n",
                              i, simProcessorState[i].next);
                 }
                 else
                 {
                     fprintf(fout, "  #%u: RUNNING (pid=%hu)\n",
                              i, simProcessorState[i].pid);
                 }
             }
         }
 
         delete[] isIdle;
 
         if (which & SimPrintJob)
             jobPrint(fout, csv);
 
         if (which & SimPrintPct)
             pctPrint(fout, csv);
 
         if (which & SimPrintFeq)
             feqPrint(fout, csv);
 
         if (which & SimPrintRdy)
             rdyPrint(fout, csv);
 
         if (which & SimPrintSwp)
             swpPrint(fout, csv);
 
         if (which & SimPrintMemGlobal)
             memPrint(fout, MemPrintGlobal, csv);
 
         if (which & SimPrintMemFreeOnly)
             memPrint(fout, MemPrintFree, csv);
 
         if (which & SimPrintMemOccupiedOnly)
             memPrint(fout, MemPrintOccupied, csv);
     }
 } 
 