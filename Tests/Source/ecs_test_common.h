#pragma once
#include <gtest/gtest.h>
#include <ECS.h>

using namespace ECS;

// Test components
struct Position {
    CompDefinition
    float x = 0, y = 0;
};

struct Velocity {
    CompDefinition
    float x = 0, y = 0;
};

struct Health {
    CompDefinition
    int value = 100;
};

struct Renderable {
    CompDefinition
    int data = 0;
};

struct Disabled {
    CompDefinition
    int data = 0;
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
            RegisterComponent<Renderable>();
            RegisterComponent<Disabled>();
            registered = true;
        }
    }
};
