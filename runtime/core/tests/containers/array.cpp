#include <catch2/catch_all.hpp>

#include "licht/core/containers/array.hpp"

using namespace licht;

struct Counter {
    static int32 constructions;
    static int32 destructions;
    int32 value;

    Counter(int32 v = 0)
        : value(v) { ++constructions; }

    Counter(const Counter& o)
        : value(o.value) { ++constructions; }

    Counter& operator=(const Counter& o) {
        value = o.value;
        return *this;
    }

    ~Counter() { ++destructions; }

    bool operator==(const Counter& o) const { return value == o.value; }
};

int32 Counter::constructions = 0;
int32 Counter::destructions = 0;

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
    Array<int32> b(a);  // copy ctor
    REQUIRE(a == b);

    Array<int32> c;
    c.append_all(a);
    REQUIRE(c == a);

    Array<int32> d(std::move(a));  // move ctor
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

    auto to_counter = a.map<Counter>([](int32 v) { return Counter(v * 10); });
    REQUIRE(to_counter.size() == a.size());
    for (size_t i = 0; i < to_counter.size(); ++i) {
        REQUIRE(to_counter[i].value == a[i] * 10);
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

TEST_CASE("Allocator/placement destruction check via Counter", "[Array::destruction]") {
    Counter::constructions = 0;
    Counter::destructions = 0;

    {
        Array<Counter> arr;
        arr.append(Counter(10));
        arr.append(Counter(20));
        arr.emplace(30);
        REQUIRE(arr.size() == 3);
    }

    REQUIRE(Counter::constructions == Counter::destructions);
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

TEST_CASE("move_data and move assignment corner cases", "[Array::move]") {
    Array<int32> src = {9, 8, 7};
    Array<int32> target(0);
    target = src;
    REQUIRE(target.size() == src.size());
    REQUIRE(target == src);

    Array<int32> m = std::move(target);
    REQUIRE(m.size() == 3);
}
