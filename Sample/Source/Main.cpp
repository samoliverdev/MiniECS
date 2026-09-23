#define UseEntt
//#define UseMyEcs
#include <ECSFacade.h>
#include <cstdio>
#include <cstdint>

struct Position {
    float x = 0, y = 0;
};

struct Velocity {
    float x = 0, y = 0;
};

int main(){
    ECSFacade::World world;
    world.RegisterComponent<Position>();
    world.RegisterComponent<Velocity>();

    ECSFacade::Entity e = world.CreateEntity();
    world.AddMultComponent(e, Position{1.0f, 2.0f}, Velocity{1.0f, 1.0f});

    auto view = world.GetView<Position, Velocity>();
    view.Each([&](ECSFacade::Entity, Position& pos, Velocity& vel){
        pos.x += vel.x;
        pos.y += vel.y;

        world.Defer([&world]{
            ECSFacade::Entity spawned = world.CreateEntity();
            world.AddMultComponent(
                spawned,
                Position{10.0f, 20.0f},
                Velocity{2.0f, 3.0f}
            );

            Position& spawnedPosition = world.GetComponent<Position>(spawned);
            spawnedPosition.x = 42.0f;
        });
    });

    // Apply queued structural changes after the view finishes iterating.
    world.PlaybackDeferred();

    view.Each([](ECSFacade::Entity entity, Position& pos, Velocity&){
        std::printf(
            "Entity %u: Position = (%.1f, %.1f)\n",
            static_cast<std::uint32_t>(entity),
            pos.x,
            pos.y
        );
    });

    return 0;
}
