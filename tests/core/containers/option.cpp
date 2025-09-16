#include <catch2/catch_test_macros.hpp>

#include "licht/core/containers/option.hpp"

using namespace licht;

TEST_CASE("Option basic construction.", "[Option]") {
    Option<int> none = Option<int>::none();
    REQUIRE_FALSE(none.has_value());

    Option<int> some = Option<int>::some(42);
    REQUIRE(some.has_value());
    REQUIRE(some.unwrap() == 42);
    REQUIRE(*some == 42);
}

TEST_CASE("Option unwrap_or works.", "[Option]") {
    Option<int> none = Option<int>::none();
    REQUIRE(none.unwrap_or(99) == 99);

    Option<int> some = Option<int>::some(5);
    REQUIRE(some.unwrap_or(99) == 5);
}

TEST_CASE("Option copy and assignment.", "[Option]") {
    Option<int> a = Option<int>::some(7);
    Option<int> b = a; // copy constructor
    REQUIRE(b.has_value());
    REQUIRE(b.unwrap() == 7);

    Option<int> c;
    c = a; // copy assignment
    REQUIRE(c.has_value());
    REQUIRE(c.unwrap() == 7);

    Option<int> none = Option<int>::none();
    c = none;
    REQUIRE_FALSE(c.has_value());
}

TEST_CASE("Option with non-trivial type.", "[Option]") {
    struct A {
        int x;
        A(int v) : x(v) {}
    };

    Option<A> o1 = Option<A>::some(A(123));
    REQUIRE(o1.has_value());
    REQUIRE(o1.unwrap().x == 123);

    Option<A> o2 = o1;
    REQUIRE(o2.has_value());
    REQUIRE(o2.unwrap().x == 123);
}
