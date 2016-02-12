/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		syscalls.h
* Description:	definitions for syscalls.c
*******************************************************************/

#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

/*******************************************************************
* Constants
*******************************************************************/
#define SYM_SYS_CALL_TABLE ("sys_call_table")

#define MAX_RELATIVE_CALL_OFFSET (150)
 
/*******************************************************************
* Prototypes
*******************************************************************/
int SYSCALLS_set_hooks(void);
int SYSCALLS_remove_hooks(void);

#endif /* __SYSCALLS_H__ */



