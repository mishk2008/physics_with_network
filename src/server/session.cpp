#include "session.hpp"

void Session::send_raw(const std::vector<uint8_t>& payload) {
    auto buffer{std::make_shared<std::vector<uint8_t>>()};

    uint32_t len{static_cast<uint32_t>(payload.size())};
    to_network(len);
    uint8_t* len_ptr{reinterpret_cast<uint8_t*>(&len)};
    
    buffer->insert(buffer->end(), len_ptr, len_ptr+4);
    buffer->insert(buffer->end(), payload.begin(), payload.end());

    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(*buffer),
    [this, self, buffer](std::error_code ec, size_t length) {
        if (ec) {
            if (auto server=server_weak_.lock()) server->remove_session(self);
        }
    });
}