cmake_minimum_required(VERSION 3.2)

project(async VERSION 1.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Threads)

add_library(async SHARED async.cpp )

target_link_libraries(async
  ${CMAKE_THREAD_LIBS_INIT}
  )


target_include_directories(async PRIVATE ./include)

set_target_properties(async PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wall -Wextra -Wpedantic 
)

#set (CMAKE_CXX_FLAGS "-fsanitize=thread")

install(TARGETS async LIBRARY DESTINATION lib)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT lisstic.tt@gmail.com)

include (CPack)

add_subdirectory(tests)

enable_testing()
