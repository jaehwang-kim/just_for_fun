# 최소 CMake 버전 지정
cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름 설정
project(myhttpd)

# 컴파일 경고 플래그 추가
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(myhttpd PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(test_myhttpd PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(myhttpd PRIVATE /W4)
    target_compile_options(test_myhttpd PRIVATE /W4)
endif()

# 헤더 파일이 있는 디렉토리를 포함
include_directories(src)

set(MAIN_SRC src/main.c)
set(APP_SRC src/httpd.c src/handle_request.c src/ps_cmd.c
             src/handle_request.h src/httpd.h src/ps_cmd.h)

# 실행 파일 추가
add_executable(myhttpd ${MAIN_SRC} ${APP_SRC})

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
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 테스트 실행 파일 추가
add_executable(test_myhttpd test/test_main.cpp test/test_ps_cmd.cpp 
               ${APP_SRC})            

target_link_libraries(test_myhttpd gmock pthread)

# 테스트 추가
enable_testing()
add_test(NAME test_myhttpd COMMAND test_myhttpd)
