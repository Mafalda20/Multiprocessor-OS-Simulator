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
     void pctGet(uint16_t pid, PctField field, void *value)
     {
         require(pctTable != PCT_UNDEF_TABLE, "PCT module must be open");
         require(pctPidCount > 0 && pctPidBase > 0, "PCT must be properly initialized");
         require(value != nullptr, "pctGet: value pointer must not be null");
         require(field >= PctJid && field <= PctState, "pctGet: invalid field");
 
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
                 *reinterpret_cast<uint32_t*>(value) = node->jid;
                 break;
 
             case PctMemAddr:
                 *reinterpret_cast<uint32_t*>(value) = node->memAddr;
                 break;
 
             case PctState:
                 *reinterpret_cast<PctProcessState*>(value) = node->state;
                 break;
 
             default:
                 errno = EINVAL;
                 throw Exception(errno, __func__);
         }
     }
 }
 