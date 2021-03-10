#include "CppUTestExt/MockSupport.h"

extern "C"
{
#include "core_mqtt.h"
}

MQTTStatus_t MQTT_Init(MQTTContext_t *pContext,
                       const TransportInterface_t *pTransportInterface,
                       MQTTGetCurrentTimeFunc_t getTimeFunction,
                       MQTTEventCallback_t userCallback,
                       const MQTTFixedBuffer_t *pNetworkBuffer)
{
    mock()
        .actualCall("init");

    return MQTTSuccess;
}

MQTTStatus_t MQTT_Connect(MQTTContext_t *pContext,
                          const MQTTConnectInfo_t *pConnectInfo,
                          const MQTTPublishInfo_t *pWillInfo,
                          uint32_t timeoutMs,
                          bool *pSessionPresent)
{
    return MQTTSuccess;
}

MQTTStatus_t MQTT_Subscribe(MQTTContext_t *pContext,
                            const MQTTSubscribeInfo_t *pSubscriptionList,
                            size_t subscriptionCount,
                            uint16_t packetId)
{
    return MQTTSuccess;
}

MQTTStatus_t MQTT_Publish(MQTTContext_t *pContext,
                          const MQTTPublishInfo_t *pPublishInfo,
                          uint16_t packetId)
{
    return MQTTSuccess;
}

MQTTStatus_t MQTT_Disconnect(MQTTContext_t *pContext)
{
    return MQTTSuccess;
}

uint16_t MQTT_GetPacketId(MQTTContext_t *pContext)
{
    return 0;
}

MQTTStatus_t MQTT_ProcessLoop(MQTTContext_t *pContext,
                              uint32_t timeoutMs)
{
    return MQTTSuccess;
}

MQTTStatus_t MQTT_Unsubscribe(MQTTContext_t *pContext,
                              const MQTTSubscribeInfo_t *pSubscriptionList,
                              size_t subscriptionCount,
                              uint16_t packetId)
{
    return MQTTSuccess;
}