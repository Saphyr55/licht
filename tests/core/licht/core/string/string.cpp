#include <catch2/catch_all.hpp>

#include <cstring>
#include "licht/core/string/string.hpp"

using namespace licht;

TEST_CASE("StringBase - Construction", "[string]") {
    SECTION("Default construction") {
        String str;
        REQUIRE(str.size() == 0);
        REQUIRE(str.capacity() >= 0);
        REQUIRE(strcmp(str.data(), "") == 0);
    }

    SECTION("Construction with capacity") {
        String str(100);
        REQUIRE(str.size() == 0);
        REQUIRE(str.capacity() >= 100);
        REQUIRE(strcmp(str.data(), "") == 0);
    }

    SECTION("Construction from C-string") {
        String str("Hello");
        REQUIRE(str.size() == 5);
        REQUIRE(strcmp(str.data(), "Hello") == 0);
    }

    SECTION("Copy construction") {
        String original("Hello");
        String copy(original);

        REQUIRE(copy.size() == original.size());
        REQUIRE(strcmp(copy.data(), original.data()) == 0);
    }
}

TEST_CASE("StringBase - Assignment", "[string]") {
    SECTION("Copy assignment") {
        String original("Hello");
        String assigned;

        assigned = original;

        REQUIRE(assigned.size() == original.size());
        REQUIRE(strcmp(assigned.data(), original.data()) == 0);
    }

    SECTION("Self assignment") {
        String str("Hello");
        str = str;  // Should not cause issues

        REQUIRE(strcmp(str.data(), "Hello") == 0);
    }
}

TEST_CASE("StringBase - Append operations", "[string]") {
    SECTION("Append C-string") {
        String str("Hello");
        str.append(" World");

        REQUIRE(str.size() == 11);
        REQUIRE(strcmp(str.data(), "Hello World") == 0);
    }

    SECTION("Append character") {
        String str("Hello");
        str.append('!');

        REQUIRE(str.size() == 6);
        REQUIRE(strcmp(str.data(), "Hello!") == 0);
    }

    SECTION("Append another string") {
        String str1("Hello");
        String str2(" World");

        str1.append(str2);

        REQUIRE(str1.size() == 11);
        REQUIRE(strcmp(str1.data(), "Hello World") == 0);
    }

    SECTION("Multiple appends") {
        String str;
        str.append("Hello").append(' ').append("World").append('!');

        REQUIRE(str.size() == 12);
        REQUIRE(strcmp(str.data(), "Hello World!") == 0);
    }
}

TEST_CASE("StringBase - Operator += overloads", "[string]") {
    SECTION("+= with C-string") {
        String str("Hello");
        str += " World";

        REQUIRE(strcmp(str.data(), "Hello World") == 0);
    }

    SECTION("+= with character") {
        String str("Hello");
        str += '!';

        REQUIRE(strcmp(str.data(), "Hello!") == 0);
    }

    SECTION("+= with another string") {
        String str1("Hello");
        String str2(" World");

        str1 += str2;

        REQUIRE(strcmp(str1.data(), "Hello World") == 0);
    }
}

TEST_CASE("StringBase - Comparison operations", "[string]") {
    SECTION("Equality comparison") {
        String str1("Hello");
        String str2("Hello");
        String str3("World");

        REQUIRE(str1 == str2);
        REQUIRE_FALSE(str1 == str3);
    }

    SECTION("Comparison with different lengths") {
        String str1("Hello");
        String str2("Hello World");

        REQUIRE_FALSE(str1 == str2);
    }
}

TEST_CASE("StringBase - Capacity management", "[string]") {
    SECTION("Resize operation") {
        String str("Hello");
        str.resize(10);

        REQUIRE(str.size() == 10);
        REQUIRE(str.data()[10] == '\0');  // Null-terminated
    }

    SECTION("Reserve operation") {
        String str;
        size_t initial_capacity = str.capacity();

        str.reserve(100);

        REQUIRE(str.capacity() >= 100);
        REQUIRE(str.size() == 0);  // Size unchanged
        REQUIRE(strcmp(str.data(), "") == 0);
    }

    SECTION("Clear operation") {
        String str("Hello World");
        str.clear();

        REQUIRE(str.size() == 0);
        REQUIRE(strcmp(str.data(), "") == 0);
    }

    SECTION("Empty check") {
        String empty_str;
        String non_empty_str("Hello");

        REQUIRE(empty_str.empty());
        REQUIRE_FALSE(non_empty_str.empty());
    }
}

TEST_CASE("WString - Specific functionality", "[string][wstring]") {
    SECTION("WString construction") {
        WString wstr(L"Hello");
        REQUIRE(wstr.size() == 5);
        REQUIRE(wcscmp(wstr.data(), L"Hello") == 0);
    }

    SECTION("WString append") {
        WString wstr(L"Hello");
        wstr.append(L" World");

        REQUIRE(wstr.size() == 11);
        REQUIRE(wcscmp(wstr.data(), L"Hello World") == 0);
    }
}

TEST_CASE("String - Edge cases", "[string]") {
    SECTION("Empty C-string construction") {
        String str("");
        REQUIRE(str.size() == 0);
        REQUIRE(strcmp(str.data(), "") == 0);
    }

    SECTION("Append empty string") {
        String str("Hello");
        String empty;

        str.append(empty);
        REQUIRE(strcmp(str.data(), "Hello") == 0);
    }

    SECTION("Append to empty string") {
        String str;
        str.append("Hello");

        REQUIRE(strcmp(str.data(), "Hello") == 0);
    }

    SECTION("Multiple null terminators handling") {
        // This tests that the internal buffer management correctly handles
        // the removal and re-addition of null terminators
        String str;
        str.append('H')
            .append('e')
            .append('l')
            .append('l')
            .append('o');

        REQUIRE(strcmp(str.data(), "Hello") == 0);
    }
}

TEST_CASE("String - Output stream operator", "[string][ostream]") {
    SECTION("Stream output") {
        String str("Hello World");
        std::ostringstream oss;

        oss << str;

        REQUIRE(oss.str() == "Hello World");
    }

    SECTION("Empty string stream output") {
        String str;
        std::ostringstream oss;

        oss << str;

        REQUIRE(oss.str() == "");
    }
}
