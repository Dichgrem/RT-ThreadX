/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-10     hythat       the first version
 */
#ifndef APPLICATIONS_APP_CJSON_H_
#define APPLICATIONS_APP_CJSON_H_

#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cJSON.h"

/* 上传数据结构 */
typedef struct msg_upload{
    char location[10];
    cJSON_bool fan;
    double temp;
    int humi;
}msg_upload_def, *pmsg_upload_def;

/* 下发数据结构 */
typedef struct msg_recv{
    char location[10];
    cJSON_bool fan;
}msg_recv_def, *pmsg_recv_def;

char *msg_upload_pack(pmsg_upload_def msg);
void msg_recv_parse(pmsg_recv_def msg_recv, char *json_string);

#endif /* APPLICATIONS_APP_CJSON_H_ */
