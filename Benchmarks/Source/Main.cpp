#include <chrono>
#include <cstdio>

#include "ECS.h"
#include "entt/entt.hpp"
#include <flecs.h>

using namespace ECS;

struct Timer {
    using clock = std::chrono::high_resolution_clock;
    clock::time_point start;

    Timer() : start(clock::now()) {}

    double elapsed_ms() const {
        return std::chrono::duration<double, std::milli>(
            clock::now() - start
        ).count();
    }
};

struct Position { float x, y, z; };
struct Velocity { float x, y, z; };

struct Position2 { float x, y, z; };
struct Velocity2 { float x, y, z; };

struct Position3 { float x, y, z; };
struct Velocity3 { float x, y, z; };

struct Position4 { float x, y, z; };
struct Velocity4 { float x, y, z; };

constexpr uint32_t N = 1'000'000;

const char* updateSystemLabel = "Update Systems";
const char* singleGetComponentLabel = "Single Get Component";
const char* createEntityWithComponentLabel = "Create Entity With Componets";
const char* destroyEntityWithComponentLabel = "Destroy Entity With Componets";

void BenchmarkECS(){
    World world;

    for(uint32_t i = 0; i < N; ++i) {
        Entity e = world.CreateEntity();
        world.AddComponent(e, Position{1,2,3});
        world.AddComponent(e, Velocity{4,5,6});
        world.AddComponent(e, Position2{4,5,6});
        world.AddComponent(e, Velocity2{4,5,6});
        world.AddComponent(e, Position3{4,5,6});
        world.AddComponent(e, Velocity3{4,5,6});
        world.AddComponent(e, Position4{4,5,6});
        world.AddComponent(e, Velocity4{4,5,6});
    }

    Timer t;
    
    auto view = world.GetView<Position, Velocity, Position2, Velocity2, Position3, Velocity3, Position4, Velocity4>();
    view.Each([&](auto& p, const auto& v, auto& p2, const auto& v2, auto& p3, const auto& v3, auto& p4, const auto& v4){
        p.x += v.x;
        p.y += v.y;
        p.z += v.z;
        p2.x += v2.x;
        p2.y += v2.y;
        p2.z += v2.z;
        p3.x += v3.x;
        p3.y += v3.y;
        p3.z += v3.z;
        p4.x += v4.x;
        p4.y += v4.y;
        p4.z += v4.z;
    });

    double ms = t.elapsed_ms();
    std::printf("[ECS] %s: %.3f ms\n", updateSystemLabel, ms);
}

void BenchmarkECS_SingleGet(){
    World world;

    for(uint32_t i = 0; i < N; ++i){
        Entity e = world.CreateEntity();
        world.AddComponent(e, Position{1,2,3});
        world.AddComponent(e, Velocity{1,2,3});

        world.AddComponent(e, Position2{1,2,3});
        world.AddComponent(e, Velocity2{1,2,3});

        world.AddComponent(e, Position3{1,2,3});
        world.AddComponent(e, Velocity3{1,2,3});

        world.AddComponent(e, Position4{1,2,3});
        world.AddComponent(e, Velocity4{1,2,3});
    }

    Timer t;
    for(uint32_t e = 0; e < N; ++e){
        auto& p = world.GetComponent<Position>(e);
        p.x += 1.0f;
        p.y += 2.0f;
        p.z += 3.0f;

        auto& v = world.GetComponent<Velocity>(e);
        v.x += 1.0f;
        v.y += 2.0f;
        v.z += 3.0f;

        auto& p2 = world.GetComponent<Position2>(e);
        p2.x += 1.0f;
        p2.y += 2.0f;
        p2.z += 3.0f;

        auto& v2 = world.GetComponent<Velocity2>(e);
        v2.x += 1.0f;
        v2.y += 2.0f;
        v2.z += 3.0f;

        auto& p3 = world.GetComponent<Position3>(e);
        p3.x += 1.0f;
        p3.y += 2.0f;
        p3.z += 3.0f;

        auto& v3 = world.GetComponent<Velocity3>(e);
        v3.x += 1.0f;
        v3.y += 2.0f;
        v3.z += 3.0f;

        auto& p4 = world.GetComponent<Position4>(e);
        p4.x += 1.0f;
        p4.y += 2.0f;
        p4.z += 3.0f;

        auto& v4 = world.GetComponent<Velocity4>(e);
        v4.x += 1.0f;
        v4.y += 2.0f;
        v4.z += 3.0f;
    }

    double ms = t.elapsed_ms();
    std::printf("[ECS] %s: %.3f ms\n", singleGetComponentLabel, ms);
}

