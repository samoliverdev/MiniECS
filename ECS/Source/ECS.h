#pragma once
#include <cstdint>
#include <bitset>
#include <vector>
#include <array>
#include <memory>
#include <functional>
#include <cassert>
#include <algorithm>

#define HeadOnly
//#define UseDLLSafe

#ifndef ECS_API
    #define ECS_API
#endif

namespace ECS{

////////////////////////////////

#define USE_32Bit_Types

constexpr uint16_t Invalid = 0xFFFF;

#ifdef USE_32Bit_Types
    using Entity = uint32_t;
    constexpr Entity InvalidEntity = 0xFFFFFFFFu;
    using ComponentID = uint32_t;
    constexpr Entity InvalidComponentID = 0xFFFFFFFFu;
    constexpr uint16_t MaxComponents = 2500*1;
#else
    using Entity = uint16_t;
    constexpr Entity InvalidEntity = 0xFFFF;
    using ComponentID = uint16_t;
    constexpr Entity InvalidComponentID = 0xFFFF;
    constexpr uint16_t MaxComponents = 2500*1;
#endif

#ifndef UseDLLSafe

inline ComponentID GetUniqueComponentID(){
    static ComponentID last = 0;
    return last++;
}

template<typename T>
ComponentID GetComponentID(){
    static ComponentID id = GetUniqueComponentID();
    return id;
}

#else

#define CompDefinition() static ComponentID typeID
#define CompImplment(type) static ComponentID type::typeID = InvalidComponentID

ECS_API ComponentID GetNewComponentID();

//INFO: No Tested yet!
template<typename T>
ComponentID GetComponentID(){
    return T::typeID;
}

#endif

///////////////////////////////

constexpr size_t ChunkBits = 64;
constexpr size_t NumChunks = (MaxComponents + 63) / 64;

struct Signature{
    std::array<uint64_t, NumChunks> bits{};

    void set(ComponentID id){
        bits[id / 64] |= (1ull << (id & 63));
    }

    bool test(ComponentID id) const {
        return bits[id / 64] & (1ull << (id & 63));
    }

    bool operator==(const Signature& other) const {
        for(size_t i = 0; i < NumChunks; ++i){
            if(bits[i] != other.bits[i]) return false;
        }
        return true;
    }

    bool Contains(const Signature& other) const {
        for(size_t i = 0; i < NumChunks; ++i){
            if((bits[i] & other.bits[i]) != other.bits[i]){
                return false;
            }
        }
        return true;
    }

    bool Intersects(const Signature& other) const {
        for(size_t i = 0; i < NumChunks; ++i){
            if(bits[i] & other.bits[i]){
                return true;
            }
        }
        return false;
    }

    template<typename... Cs>
    static Signature Make(){
        Signature sig;
        (sig.set(GetComponentID<Cs>()), ...);
        return sig;
    }

    template<typename... Ts>
    static Signature MakeWith(const Signature& oldSig){
        Signature sig = oldSig;
        (sig.set(GetComponentID<Ts>()), ...);
        return sig;
    }

    size_t Hash() const {
        size_t h = 1469598103934665603ull; // FNV-1a offset
        for(size_t i = 0; i < NumChunks; ++i){
            h ^= bits[i];
            h *= 1099511628211ull;
        }
        return h;
    }


};

///////////////////////////////

struct IComponentArray {
    virtual ~IComponentArray() = default;
    virtual size_t Size() = 0;
    virtual void Remove(size_t index) = 0;
    virtual void CopyElementTo(IComponentArray* dst, size_t srcIndex) = 0;

    virtual void EmplaceFromRaw(void* src) = 0;
};

template<typename T>
struct ComponentArray : IComponentArray {
    std::vector<T> data;

    size_t Size() override {
        return data.size();
    }

    void Remove(size_t index) override {
        data[index] = std::move(data.back());
        data.pop_back();
    }

    void CopyElementTo(IComponentArray* dst, size_t srcIndex) override {
        auto* d = static_cast<ComponentArray<T>*>(dst);
        d->data.push_back(data[srcIndex]);
    }

