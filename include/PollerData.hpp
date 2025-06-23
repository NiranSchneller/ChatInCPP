#ifndef POLLAR_DATA_H
#define POLLAR_DATA_H
#include "UserEntity.hpp"
namespace Chat
{
    using fd_t = int;
    template <size_t MAX_MESSAGES_PER_USER, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
    struct PollerData
    {
        UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> *userEntity;
        fd_t fileDescriptor;
    };

} // namespace Chat

#endif