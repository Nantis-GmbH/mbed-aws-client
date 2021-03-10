# Fetch CppUTest TODO put into unit tests common dir
include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        latest-passing-build # or use release tag, eg. v3.8
)
set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
FetchContent_MakeAvailable(CppUTest)

# Test executable
add_executable(AWSClientTest 
    # SUT sources
    ../AWSClient.cpp 
    # Test sources
    AWSClientTest.cpp
    creds.c
    # Mocks
    MQTT_mock.cpp
    # Stubs
    ${CUSTOM_STUBS_DIR}/InternetSocket_stub.cpp
    ${CUSTOM_STUBS_DIR}/TCPSocket_stub.cpp
    ${CUSTOM_STUBS_DIR}/TLSSocket_stub.cpp
    ${CUSTOM_STUBS_DIR}/TLSSocketWrapper_stub.cpp
    ${MBED_STUBS_DIR}/Kernel_stub.cpp
    ${MBED_STUBS_DIR}/mbed_assert_stub.cpp
    ${MBED_STUBS_DIR}/EventFlags_stub.cpp
    ${MBED_STUBS_DIR}/Mutex_stub.cpp
    ${MBED_STUBS_DIR}/SocketAddress_stub.cpp
)

target_include_directories(AWSClientTest PRIVATE 
    # SUT & test includes
    ..
    # Mock includes
    ${MQTT_INCLUDE_PUBLIC_DIRS}
)

target_compile_definitions(AWSClientTest PRIVATE 
    # Mock
    MQTT_DO_NOT_USE_CUSTOM_CONFIG=1
    # MbedTLS
    MBEDTLS_TEST_NULL_ENTROPY 
    MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
    # Mbed OS
    MBED_CONF_RTOS_PRESENT=1
    # SUT defines
    MBED_CONF_AWS_CLIENT_BUFFER_SIZE=1024
    MBED_CONF_AWS_CLIENT_SHADOW_GET_RESPONSE_MAX_SIZE=1024
    MBED_CONF_AWS_CLIENT_SOCKET_TIMEOUT=5s
    MBED_CONF_AWS_CLIENT_PORT=8883
    MBED_CONF_AWS_CLIENT_CLEAN_SESSION=1
    MBED_CONF_AWS_CLIENT_KEEPALIVE=60s
    MBED_CONF_AWS_CLIENT_SHADOW_TOPIC_MAX_SIZE=256
)

target_link_libraries(AWSClientTest PRIVATE 
    # CppUTest
    CppUTest 
    # CppUMock
    CppUTestExt
    # SUT dependencies
    shadow 
    coreJSON 
    mbed-core
    mbed-mbedtls
)