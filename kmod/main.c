/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		main.c
* Description:	Main file of the kernel module
*******************************************************************/

/*******************************************************************
* Includes
*******************************************************************/
#include <asm/unistd.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <linux/kallsyms.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>

#include "includes/general.h"
#include "includes/syscalls.h"
#include "includes/comm.h"
#include "udis86.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kfir Shtober (Kfiros) kfir97@gmail.com");
MODULE_DESCRIPTION("ExecMon");

/*******************************************************************
* Name:		mod_main
* Description:	Main function of the kernel module.
*******************************************************************/
static int mod_main(void)
{
	int ret = SUCCESS;
	int call_rv;


	/* Setup netlink kernel<->user communication */
	call_rv = COMM_nl_init();
	if (SUCCESS != call_rv) {
		KLOG_PRINT("Cannot set netlink socket, exiting...");
		ret = call_rv;
		goto cleanup;		
	}

	/* Set needed hooks */	
	call_rv = SYSCALLS_set_hooks();
	if (SUCCESS != call_rv) {
		KLOG_PRINT("Cannot set hooks, exiting...");
		ret = call_rv;
		goto cleanup;
	}

	KLOG_PRINT("kmod started successfully");

cleanup:
	return ret;
}

/*******************************************************************
* Name:		mod_exit
* Description:	The exit function of the kernel module.
*******************************************************************/
static void mod_exit(void)
{
	KLOG_PRINT("Exiting");
	
	/* Restore original syscalls */
	SYSCALLS_remove_hooks();

	/* Close netlink socket */
	COMM_nl_destruct();

	return;	
}

/* Init main entrypoints of the kernel module */
module_init(mod_main);
module_exit(mod_exit);
