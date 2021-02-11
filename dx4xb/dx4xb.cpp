// dx4xb.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "dx4xb.h"
#include "dx4xb_private.h"

namespace dx4xb {

#pragma region Error Management

	Exception dx4xb::Exception::FromError(Errors error, const char* arg, HRESULT hr)
	{
		static char fullMessage[1000];

		ZeroMemory(&fullMessage, sizeof(fullMessage));

		const char* errorMessage;

		switch (error) {
		case Errors::BadPSOConstruction:
			errorMessage = "Error in PSO construction. Check all registers are bound and the input layout is compatible.";
			break;
		case Errors::UnsupportedFormat:
			errorMessage = "Error in Format. Check resource format is supported.";
			break;
		case Errors::ShaderNotFound:
			errorMessage = "Shader file was not found. Check file path in pipeline binding.";
			break;
		case Errors::RunOutOfMemory:
			errorMessage = "Run out of memory during object creation.";
			break;
		case Errors::Invalid_Operation:
			errorMessage = "Invalid operation.";
			break;
		case Errors::Unsupported_Fallback:
			errorMessage = "Fallback raytracing device was not supported. Check OS is in Developer Mode.";
			break;
		default:
			errorMessage = "Unknown error in CA4G";
			break;
		}

		if (errorMessage != nullptr)
			strcat_s(fullMessage, errorMessage);

		if (arg != nullptr)
			strcat_s(fullMessage, arg);

		if (FAILED(hr)) // Some HRESULT to show
		{
			_com_error err(hr);

			char hrErrorMessage[1000];
			ZeroMemory(hrErrorMessage, sizeof(hrErrorMessage));
			CharToOem(err.ErrorMessage(), hrErrorMessage);

			strcat_s(fullMessage, hrErrorMessage);
		}

		std::cout << fullMessage;

		return Exception(fullMessage);
	}

#pragma endregion

#pragma region Math

	int1::operator float1() const { return float1((float)this->x); }
	int1::operator uint1() const { return uint1((uint)this->x); }

	int2::operator float2() const { return float2((float)this->x, (float)this->y); }
	int2::operator uint2() const { return uint2((uint)this->x, (uint)this->y); }

	int3::operator float3() const { return float3((float)this->x, (float)this->y, (float)this->z); }
	int3::operator uint3() const { return uint3((uint)this->x, (uint)this->y, (uint)this->z); }

	int4::operator float4() const { return float4((float)this->x, (float)this->y, (float)this->z, (float)this->w); }
	int4::operator uint4() const { return uint4((uint)this->x, (uint)this->y, (uint)this->z, (uint)this->w); }

	int1x1::operator float1x1() const { return float1x1((float)this->_m00); }
	int1x1::operator uint1x1() const { return uint1x1((uint)this->_m00); }

	int1x2::operator float1x2() const { return float1x2((float)this->_m00, (float)this->_m01); }
	int1x2::operator uint1x2() const { return uint1x2((uint)this->_m00, (uint)this->_m01); }

	int1x3::operator float1x3() const { return float1x3((float)this->_m00, (float)this->_m01, (float)this->_m02); }
	int1x3::operator uint1x3() const { return uint1x3((uint)this->_m00, (uint)this->_m01, (uint)this->_m02); }

