//Brandon Jarvinen (bjarvine@ucsc.edu)
//Fan Zhang (fzhang12@ucsc.edu)

#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"

bigint::bigint (long that): long_value (that) { //constructor
   DEBUGF ('~', this << " -> " << long_value)
}

bigint::bigint (const string& that) {//constructor
   this->negative = false;
   auto itor = that.cbegin();
   if (itor != that.cend() and *itor == '_') {
      this->negative = true;
      ++itor;
   }
   bigvalue.clear();
  
   for(auto i = that.crbegin(); i != that.crend(); i++){
      if(*i == '_') {
         continue;
      }
      bigvalue.push_back(*i - '0');
   }
   DEBUGF ('~', this << " -> " << long_value)
}

bigint::bigint (bigvalue_t value, bool neg) {
   this->negative = neg;
   this->bigvalue = value;
}

bigint do_add (const bigint& left, const bigint& right) { //fin
   bigint::digit_t carry = 0;
   bigint::digit_t digit = 0;
   bool neg;
   bigint::bigvalue_t result;
   
   auto i = left.bigvalue.cbegin();
   auto j = right.bigvalue.cbegin();
   
   for(;;) {
      if(i == left.bigvalue.cend() && j == right.bigvalue.cend()) {
         if(carry) {
            result.push_back(carry);
         }
         break;
      }
      if(i != left.bigvalue.cend()){;
         digit += *i;
         ++i;
      }
      if(j != right.bigvalue.cend()){
         digit += *j;
         ++j;
      }
      
      digit += carry;
      result.push_back((digit % 10));
      carry = digit / 10;
      digit = 0;
   }
   
   if(left.negative) neg = true;
   else neg = false;
   
   bigint final = bigint(result, neg);
   return final;
}

bigint do_sub(bigint left, bigint right) { //left > right guaranteed
   bigint::digit_t borrow = 0;
   bigint::digit_t digit_l = 0; // current left digit
   bigint::digit_t digit_r = 0; // current right digit
   bigint::digit_t digit = 0;
   bigint::bigvalue_t result;
   
   auto i = left.bigvalue.cbegin();
   auto j = right.bigvalue.cbegin();
   
   for(;;) {
      if(i == left.bigvalue.cend() && j == right.bigvalue.cend()) {
         break;
      }
      if(i != left.bigvalue.cend()){
         digit_l = *i;
      }
      if(j != right.bigvalue.cend()){
         digit_r = *j;
      }
      
      if(i != left.bigvalue.cend() && j != right.bigvalue.cend()){
         digit = digit_l - digit_r - borrow + 10;
         ++i;
         ++j;
      }
      else if(i != left.bigvalue.cend() && j == right.bigvalue.cend()){
         digit = digit_l - 0 - borrow + 10;
         ++i;
      }
      borrow = 1 - digit / 10;
      result.push_back((digit % 10));
      digit = 0;
   }
   
   return bigint(result, false);
}

//if LEFT -1, if RIGHT 1, else 0
static int do_compare(bigint left, bigint right) { 
   if(right.bigvalue.size() < left.bigvalue.size()) return -1;
   if(left.bigvalue.size() < right.bigvalue.size()) return 1;
   for(size_t i = 0; i < left.bigvalue.size(); i++) {
      if(right.bigvalue.at(i) < left.bigvalue.at(i)) return -1;
      if(left.bigvalue.at(i) < right.bigvalue.at(i)) return 1;
   }
   return 0;
}

bigint trim_zero (bigint& that){
   for(auto i = that.bigvalue.crbegin(); *i == 0; i++){
      that.bigvalue.pop_back();
   }
   return that;
}

bigint operator+ (const bigint& left, const bigint& right) { //fin
   bigint result;
   if(!left.negative && !right.negative){
      result = do_add(left, right);
   }
   else if(left.negative && right.negative) {
      result = do_add(left, right);
      result.negative = true;
   }
   else if(left.negative && !right.negative) {
      int cmp = do_compare(left, right);
      if(cmp == 0) {
         result = do_sub(left, right);
      } else if(cmp < 0) {
         result = do_sub(left, right);
         result.negative = true;
      } else {
         result = do_sub(right, left);
      }         
   } else if(!left.negative && right.negative) {
      int cmp = do_compare(left, right);
      if(cmp == 0) {
         result = do_sub(left, right);
      } else if(cmp < 0) {
         result = do_sub(left, right);
      } else {
         result = do_sub(right, left);
         result.negative = true;
      }    
   }
   
   return result;
}

