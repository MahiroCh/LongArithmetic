#ifndef DA_BIG_TEST_HPP
#define DA_BIG_TEST_HPP

#include <string>

namespace LongArithmTests {

    void TEST_CASE(std::string name, int expr);
    void piTest(int pi_precision = 100);
    void mainTest();
}

#endif
