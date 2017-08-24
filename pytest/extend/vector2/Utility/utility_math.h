#ifndef __ANGEOUTILITY_ANGEOMATH_H__
#define __ANGEOUTILITY_ANGEOMATH_H__

#include "utility_base.h"
#include <math.h>

template <class real>
class TMath
{
public:
	static real fabs_(real value)
	{
		return (real)fabs((Gdouble)value);
	}

	static real sin_(real value)
	{
		return (real)sin((Gdouble)value);
	}
    
    static real cos_(real value)
    {
        return (real)sin(TMath<real>::s_half_pi + (Gdouble)value);
    }
    
	static real sinh_(real value)
	{
		return (real)sinh((Gdouble)value);
	}

	static real cosh_(real value)
	{
		return (real)cosh((Gdouble)value);
	}

    static real tan_(real value)
    {
        return (real)tan((Gdouble)value);
    }
    
    static real cot_(real value)
    {
        return (real)1.0 / (real)tan((Gdouble)value);
    }
    
    static void sincos_(real& s, real& c, real angle)
    {
        s = sin_(angle);
        c = cos_(angle);
    }
    
    static real asin_ (real value)
    {
        return real(-1.0) < value ? (value < real(1.0) ? (real)asin((Gdouble)value) : s_half_pi) : -s_half_pi;
    }
    
    static real acos_(real value)
    {
        return real(-1.0) < value ? (value < real(1.0) ? (real)acos((Gdouble)value) : real(0.0)) : s_pi;
    }
    
    static real atan_ (real value)
    {
        return (real)atan((Gdouble)value);
    }
    
    static real atan2_ (real fY, real fX)
    {
        return (real)atan2((Gdouble)fY, (Gdouble)fX);
    }
    
    static real pow_(real fBase, real fExponent)
    {
        return (real)pow((Gdouble)fBase, (Gdouble)fExponent);
    }
    
    static real exp_ (real value)
    {
        return (real)exp((Gdouble)value);
    }
    
    static real log_(real value)
    {
        return (real)log((Gdouble)value);
    }
    
    static real ceil_(real value)
    {
        return (real)ceil((Gdouble)value);
    }

	static real round_(real value)
	{
		return (real)(Gint64)(value+0.5);
	}

    static real floor_ (real value)
    {
        return (real)floor((Gdouble)value);
    }
    
    static real fmod_ (real fX, real fY)
    {
        return (real)fmod((Gdouble)fX, (Gdouble)fY);
    }
    
    static real sqr_ (real value)
    {
        return value * value;
    }
    
    static real sqrt_ (real value)
    {
        return (real)sqrt((Gdouble)value);
    }
    
    static real recip_sqrt_ (real value)
    {
        return (real)(1.0 / sqrt((Gdouble)value));
    }
    
    static real dtor (real value)
    {
        return value * s_deg_to_rad;
    }
    
    static real rtod (real value)
    {
        return value * s_rad_to_deg;
    }
    
    static 	Gbool equal(real a, real b,real tolerance = s_tolerance)
    {
        return TMath<real>::abs_(b - a) <= tolerance ? true : false;
    }
    
    static real min_(real val1, real val2)
    {
        return (val1 < val2) ? val1 : val2;
    }
    
    static real max_(real val1, real val2)
    {
        return (val1 > val2) ? val1 : val2;
    }
    
    static real clamp(real value,real minVal,real maxVal)
    {
        return min_(max_(value, minVal), maxVal);
    }
    
    static real mix(real x,real y,real a)
    {
        return x * real(1.0 - a) + y * a;
    }
    
    static real step(real edge,real value)
    {
        return value < edge ? real(0.0) : real(1.0);
    }
    
    static real linearstep(real minVal,real maxVal,real value)
    {
        return clamp((value - minVal) / (maxVal - minVal), (real)0.0, (real)1.0);
    }
    
    static real smoothstep(real minVal,real maxVal,real value)
    {
        real t = clamp((value - minVal) / (maxVal - minVal), (real)0.0, (real)1.0);
        return t*t*(3 - 2 * t);
    }
    
    static real abs_(real val)
    {
        return val < 0 ? (-val) : val;
    }
    
	static Gint32 rand_()
	{
		return rand();
	}

    static void swap_(real& a, real& b)
    {
        real c = a;
        a = b;
        b = c;
    }
    
    static Gbool is_power_two(Gint32 value)
    {
        return (value & (value - 1)) == 0 && value > 0;
    }
    
public:
    static const real s_tolerance;
    
    static const real s_max_real;
    
    static const real s_min_real;
    
    static const real s_pi;
    
    static const real s_two_pi;
    
    static const real s_half_pi;
    
    static const real s_deg_to_rad;
    
    static const real s_rad_to_deg;

	static const real s_epsilon;

	static const real s_epsilonRatio;

	static const real s_epsilonAngle;
};

typedef TMath<Gfloat> TMath_f;
typedef TMath<Gdouble> TMath_d;


#endif //_US_MATH_H_