#include <gtest/gtest.h>
#include <cstdio>

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