/*
 *  \author Mafalda
 */

#include "rdy.h"
#include "exception.h"
#include "dbc.h"
#include <errno.h>

namespace group
{
    void rdyOpen(RdySchedulingPolicy policy)
    {
        require(rdyHead == RDY_UNDEF_NODE, "RDY module must be closed");
        require(policy == SPN || policy == HRRN || policy == SRT,
                "Invalid scheduling policy for RDY module");

        rdyHead  = nullptr;
        rdyPolicy = policy;

        ensure(rdyHead == nullptr, "RDY head must be nullptr after rdyOpen");
        ensure(rdyPolicy == policy, "RDY policy not set correctly in rdyOpen");
    }
}
