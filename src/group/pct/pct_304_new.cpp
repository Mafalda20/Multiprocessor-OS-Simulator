/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "pct.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

namespace group
{
    uint16_t pctNew(uint32_t jid)
    {
        // Procurar por um PID disponível de forma circular
        uint16_t pid = pctLastPid;
        uint16_t attempts = 0;
        
        while (attempts < pctPidCount)
        {
            // Incrementar PID de forma circular
            pid++;
            if (pid >= pctPidBase + pctPidCount)
            {
                pid = pctPidBase;
            }
            
            // Verificar se o PID está disponível
            uint16_t index = pid - pctPidBase;
            if (pctTable[index] == NULL)
            {
                // PID disponível encontrado
                // Alocar novo nó
                PctNode *newNode = (PctNode *)malloc(sizeof(PctNode));
                if (newNode == NULL)
                {
                    throw Exception(ENOMEM, __func__);
                }
                
                // Inicializar o nó
                newNode->jid = jid;
                newNode->memAddr = PCT_UNDEF_ADDRESS;
                newNode->state = NEW;
                
                // Adicionar à tabela
                pctTable[index] = newNode;
                
                // Atualizar lastPid
                pctLastPid = pid;
                
                return pid;
            }
            
            attempts++;
        }
        
        // Nenhum PID disponível
        throw Exception(EAGAIN, __func__);
    }
} // end of namespace group
