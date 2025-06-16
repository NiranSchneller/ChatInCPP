#include <stdio.h>
#include "Server.hpp"
int main(void)
{
    Chat::Server<100> server;
    server.Initialize(6000, 1);
    while (true)
    {
        server.HandleClient();
        printf("Moving on to next client!\n");
    }
    server.Destroy();
    return 0;
}
