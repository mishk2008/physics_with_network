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
    Session(tcp::socket socket, std::weak_ptr<Server> server_weak) 
    : socket_(std::move(socket)), server_weak_(std::move(server_weak)) {}

    void start() {}

    void send_raw(const std::vector<uint8_t>& payload);

private:
    tcp::socket socket_;
    std::weak_ptr<Server> server_weak_;
};