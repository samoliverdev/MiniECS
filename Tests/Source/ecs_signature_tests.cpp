#include "ecs_test_common.h"

TEST(SignatureTests, ContainsLogic) {
    Signature a = Signature::Make<Position, Velocity>();
    Signature b = Signature::Make<Position>();

    ASSERT_TRUE(a.Contains(b));
    ASSERT_FALSE(b.Contains(a));
}

TEST(SignatureTests, Equality) {
    Signature a = Signature::Make<Position, Velocity>();
    Signature b = Signature::Make<Position, Velocity>();
    Signature c = Signature::Make<Position>();

    ASSERT_TRUE(a == b);
    ASSERT_FALSE(a == c);
}