	int1x4::operator float1x4() const { return float1x4((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m03); }
	int1x4::operator uint1x4() const { return uint1x4((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m03); }

	int2x1::operator float2x1() const { return float2x1((float)this->_m00, (float)this->_m10); }
	int2x1::operator uint2x1() const { return uint2x1((uint)this->_m00, (uint)this->_m10); }

	int2x2::operator float2x2() const { return float2x2((float)this->_m00, (float)this->_m01, (float)this->_m10, (float)this->_m11); }
	int2x2::operator uint2x2() const { return uint2x2((uint)this->_m00, (uint)this->_m01, (uint)this->_m10, (uint)this->_m11); }

	int2x3::operator float2x3() const { return float2x3((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m10, (float)this->_m11, (float)this->_m12); }
	int2x3::operator uint2x3() const { return uint2x3((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12); }

	int2x4::operator float2x4() const { return float2x4((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m03, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m13); }
	int2x4::operator uint2x4() const { return uint2x4((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m03, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m13); }

	int3x1::operator float3x1() const { return float3x1((float)this->_m00, (float)this->_m10, (float)this->_m20); }
	int3x1::operator uint3x1() const { return uint3x1((uint)this->_m00, (uint)this->_m10, (uint)this->_m20); }

	int3x2::operator float3x2() const { return float3x2((float)this->_m00, (float)this->_m01, (float)this->_m10, (float)this->_m11, (float)this->_m20, (float)this->_m21); }
	int3x2::operator uint3x2() const { return uint3x2((uint)this->_m00, (uint)this->_m01, (uint)this->_m10, (uint)this->_m11, (uint)this->_m20, (uint)this->_m21); }

	int3x3::operator float3x3() const { return float3x3((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m20, (float)this->_m21, (float)this->_m22); }
	int3x3::operator uint3x3() const { return uint3x3((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m20, (uint)this->_m21, (uint)this->_m22); }

	int3x4::operator float3x4() const { return float3x4((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m03, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m13, (float)this->_m20, (float)this->_m21, (float)this->_m22, (float)this->_m23); }
	int3x4::operator uint3x4() const { return uint3x4((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m03, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m13, (uint)this->_m20, (uint)this->_m21, (uint)this->_m22, (uint)this->_m23); }

	int4x1::operator float4x1() const { return float4x1((float)this->_m00, (float)this->_m10, (float)this->_m20, (float)this->_m30); }
	int4x1::operator uint4x1() const { return uint4x1((uint)this->_m00, (uint)this->_m10, (uint)this->_m20, (uint)this->_m30); }

	int4x2::operator float4x2() const { return float4x2((float)this->_m00, (float)this->_m01, (float)this->_m10, (float)this->_m11, (float)this->_m20, (float)this->_m21, (float)this->_m30, (float)this->_m31); }
	int4x2::operator uint4x2() const { return uint4x2((uint)this->_m00, (uint)this->_m01, (uint)this->_m10, (uint)this->_m11, (uint)this->_m20, (uint)this->_m21, (uint)this->_m30, (uint)this->_m31); }

	int4x3::operator float4x3() const { return float4x3((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m20, (float)this->_m21, (float)this->_m22, (float)this->_m30, (float)this->_m31, (float)this->_m32); }
	int4x3::operator uint4x3() const { return uint4x3((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m20, (uint)this->_m21, (uint)this->_m22, (uint)this->_m30, (uint)this->_m31, (uint)this->_m32); }

	int4x4::operator float4x4() const { return float4x4((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m03, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m13, (float)this->_m20, (float)this->_m21, (float)this->_m22, (float)this->_m23, (float)this->_m30, (float)this->_m31, (float)this->_m32, (float)this->_m33); }
	int4x4::operator uint4x4() const { return uint4x4((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m03, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m13, (uint)this->_m20, (uint)this->_m21, (uint)this->_m22, (uint)this->_m23, (uint)this->_m30, (uint)this->_m31, (uint)this->_m32, (uint)this->_m33); }

	float1::operator int1() const { return int1((int)this->x); }
	float1::operator uint1() const { return uint1((uint)this->x); }

	float2::operator int2() const { return int2((int)this->x, (int)this->y); }
	float2::operator uint2() const { return uint2((uint)this->x, (uint)this->y); }

	float3::operator int3() const { return int3((int)this->x, (int)this->y, (int)this->z); }
	float3::operator uint3() const { return uint3((uint)this->x, (uint)this->y, (uint)this->z); }

	float4::operator int4() const { return int4((int)this->x, (int)this->y, (int)this->z, (int)this->w); }
	float4::operator uint4() const { return uint4((uint)this->x, (uint)this->y, (uint)this->z, (uint)this->w); }

	float1x1::operator int1x1() const { return int1x1((int)this->_m00); }
	float1x1::operator uint1x1() const { return uint1x1((uint)this->_m00); }

	float1x2::operator int1x2() const { return int1x2((int)this->_m00, (int)this->_m01); }
	float1x2::operator uint1x2() const { return uint1x2((uint)this->_m00, (uint)this->_m01); }

	float1x3::operator int1x3() const { return int1x3((int)this->_m00, (int)this->_m01, (int)this->_m02); }
	float1x3::operator uint1x3() const { return uint1x3((uint)this->_m00, (uint)this->_m01, (uint)this->_m02); }

	float1x4::operator int1x4() const { return int1x4((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m03); }
	float1x4::operator uint1x4() const { return uint1x4((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m03); }

	float2x1::operator int2x1() const { return int2x1((int)this->_m00, (int)this->_m10); }
	float2x1::operator uint2x1() const { return uint2x1((uint)this->_m00, (uint)this->_m10); }

	float2x2::operator int2x2() const { return int2x2((int)this->_m00, (int)this->_m01, (int)this->_m10, (int)this->_m11); }
	float2x2::operator uint2x2() const { return uint2x2((uint)this->_m00, (uint)this->_m01, (uint)this->_m10, (uint)this->_m11); }

	float2x3::operator int2x3() const { return int2x3((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m10, (int)this->_m11, (int)this->_m12); }
	float2x3::operator uint2x3() const { return uint2x3((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12); }

	float2x4::operator int2x4() const { return int2x4((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m03, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m13); }
	float2x4::operator uint2x4() const { return uint2x4((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m03, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m13); }

	float3x1::operator int3x1() const { return int3x1((int)this->_m00, (int)this->_m10, (int)this->_m20); }
	float3x1::operator uint3x1() const { return uint3x1((uint)this->_m00, (uint)this->_m10, (uint)this->_m20); }

	float3x2::operator int3x2() const { return int3x2((int)this->_m00, (int)this->_m01, (int)this->_m10, (int)this->_m11, (int)this->_m20, (int)this->_m21); }
	float3x2::operator uint3x2() const { return uint3x2((uint)this->_m00, (uint)this->_m01, (uint)this->_m10, (uint)this->_m11, (uint)this->_m20, (uint)this->_m21); }

	float3x3::operator int3x3() const { return int3x3((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m20, (int)this->_m21, (int)this->_m22); }
	float3x3::operator uint3x3() const { return uint3x3((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m20, (uint)this->_m21, (uint)this->_m22); }

	float3x4::operator int3x4() const { return int3x4((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m03, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m13, (int)this->_m20, (int)this->_m21, (int)this->_m22, (int)this->_m23); }
	float3x4::operator uint3x4() const { return uint3x4((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m03, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m13, (uint)this->_m20, (uint)this->_m21, (uint)this->_m22, (uint)this->_m23); }

	float4x1::operator int4x1() const { return int4x1((int)this->_m00, (int)this->_m10, (int)this->_m20, (int)this->_m30); }
	float4x1::operator uint4x1() const { return uint4x1((uint)this->_m00, (uint)this->_m10, (uint)this->_m20, (uint)this->_m30); }

	float4x2::operator int4x2() const { return int4x2((int)this->_m00, (int)this->_m01, (int)this->_m10, (int)this->_m11, (int)this->_m20, (int)this->_m21, (int)this->_m30, (int)this->_m31); }
	float4x2::operator uint4x2() const { return uint4x2((uint)this->_m00, (uint)this->_m01, (uint)this->_m10, (uint)this->_m11, (uint)this->_m20, (uint)this->_m21, (uint)this->_m30, (uint)this->_m31); }

	float4x3::operator int4x3() const { return int4x3((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m20, (int)this->_m21, (int)this->_m22, (int)this->_m30, (int)this->_m31, (int)this->_m32); }
	float4x3::operator uint4x3() const { return uint4x3((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m20, (uint)this->_m21, (uint)this->_m22, (uint)this->_m30, (uint)this->_m31, (uint)this->_m32); }

	float4x4::operator int4x4() const { return int4x4((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m03, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m13, (int)this->_m20, (int)this->_m21, (int)this->_m22, (int)this->_m23, (int)this->_m30, (int)this->_m31, (int)this->_m32, (int)this->_m33); }
	float4x4::operator uint4x4() const { return uint4x4((uint)this->_m00, (uint)this->_m01, (uint)this->_m02, (uint)this->_m03, (uint)this->_m10, (uint)this->_m11, (uint)this->_m12, (uint)this->_m13, (uint)this->_m20, (uint)this->_m21, (uint)this->_m22, (uint)this->_m23, (uint)this->_m30, (uint)this->_m31, (uint)this->_m32, (uint)this->_m33); }

	uint1::operator float1() const { return float1((float)this->x); }
	uint1::operator int1() const { return int1((int)this->x); }

	uint2::operator float2() const { return float2((float)this->x, (float)this->y); }
	uint2::operator int2() const { return int2((int)this->x, (int)this->y); }

	uint3::operator float3() const { return float3((float)this->x, (float)this->y, (float)this->z); }
	uint3::operator int3() const { return int3((int)this->x, (int)this->y, (int)this->z); }

	uint4::operator float4() const { return float4((float)this->x, (float)this->y, (float)this->z, (float)this->w); }
	uint4::operator int4() const { return int4((int)this->x, (int)this->y, (int)this->z, (int)this->w); }

	uint1x1::operator float1x1() const { return float1x1((float)this->_m00); }
	uint1x1::operator int1x1() const { return int1x1((int)this->_m00); }

	uint1x2::operator float1x2() const { return float1x2((float)this->_m00, (float)this->_m01); }
	uint1x2::operator int1x2() const { return int1x2((int)this->_m00, (int)this->_m01); }

	uint1x3::operator float1x3() const { return float1x3((float)this->_m00, (float)this->_m01, (float)this->_m02); }
	uint1x3::operator int1x3() const { return int1x3((int)this->_m00, (int)this->_m01, (int)this->_m02); }

	uint1x4::operator float1x4() const { return float1x4((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m03); }
	uint1x4::operator int1x4() const { return int1x4((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m03); }

	uint2x1::operator float2x1() const { return float2x1((float)this->_m00, (float)this->_m10); }
	uint2x1::operator int2x1() const { return int2x1((int)this->_m00, (int)this->_m10); }

	uint2x2::operator float2x2() const { return float2x2((float)this->_m00, (float)this->_m01, (float)this->_m10, (float)this->_m11); }
	uint2x2::operator int2x2() const { return int2x2((int)this->_m00, (int)this->_m01, (int)this->_m10, (int)this->_m11); }

	uint2x3::operator float2x3() const { return float2x3((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m10, (float)this->_m11, (float)this->_m12); }
	uint2x3::operator int2x3() const { return int2x3((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m10, (int)this->_m11, (int)this->_m12); }

	uint2x4::operator float2x4() const { return float2x4((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m03, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m13); }
	uint2x4::operator int2x4() const { return int2x4((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m03, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m13); }

	uint3x1::operator float3x1() const { return float3x1((float)this->_m00, (float)this->_m10, (float)this->_m20); }
	uint3x1::operator int3x1() const { return int3x1((int)this->_m00, (int)this->_m10, (int)this->_m20); }

	uint3x2::operator float3x2() const { return float3x2((float)this->_m00, (float)this->_m01, (float)this->_m10, (float)this->_m11, (float)this->_m20, (float)this->_m21); }
	uint3x2::operator int3x2() const { return int3x2((int)this->_m00, (int)this->_m01, (int)this->_m10, (int)this->_m11, (int)this->_m20, (int)this->_m21); }

	uint3x3::operator float3x3() const { return float3x3((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m20, (float)this->_m21, (float)this->_m22); }
	uint3x3::operator int3x3() const { return int3x3((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m20, (int)this->_m21, (int)this->_m22); }

	uint3x4::operator float3x4() const { return float3x4((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m03, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m13, (float)this->_m20, (float)this->_m21, (float)this->_m22, (float)this->_m23); }
	uint3x4::operator int3x4() const { return int3x4((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m03, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m13, (int)this->_m20, (int)this->_m21, (int)this->_m22, (int)this->_m23); }

	uint4x1::operator float4x1() const { return float4x1((float)this->_m00, (float)this->_m10, (float)this->_m20, (float)this->_m30); }
	uint4x1::operator int4x1() const { return int4x1((int)this->_m00, (int)this->_m10, (int)this->_m20, (int)this->_m30); }

	uint4x2::operator float4x2() const { return float4x2((float)this->_m00, (float)this->_m01, (float)this->_m10, (float)this->_m11, (float)this->_m20, (float)this->_m21, (float)this->_m30, (float)this->_m31); }
	uint4x2::operator int4x2() const { return int4x2((int)this->_m00, (int)this->_m01, (int)this->_m10, (int)this->_m11, (int)this->_m20, (int)this->_m21, (int)this->_m30, (int)this->_m31); }

	uint4x3::operator float4x3() const { return float4x3((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m20, (float)this->_m21, (float)this->_m22, (float)this->_m30, (float)this->_m31, (float)this->_m32); }
	uint4x3::operator int4x3() const { return int4x3((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m20, (int)this->_m21, (int)this->_m22, (int)this->_m30, (int)this->_m31, (int)this->_m32); }

	uint4x4::operator float4x4() const { return float4x4((float)this->_m00, (float)this->_m01, (float)this->_m02, (float)this->_m03, (float)this->_m10, (float)this->_m11, (float)this->_m12, (float)this->_m13, (float)this->_m20, (float)this->_m21, (float)this->_m22, (float)this->_m23, (float)this->_m30, (float)this->_m31, (float)this->_m32, (float)this->_m33); }
	uint4x4::operator int4x4() const { return int4x4((int)this->_m00, (int)this->_m01, (int)this->_m02, (int)this->_m03, (int)this->_m10, (int)this->_m11, (int)this->_m12, (int)this->_m13, (int)this->_m20, (int)this->_m21, (int)this->_m22, (int)this->_m23, (int)this->_m30, (int)this->_m31, (int)this->_m32, (int)this->_m33); }

#pragma endregion

#pragma region Scheduler

	wScheduler::wScheduler(wDevice* w_device, int frames, int threads)
	{
		this->w_device = w_device;

		this->perFrameFinishedSignal = new Signal[frames];

		DX_Device device = w_device->device;

		this->processQueue = new ProducerConsumerQueue<TagProcess>(threads);

		this->ActiveCmdLists = new DX_CommandList[threads * 3];

		this->threads = new HANDLE[threads];
		this->workers = new GPUWorkerInfo[threads];
		this->threadsCount = threads;
		this->counting = new CountEvent();

		for (int i = 0; i < threads; i++) {
			workers[i] = { i, this };

			DWORD threadId;
			if (i > 0) // only create threads for workerIndex > 0. Worker 0 will execute on main thread
				this->threads[i] = CreateThread(nullptr, 0, __WORKER_TODO, &workers[i], 0, &threadId);
		}

		for (int i = 0; i < 3; i++)
		{
			PerEngineInfo& info = Engines[i];

			D3D12_COMMAND_LIST_TYPE type;
			switch (i) {
			case 0: // Graphics
				type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
			case 1: // Compute
				type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
				break;
			case 2:
				type = D3D12_COMMAND_LIST_TYPE_COPY;
				break;
			}

			// Create queue (REUSE queue of graphics for raytracing)
			info.queue = new wCommandQueue(device, type);

			info.threadInfos = new PerThreadInfo[threads];
			info.threadCount = threads;
			info.frames = new PerFrameInfo[frames];
			info.framesCount = frames;

			// Create allocators first because they are needed for command lists
			for (int j = 0; j < frames; j++)
			{
				info.frames[j].allocatorSet = new DX_CommandAllocator[threads];
				info.frames[j].allocatorsUsed = 0;

				for (int k = 0; k < threads; k++)
					device->CreateCommandAllocator(type, IID_PPV_ARGS(&info.frames[j].allocatorSet[k]));
			}

			// Create command lists
			for (int j = 0; j < threads; j++)
			{
				device->CreateCommandList(0, type, info.frames[0].allocatorSet[0], nullptr, IID_PPV_ARGS(&info.threadInfos[j].cmdList));
				info.threadInfos[j].cmdList->Close(); // start cmdList state closed.
				info.threadInfos[j].isActive = false;

				switch ((Engine)i) {
				case Engine::Direct:
					info.threadInfos[j].manager = new RaytracingManager(); // can be used as GraphicsManager when needed
					break;
				case Engine::Compute:
					info.threadInfos[j].manager = new ComputeManager();
					break;
				case Engine::Copy:
					info.threadInfos[j].manager = new CopyManager();
					break;
				}

				wCmdList* cmdWrapper = new wCmdList();
				cmdWrapper->w_device = this->w_device;
				cmdWrapper->cmdList = info.threadInfos[j].cmdList;
				info.threadInfos[j].manager->w_cmdList = cmdWrapper;
			}
		}
	}

	void dx4xb::wScheduler::PopulateCmdListWithProcess(TagProcess tagProcess, int threadIndex)
	{
		gObj<GPUProcess> nextProcess = tagProcess.process;

		int engineIndex = (int)nextProcess->RequiredEngine();

		auto cmdListManager = this->Engines[engineIndex].threadInfos[threadIndex].manager;

		this->Engines[engineIndex].threadInfos[threadIndex].
			Activate(this->Engines[engineIndex].frames[this->CurrentFrameIndex].RequireAllocator(threadIndex));

		cmdListManager->__Tag = tagProcess.Tag;
		nextProcess->OnCollect(cmdListManager);

		if (threadIndex > 0)
			this->counting->Signal();
	}

	void dx4xb::wScheduler::SetupFrame(int frame)
	{
		if (w_device->desc.UseBuffering)
			WaitFor(perFrameFinishedSignal[frame]); // Grants the GPU finished working this frame in a previous stage

		CurrentFrameIndex = frame;

		for (int e = 0; e < 3; e++)
			Engines[e].frames[frame].ResetUsedAllocators();

		w_device->gpu_csu->RestartAllocatorForFrame(frame);
		w_device->gpu_smp->RestartAllocatorForFrame(frame);

		PrepareRenderTarget(D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	void dx4xb::wScheduler::PrepareRenderTarget(D3D12_RESOURCE_STATES state)
	{
		if (!Engines[0].threadInfos[0].isActive)
		{ // activate main cmd.
			Engines[0].threadInfos[0].Activate(
				Engines[0].frames[CurrentFrameIndex].allocatorSet[0]
			);
		}
		//// Get current RT
		//auto currentRT = this->w_device->RenderTargets[this->CurrentFrameIndex];
		//// Place a barrier at thread 0 cmdList to Present
		//DX_ResourceWrapper* rtWrapper = (DX_ResourceWrapper*)currentRT->__InternalDXWrapper;
		//rtWrapper->AddBarrier(this->Engines[0].threadInfos[0].cmdList, state);
	}

	void dx4xb::wScheduler::FinishFrame()
	{
		if (this->AsyncWorkPending)
			// Ensure all async work was done before.
			FlushAndSignal(EngineMask::All).WaitFor();

		PrepareRenderTarget(D3D12_RESOURCE_STATE_PRESENT);

		perFrameFinishedSignal[CurrentFrameIndex] = FlushAndSignal(EngineMask::All);
		if (!w_device->desc.UseBuffering)
			// Grants the GPU finished working this frame before finishing this frame
			WaitFor(perFrameFinishedSignal[CurrentFrameIndex]);
	}

	DWORD __stdcall dx4xb::wScheduler::__WORKER_TODO(LPVOID param)
	{
		GPUWorkerInfo* wi = (GPUWorkerInfo*)param;
		int index = wi->Index;
		wScheduler* scheduler = wi->Scheduler;

		while (!scheduler->IsClosed) {
			TagProcess tagProcess;
			if (!scheduler->processQueue->TryConsume(tagProcess))
				break;

			scheduler->PopulateCmdListWithProcess(tagProcess, index);
		}
		return 0;
	}

	void dx4xb::wScheduler::Enqueue(gObj<GPUProcess> process)
	{
		this->PopulateCmdListWithProcess(TagProcess{ process, this->Tag }, 0);
	}

	void dx4xb::wScheduler::EnqueueAsync(gObj<GPUProcess> process)
	{
		this->AsyncWorkPending |= process->RequiredEngine() == Engine::Direct;
		counting->Increment();
		processQueue->TryProduce(TagProcess{ process, this->Tag });
	}

	void dx4xb::wScheduler::WaitFor(const Signal& signal)
	{
		int fencesForWaiting = 0;
		HANDLE FencesForWaiting[3];
		for (int e = 0; e < 3; e++)
			if (signal.rallyPoints[e] != 0)
				FencesForWaiting[fencesForWaiting++] = Engines[e].queue->TriggerEvent(signal.rallyPoints[e]);
		WaitForMultipleObjects(fencesForWaiting, FencesForWaiting, true, INFINITE);
		if (signal.rallyPoints[0] != 0)
			this->AsyncWorkPending = false;
	}

	Signal dx4xb::wScheduler::FlushAndSignal(EngineMask mask)
	{
		int engines = (int)mask;
		long rally[3];
		// Barrier to wait for all pending workers to populate command lists
		// After this, next CPU processes can assume previous CPU collecting has finished
		counting->Wait();

#pragma region Flush Pending Workers

		for (int e = 0; e < 3; e++)
			if (engines & (1 << e))
			{
				int activeCmdLists = 0;
				for (int t = 0; t < threadsCount; t++) {
					if (this->Engines[e].threadInfos[t].isActive) // pending work here
					{
						this->Engines[e].threadInfos[t].Close();
						this->ActiveCmdLists[activeCmdLists++] = Engines[e].threadInfos[t].cmdList;
					}
					auto manager = Engines[e].threadInfos[t].manager;

					// Copy all collected descriptors from non-visible to visible DHs.
					if (manager->w_cmdList->srcDescriptors.size() > 0)
					{
						w_device->device->CopyDescriptors(
							manager->w_cmdList->dstDescriptors.size(), &manager->w_cmdList->dstDescriptors.first(), &manager->w_cmdList->dstDescriptorRangeLengths.first(),
							manager->w_cmdList->srcDescriptors.size(), &manager->w_cmdList->srcDescriptors.first(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
						);
						// Clears the lists for next usage
						manager->w_cmdList->srcDescriptors.reset();
						manager->w_cmdList->dstDescriptors.reset();
						manager->w_cmdList->dstDescriptorRangeLengths.reset();
					}
				}

				if (activeCmdLists > 0) // some cmdlist to execute
				{
					Engines[e].queue->Commit(ActiveCmdLists, activeCmdLists);

					rally[e] = Engines[e].queue->Signal();
				}
				else
					rally[e] = 0;
			}

		return Signal(this, rally);
	}

	void dx4xb::Signal::WaitFor()
	{
		((wScheduler*)this->scheduler)->WaitFor(*this);
	}

#pragma endregion

#pragma region Pipelines

	D3D12_SHADER_BYTECODE ShaderLoader::FromFile(const char* bytecodeFilePath) {
		D3D12_SHADER_BYTECODE code;
		FILE* file;
		if (fopen_s(&file, bytecodeFilePath, "rb") != 0)
		{
			throw Exception::FromError(Errors::ShaderNotFound);
		}
		fseek(file, 0, SEEK_END);
		long long count;
		count = ftell(file);
		fseek(file, 0, SEEK_SET);

		byte* bytecode = new byte[count];
		int offset = 0;
		while (offset < count) {
			offset += fread_s(&bytecode[offset], min(1024, count - offset), sizeof(byte), 1024, file);
		}
		fclose(file);

		code.BytecodeLength = count;
		code.pShaderBytecode = (void*)bytecode;
		return code;
	}

	D3D12_SHADER_BYTECODE ShaderLoader::FromMemory(const byte* bytecodeData, int count) {
		D3D12_SHADER_BYTECODE code;
		code.BytecodeLength = count;
		code.pShaderBytecode = (void*)bytecodeData;
		return code;
	}


	// Represents a binding of a resource (or resource array) to a shader slot.
	struct SlotBinding {
		// Gets or sets the root parameter bound
		D3D12_ROOT_PARAMETER Root_Parameter;

		union {
			struct ConstantData {
				// Gets the pointer to a constant buffer in memory.
				void* ptrToConstant;
			} ConstantData;
			struct DescriptorData
			{
				// Determines the dimension of the bound resource
				D3D12_RESOURCE_DIMENSION Dimension;
				// Gets the pointer to a resource field (pointer to ResourceView)
				// or to the array of resources
				void* ptrToResourceViewArray;
				// Gets the pointer to the number of resources in array
				int* ptrToCount;
			} DescriptorData;
			struct SceneData {
				void* ptrToScene;
			} SceneData;
		};
	};

	struct wBindings
	{
		// Used to collect all constant, shader, unordered views or sampler bindings
		list<SlotBinding> bindings;

		// Used to collect all render targets bindings
		gObj<Texture2D>* RenderTargets[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
		// Used to collect all render targets bindings
		D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptors[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
		// Gets or sets the maxim slot of bound render target
		int RenderTargetMax = 0;
		// Gets the bound depth buffer resource
		gObj<Texture2D>* DepthBufferField = nullptr;

		// Used to collect static samplers
		D3D12_STATIC_SAMPLER_DESC Static_Samplers[32];
		// Gets or sets the maxim sampler slot used
		int StaticSamplerMax = 0;

		Engine EngineType;

		// Table with all descriptor ranges bound by this pipeline bindings object.
		// This array must be on the CPU during bindings.
		table<D3D12_DESCRIPTOR_RANGE> ranges = table<D3D12_DESCRIPTOR_RANGE>(200);

		int globalBindings = 0;

		void AddBinding(bool collectingGlobal, const SlotBinding& binding) {
			if (collectingGlobal) {
				bindings.push(binding);
				globalBindings++;
			}
			else
			{
				bindings.add(binding);
			}
		}

		void BindToGPU(bool global, wDevice* dxwrapper, wCmdList* cmdWrapper) {
			auto device = dxwrapper->device;
			auto cmdList = cmdWrapper->cmdList;

			int startRootParameter = global ? 0 : globalBindings;
			int endRootParameter = global ? globalBindings : bindings.size();

			if (global /*Only for global bindings*/)
			{
#pragma region Bind Render Targets and DepthBuffer if any
				if (EngineType == Engine::Direct)
				{
					for (int i = 0; i < RenderTargetMax; i++)
						if (!(*RenderTargets[i]))
							RenderTargetDescriptors[i] =  dxwrapper->ResolveNullRTVDescriptor();
						else
						{
							wResource* iresource = (*RenderTargets[i])->w_resource;
							wResourceView* vresource = (*RenderTargets[i])->w_view;
							iresource->AddBarrier(cmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);

							RenderTargetDescriptors[i] = vresource->getRTVHandle();
						}

					D3D12_CPU_DESCRIPTOR_HANDLE depthHandle;
					if (!(DepthBufferField == nullptr || !(*DepthBufferField)))
					{
						wResourceView* vresource = (*DepthBufferField)->w_view;
						depthHandle = vresource->getDSVHandle();
					}

					cmdList->OMSetRenderTargets(RenderTargetMax, RenderTargetDescriptors, FALSE, (DepthBufferField == nullptr || !(*DepthBufferField)) ? nullptr : &depthHandle);
				}
				ID3D12DescriptorHeap* heaps[] = { dxwrapper->gpu_csu->getInnerHeap(), dxwrapper->gpu_smp->getInnerHeap() };
				cmdList->SetDescriptorHeaps(2, heaps);
#pragma endregion
			}

			// Foreach bound slot
			for (int i = startRootParameter; i < endRootParameter; i++)
			{
				SlotBinding binding = bindings[i];

				switch (binding.Root_Parameter.ParameterType)
				{
				case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
					switch (this->EngineType) {
					case Engine::Compute:
						cmdList->SetComputeRoot32BitConstants(i, binding.Root_Parameter.Constants.Num32BitValues,
							binding.ConstantData.ptrToConstant, 0);
						break;
					case Engine::Direct:
						cmdList->SetGraphicsRoot32BitConstants(i, binding.Root_Parameter.Constants.Num32BitValues,
							binding.ConstantData.ptrToConstant, 0);
						break;
					}
					break;
				case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
				{
#pragma region DESCRIPTOR TABLE
					// Gets the range length (if bound an array) or 1 if single.
					int count = binding.DescriptorData.ptrToCount == nullptr ? 1 : *binding.DescriptorData.ptrToCount;

					// Gets the bound resource if single
					gObj<ResourceView> resource = binding.DescriptorData.ptrToCount == nullptr ? *((gObj<ResourceView>*)binding.DescriptorData.ptrToResourceViewArray) : nullptr;

					// Gets the bound resources if array or treat the resource as a single array case
					gObj<ResourceView>* resourceArray = binding.DescriptorData.ptrToCount == nullptr ? &resource
						: *((gObj<ResourceView>**)binding.DescriptorData.ptrToResourceViewArray);

					// foreach resource in bound array (or single resource treated as array)
					for (int j = 0; j < count; j++)
					{
						// reference to the j-th resource (or bind null if array is null)
						gObj<ResourceView> resource = resourceArray == nullptr ? nullptr : *(resourceArray + j);

						if (!resource)
							// Grant a resource view to create null descriptor if missing resource.
							resource = dxwrapper->ResolveNullView(dxwrapper, binding.DescriptorData.Dimension);
						else
						{
							switch (binding.Root_Parameter.DescriptorTable.pDescriptorRanges[0].RangeType)
							{
							case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
							{
								D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
								switch (this->EngineType)
								{
								case Engine::Compute:
									state |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
									break;
								case Engine::Direct:
									state |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
									break;
								}

								wResource* iresource = resource->w_resource;
								iresource->AddBarrier(cmdList, state);
							}
							break;
							case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
								wResource* iresource = resource->w_resource;
								iresource->AddBarrier(cmdList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
								break;
							}
						}
						// Gets the cpu handle at not visible descriptor heap for the resource
						wResourceView* vresource = resource->w_view;
						D3D12_CPU_DESCRIPTOR_HANDLE handle = vresource->GetCPUHandleFor(binding.Root_Parameter.DescriptorTable.pDescriptorRanges[0].RangeType);

						// Adds the handle of the created descriptor into the src list.
						cmdWrapper->srcDescriptors.add(handle);
					}
					// add the descriptors range length
					cmdWrapper->dstDescriptorRangeLengths.add(count);
					int startIndex = dxwrapper->gpu_csu->AllocateInFrame(count);
					cmdWrapper->dstDescriptors.add(dxwrapper->gpu_csu->getCPUVersion(startIndex));

					switch (this->EngineType) {
					case Engine::Compute:
						cmdList->SetComputeRootDescriptorTable(i, dxwrapper->gpu_csu->getGPUVersion(startIndex));
						break;
					case Engine::Direct:
						cmdList->SetGraphicsRootDescriptorTable(i, dxwrapper->gpu_csu->getGPUVersion(startIndex));
						break;
					}
					break; // DESCRIPTOR TABLE
#pragma endregion
				}
				case D3D12_ROOT_PARAMETER_TYPE_CBV:
				{
#pragma region DESCRIPTOR CBV
					// Gets the range length (if bound an array) or 1 if single.
					gObj<ResourceView> resource = *((gObj<ResourceView>*)binding.DescriptorData.ptrToResourceViewArray);
					wResource* iresource = resource->w_resource;

					switch (this->EngineType) {
					case Engine::Compute:
						cmdList->SetComputeRootConstantBufferView(i, iresource->resource->GetGPUVirtualAddress());
						break;
					case Engine::Direct:
						cmdList->SetGraphicsRootConstantBufferView(i, iresource->resource->GetGPUVirtualAddress());
						break;
					}
					break; // DESCRIPTOR CBV
#pragma endregion
				}
				case D3D12_ROOT_PARAMETER_TYPE_SRV: // this type is used only for ADS
				{
					// Gets the range length (if bound an array) or 1 if single.
					gObj<InstanceCollection> scene = *((gObj<InstanceCollection>*)binding.SceneData.ptrToScene);
					if (scene->State() == CollectionState::NotBuilt)
						throw Exception::FromError(Errors::Invalid_Operation, "Scene should be loaded on the GPU first.");

					//TODO!!!
					//cmdList->SetComputeRootShaderResourceView(i, scene->wrapper->gpuVersion->topLevelAccDS->GetGPUVirtualAddress());
					break;
				}
				}
			}
		}
	};

	struct wPipelineState {
		// Gets or sets the binder.
		gObj<ComputeBinder> binder = nullptr;
		// DX12 wrapper.
		wDevice* wrapper = nullptr;
		// Gets or sets the root signature for this bindings
		DX_RootSignature rootSignature = nullptr;
		// Gets or sets the size of the root signature.
		int rootSignatureSize;
		// Gets or sets the pipeline object built for the pipeline
		DX_PipelineState pso = nullptr;
	};

#pragma region Binding Methods

	void ComputeBinder::CreateSignature(
		DX_Device device,
		D3D12_ROOT_SIGNATURE_FLAGS flags,
		DX_RootSignature& rootSignature,
		int& rootSignatureSize
	)
	{
		auto bindings = this->__InternalBindingObject->bindings;

		D3D12_ROOT_PARAMETER* parameters = new D3D12_ROOT_PARAMETER[bindings.size()];

		rootSignatureSize = 0;
		for (int i = 0; i < bindings.size(); i++)
		{
			parameters[i] = bindings[i].Root_Parameter;

			switch (bindings[i].Root_Parameter.ParameterType) {
			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
				rootSignatureSize += bindings[i].Root_Parameter.Constants.Num32BitValues * 4;
				break;
			default:
				rootSignatureSize += 4 * 4;
				break;
			}
		}

		D3D12_ROOT_SIGNATURE_DESC desc;
		desc.pParameters = parameters;
		desc.NumParameters = bindings.size();
		desc.pStaticSamplers = __InternalBindingObject->Static_Samplers;
		desc.NumStaticSamplers = __InternalBindingObject->StaticSamplerMax;
		desc.Flags = flags;

		ID3DBlob* signatureBlob;
		ID3DBlob* signatureErrorBlob;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC d = {};
		d.Desc_1_0 = desc;
		d.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;

		auto hr = D3D12SerializeVersionedRootSignature(&d, &signatureBlob, &signatureErrorBlob);

		if (hr != S_OK)
		{
			throw Exception::FromError(Errors::BadSignatureConstruction, "Error serializing root signature");
		}

		hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

		if (hr != S_OK)
		{
			throw Exception::FromError(Errors::BadSignatureConstruction, "Error creating root signature");
		}

		delete[] parameters;
	}

	ComputeBinder::ComputeBinder() {
		__InternalBindingObject = new wBindings();
		__InternalBindingObject->EngineType = Engine::Compute;
	}

	GraphicsBinder::GraphicsBinder() : ComputeBinder() {
		__InternalBindingObject->EngineType = Engine::Direct;
	}

	RaytracingBinder::RaytracingBinder() : ComputeBinder() {
		__InternalBindingObject->EngineType = Engine::Compute;
	}

	bool ComputeBinder::HasSomeBindings() {
		return
			__InternalBindingObject->bindings.size() > 0 ||
			__InternalBindingObject->StaticSamplerMax > 0;
	}

	void ComputeBinder::AddConstant(int slot, void* data, int size)
	{
		D3D12_ROOT_PARAMETER p = { };
		p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		p.Constants.Num32BitValues = size;
		p.Constants.RegisterSpace = space;
		p.Constants.ShaderRegister = slot;
		p.ShaderVisibility = visibility;

		SlotBinding b{  };
		b.Root_Parameter = p;
		b.ConstantData.ptrToConstant = data;
		__InternalBindingObject->AddBinding(collectGlobal, b);
	}

	void ComputeBinder::AddDescriptorRange(int slot, D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_RESOURCE_DIMENSION dimension, void* resource)
	{
		D3D12_ROOT_PARAMETER p = { };
		p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		p.DescriptorTable.NumDescriptorRanges = 1;
		p.ShaderVisibility = visibility;

		D3D12_DESCRIPTOR_RANGE range = { };
		range.BaseShaderRegister = slot;
		range.NumDescriptors = 1;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		range.RangeType = type;
		range.RegisterSpace = space;

		__InternalBindingObject->ranges.add(range);
		p.DescriptorTable.pDescriptorRanges = &__InternalBindingObject->ranges.last();

		SlotBinding b{  };
		b.Root_Parameter = p;
		b.DescriptorData.Dimension = dimension;
		b.DescriptorData.ptrToResourceViewArray = resource;
		__InternalBindingObject->AddBinding(collectGlobal, b);
	}

	void ComputeBinder::AddDescriptorRange(int initialSlot, D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_RESOURCE_DIMENSION dimension, void* resourceArray, int* count)
	{
		D3D12_ROOT_PARAMETER p = { };
		p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		p.DescriptorTable.NumDescriptorRanges = 1;
		p.ShaderVisibility = visibility;
		D3D12_DESCRIPTOR_RANGE range = { };
		range.BaseShaderRegister = initialSlot;
		range.NumDescriptors = -1;// undefined this moment
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		range.RangeType = type;
		range.RegisterSpace = space;

		__InternalBindingObject->ranges.add(range);
		p.DescriptorTable.pDescriptorRanges = &__InternalBindingObject->ranges.last();

		SlotBinding b{ };
		b.Root_Parameter = p;
		b.DescriptorData.Dimension = dimension;
		b.DescriptorData.ptrToResourceViewArray = resourceArray;
		b.DescriptorData.ptrToCount = count;
		__InternalBindingObject->AddBinding(collectGlobal, b);
	}

	void ComputeBinder::AddStaticSampler(int slot, const Sampler& sampler)
	{
		D3D12_STATIC_SAMPLER_DESC desc = { };
		desc.AddressU = sampler.AddressU;
		desc.AddressV = sampler.AddressV;
		desc.AddressW = sampler.AddressW;
		desc.BorderColor =
			!any(sampler.BorderColor - float4(0, 0, 0, 0)) ?
			D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK :
			!any(sampler.BorderColor - float4(0, 0, 0, 1)) ?
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK :
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		desc.ComparisonFunc = sampler.ComparisonFunc;
		desc.Filter = sampler.Filter;
		desc.MaxAnisotropy = sampler.MaxAnisotropy;
		desc.MaxLOD = sampler.MaxLOD;
		desc.MinLOD = sampler.MinLOD;
		desc.MipLODBias = sampler.MipLODBias;
		desc.RegisterSpace = space;
		desc.ShaderRegister = slot;
		desc.ShaderVisibility = visibility;

		__InternalBindingObject->Static_Samplers[__InternalBindingObject->StaticSamplerMax] = desc;
		__InternalBindingObject->StaticSamplerMax++;
	}

	void GraphicsBinder::SetRenderTarget(int slot, gObj<Texture2D>& const resource)
	{
		__InternalBindingObject->RenderTargets[slot] = &resource;
		__InternalBindingObject->RenderTargetMax = max(__InternalBindingObject->RenderTargetMax, slot + 1);
	}

	void GraphicsBinder::SetDSV(gObj<Texture2D>& const resource)
	{
		__InternalBindingObject->DepthBufferField = &resource;
	}

#pragma endregion

	void dx4xb::StaticPipelineBase::OnCreate(wDevice* w_device)
	{
		// Call setup to set all states of the setting object.
		Setup();

		// When a pipeline is loaded it should collect all bindings for global and local events.
		wPipelineState* wrapper = new wPipelineState();
		this->w_pipeline = wrapper;
		wrapper->wrapper = w_device;
		wrapper->binder = this->OnCollectBindings();
		wBindings* bindings = wrapper->binder->__InternalBindingObject;

		// Create the root signature for both groups together
		wrapper->binder->CreateSignature(
			w_device->device,
			getRootFlags(),
			wrapper->rootSignature,
			wrapper->rootSignatureSize
		);

		// Create the pipeline state object
#pragma region Create Pipeline State Object

		CompleteStateObject(wrapper->rootSignature);

		const D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = { (SIZE_T)getSettingObjectSize(), getSettingObject() };
		auto hr = w_device->device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&wrapper->pso));
		if (FAILED(hr)) {
			auto _hr = w_device->device->GetDeviceRemovedReason();
			throw Exception::FromError(Errors::BadPSOConstruction, nullptr, hr);
		}

#pragma endregion
	}

	void dx4xb::StaticPipelineBase::OnSet(wCmdList* w_cmdList)
	{
		// Set the pipeline state object into the cmdList
		wPipelineState* wrapper = this->w_pipeline;
		// Set pipeline state object
		w_cmdList->cmdList->SetPipelineState(wrapper->pso);
		// Set root signature
		if (this->GetEngine() == Engine::Compute)
			w_cmdList->cmdList->SetComputeRootSignature(wrapper->rootSignature);
		else
			w_cmdList->cmdList->SetGraphicsRootSignature(wrapper->rootSignature);
		// Bind global resources on the root signature
		gObj<ComputeBinder> binder = w_pipeline->binder;
		binder->__InternalBindingObject->BindToGPU(true, wrapper->wrapper, w_cmdList);
	}

	void dx4xb::StaticPipelineBase::OnDispatch(wCmdList* w_cmdList)
	{
		gObj<ComputeBinder> binder = w_pipeline->binder;
		binder->__InternalBindingObject->BindToGPU(false, w_pipeline->wrapper, w_cmdList);
	}

#pragma endregion

#pragma region RTX support

	CollectionState dx4xb::InstanceCollection::State()
	{
		throw Exception::FromError(Errors::Unsupported_Fallback);
	}

#pragma endregion

#pragma region Command List Management

	void dx4xb::GraphicsManager::Clear_RT(gObj<Texture2D> rt, const FLOAT values[4])
	{
		auto cmdList = this->w_cmdList->cmdList;
		wResource* resourceWrapper = rt->w_resource;
		wResourceView* view = rt->w_view;
		resourceWrapper->AddBarrier(cmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmdList->ClearRenderTargetView(view->getRTVHandle(), values, 0, nullptr);
	}

	void dx4xb::GraphicsManager::Clear_DepthStencil(gObj<Texture2D> depthStencil, float depth, UINT8 stencil, D3D12_CLEAR_FLAGS flags)
	{
		w_cmdList->cmdList->ClearDepthStencilView(depthStencil->w_view->getDSVHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}

	void dx4xb::CopyManager::Load_AllToGPU(gObj<ResourceView> resource)
	{
		resource->w_resource->UpdateResourceFromMapped(this->w_cmdList->cmdList, resource->w_view);
	}

	void dx4xb::CopyManager::Load_AllFromGPU(gObj<ResourceView> resource)
	{
		resource->w_resource->AddBarrier(this->w_cmdList->cmdList,
			D3D12_RESOURCE_STATE_COPY_SOURCE);
		resource->w_resource->UpdateMappedFromResource(this->w_cmdList->cmdList, resource->w_view);
	}

	void dx4xb::CopyManager::Load_RegionToGPU(gObj<ResourceView> singleSubresource, const D3D12_BOX& region)
	{
		singleSubresource->w_resource->UpdateRegionFromUploading(w_cmdList->cmdList,
			singleSubresource->w_view, region);
	}

	void dx4xb::CopyManager::Load_RegionFromGPU(gObj<ResourceView> singleSubresource, const D3D12_BOX& region)
	{
		singleSubresource->w_resource->UpdateRegionToDownloading(w_cmdList->cmdList,
			singleSubresource->w_view, region);
	}
	
	void dx4xb::CopyManager::Copy_Resource(gObj<Texture2D> dst, gObj<Texture2D> src)
	{
		dst->w_resource->AddBarrier(w_cmdList->cmdList,
			D3D12_RESOURCE_STATE_COPY_DEST);
		src->w_resource->AddBarrier(w_cmdList->cmdList,
			D3D12_RESOURCE_STATE_COPY_SOURCE);
		w_cmdList->cmdList->CopyResource(
			dst->w_resource->resource,
			src->w_resource->resource);
	}

	void dx4xb::ComputeManager::Set_Pipeline(gObj<Pipeline> pipeline)
	{
		w_cmdList->currentPipeline = pipeline;
		pipeline->OnSet(w_cmdList);
	}

	void dx4xb::ComputeManager::Dispatch_Threads(int dimx, int dimy, int dimz)
	{
		if (!w_cmdList->currentPipeline) {
			throw Exception::FromError(Errors::Invalid_Operation, "Pipeline should be set first");
		}
		w_cmdList->currentPipeline->OnDispatch(w_cmdList);
		w_cmdList->cmdList->Dispatch(dimx, dimy, dimz);

		// Grant a barrier for all UAVs used
		D3D12_RESOURCE_BARRIER b = { };
		b.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		b.UAV.pResource = nullptr;
		w_cmdList->cmdList->ResourceBarrier(1, &b);
	}

	void dx4xb::GraphicsManager::Set_Viewport(float width, float height, float maxDepth, float x, float y, float minDepth)
	{
		D3D12_VIEWPORT viewport;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = minDepth;
		viewport.MaxDepth = maxDepth;
		viewport.TopLeftX = x;
		viewport.TopLeftY = y;
		w_cmdList->cmdList->RSSetViewports(1, &viewport);

		D3D12_RECT rect;
		rect.left = (int)x;
		rect.top = (int)y;
		rect.right = (int)x + (int)width;
		rect.bottom = (int)y + (int)height;
		w_cmdList->cmdList->RSSetScissorRects(1, &rect);
	}

	void GraphicsManager::Set_VertexBuffer(gObj<Buffer> buffer, int slot)
	{
		buffer->w_resource->AddBarrier(w_cmdList->cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		D3D12_VERTEX_BUFFER_VIEW view;
		buffer->w_view->CreateVBV(view);
		w_cmdList->cmdList->IASetVertexBuffers(slot, 1, &view);
		if (slot == 0) {
			w_cmdList->vertexBufferSliceOffset = buffer->w_view->arrayStart;
		}
	}

	void GraphicsManager::Set_IndexBuffer(gObj<Buffer> buffer)
	{
		buffer->w_resource->AddBarrier(w_cmdList->cmdList, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		D3D12_INDEX_BUFFER_VIEW view;
		buffer->w_view->CreateIBV(view);
		w_cmdList->cmdList->IASetIndexBuffer(&view);
		w_cmdList->indexBufferSliceOffset = buffer->w_view->arrayStart;
	}

	void GraphicsManager::Dispatch_IndexedPrimitive(D3D_PRIMITIVE_TOPOLOGY topology, int count, int start)
	{
		if (!w_cmdList->currentPipeline) {
			throw Exception::FromError(Errors::Invalid_Operation, "Pipeline should be set first");
		}

		w_cmdList->currentPipeline->OnDispatch(w_cmdList);
		w_cmdList->cmdList->IASetPrimitiveTopology(topology);
		w_cmdList->cmdList->DrawIndexedInstanced(count, 1, start + w_cmdList->indexBufferSliceOffset, w_cmdList->vertexBufferSliceOffset, 0);
	}

	void GraphicsManager::Dispatch_Primitive(D3D_PRIMITIVE_TOPOLOGY topology, int count, int start)
	{
		if (!w_cmdList->currentPipeline) {
			throw Exception::FromError(Errors::Invalid_Operation, "Pipeline should be set first");
		}
		w_cmdList->currentPipeline->OnDispatch(w_cmdList);
		w_cmdList->cmdList->IASetPrimitiveTopology(topology);
		w_cmdList->cmdList->DrawInstanced(count, 1, start + w_cmdList->vertexBufferSliceOffset, 0);
	}

#pragma endregion

#pragma region Descriptor Manager

	CPUDescriptorHeapManager::CPUDescriptorHeapManager(DX_Device device, D3D12_DESCRIPTOR_HEAP_TYPE type, int capacity)
	{
		size = device->GetDescriptorHandleIncrementSize(type);
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NodeMask = 0;
		desc.NumDescriptors = capacity;
		desc.Type = type;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
		startCPU = heap->GetCPUDescriptorHandleForHeapStart().ptr;

		allocator = new Allocator();
	}

	GPUDescriptorHeapManager::GPUDescriptorHeapManager(DX_Device device, D3D12_DESCRIPTOR_HEAP_TYPE type, int capacity, int persistentCapacity, int buffers) :capacity(capacity) {
		size = device->GetDescriptorHandleIncrementSize(type);
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NodeMask = 0;
		desc.NumDescriptors = capacity;
		desc.Type = type;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		auto hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
		if (FAILED(hr))
			throw Exception::FromError(Errors::RunOutOfMemory, "Creating descriptor heaps.");
		startCPU = heap->GetCPUDescriptorHandleForHeapStart().ptr;
		startGPU = heap->GetGPUDescriptorHandleForHeapStart().ptr;

		frameCapacity = (capacity - persistentCapacity) / buffers;

		mallocOffset = 0;
		lastAvailableInBlock = frameCapacity;

		persistentAllocator = new Allocator();
	}

#pragma endregion

#pragma region Resources

	wResourceView* dx4xb::ResourceView::CreateDefaultView(
		wResource* w_resource, 
		wDevice* w_device)
	{
		wResourceView* result = new wResourceView(
			w_device,
			w_resource
		);
		auto desc = w_resource->desc;
		result->arrayCount =
			desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER ?
			w_resource->pTotalSizes / w_resource->elementStride :
			desc.DepthOrArraySize;
		result->mipCount = desc.MipLevels;
		result->ViewDimension = desc.Dimension;
		result->elementStride = w_resource->elementStride;
		return result;
	}

	dx4xb::ResourceView::ResourceView(wDevice* w_device, wResource* w_resource, wResourceView* w_view) :
		w_resource(w_resource),
		w_view(w_view ? w_view : CreateDefaultView(w_resource, w_device))
	{
		w_resource->references++;
	}

	dx4xb::ResourceView::~ResourceView()
	{
		this->w_resource->references--;
		if (this->w_resource->references <= 0)
			delete this->w_resource;
		delete this->w_view;
	}

	byte* dx4xb::ResourceView::MappedElement(int col, int row, int depthOrArray, int mip)
	{
		switch (w_resource->desc.Dimension) {
		case D3D12_RESOURCE_DIMENSION_BUFFER:
		{
			assert(row == 0 && "Row must be 0 in buffers");
			assert(depthOrArray == 0 && "Slice must be 0 in buffers");
			assert(mip == 0 && "Mip must be 0 in buffers");
			return w_resource->permanentUploadingMap + col * w_resource->elementStride;
		}
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		{
			int index = (w_view->arrayStart + depthOrArray) * w_resource->desc.MipLevels + w_view->mipStart + mip;

			auto footprint = w_resource->pLayouts[index];

			assert((row == 0 || w_resource->desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) && "Row must be 0 in Texture1D");
			return w_resource->permanentUploadingMap + footprint.Offset +
				row * footprint.Footprint.RowPitch + col * w_resource->elementStride;
		}
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		{
			int index = w_view->mipStart + mip;
			auto footprint = w_resource->pLayouts[index];
			return w_resource->permanentUploadingMap + footprint.Offset +
				(depthOrArray * footprint.Footprint.Height + row) * footprint.Footprint.RowPitch +
				col * w_resource->elementStride;
		}
		}
	}

	gObj<ResourceView> dx4xb::ResourceView::CreateNullView(wDevice* device, D3D12_RESOURCE_DIMENSION dimension)
	{
		D3D12_RESOURCE_DESC nullDesc = {
		};
		nullDesc.Dimension = dimension;
		nullDesc.Format = DXGI_FORMAT_R8_SINT;
		nullDesc.Width = 1;
		nullDesc.Height = 1;
		nullDesc.DepthOrArraySize = 1;
		nullDesc.MipLevels = 1;
		nullDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		wResource* nullresource = new wResource(device->device, nullptr, nullDesc, 1, 1, D3D12_RESOURCE_STATE_GENERIC_READ, CPUAccessibility::None);
		wResourceView* nullview = new wResourceView(device, nullresource);
		nullview->arrayCount = 0;
		nullview->mipCount = 1;
		nullview->elementStride = 1;
		nullview->ViewDimension = dimension;
		return new ResourceView(device, nullresource, nullview);
	}

	void dx4xb::ResourceView::SetDebugName(LPCWSTR name)
	{
		this->w_resource->resource->SetName(name);
	}

	CPUAccessibility dx4xb::ResourceView::CPUAccessibility() const
	{
		return w_resource->cpuaccess;
	}

	long dx4xb::ResourceView::SizeInBytes(int subresource) const
	{
		auto fp = w_resource->pLayouts[subresource].Footprint;
		return fp.Depth * fp.Height * fp.RowPitch;
	}

	int dx4xb::ResourceView::ElementStride() const
	{
		return w_view->elementStride;
	}

	int dx4xb::ResourceView::Subresources() const
	{
		return w_resource->subresources;
	}

	void dx4xb::ResourceView::Write(byte* data, bool flipRows)
	{
		w_resource->WriteToMapped(data, w_view, flipRows);
	}

	void dx4xb::ResourceView::Read(byte* data, bool flipRows)
	{
		w_resource->ReadFromMapped(data, w_view, flipRows);
	}

	void dx4xb::ResourceView::WriteRegion(byte* data, const D3D12_BOX& region, bool flipRows)
	{
		w_resource->WriteRegionToMappedSubresource(data, w_view, region, flipRows);
	}

	void dx4xb::ResourceView::ReadRegion(byte* data, const D3D12_BOX& region, bool flipRows)
	{
		w_resource->ReadRegionFromMappedSubresource(data, w_view, region, flipRows);
	}

	D3D12_GPU_VIRTUAL_ADDRESS Buffer::GPUVirtualAddress(int element) const
	{
		return w_resource->GetGPUVirtualAddress() + (element + w_view->arrayStart) * w_view->elementStride;
	}

	unsigned int dx4xb::Buffer::ElementCount() const
	{
		return w_view->arrayCount;
	}

	gObj<Buffer> Buffer::Slice(int start, int count) const {
		return new Buffer(
			w_view->w_device,
			w_resource,
			w_view->createSlicedClone(0, 0, start, count));
	}

	gObj<Texture1D> Texture1D::Slice_Mips(int start, int count) const {
		return new Texture1D(
			w_view->w_device,
			w_resource,
			w_view->createSlicedClone(start, count, 0, 0));
	}

	gObj<Texture1D> Texture1D::Slice_Array(int start, int count) const {
		return new Texture1D(
			w_view->w_device,
			w_resource,
			w_view->createSlicedClone(0, 0, start, count));
	}

	gObj<Texture2D> Texture2D::Slice_Mips(int start, int count) const {
		return new Texture2D(
			w_view->w_device,
			w_resource,
			w_view->createSlicedClone(start, count, 0, 0));
	}

	gObj<Texture2D> Texture2D::Slice_Array(int start, int count) const {
		return new Texture2D(
			w_view->w_device,
			w_resource,
			w_view->createSlicedClone(0, 0, start, count));
	}

	gObj<Texture3D> Texture3D::Slice_Mips(int start, int count) const {
		return new Texture3D(
			w_view->w_device,
			w_resource,
			w_view->createSlicedClone(start, count, 0, 0));
	}

	DXGI_FORMAT dx4xb::Texture1D::Format() const
	{
		return w_resource->desc.Format;
	}

	unsigned int dx4xb::Texture1D::Width() const
	{
		int index = w_view->arrayStart * w_resource->desc.MipLevels + w_view->mipStart;
		return w_resource->pLayouts[index].Footprint.Width;
	}
	
	unsigned int dx4xb::Texture1D::ArrayLength() const
	{
		return w_view->arrayCount;
	}

	unsigned int dx4xb::Texture1D::Mips() const
	{
		return w_view->mipCount;
	}

	DXGI_FORMAT dx4xb::Texture2D::Format() const
	{
		return w_resource->desc.Format;
	}

	unsigned int dx4xb::Texture2D::Width() const
	{
		int index = w_view->arrayStart * w_resource->desc.MipLevels + w_view->mipStart;
		return w_resource->pLayouts[index].Footprint.Width;
	}

	unsigned int dx4xb::Texture2D::Height() const
	{
		int index = w_view->arrayStart * w_resource->desc.MipLevels + w_view->mipStart;
		return w_resource->pLayouts[index].Footprint.Height;
	}

	unsigned int dx4xb::Texture2D::ArrayLength() const
	{
		return w_view->arrayCount;
	}

	unsigned int dx4xb::Texture2D::Mips() const
	{
		return w_view->mipCount;
	}

	DXGI_FORMAT dx4xb::Texture3D::Format() const
	{
		return w_resource->desc.Format;
	}

	unsigned int dx4xb::Texture3D::Width() const
	{
		return w_resource->pLayouts[w_view->mipStart].Footprint.Width;
	}

	unsigned int dx4xb::Texture3D::Height() const
	{
		return w_resource->pLayouts[w_view->mipStart].Footprint.Height;
	}

	unsigned int dx4xb::Texture3D::Depth() const
	{
		return w_resource->pLayouts[w_view->mipStart].Footprint.Depth;
	}

	unsigned int dx4xb::Texture3D::Mips() const
	{
		return w_view->mipCount;
	}

	int dx4xb::wResource::SizeOfFormatElement(DXGI_FORMAT format)
	{
		switch (format) {
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 128 / 8;
		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 96 / 8;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return 64 / 8;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return 32 / 8;
		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return 16 / 8;
		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
			return 8 / 8;
		}
	}

	wResource::wResource(
		DX_Device device,
		DX_Resource resource,
		const D3D12_RESOURCE_DESC& desc,
		int elementStride,
		int elementCount,
		D3D12_RESOURCE_STATES initialState,
		CPUAccessibility cpuAccessibility)
		: device(device), resource(resource), desc(desc), elementStride(elementStride == 0 ? SizeOfFormatElement(desc.Format) : elementStride)
	{
		if (resource == nullptr) // null resource for nullview
			return;

		cpuaccess = cpuAccessibility;

		LastUsageState = initialState; // state at creation

		subresources = desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? desc.MipLevels : desc.MipLevels * desc.DepthOrArraySize;

		pLayouts = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[subresources];
		pNumRows = new unsigned int[subresources];
		pRowSizesInBytes = new UINT64[subresources];
		device->GetCopyableFootprints(&desc, 0, subresources, 0, pLayouts, pNumRows, pRowSizesInBytes, &pTotalSizes);

		this->elementCount = elementCount != 0 ? elementCount : pTotalSizes / this->elementStride;
	}

	D3D12_GPU_VIRTUAL_ADDRESS wResource::GetGPUVirtualAddress() {
		return resource->GetGPUVirtualAddress();
	}

	//---Copied from d3d12x.h----------------------------------------------------------------------------
		// Row-by-row memcpy
	void MemcpySubresource(
		_In_ const D3D12_MEMCPY_DEST* pDest,
		_In_ const D3D12_SUBRESOURCE_DATA* pSrc,
		SIZE_T RowSizeInBytes,
		UINT NumRows,
		UINT NumSlices,
		bool flipRows = false
	)
	{
		for (UINT z = 0; z < NumSlices; ++z)
		{
			BYTE* pDestSlice = reinterpret_cast<BYTE*>(pDest->pData) + pDest->SlicePitch * z;
			const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pSrc->pData) + pSrc->SlicePitch * z;
			for (UINT y = 0; y < NumRows; ++y)
			{
				memcpy(pDestSlice + pDest->RowPitch * y,
					pSrcSlice + pSrc->RowPitch * (flipRows ? NumRows - y - 1 : y),
					RowSizeInBytes);
			}
		}
	}

	void wResource::__ResolveUploading() {
		if (!uploading) {
			mutex.Acquire();

			if (!uploading) {

				if (cpuaccess == CPUAccessibility::Write)
					uploading = resource; // use the resource for uploading directly
				else {
					D3D12_RESOURCE_DESC finalDesc = { };
					finalDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
					finalDesc.Format = DXGI_FORMAT_UNKNOWN;
					finalDesc.Width = pTotalSizes;
					finalDesc.Height = 1;
					finalDesc.DepthOrArraySize = 1;
					finalDesc.MipLevels = 1;
					finalDesc.SampleDesc.Count = 1;
					finalDesc.SampleDesc.Quality = 0;
					finalDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
					finalDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

					D3D12_HEAP_PROPERTIES uploadProp;
					uploadProp.Type = D3D12_HEAP_TYPE_UPLOAD;
					uploadProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					uploadProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
					uploadProp.VisibleNodeMask = 1;
					uploadProp.CreationNodeMask = 1;

					device->CreateCommittedResource(&uploadProp, D3D12_HEAP_FLAG_NONE, &finalDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
						IID_PPV_ARGS(&uploading));
				}

				// Automatically map the data to CPU to fill in next instructions
				// Uploading version is only required if some CPU data is gonna be copied to the GPU resource.
				D3D12_RANGE range{ };
				uploading->Map(0, &range, (void**)&permanentUploadingMap);
			}

			mutex.Release();
		}
	}

	void wResource::__ResolveDownloading() {
		if (!this->downloading) {
			mutex.Acquire();
			if (!downloading) {

				if (cpuaccess == CPUAccessibility::Read)
					downloading = resource; // Use directly the resource for downloading
				else {

					D3D12_RESOURCE_DESC finalDesc = { };
					finalDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
					finalDesc.Format = DXGI_FORMAT_UNKNOWN;
					finalDesc.Width = pTotalSizes;
					finalDesc.Height = 1;
					finalDesc.DepthOrArraySize = 1;
					finalDesc.MipLevels = 1;
					finalDesc.SampleDesc.Count = 1;
					finalDesc.SampleDesc.Quality = 0;
					finalDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
					finalDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

					D3D12_HEAP_PROPERTIES downloadProp;
					downloadProp.Type = D3D12_HEAP_TYPE_READBACK;
					downloadProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					downloadProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
					downloadProp.VisibleNodeMask = 1;
					downloadProp.CreationNodeMask = 1;
					device->CreateCommittedResource(&downloadProp, D3D12_HEAP_FLAG_NONE, &finalDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
						IID_PPV_ARGS(&downloading));
				}

			}

			mutex.Release();
		}
	}

	void wResource::AddBarrier(DX_CommandList cmdList, D3D12_RESOURCE_STATES dst) {
		// If the resource is used as UAV
		// Put a barrier to finish any pending read/write op
		if (this->LastUsageState & D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			D3D12_RESOURCE_BARRIER barrier = { };
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrier.UAV.pResource = this->resource;
			cmdList->ResourceBarrier(1, &barrier);
		}

		if (this->LastUsageState == dst)
			return;

		D3D12_RESOURCE_BARRIER barrier = { };
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = this->resource;
		barrier.Transition.StateAfter = dst;
		barrier.Transition.StateBefore = this->LastUsageState;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &barrier);
		this->LastUsageState = dst;
	}

	void wResource::AddUAVBarrier(DX_CommandList cmdList) {
		// If the resource is used as UAV
		// Put a barrier to finish any pending read/write op
		D3D12_RESOURCE_BARRIER barrier = { };
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.UAV.pResource = this->resource;
		cmdList->ResourceBarrier(1, &barrier);
	}

	void dx4xb::wResource::WriteToMapped(byte* data, wResourceView* view, bool flipRows)
	{
		__ResolveUploading();

		switch (desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			memcpy(
				permanentUploadingMap + view->arrayStart * elementStride,
				data,
				view->arrayCount * elementStride
			);
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		{
			byte* source = data;
			for (int m = 0; m < view->mipCount; m++)
			{
				int index = m + view->mipStart;
				auto subresourceFootprint = pLayouts[index].Footprint;

				D3D12_SUBRESOURCE_DATA sourceData;
				sourceData.pData = source;
				sourceData.RowPitch = subresourceFootprint.Width * elementStride;
				sourceData.SlicePitch = subresourceFootprint.Width * subresourceFootprint.Height * elementStride;

				D3D12_MEMCPY_DEST destData;
				destData.pData = permanentUploadingMap + pLayouts[index].Offset;
				destData.RowPitch = subresourceFootprint.RowPitch;
				destData.SlicePitch = subresourceFootprint.RowPitch * subresourceFootprint.Height;

				MemcpySubresource(&destData, &sourceData,
					subresourceFootprint.Width * elementStride,
					subresourceFootprint.Height,
					subresourceFootprint.Depth,
					flipRows
				);

				source += subresourceFootprint.Depth * subresourceFootprint.Height * subresourceFootprint.Width * elementStride;
			}
			break;
		}
		default:
		{
			byte* source = data;
			for (int a = 0; a < view->arrayCount; a++)
				for (int m = 0; m < view->mipCount; m++)
				{
					int index = (view->arrayStart + a) * (desc.MipLevels) + m + view->mipStart;

					auto subresourceFootprint = pLayouts[index].Footprint;

					D3D12_SUBRESOURCE_DATA sourceData;
					sourceData.pData = source;
					sourceData.RowPitch = subresourceFootprint.Width * elementStride;
					sourceData.SlicePitch = subresourceFootprint.Width * subresourceFootprint.Height * elementStride;

					D3D12_MEMCPY_DEST destData;
					destData.pData = permanentUploadingMap + pLayouts[index].Offset;
					destData.RowPitch = subresourceFootprint.RowPitch;
					destData.SlicePitch = subresourceFootprint.RowPitch * subresourceFootprint.Height;

					MemcpySubresource(&destData, &sourceData,
						subresourceFootprint.Width * elementStride,
						subresourceFootprint.Height,
						subresourceFootprint.Depth,
						flipRows
					);

					source += subresourceFootprint.Height * subresourceFootprint.Width * elementStride;
				}
			break;
		}
		}
	}

	void dx4xb::wResource::UpdateResourceFromMapped(DX_CommandList cmdList, wResourceView* view)
	{
		if (cpuaccess == CPUAccessibility::Write) // writable resource dont need to update.
			return;

		switch (desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			cmdList->CopyBufferRegion(resource, view->arrayStart * elementStride, uploading, view->arrayStart * elementStride, view->arrayCount * elementStride);
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		{
			for (int m = 0; m < view->mipCount; m++)
			{
				int index = m + view->mipStart;

				auto subresourceFootprint = pLayouts[index].Footprint;

				D3D12_TEXTURE_COPY_LOCATION dstData;
				dstData.pResource = resource;
				dstData.SubresourceIndex = index;
				dstData.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

				D3D12_TEXTURE_COPY_LOCATION srcData;
				srcData.pResource = uploading;
				srcData.PlacedFootprint = pLayouts[index];
				srcData.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

				cmdList->CopyTextureRegion(&dstData, 0, 0, 0, &srcData, nullptr);
			}
			break;
		}
		default:
		{	// Update slice region from uploading version to resource.
			for (int a = 0; a < view->arrayCount; a++)
				for (int m = 0; m < view->mipCount; m++)
				{
					int index = (view->arrayStart + a) * (desc.MipLevels) + m + view->mipStart;

					auto subresourceFootprint = pLayouts[index].Footprint;

					D3D12_TEXTURE_COPY_LOCATION dstData;
					dstData.pResource = resource;
					dstData.SubresourceIndex = index;
					dstData.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

					D3D12_TEXTURE_COPY_LOCATION srcData;
					srcData.pResource = uploading;
					srcData.PlacedFootprint = pLayouts[index];
					srcData.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

					cmdList->CopyTextureRegion(&dstData, 0, 0, 0, &srcData, nullptr);
				}
			break;
		}
		}
	}

	void dx4xb::wResource::ReadFromMapped(byte* data, wResourceView* view, bool flipRows)
	{
		// It is not necessary unless the data is in readback heap, in that case
		// only sets the downloading version to be the same resource, and maps.
		__ResolveDownloading();

		D3D12_RANGE range{ 0, pTotalSizes };

		auto hr = downloading->Map(0, &range, (void**)&permanentDownloadingMap);

		switch (desc.Dimension) {
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			memcpy(
				data,
				permanentDownloadingMap + view->arrayStart * elementStride,
				view->arrayCount * elementStride
			);
			break;
		default:
			byte* destination = data;
			for (int a = 0; a < view->arrayCount; a++)
				for (int m = 0; m < view->mipCount; m++)
				{
					int index = (view->arrayStart + a) * (desc.MipLevels) + m + view->mipStart;

					auto subresourceFootprint = pLayouts[index].Footprint;

					D3D12_SUBRESOURCE_DATA sourceData;
					sourceData.pData = permanentDownloadingMap + pLayouts[index].Offset;
					sourceData.RowPitch = subresourceFootprint.RowPitch;
					sourceData.SlicePitch = subresourceFootprint.RowPitch * subresourceFootprint.Height;

					D3D12_MEMCPY_DEST destData;
					destData.pData = destination;
					destData.RowPitch = subresourceFootprint.Width * elementStride;
					destData.SlicePitch = subresourceFootprint.Width * subresourceFootprint.Height * elementStride;

					MemcpySubresource(&destData, &sourceData,
						destData.RowPitch,
						subresourceFootprint.Height,
						subresourceFootprint.Depth,
						flipRows
					);

					destination += subresourceFootprint.Depth * subresourceFootprint.Height * destData.RowPitch;
				}
			break;
		}

		D3D12_RANGE writeRange = { };
		downloading->Unmap(0, &writeRange);
	}

	void dx4xb::wResource::UpdateMappedFromResource(DX_CommandList cmdList, wResourceView* view)
	{
		__ResolveDownloading();

		if (cpuaccess == CPUAccessibility::Read) // readable resource dont need to update.
			return;

		switch (desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			cmdList->CopyBufferRegion(downloading, view->arrayStart * elementStride, resource, view->arrayStart * elementStride, view->arrayCount * elementStride);
			break;
		default:
			// Update slice region from uploading version to resource.
			for (int a = 0; a < view->arrayCount; a++)
				for (int m = 0; m < view->mipCount; m++)
				{
					int index = (view->arrayStart + a) * (desc.MipLevels) + m + view->mipStart;

					auto subresourceFootprint = pLayouts[index].Footprint;

					D3D12_TEXTURE_COPY_LOCATION srcData;
					srcData.pResource = resource;
					srcData.SubresourceIndex = index;
					srcData.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

					D3D12_TEXTURE_COPY_LOCATION dstData;
					dstData.pResource = downloading;
					dstData.PlacedFootprint = pLayouts[index];
					dstData.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

					cmdList->CopyTextureRegion(&dstData, 0, 0, 0, &srcData, nullptr);
				}
			break;
		}
	}

	void dx4xb::wResource::WriteRegionToMappedSubresource(byte* data, wResourceView* view, const D3D12_BOX& region, bool flipRows)
	{
		__ResolveUploading();

		switch (desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			memcpy(
				permanentUploadingMap + (view->arrayStart + region.left) * elementStride,
				data,
				(region.right - region.left) * elementStride
			);
			break;
		default:
			byte* source = data;
			int index = (view->arrayStart) * (desc.MipLevels) + view->mipStart;

			auto subresourceFootprint = pLayouts[index].Footprint;

			D3D12_SUBRESOURCE_DATA sourceData;
			sourceData.pData = source;
			sourceData.RowPitch = (region.right - region.left) * elementStride;
			sourceData.SlicePitch = (region.right - region.left) * (region.bottom - region.top) * elementStride;

			D3D12_MEMCPY_DEST destData;
			destData.pData = permanentUploadingMap + pLayouts[index].Offset
				+ region.front * subresourceFootprint.RowPitch * subresourceFootprint.Height
				+ region.top * subresourceFootprint.RowPitch
				+ region.left * elementStride;
			destData.RowPitch = subresourceFootprint.RowPitch;
			destData.SlicePitch = subresourceFootprint.RowPitch * subresourceFootprint.Height;

			MemcpySubresource(&destData, &sourceData,
				(region.right - region.left) * elementStride,
				(region.bottom - region.top),
				(region.back - region.front),
				flipRows
			);
			break;
		}
	}

	void dx4xb::wResource::UpdateRegionFromUploading(DX_CommandList cmdList, wResourceView* view, const D3D12_BOX& region)
	{
		if (cpuaccess == CPUAccessibility::Write) // writable resource dont need to update.
			return;

		switch (desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			cmdList->CopyBufferRegion(
				resource, (view->arrayStart + region.left) * elementStride,
				uploading, (view->arrayStart + region.left) * elementStride, (region.right - region.left) * elementStride);
			break;
		default:
			// Update slice region from uploading version to resource.
			int index = view->arrayStart * desc.MipLevels + view->mipStart;

			auto subresourceFootprint = pLayouts[index].Footprint;

			D3D12_TEXTURE_COPY_LOCATION dstData;
			dstData.pResource = resource;
			dstData.SubresourceIndex = index;
			dstData.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

			D3D12_TEXTURE_COPY_LOCATION srcData;
			srcData.pResource = uploading;
			srcData.PlacedFootprint = pLayouts[index];
			srcData.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

			cmdList->CopyTextureRegion(&dstData, region.left, region.top, region.front, &srcData, &region);
			break;
		}
	}

	void dx4xb::wResource::ReadRegionFromMappedSubresource(byte* data, wResourceView* view, const D3D12_BOX& region, bool flipRows)
	{
		__ResolveDownloading();

		switch (desc.Dimension) {
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			memcpy(
				data,
				permanentDownloadingMap + (view->arrayStart + region.left) * elementStride,
				(region.right - region.left) * elementStride
			);
			break;
		default:
			byte* destination = data;
			int index = (view->arrayStart) * (desc.MipLevels) + view->mipStart;

			auto subresourceFootprint = pLayouts[index].Footprint;

			D3D12_SUBRESOURCE_DATA sourceData;
			sourceData.pData = permanentDownloadingMap + pLayouts[index].Offset
				+ region.front * subresourceFootprint.Height * subresourceFootprint.RowPitch
				+ region.top * subresourceFootprint.RowPitch
				+ region.left * elementStride;
			sourceData.RowPitch = subresourceFootprint.RowPitch;
			sourceData.SlicePitch = subresourceFootprint.RowPitch * subresourceFootprint.Height;

			D3D12_MEMCPY_DEST destData;
			destData.pData = destination;
			destData.RowPitch = (region.right - region.left) * elementStride;
			destData.SlicePitch = (region.right - region.left) * (region.bottom - region.top) * elementStride;

			MemcpySubresource(&destData, &sourceData,
				destData.RowPitch,
				(region.bottom - region.top),
				(region.back - region.front),
				flipRows
			);
			break;
		}
	}

	void dx4xb::wResource::UpdateRegionToDownloading(DX_CommandList cmdList, wResourceView* view, const D3D12_BOX& region)
	{
		if (cpuaccess == CPUAccessibility::Read) // readable resource dont need to update.
			return;

		switch (desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			cmdList->CopyBufferRegion(
				downloading, (view->arrayStart + region.left) * elementStride,
				resource, (view->arrayStart + region.left) * elementStride, (region.right - region.left) * elementStride);
			break;
		default:
			// Update slice region from uploading version to resource.
			int index = view->arrayStart * desc.MipLevels + view->mipStart;

			auto subresourceFootprint = pLayouts[index].Footprint;

			D3D12_TEXTURE_COPY_LOCATION dstData;
			dstData.pResource = downloading;
			dstData.PlacedFootprint = pLayouts[index];
			dstData.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

			D3D12_TEXTURE_COPY_LOCATION srcData;
			srcData.pResource = uploading;
			srcData.SubresourceIndex = index;
			srcData.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

			cmdList->CopyTextureRegion(&dstData, region.left, region.top, region.front, &srcData, &region);
			break;
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE wResourceView::GetCPUHandleFor(D3D12_DESCRIPTOR_RANGE_TYPE type) {
		switch (type) {
		case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
			return getSRVHandle();
		case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
			return getUAVHandle();
		case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
			return getCBVHandle();
		}
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}

	void wResourceView::CreateRTVDesc(D3D12_RENDER_TARGET_VIEW_DESC& d)
	{
		d.Texture2DArray.ArraySize = arrayCount;
		d.Texture2DArray.FirstArraySlice = arrayStart;
		d.Texture2DArray.MipSlice = mipStart;
		d.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		d.Format = !w_resource ? DXGI_FORMAT_UNKNOWN : w_resource->desc.Format;
	}

	void wResourceView::CreateSRVDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& d)
	{
		switch (this->ViewDimension) {
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			d.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d.Buffer.FirstElement = arrayStart;
			d.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			d.Buffer.NumElements = arrayCount;
			d.Buffer.StructureByteStride = elementStride;
			d.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			d.Format = !w_resource->resource ? DXGI_FORMAT_UNKNOWN : w_resource->desc.Format;
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			d.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d.Texture1DArray.ArraySize = arrayCount;
			d.Texture1DArray.FirstArraySlice = arrayStart;
			d.Texture1DArray.MipLevels = mipCount;
			d.Texture1DArray.MostDetailedMip = mipStart;
			d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			d.Format = !w_resource->resource ? DXGI_FORMAT_R8G8B8A8_UNORM : w_resource->desc.Format;
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			d.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d.Texture2DArray.ArraySize = arrayCount;
			d.Texture2DArray.FirstArraySlice = arrayStart;
			d.Texture2DArray.MipLevels = mipCount;
			d.Texture2DArray.MostDetailedMip = mipStart;
			d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			d.Format = !w_resource->resource ? DXGI_FORMAT_R8G8B8A8_UNORM : w_resource->desc.Format;
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			d.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d.Texture3D.MipLevels = mipCount;
			d.Texture3D.MostDetailedMip = mipStart;
			d.Texture3D.ResourceMinLODClamp = 0;
			d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			d.Format = !w_resource->resource ? DXGI_FORMAT_R8G8B8A8_UNORM : w_resource->desc.Format;
			break;
		}
	}

	void wResourceView::CreateUAVDesc(D3D12_UNORDERED_ACCESS_VIEW_DESC& d)
	{
		switch (this->ViewDimension) {
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			d.Buffer.CounterOffsetInBytes = 0;
			d.Buffer.FirstElement = arrayStart;
			d.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			d.Buffer.NumElements = arrayCount;
			d.Buffer.StructureByteStride = elementStride;
			d.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			d.Format = DXGI_FORMAT_UNKNOWN;// !resource ? DXGI_FORMAT_UNKNOWN : resource->desc.Format;
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			d.Texture1DArray.ArraySize = arrayCount;
			d.Texture1DArray.FirstArraySlice = arrayStart;
			d.Texture1DArray.MipSlice = mipStart;
			d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
			d.Format = !w_resource->resource ? DXGI_FORMAT_R8G8B8A8_UNORM : w_resource->desc.Format;
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			d.Texture2DArray.ArraySize = arrayCount;
			d.Texture2DArray.FirstArraySlice = arrayStart;
			d.Texture2DArray.MipSlice = mipStart;
			d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			d.Format = !w_resource->resource ? DXGI_FORMAT_R8G8B8A8_UNORM : w_resource->desc.Format;
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			d.Texture3D.WSize = arrayCount;
			d.Texture3D.FirstWSlice = arrayStart;
			d.Texture3D.MipSlice = mipStart;
			d.Format = !w_resource->resource ? DXGI_FORMAT_R8G8B8A8_UNORM : w_resource->desc.Format;
			break;
		}
	}

	void wResourceView::CreateVBV(D3D12_VERTEX_BUFFER_VIEW& desc) {
		desc = { };
		desc.BufferLocation = !w_resource->resource ? 0 : w_resource->resource->GetGPUVirtualAddress();
		desc.StrideInBytes = elementStride;
		desc.SizeInBytes = (arrayStart + arrayCount) * elementStride;
	}

	void wResourceView::CreateIBV(D3D12_INDEX_BUFFER_VIEW& desc) {
		desc = { };
		desc.BufferLocation = !w_resource->resource ? 0 : w_resource->resource->GetGPUVirtualAddress();
		desc.Format = !w_resource ? DXGI_FORMAT_UNKNOWN : elementStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		desc.SizeInBytes = (arrayStart + arrayCount) * elementStride;
	}

	void wResourceView::CreateDSVDesc(D3D12_DEPTH_STENCIL_VIEW_DESC& d)
	{
		d.Texture2DArray.ArraySize = arrayCount;
		d.Texture2DArray.FirstArraySlice = arrayStart;
		d.Texture2DArray.MipSlice = mipStart;
		d.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		d.Format = !w_resource->resource ? DXGI_FORMAT_UNKNOWN : w_resource->desc.Format;
	}

	void wResourceView::CreateCBVDesc(D3D12_CONSTANT_BUFFER_VIEW_DESC& d) {
		d.BufferLocation = !w_resource ? 0 : w_resource->resource->GetGPUVirtualAddress();
		d.SizeInBytes = (this->elementStride * this->arrayCount + 255) & ~255;
	}

	int wResourceView::getSRV() {
		if ((handle_mask & 1) != 0)
			return srv_cached_handle;

		mutex.Acquire();
		if ((handle_mask & 1) == 0)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC d;
			ZeroMemory(&d, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
			CreateSRVDesc(d);
			srv_cached_handle = w_device->cpu_csu->AllocateNewHandle();
			w_resource->device->CreateShaderResourceView(!w_resource ? nullptr : w_resource->resource, &d, w_device->cpu_csu->getCPUVersion(srv_cached_handle));
			handle_mask |= 1;
		}
		mutex.Release();
		return srv_cached_handle;
	}

	int wResourceView::getUAV() {
		if ((handle_mask & 2) != 0)
			return uav_cached_handle;

		mutex.Acquire();
		if ((handle_mask & 2) == 0)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC d;
			ZeroMemory(&d, sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC));
			CreateUAVDesc(d);
			uav_cached_handle = w_device->cpu_csu->AllocateNewHandle();
			w_resource->device->CreateUnorderedAccessView(!w_resource ? nullptr : w_resource->resource, nullptr, &d, w_device->cpu_csu->getCPUVersion(uav_cached_handle));
			handle_mask |= 2;
		}
		mutex.Release();
		return uav_cached_handle;
	}

	int wResourceView::getCBV() {
		if ((handle_mask & 4) != 0)
			return cbv_cached_handle;

		mutex.Acquire();
		if ((handle_mask & 4) == 0)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC d;
			ZeroMemory(&d, sizeof(D3D12_CONSTANT_BUFFER_VIEW_DESC));
			CreateCBVDesc(d);
			cbv_cached_handle = w_device->cpu_csu->AllocateNewHandle();
			w_resource->device->CreateConstantBufferView(&d, w_device->cpu_csu->getCPUVersion(cbv_cached_handle));
			handle_mask |= 4;
		}
		mutex.Release();
		return cbv_cached_handle;
	}

	int wResourceView::getRTV() {
		if ((handle_mask & 8) != 0)
			return rtv_cached_handle;

		mutex.Acquire();

		if ((handle_mask & 8) == 0) {
			D3D12_RENDER_TARGET_VIEW_DESC d;
			ZeroMemory(&d, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));
			CreateRTVDesc(d);
			rtv_cached_handle = w_device->cpu_rt->AllocateNewHandle();
			w_resource->device->CreateRenderTargetView(!w_resource ? nullptr : w_resource->resource, &d, w_device->cpu_rt->getCPUVersion(rtv_cached_handle));
			handle_mask |= 8;
		}

		mutex.Release();
		return rtv_cached_handle;
	}

	int wResourceView::getDSV() {

		if ((handle_mask & 16) != 0)
			return dsv_cached_handle;

		mutex.Acquire();

		if ((handle_mask & 16) == 0) {
			D3D12_DEPTH_STENCIL_VIEW_DESC d;
			ZeroMemory(&d, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
			CreateDSVDesc(d);
			dsv_cached_handle = w_device->cpu_ds->AllocateNewHandle();
			w_resource->device->CreateDepthStencilView(!w_resource ? nullptr : w_resource->resource, &d, w_device->cpu_ds->getCPUVersion(dsv_cached_handle));
			handle_mask |= 16;
		}

		mutex.Release();
		return dsv_cached_handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE wResourceView::getDSVHandle() {
		return w_device->cpu_ds->getCPUVersion(getDSV());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE wResourceView::getRTVHandle() {
		return w_device->cpu_rt->getCPUVersion(getRTV());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE wResourceView::getSRVHandle() {
		return w_device->cpu_csu->getCPUVersion(getSRV());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE wResourceView::getCBVHandle() {
		return w_device->cpu_csu->getCPUVersion(getCBV());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE wResourceView::getUAVHandle() {
		return w_device->cpu_csu->getCPUVersion(getUAV());
	}

	wResourceView* wResourceView::createSlicedClone(
		int mipOffset, int mipNewCount,
		int arrayOffset, int arrayNewCount) {
		wResourceView* result = new wResourceView(this->w_device, this->w_resource);

		result->elementStride = this->elementStride;
		result->ViewDimension = this->ViewDimension;

		if (mipNewCount > 0) {
			result->mipStart = this->mipStart + mipOffset;
			result->mipCount = mipNewCount;
		}
		else {
			result->mipStart = this->mipStart;
			result->mipCount = this->mipCount;
		}

		if (arrayNewCount > 0) {
			result->arrayStart = this->arrayStart + arrayOffset;
			result->arrayCount = arrayNewCount;
		}
		else {
			result->arrayStart = this->arrayStart;
			result->arrayCount = this->arrayCount;
		}

		return result;
	}


#pragma endregion

#pragma region Imaging

	gObj<TextureData> CreateTextureDataFromScratchImage(DirectX::TexMetadata& metadata, DirectX::ScratchImage& image, bool flipY) {
		byte* buffer = new byte[image.GetPixelsSize()];
		memcpy(buffer, image.GetPixels(), image.GetPixelsSize());

		return metadata.dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D ?
			new TextureData(metadata.width, metadata.height, metadata.mipLevels, metadata.arraySize, metadata.format, buffer, image.GetPixelsSize(), flipY)
			: new TextureData(metadata.width, metadata.height, metadata.depth, metadata.format, buffer, image.GetPixelsSize(), flipY);
	}

	TextureData::TextureData(int width, int height, int mipMaps, int slices, DXGI_FORMAT format, byte* data, long long dataSize, bool flipY)
		:Width(width), Height(height),
		MipMaps(mipMaps),
		ArraySlices(slices),
		Format(format),
		Data(data),
		DataSize(dataSize),
		FlipY(flipY),
		pixelStride(DirectX::BitsPerPixel(format) / 8),
		Dimension(D3D12_RESOURCE_DIMENSION_TEXTURE2D)
	{
		footprints = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[mipMaps * slices];
		int index = 0;
		UINT64 offset = 0;
		for (int s = 0; s < slices; s++)
		{
			int w = Width;
			int h = Height;
			for (int m = 0; m < mipMaps; m++)
			{
				footprints[index].Footprint.Width = w;
				footprints[index].Footprint.Height = h;
				footprints[index].Footprint.Format = format;
				footprints[index].Footprint.RowPitch = pixelStride * w;
				footprints[index].Footprint.Depth = 1;
				footprints[index].Offset = offset;

				offset += w * h * pixelStride;

				w /= 2; w = max(1, w);
				h /= 2; h = max(1, h);

				index++;
			}
		}
	}

	TextureData::TextureData(int width, int height, int depth, DXGI_FORMAT format, byte* data, long long dataSize, bool flipY)
		:Width(width), Height(height),
		MipMaps(1),
		Depth(depth),
		Format(format),
		Data(data),
		DataSize(dataSize),
		FlipY(flipY),
		pixelStride(DirectX::BitsPerPixel(format) / 8),
		Dimension(D3D12_RESOURCE_DIMENSION_TEXTURE3D)
	{
		footprints = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[1];

		footprints[0].Footprint.Width = width;
		footprints[0].Footprint.Height = height;
		footprints[0].Footprint.Depth = depth;
		footprints[0].Footprint.Format = format;
		footprints[0].Footprint.RowPitch = pixelStride * width;
		footprints[0].Offset = 0;
	}

	TextureData::~TextureData() {
		delete[] Data;
		delete[] footprints;
	}

	gObj<TextureData> TextureData::CreateEmpty(int width, int height, int mipMaps, int slices, DXGI_FORMAT format) {

		int pixelStride = DirectX::BitsPerPixel(format) / 8;
		UINT64 total = 0;
		int w = width, h = height;
		for (int i = 0; i < mipMaps; i++)
		{
			total += w * h * pixelStride;
			w /= 2;
			h /= 2;
			w = max(1, w);
			h = max(1, h);
		}
		total *= slices;
		byte* data = new byte[total];
		ZeroMemory(data, total);
		return new TextureData(width, height, mipMaps, slices, format, data, total, false);
	}

	gObj<TextureData> TextureData::CreateEmpty(int width, int height, int depth, DXGI_FORMAT format) {
		int pixelStride = DirectX::BitsPerPixel(format) / 8;
		UINT64 total = width * height * depth * pixelStride;
		byte* data = new byte[total];
		ZeroMemory(data, total);
		return new TextureData(width, height, depth, format, data, total, false);
	}

	gObj<TextureData> TextureData::LoadFromFile(const char* filename) {

		static bool InitializedWic = false;

		if (!InitializedWic) {
			void* res = nullptr;
			CoInitialize(res);
			InitializedWic = true;
		}

		wchar_t* file = new wchar_t[strlen(filename) + 1];
		OemToCharW(filename, file);
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage scratchImage;

		bool notDDS;
		bool notWIC;
		bool notTGA;

		if (
			(notDDS = FAILED(DirectX::LoadFromDDSFile(file, 0, &metadata, scratchImage)))
			&& (notWIC = FAILED(DirectX::LoadFromWICFile(file, 0, &metadata, scratchImage)))
			&& (notTGA = FAILED(DirectX::LoadFromTGAFile(file, &metadata, scratchImage))))
			return nullptr;

		return CreateTextureDataFromScratchImage(metadata, scratchImage, true);
	}

	void TextureData::SaveToFile(gObj<TextureData> data, const char* filename) {
		static bool InitializedWic = false;

		if (!InitializedWic) {
			void* res = nullptr;
			CoInitialize(res);
			InitializedWic = true;
		}

		DirectX::Image img;
		img.format = data->Format;
		img.width = data->Width;
		img.height = data->Height;
		img.rowPitch = data->pixelStride * data->Width;
		img.slicePitch = data->pixelStride * data->Width * data->Height;
		img.pixels = data->Data;

		wchar_t* file = new wchar_t[strlen(filename) + 1];
		OemToCharW(filename, file);

		DirectX::SaveToWICFile(img, DirectX::WIC_FLAGS_FORCE_RGB, DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), file);
	}

#pragma endregion

#pragma region Device Wrapper

	void wDevice::Initialize(const PresenterDescription& desc)
	{
		this->desc = desc;

		UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		{
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif

		CComPtr<IDXGIFactory4> factory;
#if _DEBUG
		CComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
		{
			CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
		else
			CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
#else
		CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
#endif

		if (desc.UseWarpDevice)
		{
			CComPtr<IDXGIAdapter> warpAdapter;
			factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

			D3D12CreateDevice(
				warpAdapter,
				D3D_FEATURE_LEVEL_12_1,
				IID_PPV_ARGS(&device)
			);
		}
		else
		{
			CComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(factory, hardwareAdapter);

			EnableComputeRaytracingFallback(hardwareAdapter);

			D3D12CreateDevice(
				hardwareAdapter,
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&device)
			);
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS ops;
		device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &ops, sizeof(ops));

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = { };
		fullScreenDesc.Windowed = true;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = desc.Frames;
		swapChainDesc.Width = desc.ResolutionWidth;
		swapChainDesc.Height = desc.ResolutionHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;


		scheduler = new wScheduler(this, desc.Frames, desc.Threads);

		IDXGISwapChain1* tmpSwapChain;

		factory->CreateSwapChainForHwnd(
			scheduler->Engines[0].queue->dxQueue,        // Swap chain needs the queue so that it can force a flush on it.
			desc.hWnd,
			&swapChainDesc,
			&fullScreenDesc,
			nullptr,
			&tmpSwapChain
		);

		this->swapChain = (IDXGISwapChain3*)tmpSwapChain;
		this->swapChain->SetMaximumFrameLatency(swapChainDesc.BufferCount);

		// This sample does not support fullscreen transitions.
		factory->MakeWindowAssociation(desc.hWnd, DXGI_MWA_NO_ALT_ENTER);

		// Initialize descriptor heaps

		gui_csu = new GPUDescriptorHeapManager(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 100, 100, 1);

		gpu_csu = new GPUDescriptorHeapManager(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 900000, 1000, swapChainDesc.BufferCount);
		gpu_smp = new GPUDescriptorHeapManager(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 2000, 100, swapChainDesc.BufferCount);

		cpu_rt = new CPUDescriptorHeapManager(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1000);
		cpu_ds = new CPUDescriptorHeapManager(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1000);
		cpu_csu = new CPUDescriptorHeapManager(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1000000);
		cpu_smp = new CPUDescriptorHeapManager(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 2000);

		// Create rendertargets resources.
		{
			RenderTargets = new gObj<Texture2D>[swapChainDesc.BufferCount];
			RenderTargetsRTV = new D3D12_CPU_DESCRIPTOR_HANDLE[swapChainDesc.BufferCount];
			// Create a RTV and a command allocator for each frame.
			for (UINT n = 0; n < swapChainDesc.BufferCount; n++)
			{
				DX_Resource rtResource;
				swapChain->GetBuffer(n, IID_PPV_ARGS(&rtResource));

				auto desc = rtResource->GetDesc();

				wResource* rw = new wResource(this->device, rtResource, desc, 0, 0, D3D12_RESOURCE_STATE_COPY_DEST, CPUAccessibility::None);

				RenderTargets[n] = new Texture2D(this, rw, nullptr);
				RenderTargetsRTV[n] = RenderTargets[n]->w_view->getRTVHandle();
			}
		}
	}

#pragma endregion

#pragma region Device Manager

	void DeviceManager::Dispatch_Process(gObj<GPUProcess> process) {
		device->scheduler->Enqueue(process);
	}

	void DeviceManager::Dispatch_Process_Async(gObj<GPUProcess> process) {
		device->scheduler->EnqueueAsync(process);
	}

	gObj<Texture2D> DeviceManager::CurrentRenderTarget() {
		return device->RenderTargets[device->scheduler->CurrentFrameIndex];
	}

	wResource* DeviceManager::CreateResource(
		const D3D12_RESOURCE_DESC& desc,
		int elementStride,
		int elementCount,
		D3D12_RESOURCE_STATES initialState,
		D3D12_CLEAR_VALUE* clearing,
		CPUAccessibility accessibility
	) {
		D3D12_HEAP_PROPERTIES heapProp;

		switch (accessibility) {
		case CPUAccessibility::None:
			heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProp.VisibleNodeMask = 1;
			heapProp.CreationNodeMask = 1;
			break;
		case CPUAccessibility::Write:
			heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProp.VisibleNodeMask = 1;
			heapProp.CreationNodeMask = 1;
			break;
		case CPUAccessibility::Read:
			heapProp.Type = D3D12_HEAP_TYPE_READBACK;
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProp.VisibleNodeMask = 1;
			heapProp.CreationNodeMask = 1;
			break;
		}

		DX_Resource resource;
		auto hr = device->device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, initialState, clearing,
			IID_PPV_ARGS(&resource));

		if (FAILED(hr))
		{
			auto _hr = device->device->GetDeviceRemovedReason();
			throw Exception::FromError(Errors::RunOutOfMemory, nullptr, _hr);
		}

		return new wResource(device->device, resource, desc, elementStride, elementCount, initialState, accessibility);
	}

	gObj<Buffer> DeviceManager::Create_Buffer_CB(int elementStride, bool dynamic) 
	{
		D3D12_RESOURCE_DESC desc = { };
		FillBufferDescription(desc, (elementStride + 255) & (~255));
		return CreateResourceView<Buffer>(desc, elementStride, 1, dynamic ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST, nullptr, dynamic ? CPUAccessibility::Write : CPUAccessibility::None);
	}

	gObj<Buffer> DeviceManager::Create_Buffer_ADS(int elementStride, int count) 
	{
		if (count == 0) return nullptr;
		D3D12_RESOURCE_DESC desc = { };
		FillBufferDescription(desc, elementStride * count, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		return CreateResourceView<Buffer>(desc, elementStride, count, D3D12_RESOURCE_STATE_COMMON | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	gObj<Buffer> DeviceManager::Create_Buffer_SRV(int elementStride, int count) 
	{
		if (count == 0) return nullptr;
		D3D12_RESOURCE_DESC desc = { };
		FillBufferDescription(desc, elementStride * count);
		return CreateResourceView<Buffer>(desc, elementStride, count, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	gObj<Buffer> DeviceManager::Create_Buffer_VB(int elementStride, int count) 
	{
		if (count == 0) return nullptr;
		D3D12_RESOURCE_DESC desc = { };
		FillBufferDescription(desc, elementStride * count);
		return CreateResourceView<Buffer>(desc, elementStride, count, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	gObj<Buffer> DeviceManager::Create_Buffer_IB(int elementStride, int count)
	{
		if (count == 0) return nullptr;
		D3D12_RESOURCE_DESC desc = { };
		FillBufferDescription(desc, elementStride * count);
		return CreateResourceView<Buffer>(desc, elementStride, count, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	gObj<Buffer> DeviceManager::Create_Buffer_UAV(int elementStride, int count) 
	{
		if (count == 0) return nullptr;
		D3D12_RESOURCE_DESC desc = { };
		FillBufferDescription(desc, elementStride * count, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		return CreateResourceView<Buffer>(desc, elementStride, count, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	gObj<Texture1D> dx4xb::DeviceManager::Create_Texture1D_SRV(DXGI_FORMAT format, int width, int mips, int arrayLength)
	{
		D3D12_RESOURCE_DESC desc = { };
		FillTexture1DDescription(format, desc, width, mips, arrayLength);
		return CreateResourceView<Texture1D>(desc, 0, 0, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	gObj<Texture1D> dx4xb::DeviceManager::Create_Texture1D_UAV(DXGI_FORMAT format, int width, int mips, int arrayLength)
	{
		D3D12_RESOURCE_DESC desc = { };
		FillTexture1DDescription(format, desc, width, mips, arrayLength, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		return CreateResourceView<Texture1D>(desc, 0, 0, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	gObj<Texture2D> dx4xb::DeviceManager::Create_Texture2D_SRV(DXGI_FORMAT format, int width, int height, int mips, int arrayLength)
	{
		D3D12_RESOURCE_DESC desc = { };
		FillTexture2DDescription(format, desc, width, height, mips, arrayLength);
		return CreateResourceView<Texture2D>(desc, 0, 0, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	gObj<Texture2D> dx4xb::DeviceManager::Create_Texture2D_SRV(dx4xb::string filePath)
	{
		auto data = TextureData::LoadFromFile(filePath.c_str());
		gObj<Texture2D> texture = Create_Texture2D_SRV(data->Format, data->Width, data->Height, data->MipMaps, data->ArraySlices);
		texture->Write(data->Data, true);
		return texture;
	}

	void dx4xb::DeviceManager::Create_File(dx4xb::string filePath, gObj<Texture2D> texture)
	{
		gObj<TextureData> data = TextureData::CreateEmpty(texture->Width(), texture->Height(), 1, 1, texture->w_resource->desc.Format);
		texture->Read(data->Data);
		TextureData::SaveToFile(data, filePath.c_str());
	}

	gObj<Texture2D> dx4xb::DeviceManager::Create_Texture2D_UAV(DXGI_FORMAT format, int width, int height, int mips, int arrayLength)
	{
		D3D12_RESOURCE_DESC desc = { };
		FillTexture2DDescription(format, desc, width, height, mips, arrayLength, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		return CreateResourceView<Texture2D>(desc, 0, 0, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	gObj<Texture2D> dx4xb::DeviceManager::Create_Texture2D_RT(DXGI_FORMAT format, int width, int height, int mips, int arrayLength)
	{
		D3D12_RESOURCE_DESC desc = { };
		FillTexture2DDescription(format, desc, width, height, mips, arrayLength, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		return CreateResourceView<Texture2D>(desc, 0, 0, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	gObj<Texture2D> dx4xb::DeviceManager::Create_Texture2D_DSV(int width, int height, DXGI_FORMAT format)
	{
		D3D12_RESOURCE_DESC desc = { };
		FillTexture2DDescription(format, desc, width, height, 1, 1, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		D3D12_CLEAR_VALUE clearing;
		clearing.DepthStencil = D3D12_DEPTH_STENCIL_VALUE{
			1.0f,
			0
		};
		clearing.Format = format;
		return CreateResourceView<Texture2D>(desc, 0, 0, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearing);
	}

	gObj<Texture3D> dx4xb::DeviceManager::Create_Texture3D_SRV(DXGI_FORMAT format, int width, int height, int depth, int mips)
	{
		D3D12_RESOURCE_DESC desc = { };
		FillTexture3DDescription(format, desc, width, height, depth, mips);
		return CreateResourceView<Texture3D>(desc, 0, 0, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	gObj<Texture3D> dx4xb::DeviceManager::Create_Texture3D_UAV(DXGI_FORMAT format, int width, int height, int depth, int mips)
	{
		D3D12_RESOURCE_DESC desc = { };
		FillTexture3DDescription(format, desc, width, height, depth, mips, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		return CreateResourceView<Texture3D>(desc, 0, 0, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	Signal DeviceManager::Flush(EngineMask mask) {
		return device->scheduler->FlushAndSignal(mask);
	}

	void Technique::OnCreate(wDevice* w_device) {
		this->device = w_device;
		OnLoad();
	}

	gObj<Presenter> Presenter::Create(const PresenterDescription& desc)
	{
		Presenter* result = new Presenter();
		result->device = new wDevice();
		result->device->Initialize(desc);
		return result;
	}

	void Presenter::BeginFrame() {
		device->scheduler->PrepareRenderTarget(D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	void Presenter::EndFrame() {
		// Wait for all 
		device->scheduler->FinishFrame();

		auto hr = device->swapChain->Present(0, 0);

		if (hr != S_OK) {
			HRESULT r = device->device->GetDeviceRemovedReason();
			throw Exception::FromError(Errors::Any, nullptr, r);
		}

		device->scheduler->SetupFrame(device->swapChain->GetCurrentBackBufferIndex());
	}

	Presenter::~Presenter() {
		delete device;
	}

	void Presenter::GetInternalDXInfo(InternalDXInfo& info) {
		info.Buffers = device->desc.Frames;
		info.device = device->device;
		info.swapChain = device->swapChain;
		info.mainCmdList = device->scheduler->Engines[0].threadInfos[0].cmdList;
		info.hWnd = device->desc.hWnd;
		info.RenderTargetFormat = device->RenderTargets[0]->Format();
		info.RenderTargets = device->RenderTargetsRTV;
	}

#pragma endregion

}


