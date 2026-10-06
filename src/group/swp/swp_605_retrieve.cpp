/*
 *  \author Mafalda
 */

 #include "swp.h"
 #include "exception.h"
 #include "dbc.h"
 
 #include <stdio.h>
 #include <stdint.h>
 #include <errno.h>
 #include <limits>
 
 namespace group
 {
     uint16_t swpRetrieve(uint32_t sizeAvailable, bool canBeBlocked)
     {
         require(swpHead != SWP_UNDEF_NODE && swpTail != SWP_UNDEF_NODE,
                 "SWP module must be open");
 
         if (swpHead == nullptr)
             return 0;
 
         SwpNode *best      = nullptr;
         SwpNode *prevBest  = nullptr;
 
         SwpNode *prev = nullptr;
         SwpNode *curr = swpHead;
 
         switch (swpPolicy)
         {
             case FirstFit:
             {
                 while (curr != nullptr)
                 {
                     bool fits = (curr->size <= sizeAvailable) &&
                                 (canBeBlocked || !curr->blocked);
 
                     if (fits)
                     {
                         best = curr;
                         prevBest = prev;
                         break;  
                     }
 
                     prev = curr;
                     curr = curr->next;
                 }
                 break;
             }
 
             case FirstBest:
             {
                 uint32_t bestWaste = std::numeric_limits<uint32_t>::max();
 
                 while (curr != nullptr)
                 {
                     bool fits = (curr->size <= sizeAvailable) &&
                                 (canBeBlocked || !curr->blocked);
 
                     if (fits)
                     {
                         uint32_t waste = sizeAvailable - curr->size;
                         if (waste < bestWaste)
                         {
                             bestWaste = waste;
                             best = curr;
                             prevBest = prev;
                         }
                     }
 
                     prev = curr;
                     curr = curr->next;
                 }
                 break;
             }
 
             default:
             {
                 errno = EINVAL;
                 throw Exception(errno, __func__);
             }
         }
 
         if (best == nullptr)
             return 0;
 
         uint16_t pid = best->pid;
 
         SwpNode *next = best->next;
 
         if (prevBest == nullptr)
         {
             swpHead = next;
         }
         else
         {
             prevBest->next = next;
         }
 
         if (best == swpTail)
         {
             swpTail = prevBest;
         }
 
         delete best;
 
         if (swpHead == nullptr)
             swpTail = nullptr;
 
         return pid;
     }
 } 
 