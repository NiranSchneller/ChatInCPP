#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <stddef.h>

namespace Chat
{
    template <size_t MAX_MESSAGE_SIZE>
    struct Message
    {
        uint8_t message[MAX_MESSAGE_SIZE];
        size_t messageSize;
    };

} // namespace Chat

#endif