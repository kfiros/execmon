/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2016
*
* File:		comm.c
* Description:	User<->Kernel communication.
*******************************************************************/

/*******************************************************************
* Includes
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "includes/general.h"
#include "includes/comm.h"
#include "../shared/proto.h"

struct comm_nl g_comm_nl; 

/*******************************************************************
* Name:		build_nl_msg	
* Description:	Builds a new netlink message with all of the needed
*		structs.
*******************************************************************/
static void build_nl_msg(struct sockaddr_nl * dest_addr,
			struct nlmsghdr ** nlh,
			struct iovec ** iov,
			struct msghdr * msg,
			size_t data_len) {

	*nlh = (struct nlmsghdr *) malloc(NLMSG_SPACE(data_len));	

	memset(*nlh, 0, NLMSG_SPACE(data_len));
	(*nlh)->nlmsg_len = NLMSG_SPACE(data_len);
	(*nlh)->nlmsg_pid = getpid();
	(*nlh)->nlmsg_flags = 0;

	/* Set iovec */
	(*iov)->iov_base = (void *) *nlh;
	(*iov)->iov_len = (*nlh)->nlmsg_len;

	/* Set msghdr struct */
	memset(msg, 0, sizeof(struct msghdr));
	msg->msg_name = (void * ) dest_addr;
	msg->msg_namelen = sizeof(struct sockaddr_nl);
	msg->msg_iov = *iov;
	msg->msg_iovlen = 1;

}

/*******************************************************************
* Name:		COMM_nl_send
* Description:	Sends a message over a netlink socket. Returns the
*		number of bytes successfully sent(including netlink
*		header size), or error.
*******************************************************************/
int COMM_nl_send(void * send_msg, size_t len) {
	int ret = SUCCESS;
	struct nlmsghdr * nlh;
	struct iovec iov;
	struct iovec * iov_p = &iov;
	struct msghdr msg;

	build_nl_msg(&(g_comm_nl.dst_addr), &nlh, &iov_p, &msg, len);
	memcpy(NLMSG_DATA(nlh), send_msg, len);

	ret = sendmsg(g_comm_nl.nl_sock, &msg, 0);
	if (0 >= ret) {
		goto cleanup;
	}

cleanup:
	if (NULL != nlh) {
		free(nlh);
	}

	return ret;

}

/*******************************************************************
* Name:		COMM_nl_recv
* Description:	Receives a message over a netlink socket. Returns
*		the number of bytes received(including netlink
*		header size), or error.
*******************************************************************/
int COMM_nl_recv(void * recv_buff, size_t len) {
	int ret = SUCCESS;
	struct nlmsghdr * nlh;
	struct iovec iov;
	struct iovec * iov_p = &iov;
	struct msghdr msg;

	build_nl_msg(&(g_comm_nl.dst_addr), &nlh, &iov_p, &msg, len);
	
	ret = recvmsg(g_comm_nl.nl_sock, &msg, 0);
	if (0 >= ret) {
		goto cleanup;
	}

	/* Copy the netlink message data to the given buffer */
	memcpy(recv_buff, NLMSG_DATA(nlh), nlh->nlmsg_len - NLMSG_HDRLEN);
	//memset(NLMSG_DATA(nlh), 0, nlh->nlmsg_len - NLMSG_HDRLEN);
	
cleanup:
	if (NULL != nlh) {
		free(nlh);
	}

	return ret;

}




/*******************************************************************
* Name:		COMM_nl_request_conn
* Description:	This function communicates with the kernel module,
*		and requests a monitoring session.	
*******************************************************************/
int COMM_nl_request_conn(void) {
	int ret = SUCCESS;
	int call_rv;
	proto_msg_t request_msg;
	proto_msg_t ans_msg;

	memset(&request_msg, 0, sizeof(proto_msg_t));
	memset(&ans_msg, 0, sizeof(proto_msg_t));
	
	// Assign needed values for starting the communication
	PROTO_assign_msg(&request_msg, CONN_REQUEST);
	
	call_rv = COMM_nl_send(&request_msg, sizeof(proto_msg_t));
	if (0 >= call_rv) {
		ret = call_rv;
		goto cleanup;	
	}

	call_rv = COMM_nl_recv(&ans_msg, sizeof(proto_msg_t));
	if (0 >= call_rv) {
		ret = ERROR;
		goto cleanup;
	}

	/* Validate the answer from the kernel */
	if ((false == PROTO_is_valid_msg(&ans_msg))
					|| ans_msg.action != CONN_ACCEPT) {
		ret = ERROR;
		goto cleanup;
	}


cleanup:
	return ret;	
}


/*******************************************************************
* Name:		COMM_nl_init
* Description:	Creates the netlink socket and binds it.
*******************************************************************/
int COMM_nl_init() {
	int ret = SUCCESS;
	int call_rv;
	int * nl_sock = &(g_comm_nl.nl_sock);
	struct sockaddr_nl * src_addr = &(g_comm_nl.src_addr);
	struct sockaddr_nl * dst_addr = &(g_comm_nl.dst_addr);

	*nl_sock = -1;
	
	/* Init the source sockaddr, with our application PID */
	memset(src_addr, 0, sizeof(struct sockaddr_nl));
	src_addr->nl_family = AF_NETLINK;
	src_addr->nl_pid = getpid();

	/* Create the netlink socket */
	*nl_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_EXECMON);
	if (0 > *nl_sock) {
		ret = ERROR;
		goto cleanup;
	}

	call_rv = bind(*nl_sock, (struct sockaddr *) src_addr,
						sizeof(struct sockaddr_nl)); 
	if (SUCCESS != call_rv) {
		ret = ERROR;
		goto cleanup;
	}

	/* Init the destination sockaddr */
	memset(dst_addr, 0, sizeof(struct sockaddr_nl));
	dst_addr->nl_family = AF_NETLINK;
	dst_addr->nl_pid = 0;
	dst_addr->nl_groups = 0;

cleanup:
	return ret;
}


/*******************************************************************
* Name:		COMM_nl_destruct
* Description:	Closes the netlink socket 
*******************************************************************/
int COMM_nl_destruct() {
	int ret = SUCCESS;
	int call_rv;
	int * nl_sock = &(g_comm_nl.nl_sock);

	if (-1 == *nl_sock) {
		goto cleanup;
	}

	/* Close the netlink socket */
	call_rv = close(*nl_sock);
	if (SUCCESS != call_rv) {
		ret = ERROR;
		goto cleanup;
	}

cleanup:
	return ret;
}







