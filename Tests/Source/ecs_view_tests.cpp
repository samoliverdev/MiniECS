#include "ecs_test_common.h"

TEST_F(ECSTest, View_IteratesCorrectEntities) {
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();

    world.AddComponent<Position>(e1);
    world.AddComponent<Velocity>(e1);

    world.AddComponent<Position>(e2);

    int count = 0;
    world.Each<Position, Velocity>([&](Position&, Velocity&) {
        count++;
    });

    ASSERT_EQ(count, 1);
}

TEST_F(ECSTest, ViewWithEntity_ReturnsCorrectEntity) {
    Entity e = world.CreateEntity();
    world.AddComponent<Position>(e, {3,4});
    world.AddComponent<Velocity>(e, {1,2});

    Entity found = InvalidEntity;

    world.EachWithEntity<Position, Velocity>(
        [&](Entity ent, Position&, Velocity&) {
            found = ent;
        }
    );

    ASSERT_EQ(found, e);
}
