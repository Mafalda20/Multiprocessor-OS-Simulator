/*
José Francisco Teixeira Mota - 113985
 */

#include "somm25nm.h"

namespace group
{
    void simOpen(SimParameters *param)
    {
        // Obter n de processadores
        uint32_t n = 1;           
        if (param != nullptr)
            n = param->processorCount;

        require(n > 0, "processorCount must be positive");

        // Alocar array dos processadores
        SimProcessorState *vec = nullptr;
        try {
            vec = new SimProcessorState[n];
        }
        catch (const std::bad_alloc &) {
            throw Exception(ENOMEM, __func__);
        }

        simProcessorCount  = n;
        simProcessorState  = vec;

        for (uint32_t i = 0; i < n; i++) {
            simProcessorState[i].idle = true;
            simProcessorState[i].next = (i + 1 < n) ? i + 1 : n; 
        }
        simIdleHead = 0;
        simIdleTail = (n > 0) ? (n - 1) : SIM_UNDEF_INDEX; 

        simTime = 0.0;

        if (param != nullptr)
        {
            // JOB e FEQ
            jobOpen();
            feqOpen();

            // PCT
            pctOpen(param->basePid, param->maxPids);

            // RDY
            rdyOpen(param->schedulingPolicy);

            // SWP
            swpOpen(param->swappingPolicy);

            // MEM
            memOpen(param->memInitAddr,
                    param->memMinLogSize,
                    param->memSizes,
                    param->memSizesCount);
        }

        ensure(simTime == 0.0, "Simulation time must start at 0");
        ensure(simProcessorCount == n, "Processor count not set correctly");
        ensure(simIdleHead == 0, "Idle head must start at 0");
        ensure(simIdleTail == n - 1, "Idle tail must be last processor");
    }
} // end of namespace group

