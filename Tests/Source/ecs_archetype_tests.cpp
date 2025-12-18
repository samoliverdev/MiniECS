#include "ecs_test_common.h"

TEST_F(ECSTest, AddComponent_MovesEntityBetweenArchetypes) {
    Entity e = world.CreateEntity();

    world.AddComponent<Position>(e);
    auto* arch1 = world.locations[e].archetype;

    world.AddComponent<Velocity>(e);
    auto* arch2 = world.locations[e].archetype;

    ASSERT_NE(arch1, arch2);
    ASSERT_TRUE(world.HasComponent<Position>(e));
    ASSERT_TRUE(world.HasComponent<Velocity>(e));
}

TEST_F(ECSTest, RemoveComponent_MovesEntityBack) {
    Entity e = world.CreateEntity();

    world.AddComponent<Position>(e);
    world.AddComponent<Velocity>(e);
    auto* arch2 = world.locations[e].archetype;

    world.RemoveComponent<Velocity>(e);
    auto* arch1 = world.locations[e].archetype;

    ASSERT_NE(arch1, arch2);
    ASSERT_TRUE(world.HasComponent<Position>(e));
}
