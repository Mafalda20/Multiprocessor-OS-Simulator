/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "swp.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>
#include <errno.h>

namespace group
{
    void swpUnblock(uint16_t pid)
    {
        // Percorrer a lista procurando o PID
        SwpNode *prev = NULL;
        SwpNode *curr = swpHead;

        while (curr != NULL && curr->pid != pid)
        {
            prev = curr;
            curr = curr->next;
        }

        if (curr == NULL)
        {
            // PID não encontrado
            throw Exception(ESRCH, __func__);
        }

        if (!curr->blocked)
        {
            // Já desbloqueado — nada a fazer
            return;
        }

        // Marcar desbloqueado
        curr->blocked = false;

        // Se já é o tail, não há necessidade de mover
        if (curr == swpTail)
            return;

        // Remover curr da sua posição
        if (prev == NULL)
        {
            // era head
            swpHead = curr->next;
        }
        else
        {
            prev->next = curr->next;
        }

        // Anexar curr ao fim
        curr->next = NULL;
        if (swpTail != NULL && swpTail != SWP_UNDEF_NODE)
            swpTail->next = curr;
        swpTail = curr;

        // Se a lista ficou vazia antes (caso raro), garantir head
        if (swpHead == NULL)
            swpHead = swpTail;
    }
} // end of namespace group


