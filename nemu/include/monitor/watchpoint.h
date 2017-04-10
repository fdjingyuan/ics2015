#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char str[32];//the expression
	char type[16];//watchpoint or breakpoint
	uint32_t result;//the monitoring value
	/* TODO: Add more members if necessary */

} WP;
	WP* head;
	WP* new_wp();
	void free_wp(WP* wp);
	
#endif
