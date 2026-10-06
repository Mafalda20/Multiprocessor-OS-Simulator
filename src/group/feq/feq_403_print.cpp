/*
José Francisco Teixeira Mota - 113985
 */

#include "feq.h"

#include "exception.h"

#include <stdio.h>
#include <stdint.h>

namespace group 
{

// ================================================================================== //

    void feqPrint(FILE *fout, bool csv)
    {
        static const char *names[] = {
            "SUBMIT","ADMIT","DISPATCH","TIMEOUT","PREEMPT",
            "WAIT_EVENT","EVENT_OCCURS","SUSPEND","ACTIVATE","EXIT","DELETE"
        };

        if (csv)
            fprintf(fout, "time;type;xid\n");

        for (FeqNode *n = feqHead; n != nullptr; n = n->next)
            fprintf(
                fout,
                csv ? "%.1f;%s;%u\n" : "time=%.1f type=%s xid=%u\n",
                n->time, names[n->type], n->xid
            );
    }

// ================================================================================== //

} // end of namespace group

