#include <catch2/catch_all.hpp>

#include "licht/core/collection/hash_map.hpp"

using namespace licht;

TEST_CASE("Check insertion.", "[HashMap::insert]") {

	HashMap<uint32, uint32> map;

    map.insert(5, 8);
    REQUIRE(map.get(5) == 8);
    map.insert(5, 4);
    REQUIRE(map.get(5) == 4);

    REQUIRE_FALSE(map.get_ptr(7));
    REQUIRE(*map.get_ptr(5) == 4);
    
    map.insert(4141, 26);
    map.insert(874, 11);
    map.insert(95, 20);
    map.insert(66, 82);
    map.insert(10, 99);

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

    map.insert(5, 8);
    map.insert(3, 4);

    REQUIRE(map.contains(5));
    REQUIRE(map.contains(3));

    REQUIRE(map.size() == 2);
}

TEST_CASE("Check if the hashmap can remove an element.", "[HashMap::remove]") {
	HashMap<uint32, uint32> map;

    map.insert(5, 8);
    map.insert(3, 4);

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
