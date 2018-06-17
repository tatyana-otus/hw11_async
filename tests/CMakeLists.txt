project(test)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(Threads)

add_executable(test_handle_cli         test_handle.cpp)
add_executable(test_log_cli            test_log.cpp)
add_executable(test_bulk_cli           test_bulk.cpp)

target_include_directories(test_handle_cli  PRIVATE ../include)
target_include_directories(test_handle_cli  PRIVATE ${Boost_INCLUDE_DIR})

target_include_directories(test_log_cli PRIVATE ../include)
target_include_directories(test_log_cli PRIVATE ${Boost_INCLUDE_DIR})

target_include_directories(test_bulk_cli PRIVATE ../include)
target_include_directories(test_bulk_cli PRIVATE ${Boost_INCLUDE_DIR})

set_target_properties(test_handle_cli  test_bulk_cli test_log_cli PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
  COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
)
#set (CMAKE_CXX_FLAGS "-fsanitize=thread")

target_link_libraries(test_handle_cli
  ${CMAKE_THREAD_LIBS_INIT}	
  ${Boost_LIBRARIES}
  async
)

target_link_libraries(test_log_cli
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  async
)

target_link_libraries(test_bulk_cli
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
)

enable_testing()

add_test(test_handle        test_handle_cli  -t test_suite_main_1)
add_test(invalid_bulk_size  test_handle_cli  -t test_suite_main_2)
add_test(invalid_cmd_length test_handle_cli  -t test_suite_main_3)

add_test(receive_by_char    test_bulk_cli  -t test_suite_1)
add_test(receive_all        test_bulk_cli  -t test_suite_2)
add_test(receive_2_h        test_bulk_cli  -t test_suite_3)
add_test(receive_2h_shuffle test_bulk_cli  -t test_suite_4)
add_test(receive_2h_diff_th test_bulk_cli  -t test_suite_5)
add_test(incomplete_command test_bulk_cli  -t test_suite_6)

add_test(create_log_file       test_log_cli   -t test_suite_1)
add_test(log_file_alredy_exist test_log_cli   -t test_suite_2)
add_test(remove_test_files     test_log_cli   -t test_suite_delete)