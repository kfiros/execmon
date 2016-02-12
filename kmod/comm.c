/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		comm.c
* Description:	This module implements the kernel netlink
*		communication with the user app.
*******************************************************************/

#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include "includes/general.h"
#include "includes/comm.h"
#include "../shared/proto.h"

/* Global structs */
struct comm_nl g_comm_nl;
struct netlink_kernel_cfg cfg = {
	.input = COMM_nl_input,
};


/*******************************************************************
* Name:		nl_init_comm_nl
* Description:	This function inits the netlink communication struct.
*******************************************************************/
static void nl_init_comm_nl(struct comm_nl * comm_nl) {
	if (NULL == comm_nl) {
		return;
	}		

	comm_nl->user_pid = -1;
	comm_nl->nl_sock = NULL;
}

/*******************************************************************
* Name:		nl_send_to_user_by_pid
* Description:	This function sends a given buffer to a specific
*		user application, by the given pid.
*******************************************************************/
int nl_send_to_user_by_pid(void * send_buff, size_t len, pid_t user_pid) {
	struct nlmsghdr * nlh = NULL;
	struct sk_buff * skb_out = NULL;

	/* Allocate a new netlink msg */
	skb_out = nlmsg_new(len, 0);
	if (NULL == skb_out) {
		return ERROR;
	}
	
	/* Add a new netlink message to the skb */	
	nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, len, 0);
	if (NULL == nlh) {
		goto nl_failure;
	}

	NETLINK_CB(skb_out).dst_group = 0;

	/* Copy the message to the nl message */
	memcpy(nlmsg_data(nlh), send_buff, len);

	/* Send the skb to the user space app */	
	return nlmsg_unicast(g_comm_nl.nl_sock, 
				skb_out, user_pid);	

nl_failure:
	nlmsg_cancel(skb_out, nlh);	
	kfree_skb(skb_out);
	return ERROR;
}


/*******************************************************************
* Name:		nl_send_to_user
* Description:	This function sends a given buffer to the registered
*		user app.
*******************************************************************/
int nl_send_to_user(void * send_buff, size_t len) {
	return nl_send_to_user_by_pid(send_buff, len, g_comm_nl.user_pid);
}

/*******************************************************************
* Name:		COMM_nl_send_exec_msg
* Description:	Sends the user message about a specific program 
*		execution event.
*******************************************************************/
int COMM_nl_send_exec_msg(char * exec_str) {
	int ret = SUCCESS;
	int call_rv;
	proto_msg_t exec_msg;	

	if (NULL == exec_str) {
		ret = ERROR;
		goto cleanup;
	}

	/* Init the protocol message */
	PROTO_assign_msg(&exec_msg, EXEC_EVENT); 
	exec_msg.msg_size = strlen(exec_str) + 1; /* + 1 for null terminator */

	/* Send packet's header */
	call_rv = nl_send_to_user(&exec_msg, sizeof(proto_msg_t));
	if (0 > call_rv) {
		ret = call_rv;
		goto cleanup;
	}

	/* Send packet's content */
	call_rv = nl_send_to_user(exec_str, exec_msg.msg_size);
	if (0 > call_rv) {
		ret = call_rv;
		goto cleanup;
	}

	
cleanup:
	if (NULL != exec_str) {
		/* Free allocated memory */
		vfree(exec_str);
	}

	return ret;
}


/*******************************************************************
* Name:		handle_new_user_app
* Description:	Currently, we allow only one instance of the user
*		client. In case we receive a message with an unknown
*		PID, we examine if an active session exists.
*		If an active session exists, we notify the new user
*		app about failure. Otherwise, we replace the old user
*		app with the new one.
*******************************************************************/
static void handle_new_user_app(pid_t user_pid) {
	int call_rv;
	proto_msg_t accept_msg, keep_alive, fail_msg;

	/* Check user application PID validity */
	if (0 >= user_pid) {
		return;
	}

	/* Assign the accept message */
	PROTO_assign_msg(&accept_msg, CONN_ACCEPT);

	/* If no user apps are connected, save the current app PID */
	if (-1 == g_comm_nl.user_pid) {
		call_rv = nl_send_to_user_by_pid(&accept_msg, sizeof(proto_msg_t), user_pid);
		if (SUCCESS != call_rv) {
			return;
		}

		g_comm_nl.user_pid = user_pid;
		return;
	}	

	/* In case we received a message from a new user application */
	if (g_comm_nl.user_pid != user_pid) {
		/* Check whether the user app is still alive */
		PROTO_assign_msg(&keep_alive, KEEP_ALIVE);
		call_rv = nl_send_to_user(&keep_alive, sizeof(proto_msg_t));
		if (SUCCESS == call_rv) {
			PROTO_assign_msg(&fail_msg, sizeof(proto_msg_t));
			nl_send_to_user_by_pid(&fail_msg, sizeof(proto_msg_t), user_pid);
		} else {
			g_comm_nl.user_pid = -1;

			/* Accept the new connection */
			call_rv = nl_send_to_user_by_pid(&accept_msg, sizeof(proto_msg_t), user_pid);
			if (SUCCESS != call_rv) {
				return;
			}

			g_comm_nl.user_pid = user_pid;
		}
	}
}

/*******************************************************************
* Name:		nl_recv_from_user
* Description:	This function handles a received nl sk_buff from the
*		user app.
*******************************************************************/
void nl_recv_from_user(struct sk_buff * skb) {
	struct nlmsghdr * nlh = NULL;
	void * recv_buff = NULL;
	proto_msg_t * proto_msg = NULL;
	pid_t user_pid;

	if (NULL == skb) {
		return;
	}	

	nlh = (struct nlmsghdr *) skb->data;
	if (NULL == nlh) {
		return;
	}

	recv_buff = nlmsg_data(nlh);
	if (NULL == recv_buff) { 
		return;
	}

	proto_msg = (proto_msg_t *) recv_buff;

	/* Validate received message from user mode */
	if (false == PROTO_is_valid_msg(proto_msg) || 
					(proto_msg->action != CONN_REQUEST)) {
		return;
	}

	/* Get user application PID */
	user_pid = nlh->nlmsg_pid; 

	/* Handle the new app connection request */
	handle_new_user_app(user_pid);


}

/*******************************************************************
* Name:		COMM_nl_input
* Description:	This function is the input entry point of the netlink
*		socket. 
*******************************************************************/
void COMM_nl_input(struct sk_buff * skb) {	
	nl_recv_from_user(skb);
}

/*******************************************************************
* Name:		COMM_nl_init
* Description:	Makes the needed preparations for using netlink ipc.
*******************************************************************/
int COMM_nl_init(void) {
	int ret = SUCCESS;

	/* Init the global netlink struct */
	nl_init_comm_nl(&g_comm_nl);

	DBG_PRINT("Creating netlink socket");
	g_comm_nl.nl_sock = netlink_kernel_create(&init_net,
							NETLINK_EXECMON,
							&cfg);
	if (NULL == g_comm_nl.nl_sock) {
		ret = ERROR;
		goto cleanup;
	}

cleanup:
	return ret;
}

/*******************************************************************
* Name:		COMM_nl_destruct
* Description:	Releases the netlink socket and restores the initial
*		state of the global netlink struct.
*******************************************************************/
int COMM_nl_destruct(void) {
	int ret = SUCCESS;

	/* Release the netlink socket */
	netlink_kernel_release(g_comm_nl.nl_sock);

	/* Reset the global netlink struct */
	nl_init_comm_nl(&g_comm_nl);

	return ret;
}
