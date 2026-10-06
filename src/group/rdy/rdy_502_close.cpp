/*
 *  \author Mafalda
 */

 #include "rdy.h"
 #include "exception.h"
 #include "dbc.h"
 
 #include <stdio.h>
 #include <stdint.h>
 #include <errno.h>
 
 namespace group
 {
     void rdyClose()
     {
         require(rdyHead != RDY_UNDEF_NODE, "RDY module must be open");
 
         RdyNode *curr = rdyHead;
         while (curr != nullptr)
         {
             RdyNode *next = curr->next;
             delete curr;          
             curr = next;
         }
 
         rdyHead   = RDY_UNDEF_NODE;
         rdyPolicy = RDY_UNDEF_POLICY;
 
         ensure(rdyHead == RDY_UNDEF_NODE, "RDY head must be RDY_UNDEF_NODE after rdyClose");
         ensure(rdyPolicy == RDY_UNDEF_POLICY, "RDY policy must be RDY_UNDEF_POLICY after rdyClose");
     }
 } 
 