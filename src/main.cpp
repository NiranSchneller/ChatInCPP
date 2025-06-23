#include <stdio.h>

#include "Server.hpp"
#include "ArgumentParser.h"
#include "Client.hpp"
#include "MessageQueue.hpp"
static constexpr size_t MAX_USERS = 10;
static constexpr size_t MAX_MESSAGES = 10;
static constexpr size_t MAX_MESSAGE_SIZE = 100;
static constexpr size_t MAX_USERNAME_LENGTH = 20;
static constexpr size_t LISTENING_QUEUE_AMOUNT = 1;

void server(uint16_t port)
{
    printf("Starting server on port %d!\n", port);
    Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> server;
    if (!server.Initialize(port, LISTENING_QUEUE_AMOUNT).has_value())
    {
        printf("Server could not be initialized!\n");
        return;
    }

    server.Run();
}

void client(uint32_t address, uint16_t port)
{
    printf("Will connect to server with address %d and port %d\n", address, port);
    Chat::Client<MAX_MESSAGE_SIZE> client;

    client.Initialize(address, port);
    client.CommunicateWithServer();
}

int main(int argc, char **argv)
{

    // Defaults to server if an error occurred

    std::expected<Chat::ProgramType, Chat::ErrorCode> programType = Chat::ArgumentParser::ParseProgramType(argc, argv);
    if (!programType.has_value())
    {
        printf("Could not parse program type!\n");
        return 1;
    }

    if (programType == Chat::ProgramType::SERVER)
    {
        std::expected<uint16_t, Chat::ErrorCode> parsedPort = Chat::ArgumentParser::ParsePort(argc, argv);
        if (!parsedPort.has_value())
        {
            printf("Could not parse server port!\n");
            return 1;
        }
        server(parsedPort.value());
    }
    else
    {
        std::expected<Chat::Address, Chat::ErrorCode> parsedArgs = Chat::ArgumentParser::ParseAddressPort(argc, argv);
        if (!parsedArgs.has_value())
        {
            printf("Could not parse address and port!\n");
            return 1;
        }
        client(parsedArgs.value().ip, parsedArgs.value().port);
    }

    return 0;
}
