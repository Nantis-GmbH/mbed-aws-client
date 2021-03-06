#ifndef AWS_CLIENT_H
#define AWS_CLIENT_H

#include "mbed.h"

extern "C"
{
#include "core_mqtt.h"
}

// Undef trace group from AWS SDK logging
#undef TRACE_GROUP

#define SHADOW_TOPIC_MAX_LENGTH 256
#define SHADOW_GET_RESPONSE_MAX_SIZE 256

/**
 * @brief Network context declaration.
 * 
 * Required by coreMQTT.
 * 
 */
struct NetworkContext
{
    /**
     * @brief TLS socket underlying the MQTT connection.
     * 
     */
    TLSSocket socket;
};

/**
 * @brief AWS IoT client.
 * 
 * Currently not thread safe as TLSSocket is not protected.
 * 
 */
class AWSClient
{
private:
    /**
     * @brief Construct a new AWSClient object
     */
    AWSClient() {}

    /**
     * @brief MQTT context to store after initialization.
     */
    MQTTContext_t mqttContext;

    /**
     * @brief Network context provided to the SDK.
     */
    NetworkContext_t networkContext;

    /**
     * @brief Network buffer provided to the SDK.
     * 
     */
    uint8_t mqttBuffer[MBED_CONF_AWS_CLIENT_BUFFER_SIZE];

    /**
     * @brief Storage for the parsed root certificate.
     * 
     */
    mbedtls_x509_crt rootCA;

    /**
     * @brief Response received flag.
     * Used to process all responses at one call until no more remains.
     */
    bool isResponseReceived;

    /**
     * @brief Application callback for subscription events.
     * 
     */
    Callback<void(string, string)> subCallback;

    /**
     * @brief Static callback to provide to the SDK.
     * Calls the application callback when a response is received
     * for one of our subscriptions.
     * Interface defined by SDK.
     */
    static void eventCallbackStatic(MQTTContext_t *pMqttContext,
                                    MQTTPacketInfo_t *pPacketInfo,
                                    MQTTDeserializedInfo_t *pDeserializedInfo);

    /**
     * @brief Thing name.
     * 
     * Should be the same as the MQTT client ID.
     * 
     * TODO check memory safety of this (string vs char array)
     */
    string thingName;

    bool shadowGetAccepted;

    bool shadowUpdateAccepted;

    /**
     * @brief Buffer for the shadow get response.
     * 
     * Gets written by the getShadowDocument() function.
     * 
     */
    char shadowGetResponse[SHADOW_GET_RESPONSE_MAX_SIZE];

    /**
     * @brief Length of the shadow get response.
     * 
     */
    size_t shadowGetResponseLength;

public:
    /**
     * @brief TLS credentials container.
     *
     */
    struct TLSCredentials_t
    {
        const char *clientCrt;
        size_t clientCrtLen;
        const char *clientKey;
        size_t clientKeyLen;
        const char *rootCrtMain;
        size_t rootCrtMainLen;
        const char *rootCrtBackup;
        size_t rootCrtBackupLen;
    };

    /**
     * @brief Get the singleton instance.
     * 
     * @return AWSClient instance
     */
    static AWSClient &getInstance()
    {
        static AWSClient instance;
        return instance;
    }

    /**
     * @brief Delete copy constructor and assignment.
     * For singleton pattern.
     */
    AWSClient(AWSClient const &) = delete;
    void operator=(AWSClient const &) = delete;

    /**
     * @brief Initialize the client.
     * 
     * Sets the subscription callback.
     * Initializes the SDK.
     * Parses the root CAs and stores them.
     * 
     * @param subCallback Subscription callback (topic, payload).
     * @param creds Credentials containing the root CA.
     * @return MBED_SUCCESS on success.  
     */
    int init(Callback<void(string, string)> subCallback,
             const TLSCredentials_t &creds);

