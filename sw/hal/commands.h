/*
 * commands.h
 *
 * Created: 23.06.2017 14:02:53
 *  Author: asinn
 */ 


#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "config.h"

#define OBCCMD_RESET_OBC 						0xAA  		// par1: 0xAB, par2: 0x:1D, par3: 0xBB
#define OBCCMD_VDD_SET							0x01		// par1: 1... enable, 0.. disable

void received2cmd(char * start);
void cmd_execute_next(void);

#endif /* COMMANDS_H_ */