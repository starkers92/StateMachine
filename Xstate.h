/*
 * state.h
 *
 *  Created on: 27/02/2019
 *      Author: chris
 */

#ifndef STATE_MACHINE_XSTATE_H_
#define STATE_MACHINE_XSTATE_H_

#include "stdint.h"

typedef enum {
	state_LedOn,
	state_LedOff,
} e_XState;

typedef enum {
	event_Any,
	X_TurnOn,
	X_TurnOff,
	event_no_event,
} e_XEvent;

#define EVENT_FIFO_LENGTH 32
typedef struct {
	e_XEvent event_fifo[EVENT_FIFO_LENGTH];
	unsigned int newest_index;
	unsigned int oldest_index;
} XEventStack_t;

typedef enum {
	eventStack_empty,
	eventStack_full,
	eventStack_ok,
} XeventStackStatus_t;

typedef struct {
    e_XState currState;
    XEventStack_t event;
    /* Define any variables required here for state machine process */
} XStateMachine_t;

void XStateMachine_Init(XStateMachine_t* stateMachine);
XeventStackStatus_t XStateMachine_IsEventWaiting(XStateMachine_t* s);
void XStateMachine_RunIterationSkipEventQueue(XStateMachine_t *stateMachine, e_XEvent event);
void XStateMachine_RunIteration(XStateMachine_t *stateMachine);
XeventStackStatus_t XStateMachine_WriteEvent(XStateMachine_t* s, e_XEvent event);
XeventStackStatus_t XStateMachine_ReadEvent(XStateMachine_t* s, e_XEvent* event);
void XStateMachine_EmptyEventQueue(XStateMachine_t* s);
#endif /* STATE_MACHINE_XSTATE_H_ */
