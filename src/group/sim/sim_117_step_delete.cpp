/*
 *  \author Antonio Caetano
 */

#include "somm25nm.h"

namespace group
{
    void simStepDelete(uint16_t pid)
    {
        /* TODO POINT: Replace next instruction with your code */
         // Obter o endereço de memória do processo
        uint32_t memAddr;
        pctGet(pid, PctMemAddr, &memAddr);
        
        // Libertar a memória ocupada pelo processo
        if (memAddr != PCT_UNDEF_ADDRESS)
        {
            memFree(memAddr);
        }
        
        // Remover o processo da tabela PCT
        pctDelete(pid);
        
        // Tentar fazer swap-in de um processo swapped-out
        if (!swpIsEmpty())
        {
            feqInsert(simTime, ACTIVATE, 0);
        }
    }
} // end of namespace group

