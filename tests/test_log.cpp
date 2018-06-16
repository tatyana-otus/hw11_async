#include "async.h"
#include <chrono>
#include <fstream>
#include <thread>

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(test_suite_1)

BOOST_AUTO_TEST_CASE(create_log_file)
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    auto main_th_id = ss.str();


    std::size_t bulk = 1;

    auto h = async::connect(bulk);

    std::time_t init_time = std::time(nullptr);
    async::receive(h, "cmd_123\n", 8);
    
    async::disconnect(h);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    size_t id = 1;
    std::string name = "bulk" + std::to_string(init_time) + "_" 
                              + main_th_id + "_" 
                              + std::to_string((reinterpret_cast<size_t>(h) << 32) | id) + ".log";   
    int watchdog_sec = 0;
    while(!(std::ifstream{name}) && (watchdog_sec < 10)){
        name = "bulk" + std::to_string(init_time) + "_" 
                      + main_th_id + "_" 
                      + std::to_string((reinterpret_cast<size_t>(h) << 32) | id) + ".log";
        ++watchdog_sec;
    }
    if(watchdog_sec >= 10) BOOST_CHECK(false);

    std::ifstream log_file {name};
    BOOST_CHECK_EQUAL( log_file.good(), true );

    std::stringstream log_buff;
    log_buff << log_file.rdbuf();
    log_file.close();

    BOOST_CHECK_EQUAL( log_buff.str(), "bulk: cmd_123" );
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_delete)

BOOST_AUTO_TEST_CASE(remove_test_files)
{
    std::system("rm -f bulk*.log");

}
BOOST_AUTO_TEST_SUITE_END()