void BenchmarkECS_CreateEntityWithComponets(){
    World world;

    Timer t;
    for(uint32_t i = 0; i < N; ++i) {
        Entity e = world.CreateEntity();

        #if 0
        //Slow
        world.AddComponent(e, Position{1,2,3});
        world.AddComponent(e, Velocity{4,5,6});
        world.AddComponent(e, Position2{4,5,6});
        world.AddComponent(e, Velocity2{4,5,6});
        world.AddComponent(e, Position3{4,5,6});
        world.AddComponent(e, Velocity3{4,5,6});
        world.AddComponent(e, Position4{4,5,6});
        world.AddComponent(e, Velocity4{4,5,6});
        #else 
        //Fast
        world.AddMultComponent(
            e, 
            Position{1,2,3}, 
            Velocity{4,5,6},
            Position2{1,2,3}, 
            Velocity2{4,5,6},
            Position3{1,2,3}, 
            Velocity3{4,5,6},
            Position4{1,2,3}, 
            Velocity4{4,5,6}
        );
        #endif
    }

    double ms = t.elapsed_ms();
    std::printf("[ECS] %s: %.3f ms\n", createEntityWithComponentLabel, ms);
}

void BenchmarkECS_DestroyEntityWithComponets(){
    World world;

    std::vector<Entity> entities;
    entities.reserve(N);

    for(uint32_t i = 0; i < N; ++i) {
        Entity e = world.CreateEntity();
        world.AddComponent(e, Position{1,2,3});
        world.AddComponent(e, Velocity{4,5,6});
        world.AddComponent(e, Position2{4,5,6});
        world.AddComponent(e, Velocity2{4,5,6});
        world.AddComponent(e, Position3{4,5,6});
        world.AddComponent(e, Velocity3{4,5,6});
        world.AddComponent(e, Position4{4,5,6});
        world.AddComponent(e, Velocity4{4,5,6});

        entities[i] = e;
    }

    Timer t;

    for(uint32_t i = 0; i < N; ++i){
        world.DestroyEntity(entities[i]);
    }

    double ms = t.elapsed_ms();
    std::printf("[ECS] %s: %.3f ms\n", destroyEntityWithComponentLabel, ms);
}

/////////////////////////////////

void BenchmarkEnTT(){
    entt::registry registry;

    for(uint32_t i = 0; i < N; ++i) {
        auto e = registry.create();
        registry.emplace<Position>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position2>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity2>(e, 4.0f, 5.0f, 6.0f);

        registry.emplace<Position3>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity3>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position4>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity4>(e, 4.0f, 5.0f, 6.0f);
    }

    Timer t;

    auto view = registry.view<Position, Velocity, Position2, Velocity2, Position3, Velocity3, Position4, Velocity4>();
    view.each([&](auto e, auto& p, auto& v, auto& p2, auto& v2, auto& p3, auto& v3, auto& p4, auto& v4){
        p.x += v.x;
        p.y += v.y;
        p.z += v.z;
        p2.x += v2.x;
        p2.y += v2.y;
        p2.z += v2.z;
        p3.x += v3.x;
        p3.y += v3.y;
        p3.z += v3.z;
        p4.x += v4.x;
        p4.y += v4.y;
        p4.z += v4.z;
    });

    double ms = t.elapsed_ms();
    std::printf("[EnTT] [View] %s: %.3f ms\n", updateSystemLabel, ms);
}

