#ifndef ANGEOUTILITY_MATRIX4X4_H_
#define ANGEOUTILITY_MATRIX4X4_H_

#include "utility_vector3.h"
#include "utility_assert.h"

#ifdef WIN32
#pragma warning(disable:4244) // double to Gfloat
#endif

//------------------------------------------------------------------------------
template<typename N>
class TMatrix4x4
{
public:
	static const TMatrix4x4 s_identity;
	static const TMatrix4x4 s_zero;

public:
	typedef N ValueType;
public:
	TMatrix4x4();
	TMatrix4x4(const N* s);
	TMatrix4x4(
		N f11, N f12, N f13, N f14,
		N f21, N f22, N f23, N f24,
		N f31, N f32, N f33, N f34,
		N f41, N f42, N f43, N f44);
public:
	TMatrix4x4 operator - () const;
	Gbool operator == (const TMatrix4x4& m) const;
	Gbool operator != (const TMatrix4x4& m) const;
	TMatrix4x4 operator * (const TMatrix4x4& m ) const;
	TMatrix4x4& operator += (const TMatrix4x4& m);
	TMatrix4x4& operator -= (const TMatrix4x4& m);
	TMatrix4x4& operator *= (const TMatrix4x4& m);
	TMatrix4x4& operator *= (N s);
	TMatrix4x4& operator /= (N s);
	N& operator () (Gint32 row, Gint32 col);
	N operator () (Gint32 row, Gint32 col) const;
	operator N* ();
	operator const N* () const;
	Gint32 I(Gint32 row, Gint32 col);

	TVector3<N> operator * (const TVector3<N>& vec) const;

	// mat4 * vector3 -----> tanslate coordinate
	void Identity()
	{
		m12 = m13 = m14 =
		m21 = m23 = m24 =
		m31 = m32 = m34 =
		m41 = m42 = m43 = ValueType(0);
		m11 = m22 = m33 = m44 = ValueType(1);
	}

public:
	// rotate by x
	static TMatrix4x4 RotateAxisX( N radian_value);

	// rotate by y
	static TMatrix4x4 RotateAxisY( N radian_value);

	// rotate by z
	static TMatrix4x4 RotateAxisZ( N radian_value);

	// translation
	static TMatrix4x4 Translation( N x, N y, N z );

	// scale
	static TMatrix4x4 Scaling( N x, N y, N z );

	// inverse
	static TMatrix4x4 Inverse( const TMatrix4x4<N>& mat );

public:
	union
	{
		struct
		{
			N m11, m12, m13, m14;
			N m21, m22, m23, m24;
			N m31, m32, m33, m34;
			N m41, m42, m43, m44;

		};
		N m[4][4];
		N mat[16];
	};
};

//------------------------------------------------------------------------------
typedef TMatrix4x4<Gfloat> Matrix4x4f;
typedef TMatrix4x4<Gdouble> Matrix4x4d;

//------------------------------------------------------------------------------
template<typename N>
inline TMatrix4x4<N>::TMatrix4x4()
{
	Identity();
}

template<typename N>
inline TMatrix4x4<N>::TMatrix4x4(const N* s)
{
	memcpy(&m11, s, sizeof(TMatrix4x4<N>));
}

template<typename N>
inline TMatrix4x4<N>::TMatrix4x4(
							   N f11, N f12, N f13, N f14,
							   N f21, N f22, N f23, N f24,
							   N f31, N f32, N f33, N f34,
							   N f41, N f42, N f43, N f44)
{
	m11 = f11; m12 = f12; m13 = f13; m14 = f14;
	m21 = f21; m22 = f22; m23 = f23; m24 = f24;
	m31 = f31; m32 = f32; m33 = f33; m34 = f34;
	m41 = f41; m42 = f42; m43 = f43; m44 = f44;
}

template<typename N>
inline Gbool TMatrix4x4<N>::operator == (const TMatrix4x4<N>& m) const
{
	return 0 == memcmp(this, &m, sizeof(TMatrix4x4<N>));
}

template<typename N>
inline Gbool TMatrix4x4<N>::operator != (const TMatrix4x4<N>& m) const
{
	return !(operator == (m));
}

template<typename N>
inline TMatrix4x4<N>& TMatrix4x4<N>::operator += (const TMatrix4x4<N>& mat)
{
	m11 += mat.m11; m12 += mat.m12; m13 += mat.m13; m14 += mat.m14;
	m21 += mat.m21; m22 += mat.m22; m23 += mat.m23; m24 += mat.m24;
	m31 += mat.m31; m32 += mat.m32; m33 += mat.m33; m34 += mat.m34;
	m41 += mat.m41; m42 += mat.m42; m43 += mat.m43; m44 += mat.m44;
	return *this;
}

template<typename N>
inline TMatrix4x4<N>& TMatrix4x4<N>::operator -= (const TMatrix4x4<N>& mat)
{
	m11 -= mat.m11; m12 -= mat.m12; m13 -= mat.m13; m14 -= mat.m14;
	m21 -= mat.m21; m22 -= mat.m22; m23 -= mat.m23; m24 -= mat.m24;
	m31 -= mat.m31; m32 -= mat.m32; m33 -= mat.m33; m34 -= mat.m34;
	m41 -= mat.m41; m42 -= mat.m42; m43 -= mat.m43; m44 -= mat.m44;
	return *this;
}


template<typename N>
inline TMatrix4x4<N> TMatrix4x4<N>::operator * (const TMatrix4x4& mat) const
{
	TMatrix4x4 mat_ret;
	for ( Gint32 row = 0; row < 4; row++ )
	{
		for ( Gint32 col = 0; col < 4; col++ )
		{
			mat_ret.m[row][col] = (N)0.0;
			for ( Gint32 mid = 0; mid < 4; mid++ )
			{
				mat_ret.m[row][col] += m[row][mid] * mat.m[mid][col];
			}
		}
	}
	return mat_ret;
}

