/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "somm25nm.h"

namespace group
{
    void simRun(uint32_t cnt, bool blocking)
    {
        if (cnt == 0)
        {
            // Executar até o fim (FEQ vazia)
            while (simStep(blocking))
            {
                // Continua enquanto houver eventos
            }
        }
        else
        {
            // Executar cnt passos
            for (uint32_t i = 0; i < cnt; i++)
            {
                if (!simStep(blocking))
                    break; // FEQ vazia, terminar antecipadamente
            }
        }
    }
} // end of namespace group

