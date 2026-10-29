#include "physics.hpp"
#include "server.hpp"

#include <iostream>
#include <asio.hpp>

constexpr int host{12345};
constexpr float interval{0.05f};

int main() {
    asio::io_context io;
    auto server{std::make_shared<Server>(io, host)};
    server->start();

    World world(10);
    uint32_t tick{0};
    asio::steady_timer timer(io);

    std::function<void()> tick_handler=[&]() {
        world.update(interval);

        //world.print();

        auto snap{world.make_snapshot(tick)};

        server->broadcast_snapshot(snap);

        timer.expires_after(std::chrono::milliseconds(50));
        timer.async_wait([&](std::error_code ec) {
            if (!ec) tick_handler();
        });

    };
    
    timer.expires_after(std::chrono::milliseconds(50));
    timer.async_wait([&](std::error_code ec) {
        if (!ec) tick_handler();
    });

    io.run();
    return 0;
}