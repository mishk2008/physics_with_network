#include <iostream>
#include <asio.hpp>
#include <memory>
#include <string>

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {

public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    tcp::socket socket_;
    enum { max_length=1024 };
    char data_[max_length];
// прочтение сколько смогло прийти
    void do_read() {
        auto self(shared_from_this());

        socket_.async_read_some(asio::buffer(data_, max_length),
        [this,self](std::error_code ec, std::size_t length) {
            if(!ec) {
                do_write(length);
            }
        }); 
    }
// запись всего что пришло
    void do_write(std::size_t length) {
        auto self(shared_from_this());

        asio::async_write(socket_, asio::buffer(data_, length),
        [this,self](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::cout << "receaved " << length << " bytes :" << std::string_view(data_, length) << "\n";
                do_read();                
            }
        });
    }
};

class Server {
public:
    Server(asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
    socket_(io_context) {
            do_accept();
        }

private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;

    void do_accept() {
        acceptor_.async_accept( socket_,
        [this](std::error_code ec) { 
            if (!ec) {
                tcp::endpoint remote = socket_.remote_endpoint();
                std::cout << "Client connected from "
                << remote.address().to_string() << ":"
                << remote.port() << "\n";

                std::make_shared<Session>(std::move(socket_))->start();
            }
            do_accept();
        });
    }
};
