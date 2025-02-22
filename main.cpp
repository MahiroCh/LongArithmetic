#include "longArithmetic.hpp"
#include "tests.hpp"

int main(int argc, char* argv[]) {
    
    if (argc > 1) {
        if ((std::string)argv[1] == "test") {
            LongArithmTests::mainTest();
        }
        
        if ((std::string)argv[1] == "pi") {
            int pi_prec = 100;
            if (argc > 2) pi_prec = atoi(argv[2]);
            LongArithmTests::piTest(pi_prec);
        }
    }
}
