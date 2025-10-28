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
    EntityRegistry reg;

    Entity e1 = reg.create();
    Entity e2 = reg.create();

    REQUIRE(e1 != e2);
    REQUIRE(reg.valid(e1));
    REQUIRE(reg.valid(e2));

    reg.destroy(e1);
    REQUIRE_FALSE(reg.valid(e1));
    REQUIRE(reg.valid(e2));
}

TEST_CASE("Add, get, and remove components.", "[EntityRegistry]") {
    EntityRegistry reg;
    Entity e = reg.create();

    REQUIRE_FALSE(reg.has<Position>(e));
    REQUIRE(reg.get<Position>(e) == nullptr);

    Position p{1.0f, 2.0f};
    reg.add<Position>(e, p);

    REQUIRE(reg.has<Position>(e));
    auto* p_ptr = reg.get<Position>(e);
    REQUIRE(p_ptr != nullptr);
    REQUIRE(p_ptr->x == 1.0f);
    REQUIRE(p_ptr->y == 2.0f);

    reg.remove<Position>(e);
    REQUIRE_FALSE(reg.has<Position>(e));
    REQUIRE(reg.get<Position>(e) == nullptr);
}

TEST_CASE("Multiple component types.", "[EntityRegistry]") {
    EntityRegistry reg;
    Entity e = reg.create();

    reg.add<Position>(e, {10, 20});
    reg.add<Velocity>(e, {1, 2});

    REQUIRE(reg.has<Position>(e));
    REQUIRE(reg.has<Velocity>(e));

    Position* pos = reg.get<Position>(e);
    Velocity* vel = reg.get<Velocity>(e);

    REQUIRE(pos->x == 10);
    REQUIRE(pos->y == 20);
    REQUIRE(vel->dx == 1);
    REQUIRE(vel->dy == 2);
}

TEST_CASE("Iterate over single components.", "[EntityRegistry]") {
    EntityRegistry reg;

    Entity e1 = reg.create();
    Entity e2 = reg.create();
    Entity e3 = reg.create();

    reg.add<Position>(e1, {1, 1});
    reg.add<Position>(e2, {2, 2});
    reg.add<Position>(e3, {3, 3});

    int sum_x = 0, sum_y = 0;
    reg.each<Position>([&](Entity e, Position& p) {
        sum_x += static_cast<int>(p.x);
        sum_y += static_cast<int>(p.y);
    });

    REQUIRE(sum_x == 6);
    REQUIRE(sum_y == 6);
}

TEST_CASE("Iterate over multiple components.", "[EntityRegistry]") {
    EntityRegistry reg;

    Entity e1 = reg.create();
    Entity e2 = reg.create();
    Entity e3 = reg.create();

    reg.add<Position>(e1, {1, 1});
    reg.add<Velocity>(e1, {10, 20});

    reg.add<Position>(e2, {2, 2});
    // e2 has no velocity

    reg.add<Position>(e3, {3, 3});
    reg.add<Velocity>(e3, {30, 40});

    float32 sum_x = 0, sum_dx = 0;
    reg.each<Position, Velocity>([&](Entity e, Position& p, Velocity& v) {
        sum_x += static_cast<float32>(p.x);
        sum_dx += static_cast<float32>(v.dx);
    });

    // Only e1 and e3 should be counted
    REQUIRE(sum_x == 1 + 3);
    REQUIRE(sum_dx == 10 + 30);
}

TEST_CASE("Destroy entities removes components.", "[EntityRegistry]") {
    EntityRegistry reg;

    Entity e1 = reg.create();
    Entity e2 = reg.create();

    reg.add<Position>(e1, {1, 1});
    reg.add<Position>(e2, {2, 2});

    reg.destroy(e1);

    REQUIRE_FALSE(reg.valid(e1));
    REQUIRE(reg.valid(e2));

    REQUIRE(reg.get<Position>(e1) == nullptr);
    REQUIRE(reg.get<Position>(e2) != nullptr);
}

TEST_CASE("Disposing registry removes all entities and components.", "[EntityRegistry]") {
    EntityRegistry reg;

    Entity e1 = reg.create();
    Entity e2 = reg.create();

    reg.add<Position>(e1, {1, 1});
    reg.add<Velocity>(e2, {5, 5});

    reg.dispose();

    REQUIRE_FALSE(reg.valid(e1));
    REQUIRE_FALSE(reg.valid(e2));

    REQUIRE(reg.get<Position>(e1) == nullptr);
    REQUIRE(reg.get<Velocity>(e2) == nullptr);
}
