#include "DllComponents.h"

#if defined(UseDLLSafe) && !defined(TestGetId)
CompImplment(DllPosition)
CompImplment(DllVelocity)
#endif

void RegisterDllSampleComponents(){
    ECS::RegisterComponent<DllPosition>();
    ECS::RegisterComponent<DllVelocity>();
}
