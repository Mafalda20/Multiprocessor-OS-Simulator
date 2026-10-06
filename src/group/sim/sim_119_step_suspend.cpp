/*
 *  \author João Pinto
 */

#include "somm25nm.h"

namespace group
{
    void simStepSuspend(uint16_t pid)
    {
        // Get process state
        PctProcessState state;
        pctGet(pid, PctState, &state);
        
        // Determine if process is blocked
        bool isBlocked = (state == BLOCKED);
        
        // Get memory address and free it
        uint32_t memAddr;
        pctGet(pid, PctMemAddr, &memAddr);
        
        if (memAddr != PCT_UNDEF_ADDRESS)
        {
            memFree(memAddr);
        }
        
        // Mark memory as undefined
        uint32_t undefinedAddr = PCT_UNDEF_ADDRESS;
        pctSet(pid, PctMemAddr, &undefinedAddr);
        
        // Update process state to suspended
        PctProcessState newState = isBlocked ? S_BLOCKED : S_READY;
        pctSet(pid, PctState, &newState);
        
        // Get memory size and insert into SWP queue
        uint32_t jid;
        pctGet(pid, PctJid, &jid);
        
        uint32_t memSize;
        jobGet(jid, JobMemSize, &memSize);
        
        swpInsert(pid, memSize, isBlocked);
    }
} // end of namespace group