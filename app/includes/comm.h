/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		comm.h
* Description:	Definitions for comm.c
*******************************************************************/

#ifndef __COMM_H__
#define __COMM_H__

#include <sys/socket.h>
#include <linux/netlink.h>

/*******************************************************************
* Structs
*******************************************************************/
struct comm_nl {
	struct sockaddr_nl src_addr;
	struct sockaddr_nl dst_addr;
	int nl_sock;
};

/*******************************************************************
* Constants
*******************************************************************/
#define NETLINK_EXECMON (31) /* Last possible netlink */


/*******************************************************************
* Prototypes
*******************************************************************/
int COMM_nl_request_conn(void);
int COMM_nl_send(void * send_msg, size_t len);
int COMM_nl_recv(void * recv_buff, size_t len);
int COMM_nl_init();
int COMM_nl_destruct();

#endif /* __COMM_H__ */



