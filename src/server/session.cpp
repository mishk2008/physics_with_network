#include "session.hpp"

Session::Session(tcp::socket socket, std::weak_ptr<IServerTransport> server_weak) 
: socket_(std::move(socket)), server_weak_(std::move(server_weak)) {}

void Session::send_raw(const std::vector<uint8_t>& payload) {
    if (write_queue_.size() >= 20) { return; }

    auto buffer{std::make_shared<std::vector<uint8_t>>()};

    uint32_t len{static_cast<uint32_t>(payload.size())};
    to_network(len);
    uint8_t* len_ptr{reinterpret_cast<uint8_t*>(&len)};
    
    buffer->insert(buffer->end(), len_ptr, len_ptr+4);
    buffer->insert(buffer->end(), payload.begin(), payload.end());

    write_queue_.push_back(buffer);

    if (!writing_) { do_write(); }
}

void Session::do_write() {
    if (write_queue_.empty()) {
        writing_ = false;
        return;
    }

    writing_ = true;
    auto buffer{write_queue_.front()};
    auto self{shared_from_this()};

    asio::async_write(socket_, asio::buffer(*buffer), 
    [this, self, buffer](std::error_code ec, size_t length) {
        if (!ec) {
            write_queue_.pop_front();
            do_write();
        } else {
            write_queue_.clear();
            writing_ = false;
            if (auto server = server_weak_.lock()) server->remove_session(self);
        }

    });
}