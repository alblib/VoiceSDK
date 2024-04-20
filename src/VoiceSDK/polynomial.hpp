#ifndef __H_VOICE_SDK_POLYNOMIAL_HPP
#define __H_VOICE_SDK_POLYNOMIAL_HPP __TIMESTAMP__

#include "definitions.hpp"

#include <valarray>

namespace VoiceSDK
{

template <class Coefficient>
class Polynomial
{
private:
public:
    using coefficient_type = Coefficient;
    std::valarray<coefficient_Type> coefficients;

    //Polynomial operator + (const Polynomial& other) const {

    //}
};

}

#endif