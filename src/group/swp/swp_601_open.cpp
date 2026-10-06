/*
 José Francisco Teixeira Mota - 113985
 */

#include "swp.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    void swpOpen(SwpSwappingPolicy policy)
    {
        if (policy != FirstFit && policy != FirstBest)
            throw Exception(EINVAL, __func__);

        swpPolicy = policy;
        swpHead = nullptr;
        swpTail = nullptr;
    }
} // end of namespace group

