#include "longArithmetic.hpp"

namespace LongArithm {

// // КОНСТРУКТОРЫ И ДЕСТРУКТОРЫ
// // ---------------------------------------------------------------------------------------------------- //

DA_BIG::DA_BIG() { // дефолтный конструкор

    point_pos_ = 0;
    precision_ = 100;
    minus_ = false;
}

DA_BIG::~DA_BIG(){} // деструктор

DA_BIG::DA_BIG(const DA_BIG& obj) { // конструктор копирования

    point_pos_ = obj.point_pos_;
    precision_ = obj.precision_;
    minus_ = obj.minus_;
    digits_ = obj.digits_;
}

DA_BIG::DA_BIG(std::string input, const int& precision, const bool& make_binary) { // основной конструктор из строки

    precision_ = precision;

    if (input[0] == '-') {
        minus_ = true;
        input.erase(input.begin());
    } else minus_ = false;

    point_pos_ = input.find('.');
    if (point_pos_ == -1) point_pos_ = input.length(); 
    else input.erase(point_pos_, 1); 

    for (int i = 0; i < input.length(); i++) 
        digits_.push_back(input[i] - '0');
    
    deleteZeroes();
    if (isZero()) minus_ = false;
    
    if (make_binary) decToBin();
}

// // ---------------------------------------------------------------------------------------------------- //

// // ПРИВАТНЫЕ ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ
// // ---------------------------------------------------------------------------------------------------- //

void DA_BIG::decToBin() { // из десятичной в двоичную систему счисления

    DA_BIG int_part, double_part;
    int_part.digits_.assign(digits_.begin(), digits_.begin() + point_pos_);
    double_part.digits_.assign(digits_.begin() + point_pos_, digits_.end());
    int_part.precision_ = 0;
    double_part.precision_ = 0;
    int_part.point_pos_ = int_part.digits_.size();
    double_part.point_pos_ = double_part.digits_.size();

    digits_.clear();
    point_pos_ = 0;

    if (!double_part.digits_.empty()) {
        int dp_len = double_part.digits_.size();
        digits_.resize(precision_ + 1);
        for (int i = precision_; i >= 0 && !double_part.isZero(); i--) {
            double_part = operatorAdd(double_part, double_part, 10);
            if (double_part.digits_.size() > dp_len) {
                digits_[i] = 1;
                double_part.digits_.erase(
                    double_part.digits_.begin(), 
                    double_part.digits_.begin() + 1
                );
                double_part.point_pos_--;
            } 
            else digits_[i] = 0;
        }  
        point_pos_ = digits_.size();
    }
    
    while (!int_part.isZero()) {
        if (int_part.digits_.back() % 2 == 1) {
            digits_.push_back(1);
            int_part.digits_.back() -= 1;
        }
        else digits_.push_back(0);
        int_part = operatorDivide(int_part, DA_BIG("2", 0, false), 10);
    }
    digits_.push_back(0);

    std::reverse(digits_.begin(), digits_.end());
    point_pos_ = digits_.size() - point_pos_;

    roundDouble(precision_, 2);
}

DA_BIG DA_BIG::binToDec() const { // из двоичной в десятичную систему счисления

    DA_BIG result("0", precision_, false), two_pow("1", precision_, false);
    DA_BIG double_denominator, double_numerator("0", precision_, false);

    int i;
    for (i = point_pos_ - 1; i >= 0; i--) {
        if (digits_[i] == 1) 
            result = operatorAdd(result, two_pow, 10);
        two_pow = operatorAdd(two_pow, two_pow, 10);
        if (point_pos_ - (i+1) < (int)digits_.size() - point_pos_ + 1) 
            double_denominator = two_pow;
    }

    for (i = 0; i < (int)digits_.size() - 2 * point_pos_ + 1; i++) 
        double_denominator = operatorAdd(double_denominator, double_denominator, 10);
    for (i = point_pos_; i < digits_.size(); i++) {
        if (digits_[i] == 1) 
            double_numerator = operatorAdd(double_numerator, DA_BIG("1", 0, false), 10);
        double_numerator = operatorAdd(double_numerator, double_numerator, 10);
    }
    result = operatorAdd(
        operatorDivide(double_numerator, double_denominator, 10),
        result, 
        10
    );

    result.minus_ = minus_;

    return result;
}

void DA_BIG::roundDouble(const int& roud_prec, const short& base) { // округление дробной части числа

    precision_ = roud_prec;

    if (roud_prec >= (int)digits_.size() - point_pos_) {
        deleteZeroes();
        return;
    }

    bool carry = (digits_[point_pos_ + roud_prec] >= base / 2);
    digits_.erase(digits_.begin() + point_pos_ + roud_prec, digits_.end());  
    for (int i = digits_.size() - 1; i >= 0 && carry != 0; --i) {
        digits_[i]++;
        carry = digits_[i] / base;
        digits_[i] = digits_[i] % base;
    }
    if (carry) digits_.insert(digits_.begin(), 1);

    deleteZeroes();
}

void DA_BIG::deleteZeroes() { // удалить ведущие и конечные нули незначащие

    int n = 0;
    for (int i = 0; digits_[i] == 0 && i < point_pos_ - 1; i++) n++;
    digits_.erase(digits_.begin(), digits_.begin() + n);
    point_pos_ -= n;

    while (digits_.size() > point_pos_ && digits_.back() == 0) 
        digits_.pop_back();
}

bool DA_BIG::isZero() const { // является ли число нулем?

    if (digits_.size() == 1 && digits_.front() == 0) 
        return true;
    return false;
}

// // ---------------------------------------------------------------------------------------------------- //

// // ОСНОВЫНЫЕ МЕТОДЫ ДЛЯ РАССЧЕТОВ
// // ---------------------------------------------------------------------------------------------------- //

DA_BIG DA_BIG::operatorDivide(DA_BIG dividend, DA_BIG divisor, const short& base) const { // деление

    if (divisor.isZero()) throw std::overflow_error("Fatal error: division by zero!");
    
    if (dividend.isZero()) return DA_BIG("0", 0, false);

    int float_diff = 
        (dividend.digits_.size() - dividend.point_pos_) -
        (divisor.digits_.size() - divisor.point_pos_);
    if (float_diff >= 0) 
        divisor.digits_.insert(divisor.digits_.end(), float_diff, 0);
    else 
        dividend.digits_.insert(dividend.digits_.end(), -float_diff, 0); 
    divisor.point_pos_ = divisor.digits_.size();
    dividend.point_pos_ = dividend.digits_.size();
    dividend.deleteZeroes(), divisor.deleteZeroes();

    DA_BIG result;
    result.precision_ = std::max(dividend.precision_, divisor.precision_);
    result.minus_ = (dividend.minus_ != divisor.minus_);
    dividend.minus_ = false, divisor.minus_ = false;

    DA_BIG adding = divisor;
    int give_zeroes = 0;
    for (auto i = 0; i < (int)dividend.digits_.size() - (int)divisor.digits_.size(); i++) {
        adding.digits_.push_back(0);
        give_zeroes++;
    }
    adding.point_pos_ = adding.digits_.size();
    for (int i = 0; i < give_zeroes + result.precision_ + 1; i++) {
        DA_BIG subtrahend = DA_BIG("0", 0, false);
        short digit = 0;
        
        DA_BIG temp_subtr = adding;
        while (temp_subtr <= dividend) {
            subtrahend = temp_subtr;
            temp_subtr = operatorAdd(temp_subtr, adding, base);
            digit++;
        }
        
        result.digits_.push_back(digit);
        dividend = operatorSubtract(dividend, subtrahend, base);
        dividend.digits_.push_back(0);
        dividend.point_pos_++;
        dividend.deleteZeroes();
        if (i <= give_zeroes) result.point_pos_++;
    }
    result.roundDouble(result.precision_, base);

    return result;
}

DA_BIG DA_BIG::operatorMultiply(DA_BIG first, DA_BIG second, const short& base) const { // умножение

    if (first.isZero() || second.isZero()) return DA_BIG("0", 0, false);

    DA_BIG result;
    result.precision_ = std::max(first.precision_, second.precision_);
    result.minus_ = (first.minus_ != second.minus_);
    first.minus_ = false, second.minus_ = false;
    result.digits_.resize(first.digits_.size() + second.digits_.size(), 0);
    int result_double = 
        first.digits_.size() - first.point_pos_ + 
        second.digits_.size() - second.point_pos_;;

    for (int i = first.digits_.size() - 1; i >= 0; i--) {
        short carry = 0;
        for (int j = second.digits_.size() - 1; j >= 0 || carry != 0; j--) {
            short second_digit = (j >= 0) ? second.digits_[j] : 0;
            short number = result.digits_[i + j + 1] + first.digits_[i] * second_digit + carry;
            result.digits_[i + j + 1] = number % base;
            carry = number / base;
        }
    }
    result.point_pos_ = result.digits_.size() - result_double;
    result.roundDouble(result.precision_, base);

    return result;
}

DA_BIG DA_BIG::operatorAdd(DA_BIG first, DA_BIG second, const short& base) const { // сложение

    if (first.minus_ != second.minus_) {
        if (second.minus_) return first - (-second);
        return second - (-first);
    }

    int float_diff = 
        (first.digits_.size() - first.point_pos_) -
        (second.digits_.size() - second.point_pos_);
    if (float_diff >= 0) 
        second.digits_.insert(second.digits_.end(), float_diff, 0);
    else 
        first.digits_.insert(first.digits_.end(), -float_diff, 0); 

    DA_BIG result;
    result.minus_ = first.minus_;
    result.precision_ = std::max(first.precision_, second.precision_);
    int result_double = first.digits_.size() - first.point_pos_;
    result.digits_.resize(std::max(first.digits_.size(), second.digits_.size()) + 1, 0); 
    
    bool carry = false;
    for (int i = first.digits_.size() - 1, j = second.digits_.size() - 1;
         i >= 0 || j >= 0; 
         i--, j--) {
        short n = carry;
        if (i >= 0) n += first.digits_[i];
        if (j >= 0) n += second.digits_[j];
        carry = n / base;
        result.digits_[std::max(i, j) + 1] = n % base;
    }
    if (carry) result.digits_[0] = 1;
    result.point_pos_ = result.digits_.size() - result_double;
    result.deleteZeroes();

    return result;
}

DA_BIG DA_BIG::operatorSubtract(DA_BIG first, DA_BIG second, const short& base) const { // вычитание

    if (second.minus_) return first + (-second);
    else if (first.minus_) return -(-first + second);
    else if (first < second) return -(second - first);

    int float_diff = 
        (first.digits_.size() - first.point_pos_) -
        (second.digits_.size() - second.point_pos_);
    if (float_diff >= 0) 
        second.digits_.insert(second.digits_.end(), float_diff, 0);
    else 
        first.digits_.insert(first.digits_.end(), -float_diff, 0);
    
    DA_BIG result;
    result.minus_ = first.minus_;
    result.precision_ = std::max(first.precision_, second.precision_);
    int result_double = first.digits_.size() - first.point_pos_;
    result.digits_.resize(std::max(first.digits_.size(), second.digits_.size()), 0); 

    bool carry = false;
    for (int i = first.digits_.size() - 1, j = second.digits_.size() - 1;
         i >= 0 || j >= 0; 
         i--, j--) {
        short n = -carry;
        if (i >= 0) n += first.digits_[i];
        if (j >= 0) n -= second.digits_[j];
        if (n < 0) {
            n += base;
            carry = true;
        } 
        else carry = false;
        result.digits_[std::max(i, j)] = n;
    }
    result.point_pos_ = result.digits_.size() - result_double;
    result.deleteZeroes();

    return result;
}

// // ---------------------------------------------------------------------------------------------------- //

// // ОПЕРАТОРЫ
// // ---------------------------------------------------------------------------------------------------- //

DA_BIG DA_BIG::operator/(const DA_BIG& other) const {

    return operatorDivide(*this, other, 2);
}
DA_BIG& DA_BIG::operator/=(const DA_BIG& other) {

    return *this = *this / other;
}

DA_BIG DA_BIG::operator*(const DA_BIG& other) const {

    return operatorMultiply(*this, other, 2);
}

DA_BIG& DA_BIG::operator*=(const DA_BIG& other) {

    return *this = *this * other;
}

DA_BIG DA_BIG::operator+(const DA_BIG& other) const {

    return operatorAdd(*this, other, 2);
}

DA_BIG& DA_BIG::operator+=(const DA_BIG& other) {

    return *this = *this + other;
}

DA_BIG DA_BIG::operator-(const DA_BIG& other) const {

    return operatorSubtract(*this, other, 2);
}

DA_BIG& DA_BIG::operator-=(const DA_BIG& other) {

    return *this = *this - other;
}

DA_BIG DA_BIG::operator-() const {

    DA_BIG n(*this);
    n.minus_ = !minus_;
    return n;
}

bool DA_BIG::operator==(const DA_BIG& other) const {

    if (minus_ != other.minus_) return false;
    if (point_pos_ != other.point_pos_) return false;
    if (digits_.size() - point_pos_ != other.digits_.size() - other.point_pos_ || 
        point_pos_ != other.point_pos_) return false;

    for (int i = digits_.size(); i >= 0; i--)
        if (digits_[i] != other.digits_[i]) return false;

    return true;
}

bool DA_BIG::operator!=(const DA_BIG& other) const { 

    return !(*this == other); 
}

bool DA_BIG::operator<(const DA_BIG& other) const {

    bool ans = 1;
    if (minus_ && other.minus_) ans = 0;

    if (minus_ && !other.minus_) return ans;
    if (!minus_ && other.minus_) return !ans;

    if (point_pos_ < other.point_pos_) return ans;
    if (point_pos_ > other.point_pos_) return !ans;

    for (int i = 0, j = 0; i < digits_.size() || j < other.digits_.size(); i++, j++) {
        short this_digit = 0, other_digit = 0;
        if (i < digits_.size()) 
            this_digit = digits_[i];
        if (j < other.digits_.size()) 
            other_digit = other.digits_[j];
        if (this_digit != other_digit)
            return ((this_digit ^ (!ans)) < (other_digit ^ (!ans)));
    }

    return false;
}

bool DA_BIG::operator<=(const DA_BIG& other) const {

    bool ans = 1;
    if (minus_ && other.minus_) ans = 0;

    if (minus_ && !other.minus_) return ans;
    if (!minus_ && other.minus_) return !ans;

    if (point_pos_ < other.point_pos_) return ans;
    if (point_pos_ > other.point_pos_) return !ans;

    for (int i = 0, j = 0; i < digits_.size() || j < other.digits_.size(); i++, j++) {
        short this_digit = 0, other_digit = 0;
        if (i < digits_.size()) 
            this_digit = digits_[i];
        if (j < other.digits_.size()) 
            other_digit = other.digits_[j];
        if (this_digit != other_digit)
            return ((this_digit ^ (!ans)) < (other_digit ^ (!ans)));
    }

    return true;
}

bool DA_BIG::operator>(const DA_BIG& other) const { 

    return other < (*this); 
}

bool DA_BIG::operator>=(const DA_BIG& other) const { 

    return other <= (*this); 
}

DA_BIG& DA_BIG::operator=(const DA_BIG& other) { // оператор присваивания

    if (this != &other) {  
        point_pos_ = other.point_pos_;
        precision_ = other.precision_;
        minus_ = other.minus_;
        digits_ = other.digits_;  
    }

    return *this; 
}

// std::ostream& operator<<(std::ostream& os, const DA_BIG& res) { // оператор вывода

//     DA_BIG dec = res.binToDec();

//     dec.roundDouble(50, 10);

//     std::string result;
//     if (dec.minus_) result += '-';
//     for (int i = 0; i < dec.digits_.size(); i++) {
//         if (i == dec.point_pos_) result += '.';
//         result += dec.digits_[i] + '0';
//     }
//     return os << result;
// }

// // ---------------------------------------------------------------------------------------------------- //

// // ДРУГИЕ ПОЛЬЗОВАТЕЛЬСКИЕ МЕТОДЫ
// // ---------------------------------------------------------------------------------------------------- //

void DA_BIG::setBinPrecision(const int precision) { // изменить точность числа

    roundDouble(precision, 2);
}

std::string DA_BIG::toDecimalStr(const int& roud_prec, const bool& in_binary) const { // перевод длинного числа в десятичную строку

    DA_BIG dec = *this;
    if (!in_binary) {
        dec = dec.binToDec();
        dec.roundDouble(roud_prec, 10);
    }
    else dec.roundDouble(roud_prec, 2);

    std::string result;
    if (dec.minus_) result += '-';
    for (int i = 0; i < dec.digits_.size(); i++) {
        if (i == dec.point_pos_) result += '.';
        result += dec.digits_[i] + '0';
    }

    return result;
}

// void DA_BIG::debugInfo() const { // для дебага

//     std::cout << std::endl << std::endl;

//     std::cout << "Minus: " << (minus_ ? "yes" : "no")
//               << "\nPrecision: " << precision_
//               << "\nPoint position: " << point_pos_
//               << "\nDigits: " << digits_.size();
              
//     std::cout << "\nNumber: ";
//     int i = 0;
//     for (auto it = digits_.begin(); it != digits_.end(); ++it) {
//         if (i++ == point_pos_) std::cout << '.';
//         std::cout << (int)(*it);
//     }

//     std::cout << std::endl << std::endl;
// }

// // ---------------------------------------------------------------------------------------------------- //

// // ПИ
// // ---------------------------------------------------------------------------------------------------- //

std::string DA_BIG::calculatePi(const int& output_prec) {

    DA_BIG Pi = DA_BIG::BellardFormula(output_prec);
    DA_BIG dec = Pi.binToDec();
    if (output_prec < (int)dec.digits_.size() - dec.point_pos_) 
        dec.digits_.erase(dec.digits_.begin() + dec.point_pos_ + output_prec, dec.digits_.end());  

    std::string result;
    for (int i = 0; i < dec.digits_.size(); i++) {
        if (i == dec.point_pos_) result += '.';
        result += dec.digits_[i] + '0';
    }

    return result;
}

DA_BIG DA_BIG::BellardFormula(const int& output_prec) { // вычисляем знаки Пи с помощью Bellard's formula
    
    const int calculation_prec = output_prec * 4;
    DA_BIG pi{"0", calculation_prec, false};

    DA_BIG up{"1", calculation_prec, false};
    DA_BIG down{"10000000000", calculation_prec, false};

    DA_BIG up0{"100000", calculation_prec, false};
    DA_BIG up1{"1", calculation_prec, false};
    DA_BIG up2{"100000000", calculation_prec, false};
    DA_BIG up3{"1000000", calculation_prec, false};
    DA_BIG up4{"100", calculation_prec, false};
    DA_BIG up5{"100", calculation_prec, false};
    DA_BIG up6{"1", calculation_prec, false};

    DA_BIG down0{"1", calculation_prec, false};
    DA_BIG down1{"11", calculation_prec, false};
    DA_BIG down2{"1", calculation_prec, false};
    DA_BIG down3{"11", calculation_prec, false};
    DA_BIG down4{"101", calculation_prec, false};
    DA_BIG down5{"111", calculation_prec, false};
    DA_BIG down6{"1001", calculation_prec, false};

    if (output_prec == 0) return DA_BIG{"11", 0, false};


    for (auto k = 0; k < output_prec / 2; ++k) {

        if (k % 2 == 0)
            pi += up*(-up0 / down0 - up1 / down1 + up2 / down2 - up3 / down3 - up4 / down4 - up5 / down5 + up6 / down6);
        else 
            pi -= up*(-up0 / down0 - up1 / down1 + up2 / down2 - up3 / down3 - up4 / down4 - up5 / down5 + up6 / down6);
            
        up /= down;
        down0 += DA_BIG{"100", calculation_prec, false};
        down1 += DA_BIG{"100", calculation_prec, false};
        down2 += DA_BIG{"1010", calculation_prec, false};
        down3 += DA_BIG{"1010", calculation_prec, false};
        down4 += DA_BIG{"1010", calculation_prec, false};
        down5 += DA_BIG{"1010", calculation_prec, false};
        down6 += DA_BIG{"1010", calculation_prec, false};
    }

    pi /= DA_BIG{"1000000", calculation_prec, false};

    return pi;
}

}

// // ---------------------------------------------------------------------------------------------------- //

// // ПОЛЬЗОВАТЕЛЬСКИЕ ЛИТЕРАЛЫ
// // ---------------------------------------------------------------------------------------------------- //

LongArithm::DA_BIG operator ""_longnum(long double num) { // оператор перевода дабла в длинное число

    return LongArithm::DA_BIG(std::to_string(num));
}

LongArithm::DA_BIG operator ""_longnum(unsigned long long num) {

    return LongArithm::DA_BIG(std::to_string(num));
}