void BenchmarkEnTT_WithGroup(){
    entt::registry registry;

    for(uint32_t i = 0; i < N; ++i) {
        auto e = registry.create();
        registry.emplace<Position>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position2>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity2>(e, 4.0f, 5.0f, 6.0f);

        registry.emplace<Position3>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity3>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position4>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity4>(e, 4.0f, 5.0f, 6.0f);
    }

    Timer t;

    auto view = registry.group<Position, Velocity, Position2, Velocity2, Position3, Velocity3, Position4, Velocity4>();
    view.each([&](auto e, auto& p, auto& v, auto& p2, auto& v2, auto& p3, auto& v3, auto& p4, auto& v4){
        p.x += v.x;
        p.y += v.y;
        p.z += v.z;
        p2.x += v2.x;
        p2.y += v2.y;
        p2.z += v2.z;
        p3.x += v3.x;
        p3.y += v3.y;
        p3.z += v3.z;
        p4.x += v4.x;
        p4.y += v4.y;
        p4.z += v4.z;
    });

    double ms = t.elapsed_ms();
    std::printf("[EnTT] [Group] %s: %.3f ms\n", updateSystemLabel, ms);
}

void BenchmarkEnTT_SingleGet(){
    entt::registry registry;

    std::vector<entt::entity> entities;
    entities.reserve(N);

    for(uint32_t i = 0; i < N; ++i){
        auto e = registry.create();
        registry.emplace<Position>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity>(e, 1.0f, 2.0f, 3.0f);

        registry.emplace<Position2>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity2>(e, 1.0f, 2.0f, 3.0f);

        registry.emplace<Position3>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity3>(e, 1.0f, 2.0f, 3.0f);

        registry.emplace<Position4>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity4>(e, 1.0f, 2.0f, 3.0f);

        entities.push_back(e);
    }

    Timer t;

    for(auto e : entities){
        auto& p = registry.get<Position>(e);
        p.x += 1.0f;
        p.y += 2.0f;
        p.z += 3.0f;

        auto& v = registry.get<Velocity>(e);
        v.x += 1.0f;
        v.y += 2.0f;
        v.z += 3.0f;

        auto& p2 = registry.get<Position2>(e);
        p2.x += 1.0f;
        p2.y += 2.0f;
        p2.z += 3.0f;

        auto& v2 = registry.get<Velocity2>(e);
        v2.x += 1.0f;
        v2.y += 2.0f;
        v2.z += 3.0f;

        auto& p3 = registry.get<Position3>(e);
        p3.x += 1.0f;
        p3.y += 2.0f;
        p3.z += 3.0f;

        auto& v3 = registry.get<Velocity3>(e);
        v3.x += 1.0f;
        v3.y += 2.0f;
        v3.z += 3.0f;

        auto& p4 = registry.get<Position4>(e);
        p4.x += 1.0f;
        p4.y += 2.0f;
        p4.z += 3.0f;

        auto& v4 = registry.get<Velocity4>(e);
        v4.x += 1.0f;
        v4.y += 2.0f;
        v4.z += 3.0f;
    }

    double ms = t.elapsed_ms();
    std::printf("[EnTT] %s: %.3f ms\n", singleGetComponentLabel, ms);
}

void BenchmarkEnTT_CreateEntityWithComponets(){
    entt::registry registry;

    Timer t;
    for(uint32_t i = 0; i < N; ++i) {
        auto e = registry.create();
        registry.emplace<Position>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position2>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity2>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position3>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity3>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position4>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity4>(e, 4.0f, 5.0f, 6.0f);
    }

    double ms = t.elapsed_ms();
    std::printf("[EnTT] %s: %.3f ms\n", createEntityWithComponentLabel, ms);
}

