/*
José Francisco Teixeira Mota - 113985
 */

#include "rdy.h"
#include "exception.h"
#include "dbc.h"

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <new>

namespace group
{
    void rdyInsert(uint16_t pid, double curTime, double runTime)
    {
        require(rdyHead != RDY_UNDEF_NODE, "RDY module must be open");
        require(pid != 0, "PID must be non-zero");
        require(runTime > 0.0, "runTime must be positive");

        RdyNode *node = nullptr;
        try {
            node = new RdyNode;
        } catch (const std::bad_alloc &) {
            throw Exception(ENOMEM, __func__);
        }

        node->pid       = pid;
        node->queueTime = curTime;
        node->runTime   = runTime;
        node->next      = nullptr;

        if (rdyHead == nullptr || runTime < rdyHead->runTime) {
            node->next = rdyHead;
            rdyHead = node;
        } else {
            RdyNode *cur = rdyHead;
            while (cur->next != nullptr && cur->next->runTime <= runTime)
                cur = cur->next;
            node->next = cur->next;
            cur->next  = node;
        }
    }

} // end of namespace group


