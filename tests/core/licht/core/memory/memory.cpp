#include <catch2/catch_all.hpp>
#include <catch2/catch_approx.hpp>

#include "licht/core/memory/memory.hpp"

using namespace licht;

TEST_CASE("Memory alignment functions.", "[Memory][Alignment]") {
    SECTION("align_address() calculates correct alignment.") {
        REQUIRE(Memory::align_address(0, 16) == 0);
        REQUIRE(Memory::align_address(1, 16) == 16);
        REQUIRE(Memory::align_address(15, 16) == 16);
        REQUIRE(Memory::align_address(16, 16) == 16);
        REQUIRE(Memory::align_address(17, 16) == 32);
    }

    SECTION("align_adjustment() calculates correct adjustment.") {
        REQUIRE(Memory::align_adjustment(0, 16) == 0);
        REQUIRE(Memory::align_adjustment(1, 16) == 15);
        REQUIRE(Memory::align_adjustment(15, 16) == 1);
        REQUIRE(Memory::align_adjustment(16, 16) == 0);
        REQUIRE(Memory::align_adjustment(17, 16) == 15);
    }

    SECTION("aligned_size() calculates correct size with alignment.") {
        REQUIRE(Memory::aligned_size(10, 16) == 25); // (16-1) + 10 = 15 + 10 = 25
        REQUIRE(Memory::aligned_size(16, 16) == 31); // (16-1) + 16 = 15 + 16 = 31
    }

    SECTION("is_aligned() correctly detects alignment.") {
        REQUIRE(Memory::is_aligned(0, 16) == true);
        REQUIRE(Memory::is_aligned(16, 16) == true);
        REQUIRE(Memory::is_aligned(32, 16) == true);
        REQUIRE(Memory::is_aligned(1, 16) == false);
        REQUIRE(Memory::is_aligned(15, 16) == false);
        REQUIRE(Memory::is_aligned(17, 16) == false);
    }
}

TEST_CASE("Memory allocation and deallocation.", "[Memory][Allocation]") {

    SECTION("Basic allocation and free.") {
        void* ptr = Memory::allocate(100);
        REQUIRE(ptr != nullptr);
        Memory::free(ptr, 100);
    }

    SECTION("Aligned allocation and free.") {
        const size_t alignment = 16;
        const size_t size = 100;
        
        void* ptr = Memory::allocate(size, alignment);
        REQUIRE(ptr != nullptr);
        
        uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
        REQUIRE(Memory::is_aligned(address, alignment));
        
        Memory::free(ptr, size, alignment);
    }

    SECTION("Aligned allocation with various alignments.") {
        const size_t sizes[] = {10, 100, 1024};
        const size_t alignments[] = {2, 4, 8, 16, 32, 64, 128};
        
        for (size_t size : sizes) {
            for (size_t alignment : alignments) {
                void* ptr = Memory::allocate(size, alignment);
                REQUIRE(ptr != nullptr);
                
                uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
                REQUIRE(Memory::is_aligned(address, alignment));
                
                Memory::free(ptr, size, alignment);
            }
        }
    }
}

TEST_CASE("Memory utility functions.", "[Memory][Utility]") {
    SECTION("copy() function works correctly.") {
        constexpr const char source[] = "Hello, World!";
        char destination[20];
        
        void* result = Memory::copy(destination, source, sizeof(source));
        REQUIRE(result == destination);
        REQUIRE(Memory::compare(destination, source, sizeof(source)) == 0);
    }

    SECTION("copy_move() function works correctly.") {
        char buffer[] = "Hello, World!";
        
        void* result = Memory::move(buffer + 6, buffer, 7);
        REQUIRE(result == buffer + 6);
        REQUIRE(std::strcmp(buffer + 6, "Hello, ") == 0);
    }

    SECTION("write() function works correctly.") {
        constexpr size_t size = 10;
        char buffer[size];
        
        void* result = Memory::write(buffer, int32('A'), size);
        REQUIRE(result == buffer);
        
        for (int8 i = 0; i < size; i++) {
            REQUIRE(buffer[i] == 0x41);
        }
    }

    SECTION("compare() function works correctly.") {
        const char str1[] = "Hello";
        const char str2[] = "Hello";
        const char str3[] = "World";
        
        REQUIRE(Memory::compare(str1, str2, 5) == 0);
        REQUIRE(Memory::compare(str1, str3, 5) < 0);
        REQUIRE(Memory::compare(str3, str1, 5) > 0);
    }
}

TEST_CASE("Edge cases and stress tests", "[Memory][Edge]") {
    SECTION("Zero size allocation.") {
        void* ptr = Memory::allocate(0);
        REQUIRE(ptr != nullptr);
        Memory::free(ptr, 0);
    }

    SECTION("Large allocation.") {
        constexpr size_t large_size = 1024 * 1024;  // 1MB
        void* ptr = Memory::allocate(large_size);
        REQUIRE(ptr != nullptr);
        Memory::free(ptr, large_size);
    }

    SECTION("Maximum alignment.") {
        constexpr size_t max_alignment = 128;
        void* ptr = Memory::allocate(100, max_alignment);
        REQUIRE(ptr != nullptr);
        REQUIRE(Memory::is_aligned(reinterpret_cast<uintptr_t>(ptr), max_alignment));
        Memory::free(ptr, 100, max_alignment);
    }

    SECTION("Multiple allocations and deallocations.") {
        constexpr int num_allocations = 100;
        void* pointers[num_allocations];
        
        for (int i = 0; i < num_allocations; ++i) {
            pointers[i] = Memory::allocate(i + 1);
            REQUIRE(pointers[i] != nullptr);
        }
        
        for (int i = 0; i < num_allocations; ++i) {
            Memory::free(pointers[i], i + 1);
        }
    }
}
