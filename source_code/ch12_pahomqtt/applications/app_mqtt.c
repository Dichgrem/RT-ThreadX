/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-08     hytha       the first version
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include "paho_mqtt.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME    "app.mqtt"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#define MQTT_URI        "tcp://139.9.138.250:1883" //MQTT服务器地址：端口
#define MQTT_USERNAME   "hythait"  //MQTT用户名
#define MQTT_PASSWORD   "hythait"  //MQTT密码
#define MQTT_SUBTOPIC   "/mqtt/test"  //默认订阅主题
#define MQTT_PUBTOPIC   "/mqtt/test"  //默认发布主题
#define MQTT_WILLMSG    "Goodbye!"    //遗嘱消息

/* 定义MQTT客户端 */
static MQTTClient client;
static int is_started = 0;

/* 订阅主题的回调函数 */
static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

/* 订阅主题的默认回调函数 */
static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

/* MQTT连接回调函数 */
static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
}

/* MQTT上线回调函数 */
static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
}

/* MQTT下线回调函数 */
static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
}

/* 启动MQTT客户端 */
static int mqtt_start(void)
{
    /* 初始化连接参数 */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };

    if (is_started)
    {
        LOG_E("mqtt client is already connected.");
        return -1;
    }
    /* 设置MQTT报文参数 */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;

        /* 随机生成客户端ID */
        rt_snprintf(cid, sizeof(cid), "rtthread%d", rt_tick_get());
        /* 配置连接参数 */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = cid;
        client.condata.keepAliveInterval = 30;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* 配置遗嘱消息参数 */
        client.condata.willFlag = 1;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

        /* 开辟内存 */
        client.buf_size = client.readbuf_size = 1024;
        client.buf = rt_calloc(1, client.buf_size);
        client.readbuf = rt_calloc(1, client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            return -1;
        }

        /* 设置连接事件回调函数 */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* 设置订阅列表及回调函数 */
        client.messageHandlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* 设置默认订阅事件回调函数 */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }

    /* 启动MQTT */
    paho_mqtt_start(&client);
    is_started = 1;

    return 0;
}

/* 停止MQTT客户端 */
static int mqtt_stop(void)
{
    is_started = 0;
    return paho_mqtt_stop(&client);
}

/* 发布消息 */
static int mqtt_publish(char *topic, char *msg)
{
    if (is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    paho_mqtt_publish(&client, QOS1, topic, msg);

    return 0;
}

/* 新订阅主题的回调函数 */
static void mqtt_new_sub_callback(MQTTClient *client, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt new subscribe callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

/* 订阅主题 */
static int mqtt_subscribe(char *topic)
{
    if (is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_subscribe(&client, QOS1, topic, mqtt_new_sub_callback);
}

/* 取消订阅主题 */
static int mqtt_unsubscribe(char *topic)
{
    if (is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_unsubscribe(&client, topic);
}

/* MQTT上传线程入口函数 */
void mqtt_upload_entry(void *parameter)
{
    char msg[10] = "msg";

    /* 启动MQTT */
    mqtt_start();
    /* 订阅主题"/mqtt/test_new" */
    mqtt_subscribe("/mqtt/test_new");

    /* 间隔10s，发布主题 */
    for(int i=0; i<5; i++)
    {
        rt_snprintf(msg,sizeof(msg), "msg %d",i);
        mqtt_publish("/mqtt/test_new",msg);
        mqtt_publish("/mqtt/test",msg);
        rt_thread_mdelay(10000);
        rt_kprintf("upload %d\n",i);
    }

    /* 取消订阅主题/mqtt/test_new */
    mqtt_unsubscribe("/mqtt/test_new");
    /* 停止MQTT */
    mqtt_stop();
}





