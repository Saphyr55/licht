#include <catch2/catch_all.hpp>

#include "licht/core/containers/hash_set.hpp"
#include "catch2/catch_test_macros.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/string.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/trace/trace.hpp"

using namespace licht;

TEST_CASE("HashSet - Constructors and Initial State", "[HashSet]") {
    SECTION("Default Constructor") {
        HashSet<int32> set;
        REQUIRE(set.empty());
        REQUIRE(set.size() == 0);
        REQUIRE(set.capacity() == 8);
    }

    SECTION("Constructor with Initial Capacity") {
        HashSet<String> set(16);
        REQUIRE(set.capacity() == 16);
    }

    SECTION("Constructor with Initializer List") {
        HashSet<int32> set = {10, 20, 30, 20, 40};
        REQUIRE_FALSE(set.empty());
        REQUIRE(set.size() == 4);
        REQUIRE(set.contains(10));
        REQUIRE(set.contains(30));
    }
}

TEST_CASE("HashSet - Insertion and Uniqueness", "[HashSet][Insert]") {
    HashSet<int32> set;

    SECTION("Inserting New Keys") {
        HashSet<int32>::Iterator it1 = set.insert(100);
        REQUIRE(*it1 == 100);
        REQUIRE(set.size() == 1);
        
        HashSet<int32>::Iterator it2 = set.insert(200);
        REQUIRE(*it2 == 200);
        REQUIRE(set.size() == 2);
    }

    SECTION("Inserting Existing Keys (Uniqueness)") {
        set.insert(50);
        REQUIRE(set.size() == 1);

        HashSet<int32>::Iterator it = set.insert(50);
        REQUIRE(*it == 50);
        REQUIRE(set.size() == 1);
    }
}

TEST_CASE("HashSet - Containment and Lookup", "[HashSet][Contains][Find]") {
    HashSet<String> set = {"apple", "banana", "orange"};

    SECTION("Existing Key") {
        REQUIRE(set.contains("apple"));
        REQUIRE(set.find("banana") != set.end());
    }

    SECTION("Non-Existing Key") {
        REQUIRE_FALSE(set.contains("strawberry"));
        REQUIRE(set.find("strawberry") == set.end());
    }

    SECTION("Lookup in an Empty Set") {
        HashSet<int32> empty_set;
        REQUIRE_FALSE(empty_set.contains(1));
        REQUIRE(empty_set.find(1) == empty_set.end());
    }
}

TEST_CASE("HashSet - Removal", "[HashSet][Remove]") {
    HashSet<int32> set = {1, 2, 3, 4};
    REQUIRE(set.size() == 4);

    SECTION("Remove Existing Key") {
        size_t count = set.remove(3);
        REQUIRE(count == 1);
        REQUIRE(set.size() == 3);
        REQUIRE_FALSE(set.contains(3));

        count = set.remove(1);
        REQUIRE(count == 1);
        REQUIRE(set.size() == 2);
        REQUIRE_FALSE(set.contains(1));
    }

    SECTION("Remove Non-Existing Key") {
        size_t count = set.remove(99);
        REQUIRE(count == 0);
        REQUIRE(set.size() == 4);
    }

    SECTION("Remove Until Empty") {
        set.remove(1);
        set.remove(2);
        set.remove(3);
        set.remove(4);
        REQUIRE(set.empty());
        REQUIRE(set.size() == 0);
    }
}

TEST_CASE("HashSet - Copy and Move Semantics", "[HashSet][Copy][Move]") {
    HashSet<String> original = {"a", "b", "c"};

    SECTION("Copy Constructor") {
        HashSet<String> copy = original;
        
        REQUIRE(copy.size() == 3);
        REQUIRE(copy.contains("a"));
        
        original.remove("a");
        REQUIRE(original.size() == 2);
        REQUIRE(copy.size() == 3);
    }

    SECTION("Copy Assignment Operator") {
        HashSet<String> assign_copy;
        assign_copy = original;
        
        REQUIRE(assign_copy.size() == 3);
        REQUIRE(assign_copy.contains("b"));

        original.add("d");
        REQUIRE_FALSE(assign_copy.contains("d"));
    }

    SECTION("Move Constructor") {
        HashSet<String> moved = std::move(original);
        
        REQUIRE(moved.size() == 3);
        REQUIRE(moved.contains("c"));
        REQUIRE(original.empty());
    }

    SECTION("Move Assignment Operator") {
        HashSet<String> target = {"x", "y"};
        target = std::move(original);
        
        REQUIRE(target.size() == 3);
        REQUIRE(target.contains("a"));
        REQUIRE(original.empty());
    }
}

TEST_CASE("HashSet - Iteration and Clear", "[HashSet][Iterators][Clear]") {
    HashSet<int32> set = {10, 20, 30, 40};

    SECTION("Basic Iteration (begin/end)") {
        Array<int32> elements;
        for (const int32& key : set) {
            elements.push_back(key);
        }
        REQUIRE(elements.contains(30));
        REQUIRE(elements.size() == 4);
    }

    SECTION("Iteration on an Empty Set") {
        HashSet<int32> empty_set;
        int32 count = 0;
        for (auto it = empty_set.begin(); it != empty_set.end(); ++it) {
            count++;
        }
        REQUIRE(count == 0);
    }

    SECTION("Clear Method") {
        REQUIRE_FALSE(set.empty());
        size_t cap = set.capacity();
        set.clear();
        
        REQUIRE(set.empty());
        REQUIRE(set.size() == 0);
        REQUIRE(set.capacity() == cap);
        REQUIRE_FALSE(set.contains(20));
    }
}

TEST_CASE("HashSet - Complex Types", "[HashSet][String]") {
    HashSet<String> set;
    
    SECTION("Inserting Strings") {
        set.insert("hello");
        set.insert("world");
        set.add("hello");
        
        REQUIRE(set.size() == 2);
        REQUIRE(set.contains("hello"));
        REQUIRE(set.contains("world"));
    }

    SECTION("Removing Strings") {
        set.insert("key1");
        String& key2 = set.add("key2");
        REQUIRE(set.size() == 2);
        REQUIRE(key2 == "key2");

        set.remove("key1");
        REQUIRE(set.size() == 1);
        REQUIRE_FALSE(set.contains("key1"));
        REQUIRE(set.contains("key2"));
    }
}
