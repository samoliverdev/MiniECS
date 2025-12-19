#include <ECS.h>

struct Position {
    float x = 0, y = 0;
};

struct Velocity {
    float x = 0, y = 0;
};

int main(){
    ECS::RegisterComponent<Position>();
    ECS::RegisterComponent<Velocity>();

    ECS::World world;

    ECS::Entity e = world.CreateEntity();
    world.AddComponent<Position>(e, {1.0f, 2.0f});
    world.AddComponent<Velocity>(e, {1.0f, 1.0f});

    auto view = world.GetView<Position, Velocity>();
    view.Each([](auto& pos, auto& vel){
        pos.x += vel.x;
        pos.y += vel.y;
    });

    return 0;
}