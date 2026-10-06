/*
 *  \author António Caetano
 */

#include "mem.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    void memOpen(uint32_t initAddr, uint32_t minLogSize, uint32_t *sizes, uint32_t cnt)
    {
        /* TODO POINT: Replace next instruction with your code */

        // 1. Calcular total de blocos
        uint32_t totalBlocks = 0;
        for (uint32_t i = 0; i < cnt; i++) {
            totalBlocks += sizes[i];
        }

        if (totalBlocks == 0) {
            throw Exception(EINVAL, __func__);
        }

        // 2. Alocar array de blocos com new[] (C++)
        memBlocks = new MemNode[totalBlocks];  // ← MUDANÇA AQUI

        // 3. Inicializar variáveis globais
        memBlockCount = totalBlocks;
        memMinLogSize = (uint16_t)minLogSize;

        // 4. Criar blocos
        uint32_t currentAddr = initAddr;
        uint32_t blockIndex = 0;

        for (uint32_t i = 0; i < cnt; i++) {
            uint32_t currentLogSize = minLogSize + i;
            uint32_t blockSize = 1U << currentLogSize;  // 2^logSize

            for (uint32_t j = 0; j < sizes[i]; j++) {
                memBlocks[blockIndex].addr = currentAddr;
                memBlocks[blockIndex].logSize = (uint16_t)currentLogSize;
                memBlocks[blockIndex].pid = 0;  // 0 = livre
                memBlocks[blockIndex].next = NULL;  // Temporário
                
                currentAddr += blockSize;
                blockIndex++;
            }
        }

        // 5. Ordenar blocos por logSize (crescente) e depois por endereço
        for (uint32_t i = 0; i < memBlockCount - 1; i++) {
            for (uint32_t j = 0; j < memBlockCount - i - 1; j++) {
                bool shouldSwap = false;
                
                
                if (memBlocks[j].logSize > memBlocks[j + 1].logSize) {
                    shouldSwap = true;
                }
                
                else if (memBlocks[j].logSize == memBlocks[j + 1].logSize && 
                         memBlocks[j].addr > memBlocks[j + 1].addr) {
                    shouldSwap = true;
                }
                
                if (shouldSwap) {
                    MemNode temp = memBlocks[j];
                    memBlocks[j] = memBlocks[j + 1];
                    memBlocks[j + 1] = temp;
                }
            }
        }

        // 6. Criar lista ligada de blocos livres
        memFreeHead = &memBlocks[0];
        
        for (uint32_t i = 0; i < memBlockCount - 1; i++) {
            memBlocks[i].next = &memBlocks[i + 1];
        }
        memBlocks[memBlockCount - 1].next = NULL;  

        // 7. Lista de blocos ocupados vazia
        memOccupiedHead = NULL;
    }
} // end of namespace group

