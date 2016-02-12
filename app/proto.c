/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2016
*
* File:		shared/proto.c
* Description:	Protocol for kernel user communication. This file
*		should be shared between the kernel part and also
*		the user application.
*******************************************************************/

/*******************************************************************
* Includes
*******************************************************************/
#include <string.h>
#include "../shared/proto.h"

/*******************************************************************
* Name:		PROTO_is_valid_proto_msg
* Description:	Validates a given protocol message. Returns true
*		if the message is valid, or false if not.
*******************************************************************/
int PROTO_is_valid_msg(proto_msg_t * msg) {
	if (NULL == msg) {
		return 0;
	}

	/* Validate message */
	if (0 == memcmp(msg->proto_prefix, PROTO_PREFIX, PROTO_PREFIX_SIZE)) {
		return 1;
	}

	return 0;
}
/*******************************************************************
* Name:		PROTO_assign_msg
* Description:	Inits a given protocol message, and assigns it
*		with the provided action type.
*******************************************************************/
void PROTO_assign_msg(proto_msg_t * msg, enum ACTION action) {
	if (NULL == msg) {
		return;
	}

	memcpy(msg->proto_prefix, PROTO_PREFIX, PROTO_PREFIX_SIZE); 
	msg->action = action;
	msg->msg_size = 0;
}

