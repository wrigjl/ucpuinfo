/*
 * Copyright (c) 2014-2016 Jason L. Wright (jason@thought.net)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct dts {
	uint16_t limit;
	unsigned long base;
} __attribute__((packed));

void showit(int s);
uint16_t cs(void);
uint16_t ds(void);
uint16_t es(void);
uint16_t fs(void);
uint16_t gs(void);
uint16_t ss(void);
void sidt(struct dts *);
void sgdt(struct dts *);
uint16_t sldt(void);
uint16_t str(void);
uint32_t smsw(void);
int verr(uint16_t);
int verw(uint16_t);
int validsl(int);
uint32_t lsl(int);
uint32_t lar(int);

const char *segtypes[] = {
	"Reserved-0",
	"Available 16-bit TSS",
	"LDT",
	"Busy 16-bit TSS",
	"16-bit call gate",
	"16-bit/32-bit task gate",
	"16-bit interrupt gate",
	"16-bit trap gate",

	"Reserved-8",
	"Available 32-bit TSS",
	"Reserved-A",
	"Busy 32-bit TSS",
	"32-bit call gate",
	"Reserved-D",
	"32-bit interrupt gate",
	"32-bit trap gate"
};

const char *datasegs[] = {
	"read-only",
	"read-only,accessed",
	"read/write",
	"read/write,accessed",
	"read-only,expand-down",
	"read-only,expand-down,accessed",
	"read/write,expand-down",
	"read/write,expand-down,accessed",
	"exec-only",
	"exec-only,accessed",
	"exec/read",
	"exec/read,accessed",
	"exec-only,conforming",
	"exec-only,conforming,accessed",
	"exec/read,conforming",
	"exec/read,conforming,accessed"
};

int
main(int argc, char *argv[]) {
	int i;
	struct dts dt;

	printf("MSW=%08x\n", smsw());
	printf("CS=0x%04x DS=0x%04x ES=0x%04x FS=0x%04x GS=0x%04x SS=0x%04x",
		cs(), ds(), es(), fs(), gs(), ss());
	printf(" TR=0x%04x\n", str());

	memset(&dt, '\0', sizeof(dt));
	sgdt(&dt);
	printf("GDT base=0x%lx limit=0x%x\n", dt.base, dt.limit);

	memset(&dt, '\0', sizeof(dt));
	sidt(&dt);
	printf("IDT base=0x%lx limit=0x%x\n", dt.base, dt.limit);

	printf("LDT=0x%04x\n", sldt());

	printf("GLOBAL DESCRIPTOR TABLE\n");
	for (i = 0; i < 65536; i += 8)
		showit(i);

	printf("LOCAL DESCRIPTOR TABLE\n");
	for (i = 4; i < 65536; i += 8)
		showit(i);

	return 0;
}

void
showit(int s)
{
	int r = verr(s), w = verw(s);
	unsigned type, la = lar(s), s_flag, dpl, p_flag, sa_flag, l_flag, db_flag, g_flag;

	if (!validsl(s))
		return;
	printf("%-2d 0x%03x: LAR=0x%08x LSL=0x%08x%s%s%s",
		s >> 3, s, lar(s), lsl(s),
		(r || w) ? " " : "",
		r ? "r" : "",
		w ? "w" : "");

	type = (la >> 8) & 0xf;
	s_flag = la & (1 << 12);
	dpl = (la >> 13) & 0x3;
	p_flag = la & (1 << 15);
	sa_flag = la & (1 << 20);
	l_flag = la & (1 << 21);
	db_flag = la & (1 << 22);
	g_flag = la & (1 << 23);

	printf("\n  ");

	if (s_flag == 0)
		printf(" %s", segtypes[type]);
	else
		printf(" %s", datasegs[type]);

	printf(" dpl=%u granularity=%s %spresent",
		dpl,
		g_flag ? "page" : "byte",
		p_flag ? "" : "not-");
	if (s_flag && type >= 8)
		printf(" %u-bit,code", l_flag ? 64 : (db_flag ? 32 : 16));
	printf(" reserved-bit=%s", sa_flag ? "set" : "clear");
	printf("\n");
}

uint16_t
cs(void) {
	uint16_t r;

	asm volatile("movw %%cs, %0" : "=r" (r));
	return r;
}

uint16_t
ds(void) {
	uint16_t r;

	asm volatile("movw %%ds, %0" : "=r" (r));
	return r;
}

uint16_t
es(void) {
	uint16_t r;

	asm volatile("movw %%es, %0" : "=r" (r));
	return r;
}

uint16_t
fs(void) {
	uint16_t r;

	asm volatile("movw %%fs, %0" : "=r" (r));
	return r;
}

uint16_t
gs(void) {
	uint16_t r;

	asm volatile("movw %%gs, %0" : "=r" (r));
	return r;
}

uint16_t
ss(void) {
	uint16_t r;

	asm volatile("movw %%ss, %0" : "=r" (r));
	return r;
}

void
sgdt(struct dts *p)
{
	asm volatile("sgdt %0" : "=m" (*p));
}

void
sidt(struct dts *p)
{
	asm volatile("sidt %0" : "=m" (*p));
}

uint16_t
sldt(void)
{
	uint16_t r;

	asm volatile("sldt %0" : "=rm" (r));
	return (r);
}

uint32_t
smsw(void) {
	uint32_t r;

	asm volatile("smsw %0" : "=r" (r));
	return (r);
}

uint16_t
str(void) {
	uint16_t r;

	asm volatile("str %0" : "=rm" (r));
	return (r);
}

int
verr(uint16_t s) {
	uint8_t r;

	asm volatile ("verr %1; setz %0" : "=r" (r): "rm" (s) : "cc");
	return (r);
}

int
verw(uint16_t s) {
	uint8_t r;

	asm volatile ("verw %1; setz %0" : "=r" (r): "rm" (s) : "cc");
	return (r);
}

int
validsl(int s)
{
	uint8_t r;

	asm volatile ("lsl %1, %%eax; setz %0" : "=r" (r) : "rm" (s) : "cc", "%eax");
	return (r);
}

uint32_t
lsl(int s) {
	uint32_t r;

	asm volatile("lsl %1, %0" : "=a" (r) : "r" (s) : "cc");
	return (r);
}

uint32_t
lar(int s) {
	uint32_t r;

	asm volatile("lar %1, %0" : "=a" (r) : "r" (s) : "cc");
	return (r);
}