    /**
     * @brief Establish the MQTT connection.
     * 
     * Creates a new TLSSocket.
     * Sets endpoint, credentials and timeout of the TLS socket.
     * Opens the socket.
     * Gets the IP address of the AWS endpoint.
     * Connects the socket to the endpoint.
     * Establishes the MQTT connection.
     * 
     * @param net The underlying network interface for the socket.
     * @param creds Credentials for TLS.
     * @param hostname AWS IoT endpoint.
     * @param clientID MQTT client ID. Should be same as the thing name.
     * @return MBED_SUCCESS on success. 
     */
    int connect(NetworkInterface *net,
                const TLSCredentials_t &creds,
                const string hostname,
                const string clientID);

    /**
     * @brief Check if the MQTT client is connected.
     * 
     * Returns the connectStatus member of the MQTT context.
     * 
     * @return true if connected, false if not connected
     */
    bool isConnected();

    /**
     * @brief Disonnect from the MQTT server.
     * 
     * Closes the TLS socket.
     * 
     * @return MBED_SUCCESS on success.
     */
    int disconnect();

    /**
     * @brief Returns the MQTT context.
     * 
     * @return MQTT context.
     */
    MQTTContext_t getMQTTContext();

    /**
     * @brief Subscribes to a topic filter.
     * 
     * TODO char array variant would be more efficient in some cases
     * 
     * @param topicFilter Topic filter.
     * @param qos QoS.
     * @return MBED_SUCCESS on success. 
     */
    int subscribe(const string topicFilter, const MQTTQoS qos = MQTTQoS0);

    /**
     * @brief Unsubscribes from a topic filter.
     * 
     * @param topicFilter Topic filter.
     * @return MBED_SUCCESS on success.  
     */
    int unsubscribe(const string topicFilter);

    /**
     * @brief Publishes to a topic.
     * 
     * @param topic Topic to publish to.
     * @param msg Message to publish.
     * @param qos QoS.
     * @return MBED_SUCCESS on success.   
     */
    int publish(const string topic, const string msg, const MQTTQoS qos = MQTTQoS0);

    /**
     * @brief Processes all of the pending incoming messages.
     * 
     * Also handles keepalive.
     * This must be called periodically by the application.
     * Triggers application callback for received subscriptions.
     * 
     * @return MBED_SUCCESS on success. 
     */
    int processResponses();

    /**
     * @brief Retrieves the device shadow document.
     * 
     * The retrieved document is written to the shadowGetResponse member.
     * 
     * @return MBED_SUCCESS on success. 
     */
    int getShadowDocument();

    /**
     * @brief Extracts the desired value of the given key from the retrieved device shadow document.
     * 
     * getShadowDocument() should be called before this.
     * 
     * Tip: use stoi() to convert the value to integer in case an integer is expected.
     * 
     * @param key Key of value to retrieve.
     * @param value Desired value extracted from the shadow.
     * @return MBED_SUCCESS on success. 
     */
    int getShadowDesiredValue(string key, string &value);

    /**
     * @brief Publishes an update to the device shadow.
     * 
     * @param updateDocument Update document to be published.
     * @return MBED_SUCCESS on success.
     */
    int updateShadowDocument(string updateDocument);

    /**
     * @brief Publishes the reported value of the given key to the device shadow.
     * 
     * Constructs the update document and calls updateShadowDocument().
     * 
     * @param key Key of the value to publish.
     * @param value String to publish. Quotation marks will be added automatically.
     * @return MBED_SUCCESS on success. 
     */
    int publishShadowReportedValue(string key, string value);

    /**
     * @brief Publishes the reported value of the given key to the device shadow.
     * 
     * Constructs the update document and calls updateShadowDocument().
     * 
     * @param key Key of value to publish.
     * @param value Integer value to publish.
     * @return MBED_SUCCESS on success. 
     */
    int publishShadowReportedValue(string key, int value);
};

#endif /* AWS_CLIENT_H */