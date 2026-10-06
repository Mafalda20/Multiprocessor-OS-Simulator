/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "pct.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

namespace group 
{
    void pctClose()
    {
        // Percorrer toda a tabela e libertar os nós alocados
        for (uint16_t i = 0; i < pctPidCount; i++)
        {
            if (pctTable[i] != NULL)
            {
                free(pctTable[i]);
                pctTable[i] = NULL;
            }
        }
        
        // Libertar o array da tabela
        free(pctTable);
        
        // Redefinir variáveis globais para o estado fechado
        pctTable = PCT_UNDEF_TABLE;
        pctLastPid = 0;
        pctPidBase = 0;
        pctPidCount = 0;
    }
} // end of namespace group