void BenchmarkEnTT_DestroyEntityWithComponets(){
    entt::registry registry;

    std::vector<entt::entity> entities;
    entities.reserve(N);

    for(uint32_t i = 0; i < N; ++i) {
        auto e = registry.create();
        registry.emplace<Position>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position2>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity2>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position3>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity3>(e, 4.0f, 5.0f, 6.0f);
        registry.emplace<Position4>(e, 1.0f, 2.0f, 3.0f);
        registry.emplace<Velocity4>(e, 4.0f, 5.0f, 6.0f);

        entities[i] = e;
    }

    Timer t;

    for(uint32_t i = 0; i < N; ++i){
        registry.destroy(entities[i]);
    }

    double ms = t.elapsed_ms();
    std::printf("[EnTT] %s: %.3f ms\n", destroyEntityWithComponentLabel, ms);
}

/////////////////////////////////

void BenchmarkFlecs(){
    flecs::world ecs;

    // Create entities
    for(uint32_t i = 0; i < N; ++i){
        ecs.entity()
            .set<Position> ({1.0f, 2.0f, 3.0f})
            .set<Velocity> ({4.0f, 5.0f, 6.0f})
            .set<Position2>({1.0f, 2.0f, 3.0f})
            .set<Velocity2>({4.0f, 5.0f, 6.0f})
            .set<Position3>({1.0f, 2.0f, 3.0f})
            .set<Velocity3>({4.0f, 5.0f, 6.0f})
            .set<Position4>({1.0f, 2.0f, 3.0f})
            .set<Velocity4>({4.0f, 5.0f, 6.0f});
    }

    Timer t;

    // Query = closest equivalent to EnTT view
    auto q = ecs.query<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4
    >();

    q.each([](
        Position& p,  Velocity& v,
        Position2& p2, Velocity2& v2,
        Position3& p3, Velocity3& v3,
        Position4& p4, Velocity4& v4
    ) {
        p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
        p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
        p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
    });

    double ms = t.elapsed_ms();
    std::printf("[Flecs] %s: %.3f ms\n", updateSystemLabel, ms);
}

void BenchmarkFlecs_SingleGet(){
    flecs::world ecs;

    std::vector<flecs::entity> entities;
    entities.reserve(N);

    for(uint32_t i = 0; i < N; ++i){
        auto e = ecs.entity()
            .set<Position> ({1.0f, 2.0f, 3.0f})
            .set<Velocity> ({1.0f, 2.0f, 3.0f})
            .set<Position2>({1.0f, 2.0f, 3.0f})
            .set<Velocity2>({1.0f, 2.0f, 3.0f})
            .set<Position3>({1.0f, 2.0f, 3.0f})
            .set<Velocity3>({1.0f, 2.0f, 3.0f})
            .set<Position4>({1.0f, 2.0f, 3.0f})
            .set<Velocity4>({1.0f, 2.0f, 3.0f});

        entities.push_back(e);
    }

    Timer t;

    for(auto& e : entities){
        auto& p  = e.get_mut<Position>();
        auto& v  = e.get_mut<Velocity>();
        auto& p2 = e.get_mut<Position2>();
        auto& v2 = e.get_mut<Velocity2>();
        auto& p3 = e.get_mut<Position3>();
        auto& v3 = e.get_mut<Velocity3>();
        auto& p4 = e.get_mut<Position4>();
        auto& v4 = e.get_mut<Velocity4>();

        p.x  += 1.0f; p.y  += 2.0f; p.z  += 3.0f;
        v.x  += 1.0f; v.y  += 2.0f; v.z  += 3.0f;

        p2.x += 1.0f; p2.y += 2.0f; p2.z += 3.0f;
        v2.x += 1.0f; v2.y += 2.0f; v2.z += 3.0f;

        p3.x += 1.0f; p3.y += 2.0f; p3.z += 3.0f;
        v3.x += 1.0f; v3.y += 2.0f; v3.z += 3.0f;

        p4.x += 1.0f; p4.y += 2.0f; p4.z += 3.0f;
        v4.x += 1.0f; v4.y += 2.0f; v4.z += 3.0f;
    }

    double ms = t.elapsed_ms();
    std::printf("[Flecs] %s: %.3f ms\n", singleGetComponentLabel, ms);
}

