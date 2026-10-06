/*
 *  \author António Caetano
 */

#include "mem.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    void memClose() 
    {
        /* TODO POINT: Replace next instruction with your code */

        // 1. Libertar o array de blocos se estiver alocado
        if (memBlocks != NULL && memBlocks != MEM_UNDEF_NODE) {
            delete[] memBlocks;  
        }
        
        // 2. Marcar todas as variáveis como "fechadas" 
        memBlocks = MEM_UNDEF_NODE;
        memFreeHead = MEM_UNDEF_NODE;
        memOccupiedHead = MEM_UNDEF_NODE;
        
        // 3. Reset de contadores
        memBlockCount = 0;
        memMinLogSize = 0;
    }
} // end of namespace group


