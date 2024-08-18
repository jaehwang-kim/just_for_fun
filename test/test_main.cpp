// test/test_main.cpp
#include <gtest/gtest.h>
#include "../src/main.h"

TEST(MyHttpdTest, ExampleTest) {
    // 테스트 코드를 여기에 작성합니다.
    EXPECT_EQ(1, 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
