#include <catch2/catch_all.hpp>

#include "licht/core/containers/array.hpp"

using namespace licht;

struct ArrayCounterTest {
    static int32 constructions;
    static int32 destructions;
    int32 value;

    ArrayCounterTest(int32 v = 0)
        : value(v) { ++constructions; }

    ArrayCounterTest(const ArrayCounterTest& o)
        : value(o.value) { ++constructions; }

    ArrayCounterTest& operator=(const ArrayCounterTest& o) {
        value = o.value;
        return *this;
    }

    ~ArrayCounterTest() { ++destructions; }

    bool operator==(const ArrayCounterTest& o) const { return value == o.value; }
};

int32 ArrayCounterTest::constructions = 0;
int32 ArrayCounterTest::destructions = 0;

TEST_CASE("Basic append/emplace/pop and indexing", "[Array]") {
    Array<int32> a;
    REQUIRE(a.empty());
    REQUIRE(a.capacity() >= 1);

    a.append(10);
    a.append(20);
    a.emplace(30);
    REQUIRE(a.size() == 3);
    REQUIRE(a[0] == 10);
    REQUIRE(a[1] == 20);
    REQUIRE(a[2] == 30);

    REQUIRE(a.front() == 10);
    REQUIRE(a.back() == 30);

    a.pop();
    REQUIRE(a.size() == 2);
    REQUIRE(a.back() == 20);

    a.pop();
    a.pop();
    REQUIRE(a.empty());
}

TEST_CASE("Reserve, resize and shrink behaviour", "[Array]") {
    Array<int32> a;
    a.reserve(50);
    REQUIRE(a.capacity() >= 50);

    a.resize(10);
    REQUIRE(a.size() == 10);
    REQUIRE(a.capacity() >= 50);

    a.shrink();
    REQUIRE(a.capacity() == a.size());
}

TEST_CASE("Remove, contains and get_if", "[Array]") {
    Array<int32> a = {1, 2, 3, 4, 5, 3, 6};

    REQUIRE(a.contains(3));
    a.remove(3);
    REQUIRE_FALSE(a.contains(3));
    REQUIRE(a.size() == 5);

    auto p = a.get_if([](int32 v) { return v % 2 == 0; }); // first even
    REQUIRE(p != nullptr);
    REQUIRE(*p % 2 == 0);

    a.remove_if([](auto& x) -> bool {
        return x == 4;
    });
    REQUIRE_FALSE(a.contains(4));
}

TEST_CASE("Copy, move, append_all and initializer list", "[Array]") {
    Array<int32> a = {5, 6, 7};
    Array<int32> b(a);  // copy
    REQUIRE(a == b);

    Array<int32> c;
    c.append_all(a);
    REQUIRE(c == a);

    Array<int32> d(std::move(a));  // move
    REQUIRE(d.size() == 3);
    // a moved-from: safe to use but unspecified contents; we won't assert on a here

    Array<int32> e;
    e = b;  // copy assign
    REQUIRE(e == b);

    Array<int32> f;
    f = std::move(b);  // move assign
    REQUIRE(f.size() == 3);
}

TEST_CASE("Map & map result type", "[Array::map]") {

    Array<int32> a = {1, 2, 3, 4};

    auto mapped = a.map<float64>([](int32 v) { return static_cast<float64>(v) / 2.0; });

    REQUIRE(mapped.size() == a.size());
    for (size_t i = 0; i < mapped.size(); ++i) {
        REQUIRE(mapped[i] == Catch::Approx(static_cast<float64>(a[i]) / 2.0));
    }

    auto to_ArrayCounterTest = a.map<ArrayCounterTest>([](int32 v) { return ArrayCounterTest(v * 10); });
    REQUIRE(to_ArrayCounterTest.size() == a.size());
    for (size_t i = 0; i < to_ArrayCounterTest.size(); ++i) {
        REQUIRE(to_ArrayCounterTest[i].value == a[i] * 10);
    }
}

TEST_CASE("Iterators and range-for", "[Array::iterators]") {
    Array<int32> a;
    for (int32 i = 0; i < 10; ++i) {
        a.append(i);
    }

    int32 sum = 0;
    for (int32 v : a) {
        sum += v;
    }
    REQUIRE(sum == 45);  // 0..9 sum
}

TEST_CASE("Equality, inequality and swap", "[Array::operators]") {
    Array<int32> a = {1, 2, 3};
    Array<int32> b = {1, 2, 3};
    Array<int32> c = {3, 2, 1};

    REQUIRE(a == b);
    REQUIRE(a != c);

    a.swap(c);
    REQUIRE(a[0] == 3);
    REQUIRE(c[0] == 1);
}

TEST_CASE("Allocator/placement destruction check via ArrayCounterTest", "[Array::destruction]") {
    ArrayCounterTest::constructions = 0;
    ArrayCounterTest::destructions = 0;

    {
        Array<ArrayCounterTest> arr;
        arr.append(ArrayCounterTest(10));
        arr.append(ArrayCounterTest(20));
        arr.emplace(30);
        REQUIRE(arr.size() == 3);
    }

    REQUIRE(ArrayCounterTest::constructions == ArrayCounterTest::destructions);
}

TEST_CASE("reallocate moves elements and preserves values", "[Array::reallocate]") {
    Array<std::string> a;
    for (int32 i = 0; i < 8; ++i) {
        a.append(std::string("s") + std::to_string(i));
    }

    auto old_values = a;
    a.reserve(32);
    REQUIRE(a.size() == old_values.size());
    REQUIRE(a == old_values);
}

TEST_CASE("Nested Array<Array<T>> behavior", "[Array::nested]") {
    ArrayCounterTest::constructions = 0;
    ArrayCounterTest::destructions = 0;

    {
        Array<Array<ArrayCounterTest>> nested;  // Array of Arrays

        // Create a few inner arrays
        Array<ArrayCounterTest> inner1;
        inner1.append(ArrayCounterTest(1));
        inner1.append(ArrayCounterTest(2));

        Array<ArrayCounterTest> inner2;
        inner2.append(ArrayCounterTest(10));
        inner2.append(ArrayCounterTest(20));
        inner2.append(ArrayCounterTest(30));

        // Append inner arrays into the outer one
        nested.append(inner1);
        nested.append(std::move(inner2));

        REQUIRE(nested.size() == 2);
        REQUIRE(nested[0].size() == 2);
        REQUIRE(nested[1].size() == 3);

        // Check values
        REQUIRE(nested[0][0].value == 1);
        REQUIRE(nested[0][1].value == 2);
        REQUIRE(nested[1][0].value == 10);
        REQUIRE(nested[1][2].value == 30);

        // Modify nested elements to ensure deep access works
        nested[0][1].value = 99;
        REQUIRE(nested[0][1].value == 99);

        // Copy the whole nested array
        Array<Array<ArrayCounterTest>> copy = nested;
        REQUIRE(copy.size() == nested.size());
        REQUIRE(copy[0][1].value == 99);

        // Move it to another
        Array<Array<ArrayCounterTest>> moved = std::move(copy);
        REQUIRE(moved.size() == 2);
        REQUIRE(moved[0][1].value == 99);
    }

    // Ensure all constructed elements were properly destroyed
    REQUIRE(ArrayCounterTest::constructions == ArrayCounterTest::destructions);
}

TEST_CASE("move_data and move assignment corner cases", "[Array::move]") {
    Array<int32> src = {9, 8, 7};
    Array<int32> target(0);
    target = src;
    REQUIRE(target.size() == src.size());
    REQUIRE(target == src);

    Array<int32> m = std::move(target);
    REQUIRE(m.size() == 3);
}
