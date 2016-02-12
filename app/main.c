/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2016	
*
* File:		main.c
* Description:	Main file of the user application
*******************************************************************/

/*******************************************************************
* Includes
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "includes/general.h"
#include "includes/comm.h"
#include "../shared/proto.h"



/*******************************************************************
* Name:		handle_exec_event
* Description:	Receives information about a specific program
*		execution from the kernel, afterward it prints it.
*******************************************************************/
static void handle_exec_event(proto_msg_t * msg) {
	int call_rv;
	char * exec_line;
	if (NULL == msg) {
		return;
	}
	
	/* Allocate memory for the message data */
	exec_line = malloc(msg->msg_size);	
	if (NULL == exec_line) {
		return;
	}

	call_rv = COMM_nl_recv(exec_line, msg->msg_size);
	if (0 >= call_rv) {
		free(exec_line);
		return;

	}

	fprintf(stdout, "%sExecuted:%s %s \n",KGRN, KNRM, exec_line);
	free(exec_line);
}

/*******************************************************************
* Name:		monitor_execs
* Description:	Receives information from the kernel about program
*		executions.
*******************************************************************/
static void monitor_execs() {
	int call_rv;
	proto_msg_t msg;

	/* Endlessly receive information about executions */
	while(true) {
		memset(&msg, 0, sizeof(proto_msg_t));	
		call_rv = COMM_nl_recv(&msg, sizeof(proto_msg_t));
		if (0 >= call_rv) {
			continue;
		}

		/* Validate received message */	
		if (false == PROTO_is_valid_msg(&msg)) {
			continue;
		}
		
		/* Determine message type and act accordingly */
		switch(msg.action) {
			case KEEP_ALIVE:
				/* Alive */
				continue;

			case EXEC_EVENT:
				/* New program execution */
				handle_exec_event(&msg);

			default:
				/* Unknown packet */
				continue;
		}
	}
}

/*******************************************************************
* Name:		main
* Description:	Main function of the user mode application.
*******************************************************************/
int main(int argc, char * argv[])
{
	int ret = SUCCESS;
	int call_rv;

	/* Eliminate unused warnings */
	UNUSED(argc);
	UNUSED(argv);

	fprintf(stdout, "%s[*] ExecMon %s\n", KGRN, KNRM);

	/* Init communication struct */
	call_rv = COMM_nl_init();
	if (SUCCESS != call_rv) {
		ret = ERROR;
		goto cleanup;
	}

	/* Connect to the kernel module */	
	call_rv = COMM_nl_request_conn();
	if (ERROR == call_rv) {
		fprintf(stderr, "Error connecting to the kernel module.\n");
		ret = call_rv;
		goto cleanup;
	} 

	fprintf(stdout, "[*] Monitoring executions \n");
	monitor_execs();

cleanup:
	COMM_nl_destruct();
	return ret;

}

