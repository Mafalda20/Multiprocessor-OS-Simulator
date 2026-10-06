/*
 *  \author António Caetano
 */

#include "feq.h"
#include "exception.h"

#include <stdint.h>
#include <stdio.h>

namespace group 
{

// ================================================================================== //

    void feqOpen()
    {
        /* TODO POINT: Replace next instruction with your code */

        // O módulo deve estar fechado para ser aberto
         if (feqHead != FEQ_UNDEF_NODE)
        {
            throw Exception(EBUSY, __func__);
        }

        // Inicializar o módulo como aberto e vazio
        feqHead = NULL;
    }

// ================================================================================== //

} // end of namespace group

