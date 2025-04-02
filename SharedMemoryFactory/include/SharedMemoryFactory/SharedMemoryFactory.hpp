#pragma once

#include <PosixSharedMemory/PosixSharedMemory.hpp>

class SharedMemoryFactory 
{
public:
    enum class Type 
    {
        POSIX,
        SYSTEM_V,
    };

    static std::unique_ptr<SharedMemory> create(Type type, const std::string& name, size_t capacity = 100) 
    {
        switch (type) 
        {
            case Type::POSIX:
                return std::make_unique<PosixSharedMemory>(name, capacity);
            default:
                throw std::invalid_argument("Unsupported shared memory type");
        }
    }
};