// test/ps_cmd_test.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include "../src/ps_cmd.h"

// Fixture class for ps_cmd test
class PsCmdTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up any necessary resources for the test
    }

    void TearDown() override {
        // Clean up any resources used by the test
    }
};

// Test case for ps_cmd
TEST_F(PsCmdTest, print) {
    char buff[4096];

    // Call the ps_cmd function
    FILE* file = fmemopen(buff, sizeof(buff), "w");
    ps_cmd(file);
    
    fclose(file);

    std::string actualOutput(buff);

    // Verify the output
    std::string expectedOutput = "PID        Name                 Path                                              \n"
                                 "0          (Unavailable)       (Unavailable)                                     \n";

    // Check if the first line of expectedOutput and actualOutput are the same
    std::istringstream expectedStream(expectedOutput);
    std::istringstream actualStream(actualOutput);
    std::string expectedLine, actualLine;

    std::getline(expectedStream, expectedLine);
    std::getline(actualStream, actualLine);

    ASSERT_EQ(actualLine, expectedLine);
}
