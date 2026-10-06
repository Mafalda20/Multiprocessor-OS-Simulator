/*
 *  \author António Caetano
 */

#include "swp.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    void swpInsert(uint16_t pid, uint32_t size, bool blocked)
    {
        /* TODO POINT: Replace next instruction with your code */
        
        // Validar que o módulo está aberto
        if (swpHead == SWP_UNDEF_NODE || swpTail == SWP_UNDEF_NODE)
        {
            throw Exception(ENOSYS, __func__);
        }

        // Validar parâmetros
        if (pid == 0)
        {
            throw Exception(EINVAL, __func__);
        }
        
        if (size == 0)
        {
            throw Exception(EINVAL, __func__);
        }
        // Criar novo nó dinamicamente
        SwpNode *newNode = new SwpNode;
        newNode->pid = pid;
        newNode->size = size;
        newNode->blocked = blocked;
        newNode->next = NULL;

    
        // Inserção SEMPRE no FIM da fila (tail)
       
        if (swpHead == NULL)
        {
            // Fila vazia - novo nó é head E tail
            swpHead = newNode;
            swpTail = newNode;
        }
        else
        {
            // Fila não vazia - adicionar ao fim
            swpTail->next = newNode;
            swpTail = newNode;
        }
    }
} // end of namespace group


