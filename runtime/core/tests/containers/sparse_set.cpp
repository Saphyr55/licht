#include <catch2/catch_all.hpp>

#include "licht/core/containers/sparse_set.hpp"
#include "licht/core/string/string.hpp"

using namespace licht;

struct SparseSetCounterTest {
    static int32 constructions;
    static int32 destructions;
    int32 value;

    SparseSetCounterTest(int32 v = 0)
        : value(v) { ++constructions; }

    SparseSetCounterTest(const SparseSetCounterTest& o)
        : value(o.value) { ++constructions; }

    SparseSetCounterTest& operator=(const SparseSetCounterTest& o) {
        value = o.value;
        return *this;
    }

    ~SparseSetCounterTest() { ++destructions; }

    bool operator==(const SparseSetCounterTest& o) const { return value == o.value; }
};

int32 SparseSetCounterTest::constructions = 0;
int32 SparseSetCounterTest::destructions = 0;

TEST_CASE("Basic put, get, and remove.", "[SparseSet]") {
    SparseSet<int32, uint32_t> set;

    REQUIRE(set.empty());

    set.put(10, 100);
    set.put(20, 200);
    set.put(30, 300);

    REQUIRE(set.size() == 3);
    REQUIRE(set.contains(10));
    REQUIRE(set.contains(20));
    REQUIRE(set.contains(30));

    REQUIRE(set[10] == 100);
    REQUIRE(set[20] == 200);
    REQUIRE(set[30] == 300);

    set.put(20, 999);
    REQUIRE(set[20] == 999);

    set.remove(20);
    REQUIRE_FALSE(set.contains(20));
    REQUIRE(set.size() == 2);

    set.remove(10);
    REQUIRE(set.size() == 1);
    REQUIRE(set.contains(30));
}

TEST_CASE("Reserve, resize, and clear.", "[SparseSet]") {
    SparseSet<int32, uint32> set(0);

    set.reserve(100);
    REQUIRE(set.empty());

    set.put(1, 11);
    set.put(2, 22);
    set.put(3, 33);
    REQUIRE(set.size() == 3);

    set.resize(5);
    REQUIRE(set.size() == 5);

    set.clear();
    REQUIRE(set.empty());
}

TEST_CASE("Contains and get pointer interface", "[SparseSet]") {
    SparseSet<String, uint32> set;

    set.put(5, "apple");
    set.put(8, "banana");

    String* p1 = set.get(5);
    String* p2 = set.get(99);

    REQUIRE(p1 != nullptr);
    REQUIRE(*p1 == "apple");
    REQUIRE(p2 == nullptr);
    REQUIRE(set.contains(8));
    REQUIRE_FALSE(set.contains(42));
}

TEST_CASE("Dense array integrity and stable iteration", "[SparseSet::elements]") {
    SparseSet<int32, uint32> set;
    for (int i = 0; i < 10; ++i) {
        set.put(i, i * 10);
    }

    const auto& dense = set.elements();
    REQUIRE(dense.size() == 10);

    // Values in dense array correspond to inserted values
    for (int i = 0; i < 10; ++i) {
        REQUIRE(set.contains(i));
        REQUIRE(set[i] == i * 10);
    }

    // Removing middle element doesn't invalidate others
    set.remove(5);
    REQUIRE_FALSE(set.contains(5));
    REQUIRE(set.size() == 9);
}

TEST_CASE("Destruction and lifecycle tracking.", "[SparseSet::destruction]") {
    SparseSetCounterTest::constructions = 0;
    SparseSetCounterTest::destructions = 0;

    {
        SparseSet<SparseSetCounterTest, uint32> set;
        set.put(1, SparseSetCounterTest(10));
        set.put(2, SparseSetCounterTest(20));
        set.put(3, SparseSetCounterTest(30));
        REQUIRE(set.size() == 3);

        set.remove(2);
        REQUIRE(set.size() == 2);
    }

    REQUIRE(SparseSetCounterTest::constructions == SparseSetCounterTest::destructions);
}

TEST_CASE("Paged indexing and large sparse indices.", "[SparseSet::paging]") {
    SparseSet<int32, uint32> set(8);

    set.put(0, 10);
    set.put(15, 150);   
    set.put(63, 630);   
    set.put(128, 1280); 

    REQUIRE(set.contains(0));
    REQUIRE(set.contains(15));
    REQUIRE(set.contains(63));
    REQUIRE(set.contains(128));

    REQUIRE(set[15] == 150);
    REQUIRE(set[63] == 630);
    REQUIRE(set[128] == 1280);
}

TEST_CASE("Clear resets all sparse pages and dense data.", "[SparseSet::clear]") {
    SparseSet<int32, uint32> set;
    for (int i = 0; i < 20; ++i) {
        set.put(i, i);
    }

    REQUIRE(set.size() == 20);
    REQUIRE_FALSE(set.empty());

    set.clear();
    REQUIRE(set.empty());
    REQUIRE_FALSE(set.contains(10));
}
