/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		general.h
* Description:	Project's general header file
*******************************************************************/

#ifndef __GENERAL_H__
#define __GENERAL_H__

/*******************************************************************
* Constants
*******************************************************************/

/* Application parameters */
#define DEBUG (0)

#define SUCCESS (0)
#define ERROR (-1)

/* Terminal colors */
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KMAG "\x1B[35m"


/* Bool typedef */
typedef enum {
	false = 0,
	true
} bool;


/*******************************************************************
* Macros
*******************************************************************/
#if DEBUG
	#define DBG_PRINT(fmt, args...) do { fprintf(stderr, "[*] DEBUG: " fmt "\n" \
						, ##args); } while (false)
#else
	#define DBG_PRINT(...) do {} while (false)
#endif

#define UNUSED(expr) do { (void)(expr); } while (false)

#endif /* __GENERAL_H__ */



