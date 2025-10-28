#include <catch2/catch_all.hpp>
#include "licht/entity/registry.hpp"
#include "licht/entity/view.hpp"

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

TEST_CASE("View with single component.", "[EntityView]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();
    Entity e3 = registry.create();

    registry.add_component<Position>(e1, {1.0f, 1.0f});
    registry.add_component<Position>(e2, {2.0f, 2.0f});
    registry.add_component<Velocity>(e3, {3.0f, 3.0f});

    SECTION("Basic view iteration") {
        float sum_x = 0.0f;
        auto view = registry.view<Position>();
        view.for_each([&](Entity e, Position& pos) {
            sum_x += pos.x;
        });

        REQUIRE(sum_x == 3.0f);
    }

    SECTION("View size") {
        auto view = registry.view<Position>();
        REQUIRE(view.size() == 2);
        REQUIRE_FALSE(view.empty());
    }
}

TEST_CASE("View with multiple components.", "[EntityView]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();
    Entity e3 = registry.create();
    Entity e4 = registry.create();

    registry.add_component<Position>(e1, {1.0f, 1.0f});
    registry.add_component<Velocity>(e1, {0.1f, 0.1f});
    registry.add_component<Health>(e1, {100});

    registry.add_component<Position>(e2, {2.0f, 2.0f});

    registry.add_component<Position>(e3, {3.0f, 3.0f});
    registry.add_component<Velocity>(e3, {0.3f, 0.3f});

    registry.add_component<Velocity>(e4, {0.4f, 0.4f});

    SECTION("Two components view") {
        int32 count = 0;
        float32 sum_x = 0.0f, sum_dx = 0.0f;
        
        auto view = registry.view<Position, Velocity>();
        view.for_each([&](Entity e, Position& pos, Velocity& vel) -> void {
            count++;
            sum_x += pos.x;
            sum_dx += vel.dx;
        });

        REQUIRE(count == 2);
        REQUIRE(sum_x == 4.0f);
        REQUIRE(sum_dx == 0.4f);
    }

    SECTION("Three components view") {
        int32 count = 0;
        
        auto view = registry.view<Position, Velocity, Health>();
        view.for_each([&](Entity e, Position& pos, Velocity& vel, Health& health) -> void {
            count++;
            REQUIRE(health.hp == 100);
        });

        REQUIRE(count == 1);
    }
}

TEST_CASE("View with range-based for loop.", "[EntityView]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();

    registry.add_component<Position>(e1, {1.0f, 2.0f});
    registry.add_component<Velocity>(e1, {3.0f, 4.0f});
    registry.add_component<Position>(e2, {5.0f, 6.0f});
    registry.add_component<Velocity>(e2, {7.0f, 8.0f});

    SECTION("Range-based iteration") {
        float32 sum_x = 0.0f;
        float32 sum_dx = 0.0f;
        int32 count = 0;

        for (const auto& [entity, pos, vel] : registry.view<Position, Velocity>()) {
            count++;
            sum_x += pos.x;
            sum_dx += vel.dx;
        }

        REQUIRE(count == 2);
        REQUIRE(sum_x == 6.0f);
        REQUIRE(sum_dx == 10.0f);
    }
}

TEST_CASE("View with entity modification.", "[EntityView]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();

    registry.add_component<Position>(e1, {1.0f, 1.0f});
    registry.add_component<Velocity>(e1, {0.5f, 0.5f});
    registry.add_component<Position>(e2, {2.0f, 2.0f});
    registry.add_component<Velocity>(e2, {1.0f, 1.0f});

    SECTION("Modify components in view") {
        EntityView<Position, Velocity> view = registry.view<Position, Velocity>();
        view.for_each([&](Entity e, Position& pos, Velocity& vel) {
            pos.x += vel.dx;
            pos.y += vel.dy;
        });

        REQUIRE(registry.get_component<Position>(e1)->x == 1.5f);
        REQUIRE(registry.get_component<Position>(e1)->y == 1.5f);
        REQUIRE(registry.get_component<Position>(e2)->x == 3.0f);
        REQUIRE(registry.get_component<Position>(e2)->y == 3.0f);
    }

    SECTION("Remove entities during iteration") {
        EntityView<Position> view = registry.view<Position>();
        int32 count = 0;

        view.for_each([&](Entity e, Position& pos) -> void {
            count++;
            if (e == e1) {
                registry.destroy(e1);
            }
        });

        REQUIRE(count == 1); // The count is 1 because we destroyed e1.
        REQUIRE_FALSE(registry.is_valid(e1));
        REQUIRE(registry.is_valid(e2));
    }
}

TEST_CASE("Empty view.", "[EntityView]") {
    EntityRegistry registry;

    Entity e1 = registry.create();
    registry.add_component<Velocity>(e1, {1.0f, 1.0f});

    SECTION("View with no matching entities") {
        EntityView<Position> view = registry.view<Position>();
        REQUIRE(view.empty());
        REQUIRE(view.size() == 0);

        int32 count = 0;
        view.for_each([&](Entity e, Position& pos) -> void {
            count++;
        });

        REQUIRE(count == 0);
    }

    SECTION("View with non-existent component") {
        EntityView<Position, Velocity> view = registry.view<Position, Velocity>();
        REQUIRE(view.size() == 0);
    }
}

TEST_CASE("View after entity destruction.", "[EntityView]") {
    EntityRegistry reg;

    Entity e1 = reg.create();
    Entity e2 = reg.create();

    reg.add_component<Position>(e1, {1.0f, 1.0f});
    reg.add_component<Position>(e2, {2.0f, 2.0f});

    EntityView<Position> view = reg.view<Position>();
    REQUIRE(view.size() == 2);

    reg.destroy(e1);

    REQUIRE(view.size() == 1);

    int32 count = 0;
    view.for_each([&](Entity e, Position& pos) -> void {
        count++;
        REQUIRE(e == e2);
    });

    REQUIRE(count == 1);
}