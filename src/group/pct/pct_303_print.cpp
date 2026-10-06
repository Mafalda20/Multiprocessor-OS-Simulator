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
     static const char *stateToString(PctProcessState st)
     {
         switch (st)
         {
             case NEW:      return "NEW";
             case RUNNING:  return "RUNNING";
             case BLOCKED:  return "BLOCKED";
             case READY:    return "READY";
             case S_BLOCKED:return "S_BLOCKED";
             case S_READY:  return "S_READY";
             case ENDED:    return "ENDED";
             default:       return "UNKNOWN";
         }
     }
 
     void pctPrint(FILE *fout, bool csv)
     {
         require(pctTable != PCT_UNDEF_TABLE, "PCT module must be open");
         require(pctPidCount > 0, "PCT must have a positive size");
         require(fout != nullptr && fileno(fout) != -1, "Invalid output stream");
 
         if (csv)
         {
             fprintf(fout, "pid;jid;memAddr;state\n");
 
             for (uint16_t i = 0; i < pctPidCount; i++)
             {
                 PctNode *node = pctTable[i];
                 if (node == nullptr) continue;
 
                 uint16_t pid = pctPidBase + i;
                 fprintf(fout, "%hu;%u;%u;%s\n",
                         pid,
                         node->jid,
                         node->memAddr,
                         stateToString(node->state));
             }
         }
         else
         {
             fprintf(fout, "PCT - Process Control Table\n");
             fprintf(fout, "  base PID : %hu\n", pctPidBase);
             fprintf(fout, "  entries  : %hu\n", pctPidCount);
             fprintf(fout, "\n");
             fprintf(fout, "  %-5s  %-10s  %-10s  %-12s\n",
                     "PID", "JID", "MEM_ADDR", "STATE");
             fprintf(fout, "  -----  ----------  ----------  ------------\n");
 
             for (uint16_t i = 0; i < pctPidCount; i++)
             {
                 PctNode *node = pctTable[i];
                 if (node == nullptr) continue;
 
                 uint16_t pid = pctPidBase + i;
                 fprintf(fout, "  %-5hu  0x%08x  0x%08x  %-12s\n",
                         pid,
                         node->jid,
                         node->memAddr,
                         stateToString(node->state));
             }
         }
     }
 } 
 

