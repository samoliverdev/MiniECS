#include <gtest/gtest.h>
#include <cstdio>
#include "ecs_test_common.h"

CompImplment(Position)
CompImplment(Velocity)
CompImplment(Health)
CompImplment(Renderable)
CompImplment(Disabled)

void Init(){
    std::printf("Engine Init!\n");
}

void Shutdown(){
    std::printf("Engine Shutdown!\n");
}

int main(int argc, char** argv){
    Init();

    ::testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

    Shutdown();
    
    return r;
}
