#include <gtest/gtest.h>

TEST(MyTest, Test1) {
    EXPECT_EQ(1, 1);
}

TEST(MyTest, Test2) {
    EXPECT_EQ(2, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
