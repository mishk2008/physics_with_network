#pragma once

#include <cstdint>
#include <vector>
#include <string>

struct Vec2 {
    float x;
    float y;
};

struct Ball {
    uint32_t id;
    Vec2 pos;
    Vec2 vel;
    float radius;
};

struct Snapshot {
    uint32_t tick;
    std::vector<Ball> balls;
};
