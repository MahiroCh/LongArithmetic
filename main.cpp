#include <iostream>
#include <chrono>

#include "funcs_header.hpp"

int main() {

    auto begin = std::chrono::steady_clock::now();

    
    LongArithm::DA_BIG num1("-34657834657.123414343", 500);
    // num1.debugInfo();
    LongArithm::DA_BIG num2("-34657834657.12341434367", 500);
    // num2.debugInfo();
    std::cout << (num1 > num2);
    // LongArithm::DA_BIG res = num2 * num1;
    // res.debugInfo();
    // std::cout << res;


    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "\n\nThe time elapsed: " << elapsed_ms.count() << " ms\n";
}