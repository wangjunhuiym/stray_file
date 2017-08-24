

#include "utility_math.h"

template<> const Gfloat TMath<Gfloat>::s_tolerance = 1e-06f;
template<> const Gfloat TMath<Gfloat>::s_min_real = 1.175494351e-38F;
template<> const Gfloat TMath<Gfloat>::s_max_real = 3.402823466e+38F;
template<> const Gfloat TMath<Gfloat>::s_pi = 3.1415926535897932f;
template<> const Gfloat TMath<Gfloat>::s_two_pi = (Gfloat)(TMath<Gfloat>::s_pi * 2.0f);
template<> const Gfloat TMath<Gfloat>::s_half_pi = (Gfloat)(TMath<Gfloat>::s_pi * 0.5f);
template<> const Gfloat TMath<Gfloat>::s_deg_to_rad = (Gfloat)(TMath<Gfloat>::s_pi / 180.0f);
template<> const Gfloat TMath<Gfloat>::s_rad_to_deg = (Gfloat)(180.0 / TMath<Gfloat>::s_pi);
template<> const Gfloat TMath<Gfloat>::s_epsilon = 0.001f;
template<> const Gfloat TMath<Gfloat>::s_epsilonRatio = 0.01f;
template<> const Gfloat TMath<Gfloat>::s_epsilonAngle = 0.08726646259971647884618453842443f;

template<> const Gdouble TMath<Gdouble>::s_tolerance = 1e-13;
template<> const Gdouble TMath<Gdouble>::s_min_real = 2.2250738585072014e-308;
template<> const Gdouble TMath<Gdouble>::s_max_real = 1.7976931348623158e+308;
template<> const Gdouble TMath<Gdouble>::s_pi = 3.1415926535897932384626433832795;
template<> const Gdouble TMath<Gdouble>::s_two_pi = TMath<Gdouble>::s_pi * 2.0;
template<> const Gdouble TMath<Gdouble>::s_half_pi = TMath<Gdouble>::s_pi * 0.5;
template<> const Gdouble TMath<Gdouble>::s_deg_to_rad = TMath<Gdouble>::s_pi / 180.0;
template<> const Gdouble TMath<Gdouble>::s_rad_to_deg = 180.0 / TMath<Gdouble>::s_pi;
template<> const Gdouble TMath<Gdouble>::s_epsilon = 0.001f;
template<> const Gdouble TMath<Gdouble>::s_epsilonRatio = 0.01f;
template<> const Gdouble TMath<Gdouble>::s_epsilonAngle = 0.08726646259971647884618453842443f;