    void EmplaceFromRaw(void* src) override {
        data.push_back(std::move(*static_cast<T*>(src)));
    }
};

////////////////////////////////

using ComponentFactory = IComponentArray*(*)();

#ifdef HeadOnly
inline std::vector<ComponentFactory>& ComponentFactories(){
    static std::vector<ComponentFactory> f;
    return f;
}
#else
ECS_API std::vector<ComponentFactory>& ComponentFactories();
#endif

template<typename T>
ComponentID RegisterComponent(){
    #ifdef UseDLLSafe
    T::typeID = GetNewComponentID();
    #endif

    ComponentID id = GetComponentID<T>();
    auto& f = ComponentFactories();
    if(id >= f.size()){
        f.resize(id + 1);
    }
    f[id] = []() -> IComponentArray* { return new ComponentArray<T>(); };
    return id;
}

////////////////////////////////

struct Archetype;

struct EntityLocation{
    //Archetype* archetype = nullptr;
    //size_t index = 0;
    uint32_t index;
    Archetype* archetype = nullptr;
};

////////////////////////////////

struct Archetype{
    Signature signature;
    std::array<uint16_t, MaxComponents> mapToComponents;
    std::vector<ComponentID> componentIDs;
    std::vector<IComponentArray*> components; // size = number of components in archetype
    std::vector<Entity> entities;

    Archetype(const Signature& sig):signature(sig){
        mapToComponents.fill(Invalid);

        for(ComponentID i = 0; i < MaxComponents; ++i){
            if(signature.test(i)){
                mapToComponents[i] = components.size();
                componentIDs.push_back(i);
                components.push_back(ComponentFactories()[i]());
            }
        }
    }

    ~Archetype(){
        for(auto* c : components){
            delete c;
        }
    }

    template<typename T>
    ComponentArray<T>* Get(){
        ComponentID id = GetComponentID<T>();
        uint16_t index = mapToComponents[id];
        assert(index != Invalid && "Component not in archetype");
        return static_cast<ComponentArray<T>*>(components[index]);
    }

    template<typename T>
    ComponentArray<T>* GetFast(){
        return static_cast<ComponentArray<T>*>(
            components[mapToComponents[GetComponentID<T>()]]
        );
    }

    bool Has(ComponentID id) const {
        return mapToComponents[id] != Invalid;
    }

    size_t Size(){
        return components.empty() ? 0 : components[0]->Size();
    }

    void Remove(size_t row){
        assert(false && "I think this is bug");
        //Entity moved = entities.back();

        for(auto& c : components){
            c->Remove(row);
        }

        //entities[row] = moved;
        //entities.pop_back();
    }

    void Remove(size_t row, std::vector<EntityLocation>& locations){
        Entity moved = entities.back();

        for(auto* c : components){
            c->Remove(row);
        }

        entities[row] = moved;
        entities.pop_back();

        if(moved != InvalidEntity){
            locations[moved].index = row;
        }
    }

    void CopyRowTo(Archetype& dst, size_t row){
        for(ComponentID id : componentIDs){
            uint16_t d = dst.mapToComponents[id];
            if(d != Invalid){
                components[mapToComponents[id]]->CopyElementTo(dst.components[d], row);
            }
        }
    }
};

////////////////////////////////

struct World;

template<typename... Cs>
struct View {
    World* world;
    Signature required;

    View(World* w):world(w){
        (required.set(GetComponentID<Cs>()), ...);
    }

    template<typename Func>
    void Each(Func&& func){
        for(auto& archPtr : world->archetypes){
            Archetype& arch = *archPtr;

            if(!arch.signature.Contains(required)) continue;

            auto arrays = std::tuple{ arch.Get<Cs>()... };

            for(size_t i = 0; i < arch.Size(); ++i){
                std::apply(
                    [&](auto*... arr) {
                        func(arr->data[i]...);
                    },
                    arrays
                );
            }
        }
    }

