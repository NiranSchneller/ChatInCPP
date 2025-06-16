#include <gtest/gtest.h>
#include "Server.hpp"
#include "StatusCodes.h"

TEST(TestServer, InitializeDestroy)
{
    Chat::Server<100> server;
    ASSERT_EQ(server.Initialize(6000, 1), Chat::StatusCode::SUCCESS);
    ASSERT_EQ(server.Destroy(), Chat::StatusCode::SUCCESS);
}

TEST(TestServer, InitializeTwice)
{
    Chat::Server<100> server;
    ASSERT_EQ(server.Initialize(6000, 1), Chat::StatusCode::SUCCESS);
    ASSERT_EQ(server.Initialize(6000, 1), Chat::StatusCode::ALREADY_INITIALIZED);

    ASSERT_EQ(server.Destroy(), Chat::StatusCode::SUCCESS);
}

TEST(TestServer, DestroyTwice)
{
    Chat::Server<100> server;
    ASSERT_EQ(server.Initialize(6000, 1), Chat::StatusCode::SUCCESS);

    ASSERT_EQ(server.Destroy(), Chat::StatusCode::SUCCESS);
    ASSERT_EQ(server.Destroy(), Chat::StatusCode::UNINITIALIZED);
}

TEST(TestServer, OperateWithoutInitialization)
{
    Chat::Server<100> server;

    ASSERT_EQ(server.Destroy(), Chat::StatusCode::UNINITIALIZED);
    ASSERT_EQ(server.HandleClient(), Chat::StatusCode::UNINITIALIZED);
}
