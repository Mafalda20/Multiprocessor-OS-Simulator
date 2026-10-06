/*
 *  \author João Pinto
 */

#include "job.h"
#include "exception.h"

#include <stdint.h>
#include <stdio.h>

namespace group
{
    void jobGet(uint32_t jid, JobField field, void *value)
    {
        // Validate parameters
        if (value == nullptr)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Find the job node
        JobNode *current = jobHead;
        while (current != nullptr && current->jid != jid)
        {
            current = current->next;
        }
        
        if (current == nullptr)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Get the requested field
        switch (field)
        {
            case JobSubmissionTime:
                *(double*)value = current->submissionTime;
                break;
                
            case JobFinishTime:
                *(double*)value = current->finishTime;
                break;
                
            case JobMemSize:
                *(uint32_t*)value = current->memSize;
                break;
                
            case JobNextBurstIndex:
                *(uint32_t*)value = current->nextBurstIndex;
                break;
                
            case JobNextBurstDuration:
            {
                uint32_t idx = current->nextBurstIndex;
                if (idx >= JOB_MAX_BURSTS || current->bursts[idx] == 0.0)
                {
                    *(double*)value = 0.0;
                }
                else
                {
                    double duration = current->bursts[idx];
                    
                    // Check if this is the last burst (last CPU burst)
                    // Last burst should be negative according to spec
                    bool isLastBurst = true;
                    for (uint32_t i = idx + 1; i < JOB_MAX_BURSTS; i++)
                    {
                        if (current->bursts[i] != 0.0)
                        {
                            isLastBurst = false;
                            break;
                        }
                    }
                    
                    if (isLastBurst && idx % 2 == 0) // CPU burst at even index
                    {
                        *(double*)value = -duration;
                    }
                    else
                    {
                        *(double*)value = duration;
                    }
                }
                break;
            }
                
            default:
                throw Exception(EINVAL, __func__);
        }
    }

} // end of namespace somm25nm
