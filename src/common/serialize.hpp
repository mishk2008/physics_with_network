#pragma once

#include "types.hpp"

#include <algorithm>

template <typename T>
inline void to_network(T& value) {
    if constexpr (std::endian::native == std::endian::little) {
        auto* bytes=reinterpret_cast<uint8_t*>(&value);
        std::reverse(bytes, bytes+sizeof(T));
    }
}

class Marshaling {
public:
    static std::vector<uint8_t> serialize(const Snapshot& snapshot_);

    static Snapshot deserialize(const uint8_t* data, size_t size);
};
