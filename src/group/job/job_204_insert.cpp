/*
 *  \author João Pinto
 */

#include "job.h"
#include "exception.h"

#include <stdint.h>
#include <stdio.h>

namespace group
{

// ================================================================================== //

    void jobInsert(uint32_t jid, double submissionTime, uint32_t memSize, double *burstProfile)
    {
        // Validate input parameters
        if (burstProfile == nullptr)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Count and validate burst profile (must be odd number and valid)
        uint32_t burstCount = 0;
        for (uint32_t i = 0; i < JOB_MAX_BURSTS; i++)
        {
            if (burstProfile[i] != 0.0)
            {
                if (burstProfile[i] < 0.0)
                {
                    throw Exception(EINVAL, __func__);
                }
                burstCount = i + 1;
            }
        }
        
        if (burstCount == 0 || burstCount % 2 == 0)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Create new node
        JobNode *newNode = new JobNode;
        if (newNode == nullptr)
        {
            throw Exception(errno, __func__);
        }
        
        // Initialize node
        newNode->jid = jid;
        newNode->submissionTime = submissionTime;
        newNode->finishTime = JOB_UNDEF_TIME;
        newNode->memSize = memSize;
        newNode->nextBurstIndex = 0;
        
        // Copy burst profile
        for (uint32_t i = 0; i < JOB_MAX_BURSTS; i++)
        {
            newNode->bursts[i] = burstProfile[i];
        }
        
        // Insert in sorted order (ascending jid)
        if (jobHead == nullptr || jobHead->jid > jid)
        {
            // Insert at head
            newNode->next = jobHead;
            jobHead = newNode;
        }
        else
        {
            // Find insertion point
            JobNode *current = jobHead;
            while (current->next != nullptr && current->next->jid < jid)
            {
                current = current->next;
            }
            
            // Check for duplicate jid
            if (current->jid == jid || (current->next != nullptr && current->next->jid == jid))
            {
                delete newNode;
                throw Exception(EINVAL, __func__);
            }
            
            // Insert after current
            newNode->next = current->next;
            current->next = newNode;
        }
    }

// ================================================================================== //

} // end of namespace group

