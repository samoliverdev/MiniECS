#include "ecs_test_common.h"

TEST_F(ECSTest, GetComponent_ReturnsReference) {
    Entity e = world.CreateEntity();
    world.AddComponent<Position>(e, {1.f, 2.f});

    auto& p = world.GetComponent<Position>(e);
    ASSERT_FLOAT_EQ(p.x, 1.f);
    ASSERT_FLOAT_EQ(p.y, 2.f);

    p.x = 5.f;
    ASSERT_FLOAT_EQ(world.GetComponent<Position>(e).x, 5.f);
}
