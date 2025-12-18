#pragma once
#include <gtest/gtest.h>
#include <ECS.h>

using namespace ECS;

// Test components
struct Position {
    float x = 0, y = 0;
};

struct Velocity {
    float x = 0, y = 0;
};

struct Health {
    int value = 100;
};

// Shared fixture
class ECSTest : public ::testing::Test {
protected:
    World world;

    void SetUp() override {
        // Register components once
        static bool registered = false;
        if(!registered){
            RegisterComponent<Position>();
            RegisterComponent<Velocity>();
            RegisterComponent<Health>();
            registered = true;
        }
    }
};
