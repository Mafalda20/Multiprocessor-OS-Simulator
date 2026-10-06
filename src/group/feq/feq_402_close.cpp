/*
 *  \author António Caetano
 */

#include "feq.h"

#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group 
{

// ================================================================================== //

    void feqClose()
    {
        /* TODO POINT: Replace next instruction with your code */

        // O módulo deve estar aberto para ser fechado
        if (feqHead == FEQ_UNDEF_NODE)
        {
            throw Exception(EINVAL, __func__);
        }

         
        // Percorrer a lista e libertar cada nó
        while (feqHead != NULL)
        {
            FeqNode *temp = feqHead;
            feqHead = feqHead->next;
            delete temp;
        }

        // Marcar o módulo como FECHADO
        feqHead = FEQ_UNDEF_NODE;


    }

// ================================================================================== //

} // end of namespace group

