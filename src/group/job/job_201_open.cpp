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

    void jobOpen()
    {
        // Initialize the job queue to empty state
        jobHead = nullptr;
    }

// ================================================================================== //

} // end of namespace group

