/**
 * @file usrcmd.c
 * @author CuBeatSystems
 * @author Shinichiro Nakamura
 * @copyright
 * ===============================================================
 * Natural Tiny Shell (NT-Shell) Version 0.3.1
 * ===============================================================
 * Copyright (c) 2010-2016 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ntopt.h"
#include "ntlibc.h"

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "task_ntshell.h"
#include "xprintf.h"

typedef int (*USRCMDFUNC)(int argc, char **argv);

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int usrcmd_help(int argc, char **argv);
static int usrcmd_info(int argc, char **argv);
static int usrcmd_ledon(int argc, char **argv);
static int usrcmd_ledoff(int argc, char **argv);
static int usrcmd_memory_read(int argc, char **argv);

static char mr_cmd_example[] = "mr <[b|h|w]> <addr> [count]\n"
"1byte * 16count read example) >mr b 0x1801e35d 16\n"
"1801E35D  54 4F 50 50 45 52 53 2F 41 53 50 20 4B 65 72 6E  TOPPERS/ASP Kern\n"
"\n"
"2byte * 8count read example) >mr h 0x1801e35d 8\n"
"1801E35D  4F54 5050 5245 2F53 5341 2050 654B 6E72\n"
"\n"
"4byte * 4count read example) >mr w 0x1801e35d 4\n"
"1801E35D  50504F54 2F535245 20505341 6E72654B\n"
"\n"
"1byte read example) >mr b 0x1801e35d\n"
"1801E35D  54  T\n"
"\n"
"2byte read example) >mr h 0x1801e35d\n"
"1801E35D  4F54\n"
"\n"
"4byte read example) >mr w 0x1801e35d\n"
"1801E35D  50504F54\n";

typedef struct {
    char *cmd;
    char *desc;
    USRCMDFUNC func;
} cmd_table_t;

static const cmd_table_t cmdlist[] = {
    { "help", "This is a description text string for help command.", usrcmd_help },
    { "info", "This is a description text string for info command.", usrcmd_info },
    { "ledon", "This is a LED turn on command.", usrcmd_ledon },
    { "ledoff", "This is a LED turn off command.", usrcmd_ledoff },
    { "mr", mr_cmd_example, usrcmd_memory_read },
};

#define LED1_RED_INDEX		(0)
#define LED2_GREEN_INDEX	(1)
#define LED3_BLUE_INDEX		(2)
#define LED4_USER_INDEX		(3)

#define INFO_SYSTEM			"GR-PEACH"
#define INFO_VERSION		"0.1.0"

extern void set_led(uint8_t led, bool_t on);


int usrcmd_execute(const char *text)
{
    return ntopt_parse(text, usrcmd_ntopt_callback, 0);
}

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
    if (argc == 0) {
        return 0;
    }
    const cmd_table_t *p = &cmdlist[0];
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
        if (ntlibc_strcmp((const char *)argv[0], p->cmd) == 0) {
            return p->func(argc, argv);
        }
        p++;
    }
	syslog_0(LOG_NOTICE, "Unknown command found.");

	return 0;
}

static int usrcmd_help(int argc, char **argv)
{
    const cmd_table_t *p = &cmdlist[0];
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
		syslog_3(LOG_NOTICE, "%s%s%s", p->cmd, "\t:", p->desc);
    	p++;
    }
    return 0;
}

static int usrcmd_info(int argc, char **argv)
{
    if (argc != 2) {
		syslog_0(LOG_NOTICE, "info sys");
		syslog_0(LOG_NOTICE, "info ver");

    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "sys") == 0) {
		syslog_0(LOG_NOTICE, INFO_SYSTEM);
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "ver") == 0) {
		syslog_0(LOG_NOTICE, INFO_VERSION);
    	return 0;
    }
	syslog_0(LOG_NOTICE,  "Unknown sub command found");
	return -1;
}


static int usrcmd_ledon(int argc, char **argv)
{
    if (argc != 2) {
		syslog_0(LOG_NOTICE, "ledon red or green or blue or user");
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "red") == 0) {
		set_led(LED1_RED_INDEX, true);
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "green") == 0) {
		set_led(LED2_GREEN_INDEX, true);
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "blue") == 0) {
		set_led(LED3_BLUE_INDEX, true);
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "user") == 0) {
		set_led(LED4_USER_INDEX, true);
    	return 0;
    }
	syslog_0(LOG_NOTICE,  "Unknown sub command found");
	return -1;
}


static int usrcmd_ledoff(int argc, char **argv)
{
    if (argc != 2) {
		syslog_0(LOG_NOTICE, "ledoff red or green or blue or user");
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "red") == 0) {
		set_led(LED1_RED_INDEX, false);
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "green") == 0) {
		set_led(LED2_GREEN_INDEX, false);
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "blue") == 0) {
		set_led(LED3_BLUE_INDEX, false);
    	return 0;
    }
    if (ntlibc_strcmp(argv[1], "user") == 0) {
		set_led(LED4_USER_INDEX, false);
    	return 0;
    }
	syslog_0(LOG_NOTICE,  "Unknown sub command found");
	return -1;
}


static int usrcmd_memory_read(int argc, char **argv){
	long address;
	long count = 1;
	int read_size;
	char *ptr;

	if (argc < 3) {
		syslog_0(LOG_NOTICE, "mrb <[b|h|w]> <addr> [count]");
    	return 0;
    }
	
    if (ntlibc_strcmp(argv[1], "b") == 0) {
		read_size = 1;
    }
    else if (ntlibc_strcmp(argv[1], "h") == 0) {
		read_size = 2;
    }
    else if (ntlibc_strcmp(argv[1], "w") == 0) {
		read_size = 4;
    } else {
		syslog_0(LOG_NOTICE, "memory read size is [b|h|w].");
    	return 0;
    }
	
	if (!xatoi(&argv[2], &address)) {
		syslog_0(LOG_NOTICE, "address error");
		return 0;
	}

	if (argc == 4) {
		if (!xatoi(&argv[3], &count)) {
			syslog_0(LOG_NOTICE, "count error");
			return 0;
		}
    }

	for (ptr = (char*)address; count >= 16 / read_size; ptr += 16, count -= 16 / read_size)
		put_dump(ptr, (ulong_t)ptr, 16 / read_size, read_size);
	if (count) put_dump((char*)ptr, (uint_t)ptr, count, read_size);
	
	return 0;
}


