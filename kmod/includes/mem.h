/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		mem.h
* Description:	Definitions for mem.c
*******************************************************************/

#ifndef __MEM_H__
#define __MEM_H__

/*******************************************************************
* Constants
*******************************************************************/
#define RELATIVE_CALL_SIZE (5)

/*******************************************************************
* Prototypes
*******************************************************************/
int MEM_make_rw(unsigned long addr);

long MEM_find_insn_off(unsigned long mem_addr,
				size_t block_size,
				int insn_type,
				size_t insn_size);

int MEM_patch_relative_call(unsigned long mem_addr,
				size_t block_size,
				unsigned long new_call_addr,
				unsigned long * orig_call_addr);

#endif /* __MEM_H__ */



