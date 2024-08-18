# 최소 CMake 버전 지정
cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름 설정
project(myhttpd)

# 헤더 파일이 있는 디렉토리를 포함
include_directories(src)

# 실행 파일 추가
add_executable(myhttpd src/main.c src/httpd.c src/handle_request.c src/ps_cmd.c)

# macOS의 경우, 시스템 라이브러리 링크가 필요할 수 있습니다.
if(APPLE)
    find_library(CORE_SERVICES_LIBRARY CoreServices)
    target_link_libraries(myhttpd ${CORE_SERVICES_LIBRARY})
    # macOS에서 추가적인 라이브러리가 필요한 경우 여기에 추가합니다.
else()
    # Linux의 경우, 소켓 라이브러리와 pthread, m 라이브러리를 링크합니다.
    target_link_libraries(myhttpd pthread m)
endif()

# gtest 설정
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 테스트 실행 파일 추가
add_executable(test_myhttpd test/test_main.cpp src/httpd.c src/handle_request.c src/ps_cmd.c)
target_link_libraries(test_myhttpd gtest pthread)

# 테스트 추가
enable_testing()
add_test(NAME test_myhttpd COMMAND test_myhttpd)