template<typename N>
inline TMatrix4x4<N>& TMatrix4x4<N>::operator *= (N s)
{
	m11 *= s; m12 *= s; m13 *= s; m14 *= s;
	m21 *= s; m22 *= s; m23 *= s; m24 *= s;
	m31 *= s; m32 *= s; m33 *= s; m34 *= s;
	m41 *= s; m42 *= s; m43 *= s; m44 *= s;
	return *this;
}

template<typename N>
inline TMatrix4x4<N>& TMatrix4x4<N>::operator /= (N s)
{
	s = N(1)/s;
	return operator*=(s);
}

template<typename N>
inline N& TMatrix4x4<N>::operator () (Gint32 row, Gint32 col)
{
	UTILITY_ASSERT(row < 4 && col < 4);
	return m[row][col];
}

template<typename N>
inline N TMatrix4x4<N>::operator () (Gint32 row, Gint32 col) const
{
	UTILITY_ASSERT(row < 4 && col < 4);
	return m[row][col];
}

template<typename N>
inline TMatrix4x4<N> operator + (const TMatrix4x4<N>& mat1, const TMatrix4x4<N>& mat2)
{
	return TMatrix4x4<N>(
		mat1.m11 + mat2.m11, mat1.m12 + mat2.m12, mat1.m13 + mat2.m13, mat1.m14 + mat2.m14, 
		mat1.m21 + mat2.m21, mat1.m22 + mat2.m22, mat1.m23 + mat2.m23, mat1.m24 + mat2.m24, 
		mat1.m31 + mat2.m31, mat1.m32 + mat2.m32, mat1.m33 + mat2.m33, mat1.m34 + mat2.m34, 
		mat1.m41 + mat2.m41, mat1.m42 + mat2.m42, mat1.m43 + mat2.m43, mat1.m44 + mat2.m44);
}

template<typename N>
inline TMatrix4x4<N> operator - (const TMatrix4x4<N>& mat1, const TMatrix4x4<N>& mat2)
{
	return TMatrix4x4<N>(
		mat1.m11 - mat2.m11, mat1.m12 - mat2.m12, mat1.m13 - mat2.m13, mat1.m14 - mat2.m14, 
		mat1.m21 - mat2.m21, mat1.m22 - mat2.m22, mat1.m23 - mat2.m23, mat1.m24 - mat2.m24, 
		mat1.m31 - mat2.m31, mat1.m32 - mat2.m32, mat1.m33 - mat2.m33, mat1.m34 - mat2.m34, 
		mat1.m41 - mat2.m41, mat1.m42 - mat2.m42, mat1.m43 - mat2.m43, mat1.m44 - mat2.m44);
}

template<typename N>
inline TMatrix4x4<N> operator * (const TMatrix4x4<N>& mat, N s)
{
	return TMatrix4x4<N>(
		mat.m11 * s, mat.m12 * s, mat.m13 * s, mat.m14 * s, 
		mat.m21 * s, mat.m22 * s, mat.m23 * s, mat.m24 * s, 
		mat.m31 * s, mat.m32 * s, mat.m33 * s, mat.m34 * s, 
		mat.m41 * s, mat.m42 * s, mat.m43 * s, mat.m44 * s);
}

template<typename N>
inline TMatrix4x4<N> operator * (N s, const TMatrix4x4<N>& mat)
{
	return operator*(mat, s);
}

template<typename N>
inline TMatrix4x4<N> operator / (const TMatrix4x4<N>& mat, N s)
{
	s = N(1)/s;
	return operator*(mat, s);
}

template<typename N>
inline TMatrix4x4<N>::operator N* ()
{
	return &m11;
}

template<typename N>
inline TMatrix4x4<N>::operator const N* () const
{
	return &m11;
}

template<typename N>
TVector3<N> TMatrix4x4<N>::operator* (const TVector3<N>& vec) const
{
	TVector3<N> vOut;
	vOut.x = vec.x * m11 + vec.y * m21 + vec.z * m31 + m41;
	vOut.y = vec.x * m12 + vec.y * m22 + vec.z * m32 + m42;
	vOut.z = vec.x * m13 + vec.y * m23 + vec.z * m33 + m43;
	return vOut;
}

template<typename N>
inline Gint32 TMatrix4x4<N>::I ( Gint32 row, Gint32 col )
{
	UTILITY_ASSERT(0 <= row && row < 4 && 0 <= col && col < 4);
	return col + row * 4;
}

// rotate by x
template <typename N>
TMatrix4x4<N> TMatrix4x4<N>::RotateAxisX( N radian_value)
{
	TMatrix4x4<N> ret_mat(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);
	ret_mat.m22 = ret_mat.m33 = (N)TMath<N>::cos_(radian_value);
	ret_mat.m23 = ret_mat.m32 = (N)TMath<N>::sin_(radian_value);
	ret_mat.m32 = -ret_mat.m32;
	return ret_mat;
}

// rotate by y
template <typename N>
TMatrix4x4<N> TMatrix4x4<N>::RotateAxisY( N radian_value)
{
	TMatrix4x4<N> ret_mat(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);
	ret_mat.m11 = ret_mat.m33 = (N)TMath<N>::cos_(radian_value);
	ret_mat.m13 = ret_mat.m31 = (N)TMath<N>::sin_(radian_value);
	ret_mat.m13 = -ret_mat.m13;
	return ret_mat;
}



