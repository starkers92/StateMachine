/*
 * state.c
 *
 *  Created on: 27/02/2019
 *      Author: chris
 */

#include "Xstate.h"
#include <stdio.h>
#include <stdlib.h>

/* State functions */
static void empty_function(XStateMachine_t* s) {

}

static void Enter_Led_On(XStateMachine_t* s) {
	puts("Enter LED ON");
}

static void Led_On(XStateMachine_t* s) {
	puts("LED ON");
}

static void Exit_Led_On(XStateMachine_t* s) {
	puts("Exit LED ON");
}

static void Enter_Led_Off(XStateMachine_t* s) {
	puts("Enter LED OFF");
}

static void Led_Off(XStateMachine_t* s) {
	puts("LED OFF");
}

static void Exit_Led_Off(XStateMachine_t* s) {
	puts("Exit LED OFF");
}

/* Global definitions for state machine */

typedef struct {
	e_XState state;
	void (*func)(XStateMachine_t*);
	void (*Entryfunc)(XStateMachine_t*);
	void (*Exitfunc)(XStateMachine_t*);
} XStateFunctionRow_t;

static XStateFunctionRow_t stateFunctionA[] = {
		// NAME         	// FUNC		//Entry Func		//Exit func
		{ state_LedOn,    &Led_On,		&Enter_Led_On,		&Exit_Led_On},
		{ state_LedOff,   &Led_Off,		&Enter_Led_Off,		&Exit_Led_Off},
};

typedef struct {
	e_XState currState;
	e_XEvent event;
	e_XState nextState;
} XStateTransMatrixRow_t;

static XStateTransMatrixRow_t stateTransMatrix[] = {
		// CURR STATE  		// EVENT        // NEXT STATE
		{ state_LedOff,     X_TurnOn,     	state_LedOn},
		{ state_LedOn,     X_TurnOff,     	state_LedOff},
};


/* State machine servicing functions */
void XStateMachine_Init(XStateMachine_t* stateMachine) {
	/* Empty Event Queue */
	XStateMachine_EmptyEventQueue(stateMachine);

	/* Update initial state FIXME: set to desired initial state */
	stateMachine->currState = state_LedOff;
}

void XStateMachine_RunIterationSkipEventQueue(XStateMachine_t *stateMachine, e_XEvent event) {
	/* Iterate through the state transition matrix, checking if there is both a match with the current state and the event */
	for(uint16_t i = 0; i < sizeof(stateTransMatrix)/sizeof(stateTransMatrix[0]); i++) {
		if(stateTransMatrix[i].currState == stateMachine->currState) {
			if((stateTransMatrix[i].event == event) || (stateTransMatrix[i].event == event_Any)) {

				/* Call exit function */
				(stateFunctionA[stateMachine->currState].Exitfunc)(stateMachine);

				/* Transition to the next state */
				stateMachine->currState =  stateTransMatrix[i].nextState;

				/* Call entry function */
				(stateFunctionA[stateMachine->currState].Entryfunc)(stateMachine);

				/* Call the function associated with transition */
				(stateFunctionA[stateMachine->currState].func)(stateMachine);
				break;
			}
		}
	}
}

void XStateMachine_RunIteration(XStateMachine_t* stateMachine) {
	e_XEvent eventFromFifo;

	/* Get the status of the event fifo. If no events have occured, run state machine (no transition) */
	if(XStateMachine_IsEventWaiting(stateMachine) == eventStack_empty) {
		/* Call the function associated with that state i.e. no transition */
		(stateFunctionA[stateMachine->currState].func)(stateMachine);
		return;
	}

	/* While FIFO is not empty, get pending event from fifo, process transition & execute function */
	while(XStateMachine_ReadEvent(stateMachine, &eventFromFifo)==eventStack_ok) {
		for(int i = 0; i < sizeof(stateTransMatrix)/sizeof(stateTransMatrix[0]); i++) {
			if(stateTransMatrix[i].currState == stateMachine->currState) {
				if((stateTransMatrix[i].event == eventFromFifo) || (stateTransMatrix[i].event == event_Any)) {

					/* Call exit function */
					(stateFunctionA[stateMachine->currState].Exitfunc)(stateMachine);

					/* Transition to the next state */
					stateMachine->currState =  stateTransMatrix[i].nextState;

					/* Call entry function */
					(stateFunctionA[stateMachine->currState].Entryfunc)(stateMachine);

					/* Call the function associated with transition */
					(stateFunctionA[stateMachine->currState].func)(stateMachine);
					break;
				}
			}
		}
	}
}

XeventStackStatus_t XStateMachine_IsEventWaiting(XStateMachine_t* s) {
	unsigned int next_index = (s->event.newest_index+1) % EVENT_FIFO_LENGTH;
	if(s->event.newest_index == s->event.oldest_index) {
			return eventStack_empty;
		}
	else if (next_index== s->event.oldest_index) {
		return eventStack_full;
	}
	else {
		return eventStack_ok;
	}
}

XeventStackStatus_t XStateMachine_WriteEvent(XStateMachine_t* s, e_XEvent event) {
	unsigned int next_index = (s->event.newest_index+1) % EVENT_FIFO_LENGTH;
	if (next_index== s->event.oldest_index) {
		return eventStack_full;
	}
	s->event.event_fifo[s->event.newest_index] = event;
	s->event.newest_index = next_index;
	return eventStack_ok;
}

XeventStackStatus_t XStateMachine_ReadEvent(XStateMachine_t* s, e_XEvent* event) {
	if(s->event.newest_index == s->event.oldest_index) {
		return eventStack_empty;
	}
	*event = s->event.event_fifo[s->event.oldest_index];
	s->event.oldest_index = (s->event.oldest_index + 1) % EVENT_FIFO_LENGTH;
	return eventStack_ok;
}

void XStateMachine_EmptyEventQueue(XStateMachine_t* s) {
	s->event.newest_index=0;
	s->event.oldest_index=0;
}
