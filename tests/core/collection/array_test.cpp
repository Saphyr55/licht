#include <catch2/catch_all.hpp>

#include "licht/core/collection/array.hpp"
#include "licht/core/string/string.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/memory/memory.hpp"

using namespace licht;

TEST_CASE("Append elements to an array.", "[Array::append]") {

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

TEST_CASE("Append ptr elements to an array.", "[Array::append]") {

    Array<StringRef> array = {"8", "0"};

	Array<const char*> items;
    REQUIRE(items.size() == 0);
    REQUIRE(items.empty());

    items.append("5");
    items.append("4");
    items.append("3");
    items.append("2");
    items.append("1");
    items.append("0");

	REQUIRE(string_compare(items[0], "5") == 0);
    REQUIRE(string_compare(items[1], "4") == 0);

    items.append(array[0].data());

    REQUIRE(string_compare(items[6], "8") == 0);
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



