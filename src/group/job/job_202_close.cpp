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

    void jobClose()
    {
        // Free all dynamically allocated nodes
        JobNode *current = jobHead;
        while (current != nullptr)
        {
            JobNode *next = current->next;
            delete current;
            current = next;
        }
        
        // Set to closed state
        jobHead = JOB_UNDEF_NODE;
    }

// ================================================================================== //

} // end of namespace group

