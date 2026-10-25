#include "server.hpp"
#include "session.hpp"

#include <algorithm>
#include <iostream>

Server::Server(asio::io_context& io_context, short port) 
: acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), socket_(io_context) {}

void Server::do_accept() {
    acceptor_.async_accept( socket_,
    [this, self=shared_from_this()](std::error_code ec) {
        if (!ec) {
            tcp::endpoint remote{socket_.remote_endpoint()};
            std::cout << "Client connected from "
            << remote.address().to_string() << ":"
            << remote.port() << "\n";

            auto new_session{std::make_shared<Session>(std::move(socket_), self)};
            sessions_.push_back(new_session);

            new_session->start();
        }
        do_accept();
    });
}

void Server::remove_session(std::shared_ptr<Session> session) {
    auto it{std::remove(sessions_.begin(), sessions_.end(), session)};

    sessions_.erase(it, sessions_.end());

    std::cout << "Client disconected. Sessions left : " << sessions_.size() << "\n";
}

void Server::broadcast_snapshot(const Snapshot& snapshot) {
    auto payload{Marshaling::serialize(snapshot)};

    for (auto& session : sessions_) {
        if (session) session->send_raw(payload);
    }
}
    