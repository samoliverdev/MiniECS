#pragma once

// Choose the backend with a compile definition:
//   UseMyEcs (default) or UseEntt
#if defined(UseEntt) && defined(UseMyEcs)
#error "Define only one ECS backend: UseEntt or UseMyEcs"
#elif !defined(UseEntt) && !defined(UseMyEcs)
#define UseMyEcs
#endif

#include <functional>
#include <mutex>
#include <utility>
#include <vector>

#if defined(UseEntt)
#if __has_include(<entt/entt.hpp>)
#include <entt/entt.hpp>
#elif __has_include("../../Benchmarks/Source/entt/entt.hpp")
#include "../../Benchmarks/Source/entt/entt.hpp"
#else
#error "UseEntt is enabled, but entt/entt.hpp was not found. Add EnTT to the include path."
#endif
#else
#include "ECS.h"
#endif

namespace ECSFacade {

#if defined(UseEntt)
using Entity = entt::entity;
#else
using Entity = ECS::Entity;
#endif

template<typename... Components>
class View {
    friend class World;

#if defined(UseEntt)
    entt::registry* registry = nullptr;
    explicit View(entt::registry& value): registry(&value) {}
#else
    ECS::World* world = nullptr;
    explicit View(ECS::World& value): world(&value) {}
#endif

public:
    template<typename Func>
    void Each(Func&& func) {
#if defined(UseEntt)
        registry->view<Components...>().each(std::forward<Func>(func));
#else
        world->GetView<Components...>().Each(std::forward<Func>(func));
#endif
    }
};

// Engine-facing ECS API. Keep backend-specific types and calls inside this class.
class World {
#if defined(UseEntt)
    entt::registry backend;
#else
    ECS::World backend;
#endif
    std::mutex deferredMutex;
    std::vector<std::function<void()>> deferredCommands;

public:
    World() = default;
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    template<typename Component>
    void RegisterComponent() {
#if defined(UseMyEcs)
        ECS::RegisterComponent<Component>();
#endif
    }

    Entity CreateEntity() {
#if defined(UseEntt)
        return backend.create();
#else
        return backend.CreateEntity();
#endif
    }

    void DestroyEntity(Entity entity) {
#if defined(UseEntt)
        backend.destroy(entity);
#else
        backend.DestroyEntity(entity);
#endif
    }

    template<typename Component, typename... Args>
    Component& AddComponent(Entity entity, Args&&... args) {
#if defined(UseEntt)
        return backend.emplace<Component>(entity, std::forward<Args>(args)...);
#else
        backend.AddComponent<Component>(
            entity, Component{std::forward<Args>(args)...}
        );
        return backend.GetComponent<Component>(entity);
#endif
    }

    template<typename... Components>
    void AddMultComponent(Entity entity, Components&&... components) {
#if defined(UseEntt)
        (backend.emplace<std::decay_t<Components>>(
            entity, std::forward<Components>(components)
        ), ...);
#else
        backend.AddMultComponent(entity, std::forward<Components>(components)...);
#endif
    }

    template<typename Component>
    void RemoveComponent(Entity entity) {
#if defined(UseEntt)
        backend.remove<Component>(entity);
#else
        backend.RemoveComponent<Component>(entity);
#endif
    }

    template<typename Component>
    bool HasComponent(Entity entity) {
#if defined(UseEntt)
        return backend.all_of<Component>(entity);
#else
        return backend.HasComponent<Component>(entity);
#endif
    }

    template<typename Component>
    Component& GetComponent(Entity entity) {
#if defined(UseEntt)
        return backend.get<Component>(entity);
#else
        return backend.GetComponent<Component>(entity);
#endif
    }

    template<typename... Components>
    View<Components...> GetView() {
#if defined(UseEntt)
        return View<Components...>(backend);
#else
        return View<Components...>(backend);
#endif
    }

    // Structural edits queued during a view iteration can be applied afterward.
    // Commands queued while playback is running are applied on the next playback.
    template<typename Func>
    void Defer(Func&& command) {
        std::lock_guard<std::mutex> lock(deferredMutex);
        deferredCommands.emplace_back(std::forward<Func>(command));
    }

    void PlaybackDeferred() {
        std::vector<std::function<void()>> pending;
        {
            std::lock_guard<std::mutex> lock(deferredMutex);
            pending.swap(deferredCommands);
        }

        for(auto& command : pending)
            command();
    }
};

} // namespace ECSFacade
