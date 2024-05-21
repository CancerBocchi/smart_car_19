#ifndef __SYSTEM_PERFORM_H__
#define __SYSTEM_PERFORM_H__

#include "zf_common_headfile.h"


//typedef void (*STATE_ACTION)(void);	

//typedef enum{
//	STATE1 = 0,
//	STATE2,
//	STATE3,
//	STATE4,
//}STATE_t; 

//typedef struct ACTION_MAP
//{
//	STATE_t 		stStateID;
//	STATE_ACTION 	EnterAct;	
//	STATE_ACTION 	RunningAct;	
//	STATE_ACTION 	ExitAct;
//}ACTION_MAP_t;

//typedef enum
//{
//	EVENT1 = 0,
//	EVENT2,
//	EVENT3,
//	EVENT4,
//	EVENT5,
//	
//	EVENT_MAP_END
//}EVENT_t;

//typedef struct EVENT_MAP
//{
//	EVENT_t	stEventID;
//	STATE_t stCurState;
//	STATE_t stNextState;
//}EVENT_MAP_t;

//typedef struct FSM
//{
//	STATE_t stCurState;
//	STATE_t stNextState;
//	ACTION_MAP_t *pActionMap;
//	EVENT_MAP_t *pEventMap;
//}FSM_t;

//void fsm_init(FSM_t* pFsm,EVENT_MAP_t* pEventMap,ACTION_MAP_t *pActionMap);
//void action_perfrom(FSM_t* pFsm);
//void fsm_state_transfer(FSM_t* pFsm, EVENT_t stEventID);


	
#endif