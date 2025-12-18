#include "ecs_test_common.h"

TEST_F(ECSTest, AddComponent_CreatesValidEntity) {
    Entity e = world.CreateEntity();
    world.AddComponent<Position>(e);

    ASSERT_TRUE(world.IsValid(e));
    ASSERT_TRUE(world.HasComponent<Position>(e));
}

TEST_F(ECSTest, AddMultipleComponents) {
    Entity e = world.CreateEntity();

    world.AddComponent<Position>(e);
    world.AddComponent<Velocity>(e);

    ASSERT_TRUE(world.HasComponent<Position>(e));
    ASSERT_TRUE(world.HasComponent<Velocity>(e));
}

TEST_F(ECSTest, RemoveComponent) {
    Entity e = world.CreateEntity();

    world.AddComponent<Position>(e);
    world.AddComponent<Velocity>(e);

    world.RemoveComponent<Velocity>(e);

    ASSERT_TRUE(world.HasComponent<Position>(e));
    ASSERT_FALSE(world.HasComponent<Velocity>(e));
}
