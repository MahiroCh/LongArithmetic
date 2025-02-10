#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "funcs_header.hpp"

namespace LongArithm {

// КОНСТРУКТОР КОПИРОВАНИЯ И ОПЕРАТОР ПРИСВАИВАНИЯ

DA_BIG::DA_BIG(const DA_BIG& obj) {

    point_pos = obj.point_pos;
    bin_precision = obj.bin_precision;
    minus = obj.minus;
    bits = obj.bits;
}

DA_BIG& DA_BIG::operator=(const DA_BIG& other) {

    if (this != &other) {  
        point_pos = other.point_pos;
        bin_precision = other.bin_precision;
        minus = other.minus;
        bits = other.bits;  
    }
    return *this; 
}

// ДРУГИЕ КОНСТРУКТОРЫ И ДЕСТРУКТОР

DA_BIG::DA_BIG() {

    unsigned point_pos = 0;
    unsigned bin_precision = 100;
    bool minus = false;
    std::vector<bool> bits;
}

DA_BIG::DA_BIG(std::string input, const unsigned prec) {

    // разбираюсь с минусом
    if (input[0] == '-') {
        minus = 1;
        input.erase(input.begin());
    } else minus = 0;

    bin_precision = prec;

    // если число целое, то привожу его к формату digits.0
    if (input.find('.') == std::string::npos) {
        input += '.';
        input += '0';
    }

    delLeadTrailZero(input);
    point_pos = input.find('.');

    // если число равно нулю, то уберу минус
    if (input.back() == '0' && input.front() == '0') minus = 0;

    // перевод числа в двоичную
    decToBin(input);
    delLeadTrailZero(bits);
}

DA_BIG::~DA_BIG(){}

// СЛЕДУЮЩИЕ 7 ФУНКЦИЙ — ВСПОМОГАТЕЛЬНЫЕ ДЛЯ ПЕРЕВОДА ЧИСЛА В ДВОИЧНЫЙ ВИД
// И ОБРАТНО

void DA_BIG::decToBin(const std::string &input) {

    std::string int_part = input.substr(0, point_pos);
    std::string double_part = input.substr(point_pos + 1, input.length() - point_pos - 1);
    point_pos = bin_precision;

    // дробная часть в двоичную
    int dp_len = double_part.length();
    bits.resize(bin_precision);
    for (int i = bin_precision - 1; i >= 0; i--) {
        double_part = intAdd(double_part, double_part);
        if (double_part.length() > dp_len) {
            double_part.erase(0, 1);
            bits[i] = 1;
        } else bits[i] = 0;
    }

    // целая часть в двоичную
    if (int_part.front() == '0') bits.push_back(0);
    while (int_part[0] != '0') {
        std::string adding = "2";
        int add_zeroes = 0;
        for (auto i = 0; i < int_part.length() - 1; i++) {
            adding += '0';
            add_zeroes++;
        }

        if ((int_part.back() - '0') % 2 == 0) bits.push_back(0);
        else bits.push_back(1);

        std::string next_div;
        for (; add_zeroes >= 0; add_zeroes--) {
            std::string subtrahend = "0";
            char digit = '0';

            while (intIsLessOrEqual(int_part, intAdd(subtrahend, adding))) {
                subtrahend = intAdd(subtrahend, adding);
                digit++;
            }
            next_div += digit;
            int_part = intSubtract(int_part, subtrahend);
            int_part += '0';
        }
        if (next_div[0] == '0' && next_div.length() > 1) next_div.erase(0, 1);
        int_part = next_div;
    }
}

std::string DA_BIG::binToDec(const DA_BIG &num) {

    std::string result = "0", double_denominator, two_pow = "1";
    int i;
    for (i = num.point_pos; i < num.bits.size(); i++) {
        if (num.bits[i] == 1) result = intAdd(result, two_pow);
        two_pow = intAdd(two_pow, two_pow);
        if (i - num.point_pos <= num.point_pos) double_denominator = two_pow;
    }

    if (num.minus) result = "-" + result;

    if (!num.point_pos) return result;

    for (i = 0; i < (int)(2 * num.point_pos - num.bits.size() + 1); i++)
        double_denominator = intAdd(double_denominator, double_denominator);

    std::string double_numerator = "0";
    for (i = num.point_pos - 1; i >= 0; i--) {
        if (num.bits[i] == 1) double_numerator = intAdd(double_numerator, "1");
        double_numerator = intAdd(double_numerator, double_numerator);
    }
 
    result += decDivide(double_numerator, double_denominator, num.bin_precision).substr(1);

    return result;
}

std::string DA_BIG::decDivide(std::string dividend, std::string divisor, const int double_precision) {

    std::string result, adding = divisor;
    int div_digits_amount = 0;

    for (auto i = 0; i < (int)(dividend.length() - divisor.length()); i++) {
        adding += '0';
        div_digits_amount++;
    }

    for (int i = 0; i < div_digits_amount + double_precision + 1; i++) {
        std::string subtrahend = "0";
        char digit = '0';

        while (intIsLessOrEqual(dividend, intAdd(subtrahend, adding))) {
            subtrahend = intAdd(subtrahend, adding);
            digit++;
        }
        
        result += digit;
        dividend = intSubtract(dividend, subtrahend);
        dividend += '0';
        if (dividend == "00") break;
        if (i == div_digits_amount && double_precision) result += '.';
    }

    // if (result.back() >= '5' && double_precision) { // могут быть проблемы, если перед последним числом стоит 9
    //     result.pop_back();
    //     result.back() += 1;
    // }
    // if (result.length() > 1) result.pop_back();

    delLeadTrailZero(result);

    return result;
}

std::string DA_BIG::intAdd(

    const std::string &num1,
    const std::string &num2) {  // работает только для положительных чисел

    short carry = 0;
    int i = num2.length() - 1, j = num1.length() - 1;
    std::string result(std::max(i, j) + 1, '0');
    for (; j >= 0 || i >= 0; i--, j--) {
        short n = carry;
        if (j >= 0) n += (num1[j] - '0');
        if (i >= 0) n += (num2[i] - '0');
        carry = n / 10;
        result[std::max(j, i)] = (n % 10) + '0';
    }

    if (carry) result.insert(result.begin(), '1');

    return result;
}

std::string DA_BIG::intSubtract(const std::string &num1, const std::string &num2) {  // работает только для положительных чисел num1 >= numm2

    std::string result = num1;
    int i = num2.length() - 1, j = num1.length() - 1;
    for (; j >= 0; i--, j--) {
        short n = result[j] - '0';
        if (i >= 0) n -= (num2[i] - '0');
        if (n < 0) {
            result[j - 1]--;
            n += 10;
        }

        result[j] = (n + '0');
    }
    delLeadTrailZero(result);

    return result;
}

bool DA_BIG::intIsLessOrEqual(const std::string &num1, const std::string &num2) const {  // работает только для положительных чисел

    int i = num2.length(), j = num1.length();
    if (j > i) return true;
    if (j < i) return false;

    for (auto k = 0; k < i; k++) {
        if (num1[k] > num2[k]) return true;
        if (num1[k] < num2[k]) return false;
    }

    return true;
}

// ВСПОМОГАТЕЛЬНАЯ ФУНКЦИЯ УДАЛЕНИЯ ВЕДУЩИХ И ПОСЛЕДНИХ НУЛЕЙ

void DA_BIG::delLeadTrailZero(std::vector<bool> &num) {

    int n = 0;
    for (int i = 0; num[i] == 0 && i < point_pos; i++) n++;
    point_pos -= n;
    num.erase(num.begin(), num.begin() + n);

    while (num.size() > point_pos + 1 && num.back() == 0) num.pop_back();
}

void DA_BIG::delLeadTrailZero(std::string &num) {

    int amount = 0;
    for (int i = 0; num.length() > 1 && num[i + 1] != '.' && num[i] == '0'; i++)
        amount++;

    num.erase(0, amount);

    if (num.find('.') != std::string::npos)
        while (num[num.length() - 2] != '.' && num.back() == '0') 
            num.pop_back();
}

// ПЕРЕГРУЗКИ ОПЕРАТОРОВ

std::ostream &operator<<(std::ostream &os, const DA_BIG &num) {

    DA_BIG temp = num;
    return os << temp.binToDec(temp);
}

bool DA_BIG::operator<(const DA_BIG &other) const {

    bool True = 1;
    if (this->minus && other.minus) True = 0;
    if (this->minus && !other.minus) return True;
    if (!this->minus && other.minus) return !True;

    if (this->bits.size() - this->point_pos <
        other.bits.size() - other.point_pos)
        return True;
    if (this->bits.size() - this->point_pos >
        other.bits.size() - other.point_pos)
        return !True;

    for (int i = this->bits.size(), j = other.bits.size(); i >= 0 || j >= 0;
         i--, j--) {
        bool this_bit = 0, other_bit = 0;
        if (i >= 0) this_bit = this->bits[i];
        if (j >= 0) other_bit = other.bits[i];
        if (this_bit != other_bit)
            return ((this_bit ^ (!True)) < (other_bit ^ (!True)));
    }

    return !True;
}

bool DA_BIG::operator>(const DA_BIG &other) const { return other < (*this); }

bool DA_BIG::operator==(const DA_BIG &other) const {

    if (this->minus != other.minus) return false;
    if (this->point_pos != other.point_pos) return false;
    if (this->bits.size() - this->point_pos !=
        other.bits.size() - other.point_pos)
        return false;

    for (int i = this->bits.size(); i >= 0; i--)
        if (this->bits[i] != other.bits[i]) return false;

    return true;
}

bool DA_BIG::operator!=(const DA_BIG &other) const { return !(*this == other); }

DA_BIG DA_BIG::operator-() const {

    DA_BIG n(*this);
    n.minus = !minus;
    return n;
}

DA_BIG DA_BIG::operator+(const DA_BIG &other) const {

    if (this->minus != other.minus) {
        if (other.minus) return *this - (-other);
        return other - (-(*this));
    }

    DA_BIG result;
    result.minus = this->minus;
    result.bin_precision = std::max(this->bin_precision, other.bin_precision);
    result.point_pos = result.bin_precision;
    bool carry = 0;

    for (int i = 0; i < result.bin_precision; i++) {
        char n = carry;
        if (i >= result.bin_precision - this->point_pos)
            n += this->bits[i - result.bin_precision + this->point_pos];
        if (i >= result.bin_precision - other.point_pos)
            n += other.bits[i - result.bin_precision + other.point_pos];
        carry = n / 2;
        result.bits.push_back(n % 2);
    }

    for (int i = this->point_pos, j = other.point_pos;
         i < this->bits.size() || j < other.bits.size(); i++, j++) {
        char n = carry;
        if (i < this->bits.size()) n += this->bits[i];
        if (j < other.bits.size()) n += other.bits[j];
        carry = n / 2;
        result.bits.push_back(n % 2);
    }

    if (carry) result.bits.push_back(1);
    result.delLeadTrailZero(result.bits);

    return result;
}

DA_BIG DA_BIG::operator-(const DA_BIG &other) const {

    if (other.minus) return *this + (-other);
    else if (this->minus) return -(-*this + other);
    else if (*this < other) return -(other - *this);

    DA_BIG result;
    result.minus = this->minus;
    result.bin_precision = std::max(this->bin_precision, other.bin_precision);
    result.point_pos = result.bin_precision;
    bool carry = 0;

    for (int i = 0; i < result.bin_precision; i++) {
        char n = -carry;
        if (i >= result.bin_precision - this->point_pos)
            n += this->bits[i - result.bin_precision + this->point_pos];
        if (i >= result.bin_precision - other.point_pos)
            n -= other.bits[i - result.bin_precision + other.point_pos];
        if (n < 0) {
            n += 2;
            carry = 1;
        } else carry = 0;

        result.bits.push_back(n);
    }

    for (int i = this->point_pos, j = other.point_pos;
         i < this->bits.size() || j < other.bits.size(); i++, j++) {
        char n = -carry;
        if (i < this->bits.size()) n += this->bits[i];
        if (j < other.bits.size()) n -= other.bits[j];
        if (n < 0) {
            n += 2;
            carry = 1;
        } else carry = 0;
        result.bits.push_back(n);
    }

    if (carry) result.bits.push_back(1);
    result.delLeadTrailZero(result.bits);

    return result;
}

DA_BIG DA_BIG::operator*(const DA_BIG &other) const {

    if ((this->bits.empty() || other.bits.empty()) ||
        (this->bits.front() == 0 && this->bits.size() == 1) ||
        (other.bits.front() == 0 && other.bits.size() == 1)) {
        return DA_BIG("0");
    }

    DA_BIG result;
    result.bin_precision = std::max(this->bin_precision, other.bin_precision);
    if (this->bits.size() == 1 && this->bits[0] == 0) result.minus = 0;
    else result.minus = (this->minus != other.minus);
    result.point_pos = this->point_pos + other.point_pos;
    result.bits.resize(this->bits.size() + other.bits.size(), 0);

    for (int i = 0; i < this->bits.size(); i++) {
        short carry = 0;
        for (int j = 0; j < other.bits.size() || carry != 0; j++) {
            short other_bit = (j < other.bits.size()) ? other.bits[j] : 0;
            short number = result.bits[i + j] + this->bits[i] * other_bit + carry;
            result.bits[i + j] = number % 2;
            carry = number / 2;
        }
    }

    result.delLeadTrailZero(result.bits);

    if (result.point_pos > result.bin_precision) {
        int erasing = result.point_pos - result.bin_precision;
        result.bits.erase(result.bits.begin(), result.bits.begin() + erasing);
        result.point_pos -= erasing;
    }

    return result;
}

DA_BIG DA_BIG::operator/(const DA_BIG &other) const {

    DA_BIG dividend = *this, divisor = other;
    
    int eliminate_point = std::max(dividend.point_pos, divisor.point_pos);
    for (int i = 0; i < eliminate_point; i++) {
        if (dividend.point_pos == 0) dividend.bits.insert(dividend.bits.begin(), 0);
        else dividend.point_pos--;
        if (divisor.point_pos == 0) divisor.bits.insert(divisor.bits.begin(), 0);
        else divisor.point_pos--;
    }

    while (divisor.bits.size() > divisor.point_pos + 1 && divisor.bits.back() == 0) divisor.bits.pop_back();
    while (dividend.bits.size() > dividend.point_pos + 1 && dividend.bits.back() == 0) dividend.bits.pop_back();

    DA_BIG result, adding = divisor;
    result.point_pos = 0;
    result.bin_precision = std::max(dividend.bin_precision, divisor.bin_precision);
    if (dividend.bits.size() == 1 && dividend.bits[0] == 0) result.minus = 0;
    else result.minus = (dividend.minus != divisor.minus);
    dividend.minus = 0, divisor.minus = 0, adding.minus = 0;
    
    int div_digits_amount = 0;
    for (auto i = 0; i < (int)(dividend.bits.size() - divisor.bits.size()); i++) {
        adding.bits.insert(adding.bits.begin(), 0);
        div_digits_amount++;
    }

    for (int i = 0; i < div_digits_amount + result.bin_precision + 1; i++) {
        DA_BIG subtrahend = DA_BIG("0", 0);
        char digit = 0;

        while (subtrahend + adding < dividend || subtrahend + adding == dividend) {
            subtrahend = subtrahend + adding;
            digit++;
        }
        
        result.bits.insert(result.bits.begin(), digit);
        dividend = dividend - subtrahend;
        dividend = dividend * DA_BIG("2", 0);
        if (i > div_digits_amount && result.bin_precision) result.point_pos++;
    }

    result.delLeadTrailZero(result.bits);
   
    return result;
}

// ДЕБАГ ИНФО

void DA_BIG::debugInfo() const {

    std::cout << "\nMinus: " << (this->minus ? "yes" : "no")
              << "\nbin_precision: " << this->bin_precision
              << "\nPoint_pos: " << this->point_pos
              << "\nSize in binary: " << bits.size() << "\nIn binary: ";

    int i = 0;
    for (auto it = bits.rbegin(); it != bits.rend(); ++it) {
        if (i++ == bits.size() - point_pos) std::cout << '.';
        std::cout << *it;
    }

    std::cout << std::endl << std::endl;
}

}

LongArithm::DA_BIG operator ""_longnum(long double num) {
    return LongArithm::DA_BIG(std::to_string(num));
}