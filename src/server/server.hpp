#pragma once

#include "types.hpp"
#include "physics.hpp"
#include "serialize.hpp"
class Session;

#include <memory>
#include <asio.hpp>

using asio::ip::tcp;

class Server : public std::enable_shared_from_this<Server> {
public:
    explicit Server(asio::io_context& io_context, short port);

    void start() { do_accept(); }

    void broadcast_snapshot(const Snapshot& snapshot);

    void remove_session(std::shared_ptr<Session> session);

private:
    void do_accept();
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::vector<std::shared_ptr<Session>> sessions_;
};