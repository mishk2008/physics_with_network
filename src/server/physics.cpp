#include "physics.hpp"

#include <memory>
#include <algorithm>

World::World(size_t balls_num) 
: width_(800.0f), height_(600.0f) {
    balls_.reserve(balls_num);

    worldGrid.resize((width_/100)*(height_/100));
    for (auto& cell : worldGrid) {        
        cell.reserve(16);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis_x(50.0f, width_-50.0f);
    std::uniform_real_distribution<float> dis_y(50.0f, height_-50.0f);
    std::uniform_real_distribution<float> dis_v(-100.0f,100.0f);
    std::uniform_real_distribution<float> dis_mass(1.0f,100.0f);

    for (size_t i{0}; i<balls_num; i++) {
        balls_.push_back(Ball{  static_cast<uint32_t>(i),
                                    dis_x(gen),
                                    dis_y(gen),
                                    dis_v(gen),
                                    dis_v(gen),
                                    10.0f,
                                    dis_mass(gen)});
    }
}

World::~World() {}

void World::add_random_ball() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis_x(50.0f, width_-50.0f);
    std::uniform_real_distribution<float> dis_y(50.0f, height_-50.0f);
    std::uniform_real_distribution<float> dis_v(-100.0f,100.0f);
    std::uniform_real_distribution<float> dis_mass(1.0f,100.0f);  

    balls_.push_back( Ball{static_cast<uint32_t>(balls_.size()), dis_x(gen), dis_y(gen), dis_v(gen), dis_v(gen), 10.0f, dis_mass(gen)});
}

void World::add_ball(const Ball& ball) {
    balls_.push_back(ball);
}

void World::delete_ball() {
    if (balls_.size() == 0) return;
    balls_.pop_back();
}

std::pair<float, float> World::get_borders() const {
    return std::pair<float, float>(width_, height_);
}

size_t World::get_balls_num() const {
    return balls_.size();
}

void World::update(float dt) {
    for (auto& ball : balls_) {
        ball.pos.x +=ball.vel.x * dt;
        ball.pos.y +=ball.vel.y * dt;
        if (ball.pos.x + ball.radius > width_) {
            ball.pos.x=width_-ball.radius;
            ball.vel.x=-ball.vel.x;
            //add_random_ball();
        } else if (ball.pos.x - ball.radius < 0.0f) {
            ball.pos.x=ball.radius;
            ball.vel.x=-ball.vel.x;
            //add_random_ball();               
        }

        if (ball.pos.y + ball.radius > height_) {
            ball.pos.y=height_-ball.radius;
            ball.vel.y=-ball.vel.y;
            //add_random_ball();
        } else if (ball.pos.y - ball.radius < 0.0f) {
            ball.pos.y=ball.radius;
            ball.vel.y=-ball.vel.y;
            //add_random_ball();                
        }            
    }

    for (auto& cell : worldGrid) {
        cell.clear();
    }

    const uint32_t COLS{static_cast<uint32_t>(width_/100)};
    const uint32_t ROWS{static_cast<uint32_t>(height_/100)};
    for (auto& ball : balls_) {
        if (!std::isfinite(ball.pos.x) || !std::isfinite(ball.pos.y)) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis_x(50.0f, width_-50.0f);
            std::uniform_real_distribution<float> dis_y(50.0f, height_-50.0f);
            ball.pos.x=dis_x(gen);
            ball.pos.y=dis_y(gen);
            ball.vel.x=0;
            ball.vel.y=0;
        }

        int min_col{static_cast<int>((ball.pos.x-ball.radius)/100)};
        int max_col{static_cast<int>((ball.pos.x+ball.radius)/100)};
        int min_row{static_cast<int>((ball.pos.y-ball.radius)/100)};
        int max_row{static_cast<int>((ball.pos.y+ball.radius)/100)};

        min_col = std::clamp(min_col, 0, static_cast<int>(COLS-1));
        max_col = std::clamp(max_col, 0, static_cast<int>(COLS-1));
        min_row = std::clamp(min_row, 0, static_cast<int>(ROWS-1));
        max_row = std::clamp(max_row, 0, static_cast<int>(ROWS-1));

        for (int row=min_row; row<=max_row; row++)
            for (int col=min_col; col<=max_col; col++) {
                uint32_t cell_index{row*COLS+col};
                if (cell_index < worldGrid.size()) worldGrid[cell_index].push_back(&ball);
            }
    }

    for (auto& cell : worldGrid) {
        if (cell.size() < 2) continue;


        for (size_t initial_index{0}; initial_index<cell.size(); initial_index++) {
            bool is_collided_this_tick{0};
            for (size_t relative_index{initial_index+1}; relative_index<cell.size(); relative_index++) {
                auto* init_ball{cell[initial_index]};
                auto* rel_ball{cell[relative_index]};

                if (init_ball->id > rel_ball->id) continue;

                float dx{rel_ball->pos.x - init_ball->pos.x};
                float dy{rel_ball->pos.y - init_ball->pos.y};
                float distancce_sq{dx*dx+dy*dy};

                float radius_sum{init_ball->radius + rel_ball->radius};
                float radius_sum_sq{radius_sum*radius_sum};

                if (distancce_sq <= radius_sum_sq && !is_collided_this_tick) {
                    is_collided_this_tick=1;
                    float distance{std::sqrt(distancce_sq)};
                    float overlap{radius_sum-distance};

                    if (distance >= 0.0f) {
                        float nx{(dx)/distance};
                        float ny{(dy)/distance};

                        float rel_ball_mass{rel_ball->mass};
                        float init_ball_mass{init_ball->mass};

                        init_ball->pos.x-=nx*overlap*(rel_ball_mass/(rel_ball_mass+init_ball_mass));
                        init_ball->pos.y-=ny*overlap*(rel_ball_mass/(rel_ball_mass+init_ball_mass));
                        rel_ball->pos.x+=nx*overlap*(init_ball_mass/(rel_ball_mass+init_ball_mass));
                        rel_ball->pos.y+=ny*overlap*(init_ball_mass/(rel_ball_mass+init_ball_mass));

                        float kx{init_ball->vel.x - rel_ball->vel.x};
                        float ky(init_ball->vel.y - rel_ball->vel.y);

                        float p{2*(nx*kx+ny*ky)/(rel_ball_mass + init_ball_mass)};

                        init_ball->vel.x-=p*rel_ball->mass*nx;
                        init_ball->vel.y-=p*rel_ball->mass*ny;
                        rel_ball->vel.x+=p*init_ball->mass*nx; 
                        rel_ball->vel.y+=p*init_ball->mass*ny;                        
                    }
                }
            }
        }
    }
}

Snapshot World::make_snapshot(uint32_t tick) const {
    return Snapshot{tick, balls_};
}
