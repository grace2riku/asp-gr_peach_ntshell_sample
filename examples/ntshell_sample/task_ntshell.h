/**
 * \file task_ntshell.h
 * \brief アプリケーション用インクルードファイル。コンフィギュレータも読み込む。
 */
#ifndef _TASK_NTSHELL_H_
#define _TASK_NTSHELL_H_

/*
 *  ターゲット依存の定義
 */
#include "target_test.h"

/*
 *  各タスクの優先度の定義
 */

#define HIGH_PRIORITY	9		/* 並行実行されるタスクの優先度 */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *  ターゲットに依存する可能性のある定数の定義
 */

#define	TASK_NT_SHELL_PORTID		3			/* 文字入力するシリアルポートID */

#define	STACK_SIZE		4096		/* タスクのスタックサイズ */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	task_ntshell(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */

#endif /* _TASK_NTSHELL_H_ */
