#include <catch2/catch_all.hpp>
#include "core/collection/array.hpp"

using namespace licht;

TEST_CASE("Aooebd elements to an array.", "[Array::append]") {

	Array<uint32> items;
    REQUIRE(items.size() == 0);
    REQUIRE(items.empty());
        
    items.append(5);
    items.append(4);
    items.append(3);
    items.append(2);
    items.append(1);
    items.append(0);

	REQUIRE(items[0] == 5);
    REQUIRE(items[1] == 4);
}

TEST_CASE("Append elements to an array corresponds correctly to the current size.", "[Array::size]") {

	Array<uint32> items;
    REQUIRE(items.size() == 0);
    REQUIRE(items.empty());

    items.append(5);
    
    REQUIRE(items.size() == 1);
    REQUIRE_FALSE(items.empty());

    items.append(4);
        
    REQUIRE(items.size() == 2);

    items.append(3);
    
    REQUIRE(items.size() == 3);

    items.append(2);
    
    REQUIRE(items.size() == 4);

    items.append(1);

    REQUIRE(items.size() == 5);

    items.append(0);

    REQUIRE(items.size() == 6);
    REQUIRE_FALSE(items.empty());

    items.pop();
    items.pop();
    items.pop();

    REQUIRE(items.size() == 3);
}



