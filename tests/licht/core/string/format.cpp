#include "licht/core/string/format.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace licht;

TEST_CASE("vformat - Basic formatting", "[format]") {
    SECTION("Simple string formatting") {
        String result = vformat("Hello %s", "World");
        REQUIRE(result == "Hello World");
    }

    SECTION("Integer formatting") {
        String result = vformat("Number: %d", 42);
        REQUIRE(result == "Number: 42");
    }

    SECTION("Multiple arguments") {
        String result = vformat("%s %d %.2f", "Test", 123, 45.67f);
        REQUIRE(result == "Test 123 45.67");
    }

    SECTION("Float formatting") {
        String result = vformat("Pi: %.2f", 3.14159f);
        REQUIRE(result == "Pi: 3.14");
    }

    SECTION("Character formatting") {
        String result = vformat("Char: %c", 'A');
        REQUIRE(result == "Char: A");
    }
}
