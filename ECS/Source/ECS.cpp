#include "ECS.h"
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace ECS {

#ifndef HeadOnly
std::vector<ComponentFactory>& ComponentFactories(){
    static std::vector<ComponentFactory> factories;
    return factories;
}
#endif

#ifdef TestGetId
ComponentID SharedRegistryGetOrCreate(const char* typeName, ComponentFactory factory){
    static std::mutex registryMutex;
    static std::unordered_map<std::string, ComponentID> idsByTypeName;
    static ComponentID nextID = 0;
    std::lock_guard<std::mutex> lock(registryMutex);

    assert(typeName != nullptr && "Component type name cannot be null");
    const std::string key(typeName ? typeName : "");
    auto it = idsByTypeName.find(key);
    if(it == idsByTypeName.end()){
        if(nextID >= MaxComponents)
            throw std::length_error("ECS::MaxComponents exceeded");
        it = idsByTypeName.emplace(key, nextID++).first;
    }

    const ComponentID id = it->second;
    if(factory != nullptr){
        auto& factories = ComponentFactories();
        if(id >= factories.size())
            factories.resize(static_cast<size_t>(id) + 1, nullptr);
        if(factories[id] == nullptr)
            factories[id] = factory;
    }

    return id;
}
#endif

#ifdef UseDLLSafe
ComponentID RegisterComponentType(ComponentID* typeID, ComponentFactory factory){
    static std::mutex registrationMutex;
    static ComponentID nextID = 0;
    std::lock_guard<std::mutex> lock(registrationMutex);

    assert(typeID != nullptr);
    assert(factory != nullptr);

    if(*typeID == InvalidComponentID){
        assert(nextID < MaxComponents && "Exceeded ECS::MaxComponents");
        *typeID = nextID++;
    }

    const ComponentID id = *typeID;
    assert(id < MaxComponents && "Component ID exceeds ECS::MaxComponents");

    auto& factories = ComponentFactories();
    if(id >= factories.size())
        factories.resize(static_cast<size_t>(id) + 1, nullptr);

    // Keep the first factory: repeated calls may originate from a different module.
    if(factories[id] == nullptr)
        factories[id] = factory;

    return id;
}
#endif

} // namespace ECS
