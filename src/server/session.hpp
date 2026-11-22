#pragma once

#include "types.hpp"
#include "physics.hpp"
#include "serialize.hpp"
#include "server.hpp"

#include <memory>
#include <functional>
#include <asio.hpp>

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::weak_ptr<IServerTransport> server_weak);

    void start() {}

    void send_raw(const std::vector<uint8_t>& payload);

private:    
    void do_write();

    tcp::socket socket_;
    std::weak_ptr<IServerTransport> server_weak_;
    
    std::deque<std::shared_ptr<std::vector<uint8_t>>> write_queue_;
    bool writing_{false};
};