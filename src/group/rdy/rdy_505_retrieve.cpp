/*
José Francisco Teixeira Mota - 113985
 */

#include "rdy.h"
#include "exception.h"
#include "dbc.h"    

#include <stdio.h>
#include <stdint.h>

namespace group
{
    uint16_t rdyRetrieve(double curTime)
    {
        require(rdyHead != RDY_UNDEF_NODE, "RDY module must be open");

        if (rdyHead == nullptr)
            return 0; 

        if (rdyPolicy == SPN || rdyPolicy == SRT) {
            RdyNode *node = rdyHead;
            rdyHead = node->next;
            uint16_t pid = node->pid;
            delete node;
            return pid;
        }


        if (rdyPolicy == HRRN) {
            RdyNode *best = nullptr;
            RdyNode *bestPrev = nullptr;
            RdyNode *prev = nullptr;

            double bestR = -1.0;

            for (RdyNode *n = rdyHead; n != nullptr; prev = n, n = n->next) {
                double waiting = curTime - n->queueTime;
                if (waiting < 0.0) waiting = 0.0;  
                double R = (waiting + n->runTime) / n->runTime;

                if (R > bestR) {
                    bestR = R;
                    best = n;
                    bestPrev = prev;
                }
            }


            if (bestPrev == nullptr)
                rdyHead = best->next;
            else
                bestPrev->next = best->next;

            uint16_t pid = best->pid;
            delete best;
            return pid;
        }


        throw Exception(EINVAL, __func__);
    }
} // end of namespace group


