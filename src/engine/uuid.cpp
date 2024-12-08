#include <engine/uuid.hpp>

#include <ctime>
#include <cstdint>

UUID GenUUID::MakeUUID()
{
    // Get current time since epoch in seconds
    u64 timePart = static_cast<u64>(time(nullptr));

    // Simple linear congruential generator for random number generation
    static u64 seed = time(nullptr);
    seed = (6364136223846793005ULL * seed + 1);
    u32 randomPart = static_cast<u32>(seed >> 32);

    // Combine time part and random part to form the UUID
    return (timePart << 32) | randomPart;
}