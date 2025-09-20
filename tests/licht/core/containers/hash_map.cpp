#include <catch2/catch_all.hpp>

#include "licht/core/containers/hash_map.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/string.hpp"

using namespace licht;

TEST_CASE("Check insertion.", "[HashMap::put]") {

    HashMap<uint32, uint32> map;

    map.put(5, 8);
    REQUIRE(map.get(5) == 8);
    map.put(5, 4);
    REQUIRE(map.get(5) == 4);

    REQUIRE_FALSE(map.get_ptr(7));
    REQUIRE(*map.get_ptr(5) == 4);

    map.put(4141, 26);
    map.put(874, 11);
    map.put(95, 20);
    map.put(66, 82);
    map.put(10, 99);

    REQUIRE(map.get(10) == 99);
    REQUIRE(map.size() == 6);

    map[10] = 4;
    REQUIRE(map[10] == 4);
    REQUIRE(map.size() == 6);

    map[12] = 12;
    REQUIRE(map[12] == 12);
    REQUIRE(map.size() == 7);
}

TEST_CASE("Check if the hashmap contains an element.", "[HashMap::contains]") {
    HashMap<uint32, uint32> map;

    map.put(5, 8);
    map.put(3, 4);

    REQUIRE(map.contains(5));
    REQUIRE(map.contains(3));

    REQUIRE(map.size() == 2);
}

TEST_CASE("Check if the hashmap can remove an element.", "[HashMap::remove]") {
    HashMap<uint32, uint32> map;

    map.put(5, 8);
    map.put(3, 4);

    REQUIRE(map.contains(5));
    REQUIRE(map.size() == 2);

    map.remove(5);

    REQUIRE_FALSE(map.contains(5));
    REQUIRE(map.size() == 1);
}

TEST_CASE("Check is create correctly with an initiliazer list.", "[HashMap::HashMap]") {
    HashMap<uint32, uint32> map = {
        {1, 2},
        {3, 20},
        {2, 2},
        {18, 50},
        {12, 10},
        {30, 20},
        {28, 111},
        {148, 147},
        {91, 954},
        {63, 454},
        {22, 4278},
        {198, 475},
    };

    REQUIRE(map.contains(22));
    REQUIRE(map.size() == 12);

    map.remove(22);

    REQUIRE_FALSE(map.contains(22));
    REQUIRE(map.size() == 11);

    REQUIRE(map[30] == 20);

}

TEST_CASE("Copy and move semantics.", "[HashMap::copy_move]") {
    HashMap<uint32, uint32> a;
    a.put(1, 10);
    a.put(2, 20);
    a.put(3, 30);

    REQUIRE(a.get(1) == 10);
    REQUIRE(a.get(2) == 20);
    REQUIRE(a.get(3) == 30);

    SECTION("copy constructor") {
        HashMap<uint32, uint32> b(a);
        REQUIRE(b.size() == a.size());
        REQUIRE(b.get(1) == 10);
        REQUIRE(b.get(2) == 20);
        REQUIRE(b.get(3) == 30);
    }

    SECTION("copy assignment") {
        HashMap<uint32, uint32> c;
        c = a;
        REQUIRE(c.size() == a.size());
        REQUIRE(c.get(1) == 10);
        REQUIRE(c.get(2) == 20);
        REQUIRE(c.get(3) == 30);
    }

    SECTION("move constructor") {
        HashMap<uint32, uint32> tmp(a);
        HashMap<uint32, uint32> d(std::move(tmp));
        REQUIRE(d.size() == 3);
    }

    SECTION("move assignment") {
        HashMap<uint32, uint32> e;
        e = std::move(a);
        REQUIRE(e.size() == 3);
    }
}

TEST_CASE("Clear and operator at behavior.", "[HashMap::clear]") {
    HashMap<uint32, uint32> map;
    map.put(10, 100);
    map.put(20, 200);

    REQUIRE(map.size() == 2);

    map.clear();
    REQUIRE(map.size() == 0);

    // operator[] should insert a default value if missing
    uint32& v = map[50];
    REQUIRE(v == 0u);
    REQUIRE(map.size() == 1);
    v = 55;
    REQUIRE(map[50] == 55u);
}

TEST_CASE("Iterator traversal and const iteration.", "[HashMap::Iterator]") {
    HashMap<uint32, uint32> map;
    for (uint32 i = 1; i <= 10; ++i) {
        map.put(i, i * 10);
    }

    Array<uint32> keys;
    Array<uint32> values;
    for (auto& [key, value] : map) {
        keys.append(key);
        values.append(value);
    }

    REQUIRE(keys.size() == map.size());
    REQUIRE(values.size() == map.size());

    const HashMap<uint32, uint32>& cmap = map;
    size_t count = 0;
    for (auto it = cmap.begin(); it != cmap.end(); ++it) {
        REQUIRE(cmap.contains(it->key));
        ++count;
    }
    REQUIRE(count == cmap.size());
}

TEST_CASE("Rehashing preserves entries for many inserts.", "[HashMap::resize_rehash]") {
    HashMap<uint32, uint32> map(4);
    const uint32 N = 200;

    for (uint32 i = 0; i < N; ++i) {
        map.put(i, i + 1000);
    }

    REQUIRE(map.size() == N);

    for (uint32 i = 0; i < N; i += 7) {
        REQUIRE(map.contains(i));
        REQUIRE(map.get(i) == i + 1000);
    }
}

TEST_CASE("Works with non-trivial value types (std::string).", "[HashMap::string_values]") {
    HashMap<uint32, String> map;
    map.put(1, String("one"));
    map.put(2, String("two"));
    map.put(3, String("three"));

    REQUIRE(map.get(1) == "one");
    REQUIRE(map.get(2) == "two");
    REQUIRE(map.get(3) == "three");

    String& s = map[4];
    REQUIRE(s.empty());
    s = "four";
    REQUIRE(map[4] == "four");

    const HashMap<uint32, String>& cmap = map;
    const String* p = cmap.get_ptr(2);
    REQUIRE(p);
    REQUIRE(*p == "two");
}

TEST_CASE("Overwrite value and repeated puts.", "[HashMap::put_overwrite]") {
    HashMap<uint32, uint32> map;
    map.put(42, 1);
    REQUIRE(map.get(42) == 1);
    map.put(42, 2);
    REQUIRE(map.get(42) == 2);
    map.put(42, 3);
    REQUIRE(map.get(42) == 3);
    REQUIRE(map.size() == 1);
}
