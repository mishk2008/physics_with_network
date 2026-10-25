#include "physics.hpp"

World::World(size_t balls_num) 
: width_(800.0f), height_(600.0f) {
    balls_.reserve(balls_num);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis_x(50.0f, width_-50.0f);
    std::uniform_real_distribution<float> dis_y(50.0f, height_-50.0f);
    std::uniform_real_distribution<float> dis_v(-100.0f,100.0f);

    for (size_t i{0}; i<balls_num; i++) {
        balls_.push_back(Ball{  static_cast<uint32_t>(i),
                                    dis_x(gen),
                                    dis_y(gen),
                                    dis_v(gen),
                                    dis_v(gen),
                                    10.0f});
    }
}

void World::update(float dt) {
    for (auto& ball : balls_) {
        ball.pos.x +=ball.vel.x * dt;
        ball.pos.y +=ball.vel.y * dt;
        if (ball.pos.x + ball.radius > width_) {
            ball.pos.x=width_-ball.radius;
            ball.vel.x=-ball.vel.x;
        } else if (ball.pos.x - ball.radius < 0.0f) {
            ball.pos.x=ball.radius;
            ball.vel.x=-ball.vel.x;                
        }

        if (ball.pos.y + ball.radius > height_) {
            ball.pos.y=height_-ball.radius;
            ball.vel.y=-ball.vel.y;
        } else if (ball.pos.y - ball.radius < 0.0f) {
            ball.pos.y=ball.radius;
            ball.vel.y=-ball.vel.y;                
        }            
    }
}

Snapshot World::make_snapshot(uint32_t tick) const {
    return Snapshot{tick, balls_};
}