void BenchmarkFlecs_CreateEntityWithComponents(){
    flecs::world ecs;

    Timer t;

    for(uint32_t i = 0; i < N; ++i){
        ecs.entity()
            .set<Position> ({1.0f, 2.0f, 3.0f})
            .set<Velocity> ({4.0f, 5.0f, 6.0f})
            .set<Position2>({1.0f, 2.0f, 3.0f})
            .set<Velocity2>({4.0f, 5.0f, 6.0f})
            .set<Position3>({1.0f, 2.0f, 3.0f})
            .set<Velocity3>({4.0f, 5.0f, 6.0f})
            .set<Position4>({1.0f, 2.0f, 3.0f})
            .set<Velocity4>({4.0f, 5.0f, 6.0f});
    }

    double ms = t.elapsed_ms();
    std::printf("[Flecs] %s: %.3f ms\n", createEntityWithComponentLabel, ms);
}

void BenchmarkFlecs_DestroyEntityWithComponents(){
    flecs::world ecs;

    std::vector<flecs::entity> entities;
    entities.reserve(N);

    for(uint32_t i = 0; i < N; ++i){
        auto e = ecs.entity()
            .set<Position> ({1.0f, 2.0f, 3.0f})
            .set<Velocity> ({4.0f, 5.0f, 6.0f})
            .set<Position2>({1.0f, 2.0f, 3.0f})
            .set<Velocity2>({4.0f, 5.0f, 6.0f})
            .set<Position3>({1.0f, 2.0f, 3.0f})
            .set<Velocity3>({4.0f, 5.0f, 6.0f})
            .set<Position4>({1.0f, 2.0f, 3.0f})
            .set<Velocity4>({4.0f, 5.0f, 6.0f});

        entities.push_back(e);
    }

    Timer t;

    for(uint32_t i = 0; i < N; ++i){
        entities[i].destruct();
    }

    double ms = t.elapsed_ms();
    std::printf("[Flecs] %s: %.3f ms\n", destroyEntityWithComponentLabel, ms);
}


int main(){
    RegisterComponent<Position>();
    RegisterComponent<Velocity>();
    RegisterComponent<Position2>();
    RegisterComponent<Velocity2>();
    RegisterComponent<Position3>();
    RegisterComponent<Velocity3>();
    RegisterComponent<Position4>();
    RegisterComponent<Velocity4>();

    //All test are using equivalent entt function comparated to my library
    //The goal is test the speed of the individual entt functions and my equivalent functions
    // So:
    // compare world.GetComponent == registry.get<Position>
    // world.GetComponent != view.get<Position>
    // world.GetComponent != group.get<Position>

    //In BenchmarkECS_CreateEntityWithComponets i use a all small "cheat" to test(AddMultComponent), what in entt i dont think is something fucntion like that
    //, but you can disable this "cheat" with "#if 1", what is more slow than entt, but AddMultComponent still valid comparation, 
    //i think very common to want add mult component at once, in any game or other project

    std::printf("\n");
    BenchmarkECS();
    BenchmarkEnTT();
    BenchmarkEnTT_WithGroup();
    BenchmarkFlecs();
    std::printf("\n");

    BenchmarkECS_SingleGet();
    BenchmarkEnTT_SingleGet();
    BenchmarkFlecs_SingleGet();
    std::printf("\n");

    BenchmarkECS_CreateEntityWithComponets();
    BenchmarkEnTT_CreateEntityWithComponets();
    BenchmarkFlecs_CreateEntityWithComponents();
    std::printf("\n");

    BenchmarkECS_DestroyEntityWithComponets();
    BenchmarkEnTT_DestroyEntityWithComponets();
    BenchmarkFlecs_DestroyEntityWithComponents();
    std::printf("\n");

    return 0;
}
