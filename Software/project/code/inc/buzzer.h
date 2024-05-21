#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "zf_common_headfile.h"

#define BUZZER_PIN B16

/**
 * @brief 使得蜂鸣器叫一次
 *
 *
*/

#define BUZZER_SPEAK	rt_mb_send(buzzer_mail,1)

extern rt_mailbox_t buzzer_mail;

void buzzer_init();


#endif