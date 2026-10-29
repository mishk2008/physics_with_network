#pragma once

#include "types.hpp"

#include <iostream>
#include <format>
#include <random>
#include <memory>

class World {
public:
    explicit World(size_t balls_num);

    void update(float dt);

    Snapshot make_snapshot(uint32_t tick) const;

    void print() const {
        for (size_t i=0; i<6;  i++)
            for (size_t j=0; j<8; j++)
                std::cout << std::format("[{:>3}][{:>3}] -> [{:>3}]\n", i, j, worldGrid[i*8+j].size());
    }

private:
    std::vector<Ball> balls_;
    std::vector<std::vector<Ball*>> worldGrid;
    float height_;
    float width_;
};
