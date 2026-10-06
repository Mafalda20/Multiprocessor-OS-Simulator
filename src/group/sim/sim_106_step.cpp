/*
 *  \author Rodrigo Lopes, nmec: 113811
 */

#include "somm25nm.h"
#include <errno.h>

namespace group
{
    bool simStep(bool blocking)
    {
        double time;
        FeqEventType type;
        uint32_t xid;
        
        // Tentar recuperar próximo evento da FEQ
        if (!feqRetrieve(&time, &type, &xid, blocking))
        {
            return false; // FEQ vazia
        }
        
        // Avançar tempo de simulação
        simTime = time;
        
        // Delegar para função auxiliar correspondente ao tipo de evento
        switch (type)
        {
            case SUBMIT:
                simStepSubmit(xid); // xid = jid
                break;
            case ADMIT:
                simStepAdmit((uint16_t)xid); // xid = pid
                break;
            case DISPATCH:
                simStepDispatch();
                break;
            case WAIT_EVENT:
                simStepWaitEvent((uint16_t)xid); // xid = cid (core/processor)
                break;
            case EXIT:
                simStepExit((uint16_t)xid); // xid = cid
                break;
            case EVENT_OCCURS:
                simStepEventOccurs((uint16_t)xid); // xid = pid
                break;
            case ACTIVATE:
                simStepActivate();
                break;
            case DELETE:
                simStepDelete((uint16_t)xid); // xid = pid
                break;
            case PREEMPT:
                simStepPreempt((uint16_t)xid); // xid = cid
                break;
            case SUSPEND:
                simStepSuspend((uint16_t)xid); // xid = pid
                break;
            default:
                throw Exception(EINVAL, __func__);
        }
        
        return true;
    }
} // end of namespace group

