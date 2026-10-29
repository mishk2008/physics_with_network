#include "serialize.hpp"

#include <iostream>
#include <cstring>
#include <bit>

template <typename T>
void write_binary(std::vector<uint8_t>& buffer, T value) {
    to_network(value);
    const auto* bytes{reinterpret_cast<const uint8_t*>(&value)};
    buffer.insert(buffer.end(), bytes, bytes+sizeof(T));
}

template <typename T>
T read_binary(const uint8_t*& ptr) {
    T value;
    std::memcpy(&value, ptr, sizeof(T));
    ptr+=sizeof(T);
    to_network(value);
    return value;
}

std::vector<uint8_t> Marshaling::serialize(const Snapshot& snapshot_) {
    std::vector<uint8_t> output_data;
    uint16_t arr_size{static_cast<uint16_t>(snapshot_.balls.size())};    
    
    write_binary(output_data, snapshot_.tick);
    write_binary(output_data, arr_size);

    for (uint16_t i=0; i<arr_size; i++) {
        write_binary(output_data, snapshot_.balls[i].id);
        write_binary(output_data, snapshot_.balls[i].radius);
        write_binary(output_data, snapshot_.balls[i].mass);
        write_binary(output_data, snapshot_.balls[i].pos.x);
        write_binary(output_data, snapshot_.balls[i].pos.y);
        write_binary(output_data, snapshot_.balls[i].vel.x);
        write_binary(output_data, snapshot_.balls[i].vel.y);       
    }

    return output_data;
}

Snapshot Marshaling::deserialize(const uint8_t* data, size_t size) {
    if (size < sizeof(uint16_t)+sizeof(uint32_t)) throw std::runtime_error("buffer too small for headers");   
    Snapshot snapshot;
    const uint8_t* ptr{data};    

    snapshot.tick=read_binary<uint32_t>(ptr);  
    uint16_t arr_size=read_binary<uint16_t>(ptr);

    constexpr size_t size_of_one_ball{6*sizeof(float)+sizeof(uint32_t)};
    if (size < (arr_size*size_of_one_ball)) throw std::runtime_error("buffer too small for all balls");   

    snapshot.balls.resize(arr_size);

    for (uint16_t i=0; i<arr_size; i++) {
        snapshot.balls[i].id=read_binary<uint32_t>(ptr);
        snapshot.balls[i].radius=read_binary<float>(ptr);
        snapshot.balls[i].mass=read_binary<float>(ptr);
        snapshot.balls[i].pos.x=read_binary<float>(ptr);
        snapshot.balls[i].pos.y=read_binary<float>(ptr);
        snapshot.balls[i].vel.x=read_binary<float>(ptr);
        snapshot.balls[i].vel.y=read_binary<float>(ptr);
    }

    return snapshot;
}
