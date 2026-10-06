/*
 *  \author João Pinto
 */

#include "job.h"
#include "exception.h"

#include <stdint.h>
#include <stdio.h>

namespace group
{
    void jobSet(uint32_t jid, JobField field, void *value)
    {
        // Validate parameters
        if (value == nullptr)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Only finishTime and nextBurstIndex are settable
        if (field != JobFinishTime && field != JobNextBurstIndex)
        {
            throw Exception(EACCES, __func__);
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
        
        // Set the requested field
        switch (field)
        {
            case JobFinishTime:
                current->finishTime = *(double*)value;
                break;
                
            case JobNextBurstIndex:
                current->nextBurstIndex = *(uint32_t*)value;
                break;
                
            default:
                throw Exception(EACCES, __func__);
        }
    }

} // end of namespace somm25nm
