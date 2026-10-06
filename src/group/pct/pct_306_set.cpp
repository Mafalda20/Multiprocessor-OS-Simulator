/*
 *  \author Mafalda
 */

 #include "pct.h"
 #include "exception.h"
 #include "dbc.h"
 
 #include <stdio.h>
 #include <stdint.h>
 #include <errno.h>
 
 namespace group
 {
     void pctSet(uint16_t pid, PctField field, void *value)
     {
         require(pctTable != PCT_UNDEF_TABLE, "PCT module must be open");
         require(pctPidCount > 0 && pctPidBase > 0, "PCT must be properly initialized");
         require(value != nullptr, "pctSet: value pointer must not be null");
         require(field >= PctJid && field <= PctState, "pctSet: invalid field");
 
         if (pid < pctPidBase || pid >= pctPidBase + pctPidCount)
         {
             errno = EINVAL;
             throw Exception(errno, __func__);
         }
 
         uint16_t index = static_cast<uint16_t>(pid - pctPidBase);
         PctNode *node = pctTable[index];
 
         if (node == nullptr)
         {
             errno = ESRCH; 
             throw Exception(errno, __func__);
         }
 
         switch (field)
         {
             case PctJid:
                 node->jid = *reinterpret_cast<uint32_t*>(value);
                 break;
 
             case PctMemAddr:
                 node->memAddr = *reinterpret_cast<uint32_t*>(value);
                 break;
 
             case PctState:
                 node->state = *reinterpret_cast<PctProcessState*>(value);
                 break;
 
             default:
                 errno = EINVAL;
                 throw Exception(errno, __func__);
         }
     }
 } 
 