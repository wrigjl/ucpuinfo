# UCPUINFO

_ucpuinfo_ reads various bits of information about the current
execution environment and displays it.  It only supports IA-32
and thus fetches information visible to user processes regarding:

* segments
* segment descriptors
* machine status word
* descriptor tables

## Compiling / Using

_ucpuinfo_ has a Makefile that works with cygwin, MacOS, Linux, FreeBSD,
and OpenBSD.  Assuming you have a C compiler and "make" installed:

    # make

The doesn't have any command line arguments, it just spits information out.

## TODO

There are a few things left on the old TODO list:

* CPUID
* initial state of floating point
* initial state of SSE/SSE2/etc
