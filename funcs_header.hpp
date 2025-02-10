#include <string>
#include <vector>

namespace LongArithm {

class DA_BIG {

private:

    unsigned point_pos = 0;
    unsigned bin_precision = 0; // кол-во знаков после запятой в двоичной системе счисления
    bool minus = 0;
    std::vector<bool> bits; // ТОДО: найти функцию, которая представляют bool'ы как 1 бит
    
    // перевод числа в двоичный вид и обратно
    bool intIsLessOrEqual(const std::string &num1, const std::string &num2) const;
    std::string intAdd(const std::string &num1, const std::string &num2);
    std::string intSubtract(const std::string &num1, const std::string &num2); 
    void decToBin(const std::string &input);
    std::string decDivide(std::string dividend, std::string divisor, const int double_precision);
    
    // удалить лишние нули
    void delLeadTrailZero(std::vector<bool> &num); 
    void delLeadTrailZero(std::string &num);
    
    
public:
    
    std::string binToDec(const DA_BIG &num); // временно public, так как иначе operator<< не работает
    
    // конструкторы
    DA_BIG();
    DA_BIG(std::string input, const unsigned precision = 10);
    
    // перегрузки операторов
    friend std::ostream &operator<<(std::ostream &os, const DA_BIG &num) ;
    bool operator<(const DA_BIG& other) const;
    bool operator>(const DA_BIG& other) const;
    bool operator==(const DA_BIG& other) const;
    bool operator!=(const DA_BIG& other) const;
    DA_BIG operator+(const DA_BIG& other) const;
    DA_BIG operator-(const DA_BIG& other) const;
    DA_BIG operator*(const DA_BIG& other) const;
    DA_BIG operator/(const DA_BIG& other) const;
    DA_BIG operator-() const;
    
    // для дебага
    void debugInfo() const;
};

}