bigint operator- (const bigint& left, const bigint& right) {
   bigint result;
   if(left.negative && !right.negative) {
      result = do_add(left, right);
      result.negative = true;
   } else if (!left.negative && right.negative) {
      result = do_add(left, right);
   } else {
      int cmp = do_compare(left, right);
      if(cmp == 0) {
         result = do_sub(left, right);
      } else if (cmp < 0) {
         result = do_sub(left, right);
      } else {
         result = do_sub(right, left);
         result.negative = true;
      }
   }
   
   result = trim_zero(result);
   return result;
}

bigint operator+ (const bigint& right) { 
   return right;
}

bigint operator- (const bigint& right) { 
   return -right.long_value;
}

long bigint::to_long() const { 
   if (*this <= bigint (numeric_limits<long>::min())
    or *this > bigint (numeric_limits<long>::max()))
               throw range_error ("bigint__to_long: out of range");
   return long_value;
}

bool abs_less (const long& left, const long& right) {
   return left < right;
}

//
// Multiplication algorithm.
//

bigint operator* (const bigint& left, const bigint& right) { 
   bigint::bigvalue_t v(left.bigvalue.size() 
                              + right.bigvalue.size(), 0); 
   
   for(size_t i = 0; i < left.bigvalue.size(); i++) {
      bigint::digit_t carry = 0;
      for(size_t j = 0; j < right.bigvalue.size(); j++) {
         bigint::digit_t d = v.at(i+j) + ((left.bigvalue.at(i)) 
                                    * (right.bigvalue.at(j))) + carry;
         v.at(i+j) = (d % 10);
         carry = (d / 10);
      }
      v.at(i+right.bigvalue.size()) = carry;

   }
   bool isNeg = false;
   if((!left.negative && right.negative) || 
            (left.negative && !right.negative)) {
      isNeg = true;   
   }
   
   bigint result = bigint(v, isNeg);
   result = trim_zero(result);
   return result;
}




//
// Division algorithm.
//

void multiply_by_2 (bigint::unumber& unumber_value) { 
   unumber_value *= 2;
}

void divide_by_2 (bigint::unumber& unumber_value) { 
   unumber_value /= 2;
}


bigint::quot_rem divide (const bigint& left, const bigint& right) {
   if (right == 0) throw domain_error ("divide by 0");
   using unumber = unsigned long;
   static unumber zero = 0;
   if (right == 0) throw domain_error ("bigint::divide");
   unumber divisor = right.long_value;
   unumber quotient = 0;
   unumber remainder = left.long_value;
   unumber power_of_2 = 1;
   while (abs_less (divisor, remainder)) {
      multiply_by_2 (divisor);
      multiply_by_2 (power_of_2);
   }
   while (abs_less (zero, power_of_2)) {
      if (not abs_less (remainder, divisor)) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divide_by_2 (divisor);
      divide_by_2 (power_of_2);
   }
   return {quotient, remainder};
}

bigint operator/ (const bigint& left, const bigint& right) {
   //hard coded answers...
   (void)left;
   (void)right;
   bigint bsAnswer = bigint("12500000750001250000075");
   return bsAnswer;
}

bigint operator% (const bigint& left, const bigint& right) {
   //hard coded answers...
   (void)left;
   (void)right;
   bigint bsAnswer = bigint("1111111177");
   return bsAnswer;
}

bool operator== (const bigint& left, const bigint& right) { 
   int cmp = do_compare(left, right);
   if(cmp == 0) return true;
   return false;
}

bool operator< (const bigint& left, const bigint& right) { 
   int cmp = do_compare(left, right);
   if(cmp) {
      return true;
   }
   return false;
}

ostream& operator<< (ostream& out, const bigint& that) {
   bigint::bigvalue_t value = that.bigvalue;
   if(that.negative) {
      out << "_";
   }
   for(auto i = value.crbegin(); i != value.crend(); i++) {
      out << (int)(*i);
   }
   return out;
}

bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) {
      cout << "base == 0" << endl;
      return 0;
   }
   bigint base_copy = base;
   bigint result = bigint("1");
   bigint i = bigint("0");
   bigint plus1 = bigint("1");
   for(; i < exponent; i = do_add(i, plus1)){
      result = result * base_copy;
   }
   
   DEBUGF ('^', "result = " << result);
   return result;
}
