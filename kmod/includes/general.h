/*******************************************************************
* Project:	ExecMon
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		general.h
* Description:	General definitions & macros
*******************************************************************/

#ifndef __GENERAL_H__
#define __GENERAL_H__

/*******************************************************************
* Constants
*******************************************************************/

/* Application parameters */
#define DEBUG (0)

/* Application return values */
enum errors {
	SUCCESS = 0,
	ERROR,
};

/*******************************************************************
* Macros
*******************************************************************/

/* Debug prints */
#if DEBUG
	#define DBG_PRINT(fmt, args...) do { printk(KERN_DEBUG "[*] ExecMon: <%s>: " fmt "\n" \
						,__func__, ##args); } while (false)
#else
	#define DBG_PRINT(...) do {} while (false)
#endif

/* Linux kernel log prints */
#define KLOG_PRINT(fmt, args...) do { printk(KERN_DEBUG "[*] ExecMon: " fmt "\n" \
					, ##args); } while (false)

#define UNUSED(expr) do { (void)(expr); } while (false)

#endif /* __GENERAL_H__ */



