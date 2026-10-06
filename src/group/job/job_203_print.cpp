/*
 *  \author: João Pinto
 */

#include "job.h"
#include "exception.h"

#include <stdint.h>
#include <stdio.h>

namespace group
{

// ================================================================================== //

    void jobPrint(FILE *fout, bool csv)
    {
        if (csv)
        {
            // CSV format with semicolon as separator
            fprintf(fout, "jid;submissionTime;finishTime;memSize;nextBurstIndex;bursts\n");
            
            JobNode *current = jobHead;
            while (current != nullptr)
            {
                fprintf(fout, "%08x;%.1f;%.1f;%u;%u;",
                    current->jid,
                    current->submissionTime,
                    current->finishTime,
                    current->memSize,
                    current->nextBurstIndex);
                
                // Print bursts
                for (uint32_t i = 0; i < JOB_MAX_BURSTS; i++)
                {
                    if (current->bursts[i] != 0.0)
                    {
                        fprintf(fout, "%.1f", current->bursts[i]);
                        // Check if there's another non-zero burst
                        bool hasMore = false;
                        for (uint32_t j = i + 1; j < JOB_MAX_BURSTS; j++)
                        {
                            if (current->bursts[j] != 0.0)
                            {
                                hasMore = true;
                                break;
                            }
                        }
                        if (hasMore) fprintf(fout, ",");
                    }
                }
                fprintf(fout, "\n");
                
                current = current->next;
            }
        }
        else
        {
            // Human-readable format
            fprintf(fout, "Job Queue:\n");
            fprintf(fout, "+----------+----------------+-------------+---------+------------+\n");
            fprintf(fout, "|   JID    | Submission Time| Finish Time | MemSize | NextBurst  |\n");
            fprintf(fout, "+----------+----------------+-------------+---------+------------+\n");
            
            JobNode *current = jobHead;
            while (current != nullptr)
            {
                fprintf(fout, "| %08x |     %10.1f |  %10.1f | %7u | %10u |\n",
                    current->jid,
                    current->submissionTime,
                    current->finishTime,
                    current->memSize,
                    current->nextBurstIndex);
                
                current = current->next;
            }
            fprintf(fout, "+----------+----------------+-------------+---------+------------+\n");
        }
    }

// ================================================================================== //

} // end of namespace group

