#ifndef DA_BIG_HPP
#define DA_BIG_HPP

#include <string>
#include <vector>

namespace LongArithm {

class DA_BIG {

private:

    int point_pos_; // реальная поизиция точки
    int precision_; // точность для вычислений, конечные незначащие нули всегда обрезаются
    bool minus_; // наличие минуса
    std::vector<short> digits_; // сами цифры (не задом наперед)
    
    // вспомогательные методы
    void decToBin();
    DA_BIG binToDec() const; // выдает столько же цифр после запятой 10 системе, сколько было в 2 системе
    void deleteZeroes(); 
    bool isZero() const;
    void roundDouble(const int& leave_digits, const short& base);

    // основные методы рассчетов (операторы просто будут ссылаться на них в двоичной системе счисления)
    DA_BIG operatorDivide(DA_BIG, DA_BIG, const short& base) const;
    DA_BIG operatorAdd(DA_BIG, DA_BIG, const short& base) const;
    DA_BIG operatorSubtract(DA_BIG, DA_BIG, const short& base) const;
    DA_BIG operatorMultiply(DA_BIG, DA_BIG, const short& base) const;
    
public:
    
    // конструкторы и деструкторы
    DA_BIG();
    ~DA_BIG();
    DA_BIG(std::string, const int& precision = 700, const bool& make_binary = true);
    DA_BIG(const DA_BIG&); // конструктор копирования
    
    // перегрузки операторов
    bool operator<(const DA_BIG&) const;
    bool operator<=(const DA_BIG&) const;
    bool operator>(const DA_BIG&) const;
    bool operator>=(const DA_BIG&) const;
    bool operator==(const DA_BIG&) const;
    bool operator!=(const DA_BIG&) const;
    DA_BIG operator+(const DA_BIG&) const;
    DA_BIG& operator+=(const DA_BIG&);
    DA_BIG operator-(const DA_BIG&) const;
    DA_BIG& operator-=(const DA_BIG&);
    DA_BIG operator*(const DA_BIG&) const;
    DA_BIG& operator*=(const DA_BIG&);
    DA_BIG operator/(const DA_BIG&) const;
    DA_BIG& operator/=(const DA_BIG&);
    DA_BIG operator-() const;
    friend std::ostream& operator<<(std::ostream&, const DA_BIG&) ;
    DA_BIG& operator=(const DA_BIG&); // оператор присваивания
    
    // другие методы
    void setBinPrecision(const int);
    void debugInfo() const; // для дебага
    std::string toDecimalStr(const int& leave_digits = 50, const bool& leave_in_bin = false) const;

    // для pi
    static DA_BIG calculatePi(const int& dec_prec, const int& bin_prec);
    static std::string getPi(const int& dec_prec = 100, const int& bin_prec = 500);
};

}

LongArithm::DA_BIG operator ""_longnum(long double);
LongArithm::DA_BIG operator ""_longnum(unsigned long long);

#endif
