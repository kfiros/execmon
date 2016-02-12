/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		shared/proto.h
* Description:	Protocol for kernel user communication. This header
*		should be shared between the kernel part and also
*		the user application.
*******************************************************************/

#ifndef __PROTO_H__
#define __PROTO_H__

#include <linux/types.h>

#define PROTO_PREFIX ("EM") /* Abbreviation for ExecMon */
#define PROTO_PREFIX_SIZE (2)

enum ACTION {
	CONN_REQUEST = 1000, 	/* App -> Kernel */
	CONN_ACCEPT,		/* Kernel -> App */
	CONN_DENY,
	KEEP_ALIVE,		/* Kernel -> App */
	EXEC_EVENT,		/* Kernel -> App */
};

/*******************************************************************
* Protocol's message struct definition
*******************************************************************/
typedef struct proto_msg {
	char proto_prefix[PROTO_PREFIX_SIZE]; /* EM */
	enum ACTION action;
	size_t msg_size; /* Data size, 0 if no data exists. */
} proto_msg_t;

/*******************************************************************
* Prototypes
*******************************************************************/
int PROTO_is_valid_msg(proto_msg_t * msg);
void PROTO_assign_msg(proto_msg_t * msg, enum ACTION action);

#endif /* __PROT_H__ */



