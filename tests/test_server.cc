#include <gtest/gtest.h>
#include "Server.hpp"
#include "StatusCodes.h"

static constexpr size_t MAX_MESSAGE_SIZE = 100;
static constexpr uint16_t SERVER_PORT = 6000;
static constexpr size_t LISTENING_QUEUE_AMOUNT = 1;

TEST(TestServer, InitializeDestroy)
{
    Chat::Server<MAX_MESSAGE_SIZE> server;
    ASSERT_EQ(server.Initialize(SERVER_PORT, LISTENING_QUEUE_AMOUNT), Chat::StatusCode::SUCCESS);
    ASSERT_EQ(server.Destroy(), Chat::StatusCode::SUCCESS);
}

TEST(TestServer, InitializeTwice)
{
    Chat::Server<MAX_MESSAGE_SIZE> server;
    ASSERT_EQ(server.Initialize(SERVER_PORT, LISTENING_QUEUE_AMOUNT), Chat::StatusCode::SUCCESS);
    ASSERT_EQ(server.Initialize(SERVER_PORT, LISTENING_QUEUE_AMOUNT), Chat::StatusCode::ALREADY_INITIALIZED);

    ASSERT_EQ(server.Destroy(), Chat::StatusCode::SUCCESS);
}

TEST(TestServer, DestroyTwice)
{
    Chat::Server<MAX_MESSAGE_SIZE> server;
    ASSERT_EQ(server.Initialize(SERVER_PORT, LISTENING_QUEUE_AMOUNT), Chat::StatusCode::SUCCESS);

    ASSERT_EQ(server.Destroy(), Chat::StatusCode::SUCCESS);
    ASSERT_EQ(server.Destroy(), Chat::StatusCode::UNINITIALIZED);
}

TEST(TestServer, OperateWithoutInitialization)
{
    Chat::Server<MAX_MESSAGE_SIZE> server;

    ASSERT_EQ(server.Destroy(), Chat::StatusCode::UNINITIALIZED);
    ASSERT_EQ(server.HandleClient(), Chat::StatusCode::UNINITIALIZED);
}