// rotate by z
template <typename N>
TMatrix4x4<N> TMatrix4x4<N>::RotateAxisZ( N radian_value)
{
	TMatrix4x4<N> ret_mat(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);
	ret_mat.m11 = ret_mat.m22 = (N)TMath<N>::cos_(radian_value);
	ret_mat.m12 = ret_mat.m21 = (N)TMath<N>::sin_(radian_value);
	ret_mat.m21 = -ret_mat.m21;
	return ret_mat;
}


// tranlate to x ,y ,z
template <typename N>
TMatrix4x4<N> TMatrix4x4<N>::Translation( N x, N y, N z )
{
	TMatrix4x4<N> ret_mat(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);
	ret_mat.m41 = x;
	ret_mat.m42 = y;
	ret_mat.m43 = z;
	return ret_mat;
}

// scale
template <typename N>
TMatrix4x4<N> TMatrix4x4<N>::Scaling(N x, N y, N z)
{
	TMatrix4x4<N> ret_mat(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);
	ret_mat.m11 = x;
	ret_mat.m22 = y;
	ret_mat.m33 = z;
	return ret_mat;
}

// inverse
template <typename N>
TMatrix4x4<N> TMatrix4x4<N>::Inverse( const TMatrix4x4<N>& mat )
{
	N a0 = mat.mat[0] * mat.mat[5] - mat.mat[1] * mat.mat[4];
	N a1 = mat.mat[0] * mat.mat[6] - mat.mat[2] * mat.mat[4];
	N a2 = mat.mat[0] * mat.mat[7] - mat.mat[3] * mat.mat[4];
	N a3 = mat.mat[1] * mat.mat[6] - mat.mat[2] * mat.mat[5];
	N a4 = mat.mat[1] * mat.mat[7] - mat.mat[3] * mat.mat[5];
	N a5 = mat.mat[2] * mat.mat[7] - mat.mat[3] * mat.mat[6];

	N b0 = mat.mat[8]  * mat.mat[13] - mat.mat[9]  * mat.mat[12];
	N b1 = mat.mat[8]  * mat.mat[14] - mat.mat[10] * mat.mat[12];
	N b2 = mat.mat[8]  * mat.mat[15] - mat.mat[11] * mat.mat[12];
	N b3 = mat.mat[9]  * mat.mat[14] - mat.mat[10] * mat.mat[13];
	N b4 = mat.mat[9]  * mat.mat[15] - mat.mat[11] * mat.mat[13];
	N b5 = mat.mat[10] * mat.mat[15] - mat.mat[11] * mat.mat[14];

	N det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
	if (TMath<N>::fabs_(det) <= TMath<N>::s_tolerance)
		return TMatrix4x4<N>::s_zero;

	TMatrix4x4 mat_ret;
	mat_ret.mat[0]  = + mat.mat[5]  * b5 - mat.mat[6]  * b4 + mat.mat[7]  * b3;
	mat_ret.mat[4]  = - mat.mat[4]  * b5 + mat.mat[6]  * b2 - mat.mat[7]  * b1;
	mat_ret.mat[8]  = + mat.mat[4]  * b4 - mat.mat[5]  * b2 + mat.mat[7]  * b0;
	mat_ret.mat[12] = - mat.mat[4]  * b3 + mat.mat[5]  * b1 - mat.mat[6]  * b0;
	mat_ret.mat[1]  = - mat.mat[1]  * b5 + mat.mat[2]  * b4 - mat.mat[3]  * b3;
	mat_ret.mat[5]  = + mat.mat[0]  * b5 - mat.mat[2]  * b2 + mat.mat[3]  * b1;
	mat_ret.mat[9]  = - mat.mat[0]  * b4 + mat.mat[1]  * b2 - mat.mat[3]  * b0;
	mat_ret.mat[13] = + mat.mat[0]  * b3 - mat.mat[1]  * b1 + mat.mat[2]  * b0;
	mat_ret.mat[2]  = + mat.mat[13] * a5 - mat.mat[14] * a4 + mat.mat[15] * a3;
	mat_ret.mat[6]  = - mat.mat[12] * a5 + mat.mat[14] * a2 - mat.mat[15] * a1;
	mat_ret.mat[10] = + mat.mat[12] * a4 - mat.mat[13] * a2 + mat.mat[15] * a0;
	mat_ret.mat[14] = - mat.mat[12] * a3 + mat.mat[13] * a1 - mat.mat[14] * a0;
	mat_ret.mat[3]  = - mat.mat[9]  * a5 + mat.mat[10] * a4 - mat.mat[11] * a3;
	mat_ret.mat[7]  = + mat.mat[8]  * a5 - mat.mat[10] * a2 + mat.mat[11] * a1;
	mat_ret.mat[11] = - mat.mat[8]  * a4 + mat.mat[9]  * a2 - mat.mat[11] * a0;
	mat_ret.mat[15] = + mat.mat[8]  * a3 - mat.mat[9]  * a1 + mat.mat[10] * a0;

	N inv_det = ((N)1.0) / det;
	for ( Gint32 row = 0; row < 4; row++ )
	{
		for (Gint32 col = 0; col < 4; col++)
		{
			mat_ret.mat[(row<<2)+col] *= inv_det;
		}
	}

	return mat_ret;
}

//------------------------------------------------------------------------------

template<typename M>
void Matrix4x4Identity(M& mOut);

template<typename M, typename M1>
void Matrix4x4Assign(M& mOut, const M1& m);

template<typename M, typename V>
void Matrix4x4SetRow(M& mOut, Gint32 n, const V& v);

template<typename M, typename V>
void Matrix4x4SetCol(M& mOut, Gint32 n, const V& v);

template<typename M>
Gbool Matrix4x4IsIdentity(const M& mat);

template<typename M, typename M1, typename M2>
void Matrix4x4Multiply(M& mOut, const M1& mat1, const M2& mat2);

