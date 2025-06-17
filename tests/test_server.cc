#include <gtest/gtest.h>
#include "Server.hpp"
#include "ErrorCodes.h"

static constexpr size_t MAX_MESSAGE_SIZE = 100;
static constexpr uint16_t SERVER_PORT = 6000;
static constexpr size_t LISTENING_QUEUE_AMOUNT = 1;

TEST(TestServer, InitializeDestroy)
{
    Chat::Server<MAX_MESSAGE_SIZE> server;
    ASSERT_TRUE(server.Initialize(SERVER_PORT, LISTENING_QUEUE_AMOUNT).has_value());
}

TEST(TestServer, InitializeTwice)
{
    Chat::Server<MAX_MESSAGE_SIZE> server;
    ASSERT_TRUE(server.Initialize(SERVER_PORT, LISTENING_QUEUE_AMOUNT).has_value());
    ASSERT_EQ(server.Initialize(SERVER_PORT, LISTENING_QUEUE_AMOUNT).error(), Chat::ErrorCode::ALREADY_INITIALIZED);
}

TEST(TestServer, OperateWithoutInitialization)
{
    Chat::Server<MAX_MESSAGE_SIZE> server;

    ASSERT_EQ(server.AcceptClient().error(), Chat::ErrorCode::UNINITIALIZED);
    ASSERT_EQ(server.HandleClient().error(), Chat::ErrorCode::UNINITIALIZED);
}
