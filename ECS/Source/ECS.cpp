#include "Ecs.h"

#ifndef HeadOnly
std::vector<ECS::ComponentFactory>& ComponentFactories(){
    static std::vector<ECS::ComponentFactory> f;
    return f;
}
#endif

#ifdef UseDLLSafe
static int counter;
ComponentID GetNewComponentID(){
    return counter++;
}
#endif
