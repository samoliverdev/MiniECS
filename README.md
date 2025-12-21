# Mini ECS

## This is not production-ready yet

A simple ECS library made quickly to test whether I can build something smaller and faster than the EnTT and Flecs ECS library.

## How to Compile

cmake -S . -B build\
cmake --build build --config Release

## Code Example

```
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
```

## Benchmarks

[ECS] Update Systems: 12.459 ms \
[EnTT] [View] Update Systems: 34.145 ms \
[EnTT] [Group] Update Systems: 147.410 ms \
[Flecs] Update Systems: 8.600 ms 

[ECS] Single Get Component: 47.751 ms \
[EnTT] Single Get Component: 107.441 ms \
[Flecs] Single Get Component: 172.845 ms

[ECS] [Single] Create Entity With Componets: 1305.615 ms \
[ECS] [Batched] Create Entity With Componets: 271.792 ms \
[EnTT] Create Entity With Componets: 357.566 ms \
[Flecs] Create Entity With Componets: 1590.492 ms

[ECS] Destroy Entity With Componets: 41.106 ms \
[EnTT] Destroy Entity With Componets: 368.698 ms \
[Flecs] Destroy Entity With Componets: 76.064 ms

[ECS] [ArchetypeExplosion] 1000000 entities, archetypes = 65535, time = 5022.878 ms \
[ECS] [Batch] [ArchetypeExplosion] 1000000 entities, archetypes = 65536, time = 2744.306 ms \
[Flecs] [Add] [ArchetypeExplosion] 1000000 entities, archetypes = X, time = 2793.100 ms \
[Flecs] [Set] [ArchetypeExplosion] 1000000 entities, archetypes = X, time = 4063.011 ms

### Benchmarks Notes

All test are using equivalent entt and flecs function comparated to my library \
The goal is test the speed of the individual entt and flecs functions and my equivalent functions \
So: \
world.GetComponent == registry.get<Position> \
world.GetComponent != view.get<Position> \
world.GetComponent != group.get<Position> 

## Goal of this ECS Library
* As fast as possible
* Small (less than 1500 lines of code), which makes it a good learning resource and very easy to modify
* Only core ECS features — the goal is not to compete with EnTT or Flecs in terms of feature count

## License
This project is licensed under the MIT License — see the LICENSE file for details.