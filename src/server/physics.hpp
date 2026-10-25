#pragma once

#include "types.hpp"

#include <random>

class World {
public:
    explicit World(size_t balls_num);

    void update(float dt);

    Snapshot make_snapshot(uint32_t tick) const;

private:
    std::vector<Ball> balls_;
    float height_;
    float width_;
};
