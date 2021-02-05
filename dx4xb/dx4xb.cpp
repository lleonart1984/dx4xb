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
		D3D12_RESOURCE_STATES initialState,
		CPUAccessibility cpuAccessibility)
		: device(device), resource(resource), desc(desc), elementStride(elementStride)
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
	}

	inline D3D12_GPU_VIRTUAL_ADDRESS wResource::GetGPUVirtualAddress() {
		return resource->GetGPUVirtualAddress();
	}

	//---Copied from d3d12x.h----------------------------------------------------------------------------
		// Row-by-row memcpy
	inline void MemcpySubresource(
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

		//// Create rendertargets resources.
		//{
		//	RenderTargets = new gObj<Texture2D>[swapChainDesc.BufferCount];
		//	RenderTargetsRTV = new D3D12_CPU_DESCRIPTOR_HANDLE[swapChainDesc.BufferCount];
		//	// Create a RTV and a command allocator for each frame.
		//	for (UINT n = 0; n < swapChainDesc.BufferCount; n++)
		//	{
		//		DX_Resource rtResource;
		//		swapChain->GetBuffer(n, IID_PPV_ARGS(&rtResource));

		//		auto desc = rtResource->GetDesc();

		//		DX_ResourceWrapper* rw = new DX_ResourceWrapper(this, rtResource, desc, D3D12_RESOURCE_STATE_COPY_DEST, CPUAccessibility::None);

		//		RenderTargets[n] = new Texture2D(rw, nullptr, desc.Format, desc.Width, desc.Height, 1, 1);
		//		RenderTargetsRTV[n] = RenderTargets[n]->__InternalViewWrapper->getRTVHandle();
		//	}
		//}
	}

#pragma endregion

#pragma region Device Manager

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

#pragma endregion

}


