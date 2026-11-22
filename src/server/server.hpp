#pragma once

#include "types.hpp"
#include "physics.hpp"
#include "serialize.hpp"
#include "itransport.hpp"
class Session;

#include <memory>
#include <deque>
#include <asio.hpp>

using asio::ip::tcp;

class TcpServer : public IServerTransport, public std::enable_shared_from_this<TcpServer> {
public:
    explicit TcpServer(asio::io_context& io_context, short port);

    void start() { do_accept(); }

    void broadcast_snapshot(const Snapshot& snapshot);

    void remove_session(std::shared_ptr<Session> session);

private:
    void do_accept();
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::vector<std::shared_ptr<Session>> sessions_;
};