    template<typename Func>
    void EachWithEntity(Func&& func) {
        for(auto& archPtr : world->archetypes){
            Archetype& arch = *archPtr;

            if(!arch.signature.Contains(required)) continue;

            auto arrays = std::tuple{ arch.Get<Cs>()... };

            for(size_t i = 0; i < arch.Size(); ++i){
                Entity e = arch.entities[i];
                std::apply(
                    [&](auto*... arr) {
                        func(e, arr->data[i]...);
                    },
                    arrays
                );
            }
        }
    }
};

////////////////////////////////

template<typename... Ts>
struct Exclude {};

template<typename... Ts>
constexpr Exclude<Ts...> exclude{};

template<typename... Cs>
struct ViewWithExclude{
    World* world;
    Signature include;
    Signature exclude;

    template<typename... Es>
    ViewWithExclude(World* w, Exclude<Es...>):world(w){
        (include.set(GetComponentID<Cs>()), ...);
        (exclude.set(GetComponentID<Es>()), ...);
    }

    template<typename Func>
    void Each(Func&& func){
        for(auto& archPtr : world->archetypes){
            Archetype& arch = *archPtr;

            if(!arch.signature.Contains(include)) continue;
            if(arch.signature.Intersects(exclude)) continue;

            auto arrays = std::tuple{ arch.Get<Cs>()... };

            for(size_t i = 0; i < arch.Size(); ++i){
                std::apply(
                    [&](auto*... arr){
                        func(arr->data[i]...);
                    },
                    arrays
                );
            }
        }
    }

    template<typename Func>
    void EachWithEntity(Func&& func){
        for(auto& archPtr : world->archetypes){
            Archetype& arch = *archPtr;

            if(!arch.signature.Contains(include)) continue;
            if(arch.signature.Intersects(exclude)) continue;

            auto arrays = std::tuple{ arch.Get<Cs>()... };

            for(size_t i = 0; i < arch.Size(); ++i){
                Entity e = arch.entities[i];
                std::apply(
                    [&](auto*... arr){
                        func(e, arr->data[i]...);
                    },
                    arrays
                );
            }
        }
    }
};

struct ComponentBatch{
    struct Entry{
        ComponentID id;
        void* data;
        void (*destroy)(void*);
    };

    Signature sig;
    std::vector<Entry> entries;

    template<typename T>
    void Add(T&& value){
        ComponentID id = GetComponentID<T>();
        sig.set(id);

        T* ptr = new T(std::forward<T>(value));

        entries.push_back({
            id,
            ptr,
            [](void* p) { delete static_cast<T*>(p); }
        });
    }

    void Clear(){
        for(auto& e : entries){
            e.destroy(e.data);
        }

        entries.clear();
        sig = {};
    }
};

struct World {
    Entity nextEntity = 0;
    std::vector<EntityLocation> locations;
    std::vector<std::unique_ptr<Archetype>> archetypes;
    std::vector<Entity> freeList;

    Entity CreateEntity(){
        if(!freeList.empty()){
            Entity e = freeList.back();
            freeList.pop_back();
            locations[e] = {};
            return e;
        }
        
        Entity e = nextEntity++;
        locations.resize(e+1);
        locations[e] = {};
        return e;
    }

    void DestroyEntity(Entity e){
        assert(e < locations.size());

        EntityLocation& loc = locations[e];

        // Entity already destroyed or never had components
        if(loc.archetype == nullptr){
            return;
        }

        Archetype* arch = loc.archetype;

        // Remove row from archetype (swap-remove)
        arch->Remove(loc.index, locations);

        // Invalidate entity location
        loc.archetype = nullptr;
        loc.index = Invalid;
        freeList.push_back(e);
    }

    inline bool IsValid(Entity e){
        if(e == InvalidEntity) return false;
        if(e >= locations.size()) return false;
        return locations[e].archetype != nullptr;
    }

    //////////////////
    struct ArchetypeSlot {
        Signature sig;
        Archetype* archetype = nullptr;
    };

    std::vector<ArchetypeSlot> archetypeTable;
    size_t archetypeMask = 0;
    size_t archetypeCount = 0;

