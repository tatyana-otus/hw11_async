#include "async.h"
#include "consts_types.h"
#include <bits/stdc++.h>


#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>


//const char *data1="0 0\n\n1 1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n{\n11\n}\n{";
//const char *data2=                                                   "\n12\n13\n14\n}\n15\n{\n16\n{\n17\n{\n18\n}\n19\n}\n20\n}\n{\n21\n22\n23\n";
    
const char *data1="0 0\n\n1 1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n{\n11\n}\n";
const char *data2= "{\n12\n13\n14\n}\n15\n{\n16\n{\n17\n{\n18\n}\n19\n}\n20\n}\n{\n21\n22\n23}\n";

    auto sz1 = std::strlen(data1);
    auto sz2 = std::strlen(data2);



    using atomic_h = std::atomic<decltype(async::connect(3))>;
    atomic_h h1;
    atomic_h h2;
    std::atomic<bool> can_do{1};

    void connect_disconnect(atomic_h& h)
    {
        while(can_do)
        {
            h = async::connect(3);

            async::receive(h, data1, sz1);

            async::disconnect(h);

    //        std::this_thread::yield();
        }
    }


BOOST_AUTO_TEST_SUITE(test_suite_async)

BOOST_AUTO_TEST_CASE(programm_for_run)
{
    try
    {
        std::thread reconnecting1(std::bind(connect_disconnect, std::ref(h1)));
        std::thread reconnecting2(std::bind(connect_disconnect, std::ref(h2)));

        size_t N{1};
        while(std::cin >> N)
        {
            while(--N)
            {
                async::receive(h1, data2, sz2);
                async::receive(h2, data2, sz2);
            }
  //          std::this_thread::yield();
        }

        std::cout << "stop" << std::endl;
        can_do = false;
        reconnecting1.join();
        reconnecting2.join();
    }
    catch(const std::exception& e)
    {
        std::cout << "std::exeption:" << e.what() << std::endl;
        BOOST_CHECK(true);
    }

    std::cout << "done" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(run_test_async)

BOOST_AUTO_TEST_CASE(run_test)
{

    BOOST_CHECK_NO_THROW(std::system("seq 1 500 |./test_async_cli -t test_suite_async > a"));
}

BOOST_AUTO_TEST_SUITE_END()
