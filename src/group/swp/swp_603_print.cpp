/*
 *  \author Antonio caetano
 */

#include "swp.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group
{
    void swpPrint(FILE *fout, bool csv)
    {
        /* TODO POINT: Replace next instruction with your code */
    
        // Validar que o módulo está aberto
        if (swpHead == SWP_UNDEF_NODE || swpTail == SWP_UNDEF_NODE)
        {
            throw Exception(ENOSYS, __func__);
        }

        // Validar o file stream
        if (fout == NULL || fileno(fout) == -1)
        {
            throw Exception(EBADF, __func__);
        }

        // Imprimir cabeçalho CSV se necessário
        if (csv)
        {
            fprintf(fout, "pid;size;blocked\n");
        }

        // Percorrer a lista em ordem natural (do início ao fim)
        SwpNode *current = swpHead;
        
        while (current != NULL)
        {
            if (csv)
            {
                // Formato CSV: pid;size;blocked
                fprintf(fout, "%hu;%u;%s\n", 
                        current->pid, 
                        current->size, 
                        current->blocked ? "true" : "false");
            }
            else
            {
                // Formato normal: pid=X size=Y blocked=Z
                fprintf(fout, "pid=%hu size=%u blocked=%s\n", 
                        current->pid, 
                        current->size, 
                        current->blocked ? "true" : "false");
            }
            
            current = current->next;
        }
    }
} // end of namespace group


