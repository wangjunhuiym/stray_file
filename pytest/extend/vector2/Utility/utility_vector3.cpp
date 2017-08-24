

#include "utility_vector3.h"


template<> const TVector3<Gfloat> TVector3<Gfloat>::s_zero(0.0f, 0.0f, 0.0f);
template<> const TVector3<Gfloat> TVector3<Gfloat>::s_unit_x(1.0f, 0.0f, 0.0f);
template<> const TVector3<Gfloat> TVector3<Gfloat>::s_unit_y(0.0f, 1.0f, 0.0f);
template<> const TVector3<Gfloat> TVector3<Gfloat>::s_unit_z(0.0f, 0.0f, 1.0f);

template<> const TVector3<Gdouble> TVector3<Gdouble>::s_zero(0.0, 0.0, 0.0);
template<> const TVector3<Gdouble> TVector3<Gdouble>::s_unit_x(1.0, 0.0, 0.0);
template<> const TVector3<Gdouble> TVector3<Gdouble>::s_unit_y(0.0, 1.0, 0.0);
template<> const TVector3<Gdouble> TVector3<Gdouble>::s_unit_z(0.0, 0.0, 1.0);
