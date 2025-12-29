#include <chrono>
#include <cstdio>
#include <random>
#include <execution>

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

struct Position5 { float x, y, z; };
struct Velocity5 { float x, y, z; };
struct Position6 { float x, y, z; };
struct Velocity6 { float x, y, z; };
struct Position7 { float x, y, z; };
struct Velocity7 { float x, y, z; };
struct Position8 { float x, y, z; };
struct Velocity8 { float x, y, z; };

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
    view.Each([&](Entity e, auto& p, const auto& v, auto& p2, const auto& v2, auto& p3, const auto& v3, auto& p4, const auto& v4){
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
    /*for(auto [p, v, p2, v2, p3, v3, p4, v4]: view){
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
    };*/

    double ms = t.elapsed_ms();
    std::printf("[ECS] %s: %.3f ms\n", updateSystemLabel, ms);
}

void BenchmarkECS_Thread(){
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

    tf::Executor executor;
    tf::Taskflow tf;

    auto view = world.GetView<Position, Velocity, Position2, Velocity2, Position3, Velocity3, Position4, Velocity4>();
    view.CachArchetypes();

    Timer t;
    
    //auto view = world.GetView<Position, Velocity, Position2, Velocity2, Position3, Velocity3, Position4, Velocity4>();
    view.EachCachedParallelBatch(tf, [&](Entity e, auto& p, const auto& v, auto& p2, const auto& v2, auto& p3, const auto& v3, auto& p4, const auto& v4){
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

    executor.run(tf).wait();

    double ms = t.elapsed_ms();
    std::printf("[ECS] [Thread] %s: %.3f ms\n", updateSystemLabel, ms);
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

        #if 1
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
    std::printf("[ECS] [Single] %s: %.3f ms\n", createEntityWithComponentLabel, ms);
}

void BenchmarkECS_CreateEntityWithComponets_Batched(){
    World world;

    Timer t;
    for(uint32_t i = 0; i < N; ++i) {
        Entity e = world.CreateEntity();
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
    }

    double ms = t.elapsed_ms();
    std::printf("[ECS] [Batched] %s: %.3f ms\n", createEntityWithComponentLabel, ms);
}

void BenchmarkECS_DestroyEntityWithComponets(){
    World world;

    std::vector<Entity> entities;
    entities.resize(N);

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

void BenchmarkECS_ArchetypeExplosion(){
    World world;

    Timer t;

    for(uint32_t i = 0; i < N; ++i){
        Entity e = world.CreateEntity();

        if(i & (1 <<  0)) world.AddComponent(e, Position{1,2,3});
        if(i & (1 <<  1)) world.AddComponent(e, Velocity{4,5,6});
        if(i & (1 <<  2)) world.AddComponent(e, Position2{1,2,3});
        if(i & (1 <<  3)) world.AddComponent(e, Velocity2{4,5,6});
        if(i & (1 <<  4)) world.AddComponent(e, Position3{1,2,3});
        if(i & (1 <<  5)) world.AddComponent(e, Velocity3{4,5,6});
        if(i & (1 <<  6)) world.AddComponent(e, Position4{1,2,3});
        if(i & (1 <<  7)) world.AddComponent(e, Velocity4{4,5,6});

        if(i & (1 <<  8)) world.AddComponent(e, Position5{1,2,3});
        if(i & (1 <<  9)) world.AddComponent(e, Velocity5{4,5,6});
        if(i & (1 << 10)) world.AddComponent(e, Position6{1,2,3});
        if(i & (1 << 11)) world.AddComponent(e, Velocity6{4,5,6});
        if(i & (1 << 12)) world.AddComponent(e, Position7{1,2,3});
        if(i & (1 << 13)) world.AddComponent(e, Velocity7{4,5,6});
        if(i & (1 << 14)) world.AddComponent(e, Position8{1,2,3});
        if(i & (1 << 15)) world.AddComponent(e, Velocity8{4,5,6});
    }

    double ms = t.elapsed_ms();

    std::printf(
        "[ECS] [ArchetypeExplosion] %u entities, archetypes = %zu, time = %.3f ms\n",
        N,
        world.archetypes.size(),
        ms
    );
}

void BenchmarkECS_ArchetypeExplosion_Batch(){
    World world;

    Timer t;

    for(uint32_t i = 0; i < N; ++i){
        Entity e = world.CreateEntity();
        ComponentBatch batch;

        if(i & (1 <<  0)) batch.Add(Position{1,2,3});
        if(i & (1 <<  1)) batch.Add(Velocity{4,5,6});
        if(i & (1 <<  2)) batch.Add(Position2{1,2,3});
        if(i & (1 <<  3)) batch.Add(Velocity2{4,5,6});
        if(i & (1 <<  4)) batch.Add(Position3{1,2,3});
        if(i & (1 <<  5)) batch.Add(Velocity3{4,5,6});
        if(i & (1 <<  6)) batch.Add(Position4{1,2,3});
        if(i & (1 <<  7)) batch.Add(Velocity4{4,5,6});

        if(i & (1 <<  8)) batch.Add(Position5{1,2,3});
        if(i & (1 <<  9)) batch.Add(Velocity5{4,5,6});
        if(i & (1 << 10)) batch.Add(Position6{1,2,3});
        if(i & (1 << 11)) batch.Add(Velocity6{4,5,6});
        if(i & (1 << 12)) batch.Add(Position7{1,2,3});
        if(i & (1 << 13)) batch.Add(Velocity7{4,5,6});
        if(i & (1 << 14)) batch.Add(Position8{1,2,3});
        if(i & (1 << 15)) batch.Add(Velocity8{4,5,6});

        world.AddBatch(e, batch);
    }

    double ms = t.elapsed_ms();

    std::printf(
        "[ECS] [Batch] [ArchetypeExplosion] %u entities, archetypes = %zu, time = %.3f ms\n",
        N,
        world.archetypes.size(),
        ms
    );
}

void BenchmarkECS_MultUpdateSystems(){
    World world;

    for(uint32_t i = 0; i < N; ++i){
        Entity e = world.CreateEntity();
        ComponentBatch batch;

        if(i & (1 <<  0)) batch.Add(Position{1,2,3});
        if(i & (1 <<  1)) batch.Add(Velocity{4,5,6});
        if(i & (1 <<  2)) batch.Add(Position2{1,2,3});
        if(i & (1 <<  3)) batch.Add(Velocity2{4,5,6});
        if(i & (1 <<  4)) batch.Add(Position3{1,2,3});
        if(i & (1 <<  5)) batch.Add(Velocity3{4,5,6});
        if(i & (1 <<  6)) batch.Add(Position4{1,2,3});
        if(i & (1 <<  7)) batch.Add(Velocity4{4,5,6});

        if(i & (1 <<  8)) batch.Add(Position5{1,2,3});
        if(i & (1 <<  9)) batch.Add(Velocity5{4,5,6});
        if(i & (1 << 10)) batch.Add(Position6{1,2,3});
        if(i & (1 << 11)) batch.Add(Velocity6{4,5,6});
        if(i & (1 << 12)) batch.Add(Position7{1,2,3});
        if(i & (1 << 13)) batch.Add(Velocity7{4,5,6});
        if(i & (1 << 14)) batch.Add(Position8{1,2,3});
        if(i & (1 << 15)) batch.Add(Velocity8{4,5,6});

        world.AddBatch(e, batch);
    }

    auto v1 = world.GetView<Position, Velocity>(); 
    auto v2 = world.GetView<Position2, Velocity2>(); 
    auto v3 = world.GetView<Position3, Velocity3>();
    auto v4 = world.GetView<Position4, Velocity4>();
    auto v5 = world.GetView<Position5, Velocity5>();
    auto v6 = world.GetView<Position, Velocity,Position2, Velocity2>();
    auto v7 = world.GetView<Position3, Velocity3,Position4, Velocity4>();
    auto v8 = world.GetView<Position, Velocity,Position3, Velocity3>(); 
    auto v9 = world.GetView<Position2, Velocity2,Position4, Velocity4>(); 
    auto v10 = world.GetView<Position5, Velocity5,Position6, Velocity6>(); 
    auto v11 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3>(); 
    auto v12 = world.GetView<Position2, Velocity2,Position3, Velocity3,Position4, Velocity4>(); 
    auto v13 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4>(); 
    auto v14 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4,Position5, Velocity5>(); 
    auto v15 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4,Position5, Velocity5,Position6, Velocity6>();

    Timer t;

    // --------------------------------------------------
    // 1–5: small signatures (hit many archetypes)
    {
        v1.Each([](Entity e, auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v2.Each([](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v3.Each([](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v4.Each([](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v5.Each([](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
    }

    // --------------------------------------------------
    // 6–10: medium signatures
    {
        v6.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v7.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v8.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v9.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v10.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
    }

    // --------------------------------------------------
    // 15: large signatures (few archetypes)
    {
        
        v11.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
        });
        v12.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
        });
        v13.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
        });
        v14.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4,
            auto& p5, const auto& v5
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
            p5.x += v5.x; p5.y += v5.y; p5.z += v5.z;
        });
        v15.Each([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4,
            auto& p5, const auto& v5,
            auto& p6, const auto& v6
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
            p5.x += v5.x; p5.y += v5.y; p5.z += v5.z;
            p6.x += v6.x; p6.y += v6.y; p6.z += v6.z;
        });
    }

    double ms = t.elapsed_ms();

    std::printf(
        "[ECS] [MultUpdateSystems] %u entities, archetypes = %zu, time = %.3f ms\n",
        N,
        world.archetypes.size(),
        ms
    );
}

void BenchmarkECS_MultUpdateSystems_Cached(){
    World world;

    for(uint32_t i = 0; i < N; ++i){
        Entity e = world.CreateEntity();
        ComponentBatch batch;

        if(i & (1 <<  0)) batch.Add(Position{1,2,3});
        if(i & (1 <<  1)) batch.Add(Velocity{4,5,6});
        if(i & (1 <<  2)) batch.Add(Position2{1,2,3});
        if(i & (1 <<  3)) batch.Add(Velocity2{4,5,6});
        if(i & (1 <<  4)) batch.Add(Position3{1,2,3});
        if(i & (1 <<  5)) batch.Add(Velocity3{4,5,6});
        if(i & (1 <<  6)) batch.Add(Position4{1,2,3});
        if(i & (1 <<  7)) batch.Add(Velocity4{4,5,6});

        if(i & (1 <<  8)) batch.Add(Position5{1,2,3});
        if(i & (1 <<  9)) batch.Add(Velocity5{4,5,6});
        if(i & (1 << 10)) batch.Add(Position6{1,2,3});
        if(i & (1 << 11)) batch.Add(Velocity6{4,5,6});
        if(i & (1 << 12)) batch.Add(Position7{1,2,3});
        if(i & (1 << 13)) batch.Add(Velocity7{4,5,6});
        if(i & (1 << 14)) batch.Add(Position8{1,2,3});
        if(i & (1 << 15)) batch.Add(Velocity8{4,5,6});

        world.AddBatch(e, batch);
    }

    auto v1 = world.GetView<Position, Velocity>(); v1.CachArchetypes();
    auto v2 = world.GetView<Position2, Velocity2>(); v2.CachArchetypes();
    auto v3 = world.GetView<Position3, Velocity3>(); v3.CachArchetypes();
    auto v4 = world.GetView<Position4, Velocity4>(); v4.CachArchetypes();
    auto v5 = world.GetView<Position5, Velocity5>(); v5.CachArchetypes();
    auto v6 = world.GetView<Position, Velocity,Position2, Velocity2>(); v6.CachArchetypes();
    auto v7 = world.GetView<Position3, Velocity3,Position4, Velocity4>(); v7.CachArchetypes();
    auto v8 = world.GetView<Position, Velocity,Position3, Velocity3>(); v8.CachArchetypes();
    auto v9 = world.GetView<Position2, Velocity2,Position4, Velocity4>(); v9.CachArchetypes();
    auto v10 = world.GetView<Position5, Velocity5,Position6, Velocity6>(); v10.CachArchetypes();
    auto v11 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3>(); v11.CachArchetypes();
    auto v12 = world.GetView<Position2, Velocity2,Position3, Velocity3,Position4, Velocity4>(); v12.CachArchetypes();
    auto v13 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4>(); v13.CachArchetypes();
    auto v14 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4,Position5, Velocity5>(); v14.CachArchetypes();
    auto v15 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4,Position5, Velocity5,Position6, Velocity6>(); v15.CachArchetypes();

    Timer t;

    // --------------------------------------------------
    // 1–5: small signatures (hit many archetypes)
    {
        v1.EachCached([](Entity e, auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v2.EachCached([](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v3.EachCached([](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v4.EachCached([](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v5.EachCached([](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
    }

    // --------------------------------------------------
    // 6–10: medium signatures
    {
        v6.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v7.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v8.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v9.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v10.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
    }

    // --------------------------------------------------
    // 15: large signatures (few archetypes)
    {
        
        v11.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
        });
        v12.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
        });
        v13.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
        });
        v14.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4,
            auto& p5, const auto& v5
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
            p5.x += v5.x; p5.y += v5.y; p5.z += v5.z;
        });
        v15.EachCached([](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4,
            auto& p5, const auto& v5,
            auto& p6, const auto& v6
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
            p5.x += v5.x; p5.y += v5.y; p5.z += v5.z;
            p6.x += v6.x; p6.y += v6.y; p6.z += v6.z;
        });
    }

    double ms = t.elapsed_ms();

    std::printf(
        "[ECS] [MultUpdateSystems][Cached] %u entities, archetypes = %zu, time = %.3f ms\n",
        N,
        world.archetypes.size(),
        ms
    );
}

// This is not safe, its only to test raw performace, in real code you will need manual sync for the all system task not write the same component in mult job tasks
void BenchmarkECS_MultUpdateSystems_Thread(){
    World world;
    tf::Executor executor;
    tf::Taskflow tf;

    for(uint32_t i = 0; i < N; ++i){
        Entity e = world.CreateEntity();
        ComponentBatch batch;

        if(i & (1 <<  0)) batch.Add(Position{1,2,3});
        if(i & (1 <<  1)) batch.Add(Velocity{4,5,6});
        if(i & (1 <<  2)) batch.Add(Position2{1,2,3});
        if(i & (1 <<  3)) batch.Add(Velocity2{4,5,6});
        if(i & (1 <<  4)) batch.Add(Position3{1,2,3});
        if(i & (1 <<  5)) batch.Add(Velocity3{4,5,6});
        if(i & (1 <<  6)) batch.Add(Position4{1,2,3});
        if(i & (1 <<  7)) batch.Add(Velocity4{4,5,6});

        if(i & (1 <<  8)) batch.Add(Position5{1,2,3});
        if(i & (1 <<  9)) batch.Add(Velocity5{4,5,6});
        if(i & (1 << 10)) batch.Add(Position6{1,2,3});
        if(i & (1 << 11)) batch.Add(Velocity6{4,5,6});
        if(i & (1 << 12)) batch.Add(Position7{1,2,3});
        if(i & (1 << 13)) batch.Add(Velocity7{4,5,6});
        if(i & (1 << 14)) batch.Add(Position8{1,2,3});
        if(i & (1 << 15)) batch.Add(Velocity8{4,5,6});

        world.AddBatch(e, batch);
    }

    auto v1 = world.GetView<Position, Velocity>(); v1.CachArchetypes();
    auto v2 = world.GetView<Position2, Velocity2>(); v2.CachArchetypes();
    auto v3 = world.GetView<Position3, Velocity3>(); v3.CachArchetypes();
    auto v4 = world.GetView<Position4, Velocity4>(); v4.CachArchetypes();
    auto v5 = world.GetView<Position5, Velocity5>(); v5.CachArchetypes();
    auto v6 = world.GetView<Position, Velocity,Position2, Velocity2>(); v6.CachArchetypes();
    auto v7 = world.GetView<Position3, Velocity3,Position4, Velocity4>(); v7.CachArchetypes();
    auto v8 = world.GetView<Position, Velocity,Position3, Velocity3>(); v8.CachArchetypes();
    auto v9 = world.GetView<Position2, Velocity2,Position4, Velocity4>(); v9.CachArchetypes();
    auto v10 = world.GetView<Position5, Velocity5,Position6, Velocity6>(); v10.CachArchetypes();
    auto v11 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3>(); v11.CachArchetypes();
    auto v12 = world.GetView<Position2, Velocity2,Position3, Velocity3,Position4, Velocity4>(); v12.CachArchetypes();
    auto v13 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4>(); v13.CachArchetypes();
    auto v14 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4,Position5, Velocity5>(); v14.CachArchetypes();
    auto v15 = world.GetView<Position, Velocity,Position2, Velocity2,Position3, Velocity3,Position4, Velocity4,Position5, Velocity5,Position6, Velocity6>(); v15.CachArchetypes();

    // --------------------------------------------------
    // 1–5: small signatures (hit many archetypes)
    {
        v1.EachCachedParallelSingle(tf, [](Entity e, auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v2.EachCachedParallelSingle(tf, [](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v3.EachCachedParallelSingle(tf, [](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v4.EachCachedParallelSingle(tf, [](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
        v5.EachCachedParallelSingle(tf, [](auto& p, const auto& v){
            p.x += v.x; p.y += v.y; p.z += v.z;
        });
    }

    // --------------------------------------------------
    // 6–10: medium signatures
    {
        v6.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v7.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v8.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v9.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
        v10.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        });
    }

    // --------------------------------------------------
    // 15: large signatures (few archetypes)
    {
        
        v11.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
        });
        v12.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
        });
        v13.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
        });
        v14.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4,
            auto& p5, const auto& v5
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
            p5.x += v5.x; p5.y += v5.y; p5.z += v5.z;
        });
        v15.EachCachedParallelSingle(tf, [](
            auto& p,  const auto& v,
            auto& p2, const auto& v2,
            auto& p3, const auto& v3,
            auto& p4, const auto& v4,
            auto& p5, const auto& v5,
            auto& p6, const auto& v6
        ){
            p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
            p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
            p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
            p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
            p5.x += v5.x; p5.y += v5.y; p5.z += v5.z;
            p6.x += v6.x; p6.y += v6.y; p6.z += v6.z;
        });
    }

    Timer t;

    executor.run(tf).wait();

    double ms = t.elapsed_ms();

    std::printf(
        "[ECS] [Thread] [MultUpdateSystems] %u entities, archetypes = %zu, time = %.3f ms\n",
        N,
        world.archetypes.size(),
        ms
    );
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
    view.each([](auto e, auto& p, auto& v, auto& p2, auto& v2, auto& p3, auto& v3, auto& p4, auto& v4){
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
    entities.resize(N);

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

void BenchmarkEnTT_MultUpdateSystems(){
    entt::registry reg;

    // --------------------------------------------------
    // Create archetype explosion
    for(uint32_t i = 0; i < N; ++i) {
        entt::entity e = reg.create();

        if(i & (1 <<  0)) reg.emplace<Position>(e, 1.0f,2.0f,3.0f);
        if(i & (1 <<  1)) reg.emplace<Velocity>(e, 4.0f,5.0f,6.0f);
        if(i & (1 <<  2)) reg.emplace<Position2>(e, 1.0f,2.0f,3.0f);
        if(i & (1 <<  3)) reg.emplace<Velocity2>(e, 4.0f,5.0f,6.0f);
        if(i & (1 <<  4)) reg.emplace<Position3>(e, 1.0f,2.0f,3.0f);
        if(i & (1 <<  5)) reg.emplace<Velocity3>(e, 4.0f,5.0f,6.0f);
        if(i & (1 <<  6)) reg.emplace<Position4>(e, 1.0f,2.0f,3.0f);
        if(i & (1 <<  7)) reg.emplace<Velocity4>(e, 4.0f,5.0f,6.0f);

        if(i & (1 <<  8)) reg.emplace<Position5>(e, 1.0f,2.0f,3.0f);
        if(i & (1 <<  9)) reg.emplace<Velocity5>(e, 4.0f,5.0f,6.0f);
        if(i & (1 << 10)) reg.emplace<Position6>(e, 1.0f,2.0f,3.0f);
        if(i & (1 << 11)) reg.emplace<Velocity6>(e, 4.0f,5.0f,6.0f);
        if(i & (1 << 12)) reg.emplace<Position7>(e, 1.0f,2.0f,3.0f);
        if(i & (1 << 13)) reg.emplace<Velocity7>(e, 4.0f,5.0f,6.0f);
        if(i & (1 << 14)) reg.emplace<Position8>(e, 1.0f,2.0f,3.0f);
        if(i & (1 << 15)) reg.emplace<Velocity8>(e, 4.0f,5.0f,6.0f);
    }

    // --------------------------------------------------
    // Cached views (EnTT internally caches pools)
    auto v1  = reg.view<Position, Velocity>();
    auto v2  = reg.view<Position2, Velocity2>();
    auto v3  = reg.view<Position3, Velocity3>();
    auto v4  = reg.view<Position4, Velocity4>();
    auto v5  = reg.view<Position5, Velocity5>();

    auto v6  = reg.view<Position, Velocity, Position2, Velocity2>();
    auto v7  = reg.view<Position3, Velocity3, Position4, Velocity4>();
    auto v8  = reg.view<Position, Velocity, Position3, Velocity3>();
    auto v9  = reg.view<Position2, Velocity2, Position4, Velocity4>();
    auto v10 = reg.view<Position5, Velocity5, Position6, Velocity6>();

    auto v11 = reg.view<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3
    >();

    auto v12 = reg.view<
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4
    >();

    auto v13 = reg.view<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4
    >();

    auto v14 = reg.view<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4,
        Position5, Velocity5
    >();

    auto v15 = reg.view<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4,
        Position5, Velocity5,
        Position6, Velocity6
    >();

    // --------------------------------------------------
    Timer t;

    // 1–5 small
    v1.each([](auto &p, const auto &v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });
    v2.each([](auto &p, const auto &v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });
    v3.each([](auto &p, const auto &v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });
    v4.each([](auto &p, const auto &v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });
    v5.each([](auto &p, const auto &v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });

    // 6–10 medium
    v6.each([](auto &p, auto &, auto &p2, auto &){
        p.x++; p2.x++;
    });
    v7.each([](auto &p, auto &, auto &p2, auto &){
        p.x++; p2.x++;
    });
    v8.each([](auto &p, auto &, auto &p2, auto &){
        p.x++; p2.x++;
    });
    v9.each([](auto &p, auto &, auto &p2, auto &){
        p.x++; p2.x++;
    });
    v10.each([](auto &p, auto &, auto &p2, auto &){
        p.x++; p2.x++;
    });

    // 11–15 large
    v11.each([](auto &p, auto &, auto &p2, auto &, auto &p3, auto &){
        p.x++; p2.x++; p3.x++;
    });
    v12.each([](auto &p, auto &, auto &p2, auto &, auto &p3, auto &){
        p.x++; p2.x++; p3.x++;
    });
    v13.each([](auto &p, auto &, auto &p2, auto &, auto &p3, auto &, auto &p4, auto &){
        p.x++; p2.x++; p3.x++; p4.x++;
    });
    v14.each([](auto &p, auto &, auto &p2, auto &, auto &p3, auto &, auto &p4, auto &, auto &p5, auto &){
        p.x++; p2.x++; p3.x++; p4.x++; p5.x++;
    });
    v15.each([](auto &p, auto &, auto &p2, auto &, auto &p3, auto &, auto &p4, auto &, auto &p5, auto &, auto &p6, auto &){
        p.x++; p2.x++; p3.x++; p4.x++; p5.x++; p6.x++;
    });

    double ms = t.elapsed_ms();

    std::printf(
        "[EnTT] [MultUpdateSystems] %u entities, archetypes = %s, time = %.3f ms\n",
        N,
        "X", //reg.storage<entt::entity>().size(),
        ms
    );
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
    auto q = ecs.query<Position, Velocity, Position2, Velocity2, Position3, Velocity3, Position4, Velocity4>();
    q.each([](Position& p,  Velocity& v, Position2& p2, Velocity2& v2, Position3& p3, Velocity3& v3, Position4& p4, Velocity4& v4){
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
    std::printf("[Flecs] [Query] %s: %.3f ms\n", updateSystemLabel, ms);
}

void BenchmarkFlecs_System(){
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

    ecs.set_threads(0);
    
    ecs.system<Position, Velocity, Position2, Velocity2, Position3, Velocity3, Position4, Velocity4>()
    .each([](Position& p,  Velocity& v, Position2& p2, Velocity2& v2, Position3& p3, Velocity3& v3, Position4& p4, Velocity4& v4){
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

    Timer t;
    ecs.progress();
    double ms = t.elapsed_ms();
    std::printf("[Flecs] [System] %s: %.3f ms\n", updateSystemLabel, ms);
}

void BenchmarkFlecs_Thread(){
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

    // Enable worker threads (N = hardware threads, or any number you want)
    ecs.set_threads(std::thread::hardware_concurrency());

    ecs.system<Position, Velocity,
            Position2, Velocity2,
            Position3, Velocity3,
            Position4, Velocity4>()
    //.kind(flecs::OnUpdate)
    .multi_threaded()
    .each([](flecs::entity,
                Position& p,  Velocity& v,
                Position2& p2, Velocity2& v2,
                Position3& p3, Velocity3& v3,
                Position4& p4, Velocity4& v4)
    {
        p.x  += v.x;  p.y  += v.y;  p.z  += v.z;
        p2.x += v2.x; p2.y += v2.y; p2.z += v2.z;
        p3.x += v3.x; p3.y += v3.y; p3.z += v3.z;
        p4.x += v4.x; p4.y += v4.y; p4.z += v4.z;
    });

    Timer t;
    ecs.progress();
    double ms = t.elapsed_ms();
    std::printf("[Flecs] [Thread] %s: %.3f ms\n", updateSystemLabel, ms);
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

        p.x += 1.0f; 
        p.y += 2.0f; 
        p.z += 3.0f;

        v.x += 1.0f; 
        v.y += 2.0f; 
        v.z += 3.0f;

        p2.x += 1.0f; 
        p2.y += 2.0f; 
        p2.z += 3.0f;
        
        v2.x += 1.0f; 
        v2.y += 2.0f; 
        v2.z += 3.0f;

        p3.x += 1.0f; 
        p3.y += 2.0f; 
        p3.z += 3.0f;
        
        v3.x += 1.0f; 
        v3.y += 2.0f; 
        v3.z += 3.0f;

        p4.x += 1.0f; 
        p4.y += 2.0f; 
        p4.z += 3.0f;
        
        v4.x += 1.0f; 
        v4.y += 2.0f; 
        v4.z += 3.0f;
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

void BenchmarkFlecs_ArchetypeExplosion_Add(){
    flecs::world world;

    Timer t;

    for(uint32_t i = 0; i < N; ++i){
        flecs::entity e = world.entity();

        if(i & (1 <<  0)) e.add<Position>();
        if(i & (1 <<  1)) e.add<Velocity>();
        if(i & (1 <<  2)) e.add<Position2>();
        if(i & (1 <<  3)) e.add<Velocity2>();
        if(i & (1 <<  4)) e.add<Position3>();
        if(i & (1 <<  5)) e.add<Velocity3>();
        if(i & (1 <<  6)) e.add<Position4>();
        if(i & (1 <<  7)) e.add<Velocity4>();

        if(i & (1 <<  8)) e.add<Position5>();
        if(i & (1 <<  9)) e.add<Velocity5>();
        if(i & (1 << 10)) e.add<Position6>();
        if(i & (1 << 11)) e.add<Velocity6>();
        if(i & (1 << 12)) e.add<Position7>();
        if(i & (1 << 13)) e.add<Velocity7>();
        if(i & (1 << 14)) e.add<Position7>();
        if(i & (1 << 15)) e.add<Velocity7>();
    }

    double ms = t.elapsed_ms();

    std::printf(
        "[Flecs] [Add] [ArchetypeExplosion] %u entities, archetypes = %s, time = %.3f ms\n",
        N,
        "X",
        ms
    );
}

void BenchmarkFlecs_ArchetypeExplosion_Set(){
    flecs::world world;

    Timer t;

    for(uint32_t i = 0; i < N; ++i){
        flecs::entity e = world.entity();

        if(i & (1 <<  0)) e.set<Position>({});
        if(i & (1 <<  1)) e.set<Velocity>({});
        if(i & (1 <<  2)) e.set<Position2>({});
        if(i & (1 <<  3)) e.set<Velocity2>({});
        if(i & (1 <<  4)) e.set<Position3>({});
        if(i & (1 <<  5)) e.set<Velocity3>({});
        if(i & (1 <<  6)) e.set<Position4>({});
        if(i & (1 <<  7)) e.set<Velocity4>({});

        if(i & (1 <<  8)) e.set<Position5>({});
        if(i & (1 <<  9)) e.set<Velocity5>({});
        if(i & (1 << 10)) e.set<Position6>({});
        if(i & (1 << 11)) e.set<Velocity6>({});
        if(i & (1 << 12)) e.set<Position7>({});
        if(i & (1 << 13)) e.set<Velocity7>({});
        if(i & (1 << 14)) e.set<Position7>({});
        if(i & (1 << 15)) e.set<Velocity7>({});
    }

    double ms = t.elapsed_ms();

    std::printf(
        "[Flecs] [Set] [ArchetypeExplosion] %u entities, archetypes = %s, time = %.3f ms\n",
        N,
        "X",
        ms
    );
}

void BenchmarkFlecs_MultUpdateSystems(){
    flecs::world world;
    world.set_threads(0);

    // --------------------------------------------------
    // Create archetype explosion
    for(uint32_t i = 0; i < N; ++i){
        auto e = world.entity();

        if(i & (1 <<  0)) e.set<Position>({1,2,3});
        if(i & (1 <<  1)) e.set<Velocity>({4,5,6});
        if(i & (1 <<  2)) e.set<Position2>({1,2,3});
        if(i & (1 <<  3)) e.set<Velocity2>({4,5,6});
        if(i & (1 <<  4)) e.set<Position3>({1,2,3});
        if(i & (1 <<  5)) e.set<Velocity3>({4,5,6});
        if(i & (1 <<  6)) e.set<Position4>({1,2,3});
        if(i & (1 <<  7)) e.set<Velocity4>({4,5,6});

        if(i & (1 <<  8)) e.set<Position5>({1,2,3});
        if(i & (1 <<  9)) e.set<Velocity5>({4,5,6});
        if(i & (1 << 10)) e.set<Position6>({1,2,3});
        if(i & (1 << 11)) e.set<Velocity6>({4,5,6});
        if(i & (1 << 12)) e.set<Position7>({1,2,3});
        if(i & (1 << 13)) e.set<Velocity7>({4,5,6});
        if(i & (1 << 14)) e.set<Position8>({1,2,3});
        if(i & (1 << 15)) e.set<Velocity8>({4,5,6});
    }

    // --------------------------------------------------
    // Register systems (15 variants)

    // 1–5 small
    world.system<Position, Velocity>()
        .each([](Position& p, const Velocity& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    world.system<Position2, Velocity2>()
        .each([](Position2& p, const Velocity2& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    world.system<Position3, Velocity3>()
        .each([](Position3& p, const Velocity3& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    world.system<Position4, Velocity4>()
        .each([](Position4& p, const Velocity4& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    world.system<Position5, Velocity5>()
        .each([](Position5& p, const Velocity5& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    // 6–10 medium
    world.system<Position, Velocity, Position2, Velocity2>()
        .each([](Position& p,const Velocity& v,
                 Position2& p2,const Velocity2& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    world.system<Position3, Velocity3, Position4, Velocity4>()
        .each([](Position3& p,const Velocity3& v,
                 Position4& p2,const Velocity4& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    world.system<Position, Velocity, Position3, Velocity3>()
        .each([](Position& p,const Velocity& v,
                 Position3& p2,const Velocity3& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    world.system<Position2, Velocity2, Position4, Velocity4>()
        .each([](Position2& p,const Velocity2& v,
                 Position4& p2,const Velocity4& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    world.system<Position5, Velocity5, Position6, Velocity6>()
        .each([](Position5& p,const Velocity5& v,
                 Position6& p2,const Velocity6& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    // 11–15 large
    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3
    >().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
    });

    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3
    >().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
    });

    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4
    >().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
    });

    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4,
        Position5, Velocity5
    >().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4,
        Position5& p5,const Velocity5& v5
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
        p5.x+=v5.x; p5.y+=v5.y; p5.z+=v5.z;
    });

    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4,
        Position5, Velocity5,
        Position6, Velocity6
    >().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4,
        Position5& p5,const Velocity5& v5,
        Position6& p6,const Velocity6& v6
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
        p5.x+=v5.x; p5.y+=v5.y; p5.z+=v5.z;
        p6.x+=v6.x; p6.y+=v6.y; p6.z+=v6.z;
    });


    // --------------------------------------------------
    // Run all systems once
    Timer t;
    world.progress();
    double ms = t.elapsed_ms();

    std::printf(
        "[Flecs] [MultUpdateSystems] %u entities, archetypes = %s, time = %.3f ms\n",
        N,
        "X",
        ms
    );
}

void BenchmarkFlecs_MultUpdateSystems_Query(){
    flecs::world world;
    world.set_threads(0);

    // --------------------------------------------------
    // Create archetype explosion
    for(uint32_t i = 0; i < N; ++i){
        auto e = world.entity();

        if(i & (1 <<  0)) e.set<Position> ({1,2,3});
        if(i & (1 <<  1)) e.set<Velocity> ({4,5,6});
        if(i & (1 <<  2)) e.set<Position2>({1,2,3});
        if(i & (1 <<  3)) e.set<Velocity2>({4,5,6});
        if(i & (1 <<  4)) e.set<Position3>({1,2,3});
        if(i & (1 <<  5)) e.set<Velocity3>({4,5,6});
        if(i & (1 <<  6)) e.set<Position4>({1,2,3});
        if(i & (1 <<  7)) e.set<Velocity4>({4,5,6});

        if(i & (1 <<  8)) e.set<Position5>({1,2,3});
        if(i & (1 <<  9)) e.set<Velocity5>({4,5,6});
        if(i & (1 << 10)) e.set<Position6>({1,2,3});
        if(i & (1 << 11)) e.set<Velocity6>({4,5,6});
        if(i & (1 << 12)) e.set<Position7>({1,2,3});
        if(i & (1 << 13)) e.set<Velocity7>({4,5,6});
        if(i & (1 << 14)) e.set<Position8>({1,2,3});
        if(i & (1 << 15)) e.set<Velocity8>({4,5,6});
    }

    // --------------------------------------------------
    // Queries (same 15 variants)

    auto q1 = world.query<Position, Velocity>();
    auto q2 = world.query<Position2, Velocity2>();
    auto q3 = world.query<Position3, Velocity3>();
    auto q4 = world.query<Position4, Velocity4>();
    auto q5 = world.query<Position5, Velocity5>();

    auto q6  = world.query<Position, Velocity, Position2, Velocity2>();
    auto q7  = world.query<Position3, Velocity3, Position4, Velocity4>();
    auto q8  = world.query<Position, Velocity, Position3, Velocity3>();
    auto q9  = world.query<Position2, Velocity2, Position4, Velocity4>();
    auto q10 = world.query<Position5, Velocity5, Position6, Velocity6>();

    auto q11 = world.query<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3>();

    auto q12 = world.query<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3>();

    auto q13 = world.query<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4>();

    auto q14 = world.query<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4,
        Position5, Velocity5>();

    auto q15 = world.query<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4,
        Position5, Velocity5,
        Position6, Velocity6>();

    // --------------------------------------------------
    // Run all queries once
    Timer t;

    q1.each([](Position& p, const Velocity& v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });

    q2.each([](Position2& p, const Velocity2& v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });

    q3.each([](Position3& p, const Velocity3& v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });

    q4.each([](Position4& p, const Velocity4& v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });

    q5.each([](Position5& p, const Velocity5& v){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
    });

    q6.each([](Position& p,const Velocity& v,
               Position2& p2,const Velocity2& v2){
        p.x+=v.x;  p.y+=v.y;  p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
    });

    q7.each([](Position3& p,const Velocity3& v,
               Position4& p2,const Velocity4& v2){
        p.x+=v.x;  p.y+=v.y;  p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
    });

    q8.each([](Position& p,const Velocity& v,
               Position3& p2,const Velocity3& v2){
        p.x+=v.x;  p.y+=v.y;  p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
    });

    q9.each([](Position2& p,const Velocity2& v,
               Position4& p2,const Velocity4& v2){
        p.x+=v.x;  p.y+=v.y;  p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
    });

    q10.each([](Position5& p,const Velocity5& v,
                Position6& p2,const Velocity6& v2){
        p.x+=v.x;  p.y+=v.y;  p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
    });

    q11.each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3){
        p.x+=v.x;   p.y+=v.y;   p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
    });

    q13.each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4){
        p.x+=v.x;   p.y+=v.y;   p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
    });

    q14.each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4,
        Position5& p5,const Velocity5& v5){
        p.x+=v.x;   p.y+=v.y;   p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
        p5.x+=v5.x; p5.y+=v5.y; p5.z+=v5.z;
    });

    q15.each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4,
        Position5& p5,const Velocity5& v5,
        Position6& p6,const Velocity6& v6){
        p.x+=v.x;   p.y+=v.y;   p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
        p5.x+=v5.x; p5.y+=v5.y; p5.z+=v5.z;
        p6.x+=v6.x; p6.y+=v6.y; p6.z+=v6.z;
    });

    double ms = t.elapsed_ms();

    std::printf(
        "[Flecs] [MultUpdateSystems][Query] %u entities, archetypes = %s, time = %.3f ms\n",
        N,
        "X",
        ms
    );
}

void BenchmarkFlecs_MultUpdateSystems_Thread(){
    flecs::world world;
    world.set_threads(std::thread::hardware_concurrency());

    // --------------------------------------------------
    // Create archetype explosion
    for(uint32_t i = 0; i < N; ++i){
        auto e = world.entity();

        if(i & (1 <<  0)) e.set<Position>({1,2,3});
        if(i & (1 <<  1)) e.set<Velocity>({4,5,6});
        if(i & (1 <<  2)) e.set<Position2>({1,2,3});
        if(i & (1 <<  3)) e.set<Velocity2>({4,5,6});
        if(i & (1 <<  4)) e.set<Position3>({1,2,3});
        if(i & (1 <<  5)) e.set<Velocity3>({4,5,6});
        if(i & (1 <<  6)) e.set<Position4>({1,2,3});
        if(i & (1 <<  7)) e.set<Velocity4>({4,5,6});

        if(i & (1 <<  8)) e.set<Position5>({1,2,3});
        if(i & (1 <<  9)) e.set<Velocity5>({4,5,6});
        if(i & (1 << 10)) e.set<Position6>({1,2,3});
        if(i & (1 << 11)) e.set<Velocity6>({4,5,6});
        if(i & (1 << 12)) e.set<Position7>({1,2,3});
        if(i & (1 << 13)) e.set<Velocity7>({4,5,6});
        if(i & (1 << 14)) e.set<Position8>({1,2,3});
        if(i & (1 << 15)) e.set<Velocity8>({4,5,6});
    }

    // --------------------------------------------------
    // Register systems (15 variants)

    // 1–5 small
    world.system<Position, Velocity>()
    .multi_threaded()
        .each([](Position& p, const Velocity& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    world.system<Position2, Velocity2>()
    .multi_threaded()
        .each([](Position2& p, const Velocity2& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    world.system<Position3, Velocity3>()
    .multi_threaded()
        .each([](Position3& p, const Velocity3& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    world.system<Position4, Velocity4>()
    .multi_threaded()
        .each([](Position4& p, const Velocity4& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    world.system<Position5, Velocity5>()
    .multi_threaded()
        .each([](Position5& p, const Velocity5& v){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
        });

    // 6–10 medium
    world.system<Position, Velocity, Position2, Velocity2>()
    .multi_threaded()
        .each([](Position& p,const Velocity& v,
                 Position2& p2,const Velocity2& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    world.system<Position3, Velocity3, Position4, Velocity4>()
    .multi_threaded()
        .each([](Position3& p,const Velocity3& v,
                 Position4& p2,const Velocity4& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    world.system<Position, Velocity, Position3, Velocity3>()
    .multi_threaded()
        .each([](Position& p,const Velocity& v,
                 Position3& p2,const Velocity3& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    world.system<Position2, Velocity2, Position4, Velocity4>()
    .multi_threaded()
        .each([](Position2& p,const Velocity2& v,
                 Position4& p2,const Velocity4& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    world.system<Position5, Velocity5, Position6, Velocity6>()
    .multi_threaded()
        .each([](Position5& p,const Velocity5& v,
                 Position6& p2,const Velocity6& v2){
            p.x+=v.x; p.y+=v.y; p.z+=v.z;
            p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        });

    // 11–15 large
    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3
    >().multi_threaded().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
    });

    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3
    >().multi_threaded().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
    });

    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4
    >().multi_threaded().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
    });

    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4,
        Position5, Velocity5
    >().multi_threaded().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4,
        Position5& p5,const Velocity5& v5
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
        p5.x+=v5.x; p5.y+=v5.y; p5.z+=v5.z;
    });

    world.system<
        Position, Velocity,
        Position2, Velocity2,
        Position3, Velocity3,
        Position4, Velocity4,
        Position5, Velocity5,
        Position6, Velocity6
    >().multi_threaded().each([](
        Position& p,const Velocity& v,
        Position2& p2,const Velocity2& v2,
        Position3& p3,const Velocity3& v3,
        Position4& p4,const Velocity4& v4,
        Position5& p5,const Velocity5& v5,
        Position6& p6,const Velocity6& v6
    ){
        p.x+=v.x; p.y+=v.y; p.z+=v.z;
        p2.x+=v2.x; p2.y+=v2.y; p2.z+=v2.z;
        p3.x+=v3.x; p3.y+=v3.y; p3.z+=v3.z;
        p4.x+=v4.x; p4.y+=v4.y; p4.z+=v4.z;
        p5.x+=v5.x; p5.y+=v5.y; p5.z+=v5.z;
        p6.x+=v6.x; p6.y+=v6.y; p6.z+=v6.z;
    });


    // --------------------------------------------------
    // Run all systems once
    Timer t;
    world.progress();
    double ms = t.elapsed_ms();

    std::printf(
        "[Flecs] [Thread] [MultUpdateSystems] %u entities, archetypes = %s, time = %.3f ms\n",
        N,
        "X",
        ms
    );
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

    RegisterComponent<Position5>();
    RegisterComponent<Velocity5>();
    RegisterComponent<Position6>();
    RegisterComponent<Velocity6>();
    RegisterComponent<Position7>();
    RegisterComponent<Velocity7>();
    RegisterComponent<Position8>();
    RegisterComponent<Velocity8>();

    //All test are using equivalent entt and flecs function comparated to my library
    //The goal is test the speed of the individual entt and flecs functions and my equivalent functions
    // So:
    // world.GetComponent == registry.get<Position>
    // world.GetComponent != view.get<Position>
    // world.GetComponent != group.get<Position>

    /*BenchmarkECS_MultUpdateSystems();
    BenchmarkECS_MultUpdateSystems_Cached();
    BenchmarkEnTT_MultUpdateSystems();
    BenchmarkFlecs_MultUpdateSystems();
    std::printf("\n");

    BenchmarkECS_MultUpdateSystems_Thread();
    //BenchmarkFlecs_MultUpdateSystems_Thread();
    std::printf("\n");
    return 0;*/

    std::printf("\n");
    BenchmarkECS();
    BenchmarkEnTT();
    BenchmarkEnTT_WithGroup();
    BenchmarkFlecs();
    BenchmarkFlecs_System();
    std::printf("\n");
    //return 0;
    
    BenchmarkECS_Thread();
    BenchmarkFlecs_Thread();
    std::printf("\n");
    //return 0;

    BenchmarkECS_SingleGet();
    BenchmarkEnTT_SingleGet();
    BenchmarkFlecs_SingleGet();
    std::printf("\n");

    BenchmarkECS_CreateEntityWithComponets();
    BenchmarkECS_CreateEntityWithComponets_Batched();
    BenchmarkEnTT_CreateEntityWithComponets();
    BenchmarkFlecs_CreateEntityWithComponents();
    std::printf("\n");

    BenchmarkECS_DestroyEntityWithComponets();
    BenchmarkEnTT_DestroyEntityWithComponets();
    BenchmarkFlecs_DestroyEntityWithComponents();
    std::printf("\n");

    BenchmarkECS_ArchetypeExplosion();
    BenchmarkECS_ArchetypeExplosion_Batch();
    BenchmarkFlecs_ArchetypeExplosion_Add();
    BenchmarkFlecs_ArchetypeExplosion_Set();
    std::printf("\n");

    BenchmarkECS_MultUpdateSystems();
    BenchmarkECS_MultUpdateSystems_Cached();
    BenchmarkEnTT_MultUpdateSystems();
    BenchmarkFlecs_MultUpdateSystems();
    BenchmarkFlecs_MultUpdateSystems_Query();
    std::printf("\n");

    BenchmarkECS_MultUpdateSystems_Thread();
    //BenchmarkFlecs_MultUpdateSystems_Thread();
    std::printf("\n");

    return 0;
}
