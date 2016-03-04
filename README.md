## execmon
`execmon` is an advanced process execution monitoring utility for linux. 

The project consists of a kernel module and a user mode utility. The kernel module tracks new process executions, or precisely, intercepts the `execve` syscall.
Whenever the kernel intercepts a new execution, it notifies the user immediately about it.

In the past, hooking syscalls in the Linux kernel was an easier task, however, in newer kernels, assembly stubs were added to the syscalls.
`execmon` overcomes this obstacle, patching the kernel on the fly. For this purpose I used the open source project Udis86.

<img src="https://raw.githubusercontent.com/kfiros/execmon/master/execmon.png" />

### Future Goals
* Intercept more syscalls
* Better graphical data presentation for the user
* Save sessions data
* Support 32 bit systems

### Notes
* Currently supports only 64 bit 
* Currently Attempted to run only on Ubuntu 14.04 (Kernel 3.13)
* Use on your own risk

### How To Use
* Compile using `make`
* Insert the kernel module (using `insmod`)
* Run the user application

## Author
Kfir Shtober (Kfiros) 2016

## Thanks & Credits
* Ilya V. Matveychikov (https://github.com/milabs)
* Udis86 (https://github.com/vmt/udis86)


