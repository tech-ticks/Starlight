// Partially sourced from https://github.com/skyline-dev/skyline/blob/master/source/skyline/logger/TcpLogger.cpp
#pragma once

#define LOGGING_PORT 8989

#include <cstddef>
#include <cstdint>

typedef __uint8_t __sa_family_t;
typedef __uint32_t __socklen_t;

struct in_addr {
	__uint32_t s_addr;
};

struct sockaddr_in {
	__uint8_t      sin_len;
	__uint8_t      sin_family;
	__uint16_t     sin_port;
	struct in_addr sin_addr;
	char           sin_zero[8];
};

#include <cstring>
#include <cstdarg>
#include <alloca.h>

#include "nn/os.hpp"
#include "nn/socket.h"
#include "rtdx/mem.h"
#include "rtdx/misc.h"

#ifdef ENABLE_LOGGING
    #define LOG_INIT(...) ::hb::logging::initLogging(__VA_ARGS__)
    #define LOG(...) ::hb::logging::log(__VA_ARGS__)
    #define LOGF(...) ::hb::logging::logf(__VA_ARGS__)
#else
    #define LOG_INIT(...)
    #define LOG(...)
    #define LOGF(...)
#endif

namespace hb::logging {

    struct Message {
        char* message = nullptr;
        Message* next = nullptr;
    };

    static Message* messageHead = nullptr;
    static nn::os::ThreadType loggerThread;

    static void ThreadMain(void* context) {
        struct sockaddr_in serverAddr;
        int socket = nn::socket::Socket(AF_INET, SOCK_STREAM, 0);
        if (socket & 0x80000000) return;

        int flags = 1;
        nn::socket::SetSockOpt(socket, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = 0x00000000;
        serverAddr.sin_port = nn::socket::InetHtons(LOGGING_PORT);

        int rval = nn::socket::Bind(socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (rval < 0) return;

        rval = nn::socket::Listen(socket, 1);
        if (rval < 0) return;

        u32 addrLen;
        socket = nn::socket::Accept(socket, (struct sockaddr*)&serverAddr, &addrLen);

        while (true) {
            // Probably not really thread-safe, sorry if this crashes
            while (messageHead != nullptr) {
                nn::socket::Send(socket, messageHead->message, strlen(messageHead->message), 0);
                rtdx_free(messageHead->message);
                Message* next = messageHead->next;
                rtdx_free(messageHead);
                messageHead = next;
            }

            nn::os::YieldThread();
            nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(100000000));
        }
    }

    void initLogging() {
        const size_t stackSize = 0x3000;
        void* threadStack = rtdx_memalign(0x1000, stackSize);

        nn::os::CreateThread(&loggerThread, ThreadMain, nullptr, threadStack, stackSize, 16, 0);
        nn::os::StartThread(&loggerThread);
    }

    void log(const char* str) {
        auto message = (Message*) rtdx_malloc(sizeof(Message));
        message->message = (char*) rtdx_malloc(strlen(str) + 1);
        message->next = nullptr;
        rtdx_memcpy(message->message, str, strlen(str) + 1);

        // Add to the end of the queue
        if (messageHead == nullptr) {
            messageHead = message;
        } else {
            auto messageTail = messageHead;
            while (messageTail->next != nullptr) {
                messageTail = messageTail->next;
            }
            messageTail->next = message;
        }
    }

    void logf(const char* str, ...) {
        // Log with format string
        va_list args;
        va_start(args, str);

        // https://stackoverflow.com/a/3923207
        int size = rtdx_vsnprintf(nullptr, 0, str, args) + 1;
        char* buffer = (char*) alloca(size); // Allocating on the stack should be enough
        rtdx_vsnprintf(buffer, size, str, args);

        va_end(args);

        log(buffer);
    }

}
