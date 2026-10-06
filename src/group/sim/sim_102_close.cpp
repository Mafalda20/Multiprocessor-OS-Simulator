/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "somm25nm.h"
#include <stdlib.h>

namespace group 
{
    void simClose(bool closeSatelliteModules) 
    {
        // Se solicitado, fechar módulos satélite (ordem inversa de abertura)
        if (closeSatelliteModules)
        {
            memClose();
            swpClose();
            rdyClose();
            feqClose();
            pctClose();
            jobClose();
        }
        
        // Libertar memória alocada para o array de processadores
        if (simProcessorState != SIM_UNDEF_POINTER)
        {
            delete[] simProcessorState;
        }
        
        // Redefinir variáveis globais para o estado fechado
        simProcessorState = SIM_UNDEF_POINTER;
        simTime = SIM_UNDEF_TIME;
        simProcessorCount = 0;
        simIdleHead = SIM_UNDEF_INDEX;
        simIdleTail = SIM_UNDEF_INDEX;
    }
} // end of namespace group

