#include "licht/core/memory/memory_pool.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/string/string.hpp"
#include "licht/core/string/string_ref.hpp"

#include <catch2/catch_all.hpp>

using namespace licht;

struct TestObject {
    String name = "TestName";
    int value = -1;

    TestObject() = default;
    TestObject(const String& n, int v)
        : name(n)
        , value(v) {}

    ~TestObject() {
        value = 0;
        name = "";
    }
};

TEST_CASE("MemoryPool basic allocation and deallocation.", "[MemoryPool]") {
    MemoryPool<TestObject> pool;

    constexpr const size_t block_count = 4;
    pool.initialize_pool(&DefaultAllocator::get_instance(), block_count);

    SECTION("Allocate and deallocate resources") {
        TestObject* obj1 = pool.allocate_resource();
        TestObject* obj2 = pool.allocate_resource();
        
        REQUIRE(obj1 != nullptr);
        REQUIRE(obj2 != nullptr);
        REQUIRE(obj1 != obj2);

        pool.deallocate_resource(obj1);
        pool.deallocate_resource(obj2);

        TestObject* obj3 = pool.allocate_resource();
        TestObject* obj4 = pool.allocate_resource();

        REQUIRE((obj3 == obj2 || obj3 == obj1));
        REQUIRE((obj4 == obj1 || obj4 == obj2));
    }

    SECTION("Construct and delete resources.") {
        TestObject* obj = pool.new_resource("ConstructTest", 42);
        REQUIRE(obj->value == 42);
        REQUIRE(obj->name == "ConstructTest");

        pool.destroy_resource(obj);
        // Undefined behavior: the union is used to reduce the overhead of the linked list.
    }

    SECTION("Pool grows when exhausted") {
        Array<TestObject*> objs(10);
        for (int i = 0; i < 10; i++) {
            objs.append(pool.new_resource("Name", i));
        }

        for (int i = 0; i < 10; i++) {
            REQUIRE(objs[i]->value == i);
            REQUIRE(objs[i]->name == "Name");
        }

        for (auto* obj : objs) {
            pool.destroy_resource(obj);
        }
    }

    pool.dispose();
}
