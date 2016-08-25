
CFLAGS=-Wall -g

ucpuinfo: ucpuinfo.c
	$(CC) $(CFLAGS) -o $@ ucpuinfo.c

clean:
	rm -f ucpuinfo.exe ucpuinfo
