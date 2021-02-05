#ifndef DX4XB_H
#define DX4XB_H

#pragma region Dependencies

#include <Windows.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3d12shader.h>
#include <dxgi1_4.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include <atlbase.h>
#include <comdef.h>
#include <stdexcept>
#include <cmath>
#include <iostream>

#define MAX_FLOAT ((std::numeric_limits<float>().max)())
#define MIN_FLOAT ((std::numeric_limits<float>().min)())
#define uint unsigned int

#pragma region DX OBJECTS

typedef CComPtr<ID3D12DescriptorHeap> DX_DescriptorHeap;
typedef CComPtr<ID3D12Device5> DX_Device;
typedef CComPtr<IDXGISwapChain3> DX_SwapChain;

typedef CComPtr<ID3D12GraphicsCommandList4> DX_CommandList;
typedef CComPtr<ID3D12CommandAllocator> DX_CommandAllocator;
typedef CComPtr<ID3D12CommandQueue> DX_CommandQueue;
typedef CComPtr<ID3D12Fence1> DX_Fence;
typedef CComPtr<ID3D12Heap1> DX_Heap;
typedef CComPtr<ID3D12PipelineState> DX_PipelineState;
typedef CComPtr<ID3D12Resource1> DX_Resource;
typedef CComPtr<ID3D12RootSignature> DX_RootSignature;
typedef CComPtr<ID3D12RootSignatureDeserializer> DX_RootSignatureDeserializer;
typedef CComPtr<ID3D12StateObject> DX_StateObject;
typedef CComPtr<ID3D12StateObjectProperties> DX_StateObjectProperties;

#pragma endregion

#pragma endregion

namespace dx4xb {

#pragma region Type Definitions

	// Core definitions (Private types used during implementation
	struct wResource; // Wrapper to DX12 resources
	struct wResourceView; // Wrapper to DX12 resource views and cached descriptors
	struct wPipeline; // Wrapper to DX12 pipelines
	struct wProgram; // Wrapper to a DX12 Raytracing program settings
	struct wCmdList; // Wrapper to DX12 command lists
	struct wScheduler; // Wrapper to DX12 command queues manager, allocators, framing
	struct wDevice; // Wrapper to DX12 device, swapchains, descriptor heaps and scheduler
	
	// Public definitions

	struct PresenterDescription;
	class GPUProcess; // Defines types can populate command lists for further GPU execution
	class DeviceManager; // Defines types can create resources and dispatch GPU processes
	class Presenter; // Defines a Device manager that can control swap chains and presenting buffers
	class CommandListManager;


#pragma endregion

#pragma region Error Management

	// Represents different error code this engine can report.
	enum class Errors {
		// Some error occurred.
		Any,
		// The Pipeline State Object has a bad construction.
		BadPSOConstruction,
		// The Signature has a bad construction
		BadSignatureConstruction,
		// The image is in an unsupported format.
		UnsupportedFormat,
		// Some shader were not found
		ShaderNotFound,
		// Some initialization failed because memory was over
		RunOutOfMemory,
		// Invalid Operation
		Invalid_Operation,
		// Fallback raytracing device was not supported
		Unsupported_Fallback
	};

	class Exception : public std::exception {
	public:
		Exception(const char* const message) :std::exception(message) {}

		static Exception FromError(Errors error, const char* arg = nullptr, HRESULT hr = S_OK);
	};

#pragma endregion

#pragma region Math

	struct int1;
	struct int2;
	struct int3;
	struct int4;

	struct int1x1;
	struct int2x1;
	struct int3x1;
	struct int4x1;

	struct int1x2;
	struct int2x2;
	struct int3x2;
	struct int4x2;

	struct int1x3;
	struct int2x3;
	struct int3x3;
	struct int4x3;

	struct int1x4;
	struct int2x4;
	struct int3x4;
	struct int4x4;

	struct uint1;
	struct uint2;
	struct uint3;
	struct uint4;

	struct uint1x1;
	struct uint2x1;
	struct uint3x1;
	struct uint4x1;

	struct uint1x2;
	struct uint2x2;
	struct uint3x2;
	struct uint4x2;

	struct uint1x3;
	struct uint2x3;
	struct uint3x3;
	struct uint4x3;

	struct uint1x4;
	struct uint2x4;
	struct uint3x4;
	struct uint4x4;

	struct float1;
	struct float2;
	struct float3;
	struct float4;

	struct float1x1;
	struct float2x1;
	struct float3x1;
	struct float4x1;

	struct float1x2;
	struct float2x2;
	struct float3x2;
	struct float4x2;

	struct float1x3;
	struct float2x3;
	struct float3x3;
	struct float4x3;

	struct float1x4;
	struct float2x4;
	struct float3x4;
	struct float4x4;


	struct int1 {
	private:
		static int __TRASH;
	public:
		int x;
		int& operator[](int idx) {
			if (idx == 0) return this->x;
			return __TRASH; // Silent return ... valid for HLSL
		}
		int1(int x) {
			this->x = x;
		}
		operator float1() const;
		operator uint1() const;
	};
	static int1 operator -(const int1& a) { return int1(-a.x); }
	static int1 operator +(const int1& a) { return int1(+a.x); }
	static int1 operator ~(const int1& a) { return int1(~a.x); }
	static int1 operator !(const int1& a) { return int1(a.x == 0 ? 1 : 0); }
	static int1 operator ==(const int1& a, const int1& b) { return int1((a.x == b.x) ? 1 : 0); }
	static int1 operator !=(const int1& a, const int1& b) { return int1((a.x != b.x) ? 1 : 0); }
	static int1 operator <(const int1& a, const int1& b) { return int1((a.x < b.x) ? 1 : 0); }
	static int1 operator <=(const int1& a, const int1& b) { return int1((a.x <= b.x) ? 1 : 0); }
	static int1 operator >=(const int1& a, const int1& b) { return int1((a.x >= b.x) ? 1 : 0); }
	static int1 operator >(const int1& a, const int1& b) { return int1((a.x > b.x) ? 1 : 0); }
	static int1 operator +(const int1& a, const int1& b) { return int1(a.x + b.x); }
	static int1 operator *(const int1& a, const int1& b) { return int1(a.x * b.x); }
	static int1 operator -(const int1& a, const int1& b) { return int1(a.x - b.x); }
	static int1 operator /(const int1& a, const int1& b) { return int1(a.x / b.x); }
	static int1 operator %(const int1& a, const int1& b) { return int1(a.x % b.x); }
	static int1 operator &(const int1& a, const int1& b) { return int1(a.x & b.x); }
	static int1 operator |(const int1& a, const int1& b) { return int1(a.x | b.x); }
	static int1 operator ^(const int1& a, const int1& b) { return int1(a.x ^ b.x); }

	struct int2 {
	private:
		static int __TRASH;
	public:
		int x;
		int y;
		int& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			return __TRASH; // Silent return ... valid for HLSL
		}
		int2() : int2(0, 0) {}
		int2(int x, int y) {
			this->x = x;
			this->y = y;
		}
		int2(int v) :int2(v, v) {}
		operator int1() const { return int1(this->x); }
		operator float2() const;
		operator uint2() const;
	};
	static int2 operator -(const int2& a) { return int2(-a.x, -a.y); }
	static int2 operator +(const int2& a) { return int2(+a.x, +a.y); }
	static int2 operator ~(const int2& a) { return int2(~a.x, ~a.y); }
	static int2 operator !(const int2& a) { return int2(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0); }
	static int2 operator ==(const int2& a, const int2& b) { return int2((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0); }
	static int2 operator !=(const int2& a, const int2& b) { return int2((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0); }
	static int2 operator <(const int2& a, const int2& b) { return int2((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0); }
	static int2 operator <=(const int2& a, const int2& b) { return int2((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0); }
	static int2 operator >=(const int2& a, const int2& b) { return int2((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0); }
	static int2 operator >(const int2& a, const int2& b) { return int2((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0); }
	static int2 operator +(const int2& a, const int2& b) { return int2(a.x + b.x, a.y + b.y); }
	static int2 operator *(const int2& a, const int2& b) { return int2(a.x * b.x, a.y * b.y); }
	static int2 operator -(const int2& a, const int2& b) { return int2(a.x - b.x, a.y - b.y); }
	static int2 operator /(const int2& a, const int2& b) { return int2(a.x / b.x, a.y / b.y); }
	static int2 operator %(const int2& a, const int2& b) { return int2(a.x % b.x, a.y % b.y); }
	static int2 operator &(const int2& a, const int2& b) { return int2(a.x & b.x, a.y & b.y); }
	static int2 operator |(const int2& a, const int2& b) { return int2(a.x | b.x, a.y | b.y); }
	static int2 operator ^(const int2& a, const int2& b) { return int2(a.x ^ b.x, a.y ^ b.y); }

	struct int3 {
	private:
		static int __TRASH;
	public:
		int x;
		int y;
		int z;
		int2 get_xy() { return int2(this->x, this->y); }
		int& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			if (idx == 2) return this->z;
			return __TRASH; // Silent return ... valid for HLSL
		}
		int3(int x, int y, int z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
		int3(int v) :int3(v, v, v) {}
		operator int1() const { return int1(this->x); }
		operator int2() const { return int2(this->x, this->y); }
		operator float3() const;
		operator uint3() const;
	};
	static int3 operator -(const int3& a) { return int3(-a.x, -a.y, -a.z); }
	static int3 operator +(const int3& a) { return int3(+a.x, +a.y, +a.z); }
	static int3 operator ~(const int3& a) { return int3(~a.x, ~a.y, ~a.z); }
	static int3 operator !(const int3& a) { return int3(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0, a.z == 0 ? 1 : 0); }
	static int3 operator ==(const int3& a, const int3& b) { return int3((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0, (a.z == b.z) ? 1 : 0); }
	static int3 operator !=(const int3& a, const int3& b) { return int3((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0, (a.z != b.z) ? 1 : 0); }
	static int3 operator <(const int3& a, const int3& b) { return int3((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0, (a.z < b.z) ? 1 : 0); }
	static int3 operator <=(const int3& a, const int3& b) { return int3((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0, (a.z <= b.z) ? 1 : 0); }
	static int3 operator >=(const int3& a, const int3& b) { return int3((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0, (a.z >= b.z) ? 1 : 0); }
	static int3 operator >(const int3& a, const int3& b) { return int3((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0, (a.z > b.z) ? 1 : 0); }
	static int3 operator +(const int3& a, const int3& b) { return int3(a.x + b.x, a.y + b.y, a.z + b.z); }
	static int3 operator *(const int3& a, const int3& b) { return int3(a.x * b.x, a.y * b.y, a.z * b.z); }
	static int3 operator -(const int3& a, const int3& b) { return int3(a.x - b.x, a.y - b.y, a.z - b.z); }
	static int3 operator /(const int3& a, const int3& b) { return int3(a.x / b.x, a.y / b.y, a.z / b.z); }
	static int3 operator %(const int3& a, const int3& b) { return int3(a.x % b.x, a.y % b.y, a.z % b.z); }
	static int3 operator &(const int3& a, const int3& b) { return int3(a.x & b.x, a.y & b.y, a.z & b.z); }
	static int3 operator |(const int3& a, const int3& b) { return int3(a.x | b.x, a.y | b.y, a.z | b.z); }
	static int3 operator ^(const int3& a, const int3& b) { return int3(a.x ^ b.x, a.y ^ b.y, a.z ^ b.z); }

	struct int4 {
	private:
		static int __TRASH;
	public:
		int x;
		int y;
		int z;
		int w;
		int2 get_xy() { return int2(this->x, this->y); }
		int3 get_xyz() { return int3(this->x, this->y, this->z); }
		int& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			if (idx == 2) return this->z;
			if (idx == 3) return this->w;
			return __TRASH; // Silent return ... valid for HLSL
		}
		int4(int x, int y, int z, int w) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
		int4(int v) :int4(v, v, v, v) {}
		operator int1() const { return int1(this->x); }
		operator int2() const { return int2(this->x, this->y); }
		operator int3() const { return int3(this->x, this->y, this->z); }
		operator float4() const;
		operator uint4() const;
	};
	static int4 operator -(const int4& a) { return int4(-a.x, -a.y, -a.z, -a.w); }
	static int4 operator +(const int4& a) { return int4(+a.x, +a.y, +a.z, +a.w); }
	static int4 operator ~(const int4& a) { return int4(~a.x, ~a.y, ~a.z, ~a.w); }
	static int4 operator !(const int4& a) { return int4(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0, a.z == 0 ? 1 : 0, a.w == 0 ? 1 : 0); }
	static int4 operator ==(const int4& a, const int4& b) { return int4((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0, (a.z == b.z) ? 1 : 0, (a.w == b.w) ? 1 : 0); }
	static int4 operator !=(const int4& a, const int4& b) { return int4((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0, (a.z != b.z) ? 1 : 0, (a.w != b.w) ? 1 : 0); }
	static int4 operator <(const int4& a, const int4& b) { return int4((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0, (a.z < b.z) ? 1 : 0, (a.w < b.w) ? 1 : 0); }
	static int4 operator <=(const int4& a, const int4& b) { return int4((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0, (a.z <= b.z) ? 1 : 0, (a.w <= b.w) ? 1 : 0); }
	static int4 operator >=(const int4& a, const int4& b) { return int4((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0, (a.z >= b.z) ? 1 : 0, (a.w >= b.w) ? 1 : 0); }
	static int4 operator >(const int4& a, const int4& b) { return int4((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0, (a.z > b.z) ? 1 : 0, (a.w > b.w) ? 1 : 0); }
	static int4 operator +(const int4& a, const int4& b) { return int4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	static int4 operator *(const int4& a, const int4& b) { return int4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
	static int4 operator -(const int4& a, const int4& b) { return int4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
	static int4 operator /(const int4& a, const int4& b) { return int4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
	static int4 operator %(const int4& a, const int4& b) { return int4(a.x % b.x, a.y % b.y, a.z % b.z, a.w % b.w); }
	static int4 operator &(const int4& a, const int4& b) { return int4(a.x & b.x, a.y & b.y, a.z & b.z, a.w & b.w); }
	static int4 operator |(const int4& a, const int4& b) { return int4(a.x | b.x, a.y | b.y, a.z | b.z, a.w | b.w); }
	static int4 operator ^(const int4& a, const int4& b) { return int4(a.x ^ b.x, a.y ^ b.y, a.z ^ b.z, a.w ^ b.w); }

	struct int1x1 {
		int _m00;
		int1 operator[] (int row) const {
			if (row == 0) return int1(_m00);
			return 0; // Silent return ... valid for HLSL
		}
		int1x1(int _m00) {
			this->_m00 = _m00;
		}
		operator int1() const { return int1(this->_m00); }
		int1x1(const int1& v) : int1x1(v.x) { }
		operator float1x1() const;
		operator uint1x1() const;
	};
	static int1x1 operator -(const int1x1& a) { return int1x1(-a._m00); }
	static int1x1 operator +(const int1x1& a) { return int1x1(+a._m00); }
	static int1x1 operator ~(const int1x1& a) { return int1x1(~a._m00); }
	static int1x1 operator !(const int1x1& a) { return int1x1(a._m00 == 0 ? 1 : 0); }
	static int1x1 operator +(const int1x1& a, const int1x1& b) { return int1x1(a._m00 + b._m00); }
	static int1x1 operator *(const int1x1& a, const int1x1& b) { return int1x1(a._m00 * b._m00); }
	static int1x1 operator -(const int1x1& a, const int1x1& b) { return int1x1(a._m00 - b._m00); }
	static int1x1 operator /(const int1x1& a, const int1x1& b) { return int1x1(a._m00 / b._m00); }
	static int1x1 operator %(const int1x1& a, const int1x1& b) { return int1x1(a._m00 % b._m00); }
	static int1x1 operator &(const int1x1& a, const int1x1& b) { return int1x1(a._m00 & b._m00); }
	static int1x1 operator |(const int1x1& a, const int1x1& b) { return int1x1(a._m00 | b._m00); }
	static int1x1 operator ^(const int1x1& a, const int1x1& b) { return int1x1(a._m00 ^ b._m00); }
	static int1x1 operator ==(const int1x1& a, const int1x1& b) { return int1x1((a._m00 == b._m00) ? 1 : 0); }
	static int1x1 operator !=(const int1x1& a, const int1x1& b) { return int1x1((a._m00 != b._m00) ? 1 : 0); }
	static int1x1 operator <(const int1x1& a, const int1x1& b) { return int1x1((a._m00 < b._m00) ? 1 : 0); }
	static int1x1 operator <=(const int1x1& a, const int1x1& b) { return int1x1((a._m00 <= b._m00) ? 1 : 0); }
	static int1x1 operator >=(const int1x1& a, const int1x1& b) { return int1x1((a._m00 >= b._m00) ? 1 : 0); }
	static int1x1 operator >(const int1x1& a, const int1x1& b) { return int1x1((a._m00 > b._m00) ? 1 : 0); }

	struct int1x2 {
		int _m00;
		int _m01;
		int2 operator[] (int row) const {
			if (row == 0) return int2(_m00, _m01);
			return 0; // Silent return ... valid for HLSL
		}
		int1x2(int _m00, int _m01) {
			this->_m00 = _m00;
			this->_m01 = _m01;
		}
		int1x2(int v) :int1x2(v, v) {}
		operator int2() const { return int2(this->_m00, this->_m01); }
		int1x2(const int2& v) : int1x2(v.x, v.y) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		operator float1x2() const;
		operator uint1x2() const;
	};
	static int1x2 operator -(const int1x2& a) { return int1x2(-a._m00, -a._m01); }
	static int1x2 operator +(const int1x2& a) { return int1x2(+a._m00, +a._m01); }
	static int1x2 operator ~(const int1x2& a) { return int1x2(~a._m00, ~a._m01); }
	static int1x2 operator !(const int1x2& a) { return int1x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0); }
	static int1x2 operator +(const int1x2& a, const int1x2& b) { return int1x2(a._m00 + b._m00, a._m01 + b._m01); }
	static int1x2 operator *(const int1x2& a, const int1x2& b) { return int1x2(a._m00 * b._m00, a._m01 * b._m01); }
	static int1x2 operator -(const int1x2& a, const int1x2& b) { return int1x2(a._m00 - b._m00, a._m01 - b._m01); }
	static int1x2 operator /(const int1x2& a, const int1x2& b) { return int1x2(a._m00 / b._m00, a._m01 / b._m01); }
	static int1x2 operator %(const int1x2& a, const int1x2& b) { return int1x2(a._m00 % b._m00, a._m01 % b._m01); }
	static int1x2 operator &(const int1x2& a, const int1x2& b) { return int1x2(a._m00 & b._m00, a._m01 & b._m01); }
	static int1x2 operator |(const int1x2& a, const int1x2& b) { return int1x2(a._m00 | b._m00, a._m01 | b._m01); }
	static int1x2 operator ^(const int1x2& a, const int1x2& b) { return int1x2(a._m00 ^ b._m00, a._m01 ^ b._m01); }
	static int1x2 operator ==(const int1x2& a, const int1x2& b) { return int1x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0); }
	static int1x2 operator !=(const int1x2& a, const int1x2& b) { return int1x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0); }
	static int1x2 operator <(const int1x2& a, const int1x2& b) { return int1x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0); }
	static int1x2 operator <=(const int1x2& a, const int1x2& b) { return int1x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0); }
	static int1x2 operator >=(const int1x2& a, const int1x2& b) { return int1x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0); }
	static int1x2 operator >(const int1x2& a, const int1x2& b) { return int1x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0); }

	struct int1x3 {
		int _m00;
		int _m01;
		int _m02;
		int3 operator[] (int row) const {
			if (row == 0) return int3(_m00, _m01, _m02);
			return 0; // Silent return ... valid for HLSL
		}
		int1x3(int _m00, int _m01, int _m02) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
		}
		int1x3(int v) :int1x3(v, v, v) {}
		operator int3() const { return int3(this->_m00, this->_m01, this->_m02); }
		int1x3(const int3& v) : int1x3(v.x, v.y, v.z) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		operator float1x3() const;
		operator uint1x3() const;
	};
	static int1x3 operator -(const int1x3& a) { return int1x3(-a._m00, -a._m01, -a._m02); }
	static int1x3 operator +(const int1x3& a) { return int1x3(+a._m00, +a._m01, +a._m02); }
	static int1x3 operator ~(const int1x3& a) { return int1x3(~a._m00, ~a._m01, ~a._m02); }
	static int1x3 operator !(const int1x3& a) { return int1x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0); }
	static int1x3 operator +(const int1x3& a, const int1x3& b) { return int1x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02); }
	static int1x3 operator *(const int1x3& a, const int1x3& b) { return int1x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02); }
	static int1x3 operator -(const int1x3& a, const int1x3& b) { return int1x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02); }
	static int1x3 operator /(const int1x3& a, const int1x3& b) { return int1x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02); }
	static int1x3 operator %(const int1x3& a, const int1x3& b) { return int1x3(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02); }
	static int1x3 operator &(const int1x3& a, const int1x3& b) { return int1x3(a._m00 & b._m00, a._m01 & b._m01, a._m02 & b._m02); }
	static int1x3 operator |(const int1x3& a, const int1x3& b) { return int1x3(a._m00 | b._m00, a._m01 | b._m01, a._m02 | b._m02); }
	static int1x3 operator ^(const int1x3& a, const int1x3& b) { return int1x3(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m02 ^ b._m02); }
	static int1x3 operator ==(const int1x3& a, const int1x3& b) { return int1x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0); }
	static int1x3 operator !=(const int1x3& a, const int1x3& b) { return int1x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0); }
	static int1x3 operator <(const int1x3& a, const int1x3& b) { return int1x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0); }
	static int1x3 operator <=(const int1x3& a, const int1x3& b) { return int1x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0); }
	static int1x3 operator >=(const int1x3& a, const int1x3& b) { return int1x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0); }
	static int1x3 operator >(const int1x3& a, const int1x3& b) { return int1x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0); }

	struct int1x4 {
		int _m00;
		int _m01;
		int _m02;
		int _m03;
		int4 operator[] (int row) const {
			if (row == 0) return int4(_m00, _m01, _m02, _m03);
			return 0; // Silent return ... valid for HLSL
		}
		int1x4(int _m00, int _m01, int _m02, int _m03) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
		}
		int1x4(int v) :int1x4(v, v, v, v) {}
		operator int4() const { return int4(this->_m00, this->_m01, this->_m02, this->_m03); }
		int1x4(const int4& v) : int1x4(v.x, v.y, v.z, v.w) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int1x3() const { return int1x3(this->_m00, this->_m01, this->_m02); }
		operator float1x4() const;
		operator uint1x4() const;
	};
	static int1x4 operator -(const int1x4& a) { return int1x4(-a._m00, -a._m01, -a._m02, -a._m03); }
	static int1x4 operator +(const int1x4& a) { return int1x4(+a._m00, +a._m01, +a._m02, +a._m03); }
	static int1x4 operator ~(const int1x4& a) { return int1x4(~a._m00, ~a._m01, ~a._m02, ~a._m03); }
	static int1x4 operator !(const int1x4& a) { return int1x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0); }
	static int1x4 operator +(const int1x4& a, const int1x4& b) { return int1x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03); }
	static int1x4 operator *(const int1x4& a, const int1x4& b) { return int1x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03); }
	static int1x4 operator -(const int1x4& a, const int1x4& b) { return int1x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03); }
	static int1x4 operator /(const int1x4& a, const int1x4& b) { return int1x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03); }
	static int1x4 operator %(const int1x4& a, const int1x4& b) { return int1x4(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m03 % b._m03); }
	static int1x4 operator &(const int1x4& a, const int1x4& b) { return int1x4(a._m00 & b._m00, a._m01 & b._m01, a._m02 & b._m02, a._m03 & b._m03); }
	static int1x4 operator |(const int1x4& a, const int1x4& b) { return int1x4(a._m00 | b._m00, a._m01 | b._m01, a._m02 | b._m02, a._m03 | b._m03); }
	static int1x4 operator ^(const int1x4& a, const int1x4& b) { return int1x4(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m02 ^ b._m02, a._m03 ^ b._m03); }
	static int1x4 operator ==(const int1x4& a, const int1x4& b) { return int1x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0); }
	static int1x4 operator !=(const int1x4& a, const int1x4& b) { return int1x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0); }
	static int1x4 operator <(const int1x4& a, const int1x4& b) { return int1x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0); }
	static int1x4 operator <=(const int1x4& a, const int1x4& b) { return int1x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0); }
	static int1x4 operator >=(const int1x4& a, const int1x4& b) { return int1x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0); }
	static int1x4 operator >(const int1x4& a, const int1x4& b) { return int1x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0); }

	struct int2x1 {
		int _m00;
		int _m10;
		int1 operator[] (int row) const {
			if (row == 0) return int1(_m00);
			if (row == 1) return int1(_m10);
			return 0; // Silent return ... valid for HLSL
		}
		int2x1(int _m00, int _m10) {
			this->_m00 = _m00;
			this->_m10 = _m10;
		}
		int2x1(int v) :int2x1(v, v) {}
		int2x1(const int1& r0, const int1& r1) : int2x1(r0.x, r1.x) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		operator float2x1() const;
		operator uint2x1() const;
	};
	static int2x1 operator -(const int2x1& a) { return int2x1(-a._m00, -a._m10); }
	static int2x1 operator +(const int2x1& a) { return int2x1(+a._m00, +a._m10); }
	static int2x1 operator ~(const int2x1& a) { return int2x1(~a._m00, ~a._m10); }
	static int2x1 operator !(const int2x1& a) { return int2x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0); }
	static int2x1 operator +(const int2x1& a, const int2x1& b) { return int2x1(a._m00 + b._m00, a._m10 + b._m10); }
	static int2x1 operator *(const int2x1& a, const int2x1& b) { return int2x1(a._m00 * b._m00, a._m10 * b._m10); }
	static int2x1 operator -(const int2x1& a, const int2x1& b) { return int2x1(a._m00 - b._m00, a._m10 - b._m10); }
	static int2x1 operator /(const int2x1& a, const int2x1& b) { return int2x1(a._m00 / b._m00, a._m10 / b._m10); }
	static int2x1 operator %(const int2x1& a, const int2x1& b) { return int2x1(a._m00 % b._m00, a._m10 % b._m10); }
	static int2x1 operator &(const int2x1& a, const int2x1& b) { return int2x1(a._m00 & b._m00, a._m10 & b._m10); }
	static int2x1 operator |(const int2x1& a, const int2x1& b) { return int2x1(a._m00 | b._m00, a._m10 | b._m10); }
	static int2x1 operator ^(const int2x1& a, const int2x1& b) { return int2x1(a._m00 ^ b._m00, a._m10 ^ b._m10); }
	static int2x1 operator ==(const int2x1& a, const int2x1& b) { return int2x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0); }
	static int2x1 operator !=(const int2x1& a, const int2x1& b) { return int2x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0); }
	static int2x1 operator <(const int2x1& a, const int2x1& b) { return int2x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0); }
	static int2x1 operator <=(const int2x1& a, const int2x1& b) { return int2x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0); }
	static int2x1 operator >=(const int2x1& a, const int2x1& b) { return int2x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0); }
	static int2x1 operator >(const int2x1& a, const int2x1& b) { return int2x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0); }

	struct int2x2 {
		int _m00;
		int _m10;
		int _m01;
		int _m11;
		int2 operator[] (int row) const {
			if (row == 0) return int2(_m00, _m01);
			if (row == 1) return int2(_m10, _m11);
			return 0; // Silent return ... valid for HLSL
		}
		int2x2(int _m00, int _m01, int _m10, int _m11) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
		}
		int2x2(int v) :int2x2(v, v, v, v) {}
		int2x2(const int2& r0, const int2& r1) : int2x2(r0.x, r0.y, r1.x, r1.y) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		operator float2x2() const;
		operator uint2x2() const;
	};
	static int2x2 operator -(const int2x2& a) { return int2x2(-a._m00, -a._m01, -a._m10, -a._m11); }
	static int2x2 operator +(const int2x2& a) { return int2x2(+a._m00, +a._m01, +a._m10, +a._m11); }
	static int2x2 operator ~(const int2x2& a) { return int2x2(~a._m00, ~a._m01, ~a._m10, ~a._m11); }
	static int2x2 operator !(const int2x2& a) { return int2x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0); }
	static int2x2 operator +(const int2x2& a, const int2x2& b) { return int2x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11); }
	static int2x2 operator *(const int2x2& a, const int2x2& b) { return int2x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11); }
	static int2x2 operator -(const int2x2& a, const int2x2& b) { return int2x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11); }
	static int2x2 operator /(const int2x2& a, const int2x2& b) { return int2x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11); }
	static int2x2 operator %(const int2x2& a, const int2x2& b) { return int2x2(a._m00 % b._m00, a._m01 % b._m01, a._m10 % b._m10, a._m11 % b._m11); }
	static int2x2 operator &(const int2x2& a, const int2x2& b) { return int2x2(a._m00 & b._m00, a._m01 & b._m01, a._m10 & b._m10, a._m11 & b._m11); }
	static int2x2 operator |(const int2x2& a, const int2x2& b) { return int2x2(a._m00 | b._m00, a._m01 | b._m01, a._m10 | b._m10, a._m11 | b._m11); }
	static int2x2 operator ^(const int2x2& a, const int2x2& b) { return int2x2(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m10 ^ b._m10, a._m11 ^ b._m11); }
	static int2x2 operator ==(const int2x2& a, const int2x2& b) { return int2x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0); }
	static int2x2 operator !=(const int2x2& a, const int2x2& b) { return int2x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0); }
	static int2x2 operator <(const int2x2& a, const int2x2& b) { return int2x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0); }
	static int2x2 operator <=(const int2x2& a, const int2x2& b) { return int2x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0); }
	static int2x2 operator >=(const int2x2& a, const int2x2& b) { return int2x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0); }
	static int2x2 operator >(const int2x2& a, const int2x2& b) { return int2x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0); }

	struct int2x3 {
		int _m00;
		int _m10;
		int _m01;
		int _m11;
		int _m02;
		int _m12;
		int3 operator[] (int row) const {
			if (row == 0) return int3(_m00, _m01, _m02);
			if (row == 1) return int3(_m10, _m11, _m12);
			return 0; // Silent return ... valid for HLSL
		}
		int2x3(int _m00, int _m01, int _m02, int _m10, int _m11, int _m12) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
		}
		int2x3(int v) :int2x3(v, v, v, v, v, v) {}
		int2x3(const int3& r0, const int3& r1) : int2x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int1x3() const { return int1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int2x2() const { return int2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		operator float2x3() const;
		operator uint2x3() const;
	};
	static int2x3 operator -(const int2x3& a) { return int2x3(-a._m00, -a._m01, -a._m02, -a._m10, -a._m11, -a._m12); }
	static int2x3 operator +(const int2x3& a) { return int2x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12); }
	static int2x3 operator ~(const int2x3& a) { return int2x3(~a._m00, ~a._m01, ~a._m02, ~a._m10, ~a._m11, ~a._m12); }
	static int2x3 operator !(const int2x3& a) { return int2x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0); }
	static int2x3 operator +(const int2x3& a, const int2x3& b) { return int2x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12); }
	static int2x3 operator *(const int2x3& a, const int2x3& b) { return int2x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12); }
	static int2x3 operator -(const int2x3& a, const int2x3& b) { return int2x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12); }
	static int2x3 operator /(const int2x3& a, const int2x3& b) { return int2x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12); }
	static int2x3 operator %(const int2x3& a, const int2x3& b) { return int2x3(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12); }
	static int2x3 operator &(const int2x3& a, const int2x3& b) { return int2x3(a._m00 & b._m00, a._m01 & b._m01, a._m02 & b._m02, a._m10 & b._m10, a._m11 & b._m11, a._m12 & b._m12); }
	static int2x3 operator |(const int2x3& a, const int2x3& b) { return int2x3(a._m00 | b._m00, a._m01 | b._m01, a._m02 | b._m02, a._m10 | b._m10, a._m11 | b._m11, a._m12 | b._m12); }
	static int2x3 operator ^(const int2x3& a, const int2x3& b) { return int2x3(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m02 ^ b._m02, a._m10 ^ b._m10, a._m11 ^ b._m11, a._m12 ^ b._m12); }
	static int2x3 operator ==(const int2x3& a, const int2x3& b) { return int2x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0); }
	static int2x3 operator !=(const int2x3& a, const int2x3& b) { return int2x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0); }
	static int2x3 operator <(const int2x3& a, const int2x3& b) { return int2x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0); }
	static int2x3 operator <=(const int2x3& a, const int2x3& b) { return int2x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0); }
	static int2x3 operator >=(const int2x3& a, const int2x3& b) { return int2x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0); }
	static int2x3 operator >(const int2x3& a, const int2x3& b) { return int2x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0); }

	struct int2x4 {
		int _m00;
		int _m10;
		int _m01;
		int _m11;
		int _m02;
		int _m12;
		int _m03;
		int _m13;
		int4 operator[] (int row) const {
			if (row == 0) return int4(_m00, _m01, _m02, _m03);
			if (row == 1) return int4(_m10, _m11, _m12, _m13);
			return 0; // Silent return ... valid for HLSL
		}
		int2x4(int _m00, int _m01, int _m02, int _m03, int _m10, int _m11, int _m12, int _m13) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
		}
		int2x4(int v) :int2x4(v, v, v, v, v, v, v, v) {}
		int2x4(const int4& r0, const int4& r1) : int2x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int1x3() const { return int1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator int1x4() const { return int1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int2x2() const { return int2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator int2x3() const { return int2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		operator float2x4() const;
		operator uint2x4() const;
	};
	static int2x4 operator -(const int2x4& a) { return int2x4(-a._m00, -a._m01, -a._m02, -a._m03, -a._m10, -a._m11, -a._m12, -a._m13); }
	static int2x4 operator +(const int2x4& a) { return int2x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13); }
	static int2x4 operator ~(const int2x4& a) { return int2x4(~a._m00, ~a._m01, ~a._m02, ~a._m03, ~a._m10, ~a._m11, ~a._m12, ~a._m13); }
	static int2x4 operator !(const int2x4& a) { return int2x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0); }
	static int2x4 operator +(const int2x4& a, const int2x4& b) { return int2x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13); }
	static int2x4 operator *(const int2x4& a, const int2x4& b) { return int2x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13); }
	static int2x4 operator -(const int2x4& a, const int2x4& b) { return int2x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13); }
	static int2x4 operator /(const int2x4& a, const int2x4& b) { return int2x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13); }
	static int2x4 operator %(const int2x4& a, const int2x4& b) { return int2x4(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m03 % b._m03, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m13 % b._m13); }
	static int2x4 operator &(const int2x4& a, const int2x4& b) { return int2x4(a._m00 & b._m00, a._m01 & b._m01, a._m02 & b._m02, a._m03 & b._m03, a._m10 & b._m10, a._m11 & b._m11, a._m12 & b._m12, a._m13 & b._m13); }
	static int2x4 operator |(const int2x4& a, const int2x4& b) { return int2x4(a._m00 | b._m00, a._m01 | b._m01, a._m02 | b._m02, a._m03 | b._m03, a._m10 | b._m10, a._m11 | b._m11, a._m12 | b._m12, a._m13 | b._m13); }
	static int2x4 operator ^(const int2x4& a, const int2x4& b) { return int2x4(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m02 ^ b._m02, a._m03 ^ b._m03, a._m10 ^ b._m10, a._m11 ^ b._m11, a._m12 ^ b._m12, a._m13 ^ b._m13); }
	static int2x4 operator ==(const int2x4& a, const int2x4& b) { return int2x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0); }
	static int2x4 operator !=(const int2x4& a, const int2x4& b) { return int2x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0); }
	static int2x4 operator <(const int2x4& a, const int2x4& b) { return int2x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0); }
	static int2x4 operator <=(const int2x4& a, const int2x4& b) { return int2x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0); }
	static int2x4 operator >=(const int2x4& a, const int2x4& b) { return int2x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0); }
	static int2x4 operator >(const int2x4& a, const int2x4& b) { return int2x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0); }

	struct int3x1 {
		int _m00;
		int _m10;
		int _m20;
		int1 operator[] (int row) const {
			if (row == 0) return int1(_m00);
			if (row == 1) return int1(_m10);
			if (row == 2) return int1(_m20);
			return 0; // Silent return ... valid for HLSL
		}
		int3x1(int _m00, int _m10, int _m20) {
			this->_m00 = _m00;
			this->_m10 = _m10;
			this->_m20 = _m20;
		}
		int3x1(int v) :int3x1(v, v, v) {}
		int3x1(const int1& r0, const int1& r1, const int1& r2) : int3x1(r0.x, r1.x, r2.x) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		operator float3x1() const;
		operator uint3x1() const;
	};
	static int3x1 operator -(const int3x1& a) { return int3x1(-a._m00, -a._m10, -a._m20); }
	static int3x1 operator +(const int3x1& a) { return int3x1(+a._m00, +a._m10, +a._m20); }
	static int3x1 operator ~(const int3x1& a) { return int3x1(~a._m00, ~a._m10, ~a._m20); }
	static int3x1 operator !(const int3x1& a) { return int3x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0); }
	static int3x1 operator +(const int3x1& a, const int3x1& b) { return int3x1(a._m00 + b._m00, a._m10 + b._m10, a._m20 + b._m20); }
	static int3x1 operator *(const int3x1& a, const int3x1& b) { return int3x1(a._m00 * b._m00, a._m10 * b._m10, a._m20 * b._m20); }
	static int3x1 operator -(const int3x1& a, const int3x1& b) { return int3x1(a._m00 - b._m00, a._m10 - b._m10, a._m20 - b._m20); }
	static int3x1 operator /(const int3x1& a, const int3x1& b) { return int3x1(a._m00 / b._m00, a._m10 / b._m10, a._m20 / b._m20); }
	static int3x1 operator %(const int3x1& a, const int3x1& b) { return int3x1(a._m00 % b._m00, a._m10 % b._m10, a._m20 % b._m20); }
	static int3x1 operator &(const int3x1& a, const int3x1& b) { return int3x1(a._m00 & b._m00, a._m10 & b._m10, a._m20 & b._m20); }
	static int3x1 operator |(const int3x1& a, const int3x1& b) { return int3x1(a._m00 | b._m00, a._m10 | b._m10, a._m20 | b._m20); }
	static int3x1 operator ^(const int3x1& a, const int3x1& b) { return int3x1(a._m00 ^ b._m00, a._m10 ^ b._m10, a._m20 ^ b._m20); }
	static int3x1 operator ==(const int3x1& a, const int3x1& b) { return int3x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0); }
	static int3x1 operator !=(const int3x1& a, const int3x1& b) { return int3x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0); }
	static int3x1 operator <(const int3x1& a, const int3x1& b) { return int3x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0); }
	static int3x1 operator <=(const int3x1& a, const int3x1& b) { return int3x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0); }
	static int3x1 operator >=(const int3x1& a, const int3x1& b) { return int3x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0); }
	static int3x1 operator >(const int3x1& a, const int3x1& b) { return int3x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0); }

	struct int3x2 {
		int _m00;
		int _m10;
		int _m20;
		int _m01;
		int _m11;
		int _m21;
		int2 operator[] (int row) const {
			if (row == 0) return int2(_m00, _m01);
			if (row == 1) return int2(_m10, _m11);
			if (row == 2) return int2(_m20, _m21);
			return 0; // Silent return ... valid for HLSL
		}
		int3x2(int _m00, int _m01, int _m10, int _m11, int _m20, int _m21) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m20 = _m20;
			this->_m21 = _m21;
		}
		int3x2(int v) :int3x2(v, v, v, v, v, v) {}
		int3x2(const int2& r0, const int2& r1, const int2& r2) : int3x2(r0.x, r0.y, r1.x, r1.y, r2.x, r2.y) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int2x2() const { return int2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator int3x1() const { return int3x1(this->_m00, this->_m10, this->_m20); }
		operator float3x2() const;
		operator uint3x2() const;
	};
	static int3x2 operator -(const int3x2& a) { return int3x2(-a._m00, -a._m01, -a._m10, -a._m11, -a._m20, -a._m21); }
	static int3x2 operator +(const int3x2& a) { return int3x2(+a._m00, +a._m01, +a._m10, +a._m11, +a._m20, +a._m21); }
	static int3x2 operator ~(const int3x2& a) { return int3x2(~a._m00, ~a._m01, ~a._m10, ~a._m11, ~a._m20, ~a._m21); }
	static int3x2 operator !(const int3x2& a) { return int3x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0); }
	static int3x2 operator +(const int3x2& a, const int3x2& b) { return int3x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11, a._m20 + b._m20, a._m21 + b._m21); }
	static int3x2 operator *(const int3x2& a, const int3x2& b) { return int3x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11, a._m20 * b._m20, a._m21 * b._m21); }
	static int3x2 operator -(const int3x2& a, const int3x2& b) { return int3x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11, a._m20 - b._m20, a._m21 - b._m21); }
	static int3x2 operator /(const int3x2& a, const int3x2& b) { return int3x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11, a._m20 / b._m20, a._m21 / b._m21); }
	static int3x2 operator %(const int3x2& a, const int3x2& b) { return int3x2(a._m00 % b._m00, a._m01 % b._m01, a._m10 % b._m10, a._m11 % b._m11, a._m20 % b._m20, a._m21 % b._m21); }
	static int3x2 operator &(const int3x2& a, const int3x2& b) { return int3x2(a._m00 & b._m00, a._m01 & b._m01, a._m10 & b._m10, a._m11 & b._m11, a._m20 & b._m20, a._m21 & b._m21); }
	static int3x2 operator |(const int3x2& a, const int3x2& b) { return int3x2(a._m00 | b._m00, a._m01 | b._m01, a._m10 | b._m10, a._m11 | b._m11, a._m20 | b._m20, a._m21 | b._m21); }
	static int3x2 operator ^(const int3x2& a, const int3x2& b) { return int3x2(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m10 ^ b._m10, a._m11 ^ b._m11, a._m20 ^ b._m20, a._m21 ^ b._m21); }
	static int3x2 operator ==(const int3x2& a, const int3x2& b) { return int3x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0); }
	static int3x2 operator !=(const int3x2& a, const int3x2& b) { return int3x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0); }
	static int3x2 operator <(const int3x2& a, const int3x2& b) { return int3x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0); }
	static int3x2 operator <=(const int3x2& a, const int3x2& b) { return int3x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0); }
	static int3x2 operator >=(const int3x2& a, const int3x2& b) { return int3x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0); }
	static int3x2 operator >(const int3x2& a, const int3x2& b) { return int3x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0); }

	struct int3x3 {
		int _m00;
		int _m10;
		int _m20;
		int _m01;
		int _m11;
		int _m21;
		int _m02;
		int _m12;
		int _m22;
		int3 operator[] (int row) const {
			if (row == 0) return int3(_m00, _m01, _m02);
			if (row == 1) return int3(_m10, _m11, _m12);
			if (row == 2) return int3(_m20, _m21, _m22);
			return 0; // Silent return ... valid for HLSL
		}
		int3x3(int _m00, int _m01, int _m02, int _m10, int _m11, int _m12, int _m20, int _m21, int _m22) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
		}
		int3x3(int v) :int3x3(v, v, v, v, v, v, v, v, v) {}
		int3x3(const int3& r0, const int3& r1, const int3& r2) : int3x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z, r2.x, r2.y, r2.z) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int1x3() const { return int1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int2x2() const { return int2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator int2x3() const { return int2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator int3x1() const { return int3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator int3x2() const { return int3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		operator float3x3() const;
		operator uint3x3() const;
	};
	static int3x3 operator -(const int3x3& a) { return int3x3(-a._m00, -a._m01, -a._m02, -a._m10, -a._m11, -a._m12, -a._m20, -a._m21, -a._m22); }
	static int3x3 operator +(const int3x3& a) { return int3x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12, +a._m20, +a._m21, +a._m22); }
	static int3x3 operator ~(const int3x3& a) { return int3x3(~a._m00, ~a._m01, ~a._m02, ~a._m10, ~a._m11, ~a._m12, ~a._m20, ~a._m21, ~a._m22); }
	static int3x3 operator !(const int3x3& a) { return int3x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0); }
	static int3x3 operator +(const int3x3& a, const int3x3& b) { return int3x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22); }
	static int3x3 operator *(const int3x3& a, const int3x3& b) { return int3x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22); }
	static int3x3 operator -(const int3x3& a, const int3x3& b) { return int3x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22); }
	static int3x3 operator /(const int3x3& a, const int3x3& b) { return int3x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22); }
	static int3x3 operator %(const int3x3& a, const int3x3& b) { return int3x3(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m20 % b._m20, a._m21 % b._m21, a._m22 % b._m22); }
	static int3x3 operator &(const int3x3& a, const int3x3& b) { return int3x3(a._m00 & b._m00, a._m01 & b._m01, a._m02 & b._m02, a._m10 & b._m10, a._m11 & b._m11, a._m12 & b._m12, a._m20 & b._m20, a._m21 & b._m21, a._m22 & b._m22); }
	static int3x3 operator |(const int3x3& a, const int3x3& b) { return int3x3(a._m00 | b._m00, a._m01 | b._m01, a._m02 | b._m02, a._m10 | b._m10, a._m11 | b._m11, a._m12 | b._m12, a._m20 | b._m20, a._m21 | b._m21, a._m22 | b._m22); }
	static int3x3 operator ^(const int3x3& a, const int3x3& b) { return int3x3(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m02 ^ b._m02, a._m10 ^ b._m10, a._m11 ^ b._m11, a._m12 ^ b._m12, a._m20 ^ b._m20, a._m21 ^ b._m21, a._m22 ^ b._m22); }
	static int3x3 operator ==(const int3x3& a, const int3x3& b) { return int3x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0); }
	static int3x3 operator !=(const int3x3& a, const int3x3& b) { return int3x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0); }
	static int3x3 operator <(const int3x3& a, const int3x3& b) { return int3x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0); }
	static int3x3 operator <=(const int3x3& a, const int3x3& b) { return int3x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0); }
	static int3x3 operator >=(const int3x3& a, const int3x3& b) { return int3x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0); }
	static int3x3 operator >(const int3x3& a, const int3x3& b) { return int3x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0); }

	struct int3x4 {
		int _m00;
		int _m10;
		int _m20;
		int _m01;
		int _m11;
		int _m21;
		int _m02;
		int _m12;
		int _m22;
		int _m03;
		int _m13;
		int _m23;
		int4 operator[] (int row) const {
			if (row == 0) return int4(_m00, _m01, _m02, _m03);
			if (row == 1) return int4(_m10, _m11, _m12, _m13);
			if (row == 2) return int4(_m20, _m21, _m22, _m23);
			return 0; // Silent return ... valid for HLSL
		}
		int3x4(int _m00, int _m01, int _m02, int _m03, int _m10, int _m11, int _m12, int _m13, int _m20, int _m21, int _m22, int _m23) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m23 = _m23;
		}
		int3x4(int v) :int3x4(v, v, v, v, v, v, v, v, v, v, v, v) {}
		int3x4(const int4& r0, const int4& r1, const int4& r2) : int3x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int1x3() const { return int1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator int1x4() const { return int1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int2x2() const { return int2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator int2x3() const { return int2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator int2x4() const { return int2x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13); }
		explicit operator int3x1() const { return int3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator int3x2() const { return int3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator int3x3() const { return int3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		operator float3x4() const;
		operator uint3x4() const;
	};
	static int3x4 operator -(const int3x4& a) { return int3x4(-a._m00, -a._m01, -a._m02, -a._m03, -a._m10, -a._m11, -a._m12, -a._m13, -a._m20, -a._m21, -a._m22, -a._m23); }
	static int3x4 operator +(const int3x4& a) { return int3x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13, +a._m20, +a._m21, +a._m22, +a._m23); }
	static int3x4 operator ~(const int3x4& a) { return int3x4(~a._m00, ~a._m01, ~a._m02, ~a._m03, ~a._m10, ~a._m11, ~a._m12, ~a._m13, ~a._m20, ~a._m21, ~a._m22, ~a._m23); }
	static int3x4 operator !(const int3x4& a) { return int3x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m23 == 0 ? 1 : 0); }
	static int3x4 operator +(const int3x4& a, const int3x4& b) { return int3x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m23 + b._m23); }
	static int3x4 operator *(const int3x4& a, const int3x4& b) { return int3x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m23 * b._m23); }
	static int3x4 operator -(const int3x4& a, const int3x4& b) { return int3x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m23 - b._m23); }
	static int3x4 operator /(const int3x4& a, const int3x4& b) { return int3x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m23 / b._m23); }
	static int3x4 operator %(const int3x4& a, const int3x4& b) { return int3x4(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m03 % b._m03, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m13 % b._m13, a._m20 % b._m20, a._m21 % b._m21, a._m22 % b._m22, a._m23 % b._m23); }
	static int3x4 operator &(const int3x4& a, const int3x4& b) { return int3x4(a._m00 & b._m00, a._m01 & b._m01, a._m02 & b._m02, a._m03 & b._m03, a._m10 & b._m10, a._m11 & b._m11, a._m12 & b._m12, a._m13 & b._m13, a._m20 & b._m20, a._m21 & b._m21, a._m22 & b._m22, a._m23 & b._m23); }
	static int3x4 operator |(const int3x4& a, const int3x4& b) { return int3x4(a._m00 | b._m00, a._m01 | b._m01, a._m02 | b._m02, a._m03 | b._m03, a._m10 | b._m10, a._m11 | b._m11, a._m12 | b._m12, a._m13 | b._m13, a._m20 | b._m20, a._m21 | b._m21, a._m22 | b._m22, a._m23 | b._m23); }
	static int3x4 operator ^(const int3x4& a, const int3x4& b) { return int3x4(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m02 ^ b._m02, a._m03 ^ b._m03, a._m10 ^ b._m10, a._m11 ^ b._m11, a._m12 ^ b._m12, a._m13 ^ b._m13, a._m20 ^ b._m20, a._m21 ^ b._m21, a._m22 ^ b._m22, a._m23 ^ b._m23); }
	static int3x4 operator ==(const int3x4& a, const int3x4& b) { return int3x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m23 == b._m23) ? 1 : 0); }
	static int3x4 operator !=(const int3x4& a, const int3x4& b) { return int3x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m23 != b._m23) ? 1 : 0); }
	static int3x4 operator <(const int3x4& a, const int3x4& b) { return int3x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m23 < b._m23) ? 1 : 0); }
	static int3x4 operator <=(const int3x4& a, const int3x4& b) { return int3x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m23 <= b._m23) ? 1 : 0); }
	static int3x4 operator >=(const int3x4& a, const int3x4& b) { return int3x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m23 >= b._m23) ? 1 : 0); }
	static int3x4 operator >(const int3x4& a, const int3x4& b) { return int3x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m23 > b._m23) ? 1 : 0); }

	struct int4x1 {
		int _m00;
		int _m10;
		int _m20;
		int _m30;
		int1 operator[] (int row) const {
			if (row == 0) return int1(_m00);
			if (row == 1) return int1(_m10);
			if (row == 2) return int1(_m20);
			if (row == 3) return int1(_m30);
			return 0; // Silent return ... valid for HLSL
		}
		int4x1(int _m00, int _m10, int _m20, int _m30) {
			this->_m00 = _m00;
			this->_m10 = _m10;
			this->_m20 = _m20;
			this->_m30 = _m30;
		}
		int4x1(int v) :int4x1(v, v, v, v) {}
		int4x1(const int1& r0, const int1& r1, const int1& r2, const int1& r3) : int4x1(r0.x, r1.x, r2.x, r3.x) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int3x1() const { return int3x1(this->_m00, this->_m10, this->_m20); }
		operator float4x1() const;
		operator uint4x1() const;
	};
	static int4x1 operator -(const int4x1& a) { return int4x1(-a._m00, -a._m10, -a._m20, -a._m30); }
	static int4x1 operator +(const int4x1& a) { return int4x1(+a._m00, +a._m10, +a._m20, +a._m30); }
	static int4x1 operator ~(const int4x1& a) { return int4x1(~a._m00, ~a._m10, ~a._m20, ~a._m30); }
	static int4x1 operator !(const int4x1& a) { return int4x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0); }
	static int4x1 operator +(const int4x1& a, const int4x1& b) { return int4x1(a._m00 + b._m00, a._m10 + b._m10, a._m20 + b._m20, a._m30 + b._m30); }
	static int4x1 operator *(const int4x1& a, const int4x1& b) { return int4x1(a._m00 * b._m00, a._m10 * b._m10, a._m20 * b._m20, a._m30 * b._m30); }
	static int4x1 operator -(const int4x1& a, const int4x1& b) { return int4x1(a._m00 - b._m00, a._m10 - b._m10, a._m20 - b._m20, a._m30 - b._m30); }
	static int4x1 operator /(const int4x1& a, const int4x1& b) { return int4x1(a._m00 / b._m00, a._m10 / b._m10, a._m20 / b._m20, a._m30 / b._m30); }
	static int4x1 operator %(const int4x1& a, const int4x1& b) { return int4x1(a._m00 % b._m00, a._m10 % b._m10, a._m20 % b._m20, a._m30 % b._m30); }
	static int4x1 operator &(const int4x1& a, const int4x1& b) { return int4x1(a._m00 & b._m00, a._m10 & b._m10, a._m20 & b._m20, a._m30 & b._m30); }
	static int4x1 operator |(const int4x1& a, const int4x1& b) { return int4x1(a._m00 | b._m00, a._m10 | b._m10, a._m20 | b._m20, a._m30 | b._m30); }
	static int4x1 operator ^(const int4x1& a, const int4x1& b) { return int4x1(a._m00 ^ b._m00, a._m10 ^ b._m10, a._m20 ^ b._m20, a._m30 ^ b._m30); }
	static int4x1 operator ==(const int4x1& a, const int4x1& b) { return int4x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0); }
	static int4x1 operator !=(const int4x1& a, const int4x1& b) { return int4x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0); }
	static int4x1 operator <(const int4x1& a, const int4x1& b) { return int4x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0); }
	static int4x1 operator <=(const int4x1& a, const int4x1& b) { return int4x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0); }
	static int4x1 operator >=(const int4x1& a, const int4x1& b) { return int4x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0); }
	static int4x1 operator >(const int4x1& a, const int4x1& b) { return int4x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0); }

	struct int4x2 {
		int _m00;
		int _m10;
		int _m20;
		int _m30;
		int _m01;
		int _m11;
		int _m21;
		int _m31;
		int2 operator[] (int row) const {
			if (row == 0) return int2(_m00, _m01);
			if (row == 1) return int2(_m10, _m11);
			if (row == 2) return int2(_m20, _m21);
			if (row == 3) return int2(_m30, _m31);
			return 0; // Silent return ... valid for HLSL
		}
		int4x2(int _m00, int _m01, int _m10, int _m11, int _m20, int _m21, int _m30, int _m31) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m30 = _m30;
			this->_m31 = _m31;
		}
		int4x2(int v) :int4x2(v, v, v, v, v, v, v, v) {}
		int4x2(const int2& r0, const int2& r1, const int2& r2, const int2& r3) : int4x2(r0.x, r0.y, r1.x, r1.y, r2.x, r2.y, r3.x, r3.y) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int2x2() const { return int2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator int3x1() const { return int3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator int3x2() const { return int3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator int4x1() const { return int4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		operator float4x2() const;
		operator uint4x2() const;
	};
	static int4x2 operator -(const int4x2& a) { return int4x2(-a._m00, -a._m01, -a._m10, -a._m11, -a._m20, -a._m21, -a._m30, -a._m31); }
	static int4x2 operator +(const int4x2& a) { return int4x2(+a._m00, +a._m01, +a._m10, +a._m11, +a._m20, +a._m21, +a._m30, +a._m31); }
	static int4x2 operator ~(const int4x2& a) { return int4x2(~a._m00, ~a._m01, ~a._m10, ~a._m11, ~a._m20, ~a._m21, ~a._m30, ~a._m31); }
	static int4x2 operator !(const int4x2& a) { return int4x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0); }
	static int4x2 operator +(const int4x2& a, const int4x2& b) { return int4x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11, a._m20 + b._m20, a._m21 + b._m21, a._m30 + b._m30, a._m31 + b._m31); }
	static int4x2 operator *(const int4x2& a, const int4x2& b) { return int4x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11, a._m20 * b._m20, a._m21 * b._m21, a._m30 * b._m30, a._m31 * b._m31); }
	static int4x2 operator -(const int4x2& a, const int4x2& b) { return int4x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11, a._m20 - b._m20, a._m21 - b._m21, a._m30 - b._m30, a._m31 - b._m31); }
	static int4x2 operator /(const int4x2& a, const int4x2& b) { return int4x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11, a._m20 / b._m20, a._m21 / b._m21, a._m30 / b._m30, a._m31 / b._m31); }
	static int4x2 operator %(const int4x2& a, const int4x2& b) { return int4x2(a._m00 % b._m00, a._m01 % b._m01, a._m10 % b._m10, a._m11 % b._m11, a._m20 % b._m20, a._m21 % b._m21, a._m30 % b._m30, a._m31 % b._m31); }
	static int4x2 operator &(const int4x2& a, const int4x2& b) { return int4x2(a._m00 & b._m00, a._m01 & b._m01, a._m10 & b._m10, a._m11 & b._m11, a._m20 & b._m20, a._m21 & b._m21, a._m30 & b._m30, a._m31 & b._m31); }
	static int4x2 operator |(const int4x2& a, const int4x2& b) { return int4x2(a._m00 | b._m00, a._m01 | b._m01, a._m10 | b._m10, a._m11 | b._m11, a._m20 | b._m20, a._m21 | b._m21, a._m30 | b._m30, a._m31 | b._m31); }
	static int4x2 operator ^(const int4x2& a, const int4x2& b) { return int4x2(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m10 ^ b._m10, a._m11 ^ b._m11, a._m20 ^ b._m20, a._m21 ^ b._m21, a._m30 ^ b._m30, a._m31 ^ b._m31); }
	static int4x2 operator ==(const int4x2& a, const int4x2& b) { return int4x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0); }
	static int4x2 operator !=(const int4x2& a, const int4x2& b) { return int4x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0); }
	static int4x2 operator <(const int4x2& a, const int4x2& b) { return int4x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0); }
	static int4x2 operator <=(const int4x2& a, const int4x2& b) { return int4x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0); }
	static int4x2 operator >=(const int4x2& a, const int4x2& b) { return int4x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0); }
	static int4x2 operator >(const int4x2& a, const int4x2& b) { return int4x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0); }

	struct int4x3 {
		int _m00;
		int _m10;
		int _m20;
		int _m30;
		int _m01;
		int _m11;
		int _m21;
		int _m31;
		int _m02;
		int _m12;
		int _m22;
		int _m32;
		int3 operator[] (int row) const {
			if (row == 0) return int3(_m00, _m01, _m02);
			if (row == 1) return int3(_m10, _m11, _m12);
			if (row == 2) return int3(_m20, _m21, _m22);
			if (row == 3) return int3(_m30, _m31, _m32);
			return 0; // Silent return ... valid for HLSL
		}
		int4x3(int _m00, int _m01, int _m02, int _m10, int _m11, int _m12, int _m20, int _m21, int _m22, int _m30, int _m31, int _m32) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m30 = _m30;
			this->_m31 = _m31;
			this->_m32 = _m32;
		}
		int4x3(int v) :int4x3(v, v, v, v, v, v, v, v, v, v, v, v) {}
		int4x3(const int3& r0, const int3& r1, const int3& r2, const int3& r3) : int4x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z, r2.x, r2.y, r2.z, r3.x, r3.y, r3.z) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int1x3() const { return int1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int2x2() const { return int2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator int2x3() const { return int2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator int3x1() const { return int3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator int3x2() const { return int3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator int3x3() const { return int3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		explicit operator int4x1() const { return int4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		explicit operator int4x2() const { return int4x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21, this->_m30, this->_m31); }
		operator float4x3() const;
		operator uint4x3() const;
	};
	static int4x3 operator -(const int4x3& a) { return int4x3(-a._m00, -a._m01, -a._m02, -a._m10, -a._m11, -a._m12, -a._m20, -a._m21, -a._m22, -a._m30, -a._m31, -a._m32); }
	static int4x3 operator +(const int4x3& a) { return int4x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12, +a._m20, +a._m21, +a._m22, +a._m30, +a._m31, +a._m32); }
	static int4x3 operator ~(const int4x3& a) { return int4x3(~a._m00, ~a._m01, ~a._m02, ~a._m10, ~a._m11, ~a._m12, ~a._m20, ~a._m21, ~a._m22, ~a._m30, ~a._m31, ~a._m32); }
	static int4x3 operator !(const int4x3& a) { return int4x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0, a._m32 == 0 ? 1 : 0); }
	static int4x3 operator +(const int4x3& a, const int4x3& b) { return int4x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m30 + b._m30, a._m31 + b._m31, a._m32 + b._m32); }
	static int4x3 operator *(const int4x3& a, const int4x3& b) { return int4x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m30 * b._m30, a._m31 * b._m31, a._m32 * b._m32); }
	static int4x3 operator -(const int4x3& a, const int4x3& b) { return int4x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m30 - b._m30, a._m31 - b._m31, a._m32 - b._m32); }
	static int4x3 operator /(const int4x3& a, const int4x3& b) { return int4x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m30 / b._m30, a._m31 / b._m31, a._m32 / b._m32); }
	static int4x3 operator %(const int4x3& a, const int4x3& b) { return int4x3(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m20 % b._m20, a._m21 % b._m21, a._m22 % b._m22, a._m30 % b._m30, a._m31 % b._m31, a._m32 % b._m32); }
	static int4x3 operator &(const int4x3& a, const int4x3& b) { return int4x3(a._m00 & b._m00, a._m01 & b._m01, a._m02 & b._m02, a._m10 & b._m10, a._m11 & b._m11, a._m12 & b._m12, a._m20 & b._m20, a._m21 & b._m21, a._m22 & b._m22, a._m30 & b._m30, a._m31 & b._m31, a._m32 & b._m32); }
	static int4x3 operator |(const int4x3& a, const int4x3& b) { return int4x3(a._m00 | b._m00, a._m01 | b._m01, a._m02 | b._m02, a._m10 | b._m10, a._m11 | b._m11, a._m12 | b._m12, a._m20 | b._m20, a._m21 | b._m21, a._m22 | b._m22, a._m30 | b._m30, a._m31 | b._m31, a._m32 | b._m32); }
	static int4x3 operator ^(const int4x3& a, const int4x3& b) { return int4x3(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m02 ^ b._m02, a._m10 ^ b._m10, a._m11 ^ b._m11, a._m12 ^ b._m12, a._m20 ^ b._m20, a._m21 ^ b._m21, a._m22 ^ b._m22, a._m30 ^ b._m30, a._m31 ^ b._m31, a._m32 ^ b._m32); }
	static int4x3 operator ==(const int4x3& a, const int4x3& b) { return int4x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0, (a._m32 == b._m32) ? 1 : 0); }
	static int4x3 operator !=(const int4x3& a, const int4x3& b) { return int4x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0, (a._m32 != b._m32) ? 1 : 0); }
	static int4x3 operator <(const int4x3& a, const int4x3& b) { return int4x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0, (a._m32 < b._m32) ? 1 : 0); }
	static int4x3 operator <=(const int4x3& a, const int4x3& b) { return int4x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0, (a._m32 <= b._m32) ? 1 : 0); }
	static int4x3 operator >=(const int4x3& a, const int4x3& b) { return int4x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0, (a._m32 >= b._m32) ? 1 : 0); }
	static int4x3 operator >(const int4x3& a, const int4x3& b) { return int4x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0, (a._m32 > b._m32) ? 1 : 0); }

	struct int4x4 {
		int _m00;
		int _m10;
		int _m20;
		int _m30;
		int _m01;
		int _m11;
		int _m21;
		int _m31;
		int _m02;
		int _m12;
		int _m22;
		int _m32;
		int _m03;
		int _m13;
		int _m23;
		int _m33;
		int4 operator[] (int row) const {
			if (row == 0) return int4(_m00, _m01, _m02, _m03);
			if (row == 1) return int4(_m10, _m11, _m12, _m13);
			if (row == 2) return int4(_m20, _m21, _m22, _m23);
			if (row == 3) return int4(_m30, _m31, _m32, _m33);
			return 0; // Silent return ... valid for HLSL
		}
		int4x4(int _m00, int _m01, int _m02, int _m03, int _m10, int _m11, int _m12, int _m13, int _m20, int _m21, int _m22, int _m23, int _m30, int _m31, int _m32, int _m33) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m23 = _m23;
			this->_m30 = _m30;
			this->_m31 = _m31;
			this->_m32 = _m32;
			this->_m33 = _m33;
		}
		int4x4(int v) :int4x4(v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v) {}
		int4x4(const int4& r0, const int4& r1, const int4& r2, const int4& r3) : int4x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w, r3.x, r3.y, r3.z, r3.w) { }
		explicit operator int1x1() const { return int1x1(this->_m00); }
		explicit operator int1x2() const { return int1x2(this->_m00, this->_m01); }
		explicit operator int1x3() const { return int1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator int1x4() const { return int1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator int2x1() const { return int2x1(this->_m00, this->_m10); }
		explicit operator int2x2() const { return int2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator int2x3() const { return int2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator int2x4() const { return int2x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13); }
		explicit operator int3x1() const { return int3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator int3x2() const { return int3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator int3x3() const { return int3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		explicit operator int3x4() const { return int3x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13, this->_m20, this->_m21, this->_m22, this->_m23); }
		explicit operator int4x1() const { return int4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		explicit operator int4x2() const { return int4x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21, this->_m30, this->_m31); }
		explicit operator int4x3() const { return int4x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22, this->_m30, this->_m31, this->_m32); }
		operator float4x4() const;
		operator uint4x4() const;
	};
	static int4x4 operator -(const int4x4& a) { return int4x4(-a._m00, -a._m01, -a._m02, -a._m03, -a._m10, -a._m11, -a._m12, -a._m13, -a._m20, -a._m21, -a._m22, -a._m23, -a._m30, -a._m31, -a._m32, -a._m33); }
	static int4x4 operator +(const int4x4& a) { return int4x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13, +a._m20, +a._m21, +a._m22, +a._m23, +a._m30, +a._m31, +a._m32, +a._m33); }
	static int4x4 operator ~(const int4x4& a) { return int4x4(~a._m00, ~a._m01, ~a._m02, ~a._m03, ~a._m10, ~a._m11, ~a._m12, ~a._m13, ~a._m20, ~a._m21, ~a._m22, ~a._m23, ~a._m30, ~a._m31, ~a._m32, ~a._m33); }
	static int4x4 operator !(const int4x4& a) { return int4x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m23 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0, a._m32 == 0 ? 1 : 0, a._m33 == 0 ? 1 : 0); }
	static int4x4 operator +(const int4x4& a, const int4x4& b) { return int4x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m23 + b._m23, a._m30 + b._m30, a._m31 + b._m31, a._m32 + b._m32, a._m33 + b._m33); }
	static int4x4 operator *(const int4x4& a, const int4x4& b) { return int4x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m23 * b._m23, a._m30 * b._m30, a._m31 * b._m31, a._m32 * b._m32, a._m33 * b._m33); }
	static int4x4 operator -(const int4x4& a, const int4x4& b) { return int4x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m23 - b._m23, a._m30 - b._m30, a._m31 - b._m31, a._m32 - b._m32, a._m33 - b._m33); }
	static int4x4 operator /(const int4x4& a, const int4x4& b) { return int4x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m23 / b._m23, a._m30 / b._m30, a._m31 / b._m31, a._m32 / b._m32, a._m33 / b._m33); }
	static int4x4 operator %(const int4x4& a, const int4x4& b) { return int4x4(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m03 % b._m03, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m13 % b._m13, a._m20 % b._m20, a._m21 % b._m21, a._m22 % b._m22, a._m23 % b._m23, a._m30 % b._m30, a._m31 % b._m31, a._m32 % b._m32, a._m33 % b._m33); }
	static int4x4 operator &(const int4x4& a, const int4x4& b) { return int4x4(a._m00 & b._m00, a._m01 & b._m01, a._m02 & b._m02, a._m03 & b._m03, a._m10 & b._m10, a._m11 & b._m11, a._m12 & b._m12, a._m13 & b._m13, a._m20 & b._m20, a._m21 & b._m21, a._m22 & b._m22, a._m23 & b._m23, a._m30 & b._m30, a._m31 & b._m31, a._m32 & b._m32, a._m33 & b._m33); }
	static int4x4 operator |(const int4x4& a, const int4x4& b) { return int4x4(a._m00 | b._m00, a._m01 | b._m01, a._m02 | b._m02, a._m03 | b._m03, a._m10 | b._m10, a._m11 | b._m11, a._m12 | b._m12, a._m13 | b._m13, a._m20 | b._m20, a._m21 | b._m21, a._m22 | b._m22, a._m23 | b._m23, a._m30 | b._m30, a._m31 | b._m31, a._m32 | b._m32, a._m33 | b._m33); }
	static int4x4 operator ^(const int4x4& a, const int4x4& b) { return int4x4(a._m00 ^ b._m00, a._m01 ^ b._m01, a._m02 ^ b._m02, a._m03 ^ b._m03, a._m10 ^ b._m10, a._m11 ^ b._m11, a._m12 ^ b._m12, a._m13 ^ b._m13, a._m20 ^ b._m20, a._m21 ^ b._m21, a._m22 ^ b._m22, a._m23 ^ b._m23, a._m30 ^ b._m30, a._m31 ^ b._m31, a._m32 ^ b._m32, a._m33 ^ b._m33); }
	static int4x4 operator ==(const int4x4& a, const int4x4& b) { return int4x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m23 == b._m23) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0, (a._m32 == b._m32) ? 1 : 0, (a._m33 == b._m33) ? 1 : 0); }
	static int4x4 operator !=(const int4x4& a, const int4x4& b) { return int4x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m23 != b._m23) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0, (a._m32 != b._m32) ? 1 : 0, (a._m33 != b._m33) ? 1 : 0); }
	static int4x4 operator <(const int4x4& a, const int4x4& b) { return int4x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m23 < b._m23) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0, (a._m32 < b._m32) ? 1 : 0, (a._m33 < b._m33) ? 1 : 0); }
	static int4x4 operator <=(const int4x4& a, const int4x4& b) { return int4x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m23 <= b._m23) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0, (a._m32 <= b._m32) ? 1 : 0, (a._m33 <= b._m33) ? 1 : 0); }
	static int4x4 operator >=(const int4x4& a, const int4x4& b) { return int4x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m23 >= b._m23) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0, (a._m32 >= b._m32) ? 1 : 0, (a._m33 >= b._m33) ? 1 : 0); }
	static int4x4 operator >(const int4x4& a, const int4x4& b) { return int4x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m23 > b._m23) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0, (a._m32 > b._m32) ? 1 : 0, (a._m33 > b._m33) ? 1 : 0); }

	struct float1 {
	private:
		static float __TRASH;
	public:
		float x;
		float& operator[](int idx) {
			if (idx == 0) return this->x;
			return __TRASH; // Silent return ... valid for HLSL
		}
		float1() = default;
		float1(float x) {
			this->x = x;
		}
		operator int1() const;
		operator uint1() const;
	};
	static float1 operator -(const float1& a) { return float1(-a.x); }
	static float1 operator +(const float1& a) { return float1(+a.x); }
	static int1 operator !(const float1& a) { return int1(a.x == 0 ? 1 : 0); }
	static int1 operator ==(const float1& a, const float1& b) { return int1((a.x == b.x) ? 1 : 0); }
	static int1 operator !=(const float1& a, const float1& b) { return int1((a.x != b.x) ? 1 : 0); }
	static int1 operator <(const float1& a, const float1& b) { return int1((a.x < b.x) ? 1 : 0); }
	static int1 operator <=(const float1& a, const float1& b) { return int1((a.x <= b.x) ? 1 : 0); }
	static int1 operator >=(const float1& a, const float1& b) { return int1((a.x >= b.x) ? 1 : 0); }
	static int1 operator >(const float1& a, const float1& b) { return int1((a.x > b.x) ? 1 : 0); }
	static float1 operator +(const float1& a, const float1& b) { return float1(a.x + b.x); }
	static float1 operator *(const float1& a, const float1& b) { return float1(a.x * b.x); }
	static float1 operator -(const float1& a, const float1& b) { return float1(a.x - b.x); }
	static float1 operator /(const float1& a, const float1& b) { return float1(a.x / b.x); }
	static float1 operator %(const float1& a, const float1& b) { return float1(fmod(a.x, b.x)); }

	struct float2 {
	private:
		static float __TRASH;
	public:
		float x;
		float y;
		float& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			return __TRASH; // Silent return ... valid for HLSL
		}
		float2() = default;
		float2(float x, float y) {
			this->x = x;
			this->y = y;
		}
		float2(float v) :float2(v, v) {}
		operator float1() const { return float1(this->x); }
		operator int2() const;
		operator uint2() const;
	};
	static float2 operator -(const float2& a) { return float2(-a.x, -a.y); }
	static float2 operator +(const float2& a) { return float2(+a.x, +a.y); }
	static int2 operator !(const float2& a) { return int2(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0); }
	static int2 operator ==(const float2& a, const float2& b) { return int2((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0); }
	static int2 operator !=(const float2& a, const float2& b) { return int2((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0); }
	static int2 operator <(const float2& a, const float2& b) { return int2((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0); }
	static int2 operator <=(const float2& a, const float2& b) { return int2((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0); }
	static int2 operator >=(const float2& a, const float2& b) { return int2((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0); }
	static int2 operator >(const float2& a, const float2& b) { return int2((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0); }
	static float2 operator +(const float2& a, const float2& b) { return float2(a.x + b.x, a.y + b.y); }
	static float2 operator *(const float2& a, const float2& b) { return float2(a.x * b.x, a.y * b.y); }
	static float2 operator -(const float2& a, const float2& b) { return float2(a.x - b.x, a.y - b.y); }
	static float2 operator /(const float2& a, const float2& b) { return float2(a.x / b.x, a.y / b.y); }
	static float2 operator %(const float2& a, const float2& b) { return float2(fmod(a.x, b.x), fmod(a.y, b.y)); }

	struct float3 {
	private:
		static float __TRASH;
	public:
		float x;
		float y;
		float z;
		float2 get_xy() { return float2(this->x, this->y); }
		float& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			if (idx == 2) return this->z;
			return __TRASH; // Silent return ... valid for HLSL
		}
		float3() = default;
		float3(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
		float3(float v) :float3(v, v, v) {}
		operator float1() const { return float1(this->x); }
		operator float2() const { return float2(this->x, this->y); }
		operator int3() const;
		operator uint3() const;
	};
	static float3 operator -(const float3& a) { return float3(-a.x, -a.y, -a.z); }
	static float3 operator +(const float3& a) { return float3(+a.x, +a.y, +a.z); }
	static int3 operator !(const float3& a) { return int3(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0, a.z == 0 ? 1 : 0); }
	static int3 operator ==(const float3& a, const float3& b) { return int3((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0, (a.z == b.z) ? 1 : 0); }
	static int3 operator !=(const float3& a, const float3& b) { return int3((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0, (a.z != b.z) ? 1 : 0); }
	static int3 operator <(const float3& a, const float3& b) { return int3((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0, (a.z < b.z) ? 1 : 0); }
	static int3 operator <=(const float3& a, const float3& b) { return int3((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0, (a.z <= b.z) ? 1 : 0); }
	static int3 operator >=(const float3& a, const float3& b) { return int3((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0, (a.z >= b.z) ? 1 : 0); }
	static int3 operator >(const float3& a, const float3& b) { return int3((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0, (a.z > b.z) ? 1 : 0); }
	static float3 operator +(const float3& a, const float3& b) { return float3(a.x + b.x, a.y + b.y, a.z + b.z); }
	static float3 operator *(const float3& a, const float3& b) { return float3(a.x * b.x, a.y * b.y, a.z * b.z); }
	static float3 operator -(const float3& a, const float3& b) { return float3(a.x - b.x, a.y - b.y, a.z - b.z); }
	static float3 operator /(const float3& a, const float3& b) { return float3(a.x / b.x, a.y / b.y, a.z / b.z); }
	static float3 operator %(const float3& a, const float3& b) { return float3(fmod(a.x, b.x), fmod(a.y, b.y), fmod(a.z, b.z)); }

	struct float4 {
	private:
		static float __TRASH;
	public:
		float x;
		float y;
		float z;
		float w;
		float2 get_xy() { return float2(this->x, this->y); }
		float3 get_xyz() { return float3(this->x, this->y, this->z); }
		float& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			if (idx == 2) return this->z;
			if (idx == 3) return this->w;
			return __TRASH; // Silent return ... valid for HLSL
		}
		float4() = default;
		float4(float x, float y, float z, float w) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
		float4(float v) :float4(v, v, v, v) {}
		operator float1() const { return float1(this->x); }
		operator float2() const { return float2(this->x, this->y); }
		operator float3() const { return float3(this->x, this->y, this->z); }
		operator int4() const;
		operator uint4() const;
	};
	static float4 operator -(const float4& a) { return float4(-a.x, -a.y, -a.z, -a.w); }
	static float4 operator +(const float4& a) { return float4(+a.x, +a.y, +a.z, +a.w); }
	static int4 operator !(const float4& a) { return int4(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0, a.z == 0 ? 1 : 0, a.w == 0 ? 1 : 0); }
	static int4 operator ==(const float4& a, const float4& b) { return int4((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0, (a.z == b.z) ? 1 : 0, (a.w == b.w) ? 1 : 0); }
	static int4 operator !=(const float4& a, const float4& b) { return int4((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0, (a.z != b.z) ? 1 : 0, (a.w != b.w) ? 1 : 0); }
	static int4 operator <(const float4& a, const float4& b) { return int4((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0, (a.z < b.z) ? 1 : 0, (a.w < b.w) ? 1 : 0); }
	static int4 operator <=(const float4& a, const float4& b) { return int4((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0, (a.z <= b.z) ? 1 : 0, (a.w <= b.w) ? 1 : 0); }
	static int4 operator >=(const float4& a, const float4& b) { return int4((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0, (a.z >= b.z) ? 1 : 0, (a.w >= b.w) ? 1 : 0); }
	static int4 operator >(const float4& a, const float4& b) { return int4((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0, (a.z > b.z) ? 1 : 0, (a.w > b.w) ? 1 : 0); }
	static float4 operator +(const float4& a, const float4& b) { return float4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	static float4 operator *(const float4& a, const float4& b) { return float4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
	static float4 operator -(const float4& a, const float4& b) { return float4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
	static float4 operator /(const float4& a, const float4& b) { return float4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
	static float4 operator %(const float4& a, const float4& b) { return float4(fmod(a.x, b.x), fmod(a.y, b.y), fmod(a.z, b.z), fmod(a.w, b.w)); }

	struct float1x1 {
		float _m00;
		float1 operator[] (int row) const {
			if (row == 0) return float1(_m00);
			return 0; // Silent return ... valid for HLSL
		}
		float1x1(float _m00) {
			this->_m00 = _m00;
		}
		operator float1() const { return float1(this->_m00); }
		float1x1(const float1& v) : float1x1(v.x) { }
		operator int1x1() const;
		operator uint1x1() const;
	};
	static float1x1 operator -(const float1x1& a) { return float1x1(-a._m00); }
	static float1x1 operator +(const float1x1& a) { return float1x1(+a._m00); }
	static int1x1 operator !(const float1x1& a) { return int1x1(a._m00 == 0 ? 1 : 0); }
	static float1x1 operator +(const float1x1& a, const float1x1& b) { return float1x1(a._m00 + b._m00); }
	static float1x1 operator *(const float1x1& a, const float1x1& b) { return float1x1(a._m00 * b._m00); }
	static float1x1 operator -(const float1x1& a, const float1x1& b) { return float1x1(a._m00 - b._m00); }
	static float1x1 operator /(const float1x1& a, const float1x1& b) { return float1x1(a._m00 / b._m00); }
	static float1x1 operator %(const float1x1& a, const float1x1& b) { return float1x1(fmod(a._m00, b._m00)); }
	static int1x1 operator ==(const float1x1& a, const float1x1& b) { return int1x1((a._m00 == b._m00) ? 1 : 0); }
	static int1x1 operator !=(const float1x1& a, const float1x1& b) { return int1x1((a._m00 != b._m00) ? 1 : 0); }
	static int1x1 operator <(const float1x1& a, const float1x1& b) { return int1x1((a._m00 < b._m00) ? 1 : 0); }
	static int1x1 operator <=(const float1x1& a, const float1x1& b) { return int1x1((a._m00 <= b._m00) ? 1 : 0); }
	static int1x1 operator >=(const float1x1& a, const float1x1& b) { return int1x1((a._m00 >= b._m00) ? 1 : 0); }
	static int1x1 operator >(const float1x1& a, const float1x1& b) { return int1x1((a._m00 > b._m00) ? 1 : 0); }

	struct float1x2 {
		float _m00;
		float _m01;
		float2 operator[] (int row) const {
			if (row == 0) return float2(_m00, _m01);
			return 0; // Silent return ... valid for HLSL
		}
		float1x2(float _m00, float _m01) {
			this->_m00 = _m00;
			this->_m01 = _m01;
		}
		float1x2(float v) :float1x2(v, v) {}
		operator float2() const { return float2(this->_m00, this->_m01); }
		float1x2(const float2& v) : float1x2(v.x, v.y) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		operator int1x2() const;
		operator uint1x2() const;
	};
	static float1x2 operator -(const float1x2& a) { return float1x2(-a._m00, -a._m01); }
	static float1x2 operator +(const float1x2& a) { return float1x2(+a._m00, +a._m01); }
	static int1x2 operator !(const float1x2& a) { return int1x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0); }
	static float1x2 operator +(const float1x2& a, const float1x2& b) { return float1x2(a._m00 + b._m00, a._m01 + b._m01); }
	static float1x2 operator *(const float1x2& a, const float1x2& b) { return float1x2(a._m00 * b._m00, a._m01 * b._m01); }
	static float1x2 operator -(const float1x2& a, const float1x2& b) { return float1x2(a._m00 - b._m00, a._m01 - b._m01); }
	static float1x2 operator /(const float1x2& a, const float1x2& b) { return float1x2(a._m00 / b._m00, a._m01 / b._m01); }
	static float1x2 operator %(const float1x2& a, const float1x2& b) { return float1x2(fmod(a._m00, b._m00), fmod(a._m01, b._m01)); }
	static int1x2 operator ==(const float1x2& a, const float1x2& b) { return int1x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0); }
	static int1x2 operator !=(const float1x2& a, const float1x2& b) { return int1x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0); }
	static int1x2 operator <(const float1x2& a, const float1x2& b) { return int1x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0); }
	static int1x2 operator <=(const float1x2& a, const float1x2& b) { return int1x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0); }
	static int1x2 operator >=(const float1x2& a, const float1x2& b) { return int1x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0); }
	static int1x2 operator >(const float1x2& a, const float1x2& b) { return int1x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0); }

	struct float1x3 {
		float _m00;
		float _m01;
		float _m02;
		float3 operator[] (int row) const {
			if (row == 0) return float3(_m00, _m01, _m02);
			return 0; // Silent return ... valid for HLSL
		}
		float1x3(float _m00, float _m01, float _m02) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
		}
		float1x3(float v) :float1x3(v, v, v) {}
		operator float3() const { return float3(this->_m00, this->_m01, this->_m02); }
		float1x3(const float3& v) : float1x3(v.x, v.y, v.z) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		operator int1x3() const;
		operator uint1x3() const;
	};
	static float1x3 operator -(const float1x3& a) { return float1x3(-a._m00, -a._m01, -a._m02); }
	static float1x3 operator +(const float1x3& a) { return float1x3(+a._m00, +a._m01, +a._m02); }
	static int1x3 operator !(const float1x3& a) { return int1x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0); }
	static float1x3 operator +(const float1x3& a, const float1x3& b) { return float1x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02); }
	static float1x3 operator *(const float1x3& a, const float1x3& b) { return float1x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02); }
	static float1x3 operator -(const float1x3& a, const float1x3& b) { return float1x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02); }
	static float1x3 operator /(const float1x3& a, const float1x3& b) { return float1x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02); }
	static float1x3 operator %(const float1x3& a, const float1x3& b) { return float1x3(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02)); }
	static int1x3 operator ==(const float1x3& a, const float1x3& b) { return int1x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0); }
	static int1x3 operator !=(const float1x3& a, const float1x3& b) { return int1x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0); }
	static int1x3 operator <(const float1x3& a, const float1x3& b) { return int1x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0); }
	static int1x3 operator <=(const float1x3& a, const float1x3& b) { return int1x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0); }
	static int1x3 operator >=(const float1x3& a, const float1x3& b) { return int1x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0); }
	static int1x3 operator >(const float1x3& a, const float1x3& b) { return int1x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0); }

	struct float1x4 {
		float _m00;
		float _m01;
		float _m02;
		float _m03;
		float4 operator[] (int row) const {
			if (row == 0) return float4(_m00, _m01, _m02, _m03);
			return 0; // Silent return ... valid for HLSL
		}
		float1x4(float _m00, float _m01, float _m02, float _m03) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
		}
		float1x4(float v) :float1x4(v, v, v, v) {}
		operator float4() const { return float4(this->_m00, this->_m01, this->_m02, this->_m03); }
		float1x4(const float4& v) : float1x4(v.x, v.y, v.z, v.w) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float1x3() const { return float1x3(this->_m00, this->_m01, this->_m02); }
		operator int1x4() const;
		operator uint1x4() const;
	};
	static float1x4 operator -(const float1x4& a) { return float1x4(-a._m00, -a._m01, -a._m02, -a._m03); }
	static float1x4 operator +(const float1x4& a) { return float1x4(+a._m00, +a._m01, +a._m02, +a._m03); }
	static int1x4 operator !(const float1x4& a) { return int1x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0); }
	static float1x4 operator +(const float1x4& a, const float1x4& b) { return float1x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03); }
	static float1x4 operator *(const float1x4& a, const float1x4& b) { return float1x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03); }
	static float1x4 operator -(const float1x4& a, const float1x4& b) { return float1x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03); }
	static float1x4 operator /(const float1x4& a, const float1x4& b) { return float1x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03); }
	static float1x4 operator %(const float1x4& a, const float1x4& b) { return float1x4(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m03, b._m03)); }
	static int1x4 operator ==(const float1x4& a, const float1x4& b) { return int1x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0); }
	static int1x4 operator !=(const float1x4& a, const float1x4& b) { return int1x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0); }
	static int1x4 operator <(const float1x4& a, const float1x4& b) { return int1x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0); }
	static int1x4 operator <=(const float1x4& a, const float1x4& b) { return int1x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0); }
	static int1x4 operator >=(const float1x4& a, const float1x4& b) { return int1x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0); }
	static int1x4 operator >(const float1x4& a, const float1x4& b) { return int1x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0); }

	struct float2x1 {
		float _m00;
		float _m10;
		float1 operator[] (int row) const {
			if (row == 0) return float1(_m00);
			if (row == 1) return float1(_m10);
			return 0; // Silent return ... valid for HLSL
		}
		float2x1(float _m00, float _m10) {
			this->_m00 = _m00;
			this->_m10 = _m10;
		}
		float2x1(float v) :float2x1(v, v) {}
		float2x1(const float1& r0, const float1& r1) : float2x1(r0.x, r1.x) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		operator int2x1() const;
		operator uint2x1() const;
	};
	static float2x1 operator -(const float2x1& a) { return float2x1(-a._m00, -a._m10); }
	static float2x1 operator +(const float2x1& a) { return float2x1(+a._m00, +a._m10); }
	static int2x1 operator !(const float2x1& a) { return int2x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0); }
	static float2x1 operator +(const float2x1& a, const float2x1& b) { return float2x1(a._m00 + b._m00, a._m10 + b._m10); }
	static float2x1 operator *(const float2x1& a, const float2x1& b) { return float2x1(a._m00 * b._m00, a._m10 * b._m10); }
	static float2x1 operator -(const float2x1& a, const float2x1& b) { return float2x1(a._m00 - b._m00, a._m10 - b._m10); }
	static float2x1 operator /(const float2x1& a, const float2x1& b) { return float2x1(a._m00 / b._m00, a._m10 / b._m10); }
	static float2x1 operator %(const float2x1& a, const float2x1& b) { return float2x1(fmod(a._m00, b._m00), fmod(a._m10, b._m10)); }
	static int2x1 operator ==(const float2x1& a, const float2x1& b) { return int2x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0); }
	static int2x1 operator !=(const float2x1& a, const float2x1& b) { return int2x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0); }
	static int2x1 operator <(const float2x1& a, const float2x1& b) { return int2x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0); }
	static int2x1 operator <=(const float2x1& a, const float2x1& b) { return int2x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0); }
	static int2x1 operator >=(const float2x1& a, const float2x1& b) { return int2x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0); }
	static int2x1 operator >(const float2x1& a, const float2x1& b) { return int2x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0); }

	struct float2x2 {
		float _m00;
		float _m10;
		float _m01;
		float _m11;
		float2 operator[] (int row) const {
			if (row == 0) return float2(_m00, _m01);
			if (row == 1) return float2(_m10, _m11);
			return 0; // Silent return ... valid for HLSL
		}
		float2x2(float _m00, float _m01, float _m10, float _m11) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
		}
		float2x2(float v) :float2x2(v, v, v, v) {}
		float2x2(const float2& r0, const float2& r1) : float2x2(r0.x, r0.y, r1.x, r1.y) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		operator int2x2() const;
		operator uint2x2() const;
	};
	static float2x2 operator -(const float2x2& a) { return float2x2(-a._m00, -a._m01, -a._m10, -a._m11); }
	static float2x2 operator +(const float2x2& a) { return float2x2(+a._m00, +a._m01, +a._m10, +a._m11); }
	static int2x2 operator !(const float2x2& a) { return int2x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0); }
	static float2x2 operator +(const float2x2& a, const float2x2& b) { return float2x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11); }
	static float2x2 operator *(const float2x2& a, const float2x2& b) { return float2x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11); }
	static float2x2 operator -(const float2x2& a, const float2x2& b) { return float2x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11); }
	static float2x2 operator /(const float2x2& a, const float2x2& b) { return float2x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11); }
	static float2x2 operator %(const float2x2& a, const float2x2& b) { return float2x2(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m10, b._m10), fmod(a._m11, b._m11)); }
	static int2x2 operator ==(const float2x2& a, const float2x2& b) { return int2x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0); }
	static int2x2 operator !=(const float2x2& a, const float2x2& b) { return int2x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0); }
	static int2x2 operator <(const float2x2& a, const float2x2& b) { return int2x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0); }
	static int2x2 operator <=(const float2x2& a, const float2x2& b) { return int2x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0); }
	static int2x2 operator >=(const float2x2& a, const float2x2& b) { return int2x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0); }
	static int2x2 operator >(const float2x2& a, const float2x2& b) { return int2x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0); }

	struct float2x3 {
		float _m00;
		float _m10;
		float _m01;
		float _m11;
		float _m02;
		float _m12;
		float3 operator[] (int row) const {
			if (row == 0) return float3(_m00, _m01, _m02);
			if (row == 1) return float3(_m10, _m11, _m12);
			return 0; // Silent return ... valid for HLSL
		}
		float2x3(float _m00, float _m01, float _m02, float _m10, float _m11, float _m12) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
		}
		float2x3(float v) :float2x3(v, v, v, v, v, v) {}
		float2x3(const float3& r0, const float3& r1) : float2x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float1x3() const { return float1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float2x2() const { return float2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		operator int2x3() const;
		operator uint2x3() const;
	};
	static float2x3 operator -(const float2x3& a) { return float2x3(-a._m00, -a._m01, -a._m02, -a._m10, -a._m11, -a._m12); }
	static float2x3 operator +(const float2x3& a) { return float2x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12); }
	static int2x3 operator !(const float2x3& a) { return int2x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0); }
	static float2x3 operator +(const float2x3& a, const float2x3& b) { return float2x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12); }
	static float2x3 operator *(const float2x3& a, const float2x3& b) { return float2x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12); }
	static float2x3 operator -(const float2x3& a, const float2x3& b) { return float2x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12); }
	static float2x3 operator /(const float2x3& a, const float2x3& b) { return float2x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12); }
	static float2x3 operator %(const float2x3& a, const float2x3& b) { return float2x3(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12)); }
	static int2x3 operator ==(const float2x3& a, const float2x3& b) { return int2x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0); }
	static int2x3 operator !=(const float2x3& a, const float2x3& b) { return int2x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0); }
	static int2x3 operator <(const float2x3& a, const float2x3& b) { return int2x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0); }
	static int2x3 operator <=(const float2x3& a, const float2x3& b) { return int2x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0); }
	static int2x3 operator >=(const float2x3& a, const float2x3& b) { return int2x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0); }
	static int2x3 operator >(const float2x3& a, const float2x3& b) { return int2x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0); }

	struct float2x4 {
		float _m00;
		float _m10;
		float _m01;
		float _m11;
		float _m02;
		float _m12;
		float _m03;
		float _m13;
		float4 operator[] (int row) const {
			if (row == 0) return float4(_m00, _m01, _m02, _m03);
			if (row == 1) return float4(_m10, _m11, _m12, _m13);
			return 0; // Silent return ... valid for HLSL
		}
		float2x4(float _m00, float _m01, float _m02, float _m03, float _m10, float _m11, float _m12, float _m13) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
		}
		float2x4(float v) :float2x4(v, v, v, v, v, v, v, v) {}
		float2x4(const float4& r0, const float4& r1) : float2x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float1x3() const { return float1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator float1x4() const { return float1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float2x2() const { return float2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator float2x3() const { return float2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		operator int2x4() const;
		operator uint2x4() const;
	};
	static float2x4 operator -(const float2x4& a) { return float2x4(-a._m00, -a._m01, -a._m02, -a._m03, -a._m10, -a._m11, -a._m12, -a._m13); }
	static float2x4 operator +(const float2x4& a) { return float2x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13); }
	static int2x4 operator !(const float2x4& a) { return int2x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0); }
	static float2x4 operator +(const float2x4& a, const float2x4& b) { return float2x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13); }
	static float2x4 operator *(const float2x4& a, const float2x4& b) { return float2x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13); }
	static float2x4 operator -(const float2x4& a, const float2x4& b) { return float2x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13); }
	static float2x4 operator /(const float2x4& a, const float2x4& b) { return float2x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13); }
	static float2x4 operator %(const float2x4& a, const float2x4& b) { return float2x4(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m03, b._m03), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m13, b._m13)); }
	static int2x4 operator ==(const float2x4& a, const float2x4& b) { return int2x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0); }
	static int2x4 operator !=(const float2x4& a, const float2x4& b) { return int2x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0); }
	static int2x4 operator <(const float2x4& a, const float2x4& b) { return int2x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0); }
	static int2x4 operator <=(const float2x4& a, const float2x4& b) { return int2x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0); }
	static int2x4 operator >=(const float2x4& a, const float2x4& b) { return int2x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0); }
	static int2x4 operator >(const float2x4& a, const float2x4& b) { return int2x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0); }

	struct float3x1 {
		float _m00;
		float _m10;
		float _m20;
		float1 operator[] (int row) const {
			if (row == 0) return float1(_m00);
			if (row == 1) return float1(_m10);
			if (row == 2) return float1(_m20);
			return 0; // Silent return ... valid for HLSL
		}
		float3x1(float _m00, float _m10, float _m20) {
			this->_m00 = _m00;
			this->_m10 = _m10;
			this->_m20 = _m20;
		}
		float3x1(float v) :float3x1(v, v, v) {}
		float3x1(const float1& r0, const float1& r1, const float1& r2) : float3x1(r0.x, r1.x, r2.x) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		operator int3x1() const;
		operator uint3x1() const;
	};
	static float3x1 operator -(const float3x1& a) { return float3x1(-a._m00, -a._m10, -a._m20); }
	static float3x1 operator +(const float3x1& a) { return float3x1(+a._m00, +a._m10, +a._m20); }
	static int3x1 operator !(const float3x1& a) { return int3x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0); }
	static float3x1 operator +(const float3x1& a, const float3x1& b) { return float3x1(a._m00 + b._m00, a._m10 + b._m10, a._m20 + b._m20); }
	static float3x1 operator *(const float3x1& a, const float3x1& b) { return float3x1(a._m00 * b._m00, a._m10 * b._m10, a._m20 * b._m20); }
	static float3x1 operator -(const float3x1& a, const float3x1& b) { return float3x1(a._m00 - b._m00, a._m10 - b._m10, a._m20 - b._m20); }
	static float3x1 operator /(const float3x1& a, const float3x1& b) { return float3x1(a._m00 / b._m00, a._m10 / b._m10, a._m20 / b._m20); }
	static float3x1 operator %(const float3x1& a, const float3x1& b) { return float3x1(fmod(a._m00, b._m00), fmod(a._m10, b._m10), fmod(a._m20, b._m20)); }
	static int3x1 operator ==(const float3x1& a, const float3x1& b) { return int3x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0); }
	static int3x1 operator !=(const float3x1& a, const float3x1& b) { return int3x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0); }
	static int3x1 operator <(const float3x1& a, const float3x1& b) { return int3x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0); }
	static int3x1 operator <=(const float3x1& a, const float3x1& b) { return int3x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0); }
	static int3x1 operator >=(const float3x1& a, const float3x1& b) { return int3x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0); }
	static int3x1 operator >(const float3x1& a, const float3x1& b) { return int3x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0); }

	struct float3x2 {
		float _m00;
		float _m10;
		float _m20;
		float _m01;
		float _m11;
		float _m21;
		float2 operator[] (int row) const {
			if (row == 0) return float2(_m00, _m01);
			if (row == 1) return float2(_m10, _m11);
			if (row == 2) return float2(_m20, _m21);
			return 0; // Silent return ... valid for HLSL
		}
		float3x2(float _m00, float _m01, float _m10, float _m11, float _m20, float _m21) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m20 = _m20;
			this->_m21 = _m21;
		}
		float3x2(float v) :float3x2(v, v, v, v, v, v) {}
		float3x2(const float2& r0, const float2& r1, const float2& r2) : float3x2(r0.x, r0.y, r1.x, r1.y, r2.x, r2.y) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float2x2() const { return float2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator float3x1() const { return float3x1(this->_m00, this->_m10, this->_m20); }
		operator int3x2() const;
		operator uint3x2() const;
	};
	static float3x2 operator -(const float3x2& a) { return float3x2(-a._m00, -a._m01, -a._m10, -a._m11, -a._m20, -a._m21); }
	static float3x2 operator +(const float3x2& a) { return float3x2(+a._m00, +a._m01, +a._m10, +a._m11, +a._m20, +a._m21); }
	static int3x2 operator !(const float3x2& a) { return int3x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0); }
	static float3x2 operator +(const float3x2& a, const float3x2& b) { return float3x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11, a._m20 + b._m20, a._m21 + b._m21); }
	static float3x2 operator *(const float3x2& a, const float3x2& b) { return float3x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11, a._m20 * b._m20, a._m21 * b._m21); }
	static float3x2 operator -(const float3x2& a, const float3x2& b) { return float3x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11, a._m20 - b._m20, a._m21 - b._m21); }
	static float3x2 operator /(const float3x2& a, const float3x2& b) { return float3x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11, a._m20 / b._m20, a._m21 / b._m21); }
	static float3x2 operator %(const float3x2& a, const float3x2& b) { return float3x2(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m20, b._m20), fmod(a._m21, b._m21)); }
	static int3x2 operator ==(const float3x2& a, const float3x2& b) { return int3x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0); }
	static int3x2 operator !=(const float3x2& a, const float3x2& b) { return int3x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0); }
	static int3x2 operator <(const float3x2& a, const float3x2& b) { return int3x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0); }
	static int3x2 operator <=(const float3x2& a, const float3x2& b) { return int3x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0); }
	static int3x2 operator >=(const float3x2& a, const float3x2& b) { return int3x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0); }
	static int3x2 operator >(const float3x2& a, const float3x2& b) { return int3x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0); }

	struct float3x3 {
		float _m00;
		float _m10;
		float _m20;
		float _m01;
		float _m11;
		float _m21;
		float _m02;
		float _m12;
		float _m22;
		float3 operator[] (int row) const {
			if (row == 0) return float3(_m00, _m01, _m02);
			if (row == 1) return float3(_m10, _m11, _m12);
			if (row == 2) return float3(_m20, _m21, _m22);
			return 0; // Silent return ... valid for HLSL
		}
		float3x3(float _m00, float _m01, float _m02, float _m10, float _m11, float _m12, float _m20, float _m21, float _m22) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
		}
		float3x3(float v) :float3x3(v, v, v, v, v, v, v, v, v) {}
		float3x3(const float3& r0, const float3& r1, const float3& r2) : float3x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z, r2.x, r2.y, r2.z) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float1x3() const { return float1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float2x2() const { return float2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator float2x3() const { return float2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator float3x1() const { return float3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator float3x2() const { return float3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		operator int3x3() const;
		operator uint3x3() const;
	};
	static float3x3 operator -(const float3x3& a) { return float3x3(-a._m00, -a._m01, -a._m02, -a._m10, -a._m11, -a._m12, -a._m20, -a._m21, -a._m22); }
	static float3x3 operator +(const float3x3& a) { return float3x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12, +a._m20, +a._m21, +a._m22); }
	static int3x3 operator !(const float3x3& a) { return int3x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0); }
	static float3x3 operator +(const float3x3& a, const float3x3& b) { return float3x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22); }
	static float3x3 operator *(const float3x3& a, const float3x3& b) { return float3x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22); }
	static float3x3 operator -(const float3x3& a, const float3x3& b) { return float3x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22); }
	static float3x3 operator /(const float3x3& a, const float3x3& b) { return float3x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22); }
	static float3x3 operator %(const float3x3& a, const float3x3& b) { return float3x3(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m22, b._m22)); }
	static int3x3 operator ==(const float3x3& a, const float3x3& b) { return int3x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0); }
	static int3x3 operator !=(const float3x3& a, const float3x3& b) { return int3x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0); }
	static int3x3 operator <(const float3x3& a, const float3x3& b) { return int3x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0); }
	static int3x3 operator <=(const float3x3& a, const float3x3& b) { return int3x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0); }
	static int3x3 operator >=(const float3x3& a, const float3x3& b) { return int3x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0); }
	static int3x3 operator >(const float3x3& a, const float3x3& b) { return int3x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0); }

	struct float3x4 {
		float _m00;
		float _m10;
		float _m20;
		float _m01;
		float _m11;
		float _m21;
		float _m02;
		float _m12;
		float _m22;
		float _m03;
		float _m13;
		float _m23;
		float4 operator[] (int row) const {
			if (row == 0) return float4(_m00, _m01, _m02, _m03);
			if (row == 1) return float4(_m10, _m11, _m12, _m13);
			if (row == 2) return float4(_m20, _m21, _m22, _m23);
			return 0; // Silent return ... valid for HLSL
		}
		float3x4(float _m00, float _m01, float _m02, float _m03, float _m10, float _m11, float _m12, float _m13, float _m20, float _m21, float _m22, float _m23) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m23 = _m23;
		}
		float3x4(float v) :float3x4(v, v, v, v, v, v, v, v, v, v, v, v) {}
		float3x4(const float4& r0, const float4& r1, const float4& r2) : float3x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float1x3() const { return float1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator float1x4() const { return float1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float2x2() const { return float2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator float2x3() const { return float2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator float2x4() const { return float2x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13); }
		explicit operator float3x1() const { return float3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator float3x2() const { return float3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator float3x3() const { return float3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		operator int3x4() const;
		operator uint3x4() const;
	};
	static float3x4 operator -(const float3x4& a) { return float3x4(-a._m00, -a._m01, -a._m02, -a._m03, -a._m10, -a._m11, -a._m12, -a._m13, -a._m20, -a._m21, -a._m22, -a._m23); }
	static float3x4 operator +(const float3x4& a) { return float3x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13, +a._m20, +a._m21, +a._m22, +a._m23); }
	static int3x4 operator !(const float3x4& a) { return int3x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m23 == 0 ? 1 : 0); }
	static float3x4 operator +(const float3x4& a, const float3x4& b) { return float3x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m23 + b._m23); }
	static float3x4 operator *(const float3x4& a, const float3x4& b) { return float3x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m23 * b._m23); }
	static float3x4 operator -(const float3x4& a, const float3x4& b) { return float3x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m23 - b._m23); }
	static float3x4 operator /(const float3x4& a, const float3x4& b) { return float3x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m23 / b._m23); }
	static float3x4 operator %(const float3x4& a, const float3x4& b) { return float3x4(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m03, b._m03), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m13, b._m13), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m22, b._m22), fmod(a._m23, b._m23)); }
	static int3x4 operator ==(const float3x4& a, const float3x4& b) { return int3x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m23 == b._m23) ? 1 : 0); }
	static int3x4 operator !=(const float3x4& a, const float3x4& b) { return int3x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m23 != b._m23) ? 1 : 0); }
	static int3x4 operator <(const float3x4& a, const float3x4& b) { return int3x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m23 < b._m23) ? 1 : 0); }
	static int3x4 operator <=(const float3x4& a, const float3x4& b) { return int3x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m23 <= b._m23) ? 1 : 0); }
	static int3x4 operator >=(const float3x4& a, const float3x4& b) { return int3x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m23 >= b._m23) ? 1 : 0); }
	static int3x4 operator >(const float3x4& a, const float3x4& b) { return int3x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m23 > b._m23) ? 1 : 0); }

	struct float4x1 {
		float _m00;
		float _m10;
		float _m20;
		float _m30;
		float1 operator[] (int row) const {
			if (row == 0) return float1(_m00);
			if (row == 1) return float1(_m10);
			if (row == 2) return float1(_m20);
			if (row == 3) return float1(_m30);
			return 0; // Silent return ... valid for HLSL
		}
		float4x1(float _m00, float _m10, float _m20, float _m30) {
			this->_m00 = _m00;
			this->_m10 = _m10;
			this->_m20 = _m20;
			this->_m30 = _m30;
		}
		float4x1(float v) :float4x1(v, v, v, v) {}
		float4x1(const float1& r0, const float1& r1, const float1& r2, const float1& r3) : float4x1(r0.x, r1.x, r2.x, r3.x) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float3x1() const { return float3x1(this->_m00, this->_m10, this->_m20); }
		operator int4x1() const;
		operator uint4x1() const;
	};
	static float4x1 operator -(const float4x1& a) { return float4x1(-a._m00, -a._m10, -a._m20, -a._m30); }
	static float4x1 operator +(const float4x1& a) { return float4x1(+a._m00, +a._m10, +a._m20, +a._m30); }
	static int4x1 operator !(const float4x1& a) { return int4x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0); }
	static float4x1 operator +(const float4x1& a, const float4x1& b) { return float4x1(a._m00 + b._m00, a._m10 + b._m10, a._m20 + b._m20, a._m30 + b._m30); }
	static float4x1 operator *(const float4x1& a, const float4x1& b) { return float4x1(a._m00 * b._m00, a._m10 * b._m10, a._m20 * b._m20, a._m30 * b._m30); }
	static float4x1 operator -(const float4x1& a, const float4x1& b) { return float4x1(a._m00 - b._m00, a._m10 - b._m10, a._m20 - b._m20, a._m30 - b._m30); }
	static float4x1 operator /(const float4x1& a, const float4x1& b) { return float4x1(a._m00 / b._m00, a._m10 / b._m10, a._m20 / b._m20, a._m30 / b._m30); }
	static float4x1 operator %(const float4x1& a, const float4x1& b) { return float4x1(fmod(a._m00, b._m00), fmod(a._m10, b._m10), fmod(a._m20, b._m20), fmod(a._m30, b._m30)); }
	static int4x1 operator ==(const float4x1& a, const float4x1& b) { return int4x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0); }
	static int4x1 operator !=(const float4x1& a, const float4x1& b) { return int4x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0); }
	static int4x1 operator <(const float4x1& a, const float4x1& b) { return int4x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0); }
	static int4x1 operator <=(const float4x1& a, const float4x1& b) { return int4x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0); }
	static int4x1 operator >=(const float4x1& a, const float4x1& b) { return int4x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0); }
	static int4x1 operator >(const float4x1& a, const float4x1& b) { return int4x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0); }

	struct float4x2 {
		float _m00;
		float _m10;
		float _m20;
		float _m30;
		float _m01;
		float _m11;
		float _m21;
		float _m31;
		float2 operator[] (int row) const {
			if (row == 0) return float2(_m00, _m01);
			if (row == 1) return float2(_m10, _m11);
			if (row == 2) return float2(_m20, _m21);
			if (row == 3) return float2(_m30, _m31);
			return 0; // Silent return ... valid for HLSL
		}
		float4x2(float _m00, float _m01, float _m10, float _m11, float _m20, float _m21, float _m30, float _m31) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m30 = _m30;
			this->_m31 = _m31;
		}
		float4x2(float v) :float4x2(v, v, v, v, v, v, v, v) {}
		float4x2(const float2& r0, const float2& r1, const float2& r2, const float2& r3) : float4x2(r0.x, r0.y, r1.x, r1.y, r2.x, r2.y, r3.x, r3.y) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float2x2() const { return float2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator float3x1() const { return float3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator float3x2() const { return float3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator float4x1() const { return float4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		operator int4x2() const;
		operator uint4x2() const;
	};
	static float4x2 operator -(const float4x2& a) { return float4x2(-a._m00, -a._m01, -a._m10, -a._m11, -a._m20, -a._m21, -a._m30, -a._m31); }
	static float4x2 operator +(const float4x2& a) { return float4x2(+a._m00, +a._m01, +a._m10, +a._m11, +a._m20, +a._m21, +a._m30, +a._m31); }
	static int4x2 operator !(const float4x2& a) { return int4x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0); }
	static float4x2 operator +(const float4x2& a, const float4x2& b) { return float4x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11, a._m20 + b._m20, a._m21 + b._m21, a._m30 + b._m30, a._m31 + b._m31); }
	static float4x2 operator *(const float4x2& a, const float4x2& b) { return float4x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11, a._m20 * b._m20, a._m21 * b._m21, a._m30 * b._m30, a._m31 * b._m31); }
	static float4x2 operator -(const float4x2& a, const float4x2& b) { return float4x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11, a._m20 - b._m20, a._m21 - b._m21, a._m30 - b._m30, a._m31 - b._m31); }
	static float4x2 operator /(const float4x2& a, const float4x2& b) { return float4x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11, a._m20 / b._m20, a._m21 / b._m21, a._m30 / b._m30, a._m31 / b._m31); }
	static float4x2 operator %(const float4x2& a, const float4x2& b) { return float4x2(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m30, b._m30), fmod(a._m31, b._m31)); }
	static int4x2 operator ==(const float4x2& a, const float4x2& b) { return int4x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0); }
	static int4x2 operator !=(const float4x2& a, const float4x2& b) { return int4x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0); }
	static int4x2 operator <(const float4x2& a, const float4x2& b) { return int4x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0); }
	static int4x2 operator <=(const float4x2& a, const float4x2& b) { return int4x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0); }
	static int4x2 operator >=(const float4x2& a, const float4x2& b) { return int4x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0); }
	static int4x2 operator >(const float4x2& a, const float4x2& b) { return int4x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0); }

	struct float4x3 {
		float _m00;
		float _m10;
		float _m20;
		float _m30;
		float _m01;
		float _m11;
		float _m21;
		float _m31;
		float _m02;
		float _m12;
		float _m22;
		float _m32;
		float3 operator[] (int row) const {
			if (row == 0) return float3(_m00, _m01, _m02);
			if (row == 1) return float3(_m10, _m11, _m12);
			if (row == 2) return float3(_m20, _m21, _m22);
			if (row == 3) return float3(_m30, _m31, _m32);
			return 0; // Silent return ... valid for HLSL
		}
		float4x3() = default;
		float4x3(float _m00, float _m01, float _m02, float _m10, float _m11, float _m12, float _m20, float _m21, float _m22, float _m30, float _m31, float _m32) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m30 = _m30;
			this->_m31 = _m31;
			this->_m32 = _m32;
		}
		float4x3(float v) :float4x3(v, v, v, v, v, v, v, v, v, v, v, v) {}
		float4x3(const float3& r0, const float3& r1, const float3& r2, const float3& r3) : float4x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z, r2.x, r2.y, r2.z, r3.x, r3.y, r3.z) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float1x3() const { return float1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float2x2() const { return float2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator float2x3() const { return float2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator float3x1() const { return float3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator float3x2() const { return float3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator float3x3() const { return float3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		explicit operator float4x1() const { return float4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		explicit operator float4x2() const { return float4x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21, this->_m30, this->_m31); }
		operator int4x3() const;
		operator uint4x3() const;
	};
	static float4x3 operator -(const float4x3& a) { return float4x3(-a._m00, -a._m01, -a._m02, -a._m10, -a._m11, -a._m12, -a._m20, -a._m21, -a._m22, -a._m30, -a._m31, -a._m32); }
	static float4x3 operator +(const float4x3& a) { return float4x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12, +a._m20, +a._m21, +a._m22, +a._m30, +a._m31, +a._m32); }
	static int4x3 operator !(const float4x3& a) { return int4x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0, a._m32 == 0 ? 1 : 0); }
	static float4x3 operator +(const float4x3& a, const float4x3& b) { return float4x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m30 + b._m30, a._m31 + b._m31, a._m32 + b._m32); }
	static float4x3 operator *(const float4x3& a, const float4x3& b) { return float4x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m30 * b._m30, a._m31 * b._m31, a._m32 * b._m32); }
	static float4x3 operator -(const float4x3& a, const float4x3& b) { return float4x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m30 - b._m30, a._m31 - b._m31, a._m32 - b._m32); }
	static float4x3 operator /(const float4x3& a, const float4x3& b) { return float4x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m30 / b._m30, a._m31 / b._m31, a._m32 / b._m32); }
	static float4x3 operator %(const float4x3& a, const float4x3& b) { return float4x3(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m22, b._m22), fmod(a._m30, b._m30), fmod(a._m31, b._m31), fmod(a._m32, b._m32)); }
	static int4x3 operator ==(const float4x3& a, const float4x3& b) { return int4x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0, (a._m32 == b._m32) ? 1 : 0); }
	static int4x3 operator !=(const float4x3& a, const float4x3& b) { return int4x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0, (a._m32 != b._m32) ? 1 : 0); }
	static int4x3 operator <(const float4x3& a, const float4x3& b) { return int4x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0, (a._m32 < b._m32) ? 1 : 0); }
	static int4x3 operator <=(const float4x3& a, const float4x3& b) { return int4x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0, (a._m32 <= b._m32) ? 1 : 0); }
	static int4x3 operator >=(const float4x3& a, const float4x3& b) { return int4x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0, (a._m32 >= b._m32) ? 1 : 0); }
	static int4x3 operator >(const float4x3& a, const float4x3& b) { return int4x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0, (a._m32 > b._m32) ? 1 : 0); }

	struct float4x4 {
		float _m00;
		float _m10;
		float _m20;
		float _m30;
		float _m01;
		float _m11;
		float _m21;
		float _m31;
		float _m02;
		float _m12;
		float _m22;
		float _m32;
		float _m03;
		float _m13;
		float _m23;
		float _m33;
		float4 operator[] (int row) const {
			if (row == 0) return float4(_m00, _m01, _m02, _m03);
			if (row == 1) return float4(_m10, _m11, _m12, _m13);
			if (row == 2) return float4(_m20, _m21, _m22, _m23);
			if (row == 3) return float4(_m30, _m31, _m32, _m33);
			return 0; // Silent return ... valid for HLSL
		}
		float4x4() = default;
		float4x4(float _m00, float _m01, float _m02, float _m03, float _m10, float _m11, float _m12, float _m13, float _m20, float _m21, float _m22, float _m23, float _m30, float _m31, float _m32, float _m33) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m23 = _m23;
			this->_m30 = _m30;
			this->_m31 = _m31;
			this->_m32 = _m32;
			this->_m33 = _m33;
		}
		float4x4(float v) :float4x4(v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v) {}
		float4x4(const float4& r0, const float4& r1, const float4& r2, const float4& r3) : float4x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w, r3.x, r3.y, r3.z, r3.w) { }
		explicit operator float1x1() const { return float1x1(this->_m00); }
		explicit operator float1x2() const { return float1x2(this->_m00, this->_m01); }
		explicit operator float1x3() const { return float1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator float1x4() const { return float1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator float2x1() const { return float2x1(this->_m00, this->_m10); }
		explicit operator float2x2() const { return float2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator float2x3() const { return float2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator float2x4() const { return float2x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13); }
		explicit operator float3x1() const { return float3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator float3x2() const { return float3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator float3x3() const { return float3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		explicit operator float3x4() const { return float3x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13, this->_m20, this->_m21, this->_m22, this->_m23); }
		explicit operator float4x1() const { return float4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		explicit operator float4x2() const { return float4x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21, this->_m30, this->_m31); }
		explicit operator float4x3() const { return float4x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22, this->_m30, this->_m31, this->_m32); }
		operator int4x4() const;
		operator uint4x4() const;
	};
	static float4x4 operator -(const float4x4& a) { return float4x4(-a._m00, -a._m01, -a._m02, -a._m03, -a._m10, -a._m11, -a._m12, -a._m13, -a._m20, -a._m21, -a._m22, -a._m23, -a._m30, -a._m31, -a._m32, -a._m33); }
	static float4x4 operator +(const float4x4& a) { return float4x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13, +a._m20, +a._m21, +a._m22, +a._m23, +a._m30, +a._m31, +a._m32, +a._m33); }
	static int4x4 operator !(const float4x4& a) { return int4x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m23 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0, a._m32 == 0 ? 1 : 0, a._m33 == 0 ? 1 : 0); }
	static float4x4 operator +(const float4x4& a, const float4x4& b) { return float4x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m23 + b._m23, a._m30 + b._m30, a._m31 + b._m31, a._m32 + b._m32, a._m33 + b._m33); }
	static float4x4 operator *(const float4x4& a, const float4x4& b) { return float4x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m23 * b._m23, a._m30 * b._m30, a._m31 * b._m31, a._m32 * b._m32, a._m33 * b._m33); }
	static float4x4 operator -(const float4x4& a, const float4x4& b) { return float4x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m23 - b._m23, a._m30 - b._m30, a._m31 - b._m31, a._m32 - b._m32, a._m33 - b._m33); }
	static float4x4 operator /(const float4x4& a, const float4x4& b) { return float4x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m23 / b._m23, a._m30 / b._m30, a._m31 / b._m31, a._m32 / b._m32, a._m33 / b._m33); }
	static float4x4 operator %(const float4x4& a, const float4x4& b) { return float4x4(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m03, b._m03), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m13, b._m13), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m22, b._m22), fmod(a._m23, b._m23), fmod(a._m30, b._m30), fmod(a._m31, b._m31), fmod(a._m32, b._m32), fmod(a._m33, b._m33)); }
	static int4x4 operator ==(const float4x4& a, const float4x4& b) { return int4x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m23 == b._m23) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0, (a._m32 == b._m32) ? 1 : 0, (a._m33 == b._m33) ? 1 : 0); }
	static int4x4 operator !=(const float4x4& a, const float4x4& b) { return int4x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m23 != b._m23) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0, (a._m32 != b._m32) ? 1 : 0, (a._m33 != b._m33) ? 1 : 0); }
	static int4x4 operator <(const float4x4& a, const float4x4& b) { return int4x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m23 < b._m23) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0, (a._m32 < b._m32) ? 1 : 0, (a._m33 < b._m33) ? 1 : 0); }
	static int4x4 operator <=(const float4x4& a, const float4x4& b) { return int4x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m23 <= b._m23) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0, (a._m32 <= b._m32) ? 1 : 0, (a._m33 <= b._m33) ? 1 : 0); }
	static int4x4 operator >=(const float4x4& a, const float4x4& b) { return int4x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m23 >= b._m23) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0, (a._m32 >= b._m32) ? 1 : 0, (a._m33 >= b._m33) ? 1 : 0); }
	static int4x4 operator >(const float4x4& a, const float4x4& b) { return int4x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m23 > b._m23) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0, (a._m32 > b._m32) ? 1 : 0, (a._m33 > b._m33) ? 1 : 0); }

	struct uint1 {
	private:
		static uint __TRASH;
	public:
		uint x;
		uint& operator[](int idx) {
			if (idx == 0) return this->x;
			return __TRASH; // Silent return ... valid for HLSL
		}
		uint1(uint x) {
			this->x = x;
		}
		operator float1() const;
		operator int1() const;
	};
	static uint1 operator +(const uint1& a) { return uint1(+a.x); }
	static int1 operator !(const uint1& a) { return int1(a.x == 0 ? 1 : 0); }
	static int1 operator ==(const uint1& a, const uint1& b) { return int1((a.x == b.x) ? 1 : 0); }
	static int1 operator !=(const uint1& a, const uint1& b) { return int1((a.x != b.x) ? 1 : 0); }
	static int1 operator <(const uint1& a, const uint1& b) { return int1((a.x < b.x) ? 1 : 0); }
	static int1 operator <=(const uint1& a, const uint1& b) { return int1((a.x <= b.x) ? 1 : 0); }
	static int1 operator >=(const uint1& a, const uint1& b) { return int1((a.x >= b.x) ? 1 : 0); }
	static int1 operator >(const uint1& a, const uint1& b) { return int1((a.x > b.x) ? 1 : 0); }
	static uint1 operator +(const uint1& a, const uint1& b) { return uint1(a.x + b.x); }
	static uint1 operator *(const uint1& a, const uint1& b) { return uint1(a.x * b.x); }
	static uint1 operator -(const uint1& a, const uint1& b) { return uint1(a.x - b.x); }
	static uint1 operator /(const uint1& a, const uint1& b) { return uint1(a.x / b.x); }
	static uint1 operator %(const uint1& a, const uint1& b) { return uint1(a.x % b.x); }

	struct uint2 {
	private:
		static uint __TRASH;
	public:
		uint x;
		uint y;
		uint& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			return __TRASH; // Silent return ... valid for HLSL
		}
		uint2(uint x, uint y) {
			this->x = x;
			this->y = y;
		}
		uint2(uint v) :uint2(v, v) {}
		operator uint1() const { return uint1(this->x); }
		operator float2() const;
		operator int2() const;
	};
	static uint2 operator +(const uint2& a) { return uint2(+a.x, +a.y); }
	static int2 operator !(const uint2& a) { return int2(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0); }
	static int2 operator ==(const uint2& a, const uint2& b) { return int2((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0); }
	static int2 operator !=(const uint2& a, const uint2& b) { return int2((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0); }
	static int2 operator <(const uint2& a, const uint2& b) { return int2((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0); }
	static int2 operator <=(const uint2& a, const uint2& b) { return int2((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0); }
	static int2 operator >=(const uint2& a, const uint2& b) { return int2((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0); }
	static int2 operator >(const uint2& a, const uint2& b) { return int2((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0); }
	static uint2 operator +(const uint2& a, const uint2& b) { return uint2(a.x + b.x, a.y + b.y); }
	static uint2 operator *(const uint2& a, const uint2& b) { return uint2(a.x * b.x, a.y * b.y); }
	static uint2 operator -(const uint2& a, const uint2& b) { return uint2(a.x - b.x, a.y - b.y); }
	static uint2 operator /(const uint2& a, const uint2& b) { return uint2(a.x / b.x, a.y / b.y); }
	static uint2 operator %(const uint2& a, const uint2& b) { return uint2(a.x % b.x, a.y % b.y); }

	struct uint3 {
	private:
		static uint __TRASH;
	public:
		uint x;
		uint y;
		uint z;
		uint2 get_xy() { return uint2(this->x, this->y); }
		uint& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			if (idx == 2) return this->z;
			return __TRASH; // Silent return ... valid for HLSL
		}
		uint3(uint x, uint y, uint z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
		uint3(uint v) :uint3(v, v, v) {}
		operator uint1() const { return uint1(this->x); }
		operator uint2() const { return uint2(this->x, this->y); }
		operator float3() const;
		operator int3() const;
	};
	static uint3 operator +(const uint3& a) { return uint3(+a.x, +a.y, +a.z); }
	static int3 operator !(const uint3& a) { return int3(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0, a.z == 0 ? 1 : 0); }
	static int3 operator ==(const uint3& a, const uint3& b) { return int3((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0, (a.z == b.z) ? 1 : 0); }
	static int3 operator !=(const uint3& a, const uint3& b) { return int3((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0, (a.z != b.z) ? 1 : 0); }
	static int3 operator <(const uint3& a, const uint3& b) { return int3((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0, (a.z < b.z) ? 1 : 0); }
	static int3 operator <=(const uint3& a, const uint3& b) { return int3((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0, (a.z <= b.z) ? 1 : 0); }
	static int3 operator >=(const uint3& a, const uint3& b) { return int3((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0, (a.z >= b.z) ? 1 : 0); }
	static int3 operator >(const uint3& a, const uint3& b) { return int3((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0, (a.z > b.z) ? 1 : 0); }
	static uint3 operator +(const uint3& a, const uint3& b) { return uint3(a.x + b.x, a.y + b.y, a.z + b.z); }
	static uint3 operator *(const uint3& a, const uint3& b) { return uint3(a.x * b.x, a.y * b.y, a.z * b.z); }
	static uint3 operator -(const uint3& a, const uint3& b) { return uint3(a.x - b.x, a.y - b.y, a.z - b.z); }
	static uint3 operator /(const uint3& a, const uint3& b) { return uint3(a.x / b.x, a.y / b.y, a.z / b.z); }
	static uint3 operator %(const uint3& a, const uint3& b) { return uint3(a.x % b.x, a.y % b.y, a.z % b.z); }

	struct uint4 {
	private:
		static uint __TRASH;
	public:
		uint x;
		uint y;
		uint z;
		uint w;
		uint2 get_xy() { return uint2(this->x, this->y); }
		uint3 get_xyz() { return uint3(this->x, this->y, this->z); }
		uint& operator[](int idx) {
			if (idx == 0) return this->x;
			if (idx == 1) return this->y;
			if (idx == 2) return this->z;
			if (idx == 3) return this->w;
			return __TRASH; // Silent return ... valid for HLSL
		}
		uint4(uint x, uint y, uint z, uint w) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
		uint4(uint v) :uint4(v, v, v, v) {}
		operator uint1() const { return uint1(this->x); }
		operator uint2() const { return uint2(this->x, this->y); }
		operator uint3() const { return uint3(this->x, this->y, this->z); }
		operator float4() const;
		operator int4() const;
	};
	static uint4 operator +(const uint4& a) { return uint4(+a.x, +a.y, +a.z, +a.w); }
	static int4 operator !(const uint4& a) { return int4(a.x == 0 ? 1 : 0, a.y == 0 ? 1 : 0, a.z == 0 ? 1 : 0, a.w == 0 ? 1 : 0); }
	static int4 operator ==(const uint4& a, const uint4& b) { return int4((a.x == b.x) ? 1 : 0, (a.y == b.y) ? 1 : 0, (a.z == b.z) ? 1 : 0, (a.w == b.w) ? 1 : 0); }
	static int4 operator !=(const uint4& a, const uint4& b) { return int4((a.x != b.x) ? 1 : 0, (a.y != b.y) ? 1 : 0, (a.z != b.z) ? 1 : 0, (a.w != b.w) ? 1 : 0); }
	static int4 operator <(const uint4& a, const uint4& b) { return int4((a.x < b.x) ? 1 : 0, (a.y < b.y) ? 1 : 0, (a.z < b.z) ? 1 : 0, (a.w < b.w) ? 1 : 0); }
	static int4 operator <=(const uint4& a, const uint4& b) { return int4((a.x <= b.x) ? 1 : 0, (a.y <= b.y) ? 1 : 0, (a.z <= b.z) ? 1 : 0, (a.w <= b.w) ? 1 : 0); }
	static int4 operator >=(const uint4& a, const uint4& b) { return int4((a.x >= b.x) ? 1 : 0, (a.y >= b.y) ? 1 : 0, (a.z >= b.z) ? 1 : 0, (a.w >= b.w) ? 1 : 0); }
	static int4 operator >(const uint4& a, const uint4& b) { return int4((a.x > b.x) ? 1 : 0, (a.y > b.y) ? 1 : 0, (a.z > b.z) ? 1 : 0, (a.w > b.w) ? 1 : 0); }
	static uint4 operator +(const uint4& a, const uint4& b) { return uint4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	static uint4 operator *(const uint4& a, const uint4& b) { return uint4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
	static uint4 operator -(const uint4& a, const uint4& b) { return uint4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
	static uint4 operator /(const uint4& a, const uint4& b) { return uint4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
	static uint4 operator %(const uint4& a, const uint4& b) { return uint4(a.x % b.x, a.y % b.y, a.z % b.z, a.w % b.w); }

	struct uint1x1 {
		uint _m00;
		uint1 operator[] (int row) const {
			if (row == 0) return uint1(_m00);
			return 0; // Silent return ... valid for HLSL
		}
		uint1x1(uint _m00) {
			this->_m00 = _m00;
		}
		operator uint1() const { return uint1(this->_m00); }
		uint1x1(const uint1& v) : uint1x1(v.x) { }
		operator float1x1() const;
		operator int1x1() const;
	};
	static uint1x1 operator +(const uint1x1& a) { return uint1x1(+a._m00); }
	static int1x1 operator !(const uint1x1& a) { return int1x1(a._m00 == 0 ? 1 : 0); }
	static uint1x1 operator +(const uint1x1& a, const uint1x1& b) { return uint1x1(a._m00 + b._m00); }
	static uint1x1 operator *(const uint1x1& a, const uint1x1& b) { return uint1x1(a._m00 * b._m00); }
	static uint1x1 operator -(const uint1x1& a, const uint1x1& b) { return uint1x1(a._m00 - b._m00); }
	static uint1x1 operator /(const uint1x1& a, const uint1x1& b) { return uint1x1(a._m00 / b._m00); }
	static uint1x1 operator %(const uint1x1& a, const uint1x1& b) { return uint1x1(a._m00 % b._m00); }
	static int1x1 operator ==(const uint1x1& a, const uint1x1& b) { return int1x1((a._m00 == b._m00) ? 1 : 0); }
	static int1x1 operator !=(const uint1x1& a, const uint1x1& b) { return int1x1((a._m00 != b._m00) ? 1 : 0); }
	static int1x1 operator <(const uint1x1& a, const uint1x1& b) { return int1x1((a._m00 < b._m00) ? 1 : 0); }
	static int1x1 operator <=(const uint1x1& a, const uint1x1& b) { return int1x1((a._m00 <= b._m00) ? 1 : 0); }
	static int1x1 operator >=(const uint1x1& a, const uint1x1& b) { return int1x1((a._m00 >= b._m00) ? 1 : 0); }
	static int1x1 operator >(const uint1x1& a, const uint1x1& b) { return int1x1((a._m00 > b._m00) ? 1 : 0); }

	struct uint1x2 {
		uint _m00;
		uint _m01;
		uint2 operator[] (int row) const {
			if (row == 0) return uint2(_m00, _m01);
			return 0; // Silent return ... valid for HLSL
		}
		uint1x2(uint _m00, uint _m01) {
			this->_m00 = _m00;
			this->_m01 = _m01;
		}
		uint1x2(uint v) :uint1x2(v, v) {}
		operator uint2() const { return uint2(this->_m00, this->_m01); }
		uint1x2(const uint2& v) : uint1x2(v.x, v.y) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		operator float1x2() const;
		operator int1x2() const;
	};
	static uint1x2 operator +(const uint1x2& a) { return uint1x2(+a._m00, +a._m01); }
	static int1x2 operator !(const uint1x2& a) { return int1x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0); }
	static uint1x2 operator +(const uint1x2& a, const uint1x2& b) { return uint1x2(a._m00 + b._m00, a._m01 + b._m01); }
	static uint1x2 operator *(const uint1x2& a, const uint1x2& b) { return uint1x2(a._m00 * b._m00, a._m01 * b._m01); }
	static uint1x2 operator -(const uint1x2& a, const uint1x2& b) { return uint1x2(a._m00 - b._m00, a._m01 - b._m01); }
	static uint1x2 operator /(const uint1x2& a, const uint1x2& b) { return uint1x2(a._m00 / b._m00, a._m01 / b._m01); }
	static uint1x2 operator %(const uint1x2& a, const uint1x2& b) { return uint1x2(a._m00 % b._m00, a._m01 % b._m01); }
	static int1x2 operator ==(const uint1x2& a, const uint1x2& b) { return int1x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0); }
	static int1x2 operator !=(const uint1x2& a, const uint1x2& b) { return int1x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0); }
	static int1x2 operator <(const uint1x2& a, const uint1x2& b) { return int1x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0); }
	static int1x2 operator <=(const uint1x2& a, const uint1x2& b) { return int1x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0); }
	static int1x2 operator >=(const uint1x2& a, const uint1x2& b) { return int1x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0); }
	static int1x2 operator >(const uint1x2& a, const uint1x2& b) { return int1x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0); }

	struct uint1x3 {
		uint _m00;
		uint _m01;
		uint _m02;
		uint3 operator[] (int row) const {
			if (row == 0) return uint3(_m00, _m01, _m02);
			return 0; // Silent return ... valid for HLSL
		}
		uint1x3(uint _m00, uint _m01, uint _m02) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
		}
		uint1x3(uint v) :uint1x3(v, v, v) {}
		operator uint3() const { return uint3(this->_m00, this->_m01, this->_m02); }
		uint1x3(const uint3& v) : uint1x3(v.x, v.y, v.z) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		operator float1x3() const;
		operator int1x3() const;
	};
	static uint1x3 operator +(const uint1x3& a) { return uint1x3(+a._m00, +a._m01, +a._m02); }
	static int1x3 operator !(const uint1x3& a) { return int1x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0); }
	static uint1x3 operator +(const uint1x3& a, const uint1x3& b) { return uint1x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02); }
	static uint1x3 operator *(const uint1x3& a, const uint1x3& b) { return uint1x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02); }
	static uint1x3 operator -(const uint1x3& a, const uint1x3& b) { return uint1x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02); }
	static uint1x3 operator /(const uint1x3& a, const uint1x3& b) { return uint1x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02); }
	static uint1x3 operator %(const uint1x3& a, const uint1x3& b) { return uint1x3(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02); }
	static int1x3 operator ==(const uint1x3& a, const uint1x3& b) { return int1x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0); }
	static int1x3 operator !=(const uint1x3& a, const uint1x3& b) { return int1x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0); }
	static int1x3 operator <(const uint1x3& a, const uint1x3& b) { return int1x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0); }
	static int1x3 operator <=(const uint1x3& a, const uint1x3& b) { return int1x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0); }
	static int1x3 operator >=(const uint1x3& a, const uint1x3& b) { return int1x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0); }
	static int1x3 operator >(const uint1x3& a, const uint1x3& b) { return int1x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0); }

	struct uint1x4 {
		uint _m00;
		uint _m01;
		uint _m02;
		uint _m03;
		uint4 operator[] (int row) const {
			if (row == 0) return uint4(_m00, _m01, _m02, _m03);
			return 0; // Silent return ... valid for HLSL
		}
		uint1x4(uint _m00, uint _m01, uint _m02, uint _m03) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
		}
		uint1x4(uint v) :uint1x4(v, v, v, v) {}
		operator uint4() const { return uint4(this->_m00, this->_m01, this->_m02, this->_m03); }
		uint1x4(const uint4& v) : uint1x4(v.x, v.y, v.z, v.w) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint1x3() const { return uint1x3(this->_m00, this->_m01, this->_m02); }
		operator float1x4() const;
		operator int1x4() const;
	};
	static uint1x4 operator +(const uint1x4& a) { return uint1x4(+a._m00, +a._m01, +a._m02, +a._m03); }
	static int1x4 operator !(const uint1x4& a) { return int1x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0); }
	static uint1x4 operator +(const uint1x4& a, const uint1x4& b) { return uint1x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03); }
	static uint1x4 operator *(const uint1x4& a, const uint1x4& b) { return uint1x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03); }
	static uint1x4 operator -(const uint1x4& a, const uint1x4& b) { return uint1x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03); }
	static uint1x4 operator /(const uint1x4& a, const uint1x4& b) { return uint1x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03); }
	static uint1x4 operator %(const uint1x4& a, const uint1x4& b) { return uint1x4(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m03 % b._m03); }
	static int1x4 operator ==(const uint1x4& a, const uint1x4& b) { return int1x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0); }
	static int1x4 operator !=(const uint1x4& a, const uint1x4& b) { return int1x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0); }
	static int1x4 operator <(const uint1x4& a, const uint1x4& b) { return int1x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0); }
	static int1x4 operator <=(const uint1x4& a, const uint1x4& b) { return int1x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0); }
	static int1x4 operator >=(const uint1x4& a, const uint1x4& b) { return int1x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0); }
	static int1x4 operator >(const uint1x4& a, const uint1x4& b) { return int1x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0); }

	struct uint2x1 {
		uint _m00;
		uint _m10;
		uint1 operator[] (int row) const {
			if (row == 0) return uint1(_m00);
			if (row == 1) return uint1(_m10);
			return 0; // Silent return ... valid for HLSL
		}
		uint2x1(uint _m00, uint _m10) {
			this->_m00 = _m00;
			this->_m10 = _m10;
		}
		uint2x1(uint v) :uint2x1(v, v) {}
		uint2x1(const uint1& r0, const uint1& r1) : uint2x1(r0.x, r1.x) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		operator float2x1() const;
		operator int2x1() const;
	};
	static uint2x1 operator +(const uint2x1& a) { return uint2x1(+a._m00, +a._m10); }
	static int2x1 operator !(const uint2x1& a) { return int2x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0); }
	static uint2x1 operator +(const uint2x1& a, const uint2x1& b) { return uint2x1(a._m00 + b._m00, a._m10 + b._m10); }
	static uint2x1 operator *(const uint2x1& a, const uint2x1& b) { return uint2x1(a._m00 * b._m00, a._m10 * b._m10); }
	static uint2x1 operator -(const uint2x1& a, const uint2x1& b) { return uint2x1(a._m00 - b._m00, a._m10 - b._m10); }
	static uint2x1 operator /(const uint2x1& a, const uint2x1& b) { return uint2x1(a._m00 / b._m00, a._m10 / b._m10); }
	static uint2x1 operator %(const uint2x1& a, const uint2x1& b) { return uint2x1(a._m00 % b._m00, a._m10 % b._m10); }
	static int2x1 operator ==(const uint2x1& a, const uint2x1& b) { return int2x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0); }
	static int2x1 operator !=(const uint2x1& a, const uint2x1& b) { return int2x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0); }
	static int2x1 operator <(const uint2x1& a, const uint2x1& b) { return int2x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0); }
	static int2x1 operator <=(const uint2x1& a, const uint2x1& b) { return int2x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0); }
	static int2x1 operator >=(const uint2x1& a, const uint2x1& b) { return int2x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0); }
	static int2x1 operator >(const uint2x1& a, const uint2x1& b) { return int2x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0); }

	struct uint2x2 {
		uint _m00;
		uint _m10;
		uint _m01;
		uint _m11;
		uint2 operator[] (int row) const {
			if (row == 0) return uint2(_m00, _m01);
			if (row == 1) return uint2(_m10, _m11);
			return 0; // Silent return ... valid for HLSL
		}
		uint2x2(uint _m00, uint _m01, uint _m10, uint _m11) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
		}
		uint2x2(uint v) :uint2x2(v, v, v, v) {}
		uint2x2(const uint2& r0, const uint2& r1) : uint2x2(r0.x, r0.y, r1.x, r1.y) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		operator float2x2() const;
		operator int2x2() const;
	};
	static uint2x2 operator +(const uint2x2& a) { return uint2x2(+a._m00, +a._m01, +a._m10, +a._m11); }
	static int2x2 operator !(const uint2x2& a) { return int2x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0); }
	static uint2x2 operator +(const uint2x2& a, const uint2x2& b) { return uint2x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11); }
	static uint2x2 operator *(const uint2x2& a, const uint2x2& b) { return uint2x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11); }
	static uint2x2 operator -(const uint2x2& a, const uint2x2& b) { return uint2x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11); }
	static uint2x2 operator /(const uint2x2& a, const uint2x2& b) { return uint2x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11); }
	static uint2x2 operator %(const uint2x2& a, const uint2x2& b) { return uint2x2(a._m00 % b._m00, a._m01 % b._m01, a._m10 % b._m10, a._m11 % b._m11); }
	static int2x2 operator ==(const uint2x2& a, const uint2x2& b) { return int2x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0); }
	static int2x2 operator !=(const uint2x2& a, const uint2x2& b) { return int2x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0); }
	static int2x2 operator <(const uint2x2& a, const uint2x2& b) { return int2x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0); }
	static int2x2 operator <=(const uint2x2& a, const uint2x2& b) { return int2x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0); }
	static int2x2 operator >=(const uint2x2& a, const uint2x2& b) { return int2x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0); }
	static int2x2 operator >(const uint2x2& a, const uint2x2& b) { return int2x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0); }

	struct uint2x3 {
		uint _m00;
		uint _m10;
		uint _m01;
		uint _m11;
		uint _m02;
		uint _m12;
		uint3 operator[] (int row) const {
			if (row == 0) return uint3(_m00, _m01, _m02);
			if (row == 1) return uint3(_m10, _m11, _m12);
			return 0; // Silent return ... valid for HLSL
		}
		uint2x3(uint _m00, uint _m01, uint _m02, uint _m10, uint _m11, uint _m12) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
		}
		uint2x3(uint v) :uint2x3(v, v, v, v, v, v) {}
		uint2x3(const uint3& r0, const uint3& r1) : uint2x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint1x3() const { return uint1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint2x2() const { return uint2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		operator float2x3() const;
		operator int2x3() const;
	};
	static uint2x3 operator +(const uint2x3& a) { return uint2x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12); }
	static int2x3 operator !(const uint2x3& a) { return int2x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0); }
	static uint2x3 operator +(const uint2x3& a, const uint2x3& b) { return uint2x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12); }
	static uint2x3 operator *(const uint2x3& a, const uint2x3& b) { return uint2x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12); }
	static uint2x3 operator -(const uint2x3& a, const uint2x3& b) { return uint2x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12); }
	static uint2x3 operator /(const uint2x3& a, const uint2x3& b) { return uint2x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12); }
	static uint2x3 operator %(const uint2x3& a, const uint2x3& b) { return uint2x3(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12); }
	static int2x3 operator ==(const uint2x3& a, const uint2x3& b) { return int2x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0); }
	static int2x3 operator !=(const uint2x3& a, const uint2x3& b) { return int2x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0); }
	static int2x3 operator <(const uint2x3& a, const uint2x3& b) { return int2x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0); }
	static int2x3 operator <=(const uint2x3& a, const uint2x3& b) { return int2x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0); }
	static int2x3 operator >=(const uint2x3& a, const uint2x3& b) { return int2x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0); }
	static int2x3 operator >(const uint2x3& a, const uint2x3& b) { return int2x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0); }

	struct uint2x4 {
		uint _m00;
		uint _m10;
		uint _m01;
		uint _m11;
		uint _m02;
		uint _m12;
		uint _m03;
		uint _m13;
		uint4 operator[] (int row) const {
			if (row == 0) return uint4(_m00, _m01, _m02, _m03);
			if (row == 1) return uint4(_m10, _m11, _m12, _m13);
			return 0; // Silent return ... valid for HLSL
		}
		uint2x4(uint _m00, uint _m01, uint _m02, uint _m03, uint _m10, uint _m11, uint _m12, uint _m13) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
		}
		uint2x4(uint v) :uint2x4(v, v, v, v, v, v, v, v) {}
		uint2x4(const uint4& r0, const uint4& r1) : uint2x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint1x3() const { return uint1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator uint1x4() const { return uint1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint2x2() const { return uint2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator uint2x3() const { return uint2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		operator float2x4() const;
		operator int2x4() const;
	};
	static uint2x4 operator +(const uint2x4& a) { return uint2x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13); }
	static int2x4 operator !(const uint2x4& a) { return int2x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0); }
	static uint2x4 operator +(const uint2x4& a, const uint2x4& b) { return uint2x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13); }
	static uint2x4 operator *(const uint2x4& a, const uint2x4& b) { return uint2x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13); }
	static uint2x4 operator -(const uint2x4& a, const uint2x4& b) { return uint2x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13); }
	static uint2x4 operator /(const uint2x4& a, const uint2x4& b) { return uint2x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13); }
	static uint2x4 operator %(const uint2x4& a, const uint2x4& b) { return uint2x4(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m03 % b._m03, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m13 % b._m13); }
	static int2x4 operator ==(const uint2x4& a, const uint2x4& b) { return int2x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0); }
	static int2x4 operator !=(const uint2x4& a, const uint2x4& b) { return int2x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0); }
	static int2x4 operator <(const uint2x4& a, const uint2x4& b) { return int2x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0); }
	static int2x4 operator <=(const uint2x4& a, const uint2x4& b) { return int2x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0); }
	static int2x4 operator >=(const uint2x4& a, const uint2x4& b) { return int2x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0); }
	static int2x4 operator >(const uint2x4& a, const uint2x4& b) { return int2x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0); }

	struct uint3x1 {
		uint _m00;
		uint _m10;
		uint _m20;
		uint1 operator[] (int row) const {
			if (row == 0) return uint1(_m00);
			if (row == 1) return uint1(_m10);
			if (row == 2) return uint1(_m20);
			return 0; // Silent return ... valid for HLSL
		}
		uint3x1(uint _m00, uint _m10, uint _m20) {
			this->_m00 = _m00;
			this->_m10 = _m10;
			this->_m20 = _m20;
		}
		uint3x1(uint v) :uint3x1(v, v, v) {}
		uint3x1(const uint1& r0, const uint1& r1, const uint1& r2) : uint3x1(r0.x, r1.x, r2.x) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		operator float3x1() const;
		operator int3x1() const;
	};
	static uint3x1 operator +(const uint3x1& a) { return uint3x1(+a._m00, +a._m10, +a._m20); }
	static int3x1 operator !(const uint3x1& a) { return int3x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0); }
	static uint3x1 operator +(const uint3x1& a, const uint3x1& b) { return uint3x1(a._m00 + b._m00, a._m10 + b._m10, a._m20 + b._m20); }
	static uint3x1 operator *(const uint3x1& a, const uint3x1& b) { return uint3x1(a._m00 * b._m00, a._m10 * b._m10, a._m20 * b._m20); }
	static uint3x1 operator -(const uint3x1& a, const uint3x1& b) { return uint3x1(a._m00 - b._m00, a._m10 - b._m10, a._m20 - b._m20); }
	static uint3x1 operator /(const uint3x1& a, const uint3x1& b) { return uint3x1(a._m00 / b._m00, a._m10 / b._m10, a._m20 / b._m20); }
	static uint3x1 operator %(const uint3x1& a, const uint3x1& b) { return uint3x1(a._m00 % b._m00, a._m10 % b._m10, a._m20 % b._m20); }
	static int3x1 operator ==(const uint3x1& a, const uint3x1& b) { return int3x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0); }
	static int3x1 operator !=(const uint3x1& a, const uint3x1& b) { return int3x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0); }
	static int3x1 operator <(const uint3x1& a, const uint3x1& b) { return int3x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0); }
	static int3x1 operator <=(const uint3x1& a, const uint3x1& b) { return int3x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0); }
	static int3x1 operator >=(const uint3x1& a, const uint3x1& b) { return int3x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0); }
	static int3x1 operator >(const uint3x1& a, const uint3x1& b) { return int3x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0); }

	struct uint3x2 {
		uint _m00;
		uint _m10;
		uint _m20;
		uint _m01;
		uint _m11;
		uint _m21;
		uint2 operator[] (int row) const {
			if (row == 0) return uint2(_m00, _m01);
			if (row == 1) return uint2(_m10, _m11);
			if (row == 2) return uint2(_m20, _m21);
			return 0; // Silent return ... valid for HLSL
		}
		uint3x2(uint _m00, uint _m01, uint _m10, uint _m11, uint _m20, uint _m21) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m20 = _m20;
			this->_m21 = _m21;
		}
		uint3x2(uint v) :uint3x2(v, v, v, v, v, v) {}
		uint3x2(const uint2& r0, const uint2& r1, const uint2& r2) : uint3x2(r0.x, r0.y, r1.x, r1.y, r2.x, r2.y) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint2x2() const { return uint2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator uint3x1() const { return uint3x1(this->_m00, this->_m10, this->_m20); }
		operator float3x2() const;
		operator int3x2() const;
	};
	static uint3x2 operator +(const uint3x2& a) { return uint3x2(+a._m00, +a._m01, +a._m10, +a._m11, +a._m20, +a._m21); }
	static int3x2 operator !(const uint3x2& a) { return int3x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0); }
	static uint3x2 operator +(const uint3x2& a, const uint3x2& b) { return uint3x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11, a._m20 + b._m20, a._m21 + b._m21); }
	static uint3x2 operator *(const uint3x2& a, const uint3x2& b) { return uint3x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11, a._m20 * b._m20, a._m21 * b._m21); }
	static uint3x2 operator -(const uint3x2& a, const uint3x2& b) { return uint3x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11, a._m20 - b._m20, a._m21 - b._m21); }
	static uint3x2 operator /(const uint3x2& a, const uint3x2& b) { return uint3x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11, a._m20 / b._m20, a._m21 / b._m21); }
	static uint3x2 operator %(const uint3x2& a, const uint3x2& b) { return uint3x2(a._m00 % b._m00, a._m01 % b._m01, a._m10 % b._m10, a._m11 % b._m11, a._m20 % b._m20, a._m21 % b._m21); }
	static int3x2 operator ==(const uint3x2& a, const uint3x2& b) { return int3x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0); }
	static int3x2 operator !=(const uint3x2& a, const uint3x2& b) { return int3x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0); }
	static int3x2 operator <(const uint3x2& a, const uint3x2& b) { return int3x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0); }
	static int3x2 operator <=(const uint3x2& a, const uint3x2& b) { return int3x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0); }
	static int3x2 operator >=(const uint3x2& a, const uint3x2& b) { return int3x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0); }
	static int3x2 operator >(const uint3x2& a, const uint3x2& b) { return int3x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0); }

	struct uint3x3 {
		uint _m00;
		uint _m10;
		uint _m20;
		uint _m01;
		uint _m11;
		uint _m21;
		uint _m02;
		uint _m12;
		uint _m22;
		uint3 operator[] (int row) const {
			if (row == 0) return uint3(_m00, _m01, _m02);
			if (row == 1) return uint3(_m10, _m11, _m12);
			if (row == 2) return uint3(_m20, _m21, _m22);
			return 0; // Silent return ... valid for HLSL
		}
		uint3x3(uint _m00, uint _m01, uint _m02, uint _m10, uint _m11, uint _m12, uint _m20, uint _m21, uint _m22) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
		}
		uint3x3(uint v) :uint3x3(v, v, v, v, v, v, v, v, v) {}
		uint3x3(const uint3& r0, const uint3& r1, const uint3& r2) : uint3x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z, r2.x, r2.y, r2.z) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint1x3() const { return uint1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint2x2() const { return uint2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator uint2x3() const { return uint2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator uint3x1() const { return uint3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator uint3x2() const { return uint3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		operator float3x3() const;
		operator int3x3() const;
	};
	static uint3x3 operator +(const uint3x3& a) { return uint3x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12, +a._m20, +a._m21, +a._m22); }
	static int3x3 operator !(const uint3x3& a) { return int3x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0); }
	static uint3x3 operator +(const uint3x3& a, const uint3x3& b) { return uint3x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22); }
	static uint3x3 operator *(const uint3x3& a, const uint3x3& b) { return uint3x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22); }
	static uint3x3 operator -(const uint3x3& a, const uint3x3& b) { return uint3x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22); }
	static uint3x3 operator /(const uint3x3& a, const uint3x3& b) { return uint3x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22); }
	static uint3x3 operator %(const uint3x3& a, const uint3x3& b) { return uint3x3(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m20 % b._m20, a._m21 % b._m21, a._m22 % b._m22); }
	static int3x3 operator ==(const uint3x3& a, const uint3x3& b) { return int3x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0); }
	static int3x3 operator !=(const uint3x3& a, const uint3x3& b) { return int3x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0); }
	static int3x3 operator <(const uint3x3& a, const uint3x3& b) { return int3x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0); }
	static int3x3 operator <=(const uint3x3& a, const uint3x3& b) { return int3x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0); }
	static int3x3 operator >=(const uint3x3& a, const uint3x3& b) { return int3x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0); }
	static int3x3 operator >(const uint3x3& a, const uint3x3& b) { return int3x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0); }

	struct uint3x4 {
		uint _m00;
		uint _m10;
		uint _m20;
		uint _m01;
		uint _m11;
		uint _m21;
		uint _m02;
		uint _m12;
		uint _m22;
		uint _m03;
		uint _m13;
		uint _m23;
		uint4 operator[] (int row) const {
			if (row == 0) return uint4(_m00, _m01, _m02, _m03);
			if (row == 1) return uint4(_m10, _m11, _m12, _m13);
			if (row == 2) return uint4(_m20, _m21, _m22, _m23);
			return 0; // Silent return ... valid for HLSL
		}
		uint3x4(uint _m00, uint _m01, uint _m02, uint _m03, uint _m10, uint _m11, uint _m12, uint _m13, uint _m20, uint _m21, uint _m22, uint _m23) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m23 = _m23;
		}
		uint3x4(uint v) :uint3x4(v, v, v, v, v, v, v, v, v, v, v, v) {}
		uint3x4(const uint4& r0, const uint4& r1, const uint4& r2) : uint3x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint1x3() const { return uint1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator uint1x4() const { return uint1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint2x2() const { return uint2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator uint2x3() const { return uint2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator uint2x4() const { return uint2x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13); }
		explicit operator uint3x1() const { return uint3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator uint3x2() const { return uint3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator uint3x3() const { return uint3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		operator float3x4() const;
		operator int3x4() const;
	};
	static uint3x4 operator +(const uint3x4& a) { return uint3x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13, +a._m20, +a._m21, +a._m22, +a._m23); }
	static int3x4 operator !(const uint3x4& a) { return int3x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m23 == 0 ? 1 : 0); }
	static uint3x4 operator +(const uint3x4& a, const uint3x4& b) { return uint3x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m23 + b._m23); }
	static uint3x4 operator *(const uint3x4& a, const uint3x4& b) { return uint3x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m23 * b._m23); }
	static uint3x4 operator -(const uint3x4& a, const uint3x4& b) { return uint3x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m23 - b._m23); }
	static uint3x4 operator /(const uint3x4& a, const uint3x4& b) { return uint3x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m23 / b._m23); }
	static uint3x4 operator %(const uint3x4& a, const uint3x4& b) { return uint3x4(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m03 % b._m03, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m13 % b._m13, a._m20 % b._m20, a._m21 % b._m21, a._m22 % b._m22, a._m23 % b._m23); }
	static int3x4 operator ==(const uint3x4& a, const uint3x4& b) { return int3x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m23 == b._m23) ? 1 : 0); }
	static int3x4 operator !=(const uint3x4& a, const uint3x4& b) { return int3x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m23 != b._m23) ? 1 : 0); }
	static int3x4 operator <(const uint3x4& a, const uint3x4& b) { return int3x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m23 < b._m23) ? 1 : 0); }
	static int3x4 operator <=(const uint3x4& a, const uint3x4& b) { return int3x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m23 <= b._m23) ? 1 : 0); }
	static int3x4 operator >=(const uint3x4& a, const uint3x4& b) { return int3x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m23 >= b._m23) ? 1 : 0); }
	static int3x4 operator >(const uint3x4& a, const uint3x4& b) { return int3x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m23 > b._m23) ? 1 : 0); }

	struct uint4x1 {
		uint _m00;
		uint _m10;
		uint _m20;
		uint _m30;
		uint1 operator[] (int row) const {
			if (row == 0) return uint1(_m00);
			if (row == 1) return uint1(_m10);
			if (row == 2) return uint1(_m20);
			if (row == 3) return uint1(_m30);
			return 0; // Silent return ... valid for HLSL
		}
		uint4x1(uint _m00, uint _m10, uint _m20, uint _m30) {
			this->_m00 = _m00;
			this->_m10 = _m10;
			this->_m20 = _m20;
			this->_m30 = _m30;
		}
		uint4x1(uint v) :uint4x1(v, v, v, v) {}
		uint4x1(const uint1& r0, const uint1& r1, const uint1& r2, const uint1& r3) : uint4x1(r0.x, r1.x, r2.x, r3.x) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint3x1() const { return uint3x1(this->_m00, this->_m10, this->_m20); }
		operator float4x1() const;
		operator int4x1() const;
	};
	static uint4x1 operator +(const uint4x1& a) { return uint4x1(+a._m00, +a._m10, +a._m20, +a._m30); }
	static int4x1 operator !(const uint4x1& a) { return int4x1(a._m00 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0); }
	static uint4x1 operator +(const uint4x1& a, const uint4x1& b) { return uint4x1(a._m00 + b._m00, a._m10 + b._m10, a._m20 + b._m20, a._m30 + b._m30); }
	static uint4x1 operator *(const uint4x1& a, const uint4x1& b) { return uint4x1(a._m00 * b._m00, a._m10 * b._m10, a._m20 * b._m20, a._m30 * b._m30); }
	static uint4x1 operator -(const uint4x1& a, const uint4x1& b) { return uint4x1(a._m00 - b._m00, a._m10 - b._m10, a._m20 - b._m20, a._m30 - b._m30); }
	static uint4x1 operator /(const uint4x1& a, const uint4x1& b) { return uint4x1(a._m00 / b._m00, a._m10 / b._m10, a._m20 / b._m20, a._m30 / b._m30); }
	static uint4x1 operator %(const uint4x1& a, const uint4x1& b) { return uint4x1(a._m00 % b._m00, a._m10 % b._m10, a._m20 % b._m20, a._m30 % b._m30); }
	static int4x1 operator ==(const uint4x1& a, const uint4x1& b) { return int4x1((a._m00 == b._m00) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0); }
	static int4x1 operator !=(const uint4x1& a, const uint4x1& b) { return int4x1((a._m00 != b._m00) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0); }
	static int4x1 operator <(const uint4x1& a, const uint4x1& b) { return int4x1((a._m00 < b._m00) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0); }
	static int4x1 operator <=(const uint4x1& a, const uint4x1& b) { return int4x1((a._m00 <= b._m00) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0); }
	static int4x1 operator >=(const uint4x1& a, const uint4x1& b) { return int4x1((a._m00 >= b._m00) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0); }
	static int4x1 operator >(const uint4x1& a, const uint4x1& b) { return int4x1((a._m00 > b._m00) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0); }

	struct uint4x2 {
		uint _m00;
		uint _m10;
		uint _m20;
		uint _m30;
		uint _m01;
		uint _m11;
		uint _m21;
		uint _m31;
		uint2 operator[] (int row) const {
			if (row == 0) return uint2(_m00, _m01);
			if (row == 1) return uint2(_m10, _m11);
			if (row == 2) return uint2(_m20, _m21);
			if (row == 3) return uint2(_m30, _m31);
			return 0; // Silent return ... valid for HLSL
		}
		uint4x2(uint _m00, uint _m01, uint _m10, uint _m11, uint _m20, uint _m21, uint _m30, uint _m31) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m30 = _m30;
			this->_m31 = _m31;
		}
		uint4x2(uint v) :uint4x2(v, v, v, v, v, v, v, v) {}
		uint4x2(const uint2& r0, const uint2& r1, const uint2& r2, const uint2& r3) : uint4x2(r0.x, r0.y, r1.x, r1.y, r2.x, r2.y, r3.x, r3.y) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint2x2() const { return uint2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator uint3x1() const { return uint3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator uint3x2() const { return uint3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator uint4x1() const { return uint4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		operator float4x2() const;
		operator int4x2() const;
	};
	static uint4x2 operator +(const uint4x2& a) { return uint4x2(+a._m00, +a._m01, +a._m10, +a._m11, +a._m20, +a._m21, +a._m30, +a._m31); }
	static int4x2 operator !(const uint4x2& a) { return int4x2(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0); }
	static uint4x2 operator +(const uint4x2& a, const uint4x2& b) { return uint4x2(a._m00 + b._m00, a._m01 + b._m01, a._m10 + b._m10, a._m11 + b._m11, a._m20 + b._m20, a._m21 + b._m21, a._m30 + b._m30, a._m31 + b._m31); }
	static uint4x2 operator *(const uint4x2& a, const uint4x2& b) { return uint4x2(a._m00 * b._m00, a._m01 * b._m01, a._m10 * b._m10, a._m11 * b._m11, a._m20 * b._m20, a._m21 * b._m21, a._m30 * b._m30, a._m31 * b._m31); }
	static uint4x2 operator -(const uint4x2& a, const uint4x2& b) { return uint4x2(a._m00 - b._m00, a._m01 - b._m01, a._m10 - b._m10, a._m11 - b._m11, a._m20 - b._m20, a._m21 - b._m21, a._m30 - b._m30, a._m31 - b._m31); }
	static uint4x2 operator /(const uint4x2& a, const uint4x2& b) { return uint4x2(a._m00 / b._m00, a._m01 / b._m01, a._m10 / b._m10, a._m11 / b._m11, a._m20 / b._m20, a._m21 / b._m21, a._m30 / b._m30, a._m31 / b._m31); }
	static uint4x2 operator %(const uint4x2& a, const uint4x2& b) { return uint4x2(a._m00 % b._m00, a._m01 % b._m01, a._m10 % b._m10, a._m11 % b._m11, a._m20 % b._m20, a._m21 % b._m21, a._m30 % b._m30, a._m31 % b._m31); }
	static int4x2 operator ==(const uint4x2& a, const uint4x2& b) { return int4x2((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0); }
	static int4x2 operator !=(const uint4x2& a, const uint4x2& b) { return int4x2((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0); }
	static int4x2 operator <(const uint4x2& a, const uint4x2& b) { return int4x2((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0); }
	static int4x2 operator <=(const uint4x2& a, const uint4x2& b) { return int4x2((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0); }
	static int4x2 operator >=(const uint4x2& a, const uint4x2& b) { return int4x2((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0); }
	static int4x2 operator >(const uint4x2& a, const uint4x2& b) { return int4x2((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0); }

	struct uint4x3 {
		uint _m00;
		uint _m10;
		uint _m20;
		uint _m30;
		uint _m01;
		uint _m11;
		uint _m21;
		uint _m31;
		uint _m02;
		uint _m12;
		uint _m22;
		uint _m32;
		uint3 operator[] (int row) const {
			if (row == 0) return uint3(_m00, _m01, _m02);
			if (row == 1) return uint3(_m10, _m11, _m12);
			if (row == 2) return uint3(_m20, _m21, _m22);
			if (row == 3) return uint3(_m30, _m31, _m32);
			return 0; // Silent return ... valid for HLSL
		}
		uint4x3(uint _m00, uint _m01, uint _m02, uint _m10, uint _m11, uint _m12, uint _m20, uint _m21, uint _m22, uint _m30, uint _m31, uint _m32) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m30 = _m30;
			this->_m31 = _m31;
			this->_m32 = _m32;
		}
		uint4x3(uint v) :uint4x3(v, v, v, v, v, v, v, v, v, v, v, v) {}
		uint4x3(const uint3& r0, const uint3& r1, const uint3& r2, const uint3& r3) : uint4x3(r0.x, r0.y, r0.z, r1.x, r1.y, r1.z, r2.x, r2.y, r2.z, r3.x, r3.y, r3.z) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint1x3() const { return uint1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint2x2() const { return uint2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator uint2x3() const { return uint2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator uint3x1() const { return uint3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator uint3x2() const { return uint3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator uint3x3() const { return uint3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		explicit operator uint4x1() const { return uint4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		explicit operator uint4x2() const { return uint4x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21, this->_m30, this->_m31); }
		operator float4x3() const;
		operator int4x3() const;
	};
	static uint4x3 operator +(const uint4x3& a) { return uint4x3(+a._m00, +a._m01, +a._m02, +a._m10, +a._m11, +a._m12, +a._m20, +a._m21, +a._m22, +a._m30, +a._m31, +a._m32); }
	static int4x3 operator !(const uint4x3& a) { return int4x3(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0, a._m32 == 0 ? 1 : 0); }
	static uint4x3 operator +(const uint4x3& a, const uint4x3& b) { return uint4x3(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m30 + b._m30, a._m31 + b._m31, a._m32 + b._m32); }
	static uint4x3 operator *(const uint4x3& a, const uint4x3& b) { return uint4x3(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m30 * b._m30, a._m31 * b._m31, a._m32 * b._m32); }
	static uint4x3 operator -(const uint4x3& a, const uint4x3& b) { return uint4x3(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m30 - b._m30, a._m31 - b._m31, a._m32 - b._m32); }
	static uint4x3 operator /(const uint4x3& a, const uint4x3& b) { return uint4x3(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m30 / b._m30, a._m31 / b._m31, a._m32 / b._m32); }
	static uint4x3 operator %(const uint4x3& a, const uint4x3& b) { return uint4x3(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m20 % b._m20, a._m21 % b._m21, a._m22 % b._m22, a._m30 % b._m30, a._m31 % b._m31, a._m32 % b._m32); }
	static int4x3 operator ==(const uint4x3& a, const uint4x3& b) { return int4x3((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0, (a._m32 == b._m32) ? 1 : 0); }
	static int4x3 operator !=(const uint4x3& a, const uint4x3& b) { return int4x3((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0, (a._m32 != b._m32) ? 1 : 0); }
	static int4x3 operator <(const uint4x3& a, const uint4x3& b) { return int4x3((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0, (a._m32 < b._m32) ? 1 : 0); }
	static int4x3 operator <=(const uint4x3& a, const uint4x3& b) { return int4x3((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0, (a._m32 <= b._m32) ? 1 : 0); }
	static int4x3 operator >=(const uint4x3& a, const uint4x3& b) { return int4x3((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0, (a._m32 >= b._m32) ? 1 : 0); }
	static int4x3 operator >(const uint4x3& a, const uint4x3& b) { return int4x3((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0, (a._m32 > b._m32) ? 1 : 0); }

	struct uint4x4 {
		uint _m00;
		uint _m10;
		uint _m20;
		uint _m30;
		uint _m01;
		uint _m11;
		uint _m21;
		uint _m31;
		uint _m02;
		uint _m12;
		uint _m22;
		uint _m32;
		uint _m03;
		uint _m13;
		uint _m23;
		uint _m33;
		uint4 operator[] (int row) const {
			if (row == 0) return uint4(_m00, _m01, _m02, _m03);
			if (row == 1) return uint4(_m10, _m11, _m12, _m13);
			if (row == 2) return uint4(_m20, _m21, _m22, _m23);
			if (row == 3) return uint4(_m30, _m31, _m32, _m33);
			return 0; // Silent return ... valid for HLSL
		}
		uint4x4(uint _m00, uint _m01, uint _m02, uint _m03, uint _m10, uint _m11, uint _m12, uint _m13, uint _m20, uint _m21, uint _m22, uint _m23, uint _m30, uint _m31, uint _m32, uint _m33) {
			this->_m00 = _m00;
			this->_m01 = _m01;
			this->_m02 = _m02;
			this->_m03 = _m03;
			this->_m10 = _m10;
			this->_m11 = _m11;
			this->_m12 = _m12;
			this->_m13 = _m13;
			this->_m20 = _m20;
			this->_m21 = _m21;
			this->_m22 = _m22;
			this->_m23 = _m23;
			this->_m30 = _m30;
			this->_m31 = _m31;
			this->_m32 = _m32;
			this->_m33 = _m33;
		}
		uint4x4(uint v) :uint4x4(v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v) {}
		uint4x4(const uint4& r0, const uint4& r1, const uint4& r2, const uint4& r3) : uint4x4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w, r3.x, r3.y, r3.z, r3.w) { }
		explicit operator uint1x1() const { return uint1x1(this->_m00); }
		explicit operator uint1x2() const { return uint1x2(this->_m00, this->_m01); }
		explicit operator uint1x3() const { return uint1x3(this->_m00, this->_m01, this->_m02); }
		explicit operator uint1x4() const { return uint1x4(this->_m00, this->_m01, this->_m02, this->_m03); }
		explicit operator uint2x1() const { return uint2x1(this->_m00, this->_m10); }
		explicit operator uint2x2() const { return uint2x2(this->_m00, this->_m01, this->_m10, this->_m11); }
		explicit operator uint2x3() const { return uint2x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12); }
		explicit operator uint2x4() const { return uint2x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13); }
		explicit operator uint3x1() const { return uint3x1(this->_m00, this->_m10, this->_m20); }
		explicit operator uint3x2() const { return uint3x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21); }
		explicit operator uint3x3() const { return uint3x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22); }
		explicit operator uint3x4() const { return uint3x4(this->_m00, this->_m01, this->_m02, this->_m03, this->_m10, this->_m11, this->_m12, this->_m13, this->_m20, this->_m21, this->_m22, this->_m23); }
		explicit operator uint4x1() const { return uint4x1(this->_m00, this->_m10, this->_m20, this->_m30); }
		explicit operator uint4x2() const { return uint4x2(this->_m00, this->_m01, this->_m10, this->_m11, this->_m20, this->_m21, this->_m30, this->_m31); }
		explicit operator uint4x3() const { return uint4x3(this->_m00, this->_m01, this->_m02, this->_m10, this->_m11, this->_m12, this->_m20, this->_m21, this->_m22, this->_m30, this->_m31, this->_m32); }
		operator float4x4() const;
		operator int4x4() const;
	};
	static uint4x4 operator +(const uint4x4& a) { return uint4x4(+a._m00, +a._m01, +a._m02, +a._m03, +a._m10, +a._m11, +a._m12, +a._m13, +a._m20, +a._m21, +a._m22, +a._m23, +a._m30, +a._m31, +a._m32, +a._m33); }
	static int4x4 operator !(const uint4x4& a) { return int4x4(a._m00 == 0 ? 1 : 0, a._m01 == 0 ? 1 : 0, a._m02 == 0 ? 1 : 0, a._m03 == 0 ? 1 : 0, a._m10 == 0 ? 1 : 0, a._m11 == 0 ? 1 : 0, a._m12 == 0 ? 1 : 0, a._m13 == 0 ? 1 : 0, a._m20 == 0 ? 1 : 0, a._m21 == 0 ? 1 : 0, a._m22 == 0 ? 1 : 0, a._m23 == 0 ? 1 : 0, a._m30 == 0 ? 1 : 0, a._m31 == 0 ? 1 : 0, a._m32 == 0 ? 1 : 0, a._m33 == 0 ? 1 : 0); }
	static uint4x4 operator +(const uint4x4& a, const uint4x4& b) { return uint4x4(a._m00 + b._m00, a._m01 + b._m01, a._m02 + b._m02, a._m03 + b._m03, a._m10 + b._m10, a._m11 + b._m11, a._m12 + b._m12, a._m13 + b._m13, a._m20 + b._m20, a._m21 + b._m21, a._m22 + b._m22, a._m23 + b._m23, a._m30 + b._m30, a._m31 + b._m31, a._m32 + b._m32, a._m33 + b._m33); }
	static uint4x4 operator *(const uint4x4& a, const uint4x4& b) { return uint4x4(a._m00 * b._m00, a._m01 * b._m01, a._m02 * b._m02, a._m03 * b._m03, a._m10 * b._m10, a._m11 * b._m11, a._m12 * b._m12, a._m13 * b._m13, a._m20 * b._m20, a._m21 * b._m21, a._m22 * b._m22, a._m23 * b._m23, a._m30 * b._m30, a._m31 * b._m31, a._m32 * b._m32, a._m33 * b._m33); }
	static uint4x4 operator -(const uint4x4& a, const uint4x4& b) { return uint4x4(a._m00 - b._m00, a._m01 - b._m01, a._m02 - b._m02, a._m03 - b._m03, a._m10 - b._m10, a._m11 - b._m11, a._m12 - b._m12, a._m13 - b._m13, a._m20 - b._m20, a._m21 - b._m21, a._m22 - b._m22, a._m23 - b._m23, a._m30 - b._m30, a._m31 - b._m31, a._m32 - b._m32, a._m33 - b._m33); }
	static uint4x4 operator /(const uint4x4& a, const uint4x4& b) { return uint4x4(a._m00 / b._m00, a._m01 / b._m01, a._m02 / b._m02, a._m03 / b._m03, a._m10 / b._m10, a._m11 / b._m11, a._m12 / b._m12, a._m13 / b._m13, a._m20 / b._m20, a._m21 / b._m21, a._m22 / b._m22, a._m23 / b._m23, a._m30 / b._m30, a._m31 / b._m31, a._m32 / b._m32, a._m33 / b._m33); }
	static uint4x4 operator %(const uint4x4& a, const uint4x4& b) { return uint4x4(a._m00 % b._m00, a._m01 % b._m01, a._m02 % b._m02, a._m03 % b._m03, a._m10 % b._m10, a._m11 % b._m11, a._m12 % b._m12, a._m13 % b._m13, a._m20 % b._m20, a._m21 % b._m21, a._m22 % b._m22, a._m23 % b._m23, a._m30 % b._m30, a._m31 % b._m31, a._m32 % b._m32, a._m33 % b._m33); }
	static int4x4 operator ==(const uint4x4& a, const uint4x4& b) { return int4x4((a._m00 == b._m00) ? 1 : 0, (a._m01 == b._m01) ? 1 : 0, (a._m02 == b._m02) ? 1 : 0, (a._m03 == b._m03) ? 1 : 0, (a._m10 == b._m10) ? 1 : 0, (a._m11 == b._m11) ? 1 : 0, (a._m12 == b._m12) ? 1 : 0, (a._m13 == b._m13) ? 1 : 0, (a._m20 == b._m20) ? 1 : 0, (a._m21 == b._m21) ? 1 : 0, (a._m22 == b._m22) ? 1 : 0, (a._m23 == b._m23) ? 1 : 0, (a._m30 == b._m30) ? 1 : 0, (a._m31 == b._m31) ? 1 : 0, (a._m32 == b._m32) ? 1 : 0, (a._m33 == b._m33) ? 1 : 0); }
	static int4x4 operator !=(const uint4x4& a, const uint4x4& b) { return int4x4((a._m00 != b._m00) ? 1 : 0, (a._m01 != b._m01) ? 1 : 0, (a._m02 != b._m02) ? 1 : 0, (a._m03 != b._m03) ? 1 : 0, (a._m10 != b._m10) ? 1 : 0, (a._m11 != b._m11) ? 1 : 0, (a._m12 != b._m12) ? 1 : 0, (a._m13 != b._m13) ? 1 : 0, (a._m20 != b._m20) ? 1 : 0, (a._m21 != b._m21) ? 1 : 0, (a._m22 != b._m22) ? 1 : 0, (a._m23 != b._m23) ? 1 : 0, (a._m30 != b._m30) ? 1 : 0, (a._m31 != b._m31) ? 1 : 0, (a._m32 != b._m32) ? 1 : 0, (a._m33 != b._m33) ? 1 : 0); }
	static int4x4 operator <(const uint4x4& a, const uint4x4& b) { return int4x4((a._m00 < b._m00) ? 1 : 0, (a._m01 < b._m01) ? 1 : 0, (a._m02 < b._m02) ? 1 : 0, (a._m03 < b._m03) ? 1 : 0, (a._m10 < b._m10) ? 1 : 0, (a._m11 < b._m11) ? 1 : 0, (a._m12 < b._m12) ? 1 : 0, (a._m13 < b._m13) ? 1 : 0, (a._m20 < b._m20) ? 1 : 0, (a._m21 < b._m21) ? 1 : 0, (a._m22 < b._m22) ? 1 : 0, (a._m23 < b._m23) ? 1 : 0, (a._m30 < b._m30) ? 1 : 0, (a._m31 < b._m31) ? 1 : 0, (a._m32 < b._m32) ? 1 : 0, (a._m33 < b._m33) ? 1 : 0); }
	static int4x4 operator <=(const uint4x4& a, const uint4x4& b) { return int4x4((a._m00 <= b._m00) ? 1 : 0, (a._m01 <= b._m01) ? 1 : 0, (a._m02 <= b._m02) ? 1 : 0, (a._m03 <= b._m03) ? 1 : 0, (a._m10 <= b._m10) ? 1 : 0, (a._m11 <= b._m11) ? 1 : 0, (a._m12 <= b._m12) ? 1 : 0, (a._m13 <= b._m13) ? 1 : 0, (a._m20 <= b._m20) ? 1 : 0, (a._m21 <= b._m21) ? 1 : 0, (a._m22 <= b._m22) ? 1 : 0, (a._m23 <= b._m23) ? 1 : 0, (a._m30 <= b._m30) ? 1 : 0, (a._m31 <= b._m31) ? 1 : 0, (a._m32 <= b._m32) ? 1 : 0, (a._m33 <= b._m33) ? 1 : 0); }
	static int4x4 operator >=(const uint4x4& a, const uint4x4& b) { return int4x4((a._m00 >= b._m00) ? 1 : 0, (a._m01 >= b._m01) ? 1 : 0, (a._m02 >= b._m02) ? 1 : 0, (a._m03 >= b._m03) ? 1 : 0, (a._m10 >= b._m10) ? 1 : 0, (a._m11 >= b._m11) ? 1 : 0, (a._m12 >= b._m12) ? 1 : 0, (a._m13 >= b._m13) ? 1 : 0, (a._m20 >= b._m20) ? 1 : 0, (a._m21 >= b._m21) ? 1 : 0, (a._m22 >= b._m22) ? 1 : 0, (a._m23 >= b._m23) ? 1 : 0, (a._m30 >= b._m30) ? 1 : 0, (a._m31 >= b._m31) ? 1 : 0, (a._m32 >= b._m32) ? 1 : 0, (a._m33 >= b._m33) ? 1 : 0); }
	static int4x4 operator >(const uint4x4& a, const uint4x4& b) { return int4x4((a._m00 > b._m00) ? 1 : 0, (a._m01 > b._m01) ? 1 : 0, (a._m02 > b._m02) ? 1 : 0, (a._m03 > b._m03) ? 1 : 0, (a._m10 > b._m10) ? 1 : 0, (a._m11 > b._m11) ? 1 : 0, (a._m12 > b._m12) ? 1 : 0, (a._m13 > b._m13) ? 1 : 0, (a._m20 > b._m20) ? 1 : 0, (a._m21 > b._m21) ? 1 : 0, (a._m22 > b._m22) ? 1 : 0, (a._m23 > b._m23) ? 1 : 0, (a._m30 > b._m30) ? 1 : 0, (a._m31 > b._m31) ? 1 : 0, (a._m32 > b._m32) ? 1 : 0, (a._m33 > b._m33) ? 1 : 0); }




	const float PI = 3.141592653589793f;
#pragma region abs
	static float abs(const float& v) { return fabsf(v); }

	static float1 abs(const float1& v) { return float1(fabsf(v.x)); }
	static float2 abs(const float2& v) { return float2(fabsf(v.x), fabsf(v.y)); }
	static float3 abs(const float3& v) { return float3(fabsf(v.x), fabsf(v.y), fabsf(v.z)); }
	static float4 abs(const float4& v) { return float4(fabsf(v.x), fabsf(v.y), fabsf(v.z), fabsf(v.w)); }

	static float1x1 abs(const float1x1& v) { return float1x1(fabsf(v._m00)); }
	static float1x2 abs(const float1x2& v) { return float1x2(fabsf(v._m00), fabsf(v._m01)); }
	static float1x3 abs(const float1x3& v) { return float1x3(fabsf(v._m00), fabsf(v._m01), fabsf(v._m02)); }
	static float1x4 abs(const float1x4& v) { return float1x4(fabsf(v._m00), fabsf(v._m01), fabsf(v._m02), fabsf(v._m03)); }
	static float2x1 abs(const float2x1& v) { return float2x1(fabsf(v._m00), fabsf(v._m10)); }
	static float2x2 abs(const float2x2& v) { return float2x2(fabsf(v._m00), fabsf(v._m01), fabsf(v._m10), fabsf(v._m11)); }
	static float2x3 abs(const float2x3& v) { return float2x3(fabsf(v._m00), fabsf(v._m01), fabsf(v._m02), fabsf(v._m10), fabsf(v._m11), fabsf(v._m12)); }
	static float2x4 abs(const float2x4& v) { return float2x4(fabsf(v._m00), fabsf(v._m01), fabsf(v._m02), fabsf(v._m03), fabsf(v._m10), fabsf(v._m11), fabsf(v._m12), fabsf(v._m13)); }
	static float3x1 abs(const float3x1& v) { return float3x1(fabsf(v._m00), fabsf(v._m10), fabsf(v._m20)); }
	static float3x2 abs(const float3x2& v) { return float3x2(fabsf(v._m00), fabsf(v._m01), fabsf(v._m10), fabsf(v._m11), fabsf(v._m20), fabsf(v._m21)); }
	static float3x3 abs(const float3x3& v) { return float3x3(fabsf(v._m00), fabsf(v._m01), fabsf(v._m02), fabsf(v._m10), fabsf(v._m11), fabsf(v._m12), fabsf(v._m20), fabsf(v._m21), fabsf(v._m22)); }
	static float3x4 abs(const float3x4& v) { return float3x4(fabsf(v._m00), fabsf(v._m01), fabsf(v._m02), fabsf(v._m03), fabsf(v._m10), fabsf(v._m11), fabsf(v._m12), fabsf(v._m13), fabsf(v._m20), fabsf(v._m21), fabsf(v._m22), fabsf(v._m23)); }
	static float4x1 abs(const float4x1& v) { return float4x1(fabsf(v._m00), fabsf(v._m10), fabsf(v._m20), fabsf(v._m30)); }
	static float4x2 abs(const float4x2& v) { return float4x2(fabsf(v._m00), fabsf(v._m01), fabsf(v._m10), fabsf(v._m11), fabsf(v._m20), fabsf(v._m21), fabsf(v._m30), fabsf(v._m31)); }
	static float4x3 abs(const float4x3& v) { return float4x3(fabsf(v._m00), fabsf(v._m01), fabsf(v._m02), fabsf(v._m10), fabsf(v._m11), fabsf(v._m12), fabsf(v._m20), fabsf(v._m21), fabsf(v._m22), fabsf(v._m30), fabsf(v._m31), fabsf(v._m32)); }
	static float4x4 abs(const float4x4& v) { return float4x4(fabsf(v._m00), fabsf(v._m01), fabsf(v._m02), fabsf(v._m03), fabsf(v._m10), fabsf(v._m11), fabsf(v._m12), fabsf(v._m13), fabsf(v._m20), fabsf(v._m21), fabsf(v._m22), fabsf(v._m23), fabsf(v._m30), fabsf(v._m31), fabsf(v._m32), fabsf(v._m33)); }

#pragma endregion


#pragma region acos
	static float acos(const float& v) { return acosf(v); }

	static float1 acos(const float1& v) { return float1(acosf(v.x)); }
	static float2 acos(const float2& v) { return float2(acosf(v.x), acosf(v.y)); }
	static float3 acos(const float3& v) { return float3(acosf(v.x), acosf(v.y), acosf(v.z)); }
	static float4 acos(const float4& v) { return float4(acosf(v.x), acosf(v.y), acosf(v.z), acosf(v.w)); }

	static float1x1 acos(const float1x1& v) { return float1x1(acosf(v._m00)); }
	static float1x2 acos(const float1x2& v) { return float1x2(acosf(v._m00), acosf(v._m01)); }
	static float1x3 acos(const float1x3& v) { return float1x3(acosf(v._m00), acosf(v._m01), acosf(v._m02)); }
	static float1x4 acos(const float1x4& v) { return float1x4(acosf(v._m00), acosf(v._m01), acosf(v._m02), acosf(v._m03)); }
	static float2x1 acos(const float2x1& v) { return float2x1(acosf(v._m00), acosf(v._m10)); }
	static float2x2 acos(const float2x2& v) { return float2x2(acosf(v._m00), acosf(v._m01), acosf(v._m10), acosf(v._m11)); }
	static float2x3 acos(const float2x3& v) { return float2x3(acosf(v._m00), acosf(v._m01), acosf(v._m02), acosf(v._m10), acosf(v._m11), acosf(v._m12)); }
	static float2x4 acos(const float2x4& v) { return float2x4(acosf(v._m00), acosf(v._m01), acosf(v._m02), acosf(v._m03), acosf(v._m10), acosf(v._m11), acosf(v._m12), acosf(v._m13)); }
	static float3x1 acos(const float3x1& v) { return float3x1(acosf(v._m00), acosf(v._m10), acosf(v._m20)); }
	static float3x2 acos(const float3x2& v) { return float3x2(acosf(v._m00), acosf(v._m01), acosf(v._m10), acosf(v._m11), acosf(v._m20), acosf(v._m21)); }
	static float3x3 acos(const float3x3& v) { return float3x3(acosf(v._m00), acosf(v._m01), acosf(v._m02), acosf(v._m10), acosf(v._m11), acosf(v._m12), acosf(v._m20), acosf(v._m21), acosf(v._m22)); }
	static float3x4 acos(const float3x4& v) { return float3x4(acosf(v._m00), acosf(v._m01), acosf(v._m02), acosf(v._m03), acosf(v._m10), acosf(v._m11), acosf(v._m12), acosf(v._m13), acosf(v._m20), acosf(v._m21), acosf(v._m22), acosf(v._m23)); }
	static float4x1 acos(const float4x1& v) { return float4x1(acosf(v._m00), acosf(v._m10), acosf(v._m20), acosf(v._m30)); }
	static float4x2 acos(const float4x2& v) { return float4x2(acosf(v._m00), acosf(v._m01), acosf(v._m10), acosf(v._m11), acosf(v._m20), acosf(v._m21), acosf(v._m30), acosf(v._m31)); }
	static float4x3 acos(const float4x3& v) { return float4x3(acosf(v._m00), acosf(v._m01), acosf(v._m02), acosf(v._m10), acosf(v._m11), acosf(v._m12), acosf(v._m20), acosf(v._m21), acosf(v._m22), acosf(v._m30), acosf(v._m31), acosf(v._m32)); }
	static float4x4 acos(const float4x4& v) { return float4x4(acosf(v._m00), acosf(v._m01), acosf(v._m02), acosf(v._m03), acosf(v._m10), acosf(v._m11), acosf(v._m12), acosf(v._m13), acosf(v._m20), acosf(v._m21), acosf(v._m22), acosf(v._m23), acosf(v._m30), acosf(v._m31), acosf(v._m32), acosf(v._m33)); }

#pragma endregion


#pragma region all
	static bool all(const float1& v) { return (v.x != 0); }
	static bool all(const float2& v) { return (v.x != 0) && (v.y != 0); }
	static bool all(const float3& v) { return (v.x != 0) && (v.y != 0) && (v.z != 0); }
	static bool all(const float4& v) { return (v.x != 0) && (v.y != 0) && (v.z != 0) && (v.w != 0); }
	static bool all(const float1x1& m) { return (m._m00 != 0); }
	static bool all(const float1x2& m) { return (m._m00 != 0) && (m._m01 != 0); }
	static bool all(const float1x3& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m02 != 0); }
	static bool all(const float1x4& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m02 != 0) && (m._m03 != 0); }
	static bool all(const float2x1& m) { return (m._m00 != 0) && (m._m10 != 0); }
	static bool all(const float2x2& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m10 != 0) && (m._m11 != 0); }
	static bool all(const float2x3& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m02 != 0) && (m._m10 != 0) && (m._m11 != 0) && (m._m12 != 0); }
	static bool all(const float2x4& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m02 != 0) && (m._m03 != 0) && (m._m10 != 0) && (m._m11 != 0) && (m._m12 != 0) && (m._m13 != 0); }
	static bool all(const float3x1& m) { return (m._m00 != 0) && (m._m10 != 0) && (m._m20 != 0); }
	static bool all(const float3x2& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m10 != 0) && (m._m11 != 0) && (m._m20 != 0) && (m._m21 != 0); }
	static bool all(const float3x3& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m02 != 0) && (m._m10 != 0) && (m._m11 != 0) && (m._m12 != 0) && (m._m20 != 0) && (m._m21 != 0) && (m._m22 != 0); }
	static bool all(const float3x4& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m02 != 0) && (m._m03 != 0) && (m._m10 != 0) && (m._m11 != 0) && (m._m12 != 0) && (m._m13 != 0) && (m._m20 != 0) && (m._m21 != 0) && (m._m22 != 0) && (m._m23 != 0); }
	static bool all(const float4x1& m) { return (m._m00 != 0) && (m._m10 != 0) && (m._m20 != 0) && (m._m30 != 0); }
	static bool all(const float4x2& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m10 != 0) && (m._m11 != 0) && (m._m20 != 0) && (m._m21 != 0) && (m._m30 != 0) && (m._m31 != 0); }
	static bool all(const float4x3& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m02 != 0) && (m._m10 != 0) && (m._m11 != 0) && (m._m12 != 0) && (m._m20 != 0) && (m._m21 != 0) && (m._m22 != 0) && (m._m30 != 0) && (m._m31 != 0) && (m._m32 != 0); }
	static bool all(const float4x4& m) { return (m._m00 != 0) && (m._m01 != 0) && (m._m02 != 0) && (m._m03 != 0) && (m._m10 != 0) && (m._m11 != 0) && (m._m12 != 0) && (m._m13 != 0) && (m._m20 != 0) && (m._m21 != 0) && (m._m22 != 0) && (m._m23 != 0) && (m._m30 != 0) && (m._m31 != 0) && (m._m32 != 0) && (m._m33 != 0); }
#pragma endregion


#pragma region any
	static bool any(const float1& v) { return (v.x != 0); }
	static bool any(const float2& v) { return (v.x != 0) || (v.y != 0); }
	static bool any(const float3& v) { return (v.x != 0) || (v.y != 0) || (v.z != 0); }
	static bool any(const float4& v) { return (v.x != 0) || (v.y != 0) || (v.z != 0) || (v.w != 0); }
	static bool any(const float1x1& m) { return (m._m00 != 0); }
	static bool any(const float1x2& m) { return (m._m00 != 0) || (m._m01 != 0); }
	static bool any(const float1x3& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m02 != 0); }
	static bool any(const float1x4& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m02 != 0) || (m._m03 != 0); }
	static bool any(const float2x1& m) { return (m._m00 != 0) || (m._m10 != 0); }
	static bool any(const float2x2& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m10 != 0) || (m._m11 != 0); }
	static bool any(const float2x3& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m02 != 0) || (m._m10 != 0) || (m._m11 != 0) || (m._m12 != 0); }
	static bool any(const float2x4& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m02 != 0) || (m._m03 != 0) || (m._m10 != 0) || (m._m11 != 0) || (m._m12 != 0) || (m._m13 != 0); }
	static bool any(const float3x1& m) { return (m._m00 != 0) || (m._m10 != 0) || (m._m20 != 0); }
	static bool any(const float3x2& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m10 != 0) || (m._m11 != 0) || (m._m20 != 0) || (m._m21 != 0); }
	static bool any(const float3x3& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m02 != 0) || (m._m10 != 0) || (m._m11 != 0) || (m._m12 != 0) || (m._m20 != 0) || (m._m21 != 0) || (m._m22 != 0); }
	static bool any(const float3x4& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m02 != 0) || (m._m03 != 0) || (m._m10 != 0) || (m._m11 != 0) || (m._m12 != 0) || (m._m13 != 0) || (m._m20 != 0) || (m._m21 != 0) || (m._m22 != 0) || (m._m23 != 0); }
	static bool any(const float4x1& m) { return (m._m00 != 0) || (m._m10 != 0) || (m._m20 != 0) || (m._m30 != 0); }
	static bool any(const float4x2& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m10 != 0) || (m._m11 != 0) || (m._m20 != 0) || (m._m21 != 0) || (m._m30 != 0) || (m._m31 != 0); }
	static bool any(const float4x3& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m02 != 0) || (m._m10 != 0) || (m._m11 != 0) || (m._m12 != 0) || (m._m20 != 0) || (m._m21 != 0) || (m._m22 != 0) || (m._m30 != 0) || (m._m31 != 0) || (m._m32 != 0); }
	static bool any(const float4x4& m) { return (m._m00 != 0) || (m._m01 != 0) || (m._m02 != 0) || (m._m03 != 0) || (m._m10 != 0) || (m._m11 != 0) || (m._m12 != 0) || (m._m13 != 0) || (m._m20 != 0) || (m._m21 != 0) || (m._m22 != 0) || (m._m23 != 0) || (m._m30 != 0) || (m._m31 != 0) || (m._m32 != 0) || (m._m33 != 0); }
#pragma endregion


#pragma region asin
	static float asin(const float& v) { return asinf(v); }

	static float1 asin(const float1& v) { return float1(asinf(v.x)); }
	static float2 asin(const float2& v) { return float2(asinf(v.x), asinf(v.y)); }
	static float3 asin(const float3& v) { return float3(asinf(v.x), asinf(v.y), asinf(v.z)); }
	static float4 asin(const float4& v) { return float4(asinf(v.x), asinf(v.y), asinf(v.z), asinf(v.w)); }

	static float1x1 asin(const float1x1& v) { return float1x1(asinf(v._m00)); }
	static float1x2 asin(const float1x2& v) { return float1x2(asinf(v._m00), asinf(v._m01)); }
	static float1x3 asin(const float1x3& v) { return float1x3(asinf(v._m00), asinf(v._m01), asinf(v._m02)); }
	static float1x4 asin(const float1x4& v) { return float1x4(asinf(v._m00), asinf(v._m01), asinf(v._m02), asinf(v._m03)); }
	static float2x1 asin(const float2x1& v) { return float2x1(asinf(v._m00), asinf(v._m10)); }
	static float2x2 asin(const float2x2& v) { return float2x2(asinf(v._m00), asinf(v._m01), asinf(v._m10), asinf(v._m11)); }
	static float2x3 asin(const float2x3& v) { return float2x3(asinf(v._m00), asinf(v._m01), asinf(v._m02), asinf(v._m10), asinf(v._m11), asinf(v._m12)); }
	static float2x4 asin(const float2x4& v) { return float2x4(asinf(v._m00), asinf(v._m01), asinf(v._m02), asinf(v._m03), asinf(v._m10), asinf(v._m11), asinf(v._m12), asinf(v._m13)); }
	static float3x1 asin(const float3x1& v) { return float3x1(asinf(v._m00), asinf(v._m10), asinf(v._m20)); }
	static float3x2 asin(const float3x2& v) { return float3x2(asinf(v._m00), asinf(v._m01), asinf(v._m10), asinf(v._m11), asinf(v._m20), asinf(v._m21)); }
	static float3x3 asin(const float3x3& v) { return float3x3(asinf(v._m00), asinf(v._m01), asinf(v._m02), asinf(v._m10), asinf(v._m11), asinf(v._m12), asinf(v._m20), asinf(v._m21), asinf(v._m22)); }
	static float3x4 asin(const float3x4& v) { return float3x4(asinf(v._m00), asinf(v._m01), asinf(v._m02), asinf(v._m03), asinf(v._m10), asinf(v._m11), asinf(v._m12), asinf(v._m13), asinf(v._m20), asinf(v._m21), asinf(v._m22), asinf(v._m23)); }
	static float4x1 asin(const float4x1& v) { return float4x1(asinf(v._m00), asinf(v._m10), asinf(v._m20), asinf(v._m30)); }
	static float4x2 asin(const float4x2& v) { return float4x2(asinf(v._m00), asinf(v._m01), asinf(v._m10), asinf(v._m11), asinf(v._m20), asinf(v._m21), asinf(v._m30), asinf(v._m31)); }
	static float4x3 asin(const float4x3& v) { return float4x3(asinf(v._m00), asinf(v._m01), asinf(v._m02), asinf(v._m10), asinf(v._m11), asinf(v._m12), asinf(v._m20), asinf(v._m21), asinf(v._m22), asinf(v._m30), asinf(v._m31), asinf(v._m32)); }
	static float4x4 asin(const float4x4& v) { return float4x4(asinf(v._m00), asinf(v._m01), asinf(v._m02), asinf(v._m03), asinf(v._m10), asinf(v._m11), asinf(v._m12), asinf(v._m13), asinf(v._m20), asinf(v._m21), asinf(v._m22), asinf(v._m23), asinf(v._m30), asinf(v._m31), asinf(v._m32), asinf(v._m33)); }

#pragma endregion


#pragma region atan
	static float atan(const float& v) { return atanf(v); }

	static float1 atan(const float1& v) { return float1(atanf(v.x)); }
	static float2 atan(const float2& v) { return float2(atanf(v.x), atanf(v.y)); }
	static float3 atan(const float3& v) { return float3(atanf(v.x), atanf(v.y), atanf(v.z)); }
	static float4 atan(const float4& v) { return float4(atanf(v.x), atanf(v.y), atanf(v.z), atanf(v.w)); }

	static float1x1 atan(const float1x1& v) { return float1x1(atanf(v._m00)); }
	static float1x2 atan(const float1x2& v) { return float1x2(atanf(v._m00), atanf(v._m01)); }
	static float1x3 atan(const float1x3& v) { return float1x3(atanf(v._m00), atanf(v._m01), atanf(v._m02)); }
	static float1x4 atan(const float1x4& v) { return float1x4(atanf(v._m00), atanf(v._m01), atanf(v._m02), atanf(v._m03)); }
	static float2x1 atan(const float2x1& v) { return float2x1(atanf(v._m00), atanf(v._m10)); }
	static float2x2 atan(const float2x2& v) { return float2x2(atanf(v._m00), atanf(v._m01), atanf(v._m10), atanf(v._m11)); }
	static float2x3 atan(const float2x3& v) { return float2x3(atanf(v._m00), atanf(v._m01), atanf(v._m02), atanf(v._m10), atanf(v._m11), atanf(v._m12)); }
	static float2x4 atan(const float2x4& v) { return float2x4(atanf(v._m00), atanf(v._m01), atanf(v._m02), atanf(v._m03), atanf(v._m10), atanf(v._m11), atanf(v._m12), atanf(v._m13)); }
	static float3x1 atan(const float3x1& v) { return float3x1(atanf(v._m00), atanf(v._m10), atanf(v._m20)); }
	static float3x2 atan(const float3x2& v) { return float3x2(atanf(v._m00), atanf(v._m01), atanf(v._m10), atanf(v._m11), atanf(v._m20), atanf(v._m21)); }
	static float3x3 atan(const float3x3& v) { return float3x3(atanf(v._m00), atanf(v._m01), atanf(v._m02), atanf(v._m10), atanf(v._m11), atanf(v._m12), atanf(v._m20), atanf(v._m21), atanf(v._m22)); }
	static float3x4 atan(const float3x4& v) { return float3x4(atanf(v._m00), atanf(v._m01), atanf(v._m02), atanf(v._m03), atanf(v._m10), atanf(v._m11), atanf(v._m12), atanf(v._m13), atanf(v._m20), atanf(v._m21), atanf(v._m22), atanf(v._m23)); }
	static float4x1 atan(const float4x1& v) { return float4x1(atanf(v._m00), atanf(v._m10), atanf(v._m20), atanf(v._m30)); }
	static float4x2 atan(const float4x2& v) { return float4x2(atanf(v._m00), atanf(v._m01), atanf(v._m10), atanf(v._m11), atanf(v._m20), atanf(v._m21), atanf(v._m30), atanf(v._m31)); }
	static float4x3 atan(const float4x3& v) { return float4x3(atanf(v._m00), atanf(v._m01), atanf(v._m02), atanf(v._m10), atanf(v._m11), atanf(v._m12), atanf(v._m20), atanf(v._m21), atanf(v._m22), atanf(v._m30), atanf(v._m31), atanf(v._m32)); }
	static float4x4 atan(const float4x4& v) { return float4x4(atanf(v._m00), atanf(v._m01), atanf(v._m02), atanf(v._m03), atanf(v._m10), atanf(v._m11), atanf(v._m12), atanf(v._m13), atanf(v._m20), atanf(v._m21), atanf(v._m22), atanf(v._m23), atanf(v._m30), atanf(v._m31), atanf(v._m32), atanf(v._m33)); }

#pragma endregion


#pragma region atan2
	static float atan2(const float& a, const float& b) { return atan2f(a, b); }

	static float1 atan2(const float1& a, const float1& b) { return float1(atan2f(a.x, b.x)); }
	static float2 atan2(const float2& a, const float2& b) { return float2(atan2f(a.x, b.x), atan2f(a.y, b.y)); }
	static float3 atan2(const float3& a, const float3& b) { return float3(atan2f(a.x, b.x), atan2f(a.y, b.y), atan2f(a.z, b.z)); }
	static float4 atan2(const float4& a, const float4& b) { return float4(atan2f(a.x, b.x), atan2f(a.y, b.y), atan2f(a.z, b.z), atan2f(a.w, b.w)); }

	static float1x1 atan2(const float1x1& a, const float1x1& b) { return float1x1(atan2f(a._m00, b._m00)); }
	static float1x2 atan2(const float1x2& a, const float1x2& b) { return float1x2(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01)); }
	static float1x3 atan2(const float1x3& a, const float1x3& b) { return float1x3(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m02, b._m02)); }
	static float1x4 atan2(const float1x4& a, const float1x4& b) { return float1x4(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m02, b._m02), atan2f(a._m03, b._m03)); }
	static float2x1 atan2(const float2x1& a, const float2x1& b) { return float2x1(atan2f(a._m00, b._m00), atan2f(a._m10, b._m10)); }
	static float2x2 atan2(const float2x2& a, const float2x2& b) { return float2x2(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11)); }
	static float2x3 atan2(const float2x3& a, const float2x3& b) { return float2x3(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m02, b._m02), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11), atan2f(a._m12, b._m12)); }
	static float2x4 atan2(const float2x4& a, const float2x4& b) { return float2x4(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m02, b._m02), atan2f(a._m03, b._m03), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11), atan2f(a._m12, b._m12), atan2f(a._m13, b._m13)); }
	static float3x1 atan2(const float3x1& a, const float3x1& b) { return float3x1(atan2f(a._m00, b._m00), atan2f(a._m10, b._m10), atan2f(a._m20, b._m20)); }
	static float3x2 atan2(const float3x2& a, const float3x2& b) { return float3x2(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11), atan2f(a._m20, b._m20), atan2f(a._m21, b._m21)); }
	static float3x3 atan2(const float3x3& a, const float3x3& b) { return float3x3(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m02, b._m02), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11), atan2f(a._m12, b._m12), atan2f(a._m20, b._m20), atan2f(a._m21, b._m21), atan2f(a._m22, b._m22)); }
	static float3x4 atan2(const float3x4& a, const float3x4& b) { return float3x4(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m02, b._m02), atan2f(a._m03, b._m03), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11), atan2f(a._m12, b._m12), atan2f(a._m13, b._m13), atan2f(a._m20, b._m20), atan2f(a._m21, b._m21), atan2f(a._m22, b._m22), atan2f(a._m23, b._m23)); }
	static float4x1 atan2(const float4x1& a, const float4x1& b) { return float4x1(atan2f(a._m00, b._m00), atan2f(a._m10, b._m10), atan2f(a._m20, b._m20), atan2f(a._m30, b._m30)); }
	static float4x2 atan2(const float4x2& a, const float4x2& b) { return float4x2(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11), atan2f(a._m20, b._m20), atan2f(a._m21, b._m21), atan2f(a._m30, b._m30), atan2f(a._m31, b._m31)); }
	static float4x3 atan2(const float4x3& a, const float4x3& b) { return float4x3(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m02, b._m02), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11), atan2f(a._m12, b._m12), atan2f(a._m20, b._m20), atan2f(a._m21, b._m21), atan2f(a._m22, b._m22), atan2f(a._m30, b._m30), atan2f(a._m31, b._m31), atan2f(a._m32, b._m32)); }
	static float4x4 atan2(const float4x4& a, const float4x4& b) { return float4x4(atan2f(a._m00, b._m00), atan2f(a._m01, b._m01), atan2f(a._m02, b._m02), atan2f(a._m03, b._m03), atan2f(a._m10, b._m10), atan2f(a._m11, b._m11), atan2f(a._m12, b._m12), atan2f(a._m13, b._m13), atan2f(a._m20, b._m20), atan2f(a._m21, b._m21), atan2f(a._m22, b._m22), atan2f(a._m23, b._m23), atan2f(a._m30, b._m30), atan2f(a._m31, b._m31), atan2f(a._m32, b._m32), atan2f(a._m33, b._m33)); }

#pragma endregion


#pragma region ceil
	static float ceil(const float& v) { return ceilf(v); }

	static float1 ceil(const float1& v) { return float1(ceilf(v.x)); }
	static float2 ceil(const float2& v) { return float2(ceilf(v.x), ceilf(v.y)); }
	static float3 ceil(const float3& v) { return float3(ceilf(v.x), ceilf(v.y), ceilf(v.z)); }
	static float4 ceil(const float4& v) { return float4(ceilf(v.x), ceilf(v.y), ceilf(v.z), ceilf(v.w)); }

	static float1x1 ceil(const float1x1& v) { return float1x1(ceilf(v._m00)); }
	static float1x2 ceil(const float1x2& v) { return float1x2(ceilf(v._m00), ceilf(v._m01)); }
	static float1x3 ceil(const float1x3& v) { return float1x3(ceilf(v._m00), ceilf(v._m01), ceilf(v._m02)); }
	static float1x4 ceil(const float1x4& v) { return float1x4(ceilf(v._m00), ceilf(v._m01), ceilf(v._m02), ceilf(v._m03)); }
	static float2x1 ceil(const float2x1& v) { return float2x1(ceilf(v._m00), ceilf(v._m10)); }
	static float2x2 ceil(const float2x2& v) { return float2x2(ceilf(v._m00), ceilf(v._m01), ceilf(v._m10), ceilf(v._m11)); }
	static float2x3 ceil(const float2x3& v) { return float2x3(ceilf(v._m00), ceilf(v._m01), ceilf(v._m02), ceilf(v._m10), ceilf(v._m11), ceilf(v._m12)); }
	static float2x4 ceil(const float2x4& v) { return float2x4(ceilf(v._m00), ceilf(v._m01), ceilf(v._m02), ceilf(v._m03), ceilf(v._m10), ceilf(v._m11), ceilf(v._m12), ceilf(v._m13)); }
	static float3x1 ceil(const float3x1& v) { return float3x1(ceilf(v._m00), ceilf(v._m10), ceilf(v._m20)); }
	static float3x2 ceil(const float3x2& v) { return float3x2(ceilf(v._m00), ceilf(v._m01), ceilf(v._m10), ceilf(v._m11), ceilf(v._m20), ceilf(v._m21)); }
	static float3x3 ceil(const float3x3& v) { return float3x3(ceilf(v._m00), ceilf(v._m01), ceilf(v._m02), ceilf(v._m10), ceilf(v._m11), ceilf(v._m12), ceilf(v._m20), ceilf(v._m21), ceilf(v._m22)); }
	static float3x4 ceil(const float3x4& v) { return float3x4(ceilf(v._m00), ceilf(v._m01), ceilf(v._m02), ceilf(v._m03), ceilf(v._m10), ceilf(v._m11), ceilf(v._m12), ceilf(v._m13), ceilf(v._m20), ceilf(v._m21), ceilf(v._m22), ceilf(v._m23)); }
	static float4x1 ceil(const float4x1& v) { return float4x1(ceilf(v._m00), ceilf(v._m10), ceilf(v._m20), ceilf(v._m30)); }
	static float4x2 ceil(const float4x2& v) { return float4x2(ceilf(v._m00), ceilf(v._m01), ceilf(v._m10), ceilf(v._m11), ceilf(v._m20), ceilf(v._m21), ceilf(v._m30), ceilf(v._m31)); }
	static float4x3 ceil(const float4x3& v) { return float4x3(ceilf(v._m00), ceilf(v._m01), ceilf(v._m02), ceilf(v._m10), ceilf(v._m11), ceilf(v._m12), ceilf(v._m20), ceilf(v._m21), ceilf(v._m22), ceilf(v._m30), ceilf(v._m31), ceilf(v._m32)); }
	static float4x4 ceil(const float4x4& v) { return float4x4(ceilf(v._m00), ceilf(v._m01), ceilf(v._m02), ceilf(v._m03), ceilf(v._m10), ceilf(v._m11), ceilf(v._m12), ceilf(v._m13), ceilf(v._m20), ceilf(v._m21), ceilf(v._m22), ceilf(v._m23), ceilf(v._m30), ceilf(v._m31), ceilf(v._m32), ceilf(v._m33)); }

#pragma endregion





#pragma region cos
	static float cos(const float& v) { return cosf(v); }

	static float1 cos(const float1& v) { return float1(cosf(v.x)); }
	static float2 cos(const float2& v) { return float2(cosf(v.x), cosf(v.y)); }
	static float3 cos(const float3& v) { return float3(cosf(v.x), cosf(v.y), cosf(v.z)); }
	static float4 cos(const float4& v) { return float4(cosf(v.x), cosf(v.y), cosf(v.z), cosf(v.w)); }

	static float1x1 cos(const float1x1& v) { return float1x1(cosf(v._m00)); }
	static float1x2 cos(const float1x2& v) { return float1x2(cosf(v._m00), cosf(v._m01)); }
	static float1x3 cos(const float1x3& v) { return float1x3(cosf(v._m00), cosf(v._m01), cosf(v._m02)); }
	static float1x4 cos(const float1x4& v) { return float1x4(cosf(v._m00), cosf(v._m01), cosf(v._m02), cosf(v._m03)); }
	static float2x1 cos(const float2x1& v) { return float2x1(cosf(v._m00), cosf(v._m10)); }
	static float2x2 cos(const float2x2& v) { return float2x2(cosf(v._m00), cosf(v._m01), cosf(v._m10), cosf(v._m11)); }
	static float2x3 cos(const float2x3& v) { return float2x3(cosf(v._m00), cosf(v._m01), cosf(v._m02), cosf(v._m10), cosf(v._m11), cosf(v._m12)); }
	static float2x4 cos(const float2x4& v) { return float2x4(cosf(v._m00), cosf(v._m01), cosf(v._m02), cosf(v._m03), cosf(v._m10), cosf(v._m11), cosf(v._m12), cosf(v._m13)); }
	static float3x1 cos(const float3x1& v) { return float3x1(cosf(v._m00), cosf(v._m10), cosf(v._m20)); }
	static float3x2 cos(const float3x2& v) { return float3x2(cosf(v._m00), cosf(v._m01), cosf(v._m10), cosf(v._m11), cosf(v._m20), cosf(v._m21)); }
	static float3x3 cos(const float3x3& v) { return float3x3(cosf(v._m00), cosf(v._m01), cosf(v._m02), cosf(v._m10), cosf(v._m11), cosf(v._m12), cosf(v._m20), cosf(v._m21), cosf(v._m22)); }
	static float3x4 cos(const float3x4& v) { return float3x4(cosf(v._m00), cosf(v._m01), cosf(v._m02), cosf(v._m03), cosf(v._m10), cosf(v._m11), cosf(v._m12), cosf(v._m13), cosf(v._m20), cosf(v._m21), cosf(v._m22), cosf(v._m23)); }
	static float4x1 cos(const float4x1& v) { return float4x1(cosf(v._m00), cosf(v._m10), cosf(v._m20), cosf(v._m30)); }
	static float4x2 cos(const float4x2& v) { return float4x2(cosf(v._m00), cosf(v._m01), cosf(v._m10), cosf(v._m11), cosf(v._m20), cosf(v._m21), cosf(v._m30), cosf(v._m31)); }
	static float4x3 cos(const float4x3& v) { return float4x3(cosf(v._m00), cosf(v._m01), cosf(v._m02), cosf(v._m10), cosf(v._m11), cosf(v._m12), cosf(v._m20), cosf(v._m21), cosf(v._m22), cosf(v._m30), cosf(v._m31), cosf(v._m32)); }
	static float4x4 cos(const float4x4& v) { return float4x4(cosf(v._m00), cosf(v._m01), cosf(v._m02), cosf(v._m03), cosf(v._m10), cosf(v._m11), cosf(v._m12), cosf(v._m13), cosf(v._m20), cosf(v._m21), cosf(v._m22), cosf(v._m23), cosf(v._m30), cosf(v._m31), cosf(v._m32), cosf(v._m33)); }

#pragma endregion


#pragma region cosh
	static float cosh(const float& v) { return coshf(v); }

	static float1 cosh(const float1& v) { return float1(coshf(v.x)); }
	static float2 cosh(const float2& v) { return float2(coshf(v.x), coshf(v.y)); }
	static float3 cosh(const float3& v) { return float3(coshf(v.x), coshf(v.y), coshf(v.z)); }
	static float4 cosh(const float4& v) { return float4(coshf(v.x), coshf(v.y), coshf(v.z), coshf(v.w)); }

	static float1x1 cosh(const float1x1& v) { return float1x1(coshf(v._m00)); }
	static float1x2 cosh(const float1x2& v) { return float1x2(coshf(v._m00), coshf(v._m01)); }
	static float1x3 cosh(const float1x3& v) { return float1x3(coshf(v._m00), coshf(v._m01), coshf(v._m02)); }
	static float1x4 cosh(const float1x4& v) { return float1x4(coshf(v._m00), coshf(v._m01), coshf(v._m02), coshf(v._m03)); }
	static float2x1 cosh(const float2x1& v) { return float2x1(coshf(v._m00), coshf(v._m10)); }
	static float2x2 cosh(const float2x2& v) { return float2x2(coshf(v._m00), coshf(v._m01), coshf(v._m10), coshf(v._m11)); }
	static float2x3 cosh(const float2x3& v) { return float2x3(coshf(v._m00), coshf(v._m01), coshf(v._m02), coshf(v._m10), coshf(v._m11), coshf(v._m12)); }
	static float2x4 cosh(const float2x4& v) { return float2x4(coshf(v._m00), coshf(v._m01), coshf(v._m02), coshf(v._m03), coshf(v._m10), coshf(v._m11), coshf(v._m12), coshf(v._m13)); }
	static float3x1 cosh(const float3x1& v) { return float3x1(coshf(v._m00), coshf(v._m10), coshf(v._m20)); }
	static float3x2 cosh(const float3x2& v) { return float3x2(coshf(v._m00), coshf(v._m01), coshf(v._m10), coshf(v._m11), coshf(v._m20), coshf(v._m21)); }
	static float3x3 cosh(const float3x3& v) { return float3x3(coshf(v._m00), coshf(v._m01), coshf(v._m02), coshf(v._m10), coshf(v._m11), coshf(v._m12), coshf(v._m20), coshf(v._m21), coshf(v._m22)); }
	static float3x4 cosh(const float3x4& v) { return float3x4(coshf(v._m00), coshf(v._m01), coshf(v._m02), coshf(v._m03), coshf(v._m10), coshf(v._m11), coshf(v._m12), coshf(v._m13), coshf(v._m20), coshf(v._m21), coshf(v._m22), coshf(v._m23)); }
	static float4x1 cosh(const float4x1& v) { return float4x1(coshf(v._m00), coshf(v._m10), coshf(v._m20), coshf(v._m30)); }
	static float4x2 cosh(const float4x2& v) { return float4x2(coshf(v._m00), coshf(v._m01), coshf(v._m10), coshf(v._m11), coshf(v._m20), coshf(v._m21), coshf(v._m30), coshf(v._m31)); }
	static float4x3 cosh(const float4x3& v) { return float4x3(coshf(v._m00), coshf(v._m01), coshf(v._m02), coshf(v._m10), coshf(v._m11), coshf(v._m12), coshf(v._m20), coshf(v._m21), coshf(v._m22), coshf(v._m30), coshf(v._m31), coshf(v._m32)); }
	static float4x4 cosh(const float4x4& v) { return float4x4(coshf(v._m00), coshf(v._m01), coshf(v._m02), coshf(v._m03), coshf(v._m10), coshf(v._m11), coshf(v._m12), coshf(v._m13), coshf(v._m20), coshf(v._m21), coshf(v._m22), coshf(v._m23), coshf(v._m30), coshf(v._m31), coshf(v._m32), coshf(v._m33)); }

#pragma endregion


#pragma region dot
	static float dot(const float1& a, const float1& b) { return a.x * b.x; }
	static float dot(const float2& a, const float2& b) { return a.x * b.x + a.y * b.y; }
	static float dot(const float3& a, const float3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	static float dot(const float4& a, const float4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
#pragma endregion


#pragma region minf
	static float minf(const float& a, const float& b) { return a < b ? a : b; }

	static float1 minf(const float1& a, const float1& b) { return float1(a.x < b.x ? a.x : b.x); }
	static float2 minf(const float2& a, const float2& b) { return float2(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y); }
	static float3 minf(const float3& a, const float3& b) { return float3(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z); }
	static float4 minf(const float4& a, const float4& b) { return float4(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z, a.w < b.w ? a.w : b.w); }

	static float1x1 minf(const float1x1& a, const float1x1& b) { return float1x1(a._m00 < b._m00 ? a._m00 : b._m00); }
	static float1x2 minf(const float1x2& a, const float1x2& b) { return float1x2(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01); }
	static float1x3 minf(const float1x3& a, const float1x3& b) { return float1x3(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m02 < b._m02 ? a._m02 : b._m02); }
	static float1x4 minf(const float1x4& a, const float1x4& b) { return float1x4(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m02 < b._m02 ? a._m02 : b._m02, a._m03 < b._m03 ? a._m03 : b._m03); }
	static float2x1 minf(const float2x1& a, const float2x1& b) { return float2x1(a._m00 < b._m00 ? a._m00 : b._m00, a._m10 < b._m10 ? a._m10 : b._m10); }
	static float2x2 minf(const float2x2& a, const float2x2& b) { return float2x2(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11); }
	static float2x3 minf(const float2x3& a, const float2x3& b) { return float2x3(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m02 < b._m02 ? a._m02 : b._m02, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11, a._m12 < b._m12 ? a._m12 : b._m12); }
	static float2x4 minf(const float2x4& a, const float2x4& b) { return float2x4(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m02 < b._m02 ? a._m02 : b._m02, a._m03 < b._m03 ? a._m03 : b._m03, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11, a._m12 < b._m12 ? a._m12 : b._m12, a._m13 < b._m13 ? a._m13 : b._m13); }
	static float3x1 minf(const float3x1& a, const float3x1& b) { return float3x1(a._m00 < b._m00 ? a._m00 : b._m00, a._m10 < b._m10 ? a._m10 : b._m10, a._m20 < b._m20 ? a._m20 : b._m20); }
	static float3x2 minf(const float3x2& a, const float3x2& b) { return float3x2(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11, a._m20 < b._m20 ? a._m20 : b._m20, a._m21 < b._m21 ? a._m21 : b._m21); }
	static float3x3 minf(const float3x3& a, const float3x3& b) { return float3x3(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m02 < b._m02 ? a._m02 : b._m02, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11, a._m12 < b._m12 ? a._m12 : b._m12, a._m20 < b._m20 ? a._m20 : b._m20, a._m21 < b._m21 ? a._m21 : b._m21, a._m22 < b._m22 ? a._m22 : b._m22); }
	static float3x4 minf(const float3x4& a, const float3x4& b) { return float3x4(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m02 < b._m02 ? a._m02 : b._m02, a._m03 < b._m03 ? a._m03 : b._m03, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11, a._m12 < b._m12 ? a._m12 : b._m12, a._m13 < b._m13 ? a._m13 : b._m13, a._m20 < b._m20 ? a._m20 : b._m20, a._m21 < b._m21 ? a._m21 : b._m21, a._m22 < b._m22 ? a._m22 : b._m22, a._m23 < b._m23 ? a._m23 : b._m23); }
	static float4x1 minf(const float4x1& a, const float4x1& b) { return float4x1(a._m00 < b._m00 ? a._m00 : b._m00, a._m10 < b._m10 ? a._m10 : b._m10, a._m20 < b._m20 ? a._m20 : b._m20, a._m30 < b._m30 ? a._m30 : b._m30); }
	static float4x2 minf(const float4x2& a, const float4x2& b) { return float4x2(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11, a._m20 < b._m20 ? a._m20 : b._m20, a._m21 < b._m21 ? a._m21 : b._m21, a._m30 < b._m30 ? a._m30 : b._m30, a._m31 < b._m31 ? a._m31 : b._m31); }
	static float4x3 minf(const float4x3& a, const float4x3& b) { return float4x3(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m02 < b._m02 ? a._m02 : b._m02, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11, a._m12 < b._m12 ? a._m12 : b._m12, a._m20 < b._m20 ? a._m20 : b._m20, a._m21 < b._m21 ? a._m21 : b._m21, a._m22 < b._m22 ? a._m22 : b._m22, a._m30 < b._m30 ? a._m30 : b._m30, a._m31 < b._m31 ? a._m31 : b._m31, a._m32 < b._m32 ? a._m32 : b._m32); }
	static float4x4 minf(const float4x4& a, const float4x4& b) { return float4x4(a._m00 < b._m00 ? a._m00 : b._m00, a._m01 < b._m01 ? a._m01 : b._m01, a._m02 < b._m02 ? a._m02 : b._m02, a._m03 < b._m03 ? a._m03 : b._m03, a._m10 < b._m10 ? a._m10 : b._m10, a._m11 < b._m11 ? a._m11 : b._m11, a._m12 < b._m12 ? a._m12 : b._m12, a._m13 < b._m13 ? a._m13 : b._m13, a._m20 < b._m20 ? a._m20 : b._m20, a._m21 < b._m21 ? a._m21 : b._m21, a._m22 < b._m22 ? a._m22 : b._m22, a._m23 < b._m23 ? a._m23 : b._m23, a._m30 < b._m30 ? a._m30 : b._m30, a._m31 < b._m31 ? a._m31 : b._m31, a._m32 < b._m32 ? a._m32 : b._m32, a._m33 < b._m33 ? a._m33 : b._m33); }

#pragma endregion


#pragma region maxf
	static float maxf(const float& a, const float& b) { return a > b ? a : b; }

	static float1 maxf(const float1& a, const float1& b) { return float1(a.x > b.x ? a.x : b.x); }
	static float2 maxf(const float2& a, const float2& b) { return float2(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y); }
	static float3 maxf(const float3& a, const float3& b) { return float3(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z); }
	static float4 maxf(const float4& a, const float4& b) { return float4(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z, a.w > b.w ? a.w : b.w); }

	static float1x1 maxf(const float1x1& a, const float1x1& b) { return float1x1(a._m00 > b._m00 ? a._m00 : b._m00); }
	static float1x2 maxf(const float1x2& a, const float1x2& b) { return float1x2(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01); }
	static float1x3 maxf(const float1x3& a, const float1x3& b) { return float1x3(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m02 > b._m02 ? a._m02 : b._m02); }
	static float1x4 maxf(const float1x4& a, const float1x4& b) { return float1x4(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m02 > b._m02 ? a._m02 : b._m02, a._m03 > b._m03 ? a._m03 : b._m03); }
	static float2x1 maxf(const float2x1& a, const float2x1& b) { return float2x1(a._m00 > b._m00 ? a._m00 : b._m00, a._m10 > b._m10 ? a._m10 : b._m10); }
	static float2x2 maxf(const float2x2& a, const float2x2& b) { return float2x2(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11); }
	static float2x3 maxf(const float2x3& a, const float2x3& b) { return float2x3(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m02 > b._m02 ? a._m02 : b._m02, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11, a._m12 > b._m12 ? a._m12 : b._m12); }
	static float2x4 maxf(const float2x4& a, const float2x4& b) { return float2x4(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m02 > b._m02 ? a._m02 : b._m02, a._m03 > b._m03 ? a._m03 : b._m03, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11, a._m12 > b._m12 ? a._m12 : b._m12, a._m13 > b._m13 ? a._m13 : b._m13); }
	static float3x1 maxf(const float3x1& a, const float3x1& b) { return float3x1(a._m00 > b._m00 ? a._m00 : b._m00, a._m10 > b._m10 ? a._m10 : b._m10, a._m20 > b._m20 ? a._m20 : b._m20); }
	static float3x2 maxf(const float3x2& a, const float3x2& b) { return float3x2(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11, a._m20 > b._m20 ? a._m20 : b._m20, a._m21 > b._m21 ? a._m21 : b._m21); }
	static float3x3 maxf(const float3x3& a, const float3x3& b) { return float3x3(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m02 > b._m02 ? a._m02 : b._m02, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11, a._m12 > b._m12 ? a._m12 : b._m12, a._m20 > b._m20 ? a._m20 : b._m20, a._m21 > b._m21 ? a._m21 : b._m21, a._m22 > b._m22 ? a._m22 : b._m22); }
	static float3x4 maxf(const float3x4& a, const float3x4& b) { return float3x4(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m02 > b._m02 ? a._m02 : b._m02, a._m03 > b._m03 ? a._m03 : b._m03, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11, a._m12 > b._m12 ? a._m12 : b._m12, a._m13 > b._m13 ? a._m13 : b._m13, a._m20 > b._m20 ? a._m20 : b._m20, a._m21 > b._m21 ? a._m21 : b._m21, a._m22 > b._m22 ? a._m22 : b._m22, a._m23 > b._m23 ? a._m23 : b._m23); }
	static float4x1 maxf(const float4x1& a, const float4x1& b) { return float4x1(a._m00 > b._m00 ? a._m00 : b._m00, a._m10 > b._m10 ? a._m10 : b._m10, a._m20 > b._m20 ? a._m20 : b._m20, a._m30 > b._m30 ? a._m30 : b._m30); }
	static float4x2 maxf(const float4x2& a, const float4x2& b) { return float4x2(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11, a._m20 > b._m20 ? a._m20 : b._m20, a._m21 > b._m21 ? a._m21 : b._m21, a._m30 > b._m30 ? a._m30 : b._m30, a._m31 > b._m31 ? a._m31 : b._m31); }
	static float4x3 maxf(const float4x3& a, const float4x3& b) { return float4x3(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m02 > b._m02 ? a._m02 : b._m02, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11, a._m12 > b._m12 ? a._m12 : b._m12, a._m20 > b._m20 ? a._m20 : b._m20, a._m21 > b._m21 ? a._m21 : b._m21, a._m22 > b._m22 ? a._m22 : b._m22, a._m30 > b._m30 ? a._m30 : b._m30, a._m31 > b._m31 ? a._m31 : b._m31, a._m32 > b._m32 ? a._m32 : b._m32); }
	static float4x4 maxf(const float4x4& a, const float4x4& b) { return float4x4(a._m00 > b._m00 ? a._m00 : b._m00, a._m01 > b._m01 ? a._m01 : b._m01, a._m02 > b._m02 ? a._m02 : b._m02, a._m03 > b._m03 ? a._m03 : b._m03, a._m10 > b._m10 ? a._m10 : b._m10, a._m11 > b._m11 ? a._m11 : b._m11, a._m12 > b._m12 ? a._m12 : b._m12, a._m13 > b._m13 ? a._m13 : b._m13, a._m20 > b._m20 ? a._m20 : b._m20, a._m21 > b._m21 ? a._m21 : b._m21, a._m22 > b._m22 ? a._m22 : b._m22, a._m23 > b._m23 ? a._m23 : b._m23, a._m30 > b._m30 ? a._m30 : b._m30, a._m31 > b._m31 ? a._m31 : b._m31, a._m32 > b._m32 ? a._m32 : b._m32, a._m33 > b._m33 ? a._m33 : b._m33); }

#pragma endregion


#pragma region degrees
	static float degrees(const float& v) { return (v * 180.0f / PI); }

	static float1 degrees(const float1& v) { return float1((v.x * 180.0f / PI)); }
	static float2 degrees(const float2& v) { return float2((v.x * 180.0f / PI), (v.y * 180.0f / PI)); }
	static float3 degrees(const float3& v) { return float3((v.x * 180.0f / PI), (v.y * 180.0f / PI), (v.z * 180.0f / PI)); }
	static float4 degrees(const float4& v) { return float4((v.x * 180.0f / PI), (v.y * 180.0f / PI), (v.z * 180.0f / PI), (v.w * 180.0f / PI)); }

	static float1x1 degrees(const float1x1& v) { return float1x1((v._m00 * 180.0f / PI)); }
	static float1x2 degrees(const float1x2& v) { return float1x2((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI)); }
	static float1x3 degrees(const float1x3& v) { return float1x3((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m02 * 180.0f / PI)); }
	static float1x4 degrees(const float1x4& v) { return float1x4((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m02 * 180.0f / PI), (v._m03 * 180.0f / PI)); }
	static float2x1 degrees(const float2x1& v) { return float2x1((v._m00 * 180.0f / PI), (v._m10 * 180.0f / PI)); }
	static float2x2 degrees(const float2x2& v) { return float2x2((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI)); }
	static float2x3 degrees(const float2x3& v) { return float2x3((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m02 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI), (v._m12 * 180.0f / PI)); }
	static float2x4 degrees(const float2x4& v) { return float2x4((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m02 * 180.0f / PI), (v._m03 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI), (v._m12 * 180.0f / PI), (v._m13 * 180.0f / PI)); }
	static float3x1 degrees(const float3x1& v) { return float3x1((v._m00 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m20 * 180.0f / PI)); }
	static float3x2 degrees(const float3x2& v) { return float3x2((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI), (v._m20 * 180.0f / PI), (v._m21 * 180.0f / PI)); }
	static float3x3 degrees(const float3x3& v) { return float3x3((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m02 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI), (v._m12 * 180.0f / PI), (v._m20 * 180.0f / PI), (v._m21 * 180.0f / PI), (v._m22 * 180.0f / PI)); }
	static float3x4 degrees(const float3x4& v) { return float3x4((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m02 * 180.0f / PI), (v._m03 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI), (v._m12 * 180.0f / PI), (v._m13 * 180.0f / PI), (v._m20 * 180.0f / PI), (v._m21 * 180.0f / PI), (v._m22 * 180.0f / PI), (v._m23 * 180.0f / PI)); }
	static float4x1 degrees(const float4x1& v) { return float4x1((v._m00 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m20 * 180.0f / PI), (v._m30 * 180.0f / PI)); }
	static float4x2 degrees(const float4x2& v) { return float4x2((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI), (v._m20 * 180.0f / PI), (v._m21 * 180.0f / PI), (v._m30 * 180.0f / PI), (v._m31 * 180.0f / PI)); }
	static float4x3 degrees(const float4x3& v) { return float4x3((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m02 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI), (v._m12 * 180.0f / PI), (v._m20 * 180.0f / PI), (v._m21 * 180.0f / PI), (v._m22 * 180.0f / PI), (v._m30 * 180.0f / PI), (v._m31 * 180.0f / PI), (v._m32 * 180.0f / PI)); }
	static float4x4 degrees(const float4x4& v) { return float4x4((v._m00 * 180.0f / PI), (v._m01 * 180.0f / PI), (v._m02 * 180.0f / PI), (v._m03 * 180.0f / PI), (v._m10 * 180.0f / PI), (v._m11 * 180.0f / PI), (v._m12 * 180.0f / PI), (v._m13 * 180.0f / PI), (v._m20 * 180.0f / PI), (v._m21 * 180.0f / PI), (v._m22 * 180.0f / PI), (v._m23 * 180.0f / PI), (v._m30 * 180.0f / PI), (v._m31 * 180.0f / PI), (v._m32 * 180.0f / PI), (v._m33 * 180.0f / PI)); }

#pragma endregion

#pragma region clamp
	static float clamp(const float& a, const float& b, const float& c) { return maxf(b, minf(c, a)); }
	static float1 clamp(const float1& a, const float1& b, const float1& c) { return maxf(b, minf(c, a)); }
	static float2 clamp(const float2& a, const float2& b, const float2& c) { return maxf(b, minf(c, a)); }
	static float3 clamp(const float3& a, const float3& b, const float3& c) { return maxf(b, minf(c, a)); }
	static float4 clamp(const float4& a, const float4& b, const float4& c) { return maxf(b, minf(c, a)); }
	static float1x1 clamp(const float1x1& a, const float1x1& b, const float1x1& c) { return maxf(b, minf(c, a)); }
	static float1x2 clamp(const float1x2& a, const float1x2& b, const float1x2& c) { return maxf(b, minf(c, a)); }
	static float1x3 clamp(const float1x3& a, const float1x3& b, const float1x3& c) { return maxf(b, minf(c, a)); }
	static float1x4 clamp(const float1x4& a, const float1x4& b, const float1x4& c) { return maxf(b, minf(c, a)); }
	static float2x1 clamp(const float2x1& a, const float2x1& b, const float2x1& c) { return maxf(b, minf(c, a)); }
	static float2x2 clamp(const float2x2& a, const float2x2& b, const float2x2& c) { return maxf(b, minf(c, a)); }
	static float2x3 clamp(const float2x3& a, const float2x3& b, const float2x3& c) { return maxf(b, minf(c, a)); }
	static float2x4 clamp(const float2x4& a, const float2x4& b, const float2x4& c) { return maxf(b, minf(c, a)); }
	static float3x1 clamp(const float3x1& a, const float3x1& b, const float3x1& c) { return maxf(b, minf(c, a)); }
	static float3x2 clamp(const float3x2& a, const float3x2& b, const float3x2& c) { return maxf(b, minf(c, a)); }
	static float3x3 clamp(const float3x3& a, const float3x3& b, const float3x3& c) { return maxf(b, minf(c, a)); }
	static float3x4 clamp(const float3x4& a, const float3x4& b, const float3x4& c) { return maxf(b, minf(c, a)); }
	static float4x1 clamp(const float4x1& a, const float4x1& b, const float4x1& c) { return maxf(b, minf(c, a)); }
	static float4x2 clamp(const float4x2& a, const float4x2& b, const float4x2& c) { return maxf(b, minf(c, a)); }
	static float4x3 clamp(const float4x3& a, const float4x3& b, const float4x3& c) { return maxf(b, minf(c, a)); }
	static float4x4 clamp(const float4x4& a, const float4x4& b, const float4x4& c) { return maxf(b, minf(c, a)); }
#pragma endregion


#pragma region length
	static float length(const float1& v) { return sqrtf(dot(v, v)); }
	static float length(const float2& v) { return sqrtf(dot(v, v)); }
	static float length(const float3& v) { return sqrtf(dot(v, v)); }
	static float length(const float4& v) { return sqrtf(dot(v, v)); }
#pragma endregion


#pragma region sqrlength
	static float sqrlength(const float1& v) { return dot(v, v); }
	static float sqrlength(const float2& v) { return dot(v, v); }
	static float sqrlength(const float3& v) { return dot(v, v); }
	static float sqrlength(const float4& v) { return dot(v, v); }
#pragma endregion


#pragma region distance
	static float distance(const float1& a, const float1& b) { return length(a - b); }
	static float distance(const float2& a, const float2& b) { return length(a - b); }
	static float distance(const float3& a, const float3& b) { return length(a - b); }
	static float distance(const float4& a, const float4& b) { return length(a - b); }
#pragma endregion


#pragma region sqrdistance
	static float sqrdistance(const float1& a, const float1& b) { return sqrlength(a - b); }
	static float sqrdistance(const float2& a, const float2& b) { return sqrlength(a - b); }
	static float sqrdistance(const float3& a, const float3& b) { return sqrlength(a - b); }
	static float sqrdistance(const float4& a, const float4& b) { return sqrlength(a - b); }
#pragma endregion


#pragma region exp
	static float exp(const float& v) { return expf(v); }

	static float1 exp(const float1& v) { return float1(expf(v.x)); }
	static float2 exp(const float2& v) { return float2(expf(v.x), expf(v.y)); }
	static float3 exp(const float3& v) { return float3(expf(v.x), expf(v.y), expf(v.z)); }
	static float4 exp(const float4& v) { return float4(expf(v.x), expf(v.y), expf(v.z), expf(v.w)); }

	static float1x1 exp(const float1x1& v) { return float1x1(expf(v._m00)); }
	static float1x2 exp(const float1x2& v) { return float1x2(expf(v._m00), expf(v._m01)); }
	static float1x3 exp(const float1x3& v) { return float1x3(expf(v._m00), expf(v._m01), expf(v._m02)); }
	static float1x4 exp(const float1x4& v) { return float1x4(expf(v._m00), expf(v._m01), expf(v._m02), expf(v._m03)); }
	static float2x1 exp(const float2x1& v) { return float2x1(expf(v._m00), expf(v._m10)); }
	static float2x2 exp(const float2x2& v) { return float2x2(expf(v._m00), expf(v._m01), expf(v._m10), expf(v._m11)); }
	static float2x3 exp(const float2x3& v) { return float2x3(expf(v._m00), expf(v._m01), expf(v._m02), expf(v._m10), expf(v._m11), expf(v._m12)); }
	static float2x4 exp(const float2x4& v) { return float2x4(expf(v._m00), expf(v._m01), expf(v._m02), expf(v._m03), expf(v._m10), expf(v._m11), expf(v._m12), expf(v._m13)); }
	static float3x1 exp(const float3x1& v) { return float3x1(expf(v._m00), expf(v._m10), expf(v._m20)); }
	static float3x2 exp(const float3x2& v) { return float3x2(expf(v._m00), expf(v._m01), expf(v._m10), expf(v._m11), expf(v._m20), expf(v._m21)); }
	static float3x3 exp(const float3x3& v) { return float3x3(expf(v._m00), expf(v._m01), expf(v._m02), expf(v._m10), expf(v._m11), expf(v._m12), expf(v._m20), expf(v._m21), expf(v._m22)); }
	static float3x4 exp(const float3x4& v) { return float3x4(expf(v._m00), expf(v._m01), expf(v._m02), expf(v._m03), expf(v._m10), expf(v._m11), expf(v._m12), expf(v._m13), expf(v._m20), expf(v._m21), expf(v._m22), expf(v._m23)); }
	static float4x1 exp(const float4x1& v) { return float4x1(expf(v._m00), expf(v._m10), expf(v._m20), expf(v._m30)); }
	static float4x2 exp(const float4x2& v) { return float4x2(expf(v._m00), expf(v._m01), expf(v._m10), expf(v._m11), expf(v._m20), expf(v._m21), expf(v._m30), expf(v._m31)); }
	static float4x3 exp(const float4x3& v) { return float4x3(expf(v._m00), expf(v._m01), expf(v._m02), expf(v._m10), expf(v._m11), expf(v._m12), expf(v._m20), expf(v._m21), expf(v._m22), expf(v._m30), expf(v._m31), expf(v._m32)); }
	static float4x4 exp(const float4x4& v) { return float4x4(expf(v._m00), expf(v._m01), expf(v._m02), expf(v._m03), expf(v._m10), expf(v._m11), expf(v._m12), expf(v._m13), expf(v._m20), expf(v._m21), expf(v._m22), expf(v._m23), expf(v._m30), expf(v._m31), expf(v._m32), expf(v._m33)); }

#pragma endregion


#pragma region exp2
	static float exp2(const float& v) { return exp2f(v); }

	static float1 exp2(const float1& v) { return float1(exp2f(v.x)); }
	static float2 exp2(const float2& v) { return float2(exp2f(v.x), exp2f(v.y)); }
	static float3 exp2(const float3& v) { return float3(exp2f(v.x), exp2f(v.y), exp2f(v.z)); }
	static float4 exp2(const float4& v) { return float4(exp2f(v.x), exp2f(v.y), exp2f(v.z), exp2f(v.w)); }

	static float1x1 exp2(const float1x1& v) { return float1x1(exp2f(v._m00)); }
	static float1x2 exp2(const float1x2& v) { return float1x2(exp2f(v._m00), exp2f(v._m01)); }
	static float1x3 exp2(const float1x3& v) { return float1x3(exp2f(v._m00), exp2f(v._m01), exp2f(v._m02)); }
	static float1x4 exp2(const float1x4& v) { return float1x4(exp2f(v._m00), exp2f(v._m01), exp2f(v._m02), exp2f(v._m03)); }
	static float2x1 exp2(const float2x1& v) { return float2x1(exp2f(v._m00), exp2f(v._m10)); }
	static float2x2 exp2(const float2x2& v) { return float2x2(exp2f(v._m00), exp2f(v._m01), exp2f(v._m10), exp2f(v._m11)); }
	static float2x3 exp2(const float2x3& v) { return float2x3(exp2f(v._m00), exp2f(v._m01), exp2f(v._m02), exp2f(v._m10), exp2f(v._m11), exp2f(v._m12)); }
	static float2x4 exp2(const float2x4& v) { return float2x4(exp2f(v._m00), exp2f(v._m01), exp2f(v._m02), exp2f(v._m03), exp2f(v._m10), exp2f(v._m11), exp2f(v._m12), exp2f(v._m13)); }
	static float3x1 exp2(const float3x1& v) { return float3x1(exp2f(v._m00), exp2f(v._m10), exp2f(v._m20)); }
	static float3x2 exp2(const float3x2& v) { return float3x2(exp2f(v._m00), exp2f(v._m01), exp2f(v._m10), exp2f(v._m11), exp2f(v._m20), exp2f(v._m21)); }
	static float3x3 exp2(const float3x3& v) { return float3x3(exp2f(v._m00), exp2f(v._m01), exp2f(v._m02), exp2f(v._m10), exp2f(v._m11), exp2f(v._m12), exp2f(v._m20), exp2f(v._m21), exp2f(v._m22)); }
	static float3x4 exp2(const float3x4& v) { return float3x4(exp2f(v._m00), exp2f(v._m01), exp2f(v._m02), exp2f(v._m03), exp2f(v._m10), exp2f(v._m11), exp2f(v._m12), exp2f(v._m13), exp2f(v._m20), exp2f(v._m21), exp2f(v._m22), exp2f(v._m23)); }
	static float4x1 exp2(const float4x1& v) { return float4x1(exp2f(v._m00), exp2f(v._m10), exp2f(v._m20), exp2f(v._m30)); }
	static float4x2 exp2(const float4x2& v) { return float4x2(exp2f(v._m00), exp2f(v._m01), exp2f(v._m10), exp2f(v._m11), exp2f(v._m20), exp2f(v._m21), exp2f(v._m30), exp2f(v._m31)); }
	static float4x3 exp2(const float4x3& v) { return float4x3(exp2f(v._m00), exp2f(v._m01), exp2f(v._m02), exp2f(v._m10), exp2f(v._m11), exp2f(v._m12), exp2f(v._m20), exp2f(v._m21), exp2f(v._m22), exp2f(v._m30), exp2f(v._m31), exp2f(v._m32)); }
	static float4x4 exp2(const float4x4& v) { return float4x4(exp2f(v._m00), exp2f(v._m01), exp2f(v._m02), exp2f(v._m03), exp2f(v._m10), exp2f(v._m11), exp2f(v._m12), exp2f(v._m13), exp2f(v._m20), exp2f(v._m21), exp2f(v._m22), exp2f(v._m23), exp2f(v._m30), exp2f(v._m31), exp2f(v._m32), exp2f(v._m33)); }

#pragma endregion


#pragma region floor
	static float floor(const float& v) { return floorf(v); }

	static float1 floor(const float1& v) { return float1(floorf(v.x)); }
	static float2 floor(const float2& v) { return float2(floorf(v.x), floorf(v.y)); }
	static float3 floor(const float3& v) { return float3(floorf(v.x), floorf(v.y), floorf(v.z)); }
	static float4 floor(const float4& v) { return float4(floorf(v.x), floorf(v.y), floorf(v.z), floorf(v.w)); }

	static float1x1 floor(const float1x1& v) { return float1x1(floorf(v._m00)); }
	static float1x2 floor(const float1x2& v) { return float1x2(floorf(v._m00), floorf(v._m01)); }
	static float1x3 floor(const float1x3& v) { return float1x3(floorf(v._m00), floorf(v._m01), floorf(v._m02)); }
	static float1x4 floor(const float1x4& v) { return float1x4(floorf(v._m00), floorf(v._m01), floorf(v._m02), floorf(v._m03)); }
	static float2x1 floor(const float2x1& v) { return float2x1(floorf(v._m00), floorf(v._m10)); }
	static float2x2 floor(const float2x2& v) { return float2x2(floorf(v._m00), floorf(v._m01), floorf(v._m10), floorf(v._m11)); }
	static float2x3 floor(const float2x3& v) { return float2x3(floorf(v._m00), floorf(v._m01), floorf(v._m02), floorf(v._m10), floorf(v._m11), floorf(v._m12)); }
	static float2x4 floor(const float2x4& v) { return float2x4(floorf(v._m00), floorf(v._m01), floorf(v._m02), floorf(v._m03), floorf(v._m10), floorf(v._m11), floorf(v._m12), floorf(v._m13)); }
	static float3x1 floor(const float3x1& v) { return float3x1(floorf(v._m00), floorf(v._m10), floorf(v._m20)); }
	static float3x2 floor(const float3x2& v) { return float3x2(floorf(v._m00), floorf(v._m01), floorf(v._m10), floorf(v._m11), floorf(v._m20), floorf(v._m21)); }
	static float3x3 floor(const float3x3& v) { return float3x3(floorf(v._m00), floorf(v._m01), floorf(v._m02), floorf(v._m10), floorf(v._m11), floorf(v._m12), floorf(v._m20), floorf(v._m21), floorf(v._m22)); }
	static float3x4 floor(const float3x4& v) { return float3x4(floorf(v._m00), floorf(v._m01), floorf(v._m02), floorf(v._m03), floorf(v._m10), floorf(v._m11), floorf(v._m12), floorf(v._m13), floorf(v._m20), floorf(v._m21), floorf(v._m22), floorf(v._m23)); }
	static float4x1 floor(const float4x1& v) { return float4x1(floorf(v._m00), floorf(v._m10), floorf(v._m20), floorf(v._m30)); }
	static float4x2 floor(const float4x2& v) { return float4x2(floorf(v._m00), floorf(v._m01), floorf(v._m10), floorf(v._m11), floorf(v._m20), floorf(v._m21), floorf(v._m30), floorf(v._m31)); }
	static float4x3 floor(const float4x3& v) { return float4x3(floorf(v._m00), floorf(v._m01), floorf(v._m02), floorf(v._m10), floorf(v._m11), floorf(v._m12), floorf(v._m20), floorf(v._m21), floorf(v._m22), floorf(v._m30), floorf(v._m31), floorf(v._m32)); }
	static float4x4 floor(const float4x4& v) { return float4x4(floorf(v._m00), floorf(v._m01), floorf(v._m02), floorf(v._m03), floorf(v._m10), floorf(v._m11), floorf(v._m12), floorf(v._m13), floorf(v._m20), floorf(v._m21), floorf(v._m22), floorf(v._m23), floorf(v._m30), floorf(v._m31), floorf(v._m32), floorf(v._m33)); }

#pragma endregion


#pragma region fmod
	static float fmod(float a, float b) { return fmodf(a, b); }

	static float1 fmod(const float1& a, const float1& b) { return float1(fmod(a.x, b.x)); }
	static float2 fmod(const float2& a, const float2& b) { return float2(fmod(a.x, b.x), fmod(a.y, b.y)); }
	static float3 fmod(const float3& a, const float3& b) { return float3(fmod(a.x, b.x), fmod(a.y, b.y), fmod(a.z, b.z)); }
	static float4 fmod(const float4& a, const float4& b) { return float4(fmod(a.x, b.x), fmod(a.y, b.y), fmod(a.z, b.z), fmod(a.w, b.w)); }

	static float1x1 fmod(const float1x1& a, const float1x1& b) { return float1x1(fmod(a._m00, b._m00)); }
	static float1x2 fmod(const float1x2& a, const float1x2& b) { return float1x2(fmod(a._m00, b._m00), fmod(a._m01, b._m01)); }
	static float1x3 fmod(const float1x3& a, const float1x3& b) { return float1x3(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02)); }
	static float1x4 fmod(const float1x4& a, const float1x4& b) { return float1x4(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m03, b._m03)); }
	static float2x1 fmod(const float2x1& a, const float2x1& b) { return float2x1(fmod(a._m00, b._m00), fmod(a._m10, b._m10)); }
	static float2x2 fmod(const float2x2& a, const float2x2& b) { return float2x2(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m10, b._m10), fmod(a._m11, b._m11)); }
	static float2x3 fmod(const float2x3& a, const float2x3& b) { return float2x3(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12)); }
	static float2x4 fmod(const float2x4& a, const float2x4& b) { return float2x4(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m03, b._m03), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m13, b._m13)); }
	static float3x1 fmod(const float3x1& a, const float3x1& b) { return float3x1(fmod(a._m00, b._m00), fmod(a._m10, b._m10), fmod(a._m20, b._m20)); }
	static float3x2 fmod(const float3x2& a, const float3x2& b) { return float3x2(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m20, b._m20), fmod(a._m21, b._m21)); }
	static float3x3 fmod(const float3x3& a, const float3x3& b) { return float3x3(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m22, b._m22)); }
	static float3x4 fmod(const float3x4& a, const float3x4& b) { return float3x4(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m03, b._m03), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m13, b._m13), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m22, b._m22), fmod(a._m23, b._m23)); }
	static float4x1 fmod(const float4x1& a, const float4x1& b) { return float4x1(fmod(a._m00, b._m00), fmod(a._m10, b._m10), fmod(a._m20, b._m20), fmod(a._m30, b._m30)); }
	static float4x2 fmod(const float4x2& a, const float4x2& b) { return float4x2(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m30, b._m30), fmod(a._m31, b._m31)); }
	static float4x3 fmod(const float4x3& a, const float4x3& b) { return float4x3(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m22, b._m22), fmod(a._m30, b._m30), fmod(a._m31, b._m31), fmod(a._m32, b._m32)); }
	static float4x4 fmod(const float4x4& a, const float4x4& b) { return float4x4(fmod(a._m00, b._m00), fmod(a._m01, b._m01), fmod(a._m02, b._m02), fmod(a._m03, b._m03), fmod(a._m10, b._m10), fmod(a._m11, b._m11), fmod(a._m12, b._m12), fmod(a._m13, b._m13), fmod(a._m20, b._m20), fmod(a._m21, b._m21), fmod(a._m22, b._m22), fmod(a._m23, b._m23), fmod(a._m30, b._m30), fmod(a._m31, b._m31), fmod(a._m32, b._m32), fmod(a._m33, b._m33)); }

#pragma endregion


#pragma region frac
	static float frac(const float& v) { return fmod(v, 1); }
	static float1 frac(const float1& v) { return fmod(v, 1); }
	static float2 frac(const float2& v) { return fmod(v, 1); }
	static float3 frac(const float3& v) { return fmod(v, 1); }
	static float4 frac(const float4& v) { return fmod(v, 1); }
	static float1x1 frac(const float1x1& v) { return fmod(v, 1); }
	static float1x2 frac(const float1x2& v) { return fmod(v, 1); }
	static float1x3 frac(const float1x3& v) { return fmod(v, 1); }
	static float1x4 frac(const float1x4& v) { return fmod(v, 1); }
	static float2x1 frac(const float2x1& v) { return fmod(v, 1); }
	static float2x2 frac(const float2x2& v) { return fmod(v, 1); }
	static float2x3 frac(const float2x3& v) { return fmod(v, 1); }
	static float2x4 frac(const float2x4& v) { return fmod(v, 1); }
	static float3x1 frac(const float3x1& v) { return fmod(v, 1); }
	static float3x2 frac(const float3x2& v) { return fmod(v, 1); }
	static float3x3 frac(const float3x3& v) { return fmod(v, 1); }
	static float3x4 frac(const float3x4& v) { return fmod(v, 1); }
	static float4x1 frac(const float4x1& v) { return fmod(v, 1); }
	static float4x2 frac(const float4x2& v) { return fmod(v, 1); }
	static float4x3 frac(const float4x3& v) { return fmod(v, 1); }
	static float4x4 frac(const float4x4& v) { return fmod(v, 1); }
#pragma endregion


#pragma region ldexp
	static float ldexp(const float& a, const float& b) { return a * exp2(b); }
	static float1 ldexp(const float1& a, const float1& b) { return a * exp2(b); }
	static float2 ldexp(const float2& a, const float2& b) { return a * exp2(b); }
	static float3 ldexp(const float3& a, const float3& b) { return a * exp2(b); }
	static float4 ldexp(const float4& a, const float4& b) { return a * exp2(b); }
	static float1x1 ldexp(const float1x1& a, const float1x1& b) { return a * exp2(b); }
	static float1x2 ldexp(const float1x2& a, const float1x2& b) { return a * exp2(b); }
	static float1x3 ldexp(const float1x3& a, const float1x3& b) { return a * exp2(b); }
	static float1x4 ldexp(const float1x4& a, const float1x4& b) { return a * exp2(b); }
	static float2x1 ldexp(const float2x1& a, const float2x1& b) { return a * exp2(b); }
	static float2x2 ldexp(const float2x2& a, const float2x2& b) { return a * exp2(b); }
	static float2x3 ldexp(const float2x3& a, const float2x3& b) { return a * exp2(b); }
	static float2x4 ldexp(const float2x4& a, const float2x4& b) { return a * exp2(b); }
	static float3x1 ldexp(const float3x1& a, const float3x1& b) { return a * exp2(b); }
	static float3x2 ldexp(const float3x2& a, const float3x2& b) { return a * exp2(b); }
	static float3x3 ldexp(const float3x3& a, const float3x3& b) { return a * exp2(b); }
	static float3x4 ldexp(const float3x4& a, const float3x4& b) { return a * exp2(b); }
	static float4x1 ldexp(const float4x1& a, const float4x1& b) { return a * exp2(b); }
	static float4x2 ldexp(const float4x2& a, const float4x2& b) { return a * exp2(b); }
	static float4x3 ldexp(const float4x3& a, const float4x3& b) { return a * exp2(b); }
	static float4x4 ldexp(const float4x4& a, const float4x4& b) { return a * exp2(b); }
#pragma endregion


#pragma region lerp
	static float lerp(const float& a, const float& b, const float& c) { return a + c * (b - a); }
	static float1 lerp(const float1& a, const float1& b, const float1& c) { return a + c * (b - a); }
	static float2 lerp(const float2& a, const float2& b, const float2& c) { return a + c * (b - a); }
	static float3 lerp(const float3& a, const float3& b, const float3& c) { return a + c * (b - a); }
	static float4 lerp(const float4& a, const float4& b, const float4& c) { return a + c * (b - a); }
	static float1x1 lerp(const float1x1& a, const float1x1& b, const float1x1& c) { return a + c * (b - a); }
	static float1x2 lerp(const float1x2& a, const float1x2& b, const float1x2& c) { return a + c * (b - a); }
	static float1x3 lerp(const float1x3& a, const float1x3& b, const float1x3& c) { return a + c * (b - a); }
	static float1x4 lerp(const float1x4& a, const float1x4& b, const float1x4& c) { return a + c * (b - a); }
	static float2x1 lerp(const float2x1& a, const float2x1& b, const float2x1& c) { return a + c * (b - a); }
	static float2x2 lerp(const float2x2& a, const float2x2& b, const float2x2& c) { return a + c * (b - a); }
	static float2x3 lerp(const float2x3& a, const float2x3& b, const float2x3& c) { return a + c * (b - a); }
	static float2x4 lerp(const float2x4& a, const float2x4& b, const float2x4& c) { return a + c * (b - a); }
	static float3x1 lerp(const float3x1& a, const float3x1& b, const float3x1& c) { return a + c * (b - a); }
	static float3x2 lerp(const float3x2& a, const float3x2& b, const float3x2& c) { return a + c * (b - a); }
	static float3x3 lerp(const float3x3& a, const float3x3& b, const float3x3& c) { return a + c * (b - a); }
	static float3x4 lerp(const float3x4& a, const float3x4& b, const float3x4& c) { return a + c * (b - a); }
	static float4x1 lerp(const float4x1& a, const float4x1& b, const float4x1& c) { return a + c * (b - a); }
	static float4x2 lerp(const float4x2& a, const float4x2& b, const float4x2& c) { return a + c * (b - a); }
	static float4x3 lerp(const float4x3& a, const float4x3& b, const float4x3& c) { return a + c * (b - a); }
	static float4x4 lerp(const float4x4& a, const float4x4& b, const float4x4& c) { return a + c * (b - a); }
#pragma endregion


#pragma region log
	static float log(const float& v) { return logf(v); }

	static float1 log(const float1& v) { return float1(logf(v.x)); }
	static float2 log(const float2& v) { return float2(logf(v.x), logf(v.y)); }
	static float3 log(const float3& v) { return float3(logf(v.x), logf(v.y), logf(v.z)); }
	static float4 log(const float4& v) { return float4(logf(v.x), logf(v.y), logf(v.z), logf(v.w)); }

	static float1x1 log(const float1x1& v) { return float1x1(logf(v._m00)); }
	static float1x2 log(const float1x2& v) { return float1x2(logf(v._m00), logf(v._m01)); }
	static float1x3 log(const float1x3& v) { return float1x3(logf(v._m00), logf(v._m01), logf(v._m02)); }
	static float1x4 log(const float1x4& v) { return float1x4(logf(v._m00), logf(v._m01), logf(v._m02), logf(v._m03)); }
	static float2x1 log(const float2x1& v) { return float2x1(logf(v._m00), logf(v._m10)); }
	static float2x2 log(const float2x2& v) { return float2x2(logf(v._m00), logf(v._m01), logf(v._m10), logf(v._m11)); }
	static float2x3 log(const float2x3& v) { return float2x3(logf(v._m00), logf(v._m01), logf(v._m02), logf(v._m10), logf(v._m11), logf(v._m12)); }
	static float2x4 log(const float2x4& v) { return float2x4(logf(v._m00), logf(v._m01), logf(v._m02), logf(v._m03), logf(v._m10), logf(v._m11), logf(v._m12), logf(v._m13)); }
	static float3x1 log(const float3x1& v) { return float3x1(logf(v._m00), logf(v._m10), logf(v._m20)); }
	static float3x2 log(const float3x2& v) { return float3x2(logf(v._m00), logf(v._m01), logf(v._m10), logf(v._m11), logf(v._m20), logf(v._m21)); }
	static float3x3 log(const float3x3& v) { return float3x3(logf(v._m00), logf(v._m01), logf(v._m02), logf(v._m10), logf(v._m11), logf(v._m12), logf(v._m20), logf(v._m21), logf(v._m22)); }
	static float3x4 log(const float3x4& v) { return float3x4(logf(v._m00), logf(v._m01), logf(v._m02), logf(v._m03), logf(v._m10), logf(v._m11), logf(v._m12), logf(v._m13), logf(v._m20), logf(v._m21), logf(v._m22), logf(v._m23)); }
	static float4x1 log(const float4x1& v) { return float4x1(logf(v._m00), logf(v._m10), logf(v._m20), logf(v._m30)); }
	static float4x2 log(const float4x2& v) { return float4x2(logf(v._m00), logf(v._m01), logf(v._m10), logf(v._m11), logf(v._m20), logf(v._m21), logf(v._m30), logf(v._m31)); }
	static float4x3 log(const float4x3& v) { return float4x3(logf(v._m00), logf(v._m01), logf(v._m02), logf(v._m10), logf(v._m11), logf(v._m12), logf(v._m20), logf(v._m21), logf(v._m22), logf(v._m30), logf(v._m31), logf(v._m32)); }
	static float4x4 log(const float4x4& v) { return float4x4(logf(v._m00), logf(v._m01), logf(v._m02), logf(v._m03), logf(v._m10), logf(v._m11), logf(v._m12), logf(v._m13), logf(v._m20), logf(v._m21), logf(v._m22), logf(v._m23), logf(v._m30), logf(v._m31), logf(v._m32), logf(v._m33)); }

#pragma endregion


#pragma region log10
	static float log10(const float& v) { return log10f(v); }

	static float1 log10(const float1& v) { return float1(log10f(v.x)); }
	static float2 log10(const float2& v) { return float2(log10f(v.x), log10f(v.y)); }
	static float3 log10(const float3& v) { return float3(log10f(v.x), log10f(v.y), log10f(v.z)); }
	static float4 log10(const float4& v) { return float4(log10f(v.x), log10f(v.y), log10f(v.z), log10f(v.w)); }

	static float1x1 log10(const float1x1& v) { return float1x1(log10f(v._m00)); }
	static float1x2 log10(const float1x2& v) { return float1x2(log10f(v._m00), log10f(v._m01)); }
	static float1x3 log10(const float1x3& v) { return float1x3(log10f(v._m00), log10f(v._m01), log10f(v._m02)); }
	static float1x4 log10(const float1x4& v) { return float1x4(log10f(v._m00), log10f(v._m01), log10f(v._m02), log10f(v._m03)); }
	static float2x1 log10(const float2x1& v) { return float2x1(log10f(v._m00), log10f(v._m10)); }
	static float2x2 log10(const float2x2& v) { return float2x2(log10f(v._m00), log10f(v._m01), log10f(v._m10), log10f(v._m11)); }
	static float2x3 log10(const float2x3& v) { return float2x3(log10f(v._m00), log10f(v._m01), log10f(v._m02), log10f(v._m10), log10f(v._m11), log10f(v._m12)); }
	static float2x4 log10(const float2x4& v) { return float2x4(log10f(v._m00), log10f(v._m01), log10f(v._m02), log10f(v._m03), log10f(v._m10), log10f(v._m11), log10f(v._m12), log10f(v._m13)); }
	static float3x1 log10(const float3x1& v) { return float3x1(log10f(v._m00), log10f(v._m10), log10f(v._m20)); }
	static float3x2 log10(const float3x2& v) { return float3x2(log10f(v._m00), log10f(v._m01), log10f(v._m10), log10f(v._m11), log10f(v._m20), log10f(v._m21)); }
	static float3x3 log10(const float3x3& v) { return float3x3(log10f(v._m00), log10f(v._m01), log10f(v._m02), log10f(v._m10), log10f(v._m11), log10f(v._m12), log10f(v._m20), log10f(v._m21), log10f(v._m22)); }
	static float3x4 log10(const float3x4& v) { return float3x4(log10f(v._m00), log10f(v._m01), log10f(v._m02), log10f(v._m03), log10f(v._m10), log10f(v._m11), log10f(v._m12), log10f(v._m13), log10f(v._m20), log10f(v._m21), log10f(v._m22), log10f(v._m23)); }
	static float4x1 log10(const float4x1& v) { return float4x1(log10f(v._m00), log10f(v._m10), log10f(v._m20), log10f(v._m30)); }
	static float4x2 log10(const float4x2& v) { return float4x2(log10f(v._m00), log10f(v._m01), log10f(v._m10), log10f(v._m11), log10f(v._m20), log10f(v._m21), log10f(v._m30), log10f(v._m31)); }
	static float4x3 log10(const float4x3& v) { return float4x3(log10f(v._m00), log10f(v._m01), log10f(v._m02), log10f(v._m10), log10f(v._m11), log10f(v._m12), log10f(v._m20), log10f(v._m21), log10f(v._m22), log10f(v._m30), log10f(v._m31), log10f(v._m32)); }
	static float4x4 log10(const float4x4& v) { return float4x4(log10f(v._m00), log10f(v._m01), log10f(v._m02), log10f(v._m03), log10f(v._m10), log10f(v._m11), log10f(v._m12), log10f(v._m13), log10f(v._m20), log10f(v._m21), log10f(v._m22), log10f(v._m23), log10f(v._m30), log10f(v._m31), log10f(v._m32), log10f(v._m33)); }

#pragma endregion


#pragma region log2
	static float log2(const float& v) { return log2f(v); }

	static float1 log2(const float1& v) { return float1(log2f(v.x)); }
	static float2 log2(const float2& v) { return float2(log2f(v.x), log2f(v.y)); }
	static float3 log2(const float3& v) { return float3(log2f(v.x), log2f(v.y), log2f(v.z)); }
	static float4 log2(const float4& v) { return float4(log2f(v.x), log2f(v.y), log2f(v.z), log2f(v.w)); }

	static float1x1 log2(const float1x1& v) { return float1x1(log2f(v._m00)); }
	static float1x2 log2(const float1x2& v) { return float1x2(log2f(v._m00), log2f(v._m01)); }
	static float1x3 log2(const float1x3& v) { return float1x3(log2f(v._m00), log2f(v._m01), log2f(v._m02)); }
	static float1x4 log2(const float1x4& v) { return float1x4(log2f(v._m00), log2f(v._m01), log2f(v._m02), log2f(v._m03)); }
	static float2x1 log2(const float2x1& v) { return float2x1(log2f(v._m00), log2f(v._m10)); }
	static float2x2 log2(const float2x2& v) { return float2x2(log2f(v._m00), log2f(v._m01), log2f(v._m10), log2f(v._m11)); }
	static float2x3 log2(const float2x3& v) { return float2x3(log2f(v._m00), log2f(v._m01), log2f(v._m02), log2f(v._m10), log2f(v._m11), log2f(v._m12)); }
	static float2x4 log2(const float2x4& v) { return float2x4(log2f(v._m00), log2f(v._m01), log2f(v._m02), log2f(v._m03), log2f(v._m10), log2f(v._m11), log2f(v._m12), log2f(v._m13)); }
	static float3x1 log2(const float3x1& v) { return float3x1(log2f(v._m00), log2f(v._m10), log2f(v._m20)); }
	static float3x2 log2(const float3x2& v) { return float3x2(log2f(v._m00), log2f(v._m01), log2f(v._m10), log2f(v._m11), log2f(v._m20), log2f(v._m21)); }
	static float3x3 log2(const float3x3& v) { return float3x3(log2f(v._m00), log2f(v._m01), log2f(v._m02), log2f(v._m10), log2f(v._m11), log2f(v._m12), log2f(v._m20), log2f(v._m21), log2f(v._m22)); }
	static float3x4 log2(const float3x4& v) { return float3x4(log2f(v._m00), log2f(v._m01), log2f(v._m02), log2f(v._m03), log2f(v._m10), log2f(v._m11), log2f(v._m12), log2f(v._m13), log2f(v._m20), log2f(v._m21), log2f(v._m22), log2f(v._m23)); }
	static float4x1 log2(const float4x1& v) { return float4x1(log2f(v._m00), log2f(v._m10), log2f(v._m20), log2f(v._m30)); }
	static float4x2 log2(const float4x2& v) { return float4x2(log2f(v._m00), log2f(v._m01), log2f(v._m10), log2f(v._m11), log2f(v._m20), log2f(v._m21), log2f(v._m30), log2f(v._m31)); }
	static float4x3 log2(const float4x3& v) { return float4x3(log2f(v._m00), log2f(v._m01), log2f(v._m02), log2f(v._m10), log2f(v._m11), log2f(v._m12), log2f(v._m20), log2f(v._m21), log2f(v._m22), log2f(v._m30), log2f(v._m31), log2f(v._m32)); }
	static float4x4 log2(const float4x4& v) { return float4x4(log2f(v._m00), log2f(v._m01), log2f(v._m02), log2f(v._m03), log2f(v._m10), log2f(v._m11), log2f(v._m12), log2f(v._m13), log2f(v._m20), log2f(v._m21), log2f(v._m22), log2f(v._m23), log2f(v._m30), log2f(v._m31), log2f(v._m32), log2f(v._m33)); }

#pragma endregion


#pragma region mul
	static float1x1 mul(const float1x1& a, const float1x1& b) { return float1x1(a._m00 * b._m00); }
	static float1x2 mul(const float1x1& a, const float1x2& b) { return float1x2(a._m00 * b._m00, a._m00 * b._m01); }
	static float1x3 mul(const float1x1& a, const float1x3& b) { return float1x3(a._m00 * b._m00, a._m00 * b._m01, a._m00 * b._m02); }
	static float1x4 mul(const float1x1& a, const float1x4& b) { return float1x4(a._m00 * b._m00, a._m00 * b._m01, a._m00 * b._m02, a._m00 * b._m03); }
	static float1x1 mul(const float1x2& a, const float2x1& b) { return float1x1(a._m00 * b._m00 + a._m01 * b._m10); }
	static float1x2 mul(const float1x2& a, const float2x2& b) { return float1x2(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11); }
	static float1x3 mul(const float1x2& a, const float2x3& b) { return float1x3(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m00 * b._m02 + a._m01 * b._m12); }
	static float1x4 mul(const float1x2& a, const float2x4& b) { return float1x4(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m00 * b._m02 + a._m01 * b._m12, a._m00 * b._m03 + a._m01 * b._m13); }
	static float1x1 mul(const float1x3& a, const float3x1& b) { return float1x1(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20); }
	static float1x2 mul(const float1x3& a, const float3x2& b) { return float1x2(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21); }
	static float1x3 mul(const float1x3& a, const float3x3& b) { return float1x3(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22); }
	static float1x4 mul(const float1x3& a, const float3x4& b) { return float1x4(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22, a._m00 * b._m03 + a._m01 * b._m13 + a._m02 * b._m23); }
	static float1x1 mul(const float1x4& a, const float4x1& b) { return float1x1(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30); }
	static float1x2 mul(const float1x4& a, const float4x2& b) { return float1x2(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31); }
	static float1x3 mul(const float1x4& a, const float4x3& b) { return float1x3(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22 + a._m03 * b._m32); }
	static float1x4 mul(const float1x4& a, const float4x4& b) { return float1x4(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22 + a._m03 * b._m32, a._m00 * b._m03 + a._m01 * b._m13 + a._m02 * b._m23 + a._m03 * b._m33); }
	static float4 mul(const float4& a, const float4x4& b) {
		return float4(
			a.x * b._m00 + a.y * b._m10 + a.z * b._m20 + a.w * b._m30,
			a.x * b._m01 + a.y * b._m11 + a.z * b._m21 + a.w * b._m31,
			a.x * b._m02 + a.y * b._m12 + a.z * b._m22 + a.w * b._m32,
			a.x * b._m03 + a.y * b._m13 + a.z * b._m23 + a.w * b._m33);
	}
	static float2x1 mul(const float2x1& a, const float1x1& b) { return float2x1(a._m00 * b._m00, a._m10 * b._m00); }
	static float2x2 mul(const float2x1& a, const float1x2& b) { return float2x2(a._m00 * b._m00, a._m00 * b._m01, a._m10 * b._m00, a._m10 * b._m01); }
	static float2x3 mul(const float2x1& a, const float1x3& b) { return float2x3(a._m00 * b._m00, a._m00 * b._m01, a._m00 * b._m02, a._m10 * b._m00, a._m10 * b._m01, a._m10 * b._m02); }
	static float2x4 mul(const float2x1& a, const float1x4& b) { return float2x4(a._m00 * b._m00, a._m00 * b._m01, a._m00 * b._m02, a._m00 * b._m03, a._m10 * b._m00, a._m10 * b._m01, a._m10 * b._m02, a._m10 * b._m03); }
	static float2x1 mul(const float2x2& a, const float2x1& b) { return float2x1(a._m00 * b._m00 + a._m01 * b._m10, a._m10 * b._m00 + a._m11 * b._m10); }
	static float2x2 mul(const float2x2& a, const float2x2& b) { return float2x2(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11); }
	static float2x3 mul(const float2x2& a, const float2x3& b) { return float2x3(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m00 * b._m02 + a._m01 * b._m12, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11, a._m10 * b._m02 + a._m11 * b._m12); }
	static float2x4 mul(const float2x2& a, const float2x4& b) { return float2x4(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m00 * b._m02 + a._m01 * b._m12, a._m00 * b._m03 + a._m01 * b._m13, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11, a._m10 * b._m02 + a._m11 * b._m12, a._m10 * b._m03 + a._m11 * b._m13); }
	static float2x1 mul(const float2x3& a, const float3x1& b) { return float2x1(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20); }
	static float2x2 mul(const float2x3& a, const float3x2& b) { return float2x2(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21); }
	static float2x3 mul(const float2x3& a, const float3x3& b) { return float2x3(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22); }
	static float2x4 mul(const float2x3& a, const float3x4& b) { return float2x4(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22, a._m00 * b._m03 + a._m01 * b._m13 + a._m02 * b._m23, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22, a._m10 * b._m03 + a._m11 * b._m13 + a._m12 * b._m23); }
	static float2x1 mul(const float2x4& a, const float4x1& b) { return float2x1(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30); }
	static float2x2 mul(const float2x4& a, const float4x2& b) { return float2x2(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31); }
	static float2x3 mul(const float2x4& a, const float4x3& b) { return float2x3(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22 + a._m03 * b._m32, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22 + a._m13 * b._m32); }
	static float2x4 mul(const float2x4& a, const float4x4& b) { return float2x4(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22 + a._m03 * b._m32, a._m00 * b._m03 + a._m01 * b._m13 + a._m02 * b._m23 + a._m03 * b._m33, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22 + a._m13 * b._m32, a._m10 * b._m03 + a._m11 * b._m13 + a._m12 * b._m23 + a._m13 * b._m33); }
	static float3x1 mul(const float3x1& a, const float1x1& b) { return float3x1(a._m00 * b._m00, a._m10 * b._m00, a._m20 * b._m00); }
	static float3x2 mul(const float3x1& a, const float1x2& b) { return float3x2(a._m00 * b._m00, a._m00 * b._m01, a._m10 * b._m00, a._m10 * b._m01, a._m20 * b._m00, a._m20 * b._m01); }
	static float3x3 mul(const float3x1& a, const float1x3& b) { return float3x3(a._m00 * b._m00, a._m00 * b._m01, a._m00 * b._m02, a._m10 * b._m00, a._m10 * b._m01, a._m10 * b._m02, a._m20 * b._m00, a._m20 * b._m01, a._m20 * b._m02); }
	static float3x4 mul(const float3x1& a, const float1x4& b) { return float3x4(a._m00 * b._m00, a._m00 * b._m01, a._m00 * b._m02, a._m00 * b._m03, a._m10 * b._m00, a._m10 * b._m01, a._m10 * b._m02, a._m10 * b._m03, a._m20 * b._m00, a._m20 * b._m01, a._m20 * b._m02, a._m20 * b._m03); }
	static float3x1 mul(const float3x2& a, const float2x1& b) { return float3x1(a._m00 * b._m00 + a._m01 * b._m10, a._m10 * b._m00 + a._m11 * b._m10, a._m20 * b._m00 + a._m21 * b._m10); }
	static float3x2 mul(const float3x2& a, const float2x2& b) { return float3x2(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11, a._m20 * b._m00 + a._m21 * b._m10, a._m20 * b._m01 + a._m21 * b._m11); }
	static float3x3 mul(const float3x2& a, const float2x3& b) { return float3x3(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m00 * b._m02 + a._m01 * b._m12, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11, a._m10 * b._m02 + a._m11 * b._m12, a._m20 * b._m00 + a._m21 * b._m10, a._m20 * b._m01 + a._m21 * b._m11, a._m20 * b._m02 + a._m21 * b._m12); }
	static float3x4 mul(const float3x2& a, const float2x4& b) { return float3x4(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m00 * b._m02 + a._m01 * b._m12, a._m00 * b._m03 + a._m01 * b._m13, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11, a._m10 * b._m02 + a._m11 * b._m12, a._m10 * b._m03 + a._m11 * b._m13, a._m20 * b._m00 + a._m21 * b._m10, a._m20 * b._m01 + a._m21 * b._m11, a._m20 * b._m02 + a._m21 * b._m12, a._m20 * b._m03 + a._m21 * b._m13); }
	static float3x1 mul(const float3x3& a, const float3x1& b) { return float3x1(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20); }
	static float3x2 mul(const float3x3& a, const float3x2& b) { return float3x2(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21); }
	static float3x3 mul(const float3x3& a, const float3x3& b) { return float3x3(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21, a._m20 * b._m02 + a._m21 * b._m12 + a._m22 * b._m22); }
	static float3x4 mul(const float3x3& a, const float3x4& b) { return float3x4(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22, a._m00 * b._m03 + a._m01 * b._m13 + a._m02 * b._m23, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22, a._m10 * b._m03 + a._m11 * b._m13 + a._m12 * b._m23, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21, a._m20 * b._m02 + a._m21 * b._m12 + a._m22 * b._m22, a._m20 * b._m03 + a._m21 * b._m13 + a._m22 * b._m23); }
	static float3x1 mul(const float3x4& a, const float4x1& b) { return float3x1(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20 + a._m23 * b._m30); }
	static float3x2 mul(const float3x4& a, const float4x2& b) { return float3x2(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20 + a._m23 * b._m30, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21 + a._m23 * b._m31); }
	static float3x3 mul(const float3x4& a, const float4x3& b) { return float3x3(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22 + a._m03 * b._m32, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22 + a._m13 * b._m32, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20 + a._m23 * b._m30, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21 + a._m23 * b._m31, a._m20 * b._m02 + a._m21 * b._m12 + a._m22 * b._m22 + a._m23 * b._m32); }
	static float3x4 mul(const float3x4& a, const float4x4& b) { return float3x4(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22 + a._m03 * b._m32, a._m00 * b._m03 + a._m01 * b._m13 + a._m02 * b._m23 + a._m03 * b._m33, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22 + a._m13 * b._m32, a._m10 * b._m03 + a._m11 * b._m13 + a._m12 * b._m23 + a._m13 * b._m33, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20 + a._m23 * b._m30, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21 + a._m23 * b._m31, a._m20 * b._m02 + a._m21 * b._m12 + a._m22 * b._m22 + a._m23 * b._m32, a._m20 * b._m03 + a._m21 * b._m13 + a._m22 * b._m23 + a._m23 * b._m33); }
	static float4x1 mul(const float4x1& a, const float1x1& b) { return float4x1(a._m00 * b._m00, a._m10 * b._m00, a._m20 * b._m00, a._m30 * b._m00); }
	static float4x2 mul(const float4x1& a, const float1x2& b) { return float4x2(a._m00 * b._m00, a._m00 * b._m01, a._m10 * b._m00, a._m10 * b._m01, a._m20 * b._m00, a._m20 * b._m01, a._m30 * b._m00, a._m30 * b._m01); }
	static float4x3 mul(const float4x1& a, const float1x3& b) { return float4x3(a._m00 * b._m00, a._m00 * b._m01, a._m00 * b._m02, a._m10 * b._m00, a._m10 * b._m01, a._m10 * b._m02, a._m20 * b._m00, a._m20 * b._m01, a._m20 * b._m02, a._m30 * b._m00, a._m30 * b._m01, a._m30 * b._m02); }
	static float4x4 mul(const float4x1& a, const float1x4& b) { return float4x4(a._m00 * b._m00, a._m00 * b._m01, a._m00 * b._m02, a._m00 * b._m03, a._m10 * b._m00, a._m10 * b._m01, a._m10 * b._m02, a._m10 * b._m03, a._m20 * b._m00, a._m20 * b._m01, a._m20 * b._m02, a._m20 * b._m03, a._m30 * b._m00, a._m30 * b._m01, a._m30 * b._m02, a._m30 * b._m03); }
	static float4x1 mul(const float4x2& a, const float2x1& b) { return float4x1(a._m00 * b._m00 + a._m01 * b._m10, a._m10 * b._m00 + a._m11 * b._m10, a._m20 * b._m00 + a._m21 * b._m10, a._m30 * b._m00 + a._m31 * b._m10); }
	static float4x2 mul(const float4x2& a, const float2x2& b) { return float4x2(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11, a._m20 * b._m00 + a._m21 * b._m10, a._m20 * b._m01 + a._m21 * b._m11, a._m30 * b._m00 + a._m31 * b._m10, a._m30 * b._m01 + a._m31 * b._m11); }
	static float4x3 mul(const float4x2& a, const float2x3& b) { return float4x3(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m00 * b._m02 + a._m01 * b._m12, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11, a._m10 * b._m02 + a._m11 * b._m12, a._m20 * b._m00 + a._m21 * b._m10, a._m20 * b._m01 + a._m21 * b._m11, a._m20 * b._m02 + a._m21 * b._m12, a._m30 * b._m00 + a._m31 * b._m10, a._m30 * b._m01 + a._m31 * b._m11, a._m30 * b._m02 + a._m31 * b._m12); }
	static float4x4 mul(const float4x2& a, const float2x4& b) { return float4x4(a._m00 * b._m00 + a._m01 * b._m10, a._m00 * b._m01 + a._m01 * b._m11, a._m00 * b._m02 + a._m01 * b._m12, a._m00 * b._m03 + a._m01 * b._m13, a._m10 * b._m00 + a._m11 * b._m10, a._m10 * b._m01 + a._m11 * b._m11, a._m10 * b._m02 + a._m11 * b._m12, a._m10 * b._m03 + a._m11 * b._m13, a._m20 * b._m00 + a._m21 * b._m10, a._m20 * b._m01 + a._m21 * b._m11, a._m20 * b._m02 + a._m21 * b._m12, a._m20 * b._m03 + a._m21 * b._m13, a._m30 * b._m00 + a._m31 * b._m10, a._m30 * b._m01 + a._m31 * b._m11, a._m30 * b._m02 + a._m31 * b._m12, a._m30 * b._m03 + a._m31 * b._m13); }
	static float4x1 mul(const float4x3& a, const float3x1& b) { return float4x1(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20, a._m30 * b._m00 + a._m31 * b._m10 + a._m32 * b._m20); }
	static float4x2 mul(const float4x3& a, const float3x2& b) { return float4x2(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21, a._m30 * b._m00 + a._m31 * b._m10 + a._m32 * b._m20, a._m30 * b._m01 + a._m31 * b._m11 + a._m32 * b._m21); }
	static float4x3 mul(const float4x3& a, const float3x3& b) { return float4x3(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21, a._m20 * b._m02 + a._m21 * b._m12 + a._m22 * b._m22, a._m30 * b._m00 + a._m31 * b._m10 + a._m32 * b._m20, a._m30 * b._m01 + a._m31 * b._m11 + a._m32 * b._m21, a._m30 * b._m02 + a._m31 * b._m12 + a._m32 * b._m22); }
	static float4x4 mul(const float4x3& a, const float3x4& b) { return float4x4(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22, a._m00 * b._m03 + a._m01 * b._m13 + a._m02 * b._m23, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22, a._m10 * b._m03 + a._m11 * b._m13 + a._m12 * b._m23, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21, a._m20 * b._m02 + a._m21 * b._m12 + a._m22 * b._m22, a._m20 * b._m03 + a._m21 * b._m13 + a._m22 * b._m23, a._m30 * b._m00 + a._m31 * b._m10 + a._m32 * b._m20, a._m30 * b._m01 + a._m31 * b._m11 + a._m32 * b._m21, a._m30 * b._m02 + a._m31 * b._m12 + a._m32 * b._m22, a._m30 * b._m03 + a._m31 * b._m13 + a._m32 * b._m23); }
	static float4x1 mul(const float4x4& a, const float4x1& b) { return float4x1(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20 + a._m23 * b._m30, a._m30 * b._m00 + a._m31 * b._m10 + a._m32 * b._m20 + a._m33 * b._m30); }
	static float4x2 mul(const float4x4& a, const float4x2& b) { return float4x2(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20 + a._m23 * b._m30, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21 + a._m23 * b._m31, a._m30 * b._m00 + a._m31 * b._m10 + a._m32 * b._m20 + a._m33 * b._m30, a._m30 * b._m01 + a._m31 * b._m11 + a._m32 * b._m21 + a._m33 * b._m31); }
	static float4x3 mul(const float4x4& a, const float4x3& b) { return float4x3(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22 + a._m03 * b._m32, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22 + a._m13 * b._m32, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20 + a._m23 * b._m30, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21 + a._m23 * b._m31, a._m20 * b._m02 + a._m21 * b._m12 + a._m22 * b._m22 + a._m23 * b._m32, a._m30 * b._m00 + a._m31 * b._m10 + a._m32 * b._m20 + a._m33 * b._m30, a._m30 * b._m01 + a._m31 * b._m11 + a._m32 * b._m21 + a._m33 * b._m31, a._m30 * b._m02 + a._m31 * b._m12 + a._m32 * b._m22 + a._m33 * b._m32); }
	static float4x4 mul(const float4x4& a, const float4x4& b) { return float4x4(a._m00 * b._m00 + a._m01 * b._m10 + a._m02 * b._m20 + a._m03 * b._m30, a._m00 * b._m01 + a._m01 * b._m11 + a._m02 * b._m21 + a._m03 * b._m31, a._m00 * b._m02 + a._m01 * b._m12 + a._m02 * b._m22 + a._m03 * b._m32, a._m00 * b._m03 + a._m01 * b._m13 + a._m02 * b._m23 + a._m03 * b._m33, a._m10 * b._m00 + a._m11 * b._m10 + a._m12 * b._m20 + a._m13 * b._m30, a._m10 * b._m01 + a._m11 * b._m11 + a._m12 * b._m21 + a._m13 * b._m31, a._m10 * b._m02 + a._m11 * b._m12 + a._m12 * b._m22 + a._m13 * b._m32, a._m10 * b._m03 + a._m11 * b._m13 + a._m12 * b._m23 + a._m13 * b._m33, a._m20 * b._m00 + a._m21 * b._m10 + a._m22 * b._m20 + a._m23 * b._m30, a._m20 * b._m01 + a._m21 * b._m11 + a._m22 * b._m21 + a._m23 * b._m31, a._m20 * b._m02 + a._m21 * b._m12 + a._m22 * b._m22 + a._m23 * b._m32, a._m20 * b._m03 + a._m21 * b._m13 + a._m22 * b._m23 + a._m23 * b._m33, a._m30 * b._m00 + a._m31 * b._m10 + a._m32 * b._m20 + a._m33 * b._m30, a._m30 * b._m01 + a._m31 * b._m11 + a._m32 * b._m21 + a._m33 * b._m31, a._m30 * b._m02 + a._m31 * b._m12 + a._m32 * b._m22 + a._m33 * b._m32, a._m30 * b._m03 + a._m31 * b._m13 + a._m32 * b._m23 + a._m33 * b._m33); }
#pragma endregion


#pragma region normalize
	static float1 normalize(const float1& v) { return any(v) ? v / length(v) : 0; }
	static float2 normalize(const float2& v) { return any(v) ? v / length(v) : 0; }
	static float3 normalize(const float3& v) { return any(v) ? v / length(v) : 0; }
	static float4 normalize(const float4& v) { return any(v) ? v / length(v) : 0; }
#pragma endregion


#pragma region pow
	static float pow(const float& a, const float& b) { return powf(a, b); }

	static float1 pow(const float1& a, const float1& b) { return float1(powf(a.x, b.x)); }
	static float2 pow(const float2& a, const float2& b) { return float2(powf(a.x, b.x), powf(a.y, b.y)); }
	static float3 pow(const float3& a, const float3& b) { return float3(powf(a.x, b.x), powf(a.y, b.y), powf(a.z, b.z)); }
	static float4 pow(const float4& a, const float4& b) { return float4(powf(a.x, b.x), powf(a.y, b.y), powf(a.z, b.z), powf(a.w, b.w)); }

	static float1x1 pow(const float1x1& a, const float1x1& b) { return float1x1(powf(a._m00, b._m00)); }
	static float1x2 pow(const float1x2& a, const float1x2& b) { return float1x2(powf(a._m00, b._m00), powf(a._m01, b._m01)); }
	static float1x3 pow(const float1x3& a, const float1x3& b) { return float1x3(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m02, b._m02)); }
	static float1x4 pow(const float1x4& a, const float1x4& b) { return float1x4(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m02, b._m02), powf(a._m03, b._m03)); }
	static float2x1 pow(const float2x1& a, const float2x1& b) { return float2x1(powf(a._m00, b._m00), powf(a._m10, b._m10)); }
	static float2x2 pow(const float2x2& a, const float2x2& b) { return float2x2(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m10, b._m10), powf(a._m11, b._m11)); }
	static float2x3 pow(const float2x3& a, const float2x3& b) { return float2x3(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m02, b._m02), powf(a._m10, b._m10), powf(a._m11, b._m11), powf(a._m12, b._m12)); }
	static float2x4 pow(const float2x4& a, const float2x4& b) { return float2x4(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m02, b._m02), powf(a._m03, b._m03), powf(a._m10, b._m10), powf(a._m11, b._m11), powf(a._m12, b._m12), powf(a._m13, b._m13)); }
	static float3x1 pow(const float3x1& a, const float3x1& b) { return float3x1(powf(a._m00, b._m00), powf(a._m10, b._m10), powf(a._m20, b._m20)); }
	static float3x2 pow(const float3x2& a, const float3x2& b) { return float3x2(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m10, b._m10), powf(a._m11, b._m11), powf(a._m20, b._m20), powf(a._m21, b._m21)); }
	static float3x3 pow(const float3x3& a, const float3x3& b) { return float3x3(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m02, b._m02), powf(a._m10, b._m10), powf(a._m11, b._m11), powf(a._m12, b._m12), powf(a._m20, b._m20), powf(a._m21, b._m21), powf(a._m22, b._m22)); }
	static float3x4 pow(const float3x4& a, const float3x4& b) { return float3x4(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m02, b._m02), powf(a._m03, b._m03), powf(a._m10, b._m10), powf(a._m11, b._m11), powf(a._m12, b._m12), powf(a._m13, b._m13), powf(a._m20, b._m20), powf(a._m21, b._m21), powf(a._m22, b._m22), powf(a._m23, b._m23)); }
	static float4x1 pow(const float4x1& a, const float4x1& b) { return float4x1(powf(a._m00, b._m00), powf(a._m10, b._m10), powf(a._m20, b._m20), powf(a._m30, b._m30)); }
	static float4x2 pow(const float4x2& a, const float4x2& b) { return float4x2(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m10, b._m10), powf(a._m11, b._m11), powf(a._m20, b._m20), powf(a._m21, b._m21), powf(a._m30, b._m30), powf(a._m31, b._m31)); }
	static float4x3 pow(const float4x3& a, const float4x3& b) { return float4x3(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m02, b._m02), powf(a._m10, b._m10), powf(a._m11, b._m11), powf(a._m12, b._m12), powf(a._m20, b._m20), powf(a._m21, b._m21), powf(a._m22, b._m22), powf(a._m30, b._m30), powf(a._m31, b._m31), powf(a._m32, b._m32)); }
	static float4x4 pow(const float4x4& a, const float4x4& b) { return float4x4(powf(a._m00, b._m00), powf(a._m01, b._m01), powf(a._m02, b._m02), powf(a._m03, b._m03), powf(a._m10, b._m10), powf(a._m11, b._m11), powf(a._m12, b._m12), powf(a._m13, b._m13), powf(a._m20, b._m20), powf(a._m21, b._m21), powf(a._m22, b._m22), powf(a._m23, b._m23), powf(a._m30, b._m30), powf(a._m31, b._m31), powf(a._m32, b._m32), powf(a._m33, b._m33)); }

#pragma endregion


#pragma region radians
	static float radians(const float& v) { return (v * PI / 180); }

	static float1 radians(const float1& v) { return float1((v.x * PI / 180)); }
	static float2 radians(const float2& v) { return float2((v.x * PI / 180), (v.y * PI / 180)); }
	static float3 radians(const float3& v) { return float3((v.x * PI / 180), (v.y * PI / 180), (v.z * PI / 180)); }
	static float4 radians(const float4& v) { return float4((v.x * PI / 180), (v.y * PI / 180), (v.z * PI / 180), (v.w * PI / 180)); }

	static float1x1 radians(const float1x1& v) { return float1x1((v._m00 * PI / 180)); }
	static float1x2 radians(const float1x2& v) { return float1x2((v._m00 * PI / 180), (v._m01 * PI / 180)); }
	static float1x3 radians(const float1x3& v) { return float1x3((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m02 * PI / 180)); }
	static float1x4 radians(const float1x4& v) { return float1x4((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m02 * PI / 180), (v._m03 * PI / 180)); }
	static float2x1 radians(const float2x1& v) { return float2x1((v._m00 * PI / 180), (v._m10 * PI / 180)); }
	static float2x2 radians(const float2x2& v) { return float2x2((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180)); }
	static float2x3 radians(const float2x3& v) { return float2x3((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m02 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180), (v._m12 * PI / 180)); }
	static float2x4 radians(const float2x4& v) { return float2x4((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m02 * PI / 180), (v._m03 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180), (v._m12 * PI / 180), (v._m13 * PI / 180)); }
	static float3x1 radians(const float3x1& v) { return float3x1((v._m00 * PI / 180), (v._m10 * PI / 180), (v._m20 * PI / 180)); }
	static float3x2 radians(const float3x2& v) { return float3x2((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180), (v._m20 * PI / 180), (v._m21 * PI / 180)); }
	static float3x3 radians(const float3x3& v) { return float3x3((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m02 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180), (v._m12 * PI / 180), (v._m20 * PI / 180), (v._m21 * PI / 180), (v._m22 * PI / 180)); }
	static float3x4 radians(const float3x4& v) { return float3x4((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m02 * PI / 180), (v._m03 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180), (v._m12 * PI / 180), (v._m13 * PI / 180), (v._m20 * PI / 180), (v._m21 * PI / 180), (v._m22 * PI / 180), (v._m23 * PI / 180)); }
	static float4x1 radians(const float4x1& v) { return float4x1((v._m00 * PI / 180), (v._m10 * PI / 180), (v._m20 * PI / 180), (v._m30 * PI / 180)); }
	static float4x2 radians(const float4x2& v) { return float4x2((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180), (v._m20 * PI / 180), (v._m21 * PI / 180), (v._m30 * PI / 180), (v._m31 * PI / 180)); }
	static float4x3 radians(const float4x3& v) { return float4x3((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m02 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180), (v._m12 * PI / 180), (v._m20 * PI / 180), (v._m21 * PI / 180), (v._m22 * PI / 180), (v._m30 * PI / 180), (v._m31 * PI / 180), (v._m32 * PI / 180)); }
	static float4x4 radians(const float4x4& v) { return float4x4((v._m00 * PI / 180), (v._m01 * PI / 180), (v._m02 * PI / 180), (v._m03 * PI / 180), (v._m10 * PI / 180), (v._m11 * PI / 180), (v._m12 * PI / 180), (v._m13 * PI / 180), (v._m20 * PI / 180), (v._m21 * PI / 180), (v._m22 * PI / 180), (v._m23 * PI / 180), (v._m30 * PI / 180), (v._m31 * PI / 180), (v._m32 * PI / 180), (v._m33 * PI / 180)); }

#pragma endregion


#pragma region round
	static float round(const float& v) { return roundf(v); }

	static float1 round(const float1& v) { return float1(roundf(v.x)); }
	static float2 round(const float2& v) { return float2(roundf(v.x), roundf(v.y)); }
	static float3 round(const float3& v) { return float3(roundf(v.x), roundf(v.y), roundf(v.z)); }
	static float4 round(const float4& v) { return float4(roundf(v.x), roundf(v.y), roundf(v.z), roundf(v.w)); }

	static float1x1 round(const float1x1& v) { return float1x1(roundf(v._m00)); }
	static float1x2 round(const float1x2& v) { return float1x2(roundf(v._m00), roundf(v._m01)); }
	static float1x3 round(const float1x3& v) { return float1x3(roundf(v._m00), roundf(v._m01), roundf(v._m02)); }
	static float1x4 round(const float1x4& v) { return float1x4(roundf(v._m00), roundf(v._m01), roundf(v._m02), roundf(v._m03)); }
	static float2x1 round(const float2x1& v) { return float2x1(roundf(v._m00), roundf(v._m10)); }
	static float2x2 round(const float2x2& v) { return float2x2(roundf(v._m00), roundf(v._m01), roundf(v._m10), roundf(v._m11)); }
	static float2x3 round(const float2x3& v) { return float2x3(roundf(v._m00), roundf(v._m01), roundf(v._m02), roundf(v._m10), roundf(v._m11), roundf(v._m12)); }
	static float2x4 round(const float2x4& v) { return float2x4(roundf(v._m00), roundf(v._m01), roundf(v._m02), roundf(v._m03), roundf(v._m10), roundf(v._m11), roundf(v._m12), roundf(v._m13)); }
	static float3x1 round(const float3x1& v) { return float3x1(roundf(v._m00), roundf(v._m10), roundf(v._m20)); }
	static float3x2 round(const float3x2& v) { return float3x2(roundf(v._m00), roundf(v._m01), roundf(v._m10), roundf(v._m11), roundf(v._m20), roundf(v._m21)); }
	static float3x3 round(const float3x3& v) { return float3x3(roundf(v._m00), roundf(v._m01), roundf(v._m02), roundf(v._m10), roundf(v._m11), roundf(v._m12), roundf(v._m20), roundf(v._m21), roundf(v._m22)); }
	static float3x4 round(const float3x4& v) { return float3x4(roundf(v._m00), roundf(v._m01), roundf(v._m02), roundf(v._m03), roundf(v._m10), roundf(v._m11), roundf(v._m12), roundf(v._m13), roundf(v._m20), roundf(v._m21), roundf(v._m22), roundf(v._m23)); }
	static float4x1 round(const float4x1& v) { return float4x1(roundf(v._m00), roundf(v._m10), roundf(v._m20), roundf(v._m30)); }
	static float4x2 round(const float4x2& v) { return float4x2(roundf(v._m00), roundf(v._m01), roundf(v._m10), roundf(v._m11), roundf(v._m20), roundf(v._m21), roundf(v._m30), roundf(v._m31)); }
	static float4x3 round(const float4x3& v) { return float4x3(roundf(v._m00), roundf(v._m01), roundf(v._m02), roundf(v._m10), roundf(v._m11), roundf(v._m12), roundf(v._m20), roundf(v._m21), roundf(v._m22), roundf(v._m30), roundf(v._m31), roundf(v._m32)); }
	static float4x4 round(const float4x4& v) { return float4x4(roundf(v._m00), roundf(v._m01), roundf(v._m02), roundf(v._m03), roundf(v._m10), roundf(v._m11), roundf(v._m12), roundf(v._m13), roundf(v._m20), roundf(v._m21), roundf(v._m22), roundf(v._m23), roundf(v._m30), roundf(v._m31), roundf(v._m32), roundf(v._m33)); }

#pragma endregion


#pragma region rsqrt
	static float rsqrt(const float& v) { return (1.0f / sqrt(v)); }

	static float1 rsqrt(const float1& v) { return float1((1.0f / sqrt(v.x))); }
	static float2 rsqrt(const float2& v) { return float2((1.0f / sqrt(v.x)), (1.0f / sqrt(v.y))); }
	static float3 rsqrt(const float3& v) { return float3((1.0f / sqrt(v.x)), (1.0f / sqrt(v.y)), (1.0f / sqrt(v.z))); }
	static float4 rsqrt(const float4& v) { return float4((1.0f / sqrt(v.x)), (1.0f / sqrt(v.y)), (1.0f / sqrt(v.z)), (1.0f / sqrt(v.w))); }

	static float1x1 rsqrt(const float1x1& v) { return float1x1((1.0f / sqrt(v._m00))); }
	static float1x2 rsqrt(const float1x2& v) { return float1x2((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01))); }
	static float1x3 rsqrt(const float1x3& v) { return float1x3((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m02))); }
	static float1x4 rsqrt(const float1x4& v) { return float1x4((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m02)), (1.0f / sqrt(v._m03))); }
	static float2x1 rsqrt(const float2x1& v) { return float2x1((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m10))); }
	static float2x2 rsqrt(const float2x2& v) { return float2x2((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11))); }
	static float2x3 rsqrt(const float2x3& v) { return float2x3((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m02)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11)), (1.0f / sqrt(v._m12))); }
	static float2x4 rsqrt(const float2x4& v) { return float2x4((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m02)), (1.0f / sqrt(v._m03)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11)), (1.0f / sqrt(v._m12)), (1.0f / sqrt(v._m13))); }
	static float3x1 rsqrt(const float3x1& v) { return float3x1((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m20))); }
	static float3x2 rsqrt(const float3x2& v) { return float3x2((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11)), (1.0f / sqrt(v._m20)), (1.0f / sqrt(v._m21))); }
	static float3x3 rsqrt(const float3x3& v) { return float3x3((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m02)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11)), (1.0f / sqrt(v._m12)), (1.0f / sqrt(v._m20)), (1.0f / sqrt(v._m21)), (1.0f / sqrt(v._m22))); }
	static float3x4 rsqrt(const float3x4& v) { return float3x4((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m02)), (1.0f / sqrt(v._m03)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11)), (1.0f / sqrt(v._m12)), (1.0f / sqrt(v._m13)), (1.0f / sqrt(v._m20)), (1.0f / sqrt(v._m21)), (1.0f / sqrt(v._m22)), (1.0f / sqrt(v._m23))); }
	static float4x1 rsqrt(const float4x1& v) { return float4x1((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m20)), (1.0f / sqrt(v._m30))); }
	static float4x2 rsqrt(const float4x2& v) { return float4x2((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11)), (1.0f / sqrt(v._m20)), (1.0f / sqrt(v._m21)), (1.0f / sqrt(v._m30)), (1.0f / sqrt(v._m31))); }
	static float4x3 rsqrt(const float4x3& v) { return float4x3((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m02)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11)), (1.0f / sqrt(v._m12)), (1.0f / sqrt(v._m20)), (1.0f / sqrt(v._m21)), (1.0f / sqrt(v._m22)), (1.0f / sqrt(v._m30)), (1.0f / sqrt(v._m31)), (1.0f / sqrt(v._m32))); }
	static float4x4 rsqrt(const float4x4& v) { return float4x4((1.0f / sqrt(v._m00)), (1.0f / sqrt(v._m01)), (1.0f / sqrt(v._m02)), (1.0f / sqrt(v._m03)), (1.0f / sqrt(v._m10)), (1.0f / sqrt(v._m11)), (1.0f / sqrt(v._m12)), (1.0f / sqrt(v._m13)), (1.0f / sqrt(v._m20)), (1.0f / sqrt(v._m21)), (1.0f / sqrt(v._m22)), (1.0f / sqrt(v._m23)), (1.0f / sqrt(v._m30)), (1.0f / sqrt(v._m31)), (1.0f / sqrt(v._m32)), (1.0f / sqrt(v._m33))); }

#pragma endregion


#pragma region saturate
	static float saturate(const float& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float1 saturate(const float1& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float2 saturate(const float2& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float3 saturate(const float3& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float4 saturate(const float4& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float1x1 saturate(const float1x1& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float1x2 saturate(const float1x2& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float1x3 saturate(const float1x3& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float1x4 saturate(const float1x4& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float2x1 saturate(const float2x1& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float2x2 saturate(const float2x2& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float2x3 saturate(const float2x3& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float2x4 saturate(const float2x4& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float3x1 saturate(const float3x1& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float3x2 saturate(const float3x2& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float3x3 saturate(const float3x3& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float3x4 saturate(const float3x4& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float4x1 saturate(const float4x1& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float4x2 saturate(const float4x2& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float4x3 saturate(const float4x3& v) { return maxf(0.0f, minf(1.0f, v)); }
	static float4x4 saturate(const float4x4& v) { return maxf(0.0f, minf(1.0f, v)); }
#pragma endregion


#pragma region sign
	static float sign(const float& v) { return (0.0f < v) - (v < 0.0f); }
	static float1 sign(const float1& v) { return (0.0f < v) - (v < 0.0f); }
	static float2 sign(const float2& v) { return (0.0f < v) - (v < 0.0f); }
	static float3 sign(const float3& v) { return (0.0f < v) - (v < 0.0f); }
	static float4 sign(const float4& v) { return (0.0f < v) - (v < 0.0f); }
	static float1x1 sign(const float1x1& v) { return (0.0f < v) - (v < 0.0f); }
	static float1x2 sign(const float1x2& v) { return (0.0f < v) - (v < 0.0f); }
	static float1x3 sign(const float1x3& v) { return (0.0f < v) - (v < 0.0f); }
	static float1x4 sign(const float1x4& v) { return (0.0f < v) - (v < 0.0f); }
	static float2x1 sign(const float2x1& v) { return (0.0f < v) - (v < 0.0f); }
	static float2x2 sign(const float2x2& v) { return (0.0f < v) - (v < 0.0f); }
	static float2x3 sign(const float2x3& v) { return (0.0f < v) - (v < 0.0f); }
	static float2x4 sign(const float2x4& v) { return (0.0f < v) - (v < 0.0f); }
	static float3x1 sign(const float3x1& v) { return (0.0f < v) - (v < 0.0f); }
	static float3x2 sign(const float3x2& v) { return (0.0f < v) - (v < 0.0f); }
	static float3x3 sign(const float3x3& v) { return (0.0f < v) - (v < 0.0f); }
	static float3x4 sign(const float3x4& v) { return (0.0f < v) - (v < 0.0f); }
	static float4x1 sign(const float4x1& v) { return (0.0f < v) - (v < 0.0f); }
	static float4x2 sign(const float4x2& v) { return (0.0f < v) - (v < 0.0f); }
	static float4x3 sign(const float4x3& v) { return (0.0f < v) - (v < 0.0f); }
	static float4x4 sign(const float4x4& v) { return (0.0f < v) - (v < 0.0f); }
#pragma endregion


#pragma region sin
	static float sin(const float& v) { return sinf(v); }

	static float1 sin(const float1& v) { return float1(sinf(v.x)); }
	static float2 sin(const float2& v) { return float2(sinf(v.x), sinf(v.y)); }
	static float3 sin(const float3& v) { return float3(sinf(v.x), sinf(v.y), sinf(v.z)); }
	static float4 sin(const float4& v) { return float4(sinf(v.x), sinf(v.y), sinf(v.z), sinf(v.w)); }

	static float1x1 sin(const float1x1& v) { return float1x1(sinf(v._m00)); }
	static float1x2 sin(const float1x2& v) { return float1x2(sinf(v._m00), sinf(v._m01)); }
	static float1x3 sin(const float1x3& v) { return float1x3(sinf(v._m00), sinf(v._m01), sinf(v._m02)); }
	static float1x4 sin(const float1x4& v) { return float1x4(sinf(v._m00), sinf(v._m01), sinf(v._m02), sinf(v._m03)); }
	static float2x1 sin(const float2x1& v) { return float2x1(sinf(v._m00), sinf(v._m10)); }
	static float2x2 sin(const float2x2& v) { return float2x2(sinf(v._m00), sinf(v._m01), sinf(v._m10), sinf(v._m11)); }
	static float2x3 sin(const float2x3& v) { return float2x3(sinf(v._m00), sinf(v._m01), sinf(v._m02), sinf(v._m10), sinf(v._m11), sinf(v._m12)); }
	static float2x4 sin(const float2x4& v) { return float2x4(sinf(v._m00), sinf(v._m01), sinf(v._m02), sinf(v._m03), sinf(v._m10), sinf(v._m11), sinf(v._m12), sinf(v._m13)); }
	static float3x1 sin(const float3x1& v) { return float3x1(sinf(v._m00), sinf(v._m10), sinf(v._m20)); }
	static float3x2 sin(const float3x2& v) { return float3x2(sinf(v._m00), sinf(v._m01), sinf(v._m10), sinf(v._m11), sinf(v._m20), sinf(v._m21)); }
	static float3x3 sin(const float3x3& v) { return float3x3(sinf(v._m00), sinf(v._m01), sinf(v._m02), sinf(v._m10), sinf(v._m11), sinf(v._m12), sinf(v._m20), sinf(v._m21), sinf(v._m22)); }
	static float3x4 sin(const float3x4& v) { return float3x4(sinf(v._m00), sinf(v._m01), sinf(v._m02), sinf(v._m03), sinf(v._m10), sinf(v._m11), sinf(v._m12), sinf(v._m13), sinf(v._m20), sinf(v._m21), sinf(v._m22), sinf(v._m23)); }
	static float4x1 sin(const float4x1& v) { return float4x1(sinf(v._m00), sinf(v._m10), sinf(v._m20), sinf(v._m30)); }
	static float4x2 sin(const float4x2& v) { return float4x2(sinf(v._m00), sinf(v._m01), sinf(v._m10), sinf(v._m11), sinf(v._m20), sinf(v._m21), sinf(v._m30), sinf(v._m31)); }
	static float4x3 sin(const float4x3& v) { return float4x3(sinf(v._m00), sinf(v._m01), sinf(v._m02), sinf(v._m10), sinf(v._m11), sinf(v._m12), sinf(v._m20), sinf(v._m21), sinf(v._m22), sinf(v._m30), sinf(v._m31), sinf(v._m32)); }
	static float4x4 sin(const float4x4& v) { return float4x4(sinf(v._m00), sinf(v._m01), sinf(v._m02), sinf(v._m03), sinf(v._m10), sinf(v._m11), sinf(v._m12), sinf(v._m13), sinf(v._m20), sinf(v._m21), sinf(v._m22), sinf(v._m23), sinf(v._m30), sinf(v._m31), sinf(v._m32), sinf(v._m33)); }

#pragma endregion


#pragma region sinh
	static float sinh(const float& v) { return sinhf(v); }

	static float1 sinh(const float1& v) { return float1(sinhf(v.x)); }
	static float2 sinh(const float2& v) { return float2(sinhf(v.x), sinhf(v.y)); }
	static float3 sinh(const float3& v) { return float3(sinhf(v.x), sinhf(v.y), sinhf(v.z)); }
	static float4 sinh(const float4& v) { return float4(sinhf(v.x), sinhf(v.y), sinhf(v.z), sinhf(v.w)); }

	static float1x1 sinh(const float1x1& v) { return float1x1(sinhf(v._m00)); }
	static float1x2 sinh(const float1x2& v) { return float1x2(sinhf(v._m00), sinhf(v._m01)); }
	static float1x3 sinh(const float1x3& v) { return float1x3(sinhf(v._m00), sinhf(v._m01), sinhf(v._m02)); }
	static float1x4 sinh(const float1x4& v) { return float1x4(sinhf(v._m00), sinhf(v._m01), sinhf(v._m02), sinhf(v._m03)); }
	static float2x1 sinh(const float2x1& v) { return float2x1(sinhf(v._m00), sinhf(v._m10)); }
	static float2x2 sinh(const float2x2& v) { return float2x2(sinhf(v._m00), sinhf(v._m01), sinhf(v._m10), sinhf(v._m11)); }
	static float2x3 sinh(const float2x3& v) { return float2x3(sinhf(v._m00), sinhf(v._m01), sinhf(v._m02), sinhf(v._m10), sinhf(v._m11), sinhf(v._m12)); }
	static float2x4 sinh(const float2x4& v) { return float2x4(sinhf(v._m00), sinhf(v._m01), sinhf(v._m02), sinhf(v._m03), sinhf(v._m10), sinhf(v._m11), sinhf(v._m12), sinhf(v._m13)); }
	static float3x1 sinh(const float3x1& v) { return float3x1(sinhf(v._m00), sinhf(v._m10), sinhf(v._m20)); }
	static float3x2 sinh(const float3x2& v) { return float3x2(sinhf(v._m00), sinhf(v._m01), sinhf(v._m10), sinhf(v._m11), sinhf(v._m20), sinhf(v._m21)); }
	static float3x3 sinh(const float3x3& v) { return float3x3(sinhf(v._m00), sinhf(v._m01), sinhf(v._m02), sinhf(v._m10), sinhf(v._m11), sinhf(v._m12), sinhf(v._m20), sinhf(v._m21), sinhf(v._m22)); }
	static float3x4 sinh(const float3x4& v) { return float3x4(sinhf(v._m00), sinhf(v._m01), sinhf(v._m02), sinhf(v._m03), sinhf(v._m10), sinhf(v._m11), sinhf(v._m12), sinhf(v._m13), sinhf(v._m20), sinhf(v._m21), sinhf(v._m22), sinhf(v._m23)); }
	static float4x1 sinh(const float4x1& v) { return float4x1(sinhf(v._m00), sinhf(v._m10), sinhf(v._m20), sinhf(v._m30)); }
	static float4x2 sinh(const float4x2& v) { return float4x2(sinhf(v._m00), sinhf(v._m01), sinhf(v._m10), sinhf(v._m11), sinhf(v._m20), sinhf(v._m21), sinhf(v._m30), sinhf(v._m31)); }
	static float4x3 sinh(const float4x3& v) { return float4x3(sinhf(v._m00), sinhf(v._m01), sinhf(v._m02), sinhf(v._m10), sinhf(v._m11), sinhf(v._m12), sinhf(v._m20), sinhf(v._m21), sinhf(v._m22), sinhf(v._m30), sinhf(v._m31), sinhf(v._m32)); }
	static float4x4 sinh(const float4x4& v) { return float4x4(sinhf(v._m00), sinhf(v._m01), sinhf(v._m02), sinhf(v._m03), sinhf(v._m10), sinhf(v._m11), sinhf(v._m12), sinhf(v._m13), sinhf(v._m20), sinhf(v._m21), sinhf(v._m22), sinhf(v._m23), sinhf(v._m30), sinhf(v._m31), sinhf(v._m32), sinhf(v._m33)); }

#pragma endregion


#pragma region smoothstep
	static float smoothstep(const float& a, const float& b, const float& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float1 smoothstep(const float1& a, const float1& b, const float1& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float2 smoothstep(const float2& a, const float2& b, const float2& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float3 smoothstep(const float3& a, const float3& b, const float3& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float4 smoothstep(const float4& a, const float4& b, const float4& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float1x1 smoothstep(const float1x1& a, const float1x1& b, const float1x1& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float1x2 smoothstep(const float1x2& a, const float1x2& b, const float1x2& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float1x3 smoothstep(const float1x3& a, const float1x3& b, const float1x3& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float1x4 smoothstep(const float1x4& a, const float1x4& b, const float1x4& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float2x1 smoothstep(const float2x1& a, const float2x1& b, const float2x1& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float2x2 smoothstep(const float2x2& a, const float2x2& b, const float2x2& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float2x3 smoothstep(const float2x3& a, const float2x3& b, const float2x3& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float2x4 smoothstep(const float2x4& a, const float2x4& b, const float2x4& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float3x1 smoothstep(const float3x1& a, const float3x1& b, const float3x1& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float3x2 smoothstep(const float3x2& a, const float3x2& b, const float3x2& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float3x3 smoothstep(const float3x3& a, const float3x3& b, const float3x3& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float3x4 smoothstep(const float3x4& a, const float3x4& b, const float3x4& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float4x1 smoothstep(const float4x1& a, const float4x1& b, const float4x1& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float4x2 smoothstep(const float4x2& a, const float4x2& b, const float4x2& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float4x3 smoothstep(const float4x3& a, const float4x3& b, const float4x3& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
	static float4x4 smoothstep(const float4x4& a, const float4x4& b, const float4x4& c) { auto t = saturate((c - a) / (b - a)); return t * t * (3.0f - t * 2.0f); }
#pragma endregion


#pragma region sqrt
	static float sqrt(const float& v) { return sqrtf(v); }

	static float1 sqrt(const float1& v) { return float1(sqrtf(v.x)); }
	static float2 sqrt(const float2& v) { return float2(sqrtf(v.x), sqrtf(v.y)); }
	static float3 sqrt(const float3& v) { return float3(sqrtf(v.x), sqrtf(v.y), sqrtf(v.z)); }
	static float4 sqrt(const float4& v) { return float4(sqrtf(v.x), sqrtf(v.y), sqrtf(v.z), sqrtf(v.w)); }

	static float1x1 sqrt(const float1x1& v) { return float1x1(sqrtf(v._m00)); }
	static float1x2 sqrt(const float1x2& v) { return float1x2(sqrtf(v._m00), sqrtf(v._m01)); }
	static float1x3 sqrt(const float1x3& v) { return float1x3(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m02)); }
	static float1x4 sqrt(const float1x4& v) { return float1x4(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m02), sqrtf(v._m03)); }
	static float2x1 sqrt(const float2x1& v) { return float2x1(sqrtf(v._m00), sqrtf(v._m10)); }
	static float2x2 sqrt(const float2x2& v) { return float2x2(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m10), sqrtf(v._m11)); }
	static float2x3 sqrt(const float2x3& v) { return float2x3(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m02), sqrtf(v._m10), sqrtf(v._m11), sqrtf(v._m12)); }
	static float2x4 sqrt(const float2x4& v) { return float2x4(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m02), sqrtf(v._m03), sqrtf(v._m10), sqrtf(v._m11), sqrtf(v._m12), sqrtf(v._m13)); }
	static float3x1 sqrt(const float3x1& v) { return float3x1(sqrtf(v._m00), sqrtf(v._m10), sqrtf(v._m20)); }
	static float3x2 sqrt(const float3x2& v) { return float3x2(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m10), sqrtf(v._m11), sqrtf(v._m20), sqrtf(v._m21)); }
	static float3x3 sqrt(const float3x3& v) { return float3x3(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m02), sqrtf(v._m10), sqrtf(v._m11), sqrtf(v._m12), sqrtf(v._m20), sqrtf(v._m21), sqrtf(v._m22)); }
	static float3x4 sqrt(const float3x4& v) { return float3x4(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m02), sqrtf(v._m03), sqrtf(v._m10), sqrtf(v._m11), sqrtf(v._m12), sqrtf(v._m13), sqrtf(v._m20), sqrtf(v._m21), sqrtf(v._m22), sqrtf(v._m23)); }
	static float4x1 sqrt(const float4x1& v) { return float4x1(sqrtf(v._m00), sqrtf(v._m10), sqrtf(v._m20), sqrtf(v._m30)); }
	static float4x2 sqrt(const float4x2& v) { return float4x2(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m10), sqrtf(v._m11), sqrtf(v._m20), sqrtf(v._m21), sqrtf(v._m30), sqrtf(v._m31)); }
	static float4x3 sqrt(const float4x3& v) { return float4x3(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m02), sqrtf(v._m10), sqrtf(v._m11), sqrtf(v._m12), sqrtf(v._m20), sqrtf(v._m21), sqrtf(v._m22), sqrtf(v._m30), sqrtf(v._m31), sqrtf(v._m32)); }
	static float4x4 sqrt(const float4x4& v) { return float4x4(sqrtf(v._m00), sqrtf(v._m01), sqrtf(v._m02), sqrtf(v._m03), sqrtf(v._m10), sqrtf(v._m11), sqrtf(v._m12), sqrtf(v._m13), sqrtf(v._m20), sqrtf(v._m21), sqrtf(v._m22), sqrtf(v._m23), sqrtf(v._m30), sqrtf(v._m31), sqrtf(v._m32), sqrtf(v._m33)); }

#pragma endregion


#pragma region step
	static float step(const float& a, const float& b) { return a >= b ? 1.0f : 0.0f; }

	static float1 step(const float1& a, const float1& b) { return float1(a.x >= b.x ? 1.0f : 0.0f); }
	static float2 step(const float2& a, const float2& b) { return float2(a.x >= b.x ? 1.0f : 0.0f, a.y >= b.y ? 1.0f : 0.0f); }
	static float3 step(const float3& a, const float3& b) { return float3(a.x >= b.x ? 1.0f : 0.0f, a.y >= b.y ? 1.0f : 0.0f, a.z >= b.z ? 1.0f : 0.0f); }
	static float4 step(const float4& a, const float4& b) { return float4(a.x >= b.x ? 1.0f : 0.0f, a.y >= b.y ? 1.0f : 0.0f, a.z >= b.z ? 1.0f : 0.0f, a.w >= b.w ? 1.0f : 0.0f); }

	static float1x1 step(const float1x1& a, const float1x1& b) { return float1x1(a._m00 >= b._m00 ? 1.0f : 0.0f); }
	static float1x2 step(const float1x2& a, const float1x2& b) { return float1x2(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f); }
	static float1x3 step(const float1x3& a, const float1x3& b) { return float1x3(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m02 >= b._m02 ? 1.0f : 0.0f); }
	static float1x4 step(const float1x4& a, const float1x4& b) { return float1x4(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m02 >= b._m02 ? 1.0f : 0.0f, a._m03 >= b._m03 ? 1.0f : 0.0f); }
	static float2x1 step(const float2x1& a, const float2x1& b) { return float2x1(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f); }
	static float2x2 step(const float2x2& a, const float2x2& b) { return float2x2(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f); }
	static float2x3 step(const float2x3& a, const float2x3& b) { return float2x3(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m02 >= b._m02 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f, a._m12 >= b._m12 ? 1.0f : 0.0f); }
	static float2x4 step(const float2x4& a, const float2x4& b) { return float2x4(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m02 >= b._m02 ? 1.0f : 0.0f, a._m03 >= b._m03 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f, a._m12 >= b._m12 ? 1.0f : 0.0f, a._m13 >= b._m13 ? 1.0f : 0.0f); }
	static float3x1 step(const float3x1& a, const float3x1& b) { return float3x1(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m20 >= b._m20 ? 1.0f : 0.0f); }
	static float3x2 step(const float3x2& a, const float3x2& b) { return float3x2(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f, a._m20 >= b._m20 ? 1.0f : 0.0f, a._m21 >= b._m21 ? 1.0f : 0.0f); }
	static float3x3 step(const float3x3& a, const float3x3& b) { return float3x3(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m02 >= b._m02 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f, a._m12 >= b._m12 ? 1.0f : 0.0f, a._m20 >= b._m20 ? 1.0f : 0.0f, a._m21 >= b._m21 ? 1.0f : 0.0f, a._m22 >= b._m22 ? 1.0f : 0.0f); }
	static float3x4 step(const float3x4& a, const float3x4& b) { return float3x4(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m02 >= b._m02 ? 1.0f : 0.0f, a._m03 >= b._m03 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f, a._m12 >= b._m12 ? 1.0f : 0.0f, a._m13 >= b._m13 ? 1.0f : 0.0f, a._m20 >= b._m20 ? 1.0f : 0.0f, a._m21 >= b._m21 ? 1.0f : 0.0f, a._m22 >= b._m22 ? 1.0f : 0.0f, a._m23 >= b._m23 ? 1.0f : 0.0f); }
	static float4x1 step(const float4x1& a, const float4x1& b) { return float4x1(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m20 >= b._m20 ? 1.0f : 0.0f, a._m30 >= b._m30 ? 1.0f : 0.0f); }
	static float4x2 step(const float4x2& a, const float4x2& b) { return float4x2(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f, a._m20 >= b._m20 ? 1.0f : 0.0f, a._m21 >= b._m21 ? 1.0f : 0.0f, a._m30 >= b._m30 ? 1.0f : 0.0f, a._m31 >= b._m31 ? 1.0f : 0.0f); }
	static float4x3 step(const float4x3& a, const float4x3& b) { return float4x3(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m02 >= b._m02 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f, a._m12 >= b._m12 ? 1.0f : 0.0f, a._m20 >= b._m20 ? 1.0f : 0.0f, a._m21 >= b._m21 ? 1.0f : 0.0f, a._m22 >= b._m22 ? 1.0f : 0.0f, a._m30 >= b._m30 ? 1.0f : 0.0f, a._m31 >= b._m31 ? 1.0f : 0.0f, a._m32 >= b._m32 ? 1.0f : 0.0f); }
	static float4x4 step(const float4x4& a, const float4x4& b) { return float4x4(a._m00 >= b._m00 ? 1.0f : 0.0f, a._m01 >= b._m01 ? 1.0f : 0.0f, a._m02 >= b._m02 ? 1.0f : 0.0f, a._m03 >= b._m03 ? 1.0f : 0.0f, a._m10 >= b._m10 ? 1.0f : 0.0f, a._m11 >= b._m11 ? 1.0f : 0.0f, a._m12 >= b._m12 ? 1.0f : 0.0f, a._m13 >= b._m13 ? 1.0f : 0.0f, a._m20 >= b._m20 ? 1.0f : 0.0f, a._m21 >= b._m21 ? 1.0f : 0.0f, a._m22 >= b._m22 ? 1.0f : 0.0f, a._m23 >= b._m23 ? 1.0f : 0.0f, a._m30 >= b._m30 ? 1.0f : 0.0f, a._m31 >= b._m31 ? 1.0f : 0.0f, a._m32 >= b._m32 ? 1.0f : 0.0f, a._m33 >= b._m33 ? 1.0f : 0.0f); }

#pragma endregion


#pragma region tan
	static float tan(const float& v) { return tanf(v); }

	static float1 tan(const float1& v) { return float1(tanf(v.x)); }
	static float2 tan(const float2& v) { return float2(tanf(v.x), tanf(v.y)); }
	static float3 tan(const float3& v) { return float3(tanf(v.x), tanf(v.y), tanf(v.z)); }
	static float4 tan(const float4& v) { return float4(tanf(v.x), tanf(v.y), tanf(v.z), tanf(v.w)); }

	static float1x1 tan(const float1x1& v) { return float1x1(tanf(v._m00)); }
	static float1x2 tan(const float1x2& v) { return float1x2(tanf(v._m00), tanf(v._m01)); }
	static float1x3 tan(const float1x3& v) { return float1x3(tanf(v._m00), tanf(v._m01), tanf(v._m02)); }
	static float1x4 tan(const float1x4& v) { return float1x4(tanf(v._m00), tanf(v._m01), tanf(v._m02), tanf(v._m03)); }
	static float2x1 tan(const float2x1& v) { return float2x1(tanf(v._m00), tanf(v._m10)); }
	static float2x2 tan(const float2x2& v) { return float2x2(tanf(v._m00), tanf(v._m01), tanf(v._m10), tanf(v._m11)); }
	static float2x3 tan(const float2x3& v) { return float2x3(tanf(v._m00), tanf(v._m01), tanf(v._m02), tanf(v._m10), tanf(v._m11), tanf(v._m12)); }
	static float2x4 tan(const float2x4& v) { return float2x4(tanf(v._m00), tanf(v._m01), tanf(v._m02), tanf(v._m03), tanf(v._m10), tanf(v._m11), tanf(v._m12), tanf(v._m13)); }
	static float3x1 tan(const float3x1& v) { return float3x1(tanf(v._m00), tanf(v._m10), tanf(v._m20)); }
	static float3x2 tan(const float3x2& v) { return float3x2(tanf(v._m00), tanf(v._m01), tanf(v._m10), tanf(v._m11), tanf(v._m20), tanf(v._m21)); }
	static float3x3 tan(const float3x3& v) { return float3x3(tanf(v._m00), tanf(v._m01), tanf(v._m02), tanf(v._m10), tanf(v._m11), tanf(v._m12), tanf(v._m20), tanf(v._m21), tanf(v._m22)); }
	static float3x4 tan(const float3x4& v) { return float3x4(tanf(v._m00), tanf(v._m01), tanf(v._m02), tanf(v._m03), tanf(v._m10), tanf(v._m11), tanf(v._m12), tanf(v._m13), tanf(v._m20), tanf(v._m21), tanf(v._m22), tanf(v._m23)); }
	static float4x1 tan(const float4x1& v) { return float4x1(tanf(v._m00), tanf(v._m10), tanf(v._m20), tanf(v._m30)); }
	static float4x2 tan(const float4x2& v) { return float4x2(tanf(v._m00), tanf(v._m01), tanf(v._m10), tanf(v._m11), tanf(v._m20), tanf(v._m21), tanf(v._m30), tanf(v._m31)); }
	static float4x3 tan(const float4x3& v) { return float4x3(tanf(v._m00), tanf(v._m01), tanf(v._m02), tanf(v._m10), tanf(v._m11), tanf(v._m12), tanf(v._m20), tanf(v._m21), tanf(v._m22), tanf(v._m30), tanf(v._m31), tanf(v._m32)); }
	static float4x4 tan(const float4x4& v) { return float4x4(tanf(v._m00), tanf(v._m01), tanf(v._m02), tanf(v._m03), tanf(v._m10), tanf(v._m11), tanf(v._m12), tanf(v._m13), tanf(v._m20), tanf(v._m21), tanf(v._m22), tanf(v._m23), tanf(v._m30), tanf(v._m31), tanf(v._m32), tanf(v._m33)); }

#pragma endregion


#pragma region tanh
	static float tanh(const float& v) { return tanhf(v); }

	static float1 tanh(const float1& v) { return float1(tanhf(v.x)); }
	static float2 tanh(const float2& v) { return float2(tanhf(v.x), tanhf(v.y)); }
	static float3 tanh(const float3& v) { return float3(tanhf(v.x), tanhf(v.y), tanhf(v.z)); }
	static float4 tanh(const float4& v) { return float4(tanhf(v.x), tanhf(v.y), tanhf(v.z), tanhf(v.w)); }

	static float1x1 tanh(const float1x1& v) { return float1x1(tanhf(v._m00)); }
	static float1x2 tanh(const float1x2& v) { return float1x2(tanhf(v._m00), tanhf(v._m01)); }
	static float1x3 tanh(const float1x3& v) { return float1x3(tanhf(v._m00), tanhf(v._m01), tanhf(v._m02)); }
	static float1x4 tanh(const float1x4& v) { return float1x4(tanhf(v._m00), tanhf(v._m01), tanhf(v._m02), tanhf(v._m03)); }
	static float2x1 tanh(const float2x1& v) { return float2x1(tanhf(v._m00), tanhf(v._m10)); }
	static float2x2 tanh(const float2x2& v) { return float2x2(tanhf(v._m00), tanhf(v._m01), tanhf(v._m10), tanhf(v._m11)); }
	static float2x3 tanh(const float2x3& v) { return float2x3(tanhf(v._m00), tanhf(v._m01), tanhf(v._m02), tanhf(v._m10), tanhf(v._m11), tanhf(v._m12)); }
	static float2x4 tanh(const float2x4& v) { return float2x4(tanhf(v._m00), tanhf(v._m01), tanhf(v._m02), tanhf(v._m03), tanhf(v._m10), tanhf(v._m11), tanhf(v._m12), tanhf(v._m13)); }
	static float3x1 tanh(const float3x1& v) { return float3x1(tanhf(v._m00), tanhf(v._m10), tanhf(v._m20)); }
	static float3x2 tanh(const float3x2& v) { return float3x2(tanhf(v._m00), tanhf(v._m01), tanhf(v._m10), tanhf(v._m11), tanhf(v._m20), tanhf(v._m21)); }
	static float3x3 tanh(const float3x3& v) { return float3x3(tanhf(v._m00), tanhf(v._m01), tanhf(v._m02), tanhf(v._m10), tanhf(v._m11), tanhf(v._m12), tanhf(v._m20), tanhf(v._m21), tanhf(v._m22)); }
	static float3x4 tanh(const float3x4& v) { return float3x4(tanhf(v._m00), tanhf(v._m01), tanhf(v._m02), tanhf(v._m03), tanhf(v._m10), tanhf(v._m11), tanhf(v._m12), tanhf(v._m13), tanhf(v._m20), tanhf(v._m21), tanhf(v._m22), tanhf(v._m23)); }
	static float4x1 tanh(const float4x1& v) { return float4x1(tanhf(v._m00), tanhf(v._m10), tanhf(v._m20), tanhf(v._m30)); }
	static float4x2 tanh(const float4x2& v) { return float4x2(tanhf(v._m00), tanhf(v._m01), tanhf(v._m10), tanhf(v._m11), tanhf(v._m20), tanhf(v._m21), tanhf(v._m30), tanhf(v._m31)); }
	static float4x3 tanh(const float4x3& v) { return float4x3(tanhf(v._m00), tanhf(v._m01), tanhf(v._m02), tanhf(v._m10), tanhf(v._m11), tanhf(v._m12), tanhf(v._m20), tanhf(v._m21), tanhf(v._m22), tanhf(v._m30), tanhf(v._m31), tanhf(v._m32)); }
	static float4x4 tanh(const float4x4& v) { return float4x4(tanhf(v._m00), tanhf(v._m01), tanhf(v._m02), tanhf(v._m03), tanhf(v._m10), tanhf(v._m11), tanhf(v._m12), tanhf(v._m13), tanhf(v._m20), tanhf(v._m21), tanhf(v._m22), tanhf(v._m23), tanhf(v._m30), tanhf(v._m31), tanhf(v._m32), tanhf(v._m33)); }

#pragma endregion


#pragma region transpose
	static float1x1 transpose(const float1x1& a) { return float1x1(a._m00); }
	static float1x2 transpose(const float2x1& a) { return float1x2(a._m00, a._m10); }
	static float1x3 transpose(const float3x1& a) { return float1x3(a._m00, a._m10, a._m20); }
	static float1x4 transpose(const float4x1& a) { return float1x4(a._m00, a._m10, a._m20, a._m30); }
	static float2x1 transpose(const float1x2& a) { return float2x1(a._m00, a._m01); }
	static float2x2 transpose(const float2x2& a) { return float2x2(a._m00, a._m10, a._m01, a._m11); }
	static float2x3 transpose(const float3x2& a) { return float2x3(a._m00, a._m10, a._m20, a._m01, a._m11, a._m21); }
	static float2x4 transpose(const float4x2& a) { return float2x4(a._m00, a._m10, a._m20, a._m30, a._m01, a._m11, a._m21, a._m31); }
	static float3x1 transpose(const float1x3& a) { return float3x1(a._m00, a._m01, a._m02); }
	static float3x2 transpose(const float2x3& a) { return float3x2(a._m00, a._m10, a._m01, a._m11, a._m02, a._m12); }
	static float3x3 transpose(const float3x3& a) { return float3x3(a._m00, a._m10, a._m20, a._m01, a._m11, a._m21, a._m02, a._m12, a._m22); }
	static float3x4 transpose(const float4x3& a) { return float3x4(a._m00, a._m10, a._m20, a._m30, a._m01, a._m11, a._m21, a._m31, a._m02, a._m12, a._m22, a._m32); }
	static float4x1 transpose(const float1x4& a) { return float4x1(a._m00, a._m01, a._m02, a._m03); }
	static float4x2 transpose(const float2x4& a) { return float4x2(a._m00, a._m10, a._m01, a._m11, a._m02, a._m12, a._m03, a._m13); }
	static float4x3 transpose(const float3x4& a) { return float4x3(a._m00, a._m10, a._m20, a._m01, a._m11, a._m21, a._m02, a._m12, a._m22, a._m03, a._m13, a._m23); }
	static float4x4 transpose(const float4x4& a) { return float4x4(a._m00, a._m10, a._m20, a._m30, a._m01, a._m11, a._m21, a._m31, a._m02, a._m12, a._m22, a._m32, a._m03, a._m13, a._m23, a._m33); }
#pragma endregion

#pragma endregion

#pragma region Memory Management

	template<typename S>
	class gObj {
		friend S;
		template<typename T> friend class gObj;

	private:
		S* _this;
		volatile long* counter;

		void AddReference();

		void RemoveReference();

	public:
		gObj() : _this(nullptr), counter(nullptr) {
		}
		gObj(S* self) : _this(self), counter(self ? new long(1) : nullptr) {
		}

		inline bool isNull() const { return _this == nullptr; }

		// Copy constructor
		gObj(const gObj<S>& other) {
			this->counter = other.counter;
			this->_this = other._this;
			if (!isNull())
				AddReference();
		}

		template <typename Subtype>
		gObj(const gObj<Subtype>& other) {
			this->counter = other.counter;
			this->_this = (S*)other._this;
			if (!isNull())
				AddReference();
		}

		inline gObj<S>& operator = (const gObj<S>& other) {
			if (!isNull())
				RemoveReference();
			this->counter = other.counter;
			this->_this = other._this;
			if (!isNull())
				AddReference();
			return *this;
		}

		bool operator == (const gObj<S>& other) {
			return other._this == _this;
		}

		bool operator != (const gObj<S>& other) {
			return other._this != _this;
		}

		template<typename A>
		auto& operator[](A arg) {
			return (*_this)[arg];
		}

		~gObj() {
			if (!isNull())
				RemoveReference();
		}

		//Dereference operator
		inline S& operator*()
		{
			return *_this;
		}

		//Member Access operator
		inline S* operator->()
		{
			return _this;
		}

		template<typename T>
		inline gObj<T> Dynamic_Cast() {
			gObj<T> obj;
			obj._this = dynamic_cast<T*>(_this);
			obj.counter = counter;
			obj.AddReference();
			return obj;
		}

		template<typename T>
		inline gObj<T> Static_Cast() {
			gObj<T> obj;
			obj._this = static_cast<T*>(_this);
			obj.counter = counter;
			obj.AddReference();
			return obj;
		}

		inline operator bool() const {
			return !isNull();
		}
	};

	template<typename S>
	void gObj<S>::AddReference() {
		if (!counter)
			throw Exception::FromError(Errors::Invalid_Operation, "Error referencing");

		InterlockedAdd(counter, 1);
	}

	template<typename S>
	void gObj<S>::RemoveReference() {
		if (!counter)
			throw Exception::FromError(Errors::Invalid_Operation, "Error referencing");

		InterlockedAdd(counter, -1);
		if ((*counter) == 0) {
			delete _this;
			delete counter;
			//_this = nullptr;
			counter = nullptr;
		}
	}

	class string {
		char* text;
		int length;

		int* references;

		void __AddReference() {
			if (!IsNull())
				(*references)++;
		}

		void __RemoveReference() {
			if (IsNull())
				return;

			(*references)--;

			if (*references == 0) {
				delete[] text;
				delete references;
			}
		}

	public:
		bool IsNull() const {
			return text == nullptr;
		}

		~string() {
			__RemoveReference();
		}

		string() {
			text = nullptr;
			length = -1;
			references = nullptr;
		}

		string(const string& other) {
			this->text = other.text;
			this->length = other.length;
			references = other.references;
			__AddReference();
		}

		string(const char* text) {
			if (text == nullptr) {
				text = nullptr;
				length = -1;
				references = nullptr;
			}
			else
			{
				this->length = (int)strlen(text);
				this->text = new char[this->length + 1];
				ZeroMemory(this->text, this->length + 1);

				strcpy_s(this->text, this->length + 1, text);
				this->references = new int(1);
			}
		}

		string(const char* text, int count) {
			this->length = count;
			this->text = new char[this->length + 1];
			ZeroMemory(this->text, this->length + 1);

			for (int i = 0; i < count; i++)
				this->text[i] = text[i];
			this->references = new int(1);
		}

		bool operator == (const string& other) {
			if (this->IsNull() && other.IsNull())
				return true;

			if (this->IsNull() || other.IsNull())
				return false;

			if (other.text == this->text)
				return true; // same instance

			if (other.length != this->length)
				return false;

			for (int i = 0; i < this->length; i++)
				if (this->text[i] != other.text[i])
					return false;
			return true;
		}

		bool operator != (const string& other) {
			if (this->IsNull() && other.IsNull())
				return false;

			if (this->IsNull() || other.IsNull())
				return true;

			if (other.text == this->text)
				return false; // same instance

			if (other.length != this->length)
				return true;

			for (int i = 0; i < this->length; i++)
				if (this->text[i] != other.text[i])
					return true;
			return false;
		}

		string operator + (const string& other) {
			if (this->IsNull())
				return other;
			if (other.IsNull())
				return *this;

			string result;
			result.text = new char[this->length + other.length + 1];
			ZeroMemory(result.text, this->length + other.length + 1);
			strcpy_s(result.text, this->length + 1, this->text);
			strcpy_s(&result.text[this->length], other.length + 1, other.text);
			result.length = this->length + other.length;
			result.references = new int(1);
			return result;
		}

		string substr(int start, int count) {
			if (this->IsNull())
				throw Exception::FromError(Errors::Invalid_Operation, "Null reference to string");

			if (start + count > this->length)
				throw Exception::FromError(Errors::Any, "Argument error");

			return string(this->text + start, count);
		}

		string substr(int start) {
			if (this->IsNull())
				throw Exception::FromError(Errors::Invalid_Operation, "Null reference to string");

			if (start >= this->length)
				throw Exception::FromError(Errors::Any, "Argument error");

			return string(this->text + start, this->length - start);
		}

		int find_last_of(const string& sub) {
			if (sub.IsNull())
				return -1;
			if (this->length < sub.length)
				return -1;
			for (int i = this->length - sub.length; i >= 0; i--)
			{
				bool found = true;
				for (int j = 0; found && j < sub.length; j++)
					if (this->text[i + j] != sub.text[j])
						found = false;
				if (found)
					return i;
			}
			return -1;
		}

		string& operator = (const string& other) {
			__RemoveReference();
			this->references = other.references;
			this->length = other.length;
			this->text = other.text;
			__AddReference();
			return *this;
		}

		inline int len() { return this->length; }

		inline const char* c_str() { return this->text; }
	};

	template<typename T>
	class list
	{
		T* elements;
		int count;
		int capacity;
	public:
		list() {
			capacity = 32;
			count = 0;
			elements = new T[capacity];
			ZeroMemory(elements, sizeof(T) * capacity);
		}
		list(int capacity) {
			this->capacity = capacity;
			count = 0;
			elements = new T[capacity];
			ZeroMemory(elements, sizeof(T) * capacity);
		}
		list(const list<T>& other) {
			this->count = other.count;
			this->elements = new T[other.capacity];
			this->capacity = other.capacity;
			ZeroMemory(elements, sizeof(T) * capacity);
			for (int i = 0; i < this->capacity; i++)
				this->elements[i] = other.elements[i];
		}
	public:

		gObj<list<T>> clone() {
			gObj<list<T>> result = new list<T>();
			for (int i = 0; i < count; i++)
				result->add(elements[i]);
			return result;
		}

		void reset() {
			count = 0;
		}

		list(std::initializer_list<T> initialElements) {
			capacity = max(32, initialElements.size());
			count = initialElements.size();
			elements = new T[capacity];
			ZeroMemory(elements, sizeof(T) * capacity);

			for (int i = 0; i < initialElements.size(); i++)
				elements[i] = initialElements[i];
		}

		~list() {
			delete[] elements;
		}

		/// pushes an item at the begining
		void push(T item) {
			add(item);
			for (int i = count - 1; i > 0; i--)
				elements[i] = elements[i - 1];
			elements[0] = item;
		}

		int add(T item) {
			if (count == capacity)
			{
				capacity = (int)(capacity * 1.3);
				T* newelements = new T[capacity];
				ZeroMemory(newelements, sizeof(T) * capacity);

				for (int i = 0; i < count; i++)
					newelements[i] = elements[i];
				delete[] elements;
				elements = newelements;
			}
			elements[count] = item;
			return count++;
		}

		inline T& operator[](int index) const {
			return elements[index];
		}

		inline T& first() const {
			return elements[0];
		}

		inline T& last() const {
			return elements[count - 1];
		}

		inline int size() const {
			return count;
		}
	};

	template<typename T>
	class table
	{
		T* elements;
		int count;
		int capacity;
		int* references;
	public:
		table() {
			elements = nullptr;
			references = nullptr;
			count = capacity = 0;
		}
		table(int capacity) {
			this->capacity = capacity;
			count = 0;
			elements = new T[capacity];
			ZeroMemory(elements, sizeof(T) * capacity);
			references = new int(1);
		}
		table(const table<T>& other) {
			this->count = other.count;
			this->elements = other.elements;
			this->capacity = other.capacity;
			this->references = other.references;
			__AddReference();
		}
		void __RemoveReference() {
			if (references) // not null
			{
				if (--(*references) == 0)
					delete[] elements;
			}
		}
		void __AddReference() {
			if (this->references)
				++(*this->references);
		}
	public:

		void reset() {
			count = 0;
			ZeroMemory(elements, sizeof(T) * capacity);
		}

		table<T>& operator = (const table<T>& other) {
			__RemoveReference();
			this->references = other.references;
			this->length = other.length;
			this->text = other.text;
			__AddReference();
			return *this;
		}

		~table() {
			__RemoveReference();
		}

		/// pushes an item at the begining
		void push(T item) {
			add(item);
			for (int i = count - 1; i > 0; i--)
				elements[i] = elements[i - 1];
			elements[0] = item;
		}

		void add(T item) {
			if (count == capacity)
				throw Exception::FromError(Errors::Invalid_Operation, "Table overflow.");
			elements[count++] = item;
		}

		inline T& operator[](int index) const {
			return elements[index];
		}

		inline T& first() const {
			return elements[0];
		}

		inline T& last() const {
			return elements[count - 1];
		}

		inline int size() const {
			return count;
		}
	};

	class Tagging {
		void* __Tag;
		int* references;
	public:
		template<typename T>
		Tagging(T data) {
			__Tag = new T(data);
			references = new int(1);
		}

		Tagging(const Tagging& other) {
			this->__Tag = other.__Tag;
			this->references = other.references;
			if (other.__Tag != nullptr)
				(*this->references)++;
		}

		Tagging() {
			__Tag = nullptr;
			references = nullptr;
		}

		~Tagging() {
			if (__Tag != nullptr) {
				(*references)--;
				if (*references == 0)
					delete __Tag;
			}
		}

		Tagging& operator = (const Tagging& other) {
			if (__Tag != nullptr)
			{
				(*references)--;
				if (*references == 0)
					delete __Tag;
			}
			this->references = other.references;
			this->__Tag = other.__Tag;
			if (__Tag != nullptr)
			{
				(*references)++;
			}
			return *this;
		}

		template<typename T>
		T getData() {
			return *((T*)__Tag);
		}
	};

#pragma endregion

#pragma region Scheduling objects

	/// <summary>
	/// Object used to represent a complete flush signal.
	/// Used to sync CPU execution with a specific GPU process termination
	/// </summary>
	struct Signal {
		friend wScheduler;
	private:
		Signal(wScheduler* scheduler, long rallyPoints[3]) {
			this->scheduler = scheduler;
			this->rallyPoints[0] = rallyPoints[0];
			this->rallyPoints[1] = rallyPoints[1];
			this->rallyPoints[2] = rallyPoints[2];
		}
		long rallyPoints[3] = { };
		wScheduler* scheduler = nullptr;
	public:
		Signal() {}
		void WaitFor();
	};

	enum class EngineMask : int {
		Direct = 1,
		Compute = 2,
		Copy = 4,
		All = 7
	};

	// Represents the different engines can be used to work with the GPU.
	enum class Engine : int {
		// Engine used to generate graphics using rasterization with vertex, hull, domain, geometry and pixel shaders.
		// Engine also used for ray-tracing programs using libraries. (This engine uses the same queue that graphics).
		Direct = 0,
		// Engine used for parallel computation using compute shaders
		Compute = 1,
		// Engine used for copying on the GPU.
		Copy = 2
	};

	/// Base type for any graphic process that populates a command list.
	class GPUProcess {
		friend wScheduler;
	protected:
		// When implemented, populate the specific command list with the instructions to draw.
		virtual void OnCollect(gObj<CommandListManager> manager) = 0;
		// Gets the type of engine required by this process
		virtual Engine RequiredEngine() = 0;
	};

#pragma endregion

#pragma region Command List Management

	/// Base type for all types of engines
	class CommandListManager {
		friend wScheduler;
		Tagging __Tag; // Allows the scheduler to 'plant' a value for async processes to access to it.
		wCmdList* w_cmdList;
	public:
		virtual ~CommandListManager() { }
		// Tag data when the process was enqueue
		template<typename T>
		inline T Tag() { return __Tag.getData<T>(); }
	};

#pragma endregion

#pragma region Device Manager

	typedef struct PresenterDescription {
		HWND hWnd = 0; // Window handle the device is associated with.
		bool UseWarpDevice = false; // Uses a warp device to simulate all DX12 functionalities
		int Frames = 2; // Number of frames on-the-fly. This value is used to allocate resources, defines the number of Backbuffers in swap chain
		bool UseBuffering = false; // Defines if the scheduler should continue with next frame without awaiting present of previous frame.
		int ResolutionWidth = 0;  // Width of the Backbuffers
		int ResolutionHeight = 0; // Height of the Backbuffers
		int Threads = 8; // Number of parallel population process in the scheduler...
		/// ... TO BE CONTINUED
	} PresenterDescription;

	class DeviceManager {
		friend Presenter;

		wDevice* device = nullptr;
	protected:

		DeviceManager() {}

	public:
		
		template<typename T, typename ...A>
		inline void Load(gObj<T>& technique, A... args) {
			technique = new T(args...);
			technique->device = device;
			technique->OnLoad();
		}

		template<typename T>
		inline void Dispatch(gObj<T> technique) {
			technique->OnDispatch();
		}
	};

	class Technique : protected DeviceManager {
		friend DeviceManager;
	protected:
		virtual void OnLoad() = 0;

		virtual void OnDispatch() = 0;
	};

	class Presenter : public DeviceManager
	{
		Presenter() {}
	public:
		static gObj<Presenter> Create(const PresenterDescription& desc);

		void BeginFrame();

		void EndFrame();

		~Presenter();
	};


#pragma endregion

}


#endif
