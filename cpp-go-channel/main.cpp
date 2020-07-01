#include <iostream>
#include <string>
#include <future>
#include <chrono>
#include <thread>
#include <sstream>

#include "Channel.h"
#define MAIN_VERSION2

#define THREADSAFE(MESSAGE) \
        ( static_cast<std::ostringstream&>(std::ostringstream().flush() << MESSAGE).str())

#ifdef MAIN_VERSION1
int main() {
    std::cout << "Channel created.\n";
    Channel<std::string> chan;

    auto future = std::async(std::launch::async,
            [&chan](){
                {
                    std::cout << "Async about to send ping\n";
                    chan.send("ping");
                }
        std::cout << "Async ping sent, sending pong next\n";
        chan.send("pong");
        std::cout << "Async pong sent\n";
    });

    std::cout << "Main thread about to call receive on channel:\n";
    auto val = chan.receive_blocking();
    std::cout << THREADSAFE("Got:" << val << std::endl);

    std::cout << "Main thread about to call receive on channel:\n";
    auto val = chan.receive_blocking();
    std::cout << THREADSAFE("Got:" << val << std::endl);

    future.get();
    return 0;
}
#endif

#ifdef MAIN_VERSION2

struct timer {
    timer(const std::string& label)
    {
        m_label = label;
        m_start = std::chrono::high_resolution_clock::now();
    }
    ~timer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>( end - m_start ).count();
        std::cout << THREADSAFE(m_label << " took " << duration << " s.\n");
    }

    std::chrono::time_point<std::chrono::steady_clock> m_start;
    std::string m_label;
};

int main() {
    using namespace std::chrono_literals;

    std::cout << "Channel created.\n";
    Channel<std::string> chan;

    auto future = std::async(std::launch::async,
            [&chan](){
                {
                    auto t = timer("send1");
                    std::cout << "Async about to send ping\n";
                    chan.send("ping");
                }
        std::cout << "Async ping sent, sending pong next\n";
        chan.send("pong");
        std::cout << "Async pong sent\n";
    });

    {
        auto t2 = timer("receive1");
        std::this_thread::sleep_for(2s);
        std::cout << "Main thread about to call receive on channel:\n";
        auto val = chan.receive_blocking();
        std::cout << THREADSAFE("Got:" << val << std::endl);
    }

    std::cout << "Main thread about to call receive on channel:\n";
    auto val = chan.receive_blocking();
    std::cout << THREADSAFE("Got:" << val << std::endl);

    future.get();
    return 0;
}
#endif
