#include "licht/core/string/string_ref.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace licht;

TEST_CASE("StringRefBase - Basic functionality", "[StringRefBase]") {
    SECTION("Default construction") {
        StringRef ref;
        REQUIRE(strcmp(ref.data(), "") == 0);
        REQUIRE(ref.empty());
        REQUIRE(ref.size() == 0);
    }

    SECTION("Construction from C-string") {
        const char* test_str = "Hello";
        StringRef ref(test_str);
        REQUIRE(ref.data() == test_str);
        REQUIRE(ref.size() == 5);
        REQUIRE_FALSE(ref.empty());
    }

    SECTION("Construction from String") {
        String str("Test");
        StringRef ref(str);
        REQUIRE(ref.data() == str.data());
        REQUIRE(ref.size() == 4);
        REQUIRE(ref == "Test");
    }

    SECTION("Copy construction and assignment") {
        StringRef original("Original");
        StringRef copy(original);
        StringRef assigned;
        assigned = original;

        REQUIRE(copy == original);
        REQUIRE(assigned == original);
    }

    SECTION("Element access") {
        StringRef ref("ABC");
        REQUIRE(ref[0] == 'A');
        REQUIRE(ref[1] == 'B');
        REQUIRE(ref[2] == 'C');
    }

    SECTION("Comparison operators") {
        StringRef ref1("Hello");
        StringRef ref2("Hello");
        StringRef ref3("World");

        REQUIRE(ref1 == ref2);
        REQUIRE_FALSE(ref1 == ref3);
        REQUIRE(ref1 == "Hello");
        REQUIRE_FALSE(ref1 == "World");
    }

    SECTION("Implicit conversion to const char*") {
        StringRef ref("Test");
        const char* cstr = ref;
        REQUIRE(std::strcmp(cstr, "Test") == 0);
    }
}

TEST_CASE("StringRefBase - Edge cases", "[StringRefBase]") {
    SECTION("Empty string") {
        licht::StringRef ref("");
        REQUIRE(ref.empty());
        REQUIRE(ref.size() == 0);
        REQUIRE(ref.data()[0] == '\0');
    }
}


TEST_CASE("StringRefBase - Output stream", "[StringRefBase][ostream]") {
    SECTION("Stream output") {
        licht::StringRef ref("StreamTest");
        std::ostringstream oss;
        oss << ref;
        REQUIRE(oss.str() == "StreamTest");
    }
}
