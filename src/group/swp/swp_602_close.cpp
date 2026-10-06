/*
José Francisco Teixeira Mota - 113985
 */

#include "swp.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    void swpClose()
    {
        SwpNode *curr = swpHead;
        while (curr != nullptr)
        {
            SwpNode *next = curr->next;
            delete curr;
            curr = next;
        }

        // modulo no estado closed
        swpHead   = SWP_UNDEF_NODE;
        swpTail   = SWP_UNDEF_NODE;
        swpPolicy = SWP_UNDEF_POLICY;
    }
} // end of namespace group

