#include "async.h"
#include "consts_types.h"
#include <bits/stdc++.h>


#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(run_test_async)

BOOST_AUTO_TEST_CASE(run_test)
{
    std::system("rm -f *.log res.txt res_bulk.txt");
    std::string data =  "bulk: 0 0, , 1 1\n"
                        "bulk: 11\n"
                        "bulk: 12, 13, 14\n"
                        "bulk: 15\n"
                        "bulk: 16, 17, 18, 19, 20\n"
                        "bulk: 2, 3, 4\n"
                        "bulk: 5, 6, 7\n"
                        "bulk: 8, 9, 10\n";

    std::system("seq 1 200 |./programm_test > res.txt");
    std::system("sort res.txt | uniq | grep bulk > res_bulk.txt");

    auto file = std::ifstream("res_bulk.txt");
    std::stringstream ss;
    ss << file.rdbuf();
    BOOST_CHECK_EQUAL(ss.str(), data);
}

BOOST_AUTO_TEST_SUITE_END()
