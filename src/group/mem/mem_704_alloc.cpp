/*
 *  \author João Pinto
 */

#include "mem.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>

namespace group
{
    uint32_t memAlloc(uint32_t pid, uint32_t size)
    {
        // Validate parameters
        if (pid == 0 || size == 0)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Calculate minimum log size needed
        uint32_t minLogNeeded = (uint32_t)ceil(log2((double)size));
        
        // Find best fit block - smallest block that fits the request
        MemNode *bestFit = nullptr;
        MemNode *prevBestFit = nullptr;
        MemNode *prev = nullptr;
        MemNode *current = memFreeHead;
        
        while (current != nullptr)
        {
            // Check if block is large enough
            if (current->logSize >= minLogNeeded)
            {
                // First fit or better fit found
                if (bestFit == nullptr || current->logSize < bestFit->logSize)
                {
                    bestFit = current;
                    prevBestFit = prev;
                }
            }
            prev = current;
            current = current->next;
        }
        
        // No suitable block found
        if (bestFit == nullptr)
        {
            return 0;
        }
        
        // Remove block from free list
        if (prevBestFit == nullptr)
        {
            // Block is at the head
            memFreeHead = bestFit->next;
        }
        else
        {
            prevBestFit->next = bestFit->next;
        }
        
        // Assign PID to block
        bestFit->pid = (uint16_t)pid;
        bestFit->next = nullptr;
        
        // Insert into occupied list (sorted by address)
        MemNode *prevOcc = nullptr;
        MemNode *currOcc = memOccupiedHead;
        
        while (currOcc != nullptr && currOcc->addr < bestFit->addr)
        {
            prevOcc = currOcc;
            currOcc = currOcc->next;
        }
        
        if (prevOcc == nullptr)
        {
            // Insert at head
            bestFit->next = memOccupiedHead;
            memOccupiedHead = bestFit;
        }
        else
        {
            // Insert in middle or end
            bestFit->next = currOcc;
            prevOcc->next = bestFit;
        }
        
        return bestFit->addr;
    }
} // end of namespace group


