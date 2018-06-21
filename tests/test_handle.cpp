#include "async.h"
#include "consts_types.h"

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(test_suite_main_1)

BOOST_AUTO_TEST_CASE(handle_to_index)
{
    std::size_t bulk = 1;

    auto h1 = async::connect(bulk);
    BOOST_CHECK_EQUAL( reinterpret_cast<size_t>(h1), 1 );

    auto h2 = async::connect(bulk);
    BOOST_CHECK_EQUAL( reinterpret_cast<size_t>(h2), 2 );

    auto h3 = async::connect(bulk);
    BOOST_CHECK_EQUAL( reinterpret_cast<size_t>(h3), 3 );

    auto h4 = async::connect(bulk);
    BOOST_CHECK_EQUAL( reinterpret_cast<size_t>(h4), 4 );

    async::disconnect(h1);
    async::disconnect(h2);

    auto h5 = async::connect(bulk);
    BOOST_CHECK_EQUAL( reinterpret_cast<size_t>(h5), 1 );

    auto h6 = async::connect(bulk);
    BOOST_CHECK_EQUAL( reinterpret_cast<size_t>(h6), 2 );

    async::disconnect(h3);
    async::disconnect(h4);
    async::disconnect(h5);
    async::disconnect(h6);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_main_2)

BOOST_AUTO_TEST_CASE(invalid_bulk_size)
{
    BOOST_CHECK_THROW(async::connect(MAX_BULK_SIZE + 1), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_main_3)

BOOST_AUTO_TEST_CASE(invalid_cmd_length)
{
    std::string in_data (MAX_CMD_LENGTH + 1, 'a');

    auto h = async::connect(1);   
    
    BOOST_CHECK_NO_THROW(async::receive(h, in_data.c_str(), MAX_CMD_LENGTH + 1));
    BOOST_CHECK_THROW(async::receive(h, "\n", 1), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_main_4)

BOOST_AUTO_TEST_CASE(wrong_cmd)
{
    auto h = async::connect(1);   
    
    BOOST_CHECK_THROW(async::receive(h, "}\n", 2), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()