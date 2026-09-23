#include "DllComponents.h"

#include <cstdio>

int main(){
    // The engine DLL owns the component ID fields and registers the factories.
    RegisterDllSampleComponents();

    ECS::World world;
    ECS::Entity entity = world.CreateEntity();
    world.AddMultComponent(entity, DllPosition{1.0f, 2.0f}, DllVelocity{3.0f, 4.0f});

    // This call is compiled in the EXE and directly reads the DLL-owned field.
    const ECS::ComponentID positionID = ECS::GetComponentID<DllPosition>();
    DllPosition& position = world.GetComponent<DllPosition>(entity);
    DllVelocity& velocity = world.GetComponent<DllVelocity>(entity);
    position.x += velocity.x;

    std::printf(
        "EXE read DLL component ID %u; Position.x = %.1f\n",
        static_cast<unsigned>(positionID),
        position.x
    );
    return position.x == 4.0f ? 0 : 1;
}
