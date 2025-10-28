#include <catch2/catch_all.hpp>
#include "licht/entity/registry.hpp"

using namespace licht;

struct Position {
    float x, y;
};

struct Velocity {
    float dx, dy;
};

struct Health {
    int hp;
};

TEST_CASE("Entity creation and validity.", "[EntityRegistry]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();

    REQUIRE(e1 != e2);
    REQUIRE(registry.is_valid(e1));
    REQUIRE(registry.is_valid(e2));

    registry.destroy(e1);
    REQUIRE_FALSE(registry.is_valid(e1));
    REQUIRE(registry.is_valid(e2));
}

TEST_CASE("Add, get, and remove components.", "[EntityRegistry]") {
    EntityRegistry registry;
    Entity e = registry.create();

    REQUIRE_FALSE(registry.has_component<Position>(e));
    REQUIRE(registry.get_component<Position>(e) == nullptr);

    Position p{1.0f, 2.0f};
    registry.add_component<Position>(e, p);

    REQUIRE(registry.has_component<Position>(e));
    auto* p_ptr = registry.get_component<Position>(e);
    REQUIRE(p_ptr != nullptr);
    REQUIRE(p_ptr->x == 1.0f);
    REQUIRE(p_ptr->y == 2.0f);

    registry.remove_component<Position>(e);
    REQUIRE_FALSE(registry.has_component<Position>(e));
    REQUIRE(registry.get_component<Position>(e) == nullptr);
}

TEST_CASE("Multiple component types.", "[EntityRegistry]") {
    EntityRegistry registry;
    Entity e = registry.create();

    registry.add_component<Position>(e, {10, 20});
    registry.add_component<Velocity>(e, {1, 2});

    REQUIRE(registry.has_component<Position>(e));
    REQUIRE(registry.has_component<Velocity>(e));

    Position* pos = registry.get_component<Position>(e);
    Velocity* vel = registry.get_component<Velocity>(e);

    REQUIRE(pos->x == 10);
    REQUIRE(pos->y == 20);
    REQUIRE(vel->dx == 1);
    REQUIRE(vel->dy == 2);
}

TEST_CASE("Iterate over single components.", "[EntityRegistry]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();
    Entity e3 = registry.create();

    registry.add_component<Position>(e1, {1, 1});
    registry.add_component<Position>(e2, {2, 2});
    registry.add_component<Position>(e3, {3, 3});

    int32 sum_x = 0, sum_y = 0;
    registry.for_each<Position>([&](Entity e, Position& p) -> void {
        sum_x += static_cast<int>(p.x);
        sum_y += static_cast<int>(p.y);
    });

    REQUIRE(sum_x == 6);
    REQUIRE(sum_y == 6);
}

TEST_CASE("Iterate over multiple components.", "[EntityRegistry]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();
    Entity e3 = registry.create();

    registry.add_component<Position>(e1, {1, 1});
    registry.add_component<Velocity>(e1, {10, 20});

    registry.add_component<Position>(e2, {2, 2});
    // e2 has no velocity

    registry.add_component<Position>(e3, {3, 3});
    registry.add_component<Velocity>(e3, {30, 40});

    float32 sum_x = 0, sum_dx = 0;
    registry.for_each<Position, Velocity>([&](Entity e, Position& p, Velocity& v) -> void {
        sum_x += static_cast<float32>(p.x);
        sum_dx += static_cast<float32>(v.dx);
    });

    // Only e1 and e3 should be counted
    REQUIRE(sum_x == 1 + 3);
    REQUIRE(sum_dx == 10 + 30);
}

TEST_CASE("Destroy entities removes components.", "[EntityRegistry]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();

    registry.add_component<Position>(e1, {1, 1});
    registry.add_component<Position>(e2, {2, 2});

    registry.destroy(e1);

    REQUIRE_FALSE(registry.is_valid(e1));
    REQUIRE(registry.is_valid(e2));

    REQUIRE(registry.get_component<Position>(e1) == nullptr);
    REQUIRE(registry.get_component<Position>(e2) != nullptr);
}

TEST_CASE("Disposing registry removes all entities and components.", "[EntityRegistry]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();

    registry.add_component<Position>(e1, {1, 1});
    registry.add_component<Velocity>(e2, {5, 5});

    registry.dispose();

    REQUIRE_FALSE(registry.is_valid(e1));
    REQUIRE_FALSE(registry.is_valid(e2));

    REQUIRE(registry.get_component<Position>(e1) == nullptr);
    REQUIRE(registry.get_component<Velocity>(e2) == nullptr);
}
