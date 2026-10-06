/*
 *  \author João Pinto
 */

#include "pct.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

namespace group
{
    void pctDelete(uint16_t pid)
    {
        // Validate PID range
        if (pid < pctPidBase || pid >= pctPidBase + pctPidCount)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Calculate index in table
        uint16_t index = pid - pctPidBase;
        
        // Check if process exists
        if (pctTable[index] == NULL)
        {
            throw Exception(EINVAL, __func__);
        }
        
        // Free the node
        free(pctTable[index]);
        
        // Mark as free
        pctTable[index] = NULL;
    }
} // end of namespace group
