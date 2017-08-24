#include "utility_typedef.h"
#include "utility_matrix4x4.h"

void Matrix4x4MultiplySSE2(Matrix4x4f* pOut, const Matrix4x4f* pIn1, const Matrix4x4f* pIn2)
{

	Matrix4x4Multiply(*pOut, *pIn1, *pIn2);
// 	_asm
// 	{
// 		mov edx, pIn2;			// 这时保存的是pIn2
// 		movups xmm4, [edx];		//pIn2的第1行
// 		movups xmm5, [edx+16];	//pIn2的第2行
// 		movups xmm6, [edx+32];	//pIn2的第3行
// 		movups xmm7, [edx+48];	//pIn2的第4行
// 
// 		mov eax, pIn1;			// 这时保存的是pIn1
// 		mov edx, pOut;
// 
// 		mov ecx, 4;				// 循环4次
// 
// LOOPIT:						        // 开始循环
// 		movss xmm0, [eax];		//xmm0 = pIn1->x
// 		shufps xmm0, xmm0, 0;	//洗牌xmm0 = pIn1->x, pIn1->x, pIn1->x, pIn1->x
// 		mulps xmm0, xmm4;
// 
// 		movss xmm1, [eax+4];	//xmm1 = pIn1->y
// 		shufps xmm1, xmm1, 0;	//洗牌xmm1 = pIn1->y, pIn1->y, pIn1->y, pIn1->y
// 		mulps xmm1, xmm5;
// 
// 		movss xmm2, [eax+8];	//xmm2 = pIn1->z
// 		shufps xmm2, xmm2, 0;	//洗牌xmm2 = pIn1->z, pIn1->z, pIn1->z, pIn1->z
// 		mulps xmm2, xmm6;
// 
// 		movss xmm3, [eax+12];	//xmm3 = pIn1->w
// 		shufps xmm3, xmm3, 0;	//洗牌xmm3 = pIn1->w, pIn1->w, pIn1->w, pIn1->w
// 		mulps xmm3, xmm7;
// 
// 		addps xmm0, xmm1;
// 		addps xmm2, xmm3;
// 		addps xmm0, xmm2;		//最终结果行保存在xmm0
// 
// 		movups [edx], xmm0;		//将结果保存到pOut中
// 		add edx, 16;
// 		add eax, 16;			////作为变址用
// 
// 		loop LOOPIT;
// 	}
}

template<> const TMatrix4x4<Gfloat> TMatrix4x4<Gfloat>::s_zero(\
	0.0f,0.0f,0.0f,0.0f,
	0.0f,0.0f,0.0f,0.0f,
	0.0f,0.0f,0.0f,0.0f,
	0.0f,0.0f,0.0f,0.0f);

template<> const TMatrix4x4<Gfloat> TMatrix4x4<Gfloat>::s_identity(\
	1.0f,0.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,0.0f,
	0.0f,0.0f,1.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f);


template<> const TMatrix4x4<Gdouble> TMatrix4x4<Gdouble>::s_zero(\
	0.0,0.0,0.0,0.0,
	0.0,0.0,0.0,0.0,
	0.0,0.0,0.0,0.0,
	0.0,0.0,0.0,0.0);

template<> const TMatrix4x4<Gdouble> TMatrix4x4<Gdouble>::s_identity(\
	1.0,0.0,0.0,0.0,
	0.0,1.0,0.0,0.0,
	0.0,0.0,1.0,0.0,
	0.0,0.0,0.0,1.0);