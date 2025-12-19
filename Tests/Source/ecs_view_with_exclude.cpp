#include "ecs_test_common.h"

TEST_F(ECSTest, ViewWithExclude_ExcludesEntitiesWithComponent) {
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();

    world.AddComponent<Position>(e1);
    world.AddComponent<Velocity>(e1);

    world.AddComponent<Position>(e2);
    world.AddComponent<Velocity>(e2);
    world.AddComponent<Renderable>(e2); // should be excluded

    int count = 0;

    auto view = world.GetViewWithExclude<Position, Velocity>(
        ECS::exclude<Renderable>
    );

    view.Each([&](Position&, Velocity&) {
        ++count;
    });

    ASSERT_EQ(count, 1);
}

TEST_F(ECSTest, ViewWithExclude_IgnoresEntitiesMissingIncludedComponents) {
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();

    world.AddComponent<Position>(e1);
    world.AddComponent<Velocity>(e1);

    world.AddComponent<Position>(e2); // missing Velocity

    int count = 0;

    auto view = world.GetViewWithExclude<Position, Velocity>(
        ECS::exclude<Renderable>
    );

    view.Each([&](Position&, Velocity&) {
        ++count;
    });

    ASSERT_EQ(count, 1);
}

TEST_F(ECSTest, ViewWithExclude_ExcludesMultipleComponents) {
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();
    Entity e3 = world.CreateEntity();

    world.AddComponent<Position>(e1);
    world.AddComponent<Velocity>(e1);

    world.AddComponent<Position>(e2);
    world.AddComponent<Velocity>(e2);
    world.AddComponent<Renderable>(e2);

    world.AddComponent<Position>(e3);
    world.AddComponent<Velocity>(e3);
    world.AddComponent<Disabled>(e3);

    int count = 0;

    auto view = world.GetViewWithExclude<Position, Velocity>(
        ECS::exclude<Renderable, Disabled>
    );

    view.Each([&](Position&, Velocity&) {
        ++count;
    });

    ASSERT_EQ(count, 1);
}

TEST_F(ECSTest, ViewWithExclude_ReturnsZeroWhenAllExcluded) {
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();

    world.AddComponent<Position>(e1);
    world.AddComponent<Velocity>(e1);
    world.AddComponent<Renderable>(e1);

    world.AddComponent<Position>(e2);
    world.AddComponent<Velocity>(e2);
    world.AddComponent<Renderable>(e2);

    int count = 0;

    auto view = world.GetViewWithExclude<Position, Velocity>(
        ECS::exclude<Renderable>
    );

    view.Each([&](Position&, Velocity&) {
        ++count;
    });

    ASSERT_EQ(count, 0);
}
