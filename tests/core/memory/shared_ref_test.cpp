#include <catch2/catch_all.hpp>

#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref.hpp"

using namespace licht;

TEST_CASE("Construct correctely.", "[SharedRef::SharedRef]") {

	SharedRef<uint32> ptr = new_ref<uint32>(9);
    REQUIRE(ptr.is_valid());
    REQUIRE(ptr);
    REQUIRE(ptr.get_shared_reference_count() == 1);
    REQUIRE(*ptr == 9);

    ptr = new_ref<uint32>(2);
    REQUIRE(ptr.is_valid());
    REQUIRE(ptr);
    REQUIRE(ptr.get_shared_reference_count() == 1);
    REQUIRE(*ptr == 2);

    SharedRef<uint32> ptr2 = ptr;
    REQUIRE(ptr2.is_valid());
    REQUIRE(ptr2);
    REQUIRE(ptr2.get_shared_reference_count() == 2); // X
    REQUIRE(*ptr2 == 2);

    {
        SharedRef<uint32> ptr3 = ptr;
        REQUIRE(ptr3.is_valid());
        REQUIRE(ptr3);
        REQUIRE(ptr.get_shared_reference_count() == 3);
        REQUIRE(ptr2.get_shared_reference_count() == 3);
        REQUIRE(ptr3.get_shared_reference_count() == 3);
        REQUIRE(*ptr3 == 2);
    }

    REQUIRE(ptr.get_shared_reference_count() == 2);
    REQUIRE(ptr2.get_shared_reference_count() == 2);
}

TEST_CASE("Construct correctely devired from.", "[SharedRef::SharedRef]") {
    
    struct Base {
        uint32 x;
    };
    struct Derived : Base { 
    };

    SharedRef<Base> ptr = new_ref<Derived>();
    REQUIRE(ptr.is_valid());
    REQUIRE(ptr);
    REQUIRE(ptr.get_shared_reference_count() == 1);

    ptr = new_ref<Derived>();
    REQUIRE(ptr.is_valid());
    REQUIRE(ptr);
    REQUIRE(ptr.get_shared_reference_count() == 1);

    SharedRef<Base> ptr2 = ptr;
    REQUIRE(ptr2.is_valid());
    REQUIRE(ptr2);
    REQUIRE(ptr2.get_shared_reference_count() == 2); // X

    {
        SharedRef<Base> ptr3 = ptr;
        REQUIRE(ptr3.is_valid());
        REQUIRE(ptr3);
        REQUIRE(ptr.get_shared_reference_count() == 3);
        REQUIRE(ptr2.get_shared_reference_count() == 3);
        REQUIRE(ptr3.get_shared_reference_count() == 3);
    }

    REQUIRE(ptr.get_shared_reference_count() == 2);
    REQUIRE(ptr2.get_shared_reference_count() == 2);
}
    
TEST_CASE("Construct correctely in an Array.", "[SharedRef::SharedRef]") {
    
    struct Base {
        uint32 x = 0;
    };

    struct Derived : Base { 
    };

    Array<SharedRef<Base>> bases;
    bases.resize(5);

    for (SharedRef<Base> base : bases) {
        REQUIRE(!base.is_valid());
    }

    Array<SharedRef<Base>> bases2;
    SharedRef<Base> pbase = new_ref<Base>(2);
    bases2.resize(5, pbase);

    for (SharedRef<Base> base : bases2) {
        REQUIRE(base.is_valid());
        REQUIRE(base->x == 2);
    }
}
