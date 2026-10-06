/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "somm25nm.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

namespace group
{
    void simLoadBatch(FILE *fin, uint32_t maxMemSize)
    {
        char line[1024];
        double lastSubmitTime = -1.0;
        
        while (fgets(line, sizeof(line), fin) != NULL)
        {
            // Ignorar comentários e linhas vazias
            if (line[0] == '%' || line[0] == '\n' || line[0] == '\r')
                continue;
            
            // Remover espaços no início
            char *ptr = line;
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;
            
            if (*ptr == '\n' || *ptr == '\0')
                continue;
            
            // Parse: JID;submitTime;memSize;burst1,burst2,...
            uint32_t jid;
            double submitTime;
            uint32_t memSize;
            char burstsStr[512];
            
            if (sscanf(ptr, "%x;%lf;%i;%[^\n]", &jid, &submitTime, &memSize, burstsStr) != 4)
            {
                fprintf(stderr, "Erro de sintaxe na linha: %s", line);
                throw Exception(EINVAL, __func__);
            }
            
            // Validar ordem crescente dos tempos de submissão
            if (submitTime < lastSubmitTime)
            {
                fprintf(stderr, "Tempos de submissão fora de ordem\n");
                throw Exception(EINVAL, __func__);
            }
            lastSubmitTime = submitTime;
            
            // Parse dos bursts (separados por vírgula)
            double bursts[JOB_MAX_BURSTS];
            int burstCount = 0;
            char *token = strtok(burstsStr, ",");
            
            while (token != NULL && burstCount < JOB_MAX_BURSTS)
            {
                double burst = atof(token);
                if (burst <= 0.0)
                {
                    fprintf(stderr, "Burst inválido (deve ser > 0)\n");
                    throw Exception(EINVAL, __func__);
                }
                bursts[burstCount++] = burst;
                token = strtok(NULL, ",");
            }
            
            // Validar número ímpar de bursts
            if (burstCount == 0 || burstCount % 2 == 0)
            {
                fprintf(stderr, "Número de bursts deve ser ímpar\n");
                throw Exception(EINVAL, __func__);
            }
            
            // Validar memSize
            if (memSize == 0 || memSize > maxMemSize)
            {
                fprintf(stderr, "Memory size inválido (deve ser > 0 e <= maxMemSize)\n");
                throw Exception(EINVAL, __func__);
            }
            
            // Adicionar job à fila JOB
            jobInsert(jid, submitTime, memSize, bursts);

            
            // Adicionar evento SUBMIT à FEQ
            feqInsert(submitTime, SUBMIT, jid);
        }
    }
} // end of namespace group

