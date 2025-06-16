#include <stdio.h>
#include "Server.hpp"

static constexpr size_t MAX_MESSAGE_SIZE = 100;
static constexpr uint16_t SERVER_PORT = 6000;
static constexpr size_t LISTENING_QUEUE_AMOUNT = 1;

int main(void)
{
    Chat::Server<MAX_MESSAGE_SIZE> server;
    server.Initialize(SERVER_PORT, LISTENING_QUEUE_AMOUNT);
    while (true)
    {
        server.HandleClient();
        printf("Moving on to next client!\n");
    }
    server.Destroy();
    return 0;
}
