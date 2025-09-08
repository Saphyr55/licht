#include <catch2/catch_all.hpp>

#include "licht/core/collection/array.hpp"
#include "licht/core/memory/linear_allocator.hpp"

using namespace licht;

TEST_CASE("Array with LinearAllocator basic behavior.", "[Array][LinearAllocator]") {
    struct IntPair {
        int32 first;
        int32 second;
    };

    LinearMemoryPool pool(1024);  // 1 KB
    LinearAllocator<int32> int32Alloc(&pool);
    LinearAllocator<double> float64Alloc(&pool);

    SECTION("Construct array with linear allocator and append elements.") {
        Array<int32, LinearAllocator<int32>> arr(int32Alloc);

        REQUIRE(arr.size() == 0);
        REQUIRE(arr.capacity() >= 1);

        arr.append(10);
        arr.append(20);
        arr.append(30);

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 10);
        REQUIRE(arr[1] == 20);
        REQUIRE(arr[2] == 30);
    }

    SECTION("Emplace and pop behave correctly.") {
        LinearAllocator<IntPair> alloc(&pool);
        Array<IntPair, LinearAllocator<IntPair>> pairArr(alloc);

        pairArr.emplace(1, 2);
        pairArr.emplace(3, 4);

        REQUIRE(pairArr.size() == 2);
        REQUIRE(pairArr[0].first == 1);
        REQUIRE(pairArr[0].second == 2);

        pairArr.pop();
        REQUIRE(pairArr.size() == 1);
        REQUIRE(pairArr[0].first == 1);
    }

    SECTION("Map produces a new array using same allocator type (but can use other allocator).") {
        Array<int32, LinearAllocator<int32>> ints(int32Alloc);
        ints.append(1);
        ints.append(2);
        ints.append(3);

        // map to float64 using same pool but different allocator type
        Array<float64, LinearAllocator<float64>> doubles = ints.map<float64, LinearAllocator<float64>>([](int32 v) -> float64 {
            return static_cast<float64>(v) + 0.5;
        },
                                                                                                       float64Alloc);

        REQUIRE(doubles.size() == ints.size());
        REQUIRE(doubles[0] == Catch::Approx(1.5));
        REQUIRE(doubles[1] == Catch::Approx(2.5));
        REQUIRE(doubles[2] == Catch::Approx(3.5));
    }

    SECTION("Copy and move semantics.") {
        Array<int32, LinearAllocator<int32>> a(int32Alloc);
        a.append(5);
        a.append(6);

        Array<int32, LinearAllocator<int32>> b = a;
        REQUIRE(b.size() == 2);
        REQUIRE(b[0] == 5);
        REQUIRE(b[1] == 6);

        Array<int32, LinearAllocator<int32>> c = std::move(a);
        REQUIRE(c.size() == 2);
        REQUIRE(c[0] == 5);
    }

    SECTION("Reserve, resize, shrink and clear.") {
        Array<int32, LinearAllocator<int32>> arr(int32Alloc);

        arr.reserve(16);
        REQUIRE(arr.capacity() >= 16);

        arr.resize(8, 42);
        REQUIRE(arr.size() == 8);

        for (usize i = 0; i < arr.size(); ++i) {
            REQUIRE(arr[i] == 42);
        }

        arr.shrink();
        REQUIRE(arr.capacity() >= arr.size());

        arr.clear();
        REQUIRE(arr.size() == 0);
    }

    SECTION("Check contains() and get_if() methods.") {
        Array<int32, LinearAllocator<int32>> arr(int32Alloc);
        arr.append(11);
        arr.append(22);
        arr.append(33);

        REQUIRE(arr.contains(22));
        REQUIRE(!arr.contains(99));

        int32* found = arr.get_if([](int32 v) { return v == 33; });
        REQUIRE(found != nullptr);
        REQUIRE(*found == 33);
    }

    pool.reset();
    pool.destroy(); // Optional, raii
} 
