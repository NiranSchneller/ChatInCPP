#ifndef POLLER_H
#define POLLER_H

#include <expected>
#include <stddef.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "PollerData.hpp"
#include "ErrorCodes.h"

static constexpr int FAILED_FD = -1;

namespace Chat
{
    using fd_t = int;

    template <size_t MAX_USERS>
    class Poller
    {
    public:
        Poller();
        ~Poller();

        std::expected<void, ErrorCode> Initialize();

        /**
         * @brief Adds an FD to the poller
         * @param data The data to put as the event
         *
         * @returns Error Code
         */
        std::expected<void, ErrorCode> AddToPoll(struct PollerData *data);

        /**
         * @brief Removes specified file descriptor from Polling
         *
         * @param fileDescriptor File Descriptor to remove
         *
         * @returns Error Code
         */
        std::expected<void, ErrorCode> RemoveFromPoll(fd_t fileDescriptor);

        /**
         * @brief Returns all active FDs to o_dataArray
         *
         * @param dataArraySize The size of the array
         * @param o_dataArray The output array (dataArraySize >= MAX_USERS)
         *
         * @returns amount of PollerData's in the array or error code.
         */
        std::expected<size_t, ErrorCode> GetActive(struct PollerData **o_dataArray, size_t dataArraySize); // Returns amount of PollerData in arr

    private:
        fd_t m_interestListFD = 0;
        bool m_init = false;
    };

    template <size_t MAX_USERS>
    Poller<MAX_USERS>::Poller() : m_interestListFD(0), m_init(false) {}

    template <size_t MAX_USERS>
    std::expected<void, ErrorCode> Poller<MAX_USERS>::Initialize()
    {
        if (m_init)
        {
            return std::unexpected(ErrorCode::ALREADY_INITIALIZED);
        }

        m_interestListFD = epoll_create(MAX_USERS);
        if (m_interestListFD == FAILED_FD)
        {
            return std::unexpected(ErrorCode::EPOLL_ERROR);
        }

        return {};
    }

    template <size_t MAX_USERS>
    std::expected<void, ErrorCode> Poller<MAX_USERS>::AddToPoll(struct PollerData *data)
    {
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET; // Edge-triggered for readability
        event.data.u64 = data;

        if (epoll_ctl(m_interestListFD, EPOLL_CTL_ADD, data->fileDescriptor, &event) == FAILED_FD)
        {
            return std::unexpected(ErrorCode::EPOLL_ERROR);
        }

        return {};
    }

    template <size_t MAX_USERS>
    std::expected<void, ErrorCode> Poller<MAX_USERS>::RemoveFromPoll(fd_t fileDescriptor)
    {
        if (epoll_ctl(m_interestListFD, EPOLL_CTL_DEL, fileDescriptor, nullptr) == FAILED_FD)
        {
            return std::unexpected(ErrorCode::EPOLL_ERROR);
        }

        return {};
    }

    template <size_t MAX_USERS>
    std::expected<size_t, ErrorCode> Poller<MAX_USERS>::GetActive(struct PollerData **o_dataArray, size_t dataArraySize)
    {
        if (dataArraySize < MAX_USERS)
        {
            return std::unexpected(ErrorCode::INVALID_ARGUMENT);
        }

        struct epoll_event events[MAX_USERS];
        int amount = epoll_wait(m_interestListFD, &events, MAX_USERS, -1);
        if (amount == FAILED_FD)
        {
            return std::unexpected(ErrorCode::EPOLL_ERROR);
        }

        for (size_t i = 0; i < amount; i++)
        {
            o_dataArray[i] = events[i].data.u64;
        }

        return amount;
    }

    template <size_t MAX_USERS>
    Poller<MAX_USERS>::~Poller()
    {
        m_init = false;
        close(m_interestListFD);
        m_interestListFD = 0;
    }
} // namespace Chat

#endif