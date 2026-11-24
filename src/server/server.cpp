#include "server.hpp"
#include "session.hpp"

#include <algorithm>
#include <iostream>

TcpServer::TcpServer(asio::io_context& io_context, short port) 
: acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), socket_(io_context) {
    std::cout   << "Server is running  on: " << acceptor_.local_endpoint().address().to_string()
                << ":" << acceptor_.local_endpoint().port() << '\n';
}

void TcpServer::do_accept() {
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

void TcpServer::remove_session(std::shared_ptr<Session> session) {
    auto it{std::remove(sessions_.begin(), sessions_.end(), session)};

    sessions_.erase(it, sessions_.end());

    std::cout << "Client disconected. Sessions left : " << sessions_.size() << "\n";
}

void TcpServer::broadcast_snapshot(const Snapshot& snapshot) {
    auto payload{Marshaling::serialize(snapshot)};

    for (auto& session : sessions_) {
        if (session) {            
            bytes_sent_+=payload.size();
            session->send_raw(payload);
        }
    }
}
    
size_t TcpServer::get_session_count() const {
    return sessions_.size();
}

uint64_t TcpServer::get_bytes_sent() const {
    return bytes_sent_;
}
