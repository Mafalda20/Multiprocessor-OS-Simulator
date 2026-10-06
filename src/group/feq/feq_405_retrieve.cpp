/*
José Francisco Teixeira Mota - 113985
 */

#include "feq.h"

#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group 
{

// ================================================================================== //

    bool feqRetrieve(double *time, FeqEventType *type, uint32_t *xid, bool blocking)
    {
        (void)blocking;

        // se a fila está vazia não há evento para devolver
        if (feqHead == nullptr)
            return false;

        FeqNode *node = feqHead;
        feqHead = node->next;

        *time = node->time;
        *type = node->type;
        *xid  = node->xid;

        delete node;

        return true;
    }


// ================================================================================== //

} // end of namespace group

