/*
 ============================================================================
 Name        : linux_gcc_test_proj.c
 Author      : chris
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "Xstate.h"

int main(void) {

	/* Declare state machine */
	XStateMachine_t *LedMachine =  calloc(1,sizeof(XStateMachine_t));

	/* Call init function */
	XStateMachine_Init(LedMachine);

	/* Run iteration */
	XStateMachine_RunIteration(LedMachine);

	/* Generate event */
	if(XStateMachine_WriteEvent(LedMachine, X_TurnOn) != eventStack_ok) {
		/* Error handler */
		free(LedMachine);
		return -1;
	}

	/* Run iteration */
	XStateMachine_RunIteration(LedMachine);

	/* Generate event */
	if(XStateMachine_WriteEvent(LedMachine, X_TurnOff) != eventStack_ok) {
		/* Error handler */
		free(LedMachine);
		return -1;
	}

	/* Run iteration */
	XStateMachine_RunIteration(LedMachine);

	free(LedMachine);
	
	return EXIT_SUCCESS;
}
