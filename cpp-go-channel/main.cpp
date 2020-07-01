#include <iostream>
#include <string>
#include <future>
#include <chrono>
#include <thread>
#include <sstream>

#define MAIN_VERSION3

#ifdef MAIN_VERSION3
#include "MultiChannel.h"
#else
#include "Channel.h"
#endif
/* Common utility functions  */
#define THREADSAFE(MESSAGE) \
        ( static_cast<std::ostringstream&>(std::ostringstream().flush() << MESSAGE).str())

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


// Version 1 - basic
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

// verison 2 - with timing and 2 sends
#ifdef MAIN_VERSION2
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


#ifdef MAIN_VERSION3

int main() {
    Channel<std::string> chan;
    std::cout << "Channel created.\n";
    const auto capacity = 5;

    // kick off thread to send
    auto future = std::async(std::launch::async,
                             [&chan, capacity](){
                                 {
                                     auto t = timer("send thread");
                                     for (size_t i = 0; i < capacity; ++i)
                                     {
                                         std::cout << THREADSAFE("send thread: sending ping " << i << "\n");
                                         if (i == 4)
                                         {
                                             std::this_thread::sleep_for(std::chrono::seconds(2));
                                         }
                                         chan.send(THREADSAFE("ping "<<i));
                                     }
                                 }
                             });

    // receive
    std::string val;
    for (size_t i = 0; i < capacity; ++i)
    {
        std::cout << "main thread: calling receive:\n";
        val = chan.receive_blocking();
        std::cout << THREADSAFE("main thread: received " << val << std::endl);
    }

    future.get();
    return 0;
}
#endif