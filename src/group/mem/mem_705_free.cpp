/*
 *  \author João Pinto
 */

#include "mem.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    void memFree(uint32_t addr)
    {
        // Find block in occupied list
        MemNode *prev = nullptr;
        MemNode *current = memOccupiedHead;
        
        while (current != nullptr && current->addr != addr)
        {
            prev = current;
            current = current->next;
        }
        
        // Block not found in occupied list
        if (current == nullptr)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Remove from occupied list
        if (prev == nullptr)
        {
            // Block is at the head
            memOccupiedHead = current->next;
        }
        else
        {
            prev->next = current->next;
        }
        
        // Mark as free
        current->pid = 0;
        current->next = nullptr;
        
        // Insert into free list (sorted by logSize then by address)
        MemNode *prevFree = nullptr;
        MemNode *currFree = memFreeHead;
        
        while (currFree != nullptr)
        {
            // Sort first by logSize, then by address
            if (currFree->logSize > current->logSize ||
                (currFree->logSize == current->logSize && currFree->addr > current->addr))
            {
                break;
            }
            prevFree = currFree;
            currFree = currFree->next;
        }
        
        if (prevFree == nullptr)
        {
            // Insert at head
            current->next = memFreeHead;
            memFreeHead = current;
        }
        else
        {
            // Insert in middle or end
            current->next = currFree;
            prevFree->next = current;
        }
    }
} // end of namespace group


