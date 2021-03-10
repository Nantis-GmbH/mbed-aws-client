#include "AWSClient.h"

// CppUTest must be included after mbed stuff due to weird reasons
#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"

extern const char root_cert[];
extern const unsigned root_cert_len;

TEST_GROUP(AWSClient){
    // TODO fix formatting
    void teardown(){
        mock().clear();
}
}
;

TEST(AWSClient, Init)
{
    mock()
        .expectOneCall("init");

    auto &awsClient = AWSClient::getInstance();

    AWSClient::TLSCredentials_t creds = {
        NULL,
        0,
        NULL,
        0,
        root_cert,
        root_cert_len,
        NULL,
        0};

    auto ret = awsClient.init(NULL, creds);

    CHECK_EQUAL(0, ret);
}

int main(int ac, char **av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}