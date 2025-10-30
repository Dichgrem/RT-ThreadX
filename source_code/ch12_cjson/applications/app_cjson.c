/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-10     hytha       the first version
 */
/* 该文件根据协议完成组包和解析 */
/* 协议数据示例
***********上传**********
{
    "location":"room1",
    "fan":true,
    "environment":{
        "temp":25.5,
        "humi":45
    }
}
***********下发**********
{
    "location":"room1",
    "fan":true
}
*/

#include "app_cjson.h"

/* 上传数据组包 */
char *msg_upload_pack(pmsg_upload_def msg)
{
    char *json_string = NULL;  // 返回值json字符串
    cJSON *monitor = NULL;     //所有监控信息节点
    cJSON *environment = NULL; //温湿度信息节点
    cJSON *location = NULL;    //位置
    cJSON *fan = NULL;         //风扇开关
    cJSON *temp = NULL;        //温度
    cJSON *humi = NULL;        //湿度

    /* 创建monitor节点，包含所有参数 */
    monitor = cJSON_CreateObject();
    if (monitor == NULL)
    {
        goto end;
    }

    /* 创建location字段，并添加至monitor */
    location = cJSON_CreateString(msg->location);
    if(location == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "location", location);

    /* 创建fan字段，并添加至monitor */
    fan = cJSON_CreateBool(msg->fan);
    if(fan == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "fan", fan);

    /* 创建environment节点，包含温湿度参数 */
    environment = cJSON_CreateObject();
    cJSON_AddItemToObject(monitor, "environment", environment);
    if (environment == NULL)
    {
        goto end;
    }
    /* 创建temp字段，并添加至environment */
    temp = cJSON_CreateNumber(msg->temp);
    if(temp == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(environment, "temp", temp);

    /* 创建humi字段，并添加至environment */
    humi = cJSON_CreateNumber(msg->humi);
    if(humi == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(environment, "humi", humi);

    json_string = cJSON_PrintUnformatted(monitor);

    /* 异常情况统一Delete（free） */
end:
    /* 删除节点，释放内存 */
    cJSON_Delete(monitor);
    return json_string;
}
/* 下发数据解析 */
void msg_recv_parse(pmsg_recv_def msg_recv, char *json_string)
{
    /* 创建解析节点 */
    cJSON *msg_json = NULL;
    /* 解析数据 */
    msg_json = cJSON_Parse(json_string);
    /* 获取有用数据 */
    msg_recv->fan = (cJSON_bool)(cJSON_GetObjectItem(msg_json, "fan")->valueint);
    rt_strncpy(msg_recv->location, cJSON_GetObjectItem(msg_json, "location")->valuestring, 10);
    /* 删除节点，释放内存 */
    cJSON_Delete(msg_json);
}