template<typename M>
void Matrix4x4Transpose(Matrix4x4f& mOut);

template<typename M, typename M1>
void Matrix4x4Transpose(M& mOut, const M1& mat);

template<typename M, typename M1>
void Matrix4x4Inverse(M& mOut, const M1& mat);

template<typename M, typename N>
void Matrix4x4Scaling(M& mOut, N s);

template<typename M, typename N>
void Matrix4x4Scaling(M& mOut, N x, N y, N z);

template<typename M, typename N>
void Matrix4x4Translation(M& mOut, N x, N y, N z);

template<typename M, typename N>
void Matrix4x4RotationX(M& mOut, N angle);

template<typename M, typename N>
void Matrix4x4RotationY(M& mOut, N angle);

template<typename M, typename N>
void Matrix4x4RotationZ(M& mOut, N angle);

template<typename M, typename N>
void Matrix4x4RotationYawPitchRoll(M& mOut, N yaw, N pitch, N roll);

template<typename M, typename N>
void Matrix4x4PerspectiveRH(M& mOut, N w, N h, N zn, N zf);

template<typename M, typename N>
void Matrix4x4PerspectiveLH(M& mOut, N w, N h, N zn, N zf);

template<typename M, typename N>
void Matrix4x4PerspectiveFovRH(M& mOut, N fovy, N aspect, N zn, N zf);

template<typename M, typename N>
void Matrix4x4PerspectiveFovLH(M& mOut, N fovy, N aspect, N zn, N zf);

template<typename M, typename N>
void Matrix4x4PerspectiveOffCenterRH(M& mOut, N l, N r, N b, N t, N zn, N zf);

template<typename M, typename N>
void Matrix4x4PerspectiveOffCenterLH(M& mOut, N l, N r, N b, N t, N zn, N zf);

template<typename M, typename N>
void Matrix4x4OrthoRH(M& mOut, N w, N h, N zn, N zf);

template<typename M, typename N>
void Matrix4x4OrthoLH(M& mOut, N w, N h, N zn, N zf);

template<typename M, typename N>
void Matrix4x4OrthoOffCenterRH(M& mOut, N l, N r, N b, N t, N zn, N zf);

template<typename M, typename N>
void Matrix4x4OrthoOffCenterLH(M& mOut, N l, N r, N b, N t, N zn, N zf);

template<typename M, typename V, typename N>
void Matrix4x4RotationAxis(M& mOut, const V& axis, N angle);

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
template<typename M>
inline void Matrix4x4Identity(M& mat)
{
	mat.m12 = mat.m13 = mat.m14 =
	mat.m21 = mat.m23 = mat.m24 =
	mat.m31 = mat.m32 = mat.m34 =
	mat.m41 = mat.m42 = mat.m43 = typename M::ValueType(0);
	mat.m11 = mat.m22 = mat.m33 = mat.m44 = typename M::ValueType(1);
}

template<typename M, typename M1>
inline void Matrix4x4Assign(M& mOut, const M1& m)
{
	mOut.m11 = m.m11;
	mOut.m12 = m.m12;
	mOut.m13 = m.m13;
	mOut.m14 = m.m14;
	mOut.m21 = m.m21;
	mOut.m22 = m.m22;
	mOut.m23 = m.m23;
	mOut.m24 = m.m24;
	mOut.m31 = m.m31;
	mOut.m32 = m.m32;
	mOut.m33 = m.m33;
	mOut.m34 = m.m34;
	mOut.m41 = m.m41;
	mOut.m42 = m.m42;
	mOut.m43 = m.m43;
	mOut.m44 = m.m44;
}

template<typename M>
inline Gbool Matrix4x4IsIdentity(M& mat)
{
	M srcMat;
	Matrix4x4Identity( srcMat );

	return !( mat != srcMat );
}

template<typename M, typename V>
inline void Matrix4x4SetRow(M& mOut, Gint32 n, const V& v)
{
	mOut.m[n][0] = v.x;
	mOut.m[n][1] = v.y;
	mOut.m[n][2] = v.z;
	mOut.m[n][3] = v.w;
}

template<typename M, typename V>
void Matrix4x4SetCol(M& mOut, Gint32 n, const V& v)
{
	mOut.m[0][n] = v.x;
	mOut.m[1][n] = v.y;
	mOut.m[2][n] = v.z;
	mOut.m[3][n] = v.w;
}


template<typename M>
inline void Matrix4x4Transpose(Matrix4x4f& mOut)
{
	typename M::ValueType tmp;
	tmp = mOut.m12; mOut.m12 = mOut.m21; mOut.m21 = tmp;
	tmp = mOut.m13; mOut.m13 = mOut.m31; mOut.m31 = tmp;
	tmp = mOut.m14; mOut.m14 = mOut.m41; mOut.m41 = tmp;
	tmp = mOut.m23; mOut.m23 = mOut.m32; mOut.m32 = tmp;
	tmp = mOut.m24; mOut.m24 = mOut.m42; mOut.m42 = tmp;
	tmp = mOut.m34; mOut.m34 = mOut.m43; mOut.m43 = tmp;
}

