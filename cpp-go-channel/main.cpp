#include <iostream>
#include <string>
#include <future>

#include "Channel.h"


int main() {
    std::cout << "Channel created.\n";
    Channel<std::string> chan;

    auto future = std::async(std::launch::async,
            [&chan](){
        std::cout << "Async about to send ping\n";
        chan.send("ping");
        std::cout << "Async ping sent, sending pong next\n";
        chan.send("pong");
        std::cout << "Async pong sent\n";
    });

    std::cout << "Main thread about to call receive on channel:\n";
    auto val = chan.receive_blocking();
    std::cout << "Got:" << val << std::endl;

    std::cout << "Main thread about to call receive on channel:\n";
    val = chan.receive_blocking();
    std::cout << "Got:" << val << std::endl;

    future.get();
    return 0;
}
