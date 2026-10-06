/*
 *  \author João Pinto
 */

#include "mem.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    uint32_t memBiggestFreeBlock()
    {
        uint32_t maxSize = 0;
        
        // Percorrer lista de blocos livres
        MemNode *current = memFreeHead;
        while (current != nullptr)
        {
            // Calcular tamanho do bloco (2^logSize)
            uint32_t blockSize = 1U << current->logSize;
            
            if (blockSize > maxSize)
            {
                maxSize = blockSize;
            }
            
            current = current->next;
        }
        
        return maxSize;
    }
} // end of namespace group