    void InitArchetypeTable(size_t initialCapacity = 64){
        // must be power of two
        size_t cap = 1;
        while(cap < initialCapacity) cap <<= 1;

        archetypeTable.clear();
        archetypeTable.resize(cap);
        archetypeMask = cap - 1;
        archetypeCount = 0;
    }

    void RehashArchetypes(size_t newCapacity) {
        std::vector<ArchetypeSlot> old = std::move(archetypeTable);

        InitArchetypeTable(newCapacity);

        for(auto& slot : old){
            if(!slot.archetype) continue;

            size_t h = slot.sig.Hash() & archetypeMask;
            while(archetypeTable[h].archetype)
                h = (h + 1) & archetypeMask;

            archetypeTable[h] = slot;
            archetypeCount++;
        }
    }

    //INFO: Not full tested yet
    Archetype* GetOrCreateArchetype(const Signature& sig){
        // Lazy init
        if(archetypeTable.empty()){
            InitArchetypeTable(64);
        }

        // Resize if load factor > 70%
        if((archetypeCount + 1) * 10 >= archetypeTable.size() * 7){
            RehashArchetypes(archetypeTable.size() * 2);
        }

        size_t h = sig.Hash() & archetypeMask;

        while(true){
            ArchetypeSlot& slot = archetypeTable[h];

            if(!slot.archetype){
                // Create new archetype
                archetypes.push_back(std::make_unique<Archetype>(sig));

                slot.sig = sig;
                slot.archetype = archetypes.back().get();
                archetypeCount++;

                return slot.archetype;
            }

            if(slot.sig == sig){
                return slot.archetype;
            }

            h = (h + 1) & archetypeMask;
        }
    }
    ///////////////////
    
    Archetype* GetOrCreateArchetype2(const Signature& signature){
        //TODO: update this with std::unordered_map<Signature, Archetype*, SignatureHash>
        for(auto& a : archetypes){
            if(a->signature == signature) return a.get();
        }

        archetypes.push_back(std::make_unique<Archetype>(signature));
        return archetypes.back().get();
    }

    //INFO: Not full tested yet
    void AddBatch(Entity e, ComponentBatch& batch){
        /*std::sort(
            batch.entries.begin(),
            batch.entries.end(),
            [](auto& a, auto& b){ return a.id < b.id; }
        );*/

        EntityLocation& loc = locations[e];

        Signature oldSig;
        if(loc.archetype) oldSig = loc.archetype->signature;

        Signature newSig = oldSig;
        for(auto& ent : batch.entries){
            newSig.set(ent.id);
        }

        Archetype* dst = GetOrCreateArchetype(newSig);
        uint32_t newRow = dst->Size();

        if(loc.archetype){
            loc.archetype->CopyRowTo(*dst, loc.index);
            loc.archetype->Remove(loc.index, locations);
        }

        for(auto& ent : batch.entries){
            uint16_t idx = dst->mapToComponents[ent.id];
            assert(idx != Invalid);

            dst->components[idx]->EmplaceFromRaw(ent.data);
        }

        dst->entities.push_back(e);
        loc = { newRow, dst };

        batch.Clear();
    }

    template<typename T>
    void AddComponent(Entity e, T value = {}){
        ComponentID id = GetComponentID<T>();
        EntityLocation& loc = locations[e];

        Signature oldSig;
        if(loc.archetype != nullptr){
            oldSig = loc.archetype->signature;
            if(oldSig.test(id)){
                assert(false && "Already Contain Comp!!");
                return;
            }
        }

        Signature newSig = oldSig;
        newSig.set(id);

        Archetype* dst = GetOrCreateArchetype(newSig);

        uint32_t newRow = dst->Size();

        if(loc.archetype != nullptr){
            loc.archetype->CopyRowTo(*dst, loc.index);
            //loc.archetype->Remove(loc.index);
            loc.archetype->Remove(loc.index, locations);
        }

        dst->Get<T>()->data.push_back(value);
        dst->entities.push_back(e);

        loc = { newRow, dst };
    }

