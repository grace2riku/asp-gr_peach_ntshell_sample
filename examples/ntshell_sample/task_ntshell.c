/**
 * \file task_ntshell.c
 * \brief アプリケーションの本体ファイル。
*/

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "task_ntshell.h"
#include "ntshell.h"
#include "ntopt.h"
#include "usrcmd.h"
#include "xprintf.h"

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

#define UNUSED_VARIABLE(N)  do { (void)(N); } while (0)

extern void target_fput_log(char c);


static int func_read(char *buf, int cnt, void *extobj)
{
	UNUSED_VARIABLE(extobj);
    return serial_rea_dat(TASK_NT_SHELL_PORTID, buf, cnt);
}

static int func_write(const char *buf, int cnt, void *extobj)
{
	UNUSED_VARIABLE(extobj);
    return serial_wri_dat(TASK_NT_SHELL_PORTID, buf, cnt);
}

static int func_callback(const char *text, void *extobj)
{
#if 1
	usrcmd_execute(text);
#else
	ntshell_t *ntshell = (ntshell_t *)extobj;
	UNUSED_VARIABLE(ntshell);
	UNUSED_VARIABLE(extobj);
	int text_length = ntlibc_strlen(text);
	if (text_length > 0) {
		serial_wri_dat(TASK_NT_SHELL_PORTID, "User input text:'", sizeof("User input text:'"));
		serial_wri_dat(TASK_NT_SHELL_PORTID, text, text_length);
		serial_wri_dat(TASK_NT_SHELL_PORTID, "'\r\n", sizeof("'\r\n"));
	}
#endif
	return 0;
}


/**
 * \brief メインタスク
 * \param exinf コンフィギュレータから渡す引数。今回は利用しない
 * \details
 */
void task_ntshell(intptr_t exinf)
{
	ER_UINT	ercd;
	ntshell_t ntshell;

	syslog(LOG_NOTICE, "task_ntshell: Started.");
    serial_opn_por(TASK_NT_SHELL_PORTID);

	ercd = serial_opn_por(TASK_NT_SHELL_PORTID);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_NT_SHELL_PORTID,
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	/* xprintfシリアル送信関数登録 */
	xdev_out(target_fput_log);
	
	ntshell_init(
		&ntshell,
		func_read,
		func_write,
		func_callback,
		(void *)&ntshell);
	ntshell_set_prompt(&ntshell, ">");
	ntshell_execute(&ntshell);
	
}

