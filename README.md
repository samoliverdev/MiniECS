# Mini ECS

## This is not production-ready yet

A simple ECS library made quickly to test whether I can build something smaller and faster than the EnTT ECS library.

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

[ECS] Update Systems: 11.171 ms \
[EnTT] [View] Update Systems: 33.173 ms \
[EnTT] [Group] Update Systems: 145.170 ms \
[Flecs] Update Systems: 7.970 ms

[ECS] Single Get Component: 44.943 ms \
[EnTT] Single Get Component: 110.646 ms \
[Flecs] Single Get Component: 160.516 ms

[ECS] [Single] Create Entity With Componets: 1246.050 ms \
[ECS] [Batched] Create Entity With Componets: 261.101 ms \
[EnTT] Create Entity With Componets: 315.863 ms \
[Flecs] Create Entity With Componets: 1583.299 ms \

[ECS] Destroy Entity With Componets: 46.252 ms \
[EnTT] Destroy Entity With Componets: 397.077 ms \
[Flecs] Destroy Entity With Componets: 76.986 ms

[ECS] [ArchetypeExplosion] 1000000 entities, archetypes = 65535, time = 4629.157 ms \
[ECS] [Batch] [ArchetypeExplosion] 1000000 entities, archetypes = 65536, time = 2549.546 ms \
[Flecs] [ArchetypeExplosion] 1000000 entities, archetypes = X, time = 2642.299 ms

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