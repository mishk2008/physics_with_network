#include "physics.hpp"
#include "server.hpp"

#include <iostream>
#include <asio.hpp>
#include <string>
#include <sstream>
#include <fstream>

constexpr float interval{0.05f};

int main(int argc, char** argv) {
    uint32_t port{12345};
    if (argc > 1 &&
        std::strlen(argv[1]) > 0 && std::strlen(argv[1]) < 6 &&
        std::all_of(argv[1], argv[1]+std::strlen(argv[1]), [](unsigned char ch) { return std::isdigit(ch); })
    ) port=static_cast<uint32_t>(std::stoi(argv[1]));
    else std::cout << "Port not entered or entered incorrectly; running at basic port 12345;\nBasic program using : " << argv[0] << " <port>\n\n";

    asio::io_context io;
    std::shared_ptr<IServerTransport> server{std::make_shared<TcpServer>(io, port)};
    server->start();

    asio::ip::udp::resolver resolver(io);
    auto endpoints{resolver.resolve(asio::ip::host_name(), "")};
    for (const auto& entry : endpoints) {
        asio::ip::address addr{entry.endpoint().address()};

        if (addr.is_v4() && !addr.is_loopback()) {
            std::cout << "addr -> " << addr.to_string() << '\n';
        }
    }

    World world(10);
    uint32_t tick{0};
    asio::steady_timer timer(io);

    std::thread control_thread([&io, &world]() {
        std::cout << "\nControl thread started.\n";
        while (true) {
            std::cout << "\nSTOP SERVER == 0\nDELETE BALL == 1 (LIFO)\nADD RANDOM BALL == 2\nADD INSCRIBED BALL == 3\nMessages about connecting and disconnecting may overlap\nInput : ";
            std::string command{""};
            while (getline(std::cin, command) && command.empty() || !(std::all_of(command.begin(), command.end(), [](const char& a) { return isdigit(a); }))) { 
                std::cout << "Wrong input : ";  
            }
            int cmd{std::stoi(command)};

            if (cmd == 0) { io.stop(); break; }
            else if (cmd == 1) { asio::post(io, [&world]() { world.delete_ball(); }); }
            else if (cmd == 2) { asio::post(io, [&world]() { world.add_random_ball(); }); }
            else if (cmd == 3) {
                std::cout << "0 < x < " << world.get_borders().first <<
                            "; 0 < y < " << world.get_borders().second << "; 0 < radius < 100; mass > 0;\nAll parameters are float type (0.0f);\n" <<
                            "The basic input format :\n<x>;<y>;<vel_x>;<vel_y>;<radius>;<mass>\\n\nInput :\n";
                std::string input{""};
                getline(std::cin, input);
                std::istringstream iss(input);
                Ball ball;
                if ((iss >> ball.pos.x && iss.get() == ';') && (iss >> ball.pos.y && iss.get() == ';') &&
                (iss >> ball.vel.x && iss.get() == ';') && (iss >> ball.vel.y && iss.get() == ';') &&
                (iss >> ball.radius && iss.get() == ';') && (iss >> ball.mass) &&
                (ball.pos.x > 0.0f && ball.pos.x < world.get_borders().first) &&
                (ball.pos.y > 0.0f && ball.pos.y < world.get_borders().second) &&
                (ball.radius > 0.0f && ball.radius < 100.0f) && (ball.mass > 0.0f)) {
                    ball.id=world.get_balls_num();
                    asio::post(io, [&world, ball]() { world.add_ball(ball); });
                } else std::cerr << "Error, input data does not conform to the agreed-upon format.\n";
            }
            else std::cout << "Wrong input : ";
        }
    });

    std::function<void()> tick_handler=[&]() {
        //auto start{std::chrono::high_resolution_clock::now()};
        world.update(interval);
        //auto end{std::chrono::high_resolution_clock::now()};
        //auto ms{std::chrono::duration<float, std::milli>(end - start).count()};

        //static int counter{0};
        //static float total_ms{0};
        //total_ms+=ms;
        //counter++;

        //if (counter==100) {
        //    std::cout << "Balls : " << world.get_balls_num()
        //    << "| Avg update time : " << (total_ms / 100.0f) << "ms\n";
        //    counter=0; total_ms=0; 
        //}

        auto snap{world.make_snapshot(tick)};

        server->broadcast_snapshot(snap);

        timer.expires_after(std::chrono::milliseconds(50));
        timer.async_wait([&](std::error_code ec) {
            if (!ec) tick_handler();
        });

        tick++;
    };
    
    timer.expires_after(std::chrono::milliseconds(50));
    timer.async_wait([&](std::error_code ec) {
        if (!ec) tick_handler();
    });
/*
    asio::steady_timer log_timer(io);
    uint64_t last_bytes{0};

    std::ofstream file("server_log.csv", std::ios::app);
    if (file.is_open()) {
        file << "tick,clients,balls,bytes_per_sec\n";
    }

    std::function<void()> log_handler{[&]() {

        if (file.is_open()) {
            uint64_t current_bytes{server->get_bytes_sent()};
            uint64_t bytes_sec{current_bytes - last_bytes};
            last_bytes = current_bytes;

            file << tick << ","
            << server->get_session_count() << ","
            << world.get_balls_num() << ","
            << bytes_sec << "\n";
        }

        log_timer.expires_after(std::chrono::seconds(1));
        log_timer.async_wait([&](std::error_code ec) {
            if (!ec) log_handler();
        });
    }};

    log_timer.expires_after(std::chrono::seconds(1));
    log_timer.async_wait([&](std::error_code ec) {
        if (!ec) log_handler();
    });    
*/
    io.run();

    if (control_thread.joinable()) control_thread.join();

    return 0;
}