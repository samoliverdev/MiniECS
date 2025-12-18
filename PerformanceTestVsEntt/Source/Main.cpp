#include <chrono>
#include <cstdio>

#include "ECS.h"
#include "entt/entt.hpp"

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
    std::printf("[EnTT] %s: %.3f ms\n", updateSystemLabel, ms);
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

int main(){
    RegisterComponent<Position>();
    RegisterComponent<Velocity>();
    RegisterComponent<Position2>();
    RegisterComponent<Velocity2>();
    RegisterComponent<Position3>();
    RegisterComponent<Velocity3>();
    RegisterComponent<Position4>();
    RegisterComponent<Velocity4>();

    std::printf("\n");
    BenchmarkECS();
    BenchmarkEnTT();
    std::printf("\n");

    BenchmarkECS_SingleGet();
    BenchmarkEnTT_SingleGet();
    std::printf("\n");

    BenchmarkECS_CreateEntityWithComponets();
    BenchmarkEnTT_CreateEntityWithComponets();
    std::printf("\n");

    BenchmarkECS_DestroyEntityWithComponets();
    BenchmarkEnTT_DestroyEntityWithComponets();
    std::printf("\n");
}