    template<typename... Ts>
    void AddMultComponent(Entity e, Ts&&... values){
        static_assert(sizeof...(Ts) > 0, "AddComponent requires at least one component");

        EntityLocation& loc = locations[e];

        Signature oldSig;
        if(loc.archetype){
            oldSig = loc.archetype->signature;

            bool alreadyHas = (oldSig.test(GetComponentID<std::decay_t<Ts>>()) || ...);
            assert(!alreadyHas && "Entity already has one of the components");
        }

        Signature newSig = Signature::MakeWith<std::decay_t<Ts>...>(oldSig);

        Archetype* dst = GetOrCreateArchetype(newSig);
        uint32_t newRow = dst->Size();

        if(loc.archetype){
            loc.archetype->CopyRowTo(*dst, loc.index);
            loc.archetype->Remove(loc.index, locations);
        }

        (
            dst->Get<std::decay_t<Ts>>()->data.emplace_back(
                std::forward<Ts>(values)
            ),
            ...
        );

        dst->entities.push_back(e);
        loc = { newRow, dst };
    }

    template<typename T>
    void RemoveComponent(Entity e) {
        EntityLocation& loc = locations[e];
        Archetype* src = loc.archetype;
        assert(src && "Entity has no components");

        ComponentID id = GetComponentID<T>();
        assert(src->Has(id) && "Entity does not have component");

        Signature oldSig = src->signature;
        Signature newSig = oldSig;
        newSig.bits[id / 64] &= ~(1ull << (id & 63));

        Archetype* dst = GetOrCreateArchetype(newSig);
        uint32_t newRow = dst->Size();

        // Copy all components except T
        src->CopyRowTo(*dst, loc.index);

        // Add entity to destination
        dst->entities.push_back(e);

        // Remove from source archetype
        src->Remove(loc.index, locations);

        // Update entity location
        loc = { newRow, dst };
    }

    template<typename T>
    bool HasComponent(Entity e){
        EntityLocation& loc = locations[e];
        Archetype* arch = loc.archetype;

        uint16_t compIndex = arch->mapToComponents[GetComponentID<T>()];
        return compIndex != Invalid;
    }

    template<typename T>
    T& GetComponent(Entity e){
        EntityLocation& loc = locations[e];
        Archetype* arch = loc.archetype;

        uint16_t compIndex = arch->mapToComponents[GetComponentID<T>()];
        assert(compIndex != Invalid);

        auto* array = arch->GetFast<T>();
        return array->data[loc.index];
    }

    template<typename... Cs>
    Signature MakeSignature(){
        Signature sig;
        (sig.set(GetComponentID<Cs>()), ...);
        return sig;
    }

    template<typename... Cs, typename Func>
    void Each(Func&& func){
        Signature required;
        (required.set(GetComponentID<Cs>()), ...);

        for(auto& archPtr : archetypes){
            Archetype& arch = *archPtr;

            //if(((arch.signature & required) != required)) continue;
            if(!arch.signature.Contains(required)) continue;

            auto arrays = std::tuple{ arch.Get<Cs>()... };

            for(size_t i = 0; i < arch.Size(); ++i){
                std::apply(
                    [&](auto*... arr){
                        func(arr->data[i]...);
                    }, 
                    arrays
                );
            }
        }
    }

    template<typename... Cs, typename Func>
    void EachWithEntity(Func&& func){
        Signature required;
        (required.set(GetComponentID<Cs>()), ...);

        for(auto& archPtr : archetypes){
            Archetype& arch = *archPtr;

            //if(((arch.signature & required) != required)) continue;
            if(!arch.signature.Contains(required)) continue;

            auto arrays = std::tuple{ arch.Get<Cs>()... };

            for(size_t i = 0; i < arch.Size(); ++i){
                Entity e = arch.entities[i];
                std::apply(
                    [&](auto*... arr){
                        func(e, arr->data[i]...);
                    }, 
                    arrays
                );
            }
        }
    }

    template<typename... Cs>
    auto GetView(){
        return View<Cs...>(this);
    }

    template<typename... Cs, typename... Es>
    auto GetViewWithExclude(Exclude<Es...>) {
        return ViewWithExclude<Cs...>{ this, Exclude<Es...>{} };
    }

};

}