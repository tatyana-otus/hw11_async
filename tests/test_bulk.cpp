#include "application.h"
#include <sstream>
#include <iostream>

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

std::string out_1h = "bulk: a1, a2\n"
                     "bulk: a3, a4\n"
                     "bulk: a5\n";

std::string out_2h = "bulk: a1, a2\n"
                     "bulk: b1, b2\n"
                     "bulk: a3\n"
                     "bulk: b3\n";

BOOST_AUTO_TEST_SUITE(test_suite_1)


BOOST_AUTO_TEST_CASE(receive_by_char)
{    
    std::stringstream oss;
    {
        Application app(oss, 2);

        std::size_t bulk = 2;
        auto h1 = app.connect(bulk);
        app.receive(h1, "a",  1);
        app.receive(h1, "1",  1);
        app.receive(h1, "\n", 1);
        app.receive(h1, "a",  1);
        app.receive(h1, "2",  1);
        app.receive(h1, "\n", 1);
        app.receive(h1, "a",  1);
        app.receive(h1, "3",  1);
        app.receive(h1, "\n", 1);
        app.receive(h1, "a",  1);
        app.receive(h1, "4",  1);
        app.receive(h1, "\n", 1);
        app.receive(h1, "a",  1);
        app.receive(h1, "5",  1);
        app.receive(h1, "\n", 1);

        app.disconnect(h1);
    }
    BOOST_CHECK_EQUAL( oss.str(), out_1h);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_2)

BOOST_AUTO_TEST_CASE(receive_all)
{  
    
    std::stringstream oss;

    {
        Application app(oss, 2);

        std::size_t bulk = 2;
        auto h1 = app.connect(bulk);

        app.receive(h1, "a1\na2\na3\na4\na5\n", 15);

        app.disconnect(h1);
    }
    BOOST_CHECK_EQUAL( oss.str(), out_1h);
 }   

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_3)

BOOST_AUTO_TEST_CASE(receive_2h)
{  
    
    std::stringstream oss;

    {
        Application app(oss, 2);

        std::size_t bulk = 2;
        auto h1 = app.connect(bulk);
        auto h2 = app.connect(bulk);

        app.receive(h1, "a",  1);
        app.receive(h1, "1",  1);
        app.receive(h1, "\n", 1);       
        app.receive(h1, "a",  1);
        app.receive(h1, "2",  1);
        app.receive(h1, "\n", 1);
        app.receive(h1, "a",  1);
        app.receive(h1, "3",  1);
        app.receive(h1, "\n", 1);

        app.receive(h2, "b",  1);
        app.receive(h2, "1",  1);
        app.receive(h2, "\n", 1);
        app.receive(h2, "b",  1);
        app.receive(h2, "2",  1);
        app.receive(h2, "\n", 1);
        app.receive(h2, "b",  1);
        app.receive(h2, "3",  1);
        app.receive(h2, "\n", 1);
        
        app.disconnect(h1);
        app.disconnect(h2);
    }
    BOOST_CHECK_EQUAL( oss.str(), out_2h);
 }   

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_4)

BOOST_AUTO_TEST_CASE(receive_2h_shuffle)
{  
    
    std::stringstream oss;

    {
        Application app(oss, 2);

        std::size_t bulk = 2;
        auto h1 = app.connect(bulk);
        auto h2 = app.connect(bulk);

        app.receive(h1, "a",  1);
        app.receive(h2, "b",  1);
        app.receive(h1, "1",  1);
        app.receive(h2, "1",  1);
        app.receive(h1, "\n", 1); 
        app.receive(h2, "\n", 1);      
        app.receive(h1, "a",  1);
        app.receive(h2, "b",  1);
        app.receive(h1, "2",  1);
        app.receive(h2, "2",  1);
        app.receive(h1, "\n", 1);
        app.receive(h2, "\n", 1);
        app.receive(h1, "a",  1);
        app.receive(h2, "b",  1);
        app.receive(h1, "3",  1);
        app.receive(h2, "3",  1);
        app.receive(h1, "\n", 1);
        app.receive(h2, "\n", 1);

        app.disconnect(h1);
        app.disconnect(h2);
    }
    BOOST_CHECK_EQUAL( oss.str(), out_2h);
 }   

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_5)

BOOST_AUTO_TEST_CASE(receive_2h_diff_th)
{  
    
    std::stringstream oss;

    {
        Application app(oss, 2);
        std::size_t bulk = 2;

        std::thread helper1([&app, bulk](){
            auto h = app.connect(bulk);
            app.receive(h, "a",  1);
            app.receive(h, "b",  1);
            app.receive(h, "c",  1);
            app.receive(h, "d",  1);
            app.receive(h, "e",  1);
            app.receive(h, "f",  1);
            app.receive(h, "1",  1);
            app.receive(h, "2",  1);
            app.receive(h, "3",  1);
            app.receive(h, "4",  1);
            app.receive(h, "\n", 1);

            app.disconnect(h);
        });
        

        std::thread helper2([&app, bulk](){
            auto h = app.connect(bulk);
            app.receive(h, "a",  1);
            app.receive(h, "b",  1);
            app.receive(h, "c",  1);
            app.receive(h, "d",  1);
            app.receive(h, "e",  1);
            app.receive(h, "f",  1);
            app.receive(h, "1",  1);
            app.receive(h, "2",  1);
            app.receive(h, "3",  1);
            app.receive(h, "4",  1);
            app.receive(h, "\n", 1);

            app.disconnect(h);
        });       

        helper1.join();
        helper2.join();
    }

    BOOST_CHECK_EQUAL( oss.str(), 
                      "bulk: abcdef1234\nbulk: abcdef1234\n");
 }   

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_6)

BOOST_AUTO_TEST_CASE(incomplete_command)
{     
    std::stringstream oss;

    {
        Application app(oss, 2);

        std::size_t bulk = 2;
        auto h1 = app.connect(bulk);

        app.receive(h1, "a1\na2\na3\na4\na5\na6", 17);

        app.disconnect(h1);
    }
    BOOST_CHECK_EQUAL( oss.str(), out_1h);
 }   

BOOST_AUTO_TEST_SUITE_END()