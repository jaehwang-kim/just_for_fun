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