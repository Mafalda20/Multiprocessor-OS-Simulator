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

    void feqInsert(double time, FeqEventType type, uint32_t xid)
    {
        /* TODO POINT: Replace next instruction with your code */

        // Validar tempo não negativo
        if (time < 0) {
            throw Exception(EINVAL, __func__);
        }
        
        // Criar novo nó dinamicamente
        FeqNode *newNode = new FeqNode;
        newNode->time = time;
        newNode->type = type;
        newNode->xid = xid;
        newNode->next = NULL;
        
    
        // 1: Fila vazia - inserir como primeiro

        if (feqHead == NULL) {
            feqHead = newNode;
            return;
        }
        
        // 2: Inserir ANTES do primeiro nó
       
        
        // Verificar se novo evento deve ir antes do primeiro
        bool insertBeforeHead = false;
        
        //  Ordenar por tempo
        if (newNode->time < feqHead->time) {
            insertBeforeHead = true;
        }
        // Mesmo tempo - aplicar regras de prioridade
        else if (newNode->time == feqHead->time) {
            // DISPATCH tem prioridade máxima
            if (newNode->type == DISPATCH && feqHead->type != DISPATCH) {
                insertBeforeHead = true;
            }
            // Eventos de alta prioridade 
            else if (newNode->type != DISPATCH && feqHead->type != DISPATCH) {
                bool newIsHigh = (newNode->type == WAIT_EVENT || newNode->type == EXIT || 
                                  newNode->type == TIMEOUT || newNode->type == PREEMPT);
                bool headIsHigh = (feqHead->type == WAIT_EVENT || feqHead->type == EXIT || 
                                   feqHead->type == TIMEOUT || feqHead->type == PREEMPT);
                
                if (newIsHigh && !headIsHigh) {
                    insertBeforeHead = true;
                }
            }
        }
        
        if (insertBeforeHead) {
            newNode->next = feqHead;
            feqHead = newNode;
            return;
        }
        
        // 3: Procurar posição no MEIO/FIM da lista
        
        
        FeqNode *current = feqHead;
        
        // Percorrer a lista até encontrar posição correta
        while (current->next != NULL) {
            bool insertHere = false;
            
            // Ordenar por tempo
            if (newNode->time < current->next->time) {
                insertHere = true;
            }
            // Mesmo tempo - aplicar regras de prioridade
            else if (newNode->time == current->next->time) {
                // DISPATCH tem prioridade máxima
                if (newNode->type == DISPATCH && current->next->type != DISPATCH) {
                    insertHere = true;
                }
                // Eventos de alta prioridade
                else if (newNode->type != DISPATCH && current->next->type != DISPATCH) {
                    bool newIsHigh = (newNode->type == WAIT_EVENT || newNode->type == EXIT || 
                                      newNode->type == TIMEOUT || newNode->type == PREEMPT);
                    bool nextIsHigh = (current->next->type == WAIT_EVENT || current->next->type == EXIT || 
                                       current->next->type == TIMEOUT || current->next->type == PREEMPT);
                    
                    if (newIsHigh && !nextIsHigh) {
                        insertHere = true;
                    }
                }
                //  4: Mesma prioridade - manter ordem de inserção (não inserir)
            }
            
            if (insertHere) {
                // Inserir entre current e current->next
                newNode->next = current->next;
                current->next = newNode;
                return;
            }
            
            current = current->next;
        }
        
        //4: Inserir no FIM da lista
        current->next = newNode;
    }

// ================================================================================== //

} // end of namespace group

