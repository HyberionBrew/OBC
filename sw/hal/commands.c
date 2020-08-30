/*
 * commands.c
 *
 * Created: 23.06.2017 14:00:59
 *  Author: asinn
 */ 
#include "commands.h"
#include <string.h>
#include <avr/wdt.h>

typedef struct obc_cmd_s
{
	uint8_t id; /* Command ID */
	uint8_t par1; /* Parameter 1  (optional) */
	uint8_t par2; /* Parameter 2  (optional) */
	uint8_t par3; /* Parameter 2  (optional) */

} obc_cmd_t;

#define CMD_BUFFER_SIZE 8 /* Max 255 */

typedef struct obc_cmd_buffer_s
{
	obc_cmd_t cmd[CMD_BUFFER_SIZE];
	uint8_t pending; /* Number of pending commands */
	uint8_t current; /* Current command */
}
volatile obc_cmd_buffer_t;

obc_cmd_buffer_t obc_cmd_buffer;

RetVal cmd_store(uint8_t id, uint8_t par1, uint8_t par2, uint8_t par3)
{
	if (obc_cmd_buffer.pending >= CMD_BUFFER_SIZE)
	{
		/* CMD buffer is full */
		return FAILED;
	}

	uint8_t pos = (obc_cmd_buffer.current + obc_cmd_buffer.pending) % CMD_BUFFER_SIZE;

	obc_cmd_buffer.cmd[pos].id = id;
	obc_cmd_buffer.cmd[pos].par1 = par1;
	obc_cmd_buffer.cmd[pos].par2 = par2;
	obc_cmd_buffer.cmd[pos].par3 = par3;

	obc_cmd_buffer.pending++;

	return DONE;
}

void cmd_execute_next(void)
{
	obc_cmd_t *cc; /* Current command */

	if (obc_cmd_buffer.pending == 0)
	{
		/* No pending commands */
		return;
	}

	/* Current command */
	cc = (obc_cmd_t *) &(obc_cmd_buffer.cmd[obc_cmd_buffer.current]);

	/* Execute command */
	switch (cc->id)
	{
		case OBCCMD_RESET_OBC:			
			if((cc->par1 == 0xAB) && (cc->par2 == 0xCD) && (cc->par3 == 0xEF))
			{
				radio_printf("Reseting...\n");
				// Reset OBC
				while(1)
				{
			 		wdt_enable(WDTO_15MS);
			 		while(1);
				}
			}
			break;
			
			case OBCCMD_VDD_SET:
			if(cc->par1 == 0)
			{
				vdd_disable();
			}
			else
			{
				vdd_enable();
			}
			
			break;
		
		
		default:
		break;
	}

	/* Command done */
	obc_cmd_buffer.current++;
	if (obc_cmd_buffer.current >= CMD_BUFFER_SIZE)
	{
		obc_cmd_buffer.current = 0;
	}
	obc_cmd_buffer.pending--;
	
	return;
}

void received2cmd(char * start)
{
	/* Interprets a received data string from the debug interface
	 *  -> If a valid string in the format $C,cmd,par1,par2*\r\n is found,
	 *  it is added to the cmd execution pipeline
	 *  returns number of commands found */

	uint8_t i, cmd, par1, par2, par3;
	char * help = NULL;
	char *cmd_end = NULL;

	if (start == NULL)
	{
		return;
	}

	if (strchr(start, '\n') == NULL && strchr(start, '\r') == NULL)
	{
		return;
	}

	for (i = 0; i < CMD_BUFFER_SIZE; i++)
	{
		help = strchr(start, '$'); /* Search for start of command */

		if (help == NULL)
		{
			/* No cmd start character found */
			return;
		}
		start = help;

		cmd_end = strchr(&start[2], '*');
		if ((start[1] == 'c' || start[1] == 'C') && start[2] == ',' && (cmd_end != NULL))
		{
			/* Header for OBC-command is correct */
			cmd = atoi(&start[3]); /* Get cmd number */

			help = strchr(&start[3], ',');
			if (help == NULL)
			{

				continue;
			}
			start = help;
			par1 = atoi(&start[1]); /* Get par1 */
		
			help = strchr(&start[1], ','); /* Get par2 */
			if (help == NULL)
			{
				continue;
			}
			start = help;
			par2 = atoi(&start[1]); /* Get par2 */
			
			help = strchr(&start[1], ','); /* Get par3 */
			if (help == NULL)
			{
				continue;
			}
			start = help;
			par3 = atoi(&start[1]); /* Get par3 */

			help = strchr(&start[1], ',');
			if ((cmd_end > help) && (help != NULL))
			{
				/* Too many ',' -> return error */
				continue;
			}

			/* Cmd received correctly -> add to cmd buffer */
			cmd_store(cmd, par1, par2, par3);
			radio_printf("CMD: %d, %d, %d, %d\n", cmd, par1, par2, par3);

			break;
		}		
	}

	return;
}


void vdd_disable(void)
{
	vdd_pin_disable();	
}

void vdd_enable(void)
{
	vdd_pin_enable();
}

void vee_disable(void)
{
	vee_pin_disable();
}

void vee_enable(void)
{
	vee_pin_enable();
}

void vdd_init(void)
{	
	DDR(VDD_DISABLE_PORT) |= (1<<VDD_DISABLE_PIN);
	vdd_pin_disable();	
}

void vee_init(void)
{
	DDR(VEE_DISABLE_PORT) |= (1<<VEE_DISABLE_PIN);
	vee_pin_disable();
}

void mt_init(void)
{
	soft_pwm_init();
	// Todo: Add rest of initialization
}