/*
 *  \author Mafalda
 */

 #include "pct.h"
 #include "exception.h"
 #include "dbc.h"
 
 #include <stdio.h>
 #include <stdint.h>
 #include <new>
 #include <errno.h>
 
 namespace group 
 {
     void pctOpen(uint16_t base, uint16_t cnt)
     {
         require(pctTable == PCT_UNDEF_TABLE, "PCT module must be closed before pctOpen");
         require(cnt > 0, "pctOpen: process count must be greater than zero");
 
         PctNode **table = new (std::nothrow) PctNode*[cnt];
         if (table == nullptr)
         {
             errno = ENOMEM;
             throw Exception(errno, __func__);
         }
 
         for (uint16_t i = 0; i < cnt; i++)
             table[i] = nullptr;
 
         pctTable   = table;
         pctPidBase = base;
         pctPidCount = cnt;
 
         pctLastPid = (base == 0) ? 0 : uint16_t(base - 1);
 
         ensure(pctTable != PCT_UNDEF_TABLE, "pctTable must not be PCT_UNDEF_TABLE after pctOpen");
         ensure(pctPidCount == cnt, "pctPidCount not set correctly in pctOpen");
     }
 } 
 