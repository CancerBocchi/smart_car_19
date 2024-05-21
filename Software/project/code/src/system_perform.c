#include "system_perform.h"




void Sys_Init()
{
	//创建信号量来控制线程

}

////状态机初始化
//void fsm_init(FSM_t* pFsm,EVENT_MAP_t* pEventMap,ACTION_MAP_t *pActionMap)
//{
//	pFsm->stCurState = 0;
//	pFsm->stNextState = EVENT_MAP_END;
//	pFsm->pEventMap = pEventMap;
//	pFsm->pActionMap = pActionMap;
//}

////状态切换
//void fsm_state_transfer(FSM_t* pFsm, EVENT_t stEventID)
//{
//	uint8_t i = 0;
//	
//	for(i=0; pFsm->pEventMap[i].stEventID<EVENT_MAP_END; i++)
//	{
//		if((stEventID == pFsm->pEventMap[i].stEventID) 
//			&& (pFsm->stCurState == pFsm->pEventMap[i].stCurState))
//		{
//			pFsm->stNextState = pFsm->pEventMap[i].stNextState;
//			
//			return;
//		}
//	}	
//}

////状态函数执行
//void action_perfrom(FSM_t* pFsm)
//{
//	if(EVENT_MAP_END != pFsm->stNextState)
//	{
//		pFsm->pActionMap[pFsm->stCurState].ExitAct();
//		pFsm->pActionMap[pFsm->stNextState].EnterAct();
//		
//		pFsm->stCurState = pFsm->stNextState;
//		pFsm->stNextState = EVENT_MAP_END;
//	}
//	else
//	{
//		pFsm->pActionMap[pFsm->stCurState].RunningAct();
//	}
//}