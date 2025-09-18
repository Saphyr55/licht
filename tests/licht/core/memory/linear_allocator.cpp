#include <catch2/catch_all.hpp>
#include <catch2/catch_approx.hpp>

#include "licht/core/memory/linear_allocator.hpp"

using namespace licht;

TEST_CASE("LinearMemoryAllocator basic allocation", "[LinearMemoryAllocator]") {
    LinearMemoryAllocator alloc;

    alloc.initialize(1024);

    SECTION("Allocate returns non-null and respects alignment.") {
        void* ptr1 = alloc.allocate(16, alignof(int32));
        REQUIRE(ptr1 != nullptr);
        REQUIRE(reinterpret_cast<std::uintptr_t>(ptr1) % alignof(int32) == 0);

        void* ptr2 = alloc.allocate(32, alignof(float64));
        REQUIRE(ptr2 != nullptr);
        REQUIRE(reinterpret_cast<std::uintptr_t>(ptr2) % alignof(float64) == 0);
    }

    SECTION("Reset allows reuse of memory.") {
        
        float64* ptr1 = static_cast<float64*>(alloc.allocate(sizeof(float64), alignof(float64)));
        REQUIRE(ptr1 != nullptr);
        *ptr1 = 42.0;

        alloc.reset();

        float64* ptr2 = static_cast<float64*>(alloc.allocate(sizeof(float64), alignof(float64)));
        REQUIRE(ptr2 != nullptr);
        *ptr2 = 99.0;

        REQUIRE(*ptr2 == 99.0);
    }

    SECTION("Allocation beyond pool size fails with nullptr.") {
        void* big = alloc.allocate(2000, alignof(int32));
        REQUIRE(big == nullptr);

        void* small = alloc.allocate(64, alignof(int32));
        REQUIRE(small != nullptr);
    }

    alloc.destroy();
}

TEST_CASE("LinearAllocator usage.", "[LinearAllocator]") {

    LinearAllocator<int32> allocator(512);

    SECTION("Allocate array of ints.") {
        int32* arr = allocator.allocate(10);
        REQUIRE(arr != nullptr);

        for (int32 i = 0; i < 10; ++i) {
            arr[i] = i * 2;
        }
        for (int32 i = 0; i < 10; ++i) {
            REQUIRE(arr[i] == i * 2);
        }
    }

    SECTION("Allocate multiple different types with same pool.") {
        LinearAllocator<float64> float64Alloc(512);

        int32* ints = allocator.allocate(4);
        REQUIRE(ints != nullptr);

        float64* float64s = float64Alloc.allocate(4);
        REQUIRE(float64s != nullptr);

        ints[0] = 7;
        float64s[0] = 3.1415;

        REQUIRE(ints[0] == 7);
        REQUIRE(float64s[0] == Catch::Approx(3.1415));
    }

    SECTION("Out of memory returns nullptr.") {
        int32* arr = allocator.allocate(10'000);
        REQUIRE(arr == nullptr);
    }

}