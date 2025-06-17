#include <gtest/gtest.h>
#include "Client.hpp"
#include "ErrorCodes.h"

static constexpr size_t MAX_MESSAGE_SIZE = 100;
static constexpr uint16_t SERVER_PORT = 6000;
static constexpr size_t LISTENING_QUEUE_AMOUNT = 1;

TEST(TestClient, OperateWithoutInitialization)
{
    Chat::Client<MAX_MESSAGE_SIZE> client;

    ASSERT_EQ(client.CommunicateWithServer().error(), Chat::ErrorCode::UNINITIALIZED);
}
