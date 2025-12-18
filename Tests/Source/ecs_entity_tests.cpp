#include "ecs_test_common.h"

TEST_F(ECSTest, CreateEntity_IsInvalidUntilComponentAdded) {
    Entity e = world.CreateEntity();
    ASSERT_FALSE(world.IsValid(e));
}

TEST_F(ECSTest, DestroyEntity_MakesEntityInvalid) {
    Entity e = world.CreateEntity();
    world.AddComponent<Position>(e);

    ASSERT_TRUE(world.IsValid(e));

    world.DestroyEntity(e);
    ASSERT_FALSE(world.IsValid(e));
}

TEST_F(ECSTest, DestroyEntity_ReusesEntityID) {
    Entity e1 = world.CreateEntity();
    world.AddComponent<Position>(e1);
    world.DestroyEntity(e1);

    Entity e2 = world.CreateEntity();
    ASSERT_EQ(e1, e2);
}
