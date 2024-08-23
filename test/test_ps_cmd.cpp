// test/ps_cmd_test.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include "../src/ps_cmd.h"

// Mock FILE struct for testing
struct MockFile {
    std::stringstream buffer;
};

static MockFile mockFile;

extern "C" {
    FILE* (*__real_fopen)(const char* path, const char* mode) ;
    int (*__real_fprintf)(FILE* stream, const char* format, ...);

    FILE* ___wrap_fopen(const char* path, const char* mode) {
        return reinterpret_cast<FILE*>(&mockFile);
    }

    FILE* (*__wrap_fopen)(const char* path, const char* mode);

    int ____wrap_fprintf(FILE* stream, const char* format, ...) {
        va_list args;
        va_start(args, format);
        char buffer[4096];
        int result = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        mockFile.buffer << buffer;
        return result;
    }

    int (*__wrap_fprintf)(FILE* stream, const char* format, ...);

}

// Test fixture for ps_cmd tests
class PsCmdTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Replace fopen and fprintf with the wrapped versions
        __real_fopen = fopen;
        __real_fprintf = fprintf;
        __wrap_fopen = ___wrap_fopen;
        __wrap_fprintf = ____wrap_fprintf;
    }

    void TearDown() override {
        // Restore the original fopen and fprintf functions
        __wrap_fopen = __real_fopen;
        __wrap_fprintf = __real_fprintf;
    }

};

// Test case for ps_cmd
TEST_F(PsCmdTest, PsCmdTest) {
    // Call the ps_cmd function
    ps_cmd(reinterpret_cast<FILE*>(&mockFile));

    // Verify the output
    std::string expectedOutput = "PID        Name                 Path                                              \n"
                                 "0          (Unavailable)       (Unavailable)                                     \n";
    ASSERT_EQ(mockFile.buffer.str(), expectedOutput);
}