template<typename M, typename M1>
inline void Matrix4x4Transpose(M& mOut, const M1& mat)
{
	if(&mat == &mOut)
	{
		typename M::ValueType tmp;
		tmp = mOut.m12; mOut.m12 = mOut.m21; mOut.m21 = tmp;
		tmp = mOut.m13; mOut.m13 = mOut.m31; mOut.m31 = tmp;
		tmp = mOut.m14; mOut.m14 = mOut.m41; mOut.m41 = tmp;
		tmp = mOut.m23; mOut.m23 = mOut.m32; mOut.m32 = tmp;
		tmp = mOut.m24; mOut.m24 = mOut.m42; mOut.m42 = tmp;
		tmp = mOut.m34; mOut.m34 = mOut.m43; mOut.m43 = tmp;
	}
	else
	{
		mOut.m11 = mat.m11; mOut.m12 = mat.m21; mOut.m13 = mat.m31; mOut.m14 = mat.m41;
		mOut.m21 = mat.m12; mOut.m22 = mat.m22; mOut.m23 = mat.m32; mOut.m24 = mat.m42;
		mOut.m31 = mat.m13; mOut.m32 = mat.m23; mOut.m33 = mat.m33; mOut.m34 = mat.m43;
		mOut.m41 = mat.m14; mOut.m42 = mat.m24; mOut.m43 = mat.m34; mOut.m44 = mat.m44;
	}
}

template<typename M, typename N>
inline void Matrix4x4Scaling(M& mOut, N s)
{
	mOut.m43 = mOut.m42 = mOut.m41 = mOut.m34 = mOut.m32 = mOut.m31 = mOut.m24 = mOut.m23 = mOut.m21 = mOut.m14 = mOut.m13 = mOut.m12 = N(0);
	mOut.m33 = mOut.m22 = mOut.m11 = s;
	mOut.m44 = N(1);
}

template<typename M, typename N>
inline void Matrix4x4Scaling(M& mOut, N x, N y, N z)
{
	mOut.m43 = mOut.m42 = mOut.m41 = mOut.m34 = mOut.m32 = mOut.m31 = mOut.m24 = mOut.m23 = mOut.m21 = mOut.m14 = mOut.m13 = mOut.m12 = N(0);
	mOut.m11 = x;
	mOut.m22 = y;
	mOut.m33 = z;
	mOut.m44 = N(1);
}

template<typename M, typename N>
inline void Matrix4x4Translation(M& mOut, N x, N y, N z)
{
	mOut.m34 = mOut.m32 = mOut.m31 = mOut.m24 = mOut.m23 = mOut.m21 = mOut.m14 = mOut.m13 = mOut.m12 = N(0);
	mOut.m41 = x;
	mOut.m42 = y;
	mOut.m43 = z;
	mOut.m44 = mOut.m33 = mOut.m22 = mOut.m11 = N(1);
}

