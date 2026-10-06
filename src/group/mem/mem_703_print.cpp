/*
 *  \author António Caetano
 */

#include "mem.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    void memPrint(FILE *fout, MemPrintMode mode, bool csv)
    {
        /* TODO POINT: Replace next instruction with your code */
         // Validar stream de saída
        if (fout == NULL) {
            throw Exception(EINVAL, __func__);
        }
        
        // Colectar e ordenar blocos livres
        
        
        // Contar quantos blocos livres existem
        uint32_t freeCount = 0;
        MemNode *current = memFreeHead;
        while (current != NULL) {
            freeCount++;
            current = current->next;
        }
        
        // Criar array temporário para ordenar blocos livres
        MemNode **sortedFree = NULL;
        if (freeCount > 0) {
            sortedFree = new MemNode*[freeCount];
            
            // Copiar ponteiros para o array
            current = memFreeHead;
            for (uint32_t i = 0; i < freeCount; i++) {
                sortedFree[i] = current;
                current = current->next;
            }
            
            // Ordenar por endereço (bubble sort) 
            for (uint32_t i = 0; i < freeCount - 1; i++) {
                for (uint32_t j = 0; j < freeCount - i - 1; j++) {
                    if (sortedFree[j]->addr > sortedFree[j + 1]->addr) {
                        MemNode *temp = sortedFree[j];
                        sortedFree[j] = sortedFree[j + 1];
                        sortedFree[j + 1] = temp;
                    }
                }
            }
        }
        
        
        //  Colectar e ordenar blocos OCUPADOS
        
        
        // Contar quantos blocos ocupados existem
        uint32_t occupiedCount = 0;
        current = memOccupiedHead;
        while (current != NULL) {
            occupiedCount++;
            current = current->next;
        }
        
        // Criar array temporário para ordenar blocos ocupados
        MemNode **sortedOccupied = NULL;
        if (occupiedCount > 0) {
            sortedOccupied = new MemNode*[occupiedCount];
            
            // Copiar ponteiros para o array
            current = memOccupiedHead;
            for (uint32_t i = 0; i < occupiedCount; i++) {
                sortedOccupied[i] = current;
                current = current->next;
            }
            
            // Ordenar por endereço (bubble sort)
            for (uint32_t i = 0; i < occupiedCount - 1; i++) {
                for (uint32_t j = 0; j < occupiedCount - i - 1; j++) {
                    if (sortedOccupied[j]->addr > sortedOccupied[j + 1]->addr) {
                        MemNode *temp = sortedOccupied[j];
                        sortedOccupied[j] = sortedOccupied[j + 1];
                        sortedOccupied[j + 1] = temp;
                    }
                }
            }
        }
        
        
        // Imprimir conforme o modo solicitado
        
        
        try {
            switch (mode) {
                
                //  Imprimir TUDO 
                case MemPrintGlobal:
                    if (!csv) {
                        // Formato TABELA
                        fprintf(fout, "\nMemory state:\n");
                        fprintf(fout, "=============\n\n");
                        
                        // Blocos livres
                        fprintf(fout, "Free blocks:\n");
                        if (freeCount == 0) {
                            fprintf(fout, "FREE list: empty\n");
                        } else {
                            fprintf(fout, "+========+============+============+============+\n");
                            fprintf(fout, "|  FREE  |    addr    |    size    |    pid     |\n");
                            fprintf(fout, "+========+============+============+============+\n");
                            for (uint32_t i = 0; i < freeCount; i++) {
                                uint32_t size = 1U << sortedFree[i]->logSize;  // 2^logSize
                                fprintf(fout, "| %6u | 0x%08x | %10u | %10u |\n", 
                                        i + 1, sortedFree[i]->addr, size, sortedFree[i]->pid);
                            }
                            fprintf(fout, "+========+============+============+============+\n");
                        }
                        
                        fprintf(fout, "\n");
                        
                        // Blocos ocupados
                        fprintf(fout, "Occupied blocks:\n");
                        if (occupiedCount == 0) {
                            fprintf(fout, "OCC list: empty\n");
                        } else {
                            fprintf(fout, "+========+============+============+============+\n");
                            fprintf(fout, "|  OCC   |    addr    |    size    |    pid     |\n");
                            fprintf(fout, "+========+============+============+============+\n");
                            for (uint32_t i = 0; i < occupiedCount; i++) {
                                uint32_t size = 1U << sortedOccupied[i]->logSize;
                                fprintf(fout, "| %6u | 0x%08x | %10u | %10u |\n", 
                                        i + 1, sortedOccupied[i]->addr, size, sortedOccupied[i]->pid);
                            }
                            fprintf(fout, "+========+============+============+============+\n");
                        }
                        fprintf(fout, "\n");
                        
                    } else {
                        // Formato CSV
                        fprintf(fout, "Free blocks:\n");
                        if (freeCount > 0) {
                            fprintf(fout, "addr,size,pid\n");
                            for (uint32_t i = 0; i < freeCount; i++) {
                                uint32_t size = 1U << sortedFree[i]->logSize;
                                fprintf(fout, "0x%08x,%u,%u\n", sortedFree[i]->addr, size, sortedFree[i]->pid);
                            }
                        }
                        fprintf(fout, "\nOccupied blocks:\n");
                        if (occupiedCount > 0) {
                            fprintf(fout, "addr,size,pid\n");
                            for (uint32_t i = 0; i < occupiedCount; i++) {
                                uint32_t size = 1U << sortedOccupied[i]->logSize;
                                fprintf(fout, "0x%08x,%u,%u\n", sortedOccupied[i]->addr, size, sortedOccupied[i]->pid);
                            }
                        }
                    }
                    break;
                
                //  Imprimir SÓ blocos livres
                case MemPrintFree:
                    if (!csv) {
                        // Formato TABELA
                        fprintf(fout, "\nFree blocks:\n");
                        if (freeCount == 0) {
                            fprintf(fout, "FREE list: empty\n");
                        } else {
                            fprintf(fout, "+========+============+============+============+\n");
                            fprintf(fout, "|  FREE  |    addr    |    size    |    pid     |\n");
                            fprintf(fout, "+========+============+============+============+\n");
                            for (uint32_t i = 0; i < freeCount; i++) {
                                uint32_t size = 1U << sortedFree[i]->logSize;
                                fprintf(fout, "| %6u | 0x%08x | %10u | %10u |\n", 
                                        i + 1, sortedFree[i]->addr, size, sortedFree[i]->pid);
                            }
                            fprintf(fout, "+========+============+============+============+\n");
                        }
                        fprintf(fout, "\n");
                    } else {
                        // Formato CSV
                        fprintf(fout, "Free blocks:\n");
                        if (freeCount > 0) {
                            fprintf(fout, "addr,size,pid\n");
                            for (uint32_t i = 0; i < freeCount; i++) {
                                uint32_t size = 1U << sortedFree[i]->logSize;
                                fprintf(fout, "0x%08x,%u,%u\n", sortedFree[i]->addr, size, sortedFree[i]->pid);
                            }
                        }
                    }
                    break;
                
                // Imprimir SÓ blocos ocupados
                case MemPrintOccupied:
                    if (!csv) {
                        // Formato TABELA
                        fprintf(fout, "\nOccupied blocks:\n");
                        if (occupiedCount == 0) {
                            fprintf(fout, "OCC list: empty\n");
                        } else {
                            fprintf(fout, "+========+============+============+============+\n");
                            fprintf(fout, "|  OCC   |    addr    |    size    |    pid     |\n");
                            fprintf(fout, "+========+============+============+============+\n");
                            for (uint32_t i = 0; i < occupiedCount; i++) {
                                uint32_t size = 1U << sortedOccupied[i]->logSize;
                                fprintf(fout, "| %6u | 0x%08x | %10u | %10u |\n", 
                                        i + 1, sortedOccupied[i]->addr, size, sortedOccupied[i]->pid);
                            }
                            fprintf(fout, "+========+============+============+============+\n");
                        }
                        fprintf(fout, "\n");
                    } else {
                        // Formato CSV
                        fprintf(fout, "Occupied blocks:\n");
                        if (occupiedCount > 0) {
                            fprintf(fout, "addr,size,pid\n");
                            for (uint32_t i = 0; i < occupiedCount; i++) {
                                uint32_t size = 1U << sortedOccupied[i]->logSize;
                                fprintf(fout, "0x%08x,%u,%u\n", sortedOccupied[i]->addr, size, sortedOccupied[i]->pid);
                            }
                        }
                    }
                    break;
                
                default:
                    // Modo inválido
                    if (sortedFree != NULL) delete[] sortedFree;
                    if (sortedOccupied != NULL) delete[] sortedOccupied;
                    throw Exception(EINVAL, __func__);
            }
            
            // Forçar escrita
            fflush(fout);
            
            // Limpar memória
            if (sortedFree != NULL) delete[] sortedFree;
            if (sortedOccupied != NULL) delete[] sortedOccupied;
        }
        catch (...) {
            // Limpar memória em caso de erro
            if (sortedFree != NULL) delete[] sortedFree;
            if (sortedOccupied != NULL) delete[] sortedOccupied;
            throw;
        }
    }
} // end of namespace group


