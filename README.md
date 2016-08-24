# UCPUINFO

_ucpuinfo_ reads various bits of information about the current
execution environment and displays it.  It only supports IA-32
and thus fetches information visible to user processes regarding:

* segments
* segment descriptors
* machine status word
* descriptor tables

## TODO

There are a few things left on the old TODO list:

* CPUID
* initial state of floating point
* initial state of SSE/SSE2/etc