template<typename M, typename N>
inline void Matrix4x4RotationX(M& mOut, N angle)
{
	typename M::ValueType sinTheta, cosTheta;
	sinTheta = TMath<N>::sin_(angle);
	cosTheta = TMath<N>::cos_(angle);
	mOut.m22 = cosTheta;
	mOut.m23 = sinTheta;
	mOut.m32 = -sinTheta;
	mOut.m33 = cosTheta;
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m24 = mOut.m31 = mOut.m34 = mOut.m41 = mOut.m42 = mOut.m43 = M::ValueType(0);
	mOut.m11 = mOut.m44 = M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4RotationY(M& mOut, N angle)
{
	typename M::ValueType sinTheta, cosTheta;
	sinTheta = TMath<N>::sin_(angle);
	cosTheta = TMath<N>::cos_(angle);
	mOut.m11 = cosTheta;
	mOut.m13 = -sinTheta;
	mOut.m31 = sinTheta;
	mOut.m33 = cosTheta;
	mOut.m12 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m32 = mOut.m34 = mOut.m41 = mOut.m42 = mOut.m43 = M::ValueType(0);
	mOut.m22 = mOut.m44 = M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4RotationZ(M& mOut, N angle)
{
	typename M::ValueType sinTheta, cosTheta;
	sinTheta = TMath<N>::sin_(angle);
	cosTheta = TMath<N>::cos_(angle);
	mOut.m11 = cosTheta;
	mOut.m12 = sinTheta;
	mOut.m21 = -sinTheta;
	mOut.m22 = cosTheta;
	mOut.m13 = mOut.m14 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m34 = mOut.m41 = mOut.m42 = mOut.m43 = typename M::ValueType(0);
	mOut.m33 = mOut.m44 = typename M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4RotationYawPitchRoll(M& mOut, N yaw, N pitch, N roll)
{
	typename M::ValueType sinYaw, cosYaw, sinPitch, cosPitch, sinRoll,cosRoll;
	sinYaw = TMath<N>::sin_(yaw);
	cosYaw = TMath<N>::cos_(yaw);
	sinPitch = TMath<N>::sin_(pitch);
	cosPitch = TMath<N>::cos_(pitch);
	sinRoll = TMath<N>::sin_(roll);
	cosRoll = TMath<N>::cos_(roll);
	mOut.m14 = mOut.m24 = mOut.m34 = mOut.m41 = mOut.m42 = mOut.m43 = M::ValueType(0);
	mOut.m44 = M::ValueType(1);
	mOut.m11 = cosRoll * cosYaw - sinRoll * sinPitch * sinYaw;
	mOut.m12 = sinRoll * cosYaw + cosRoll * sinPitch * sinYaw;
	mOut.m13 = -cosPitch * sinYaw;
	mOut.m21 = -sinRoll * cosPitch;
	mOut.m22 = cosRoll * cosPitch;
	mOut.m23 = sinPitch;
	mOut.m31 = cosRoll * sinYaw + sinRoll * sinPitch * cosYaw;
	mOut.m32 = sinRoll * sinYaw - cosRoll * sinPitch * cosYaw;
	mOut.m33 = cosPitch * cosYaw;
}

template<typename M, typename N>
inline void Matrix4x4PerspectiveRH(M& mOut, N w, N h, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m41 = mOut.m42 = mOut.m44 = M::ValueType(0);
	mOut.m11 = M::ValueType(2) * zn / w;
	mOut.m22 = M::ValueType(2) * zn / h;
	mOut.m33 = zf / (zn - zf);
	mOut.m43 = zn * zf / (zn - zf);
	mOut.m34 = -M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4PerspectiveLH(M& mOut, N w, N h, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m41 = mOut.m42 = mOut.m44 = M::ValueType(0);
	mOut.m11 = M::ValueType(2) * zn / w;
	mOut.m22 = M::ValueType(2) * zn / h;
	mOut.m33 = zf / (zf - zn);
	mOut.m43 = zn * zf / (zn - zf);
	mOut.m34 = M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4PerspectiveFovRH(M& mOut, N fovy, N aspect, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m41 = mOut.m42 = mOut.m44 = typename M::ValueType(0);
	mOut.m22 = typename M::ValueType(1) / (Gfloat)TMath<N>::tan_(fovy * 0.5f);
	mOut.m11 = mOut.m22 / aspect;
	mOut.m33 = zf / (zn - zf);
	mOut.m43 = zn * zf / (zn - zf);
	mOut.m34 = typename M::ValueType(-1);
}

template<typename M, typename N>
inline void Matrix4x4PerspectiveFovLH(M& mOut, N fovy, N aspect, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m41 = mOut.m42 = mOut.m44 = M::ValueType(0);
	mOut.m22 = M::ValueType(1) / (Gfloat)TMath<N>::tan_(fovy * 0.5f);
	mOut.m11 = mOut.m22 / aspect;
	mOut.m33 = zf / (zf - zn);
	mOut.m43 = zn * zf / (zn - zf);
	mOut.m34 = M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4PerspectiveOffCenterRH(M& mOut, N l, N r, N b, N t, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m41 = mOut.m42 = mOut.m44 = M::ValueType(0);
	mOut.m11 = M::ValueType(2) * zn / (r - M::ValueType(1));
	mOut.m22 = M::ValueType(2) * zn / (t - b);
	mOut.m31 = (M::ValueType(1) + r) / (M::ValueType(1) - r);
	mOut.m32 = (t + b) / (b - t);
	mOut.m33 = zf / (zn - zf);
	mOut.m43 = zn * zf / (zn - zf);
	mOut.m34 = -M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4PerspectiveOffCenterLH(M& mOut, N l, N r, N b, N t, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m41 = mOut.m42 = mOut.m44 = M::ValueType(0);
	mOut.m11 = M::ValueType(2) * zn / (r - M::ValueType(1));
	mOut.m22 = M::ValueType(2) * zn / (t - b);
	mOut.m31 = (M::ValueType(1) + r) / (M::ValueType(1) - r);
	mOut.m32 = (t + b) / (b - t);
	mOut.m33 = zf / (zf - zn);
	mOut.m43 = zn * zf / (zn - zf);
	mOut.m34 = M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4OrthoRH(M& mOut, N w, N h, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m34 = mOut.m41 = mOut.m42 = typename M::ValueType(0);
	mOut.m11 = typename M::ValueType(2) / w;
	mOut.m22 = typename M::ValueType(2) / h;
	mOut.m33 = typename M::ValueType(1) / (zn - zf);
	mOut.m43 = zn / (zn - zf);
	mOut.m44 = typename M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4OrthoLH(M& mOut, N w, N h, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m34 = mOut.m41 = mOut.m42 = M::ValueType(0);
	mOut.m11 = typename M::ValueType(2) / w;
	mOut.m22 = typename M::ValueType(2) / h;
	mOut.m33 = typename M::ValueType(1) / (zf - zn);
	mOut.m43 = zn / (zn - zf);
	mOut.m44 = typename M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4OrthoOffCenterRH(M& mOut, N l, N r, N b, N t, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m34 = M::ValueType(0);
	mOut.m11 = typename M::ValueType(2) / (r - l);
	mOut.m22 = typename M::ValueType(2) / (t - b);
	mOut.m33 = typename M::ValueType(1) / (zn - zf);
	mOut.m41 = (l + r) / (l - r);
	mOut.m42 = (t + b) / (b - t);
	mOut.m43 = zn / (zn - zf);
	mOut.m44 = typename M::ValueType(1);
}

template<typename M, typename N>
inline void Matrix4x4OrthoOffCenterLH(M& mOut, N l, N r, N b, N t, N zn, N zf)
{
	mOut.m12 = mOut.m13 = mOut.m14 = mOut.m21 = mOut.m23 = mOut.m24 = mOut.m31 = mOut.m32 = mOut.m34 = M::ValueType(0);
	mOut.m11 = typename M::ValueType(2) / (r - l);
	mOut.m22 = typename M::ValueType(2) / (t - b);
	mOut.m33 = typename M::ValueType(1) / (zf - zn);
	mOut.m41 = (l + r) / (l - r);
	mOut.m42 = (t + b) / (b - t);
	mOut.m43 = zn / (zn - zf);
	mOut.m44 = typename M::ValueType(1);
}

template<typename N>
inline N GetZnFromProjectionMatrix(const TMatrix4x4<N>& mat)
{
	return -mat.m43 * mat.m34 / mat.m33;
}

template<typename N>
inline N GetZfFromProjectionMatrix(const TMatrix4x4<N>& mat)
{
	return TMath<N>::abs_((mat.m44 - mat.m43) / (mat.m33 - mat.m34));
}

template<typename M, typename M1, typename M2>
inline void Matrix4x4Multiply(M& mOut, const M1& mat1, const M2& mat2)
{
	M tmpMat;
	M* mat;
	if((void*)&mOut == (void*)&mat1 || (void*)&mOut == (void*)&mat2)
	{
		mat = &tmpMat;
	}
	else
	{
		mat = &mOut;
	}
	mat->m[0][0] = mat1.m[0][0] * mat2.m[0][0] + mat1.m[0][1] * mat2.m[1][0] + mat1.m[0][2] * mat2.m[2][0] + mat1.m[0][3] * mat2.m[3][0];
	mat->m[0][1] = mat1.m[0][0] * mat2.m[0][1] + mat1.m[0][1] * mat2.m[1][1] + mat1.m[0][2] * mat2.m[2][1] + mat1.m[0][3] * mat2.m[3][1];
	mat->m[0][2] = mat1.m[0][0] * mat2.m[0][2] + mat1.m[0][1] * mat2.m[1][2] + mat1.m[0][2] * mat2.m[2][2] + mat1.m[0][3] * mat2.m[3][2];
	mat->m[0][3] = mat1.m[0][0] * mat2.m[0][3] + mat1.m[0][1] * mat2.m[1][3] + mat1.m[0][2] * mat2.m[2][3] + mat1.m[0][3] * mat2.m[3][3];

	mat->m[1][0] = mat1.m[1][0] * mat2.m[0][0] + mat1.m[1][1] * mat2.m[1][0] + mat1.m[1][2] * mat2.m[2][0] + mat1.m[1][3] * mat2.m[3][0];
	mat->m[1][1] = mat1.m[1][0] * mat2.m[0][1] + mat1.m[1][1] * mat2.m[1][1] + mat1.m[1][2] * mat2.m[2][1] + mat1.m[1][3] * mat2.m[3][1];
	mat->m[1][2] = mat1.m[1][0] * mat2.m[0][2] + mat1.m[1][1] * mat2.m[1][2] + mat1.m[1][2] * mat2.m[2][2] + mat1.m[1][3] * mat2.m[3][2];
	mat->m[1][3] = mat1.m[1][0] * mat2.m[0][3] + mat1.m[1][1] * mat2.m[1][3] + mat1.m[1][2] * mat2.m[2][3] + mat1.m[1][3] * mat2.m[3][3];

	mat->m[2][0] = mat1.m[2][0] * mat2.m[0][0] + mat1.m[2][1] * mat2.m[1][0] + mat1.m[2][2] * mat2.m[2][0] + mat1.m[2][3] * mat2.m[3][0];
	mat->m[2][1] = mat1.m[2][0] * mat2.m[0][1] + mat1.m[2][1] * mat2.m[1][1] + mat1.m[2][2] * mat2.m[2][1] + mat1.m[2][3] * mat2.m[3][1];
	mat->m[2][2] = mat1.m[2][0] * mat2.m[0][2] + mat1.m[2][1] * mat2.m[1][2] + mat1.m[2][2] * mat2.m[2][2] + mat1.m[2][3] * mat2.m[3][2];
	mat->m[2][3] = mat1.m[2][0] * mat2.m[0][3] + mat1.m[2][1] * mat2.m[1][3] + mat1.m[2][2] * mat2.m[2][3] + mat1.m[2][3] * mat2.m[3][3];

	mat->m[3][0] = mat1.m[3][0] * mat2.m[0][0] + mat1.m[3][1] * mat2.m[1][0] + mat1.m[3][2] * mat2.m[2][0] + mat1.m[3][3] * mat2.m[3][0];
	mat->m[3][1] = mat1.m[3][0] * mat2.m[0][1] + mat1.m[3][1] * mat2.m[1][1] + mat1.m[3][2] * mat2.m[2][1] + mat1.m[3][3] * mat2.m[3][1];
	mat->m[3][2] = mat1.m[3][0] * mat2.m[0][2] + mat1.m[3][1] * mat2.m[1][2] + mat1.m[3][2] * mat2.m[2][2] + mat1.m[3][3] * mat2.m[3][2];
	mat->m[3][3] = mat1.m[3][0] * mat2.m[0][3] + mat1.m[3][1] * mat2.m[1][3] + mat1.m[3][2] * mat2.m[2][3] + mat1.m[3][3] * mat2.m[3][3];
	
	if(mat != &mOut)
	{
		mOut = *mat;
	}
}



template<typename M, typename M1>
inline void Matrix4x4Inverse(M& mOut, const M1& mat)
{
	Gdouble m00 = mat.m[0][0], m01 = mat.m[0][1], m02 = mat.m[0][2], m03 = mat.m[0][3];
	Gdouble m10 = mat.m[1][0], m11 = mat.m[1][1], m12 = mat.m[1][2], m13 = mat.m[1][3];
	Gdouble m20 = mat.m[2][0], m21 = mat.m[2][1], m22 = mat.m[2][2], m23 = mat.m[2][3];
	Gdouble m30 = mat.m[3][0], m31 = mat.m[3][1], m32 = mat.m[3][2], m33 = mat.m[3][3];

	Gdouble v0 = m20 * m31 - m21 * m30;
	Gdouble v1 = m20 * m32 - m22 * m30;
	Gdouble v2 = m20 * m33 - m23 * m30;
	Gdouble v3 = m21 * m32 - m22 * m31;
	Gdouble v4 = m21 * m33 - m23 * m31;
	Gdouble v5 = m22 * m33 - m23 * m32;

	Gdouble t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
	Gdouble t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
	Gdouble t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
	Gdouble t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

	Gdouble invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

	Gdouble d00 = t00 * invDet;
	Gdouble d10 = t10 * invDet;
	Gdouble d20 = t20 * invDet;
	Gdouble d30 = t30 * invDet;

	Gdouble d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	Gdouble d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	Gdouble d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	Gdouble d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;

	Gdouble d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	Gdouble d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	Gdouble d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	Gdouble d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;

	Gdouble d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	Gdouble d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	Gdouble d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	Gdouble d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	mOut = M(
		d00, d01, d02, d03,
		d10, d11, d12, d13,
		d20, d21, d22, d23,
		d30, d31, d32, d33);
}

template<typename M, typename M1, typename M2>
inline void Matrix4x4MultiplyTranspose(M& mOut, const M1& mat1, const M2& mat2)
{
	M tmpMat;
	M* mat;
	if(&mOut == &mat1 || &mOut == &mat2)
	{
		mat = &tmpMat;
	}
	else
	{
		mat = &mOut;
	}

	mat->m[0][0] = mat1.m[0][0] * mat2.m[0][0] + mat1.m[0][1] * mat2.m[1][0] + mat1.m[0][2] * mat2.m[2][0] + mat1.m[0][3] * mat2.m[3][0];
	mat->m[1][0] = mat1.m[0][0] * mat2.m[0][1] + mat1.m[0][1] * mat2.m[1][1] + mat1.m[0][2] * mat2.m[2][1] + mat1.m[0][3] * mat2.m[3][1];
	mat->m[2][0] = mat1.m[0][0] * mat2.m[0][2] + mat1.m[0][1] * mat2.m[1][2] + mat1.m[0][2] * mat2.m[2][2] + mat1.m[0][3] * mat2.m[3][2];
	mat->m[3][0] = mat1.m[0][0] * mat2.m[0][3] + mat1.m[0][1] * mat2.m[1][3] + mat1.m[0][2] * mat2.m[2][3] + mat1.m[0][3] * mat2.m[3][3];

	mat->m[0][1] = mat1.m[1][0] * mat2.m[0][0] + mat1.m[1][1] * mat2.m[1][0] + mat1.m[1][2] * mat2.m[2][0] + mat1.m[1][3] * mat2.m[3][0];
	mat->m[1][1] = mat1.m[1][0] * mat2.m[0][1] + mat1.m[1][1] * mat2.m[1][1] + mat1.m[1][2] * mat2.m[2][1] + mat1.m[1][3] * mat2.m[3][1];
	mat->m[2][1] = mat1.m[1][0] * mat2.m[0][2] + mat1.m[1][1] * mat2.m[1][2] + mat1.m[1][2] * mat2.m[2][2] + mat1.m[1][3] * mat2.m[3][2];
	mat->m[3][1] = mat1.m[1][0] * mat2.m[0][3] + mat1.m[1][1] * mat2.m[1][3] + mat1.m[1][2] * mat2.m[2][3] + mat1.m[1][3] * mat2.m[3][3];

	mat->m[0][2] = mat1.m[2][0] * mat2.m[0][0] + mat1.m[2][1] * mat2.m[1][0] + mat1.m[2][2] * mat2.m[2][0] + mat1.m[2][3] * mat2.m[3][0];
	mat->m[1][2] = mat1.m[2][0] * mat2.m[0][1] + mat1.m[2][1] * mat2.m[1][1] + mat1.m[2][2] * mat2.m[2][1] + mat1.m[2][3] * mat2.m[3][1];
	mat->m[2][2] = mat1.m[2][0] * mat2.m[0][2] + mat1.m[2][1] * mat2.m[1][2] + mat1.m[2][2] * mat2.m[2][2] + mat1.m[2][3] * mat2.m[3][2];
	mat->m[3][2] = mat1.m[2][0] * mat2.m[0][3] + mat1.m[2][1] * mat2.m[1][3] + mat1.m[2][2] * mat2.m[2][3] + mat1.m[2][3] * mat2.m[3][3];

	mat->m[0][3] = mat1.m[3][0] * mat2.m[0][0] + mat1.m[3][1] * mat2.m[1][0] + mat1.m[3][2] * mat2.m[2][0] + mat1.m[3][3] * mat2.m[3][0];
	mat->m[1][3] = mat1.m[3][0] * mat2.m[0][1] + mat1.m[3][1] * mat2.m[1][1] + mat1.m[3][2] * mat2.m[2][1] + mat1.m[3][3] * mat2.m[3][1];
	mat->m[2][3] = mat1.m[3][0] * mat2.m[0][2] + mat1.m[3][1] * mat2.m[1][2] + mat1.m[3][2] * mat2.m[2][2] + mat1.m[3][3] * mat2.m[3][2];
	mat->m[3][3] = mat1.m[3][0] * mat2.m[0][3] + mat1.m[3][1] * mat2.m[1][3] + mat1.m[3][2] * mat2.m[2][3] + mat1.m[3][3] * mat2.m[3][3];

	if(mat != &mOut)
	{
		mOut = *mat;
	}
}

template<typename M, typename V, typename N>
inline void Matrix4x4RotationAxis(M& mOut, const V& axis, N angle)
{
	// Normalize axis
	V normal_axis;
	Vector3Normalize(normal_axis, axis);
	typename V::ValueType x = normal_axis.x;
	typename V::ValueType y = normal_axis.y;
	typename V::ValueType z = normal_axis.z;

	// angle
	Gdouble c = TMath<N>::cos_(angle);
	Gdouble s = TMath<N>::sin_(angle);
	Gdouble p = 1 - c;

	mOut.m11 = x * x * p + c;
	mOut.m12 = x * y * p + z * s;
	mOut.m13 = x * z * p - y * s;

	mOut.m21 = x * y * p - z * s;
	mOut.m22 = y * y * p + c;
	mOut.m23 = y * z * p + x * s;

	mOut.m31 = x * z * p + y * s;
	mOut.m32 = y * z * p - x * s;
	mOut.m33 = z * z * p + c;

	mOut.m14 = mOut.m24 = mOut.m34 = mOut.m41 = mOut.m42 = mOut.m43 = 0.0f;
	mOut.m44 = 1.0f;
}

void Matrix4x4MultiplySSE2(Matrix4x4f* pOut, const Matrix4x4f* pIn1, const Matrix4x4f* pIn2);

#endif//ANGEOUTILITY_MATRIX4X4_H_
