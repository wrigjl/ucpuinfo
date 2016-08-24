
CFLAGS=-Wall

ucpuinfo: ucpuinfo.c
	$(CC) $(CFLAGS) -o $@ ucpuinfo.c
