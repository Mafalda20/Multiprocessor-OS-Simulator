/*
José Francisco Teixeira Mota - 113985
 */

#include "rdy.h"
#include "exception.h"

#include <stdio.h>
#include <stdint.h>
#include <errno.h>

namespace group
{
    void rdyPrint(FILE *fout, bool csv)
    {
        if (csv) {
            if (fprintf(fout, "pid;queueTime;runTime\n") < 0)
                throw Exception(errno, __func__);
        }

        for (RdyNode *n = rdyHead; n != nullptr; n = n->next) {
            int r = fprintf(
                fout,
                csv ? "%u;%.1f;%.1f\n"
                    : "pid=%u queueTime=%.1f runTime=%.1f\n",
                n->pid, n->queueTime, n->runTime
            );
            if (r < 0)
                throw Exception(errno, __func__);
        }
    }
} // end of namespace group


