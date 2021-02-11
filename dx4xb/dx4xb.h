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
	struct wProgram; // Wrapper to a DX12 Raytracing program settings
	struct wStateObject; // Wrapper to a DX12 raytracing state object
	struct wPipelineState; // Wrapper to a DX12 traditional Pipeline state object
	struct wBindings; // Wrapper to a DX12 pipeline binding object.
	struct wCmdList; // Wrapper to DX12 command lists
	struct wScheduler; // Wrapper to DX12 command queues manager, allocators, framing
	struct wDevice; // Wrapper to DX12 device, swapchains, descriptor heaps and scheduler
	
	// Public definitions

	struct PresenterDescription;
	class GPUProcess; // Defines types can populate command lists for further GPU execution
	class DeviceManager; // Defines base type for entities that can create resources and dispatch GPU processes
	class Technique; // Defines types can create resources and dispatch GPU processes
	class Presenter; // Defines a Device manager that can control swap chains and presenting buffers
	class CommandListManager;
	class CopyManager;
	class ComputeManager;
	class GraphicsManager;
	class RaytracingManager;
	class Pipeline;
	class StaticPipelineBase;
	template<typename ...PSS> class StaticPipeline;
	class RaytracingPipeline;
	class RaytracingProgramBase;
	class ComputeBinder;
	class GraphicsBinder;
	class RaytracingBinder;
	class GeometryCollection;
	class TriangleGeometryCollection;
	class ProceduralGeometryCollection;
	class InstanceCollection;

#pragma endregion

#pragma region DSL

	template<typename T>
	T ___dr(T* a) { return &a; }

#define Execute_OnGPU(methodName) Dispatch_Method(this, &decltype(___dr(this))::methodName)
#define Execute_OnGPU_Async(methodName) Dispatch_Method_Async(this, &decltype(___dr(this))::methodName)

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

#pragma region GMath

#pragma region cross
	static float3 cross(const float3& pto1, const float3& pto2)
	{
		return float3(
			pto1.y * pto2.z - pto1.z * pto2.y,
			pto1.z * pto2.x - pto1.x * pto2.z,
			pto1.x * pto2.y - pto1.y * pto2.x);
	}
#pragma endregion

#pragma region determinant

	static float determinant(const float1x1& m)
	{
		return m._m00;
	}

	static float determinant(const float2x2& m)
	{
		return m._m00 * m._m11 - m._m01 * m._m10;
	}

	static float determinant(const float3x3& m)
	{
		/// 00 01 02
		/// 10 11 12
		/// 20 21 22
		float Min00 = m._m11 * m._m22 - m._m12 * m._m21;
		float Min01 = m._m10 * m._m22 - m._m12 * m._m20;
		float Min02 = m._m10 * m._m21 - m._m11 * m._m20;

		return Min00 * m._m00 - Min01 * m._m01 + Min02 * m._m02;
	}

	static float determinant(const float4x4& m)
	{
		float Min00 = m._m11 * m._m22 * m._m33 + m._m12 * m._m23 * m._m31 + m._m13 * m._m21 * m._m32 - m._m11 * m._m23 * m._m32 - m._m12 * m._m21 * m._m33 - m._m13 * m._m22 * m._m31;
		float Min01 = m._m10 * m._m22 * m._m33 + m._m12 * m._m23 * m._m30 + m._m13 * m._m20 * m._m32 - m._m10 * m._m23 * m._m32 - m._m12 * m._m20 * m._m33 - m._m13 * m._m22 * m._m30;
		float Min02 = m._m10 * m._m21 * m._m33 + m._m11 * m._m23 * m._m30 + m._m13 * m._m20 * m._m31 - m._m10 * m._m23 * m._m31 - m._m11 * m._m20 * m._m33 - m._m13 * m._m21 * m._m30;
		float Min03 = m._m10 * m._m21 * m._m32 + m._m11 * m._m22 * m._m30 + m._m12 * m._m20 * m._m31 - m._m10 * m._m22 * m._m31 - m._m11 * m._m20 * m._m32 - m._m12 * m._m21 * m._m30;

		return Min00 * m._m00 - Min01 * m._m01 + Min02 * m._m02 - Min03 * m._m03;
	}

#pragma endregion

#pragma region faceNormal
	static float3 faceNormal(float3 normal, float3 direction)
	{
		return dot(normal, direction) > 0 ? normal : -normal;
	}
#pragma endregion

#pragma region lit
	static float4 lit(float NdotL, float NdotH, float power)
	{
		return float4(1, NdotL < 0 ? 0 : NdotL, NdotL < 0 || NdotH < 0 ? 0 : pow(NdotH, power), 1);
	}
#pragma endregion

#pragma region reflect

	/// <summary>
	/// Performs the reflect function to the specified float3 objects.
	/// Gets the reflection vector. L is direction to Light, N is the surface normal
	/// </summary>
	static float3 reflect(float3 L, float3 N)
	{
		return 2 * dot(L, N) * N - L;
	}

#pragma endregion

#pragma region refract

	/// <summary>
	/// Performs the refract function to the specified float3 objects.
	/// Gets the refraction vector.
	/// L is direction to Light, N is the normal, eta is the refraction index factor.
	/// </summary>
	static float3 refract(float3 L, float3 N, float eta)
	{
		float3 I = -1 * L;

		float cosAngle = dot(I, N);
		float delta = 1.0f - eta * eta * (1.0f - cosAngle * cosAngle);

		if (delta < 0)
			return float3(0, 0, 0);

		return normalize(eta * I - N * (eta * cosAngle + sqrt(delta)));
	}

#pragma endregion

#pragma region ortho basis

	static float copysign(float f, float t)
	{
		return copysignf(f, t);
	}

	/// <summary>
	/// Given a direction, creates two othonormal vectors to it.
	/// From the paper: Building an Orthonormal Basis, Revisited, Tom Duff, et.al.
	/// </summary>
	static void createOrthoBasis(const float3& N, float3& T, float3& B)
	{
		float sign = copysign(1.0f, N.z);
		float a = -1.0f / (sign + N.z);
		float b = N.x * N.y * a;
		T = float3(1.0f + sign * N.x * N.x * a, sign * b, -sign * N.x);
		B = float3(b, sign + N.y * N.y * a, -N.y);
	}

#pragma endregion

#pragma region inverse

	static float1x1 inverse(const float1x1& m) {
		if (m._m00 == 0.0f)
			return 0.0f;

		return float1x1(1.0 / m._m00);
	}

	static float2x2 inverse(const float2x2& m) {
		float det = m._m00 * m._m11 - m._m10 * m._m01;
		if (det == 0)
			return 0.0f;

		return float2x2(m._m11, m._m01, m._m10, m._m00) / det;
	}

	static float3x3 inverse(const float3x3& m)
	{
		/// 00 01 02
		/// 10 11 12
		/// 20 21 22
		float Min00 = m._m11 * m._m22 - m._m12 * m._m21;
		float Min01 = m._m10 * m._m22 - m._m12 * m._m20;
		float Min02 = m._m10 * m._m21 - m._m11 * m._m20;

		float det = Min00 * m._m00 - Min01 * m._m01 + Min02 * m._m02;

		if (det == 0)
			return float3x3(0);

		float Min10 = m._m01 * m._m22 - m._m02 * m._m21;
		float Min11 = m._m00 * m._m22 - m._m02 * m._m20;
		float Min12 = m._m00 * m._m21 - m._m01 * m._m20;

		float Min20 = m._m01 * m._m12 - m._m02 * m._m11;
		float Min21 = m._m00 * m._m12 - m._m02 * m._m10;
		float Min22 = m._m00 * m._m11 - m._m01 * m._m10;

		return float3x3(
			(+Min00 / det), (-Min10 / det), (+Min20 / det),
			(-Min01 / det), (+Min11 / det), (-Min21 / det),
			(+Min02 / det), (-Min12 / det), (+Min22 / det));
	}

	static float4x4 inverse(const float4x4& m) {
		float Min00 = m._m11 * m._m22 * m._m33 + m._m12 * m._m23 * m._m31 + m._m13 * m._m21 * m._m32 - m._m11 * m._m23 * m._m32 - m._m12 * m._m21 * m._m33 - m._m13 * m._m22 * m._m31;
		float Min01 = m._m10 * m._m22 * m._m33 + m._m12 * m._m23 * m._m30 + m._m13 * m._m20 * m._m32 - m._m10 * m._m23 * m._m32 - m._m12 * m._m20 * m._m33 - m._m13 * m._m22 * m._m30;
		float Min02 = m._m10 * m._m21 * m._m33 + m._m11 * m._m23 * m._m30 + m._m13 * m._m20 * m._m31 - m._m10 * m._m23 * m._m31 - m._m11 * m._m20 * m._m33 - m._m13 * m._m21 * m._m30;
		float Min03 = m._m10 * m._m21 * m._m32 + m._m11 * m._m22 * m._m30 + m._m12 * m._m20 * m._m31 - m._m10 * m._m22 * m._m31 - m._m11 * m._m20 * m._m32 - m._m12 * m._m21 * m._m30;

		float det = Min00 * m._m00 - Min01 * m._m01 + Min02 * m._m02 - Min03 * m._m03;

		if (det == 0)
			return float4x4(0);

		float Min10 = m._m01 * m._m22 * m._m33 + m._m02 * m._m23 * m._m31 + m._m03 * m._m21 * m._m32 - m._m01 * m._m23 * m._m32 - m._m02 * m._m21 * m._m33 - m._m03 * m._m22 * m._m31;
		float Min11 = m._m00 * m._m22 * m._m33 + m._m02 * m._m23 * m._m30 + m._m03 * m._m20 * m._m32 - m._m00 * m._m23 * m._m32 - m._m02 * m._m20 * m._m33 - m._m03 * m._m22 * m._m30;
		float Min12 = m._m00 * m._m21 * m._m33 + m._m01 * m._m23 * m._m30 + m._m03 * m._m20 * m._m31 - m._m00 * m._m23 * m._m31 - m._m01 * m._m20 * m._m33 - m._m03 * m._m21 * m._m30;
		float Min13 = m._m00 * m._m21 * m._m32 + m._m01 * m._m22 * m._m30 + m._m02 * m._m20 * m._m31 - m._m00 * m._m22 * m._m31 - m._m01 * m._m20 * m._m32 - m._m02 * m._m21 * m._m30;

		float Min20 = m._m01 * m._m12 * m._m33 + m._m02 * m._m13 * m._m31 + m._m03 * m._m11 * m._m32 - m._m01 * m._m13 * m._m32 - m._m02 * m._m11 * m._m33 - m._m03 * m._m12 * m._m31;
		float Min21 = m._m00 * m._m12 * m._m33 + m._m02 * m._m13 * m._m30 + m._m03 * m._m10 * m._m32 - m._m00 * m._m13 * m._m32 - m._m02 * m._m10 * m._m33 - m._m03 * m._m12 * m._m30;
		float Min22 = m._m00 * m._m11 * m._m33 + m._m01 * m._m13 * m._m30 + m._m03 * m._m10 * m._m31 - m._m00 * m._m13 * m._m31 - m._m01 * m._m10 * m._m33 - m._m03 * m._m11 * m._m30;
		float Min23 = m._m00 * m._m11 * m._m32 + m._m01 * m._m12 * m._m30 + m._m02 * m._m10 * m._m31 - m._m00 * m._m12 * m._m31 - m._m01 * m._m10 * m._m32 - m._m02 * m._m11 * m._m30;

		float Min30 = m._m01 * m._m12 * m._m23 + m._m02 * m._m13 * m._m21 + m._m03 * m._m11 * m._m22 - m._m01 * m._m13 * m._m22 - m._m02 * m._m11 * m._m23 - m._m03 * m._m12 * m._m21;
		float Min31 = m._m00 * m._m12 * m._m23 + m._m02 * m._m13 * m._m20 + m._m03 * m._m10 * m._m22 - m._m00 * m._m13 * m._m22 - m._m02 * m._m10 * m._m23 - m._m03 * m._m12 * m._m20;
		float Min32 = m._m00 * m._m11 * m._m23 + m._m01 * m._m13 * m._m20 + m._m03 * m._m10 * m._m21 - m._m00 * m._m13 * m._m21 - m._m01 * m._m10 * m._m23 - m._m03 * m._m11 * m._m20;
		float Min33 = m._m00 * m._m11 * m._m22 + m._m01 * m._m12 * m._m20 + m._m02 * m._m10 * m._m21 - m._m00 * m._m12 * m._m21 - m._m01 * m._m10 * m._m22 - m._m02 * m._m11 * m._m20;

		return float4x4(
			(+Min00 / det), (-Min10 / det), (+Min20 / det), (-Min30 / det),
			(-Min01 / det), (+Min11 / det), (-Min21 / det), (+Min31 / det),
			(+Min02 / det), (-Min12 / det), (+Min22 / det), (-Min32 / det),
			(-Min03 / det), (+Min13 / det), (-Min23 / det), (+Min33 / det));
	}

#pragma endregion

	class Transforms {
	public:

		/// matrices
	/// <summary>
	/// Builds a mat using specified offsets.
	/// </summary>
	/// <param name="xslide">x offsets</param>
	/// <param name="yslide">y offsets</param>
	/// <param name="zslide">z offsets</param>
	/// <returns>A mat structure that contains a translated transformation </returns>
		static float4x4 Translate(float xoffset, float yoffset, float zoffset)
		{
			return float4x4(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				xoffset, yoffset, zoffset, 1
			);
		}
		/// <summary>
		/// Builds a mat using specified offsets.
		/// </summary>
		/// <param name="vec">A Vector structure that contains the x-coordinate, y-coordinate, and z-coordinate offsets.</param>
		/// <returns>A mat structure that contains a translated transformation </returns>
		static float4x4 Translate(float3 vec)
		{
			return Translate(vec.x, vec.y, vec.z);
		}
		//

		static float4x4 FromAffine(float4x3 affine) {
			return float4x4(
				affine._m00, affine._m01, affine._m02, 0,
				affine._m10, affine._m11, affine._m12, 0,
				affine._m20, affine._m21, affine._m22, 0,
				affine._m30, affine._m31, affine._m32, 1
			);
		}

		// Rotations
		/// <summary>
		/// Rotation mat around Z axis
		/// </summary>
		/// <param name="alpha">value in radian for rotation</param>
		static float4x4 RotateZ(float alpha)
		{
			float cos = cosf(alpha);
			float sin = sinf(alpha);
			return float4x4(
				cos, -sin, 0, 0,
				sin, cos, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			);
		}
		/// <summary>
		/// Rotation mat around Z axis
		/// </summary>
		/// <param name="alpha">value in grades for rotation</param>
		static float4x4 RotateZGrad(float alpha)
		{
			return RotateZ(alpha * PI / 180);
		}
		/// <summary>
		/// Rotation mat around Z axis
		/// </summary>
		/// <param name="alpha">value in radian for rotation</param>
		static float4x4 RotateY(float alpha)
		{
			float cos = cosf(alpha);
			float sin = sinf(alpha);
			return float4x4(
				cos, 0, -sin, 0,
				0, 1, 0, 0,
				sin, 0, cos, 0,
				0, 0, 0, 1
			);
		}
		/// <summary>
		/// Rotation mat around Z axis
		/// </summary>
		/// <param name="alpha">value in grades for rotation</param>
		static float4x4 RotateYGrad(float alpha)
		{
			return RotateY(alpha * PI / 180);
		}
		/// <summary>
		/// Rotation mat around Z axis
		/// </summary>
		/// <param name="alpha">value in radian for rotation</param>
		static float4x4 RotateX(float alpha)
		{
			float cos = cosf(alpha);
			float sin = sinf(alpha);
			return float4x4(
				1, 0, 0, 0,
				0, cos, -sin, 0,
				0, sin, cos, 0,
				0, 0, 0, 1
			);
		}
		/// <summary>
		/// Rotation mat around Z axis
		/// </summary>
		/// <param name="alpha">value in grades for rotation</param>
		static float4x4 RotateXGrad(float alpha)
		{
			return RotateX(alpha * PI / 180);
		}
		static float4x4 Rotate(float angle, const float3& dir)
		{
			float x = dir.x;
			float y = dir.y;
			float z = dir.z;
			float cos = cosf(angle);
			float sin = sinf(angle);

			return float4x4(
				x * x * (1 - cos) + cos, y * x * (1 - cos) + z * sin, z * x * (1 - cos) - y * sin, 0,
				x * y * (1 - cos) - z * sin, y * y * (1 - cos) + cos, z * y * (1 - cos) + x * sin, 0,
				x * z * (1 - cos) + y * sin, y * z * (1 - cos) - x * sin, z * z * (1 - cos) + cos, 0,
				0, 0, 0, 1
			);
		}
		static float4x4 RotateRespectTo(const float3& center, const float3& direction, float angle)
		{
			return mul(Translate(center), mul(Rotate(angle, direction), Translate(center * -1.0f)));
		}
		static float4x4 RotateGrad(float angle, const float3& dir)
		{
			return Rotate(PI * angle / 180, dir);
		}

		//

		// Scale

		static float4x4 Scale(float xscale, float yscale, float zscale)
		{
			return float4x4(
				xscale, 0, 0, 0,
				0, yscale, 0, 0,
				0, 0, zscale, 0,
				0, 0, 0, 1);
		}
		static float4x4 Scale(const float3& size)
		{
			return Scale(size.x, size.y, size.z);
		}

		static float4x4 ScaleRespectTo(const float3& center, const float3& size)
		{
			return mul(mul(Translate(center), Scale(size)), Translate(center * -1));
		}
		static float4x4 ScaleRespectTo(const float3& center, float sx, float sy, float sz)
		{
			return ScaleRespectTo(center, float3(sx, sy, sz));
		}

		//

		// Viewing

		static float4x4 LookAtLH(const float3& camera, const float3& target, const float3& upVector)
		{
			float3 zaxis = normalize(target - camera);
			float3 xaxis = normalize(cross(upVector, zaxis));
			float3 yaxis = cross(zaxis, xaxis);

			return float4x4(
				xaxis.x, yaxis.x, zaxis.x, 0,
				xaxis.y, yaxis.y, zaxis.y, 0,
				xaxis.z, yaxis.z, zaxis.z, 0,
				-dot(xaxis, camera), -dot(yaxis, camera), -dot(zaxis, camera), 1);
		}

		static float4x4 LookAtRH(const float3& camera, const float3& target, const float3& upVector)
		{
			float3 zaxis = normalize(camera - target);
			float3 xaxis = normalize(cross(upVector, zaxis));
			float3 yaxis = cross(zaxis, xaxis);

			return float4x4(
				xaxis.x, yaxis.x, zaxis.x, 0,
				xaxis.y, yaxis.y, zaxis.y, 0,
				xaxis.z, yaxis.z, zaxis.z, 0,
				-dot(xaxis, camera), -dot(yaxis, camera), -dot(zaxis, camera), 1);
		}

		//

		// Projection Methods

		/// <summary>
		/// Returns the near plane distance to a given projection
		/// </summary>
		/// <param name="proj">A mat structure containing the projection</param>
		/// <returns>A float value representing the distance.</returns>
		static float ZnearPlane(const float4x4& proj)
		{
			float4 pos = mul(float4(0, 0, 0, 1), inverse(proj));
			return pos.z / pos.w;
		}

		/// <summary>
		/// Returns the far plane distance to a given projection
		/// </summary>
		/// <param name="proj">A mat structure containing the projection</param>
		/// <returns>A float value representing the distance.</returns>
		static float ZfarPlane(const float4x4& proj)
		{
			float4 targetPos = mul(float4(0, 0, 1, 1), inverse(proj));
			return targetPos.z / targetPos.w;
		}

		static float4x4 PerspectiveFovLH(float fieldOfView, float aspectRatio, float znearPlane, float zfarPlane)
		{
			float h = 1.0f / tanf(fieldOfView / 2);
			float w = h * aspectRatio;

			return float4x4(
				w, 0, 0, 0,
				0, h, 0, 0,
				0, 0, zfarPlane / (zfarPlane - znearPlane), 1,
				0, 0, -znearPlane * zfarPlane / (zfarPlane - znearPlane), 0);
		}

		static float4x4 PerspectiveFovRH(float fieldOfView, float aspectRatio, float znearPlane, float zfarPlane)
		{
			float h = 1.0f / tanf(fieldOfView / 2);
			float w = h * aspectRatio;

			return float4x4(
				w, 0, 0, 0,
				0, h, 0, 0,
				0, 0, zfarPlane / (znearPlane - zfarPlane), -1,
				0, 0, znearPlane * zfarPlane / (znearPlane - zfarPlane), 0);
		}

		static float4x4 PerspectiveLH(float width, float height, float znearPlane, float zfarPlane)
		{
			return float4x4(
				2 * znearPlane / width, 0, 0, 0,
				0, 2 * znearPlane / height, 0, 0,
				0, 0, zfarPlane / (zfarPlane - znearPlane), 1,
				0, 0, znearPlane * zfarPlane / (znearPlane - zfarPlane), 0);
		}

		static float4x4 PerspectiveRH(float width, float height, float znearPlane, float zfarPlane)
		{
			return float4x4(
				2 * znearPlane / width, 0, 0, 0,
				0, 2 * znearPlane / height, 0, 0,
				0, 0, zfarPlane / (znearPlane - zfarPlane), -1,
				0, 0, znearPlane * zfarPlane / (znearPlane - zfarPlane), 0);
		}

		static float4x4 OrthoLH(float width, float height, float znearPlane, float zfarPlane)
		{
			return float4x4(
				2 / width, 0, 0, 0,
				0, 2 / height, 0, 0,
				0, 0, 1 / (zfarPlane - znearPlane), 0,
				0, 0, znearPlane / (znearPlane - zfarPlane), 1);
		}

		static float4x4 OrthoRH(float width, float height, float znearPlane, float zfarPlane)
		{
			return float4x4(
				2 / width, 0, 0, 0,
				0, 2 / height, 0, 0,
				0, 0, 1 / (znearPlane - zfarPlane), 0,
				0, 0, znearPlane / (znearPlane - zfarPlane), 1);
		}
	};

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
			obj._this = _this ? dynamic_cast<T*>(_this) : nullptr;
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

#pragma region Resources

#pragma region Color Formats

	typedef struct ARGB
	{
		unsigned int value;

		int Alpha() { return value >> 24; }
		int Red() { return (value & 0xFF0000) >> 16; }
		int Green() { return (value & 0xFF00) >> 8; }
		int Blue() { return (value & 0xFF); }

		ARGB() { value = 0; }

		ARGB(int alpha, int red, int green, int blue) {
			value =
				((unsigned int)alpha) << 24 |
				((unsigned int)red) << 16 |
				((unsigned int)green) << 8 |
				((unsigned int)blue);
		}
	} ARGB;

	typedef struct RGBA
	{
		unsigned int value;

		int Alpha() { return value >> 24; }
		int Blue() { return (value & 0xFF0000) >> 16; }
		int Green() { return (value & 0xFF00) >> 8; }
		int Red() { return (value & 0xFF); }

		RGBA() { value = 0; }

		RGBA(int alpha, int red, int green, int blue) {
			value =
				((unsigned int)alpha) << 24 |
				((unsigned int)blue) << 16 |
				((unsigned int)green) << 8 |
				((unsigned int)red);
		}
	} RGBA;

#pragma endregion

#pragma region Formats from Generic type

	template<typename T>
	struct Formats {
		static const DXGI_FORMAT Value = DXGI_FORMAT_UNKNOWN;
		static const int Size = 1;
	};
	template<>
	struct Formats<char> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R8_SINT;
		static const int Size = 1;
	};
	template<>
	struct Formats<float> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32_FLOAT;
		static const int Size = 4;
	};
	template<>
	struct Formats <int> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32_SINT;
		static const int Size = 4;
	};
	template<>
	struct Formats <uint> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32_UINT;
		static const int Size = 4;
	};

	template<>
	struct Formats<float1> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32_FLOAT;
		static const int Size = 4;
	};
	template<>
	struct Formats <int1> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32_SINT;
		static const int Size = 4;
	};
	template<>
	struct Formats <uint1> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32_UINT;
		static const int Size = 4;
	};

	template<>
	struct Formats <ARGB> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_B8G8R8A8_UNORM;
		static const int Size = 4;
	};

	template<>
	struct Formats <RGBA> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const int Size = 4;
	};

	template<>
	struct Formats <float2> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32_FLOAT;
		static const int Size = 8;
	};
	template<>
	struct Formats <float3> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32B32_FLOAT;
		static const int Size = 12;
	};
	template<>
	struct Formats <float4> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32B32A32_FLOAT;
		static const int Size = 16;
	};

	template<>
	struct Formats <int2> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32_SINT;
		static const int Size = 8;
	};
	template<>
	struct Formats <int3> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32B32_SINT;
		static const int Size = 12;
	};
	template<>
	struct Formats <int4> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32B32A32_SINT;
		static const int Size = 16;
	};

	template<>
	struct Formats <uint2> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32_UINT;
		static const int Size = 8;
	};
	template<>
	struct Formats <uint3> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32B32_UINT;
		static const int Size = 12;
	};
	template<>
	struct Formats <uint4> {
		static const DXGI_FORMAT Value = DXGI_FORMAT_R32G32B32A32_UINT;
		static const int Size = 16;
	};

#pragma endregion

	enum class CPUAccessibility {
		// The resource can not be read or write directly from the CPU.
		// A stagging internal version is used for so.
		// This grant an optimized use of the GPU access. It is the most common usage for resources.
		None,
		// The resource can be written fast by the CPU and the GPU can read moderatly.
		// Can be used for dynamic constant buffers that requires update every frame.
		Write,
		// The resource can be read fast by the CPU and the GPU can write moderately.
		// Only use in case of a every-frame data required to be drawback from the GPU
		Read
	};

	// Represents a sampler object
	struct Sampler {
		D3D12_FILTER Filter;
		D3D12_TEXTURE_ADDRESS_MODE AddressU;
		D3D12_TEXTURE_ADDRESS_MODE AddressV;
		D3D12_TEXTURE_ADDRESS_MODE AddressW;
		FLOAT MipLODBias;
		UINT MaxAnisotropy;
		D3D12_COMPARISON_FUNC ComparisonFunc;
		float4 BorderColor;
		FLOAT MinLOD;
		FLOAT MaxLOD;

		// Creates a default point sampling object.
		static Sampler Point(
			D3D12_TEXTURE_ADDRESS_MODE AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP
		)
		{
			return Create(D3D12_FILTER_MIN_MAG_MIP_POINT,
				AddressU,
				AddressV,
				AddressW);
		}

		// Creates a default point sampling object.
		static Sampler PointWithoutMipMaps(
			D3D12_TEXTURE_ADDRESS_MODE AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP
		)
		{
			return Create(D3D12_FILTER_MIN_MAG_MIP_POINT,
				AddressU,
				AddressV,
				AddressW, 0, 0, D3D12_COMPARISON_FUNC_ALWAYS, float4(0, 0, 0, 0), 0, 0);
		}

		// Creates a default linear sampling object
		static Sampler Linear(
			D3D12_TEXTURE_ADDRESS_MODE AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP
		)
		{
			return Create(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				AddressU,
				AddressV,
				AddressW);
		}

		static Sampler LinearWithoutMipMaps(
			D3D12_TEXTURE_ADDRESS_MODE AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			D3D12_TEXTURE_ADDRESS_MODE AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			D3D12_TEXTURE_ADDRESS_MODE AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER
		)
		{
			return Create(D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
				AddressU,
				AddressV,
				AddressW, 0, 0, D3D12_COMPARISON_FUNC_ALWAYS, float4(0, 0, 0, 0), 0, 0);
		}

		// Creates a default anisotropic sampling object
		static Sampler Anisotropic(
			D3D12_TEXTURE_ADDRESS_MODE AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP
		)
		{
			return Create(D3D12_FILTER_ANISOTROPIC,
				AddressU,
				AddressV,
				AddressW);
		}
	private:
		static Sampler Create(
			D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			float mipLODBias = 0.0f,
			UINT maxAnisotropy = 16,
			D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS,
			float4 borderColor = float4(0, 0, 0, 0),
			float minLOD = 0,
			float maxLOD = D3D12_FLOAT32_MAX
		) {
			return Sampler{
				filter,
				AddressU,
				AddressV,
				AddressW,
				mipLODBias,
				maxAnisotropy,
				comparisonFunc,
				borderColor,
				minLOD,
				maxLOD
			};
		}
	};

	class ResourceView {
		friend wDevice;
		friend CopyManager;
		friend ComputeManager;
		friend GraphicsManager;
		friend RaytracingManager;
		friend wBindings;
		friend TriangleGeometryCollection;
		friend ProceduralGeometryCollection;
		friend InstanceCollection;

	protected:
		/// <summary>
		/// Internal object used to wrap a DX resource 
		/// </summary>
		wResource* w_resource = nullptr;
		/// <summary>
		/// Internal object used to wrap a DX resource view
		/// </summary>
		wResourceView* w_view = nullptr;

		/// <summary>
		/// Creates a new view to a existing resource.
		/// </summary>
		ResourceView(wDevice* w_device, wResource* w_resource, wResourceView* w_view = nullptr);

		/// <summary>
		/// Tool method to create the initial view for a DX12 resource.
		/// </summary>
		static wResourceView* CreateDefaultView(wResource* w_resource, wDevice* w_device);

		/// <summary>
		/// Gets the offset of a specific element in the permanent mapped data to write.
		/// </summary>
		byte* MappedElement(int col = 0, int row = 0, int depthOrArray = 0, int mip = 0);

	public:
		static gObj<ResourceView> CreateNullView(wDevice* device, D3D12_RESOURCE_DIMENSION dimension);

		/// <summary>
		/// Sets a debug name to tag the resource in debug output.
		/// </summary>
		void SetDebugName(LPCWSTR name);

		/// <summary>
		/// Gets the accessibility of the underlaying resource for read or write from cpu.
		/// </summary>
		CPUAccessibility CPUAccessibility() const;

		/// <summary>
		/// Gets the total size in bytes this resource (or subresource) requires 
		/// </summary>
		long SizeInBytes(int subresource = 0) const;

		/// <summary>
		/// Gets the size in bytes of a single element of this resource 
		/// </summary>
		uint ElementStride() const;

		/// <summary>
		/// Gets the number of subresources in this view
		/// </summary>
		int Subresources() const;

		virtual ~ResourceView();

		/// <summary>
		/// Writes to uploading memory the data.
		/// </summary>
		/// <param name="flipRows">True if rows should be consider inverted.</param>
		void Write(byte* data, bool flipRows = false);

		/// <summary>
		/// Reads from downloading memory the data.
		/// </summary>
		/// <param name="flipRows">True if rows should be consider inverted.</param>
		void Read(byte* data, bool flipRows = false);

		/// <summary>
		/// Writes a region in the main subresource to uploading memory.
		/// </summary>
		/// <param name="flipRows">True if rows should be consider inverted.</param>
		void WriteRegion(byte* data, const D3D12_BOX& region, bool flipRows = false);

		/// <summary>
		/// Reads a region in the main subresource from downloading memory.
		/// </summary>
		/// <param name="flipRows">True if rows should be consider inverted.</param>
		void ReadRegion(byte* data, const D3D12_BOX& region, bool flipRows = false);

		/// <summary>
		/// Writes to uploading memory the data.
		/// </summary>
		template <typename T>
		void Write_Ptr(T* data) {
			Write((byte*)data);
		}

		/// <summary>
		/// Writes to uploading memory the data.
		/// </summary>
		template<typename T>
		void Write_List(std::initializer_list<T> data) {
			Write_Ptr(data.begin());
		}

		/// <summary>
		/// Writes to uploading memory the data.
		/// </summary>
		template<typename T>
		void Write_Value(const T& elementValue) {
			Write_Ptr(&elementValue);
		}
	};

	class Buffer : public ResourceView {
		friend DeviceManager;

		Buffer(
			wDevice* w_device,
			wResource* w_resource,
			wResourceView* w_view) :
			ResourceView(w_device, w_resource, w_view)
		{
		}
	public:
		/// <summary>
		/// Number of elements in this buffer.
		/// </summary>
		unsigned int ElementCount() const;

		/// <summary>
		/// Gets the virtual address of a specific element of this buffer on GPU memory.
		/// </summary>
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress(int element = 0) const;

		template<typename T>
		void Write_Element(int index, const T& value) {
			if (sizeof(T) != ElementStride())
				throw Exception::FromError(Errors::Invalid_Operation, "Invalid element size");
			memcpy((void*)MappedElement(index, 0, 0, 0), (void*)&value, sizeof(T));
		}

		/// <summary>
		/// Creates a slice of this buffer considering only elements from start, taking count.
		/// </summary>
		gObj<Buffer> Slice(int start, int count) const;
	};

	class Texture1D : public ResourceView {
		friend DeviceManager;

	protected:
		Texture1D(
			wDevice* w_device,
			wResource* w_resource,
			wResourceView* w_view) :
			ResourceView(w_device, w_resource, w_view)
		{
		}
	public:
		// Gets the format of each element in this Texture1D
		DXGI_FORMAT Format() const;

		// Gets the number of elements for this Texture1D
		unsigned int Width() const;

		// Gets the length of this Texture1D array
		unsigned int ArrayLength() const;

		// Gets the number of mips of this Texture1D
		unsigned int Mips() const;

		gObj<Texture1D> Slice_Mips(int start, int count) const;

		gObj<Texture1D> Slice_Array(int start, int count) const;

		gObj<Texture1D> Subresource(int mip, int arrayIndex) const {
			return Slice_Array(arrayIndex, 1)->Slice_Mips(mip, 1);
		}

		template<typename T>
		void Write_Element(int tx, const T& value, int mip = 0, int slice = 0) {
			if (sizeof(T) != ElementStride())
				throw Exception::FromError(Errors::Invalid_Operation, "Invalid element size");
			memcpy((void*)MappedElement(tx, 0, slice, mip), (void*)&value, sizeof(T));
		}
	};

	class Texture2D : public ResourceView {
		friend DeviceManager;
		friend wDevice;
		friend CopyManager;
	protected:
		Texture2D(
			wDevice* w_device,
			wResource* w_resource,
			wResourceView* w_view) :
			ResourceView(w_device, w_resource, w_view) {
		}
	public:
		// Gets the format of each element in this Texture2D
		DXGI_FORMAT Format() const;

		// Gets the width for this Texture2D
		unsigned int Width() const;

		// Gets the height for this Texture2D
		unsigned int Height() const;

		// Gets the length of this Texture2D array
		unsigned int ArrayLength() const;

		// Gets the number of mips of this Texture2D
		unsigned int Mips() const;

		gObj<Texture2D> Slice_Mips(int start, int count) const;

		gObj<Texture2D> Slice_Array(int start, int count) const;

		gObj<Texture2D> Subresource(int mip, int arrayIndex) const {
			return Slice_Array(arrayIndex, 1)->Slice_Mips(mip, 1);
		}

		template<typename T>
		void Write_Element(int tx, int ty, const T& value, int mip = 0, int slice = 0) {
			if (sizeof(T) != this->ElementStride())
				throw Exception::FromError(Errors::Invalid_Operation, "Invalid element size");
			memcpy((void*)MappedElement(tx, ty, slice, mip), (void*)&value, sizeof(T));
		}
	};

	class Texture3D : public ResourceView {
		friend DeviceManager;

	protected:
		Texture3D(
			wDevice* w_device,
			wResource* w_resource,
			wResourceView* w_view) :
			ResourceView(w_device, w_resource, w_view) {
		}
	public:
		// Gets the format of each element in this Texture2D
		DXGI_FORMAT Format() const;
		// Gets the width for this Texture3D
		unsigned int Width() const;
		// Gets the height for this Texture3D
		unsigned int Height() const;
		// Gets the depth (number of slices) of this Texture3D
		unsigned int Depth() const;
		// Gets the number of mips of this Texture3D
		unsigned int Mips() const;

		gObj<Texture3D> Slice_Mips(int start, int count) const;

		template<typename T>
		void Write_Element(int tx, int ty, int tz, const T& value, int mip = 0) {
			if (sizeof(T) != ElementStride())
				throw Exception::FromError(Errors::Invalid_Operation, "Invalid element size");
			memcpy((void*)MappedElement(tx, ty, tz, mip), (void*)&value, sizeof(T));
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

	template<typename A>
	struct EngineType {
		static const Engine Type;
	};
	template<>
	struct EngineType<GraphicsManager> {
		static const Engine Type = Engine::Direct;
	};
	template<>
	struct EngineType<RaytracingManager> {
		static const Engine Type = Engine::Direct;
	};
	template<>
	struct EngineType<ComputeManager> {
		static const Engine Type = Engine::Compute;
	};
	template<>
	struct EngineType<CopyManager> {
		static const Engine Type = Engine::Copy;
	};

	template<typename T, typename A>
	struct MethodAsGPUProcess : public GPUProcess {
		T* instance;
		typedef void(T::* Member)(gObj<A>);
		Member function;
		MethodAsGPUProcess(T* instance, Member function) : instance(instance), function(function) {
		}

		void OnCollect(gObj<CommandListManager> manager) {
			(instance->*function)(manager.Dynamic_Cast<A>());
		}

		Engine RequiredEngine() {
			return EngineType<A>::Type;
		}
	};

#pragma endregion

#pragma region Pipelines

#pragma region Compiled Shaders

#if 0
	//
	// Generated by Microsoft (R) HLSL Shader Compiler 10.1
	//
	//
	//
	// Input signature:
	//
	// Name                 Index   Mask Register SysValue  Format   Used
	// -------------------- ----- ------ -------- -------- ------- ------
	// POSITION                 0   xy          0     NONE   float   xy  
	//
	//
	// Output signature:
	//
	// Name                 Index   Mask Register SysValue  Format   Used
	// -------------------- ----- ------ -------- -------- ------- ------
	// SV_POSITION              0   xyzw        0      POS   float   xyzw
	// TEXCOORD                 0   xy          1     NONE   float   xy  
	//
	vs_5_0
		dcl_globalFlags refactoringAllowed | skipOptimization
		dcl_input v0.xy
		dcl_output_siv o0.xyzw, position
		dcl_output o1.xy
		dcl_temps 1
		//
		// Initial variable locations:
		//   v0.x <- pos.x; v0.y <- pos.y; 
		//   o0.x <- <main return value>.x; o0.y <- <main return value>.y; o0.z <- <main return value>.z; o0.w <- <main return value>.w; 
		//   o1.x <- C.x; o1.y <- C.y
		//
#line 3 "C:\Users\lleonart\Desktop\PhD Project\CA4G\Shaders\DrawScreen_VS.hlsl"
		itof r0.x, l(1)
		add r0.x, r0.x, v0.x
		mul o1.x, r0.x, l(0.500000)
		itof r0.x, l(1)
		mov r0.y, -v0.y
		add r0.x, r0.y, r0.x
		mul o1.y, r0.x, l(0.500000)

#line 4
		itof o0.w, l(1)
		mov o0.xy, v0.xyxx
		mov o0.z, l(0.500000)
		ret
		// Approximately 11 instruction slots used
#endif

		const BYTE cso_DrawScreen_VS[] =
	{
		 68,  88,  66,  67,  13, 138,
		194, 211,  30, 104, 182, 169,
		 70,  65, 168, 232, 119, 129,
		165, 231,   1,   0,   0,   0,
		  8,  49,   0,   0,   6,   0,
		  0,   0,  56,   0,   0,   0,
		164,   0,   0,   0, 216,   0,
		  0,   0,  48,   1,   0,   0,
		100,   2,   0,   0,   0,   3,
		  0,   0,  82,  68,  69,  70,
		100,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  60,   0,
		  0,   0,   0,   5, 254, 255,
		  5,   1,   0,   0,  60,   0,
		  0,   0,  82,  68,  49,  49,
		 60,   0,   0,   0,  24,   0,
		  0,   0,  32,   0,   0,   0,
		 40,   0,   0,   0,  36,   0,
		  0,   0,  12,   0,   0,   0,
		  0,   0,   0,   0,  77, 105,
		 99, 114, 111, 115, 111, 102,
		116,  32,  40,  82,  41,  32,
		 72,  76,  83,  76,  32,  83,
		104,  97, 100, 101, 114,  32,
		 67, 111, 109, 112, 105, 108,
		101, 114,  32,  49,  48,  46,
		 49,   0,  73,  83,  71,  78,
		 44,   0,   0,   0,   1,   0,
		  0,   0,   8,   0,   0,   0,
		 32,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  3,   0,   0,   0,   0,   0,
		  0,   0,   3,   3,   0,   0,
		 80,  79,  83,  73,  84,  73,
		 79,  78,   0, 171, 171, 171,
		 79,  83,  71,  78,  80,   0,
		  0,   0,   2,   0,   0,   0,
		  8,   0,   0,   0,  56,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   3,   0,
		  0,   0,   0,   0,   0,   0,
		 15,   0,   0,   0,  68,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   3,   0,
		  0,   0,   1,   0,   0,   0,
		  3,  12,   0,   0,  83,  86,
		 95,  80,  79,  83,  73,  84,
		 73,  79,  78,   0,  84,  69,
		 88,  67,  79,  79,  82,  68,
		  0, 171, 171, 171,  83,  72,
		 69,  88,  44,   1,   0,   0,
		 80,   0,   1,   0,  75,   0,
		  0,   0, 106, 136,   0,   1,
		 95,   0,   0,   3,  50,  16,
		 16,   0,   0,   0,   0,   0,
		103,   0,   0,   4, 242,  32,
		 16,   0,   0,   0,   0,   0,
		  1,   0,   0,   0, 101,   0,
		  0,   3,  50,  32,  16,   0,
		  1,   0,   0,   0, 104,   0,
		  0,   2,   1,   0,   0,   0,
		 43,   0,   0,   5,  18,   0,
		 16,   0,   0,   0,   0,   0,
		  1,  64,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   7,
		 18,   0,  16,   0,   0,   0,
		  0,   0,  10,   0,  16,   0,
		  0,   0,   0,   0,  10,  16,
		 16,   0,   0,   0,   0,   0,
		 56,   0,   0,   7,  18,  32,
		 16,   0,   1,   0,   0,   0,
		 10,   0,  16,   0,   0,   0,
		  0,   0,   1,  64,   0,   0,
		  0,   0,   0,  63,  43,   0,
		  0,   5,  18,   0,  16,   0,
		  0,   0,   0,   0,   1,  64,
		  0,   0,   1,   0,   0,   0,
		 54,   0,   0,   6,  34,   0,
		 16,   0,   0,   0,   0,   0,
		 26,  16,  16, 128,  65,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   7,  18,   0,
		 16,   0,   0,   0,   0,   0,
		 26,   0,  16,   0,   0,   0,
		  0,   0,  10,   0,  16,   0,
		  0,   0,   0,   0,  56,   0,
		  0,   7,  34,  32,  16,   0,
		  1,   0,   0,   0,  10,   0,
		 16,   0,   0,   0,   0,   0,
		  1,  64,   0,   0,   0,   0,
		  0,  63,  43,   0,   0,   5,
		130,  32,  16,   0,   0,   0,
		  0,   0,   1,  64,   0,   0,
		  1,   0,   0,   0,  54,   0,
		  0,   5,  50,  32,  16,   0,
		  0,   0,   0,   0,  70,  16,
		 16,   0,   0,   0,   0,   0,
		 54,   0,   0,   5,  66,  32,
		 16,   0,   0,   0,   0,   0,
		  1,  64,   0,   0,   0,   0,
		  0,  63,  62,   0,   0,   1,
		 83,  84,  65,  84, 148,   0,
		  0,   0,  11,   0,   0,   0,
		  1,   0,   0,   0,   0,   0,
		  0,   0,   3,   0,   0,   0,
		  5,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  2,   0,   0,   0,   0,   0,
		  0,   0,   3,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 83,  80,  68,  66,   0,  46,
		  0,   0,  77, 105,  99, 114,
		111, 115, 111, 102, 116,  32,
		 67,  47,  67,  43,  43,  32,
		 77,  83,  70,  32,  55,  46,
		 48,  48,  13,  10,  26,  68,
		 83,   0,   0,   0,   0,   2,
		  0,   0,   2,   0,   0,   0,
		 23,   0,   0,   0, 132,   0,
		  0,   0,   0,   0,   0,   0,
		 22,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 192, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		 56,   0, 128, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255,   5,   0,   0,   0,
		 32,   0,   0,   0,  60,   0,
		  0,   0,   0,   0,   0,   0,
		255, 255, 255, 255,   0,   0,
		  0,   0,   6,   0,   0,   0,
		  5,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   3,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		148,  46,  49,   1, 251, 103,
		166,  92,   1,   0,   0,   0,
		 89,  73, 115,  61, 234,  36,
		100,  75, 169,  34, 138, 198,
		225, 156,  23, 109,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0, 220,  81,  51,   1,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  68,  51,  68,  83,
		 72,  68,  82,   0,  44,   1,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  32,   0,   0,  96,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 117, 131,
		  1,   0, 242, 228,   1,   0,
		198,  90,   0,   0, 229,  18,
		  2,   0, 182,  61,   0,   0,
		  0,  16,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		102, 108, 111,  97, 116,  52,
		 32, 109,  97, 105, 110,  40,
		102, 108, 111,  97, 116,  50,
		 32, 112, 111, 115,  32,  58,
		 32,  80,  79,  83,  73,  84,
		 73,  79,  78,  44,  32, 111,
		117, 116,  32, 102, 108, 111,
		 97, 116,  50,  32,  67,  32,
		 58,  32,  84,  69,  88,  67,
		 79,  79,  82,  68,  41,  32,
		 58,  32,  83,  86,  95,  80,
		 79,  83,  73,  84,  73,  79,
		 78,  13,  10, 123,  13,  10,
		  9,  67,  32,  61,  32, 102,
		108, 111,  97, 116,  50,  40,
		 40, 112, 111, 115,  46, 120,
		 32,  43,  32,  49,  41,  42,
		 48,  46,  53,  44,  32,  40,
		 49,  32,  45,  32, 112, 111,
		115,  46, 121,  41,  42,  48,
		 46,  53,  41,  59,  13,  10,
		  9, 114, 101, 116, 117, 114,
		110,  32, 102, 108, 111,  97,
		116,  52,  40, 112, 111, 115,
		 46, 120,  44,  32, 112, 111,
		115,  46, 121,  44,  32,  48,
		 46,  53,  44,  32,  49,  41,
		 59,  13,  10, 125,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0, 254, 239, 254, 239,
		  1,   0,   0,   0,  53,   1,
		  0,   0,   0,  67,  58,  92,
		 85, 115, 101, 114, 115,  92,
		108, 108, 101, 111, 110,  97,
		114, 116,  92,  68, 101, 115,
		107, 116, 111, 112,  92,  80,
		104,  68,  32,  80, 114, 111,
		106, 101,  99, 116,  92,  67,
		 65,  52,  71,  92,  83, 104,
		 97, 100, 101, 114, 115,  92,
		 68, 114,  97, 119,  83,  99,
		114, 101, 101, 110,  95,  86,
		 83,  46, 104, 108, 115, 108,
		  0,   0,  99,  58,  92, 117,
		115, 101, 114, 115,  92, 108,
		108, 101, 111, 110,  97, 114,
		116,  92, 100, 101, 115, 107,
		116, 111, 112,  92, 112, 104,
		100,  32, 112, 114, 111, 106,
		101,  99, 116,  92,  99,  97,
		 52, 103,  92, 115, 104,  97,
		100, 101, 114, 115,  92, 100,
		114,  97, 119, 115,  99, 114,
		101, 101, 110,  95, 118, 115,
		 46, 104, 108, 115, 108,   0,
		102, 108, 111,  97, 116,  52,
		 32, 109,  97, 105, 110,  40,
		102, 108, 111,  97, 116,  50,
		 32, 112, 111, 115,  32,  58,
		 32,  80,  79,  83,  73,  84,
		 73,  79,  78,  44,  32, 111,
		117, 116,  32, 102, 108, 111,
		 97, 116,  50,  32,  67,  32,
		 58,  32,  84,  69,  88,  67,
		 79,  79,  82,  68,  41,  32,
		 58,  32,  83,  86,  95,  80,
		 79,  83,  73,  84,  73,  79,
		 78,  13,  10, 123,  13,  10,
		  9,  67,  32,  61,  32, 102,
		108, 111,  97, 116,  50,  40,
		 40, 112, 111, 115,  46, 120,
		 32,  43,  32,  49,  41,  42,
		 48,  46,  53,  44,  32,  40,
		 49,  32,  45,  32, 112, 111,
		115,  46, 121,  41,  42,  48,
		 46,  53,  41,  59,  13,  10,
		  9, 114, 101, 116, 117, 114,
		110,  32, 102, 108, 111,  97,
		116,  52,  40, 112, 111, 115,
		 46, 120,  44,  32, 112, 111,
		115,  46, 121,  44,  32,  48,
		 46,  53,  44,  32,  49,  41,
		 59,  13,  10, 125,   0,   7,
		  0,   0,   0,   0,   0,   0,
		  0,  71,   0,   0,   0, 142,
		  0,   0,   0,   0,   0,   0,
		  0,  72,   0,   0,   0,   1,
		  0,   0,   0,   0,   0,   0,
		  0,   4,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  27, 226,
		 48,   1, 128,   0,   0,   0,
		  7,  15, 215, 102,  36, 235,
		212,   1,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  2,   0,   0,   0,   1,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,  72,   0,
		  0,   0,  40,   0,   0,   0,
		 27, 226,  48,   1, 219, 168,
		 71,   1, 166,   0,   0,   0,
		  1,   0,   0,   0,  71,   0,
		  0,   0,  72,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  4,   0,   0,   0,  66,   0,
		 60,  17,  16,   1,   0,   0,
		  0,   1,  10,   0,   1,   0,
		132,   0,  99,  69,  10,   0,
		  1,   0, 132,   0,  99,  69,
		 77, 105,  99, 114, 111, 115,
		111, 102, 116,  32,  40,  82,
		 41,  32,  72,  76,  83,  76,
		 32,  83, 104,  97, 100, 101,
		114,  32,  67, 111, 109, 112,
		105, 108, 101, 114,  32,  49,
		 48,  46,  49,   0,   0,   0,
		 54,   0,  61,  17,   1, 104,
		108, 115, 108,  70, 108,  97,
		103, 115,   0,  48, 120,  53,
		  0, 104, 108, 115, 108,  84,
		 97, 114, 103, 101, 116,   0,
		118, 115,  95,  53,  95,  48,
		  0, 104, 108, 115, 108,  69,
		110, 116, 114, 121,   0, 109,
		 97, 105, 110,   0,   0,   0,
		  0,   0,  42,   0,  16,  17,
		  0,   0,   0,   0,   0,   2,
		  0,   0,   0,   0,   0,   0,
		240,   0,   0,   0,   0,   0,
		  0,   0, 240,   0,   0,   0,
		  4,  16,   0,   0,  60,   0,
		  0,   0,   1,   0, 160, 109,
		 97, 105, 110,   0,  42,   0,
		 62,  17,   0,  16,   0,   0,
		  9,   0, 112, 111, 115,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 22,   0,  80,  17,   1,   0,
		  5,   0,   0,   0,   4,   0,
		 60,   0,   0,   0,   1,   0,
		240,   0,   0,   0,   0,   0,
		 22,   0,  80,  17,   1,   0,
		  5,   0,   4,   0,   4,   0,
		 60,   0,   0,   0,   1,   0,
		240,   0,   4,   0,   0,   0,
		 58,   0,  62,  17,   3,  16,
		  0,   0, 136,   0,  60, 109,
		 97, 105, 110,  32, 114, 101,
		116, 117, 114, 110,  32, 118,
		 97, 108, 117, 101,  62,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 22,   0,  80,  17,   2,   0,
		  5,   0,   0,   0,   4,   0,
		 60,   0,   0,   0,   1,   0,
		240,   0,   0,   0,   0,   0,
		 22,   0,  80,  17,   2,   0,
		  5,   0,   4,   0,   4,   0,
		 60,   0,   0,   0,   1,   0,
		240,   0,   4,   0,   0,   0,
		 22,   0,  80,  17,   2,   0,
		  5,   0,   8,   0,   4,   0,
		 60,   0,   0,   0,   1,   0,
		240,   0,   8,   0,   0,   0,
		 22,   0,  80,  17,   2,   0,
		  5,   0,  12,   0,   4,   0,
		 60,   0,   0,   0,   1,   0,
		240,   0,  12,   0,   0,   0,
		 42,   0,  62,  17,   0,  16,
		  0,   0,   9,   0,  67,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  22,   0,  80,  17,
		  2,   0,   5,   0,   0,   0,
		  4,   0,  60,   0,   0,   0,
		  1,   0, 240,   0,  16,   0,
		  0,   0,  22,   0,  80,  17,
		  2,   0,   5,   0,   4,   0,
		  4,   0,  60,   0,   0,   0,
		  1,   0, 240,   0,  20,   0,
		  0,   0,   2,   0,   6,   0,
		244,   0,   0,   0,  24,   0,
		  0,   0,   1,   0,   0,   0,
		 16,   1, 119, 215,   1,  38,
		152, 235,  85, 238,  17, 147,
		 85, 162, 251, 248,  73,  96,
		  0,   0, 242,   0,   0,   0,
		 32,   1,   0,   0,   0,   0,
		  0,   0,   1,   0,   1,   0,
		 44,   1,   0,   0,   0,   0,
		  0,   0,  22,   0,   0,   0,
		 20,   1,   0,   0,  60,   0,
		  0,   0,   3,   0,   0, 128,
		 60,   0,   0,   0,   3,   0,
		  0,   0,  80,   0,   0,   0,
		  3,   0,   0, 128,  80,   0,
		  0,   0,   3,   0,   0,   0,
		108,   0,   0,   0,   3,   0,
		  0, 128, 108,   0,   0,   0,
		  3,   0,   0,   0, 136,   0,
		  0,   0,   3,   0,   0, 128,
		136,   0,   0,   0,   3,   0,
		  0,   0, 156,   0,   0,   0,
		  3,   0,   0, 128, 156,   0,
		  0,   0,   3,   0,   0,   0,
		180,   0,   0,   0,   3,   0,
		  0, 128, 180,   0,   0,   0,
		  3,   0,   0,   0, 208,   0,
		  0,   0,   3,   0,   0, 128,
		208,   0,   0,   0,   3,   0,
		  0,   0, 236,   0,   0,   0,
		  4,   0,   0, 128, 236,   0,
		  0,   0,   4,   0,   0,   0,
		  0,   1,   0,   0,   4,   0,
		  0, 128,   0,   1,   0,   0,
		  4,   0,   0,   0,  20,   1,
		  0,   0,   4,   0,   0, 128,
		 20,   1,   0,   0,   4,   0,
		  0,   0,  40,   1,   0,   0,
		  4,   0,   0, 128,  40,   1,
		  0,   0,   4,   0,   0,   0,
		  2,   0,  46,   0,  14,   0,
		 22,   0,   2,   0,  46,   0,
		 14,   0,  22,   0,   2,   0,
		 46,   0,  13,   0,  27,   0,
		  2,   0,  46,   0,  31,   0,
		 39,   0,   2,   0,  46,   0,
		 31,   0,  39,   0,   2,   0,
		 46,   0,  31,   0,  39,   0,
		  2,   0,  46,   0,  30,   0,
		 44,   0,   2,   0,  37,   0,
		  9,   0,  36,   0,   2,   0,
		 37,   0,   2,   0,  37,   0,
		  2,   0,  37,   0,   2,   0,
		 37,   0,   2,   0,  37,   0,
		  2,   0,  37,   0, 246,   0,
		  0,   0,   4,   0,   0,   0,
		  0,   0,   0,   0,   4,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  11, 202,
		 49,   1,  56,   0,   0,   0,
		  0,  16,   0,   0,   5,  16,
		  0,   0,  92,   0,   0,   0,
		 10,   0, 255, 255,   4,   0,
		  0,   0, 255, 255,   3,   0,
		  0,   0,   0,   0,  20,   0,
		  0,   0,  20,   0,   0,   0,
		  8,   0,   0,   0,  28,   0,
		  0,   0,   0,   0,   0,   0,
		 22,   0,  27,  21,  64,   0,
		  0,   0,   2,   0,   0,   0,
		  8,   0, 102, 108, 111,  97,
		116,  50,   0, 243, 242, 241,
		 14,   0,   1,  18,   2,   0,
		  0,   0,   0,  16,   0,   0,
		  0,  16,   0,   0,  22,   0,
		 27,  21,  64,   0,   0,   0,
		  4,   0,   0,   0,  16,   0,
		102, 108, 111,  97, 116,  52,
		  0, 243, 242, 241,  10,   0,
		 24,  21,   2,  16,   0,   0,
		  1,   0,   1,   0,  14,   0,
		  8,  16,   3,  16,   0,   0,
		 23,   0,   2,   0,   1,  16,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 11, 202,  49,   1,  56,   0,
		  0,   0,   0,  16,   0,   0,
		  0,  16,   0,   0,   0,   0,
		  0,   0,  11,   0, 255, 255,
		  4,   0,   0,   0, 255, 255,
		  3,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0, 255, 255, 255, 255,
		 26,   9,  47, 241,   8,   0,
		  0,   0,   8,   2,   0,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  32,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 18,   0,  37,  17,   0,   0,
		  0,   0, 128,   0,   0,   0,
		  1,   0, 109,  97, 105, 110,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 255, 255,
		255, 255,  26,   9,  47, 241,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  16,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		255, 255, 255, 255,  26,   9,
		 47, 241,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 255, 255,
		255, 255, 119,   9,  49,   1,
		  1,   0,   0,   0,  13,   0,
		  0, 142,  14,   0,  63,  92,
		 15,   0,   0,   0,  76,   0,
		  0,   0,  32,   0,   0,   0,
		 44,   0,   0,   0,  84,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		  0,   0,  25,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   0,   0,
		  0,   0,  44,   1,   0,   0,
		 32,   0,   0,  96,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   2,   0,
		  9,   0,   4,   2,   0,   0,
		  0,   0,   0,   0,  84,   1,
		  0,   0,   1,   0,   0,   0,
		  8, 212,  87,   3,   0,   0,
		  0,   0,   0,   0,   0,   0,
		109,  97, 105, 110,   0, 110,
		111, 110, 101,   0,   0,   0,
		 45, 186,  46, 241,   1,   0,
		  0,   0,   0,   0,   0,   0,
		 44,   1,   0,   0,  32,   0,
		  0,  96,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   2,   0,   2,   0,
		  7,   0,   0,   0,   0,   0,
		  1,   0, 255, 255, 255, 255,
		  0,   0,   0,   0,  44,   1,
		  0,   0,   8,   2,   0,   0,
		  0,   0,   0,   0, 255, 255,
		255, 255,   0,   0,   0,   0,
		255, 255, 255, 255,   1,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,  67,  58,
		 92,  85, 115, 101, 114, 115,
		 92, 108, 108, 101, 111, 110,
		 97, 114, 116,  92,  68, 101,
		115, 107, 116, 111, 112,  92,
		 80, 104,  68,  32,  80, 114,
		111, 106, 101,  99, 116,  92,
		 67,  65,  52,  71,  92,  83,
		104,  97, 100, 101, 114, 115,
		 92,  68, 114,  97, 119,  83,
		 99, 114, 101, 101, 110,  95,
		 86,  83,  46, 104, 108, 115,
		108,   0,   0,   0, 254, 239,
		254, 239,   1,   0,   0,   0,
		  1,   0,   0,   0,   0,   1,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255,  12,   0, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		148,  46,  49,   1, 251, 103,
		166,  92,   1,   0,   0,   0,
		 89,  73, 115,  61, 234,  36,
		100,  75, 169,  34, 138, 198,
		225, 156,  23, 109, 115,   0,
		  0,   0,  47,  76, 105, 110,
		107,  73, 110, 102, 111,   0,
		 47, 110,  97, 109, 101, 115,
		  0,  47, 115, 114,  99,  47,
		104, 101,  97, 100, 101, 114,
		 98, 108, 111,  99, 107,   0,
		 47, 115, 114,  99,  47, 102,
		105, 108, 101, 115,  47,  99,
		 58,  92, 117, 115, 101, 114,
		115,  92, 108, 108, 101, 111,
		110,  97, 114, 116,  92, 100,
		101, 115, 107, 116, 111, 112,
		 92, 112, 104, 100,  32, 112,
		114, 111, 106, 101,  99, 116,
		 92,  99,  97,  52, 103,  92,
		115, 104,  97, 100, 101, 114,
		115,  92, 100, 114,  97, 119,
		115,  99, 114, 101, 101, 110,
		 95, 118, 115,  46, 104, 108,
		115, 108,   0,   4,   0,   0,
		  0,   6,   0,   0,   0,   1,
		  0,   0,   0,  58,   0,   0,
		  0,   0,   0,   0,   0,  17,
		  0,   0,   0,   7,   0,   0,
		  0,  10,   0,   0,   0,   6,
		  0,   0,   0,   0,   0,   0,
		  0,   5,   0,   0,   0,  34,
		  0,   0,   0,   8,   0,   0,
		  0,   0,   0,   0,   0, 220,
		 81,  51,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  16,   0,   0,   0,
		 32,   0,   0,   0, 207,   0,
		  0,   0, 148,   0,   0,   0,
		 91,   1,   0,   0,  56,   0,
		  0,   0,   0,   0,   0,   0,
		101,   1,   0,   0, 128,   0,
		  0,   0, 166,   0,   0,   0,
		 96,   3,   0,   0,  28,   0,
		  0,   0,   0,   0,   0,   0,
		 40,   0,   0,   0,  32,   2,
		  0,   0,  44,   0,   0,   0,
		 20,   0,   0,   0,   3,   0,
		  0,   0,  20,   0,   0,   0,
		 13,   0,   0,   0,  19,   0,
		  0,   0,  14,   0,   0,   0,
		  9,   0,   0,   0,  10,   0,
		  0,   0,   8,   0,   0,   0,
		 11,   0,   0,   0,  12,   0,
		  0,   0,   7,   0,   0,   0,
		  6,   0,   0,   0,  15,   0,
		  0,   0,  16,   0,   0,   0,
		 18,   0,   0,   0,  17,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  21,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0
	};
#if 0
	//
	// Generated by Microsoft (R) HLSL Shader Compiler 10.1
	//
	//
	// Resource Bindings:
	//
	// Name                                 Type  Format         Dim      HLSL Bind  Count
	// ------------------------------ ---------- ------- ----------- -------------- ------
	// tex                               texture    sint          2d             t0      1 
	//
	//
	//
	// Input signature:
	//
	// Name                 Index   Mask Register SysValue  Format   Used
	// -------------------- ----- ------ -------- -------- ------- ------
	// SV_POSITION              0   xyzw        0      POS   float       
	// TEXCOORD                 0   xy          1     NONE   float   xy  
	//
	//
	// Output signature:
	//
	// Name                 Index   Mask Register SysValue  Format   Used
	// -------------------- ----- ------ -------- -------- ------- ------
	// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
	//
	ps_5_0
		dcl_globalFlags refactoringAllowed | skipOptimization
		dcl_resource_texture2d(sint, sint, sint, sint) t0
		dcl_input_ps linear v1.xy
		dcl_output o0.xyzw
		dcl_temps 12
		dcl_indexableTemp x0[11], 4
		//
		// Initial variable locations:
		//   v0.x <- proj.x; v0.y <- proj.y; v0.z <- proj.z; v0.w <- proj.w; 
		//   v1.x <- C.x; v1.y <- C.y; 
		//   o0.x <- <main return value>.x; o0.y <- <main return value>.y; o0.z <- <main return value>.z; o0.w <- <main return value>.w
		//
#line 34 "C:\Users\lleonart\Desktop\PhD Project\CA4G\Shaders\DrawComplexity_PS.hlsl"
		resinfo_indexable(texture2d)(sint, sint, sint, sint) r0.xy, l(0), t0.xyzw
		ftoi r0.x, r0.x  // r0.x <- width
		ftoi r0.y, r0.y  // r0.y <- height

#line 35
		nop
		itof r0.x, r0.x
		mul r0.x, r0.x, v1.x
		itof r0.y, r0.y
		mul r0.y, r0.y, v1.y
		ftou r1.x, r0.x
		ftou r1.y, r0.y
		mov r1.zw, l(0, 0, 0, 0)
		ld_indexable(texture2d)(sint, sint, sint, sint) r0.x, r1.xyzw, t0.xyzw
		mov r0.x, r0.x

#line 5
		ieq r0.y, r0.x, l(0)
		if_z r0.x

#line 6
		itof r1.xyz, l(1, 1, 1, 0)
		endif   // r1.x <- <GetColor return value>.x; r1.y <- <GetColor return value>.y; r1.z <- <GetColor return value>.z

#line 10
		if_z r0.y
		itof r0.x, r0.x
		log r0.x, r0.x  // r0.x <- level

#line 11
		itof r2.xy, l(0, 0, 0, 0)
		itof r0.yzw, l(0, 0, 0, 1)
		itof r3.xz, l(0, 0, 1, 0)
		itof r4.xyz, l(0, 1, 1, 0)
		itof r5.xy, l(0, 1, 0, 0)
		itof r6.xyz, l(0, 1, 0, 0)
		itof r7.yz, l(0, 1, 0, 0)
		itof r8.xyz, l(1, 1, 0, 0)
		itof r9.xz, l(1, 0, 0, 0)
		itof r10.xyz, l(1, 0, 0, 0)
		itof r11.xyz, l(1, 0, 1, 0)
		mov r2.z, l(0.500000)
		mov x0[0].xyz, r2.xyzx
		mov x0[1].xyz, r0.yzwy
		mov r3.y, l(0.500000)
		mov x0[2].xyz, r3.xyzx
		mov x0[3].xyz, r4.xyzx
		mov r5.z, l(0.500000)
		mov x0[4].xyz, r5.xyzx
		mov x0[5].xyz, r6.xyzx
		mov r7.x, l(0.500000)
		mov x0[6].xyz, r7.xyzx
		mov x0[7].xyz, r8.xyzx
		mov r9.y, l(0.500000)
		mov x0[8].xyz, r9.xyzx
		mov x0[9].xyz, r10.xyzx
		mov x0[10].xyz, r11.xyzx

#line 25
		itof r0.y, l(10)
		ge r0.y, r0.x, r0.y
		if_nz r0.y

#line 26
		mov r1.xyz, x0[10].xyzx
		endif

#line 28
		if_z r0.y
		ftoi r0.y, r0.x
		mov r0.yzw, x0[r0.y + 0].xxyz
		ftoi r1.w, r0.x
		iadd r1.w, r1.w, l(1)
		mov r2.xyz, x0[r1.w + 0].xyzx
		itof r1.w, l(1)
		mul r2.w, r0.x, r1.w
		mov r3.x, -r2.w
		ge r2.w, r2.w, r3.x
		mov r3.x, -r1.w
		movc r1.w, r2.w, r1.w, r3.x
		div r2.w, l(1.000000, 1.000000, 1.000000, 1.000000), r1.w
		mul r0.x, r0.x, r2.w
		frc r0.x, r0.x
		mul r0.x, r0.x, r1.w
		mov r3.xyz, -r0.yzwy
		add r2.xyz, r2.xyzx, r3.xyzx
		mul r2.xyz, r0.xxxx, r2.xyzx
		add r1.xyz, r0.yzwy, r2.xyzx
		endif
		endif

#line 35
		itof o0.w, l(1)
		mov o0.xyz, r1.xyzx
		ret
		// Approximately 77 instruction slots used
#endif

		const BYTE cso_DrawComplexity_PS[] =
	{
		 68,  88,  66,  67, 179, 179,
		 58, 164, 184,  29, 196,  29,
		 46,   4, 170,  29, 129, 179,
		  9, 167,   1,   0,   0,   0,
		 92,  79,   0,   0,   6,   0,
		  0,   0,  56,   0,   0,   0,
		200,   0,   0,   0,  32,   1,
		  0,   0,  84,   1,   0,   0,
		184,   8,   0,   0,  84,   9,
		  0,   0,  82,  68,  69,  70,
		136,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,  60,   0,
		  0,   0,   0,   5, 255, 255,
		  5,   1,   0,   0,  96,   0,
		  0,   0,  82,  68,  49,  49,
		 60,   0,   0,   0,  24,   0,
		  0,   0,  32,   0,   0,   0,
		 40,   0,   0,   0,  36,   0,
		  0,   0,  12,   0,   0,   0,
		  0,   0,   0,   0,  92,   0,
		  0,   0,   2,   0,   0,   0,
		  3,   0,   0,   0,   4,   0,
		  0,   0, 255, 255, 255, 255,
		  0,   0,   0,   0,   1,   0,
		  0,   0,   1,   0,   0,   0,
		116, 101, 120,   0,  77, 105,
		 99, 114, 111, 115, 111, 102,
		116,  32,  40,  82,  41,  32,
		 72,  76,  83,  76,  32,  83,
		104,  97, 100, 101, 114,  32,
		 67, 111, 109, 112, 105, 108,
		101, 114,  32,  49,  48,  46,
		 49,   0,  73,  83,  71,  78,
		 80,   0,   0,   0,   2,   0,
		  0,   0,   8,   0,   0,   0,
		 56,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  3,   0,   0,   0,   0,   0,
		  0,   0,  15,   0,   0,   0,
		 68,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  3,   0,   0,   0,   1,   0,
		  0,   0,   3,   3,   0,   0,
		 83,  86,  95,  80,  79,  83,
		 73,  84,  73,  79,  78,   0,
		 84,  69,  88,  67,  79,  79,
		 82,  68,   0, 171, 171, 171,
		 79,  83,  71,  78,  44,   0,
		  0,   0,   1,   0,   0,   0,
		  8,   0,   0,   0,  32,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   3,   0,
		  0,   0,   0,   0,   0,   0,
		 15,   0,   0,   0,  83,  86,
		 95,  84,  65,  82,  71,  69,
		 84,   0, 171, 171,  83,  72,
		 69,  88,  92,   7,   0,   0,
		 80,   0,   0,   0, 215,   1,
		  0,   0, 106, 136,   0,   1,
		 88,  24,   0,   4,   0, 112,
		 16,   0,   0,   0,   0,   0,
		 51,  51,   0,   0,  98,  16,
		  0,   3,  50,  16,  16,   0,
		  1,   0,   0,   0, 101,   0,
		  0,   3, 242,  32,  16,   0,
		  0,   0,   0,   0, 104,   0,
		  0,   2,  12,   0,   0,   0,
		105,   0,   0,   4,   0,   0,
		  0,   0,  11,   0,   0,   0,
		  4,   0,   0,   0,  61,   0,
		  0, 137, 194,   0,   0, 128,
		195, 204,  12,   0,  50,   0,
		 16,   0,   0,   0,   0,   0,
		  1,  64,   0,   0,   0,   0,
		  0,   0,  70, 126,  16,   0,
		  0,   0,   0,   0,  27,   0,
		  0,   5,  18,   0,  16,   0,
		  0,   0,   0,   0,  10,   0,
		 16,   0,   0,   0,   0,   0,
		 27,   0,   0,   5,  34,   0,
		 16,   0,   0,   0,   0,   0,
		 26,   0,  16,   0,   0,   0,
		  0,   0,  58,   0,   0,   1,
		 43,   0,   0,   5,  18,   0,
		 16,   0,   0,   0,   0,   0,
		 10,   0,  16,   0,   0,   0,
		  0,   0,  56,   0,   0,   7,
		 18,   0,  16,   0,   0,   0,
		  0,   0,  10,   0,  16,   0,
		  0,   0,   0,   0,  10,  16,
		 16,   0,   1,   0,   0,   0,
		 43,   0,   0,   5,  34,   0,
		 16,   0,   0,   0,   0,   0,
		 26,   0,  16,   0,   0,   0,
		  0,   0,  56,   0,   0,   7,
		 34,   0,  16,   0,   0,   0,
		  0,   0,  26,   0,  16,   0,
		  0,   0,   0,   0,  26,  16,
		 16,   0,   1,   0,   0,   0,
		 28,   0,   0,   5,  18,   0,
		 16,   0,   1,   0,   0,   0,
		 10,   0,  16,   0,   0,   0,
		  0,   0,  28,   0,   0,   5,
		 34,   0,  16,   0,   1,   0,
		  0,   0,  26,   0,  16,   0,
		  0,   0,   0,   0,  54,   0,
		  0,   8, 194,   0,  16,   0,
		  1,   0,   0,   0,   2,  64,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 45,   0,   0, 137, 194,   0,
		  0, 128, 195, 204,  12,   0,
		 18,   0,  16,   0,   0,   0,
		  0,   0,  70,  14,  16,   0,
		  1,   0,   0,   0,  70, 126,
		 16,   0,   0,   0,   0,   0,
		 54,   0,   0,   5,  18,   0,
		 16,   0,   0,   0,   0,   0,
		 10,   0,  16,   0,   0,   0,
		  0,   0,  32,   0,   0,   7,
		 34,   0,  16,   0,   0,   0,
		  0,   0,  10,   0,  16,   0,
		  0,   0,   0,   0,   1,  64,
		  0,   0,   0,   0,   0,   0,
		 31,   0,   0,   3,  10,   0,
		 16,   0,   0,   0,   0,   0,
		 43,   0,   0,   8, 114,   0,
		 16,   0,   1,   0,   0,   0,
		  2,  64,   0,   0,   1,   0,
		  0,   0,   1,   0,   0,   0,
		  1,   0,   0,   0,   0,   0,
		  0,   0,  21,   0,   0,   1,
		 31,   0,   0,   3,  26,   0,
		 16,   0,   0,   0,   0,   0,
		 43,   0,   0,   5,  18,   0,
		 16,   0,   0,   0,   0,   0,
		 10,   0,  16,   0,   0,   0,
		  0,   0,  47,   0,   0,   5,
		 18,   0,  16,   0,   0,   0,
		  0,   0,  10,   0,  16,   0,
		  0,   0,   0,   0,  43,   0,
		  0,   8,  50,   0,  16,   0,
		  2,   0,   0,   0,   2,  64,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 43,   0,   0,   8, 226,   0,
		 16,   0,   0,   0,   0,   0,
		  2,  64,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   1,   0,
		  0,   0,  43,   0,   0,   8,
		 82,   0,  16,   0,   3,   0,
		  0,   0,   2,  64,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,  43,   0,
		  0,   8, 114,   0,  16,   0,
		  4,   0,   0,   0,   2,  64,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		 43,   0,   0,   8,  50,   0,
		 16,   0,   5,   0,   0,   0,
		  2,  64,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  43,   0,   0,   8,
		114,   0,  16,   0,   6,   0,
		  0,   0,   2,  64,   0,   0,
		  0,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  43,   0,
		  0,   8,  98,   0,  16,   0,
		  7,   0,   0,   0,   2,  64,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 43,   0,   0,   8, 114,   0,
		 16,   0,   8,   0,   0,   0,
		  2,  64,   0,   0,   1,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  43,   0,   0,   8,
		 82,   0,  16,   0,   9,   0,
		  0,   0,   2,  64,   0,   0,
		  1,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  43,   0,
		  0,   8, 114,   0,  16,   0,
		 10,   0,   0,   0,   2,  64,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 43,   0,   0,   8, 114,   0,
		 16,   0,  11,   0,   0,   0,
		  2,  64,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   0,   0,
		  0,   0,  54,   0,   0,   5,
		 66,   0,  16,   0,   2,   0,
		  0,   0,   1,  64,   0,   0,
		  0,   0,   0,  63,  54,   0,
		  0,   6, 114,  48,  32,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  70,   2,  16,   0,
		  2,   0,   0,   0,  54,   0,
		  0,   6, 114,  48,  32,   0,
		  0,   0,   0,   0,   1,   0,
		  0,   0, 150,   7,  16,   0,
		  0,   0,   0,   0,  54,   0,
		  0,   5,  34,   0,  16,   0,
		  3,   0,   0,   0,   1,  64,
		  0,   0,   0,   0,   0,  63,
		 54,   0,   0,   6, 114,  48,
		 32,   0,   0,   0,   0,   0,
		  2,   0,   0,   0,  70,   2,
		 16,   0,   3,   0,   0,   0,
		 54,   0,   0,   6, 114,  48,
		 32,   0,   0,   0,   0,   0,
		  3,   0,   0,   0,  70,   2,
		 16,   0,   4,   0,   0,   0,
		 54,   0,   0,   5,  66,   0,
		 16,   0,   5,   0,   0,   0,
		  1,  64,   0,   0,   0,   0,
		  0,  63,  54,   0,   0,   6,
		114,  48,  32,   0,   0,   0,
		  0,   0,   4,   0,   0,   0,
		 70,   2,  16,   0,   5,   0,
		  0,   0,  54,   0,   0,   6,
		114,  48,  32,   0,   0,   0,
		  0,   0,   5,   0,   0,   0,
		 70,   2,  16,   0,   6,   0,
		  0,   0,  54,   0,   0,   5,
		 18,   0,  16,   0,   7,   0,
		  0,   0,   1,  64,   0,   0,
		  0,   0,   0,  63,  54,   0,
		  0,   6, 114,  48,  32,   0,
		  0,   0,   0,   0,   6,   0,
		  0,   0,  70,   2,  16,   0,
		  7,   0,   0,   0,  54,   0,
		  0,   6, 114,  48,  32,   0,
		  0,   0,   0,   0,   7,   0,
		  0,   0,  70,   2,  16,   0,
		  8,   0,   0,   0,  54,   0,
		  0,   5,  34,   0,  16,   0,
		  9,   0,   0,   0,   1,  64,
		  0,   0,   0,   0,   0,  63,
		 54,   0,   0,   6, 114,  48,
		 32,   0,   0,   0,   0,   0,
		  8,   0,   0,   0,  70,   2,
		 16,   0,   9,   0,   0,   0,
		 54,   0,   0,   6, 114,  48,
		 32,   0,   0,   0,   0,   0,
		  9,   0,   0,   0,  70,   2,
		 16,   0,  10,   0,   0,   0,
		 54,   0,   0,   6, 114,  48,
		 32,   0,   0,   0,   0,   0,
		 10,   0,   0,   0,  70,   2,
		 16,   0,  11,   0,   0,   0,
		 43,   0,   0,   5,  34,   0,
		 16,   0,   0,   0,   0,   0,
		  1,  64,   0,   0,  10,   0,
		  0,   0,  29,   0,   0,   7,
		 34,   0,  16,   0,   0,   0,
		  0,   0,  10,   0,  16,   0,
		  0,   0,   0,   0,  26,   0,
		 16,   0,   0,   0,   0,   0,
		 31,   0,   4,   3,  26,   0,
		 16,   0,   0,   0,   0,   0,
		 54,   0,   0,   6, 114,   0,
		 16,   0,   1,   0,   0,   0,
		 70,  50,  32,   0,   0,   0,
		  0,   0,  10,   0,   0,   0,
		 21,   0,   0,   1,  31,   0,
		  0,   3,  26,   0,  16,   0,
		  0,   0,   0,   0,  27,   0,
		  0,   5,  34,   0,  16,   0,
		  0,   0,   0,   0,  10,   0,
		 16,   0,   0,   0,   0,   0,
		 54,   0,   0,   7, 226,   0,
		 16,   0,   0,   0,   0,   0,
		  6,  57,  32,   4,   0,   0,
		  0,   0,  26,   0,  16,   0,
		  0,   0,   0,   0,  27,   0,
		  0,   5, 130,   0,  16,   0,
		  1,   0,   0,   0,  10,   0,
		 16,   0,   0,   0,   0,   0,
		 30,   0,   0,   7, 130,   0,
		 16,   0,   1,   0,   0,   0,
		 58,   0,  16,   0,   1,   0,
		  0,   0,   1,  64,   0,   0,
		  1,   0,   0,   0,  54,   0,
		  0,   7, 114,   0,  16,   0,
		  2,   0,   0,   0,  70,  50,
		 32,   4,   0,   0,   0,   0,
		 58,   0,  16,   0,   1,   0,
		  0,   0,  43,   0,   0,   5,
		130,   0,  16,   0,   1,   0,
		  0,   0,   1,  64,   0,   0,
		  1,   0,   0,   0,  56,   0,
		  0,   7, 130,   0,  16,   0,
		  2,   0,   0,   0,  10,   0,
		 16,   0,   0,   0,   0,   0,
		 58,   0,  16,   0,   1,   0,
		  0,   0,  54,   0,   0,   6,
		 18,   0,  16,   0,   3,   0,
		  0,   0,  58,   0,  16, 128,
		 65,   0,   0,   0,   2,   0,
		  0,   0,  29,   0,   0,   7,
		130,   0,  16,   0,   2,   0,
		  0,   0,  58,   0,  16,   0,
		  2,   0,   0,   0,  10,   0,
		 16,   0,   3,   0,   0,   0,
		 54,   0,   0,   6,  18,   0,
		 16,   0,   3,   0,   0,   0,
		 58,   0,  16, 128,  65,   0,
		  0,   0,   1,   0,   0,   0,
		 55,   0,   0,   9, 130,   0,
		 16,   0,   1,   0,   0,   0,
		 58,   0,  16,   0,   2,   0,
		  0,   0,  58,   0,  16,   0,
		  1,   0,   0,   0,  10,   0,
		 16,   0,   3,   0,   0,   0,
		 14,   0,   0,  10, 130,   0,
		 16,   0,   2,   0,   0,   0,
		  2,  64,   0,   0,   0,   0,
		128,  63,   0,   0, 128,  63,
		  0,   0, 128,  63,   0,   0,
		128,  63,  58,   0,  16,   0,
		  1,   0,   0,   0,  56,   0,
		  0,   7,  18,   0,  16,   0,
		  0,   0,   0,   0,  10,   0,
		 16,   0,   0,   0,   0,   0,
		 58,   0,  16,   0,   2,   0,
		  0,   0,  26,   0,   0,   5,
		 18,   0,  16,   0,   0,   0,
		  0,   0,  10,   0,  16,   0,
		  0,   0,   0,   0,  56,   0,
		  0,   7,  18,   0,  16,   0,
		  0,   0,   0,   0,  10,   0,
		 16,   0,   0,   0,   0,   0,
		 58,   0,  16,   0,   1,   0,
		  0,   0,  54,   0,   0,   6,
		114,   0,  16,   0,   3,   0,
		  0,   0, 150,   7,  16, 128,
		 65,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   7,
		114,   0,  16,   0,   2,   0,
		  0,   0,  70,   2,  16,   0,
		  2,   0,   0,   0,  70,   2,
		 16,   0,   3,   0,   0,   0,
		 56,   0,   0,   7, 114,   0,
		 16,   0,   2,   0,   0,   0,
		  6,   0,  16,   0,   0,   0,
		  0,   0,  70,   2,  16,   0,
		  2,   0,   0,   0,   0,   0,
		  0,   7, 114,   0,  16,   0,
		  1,   0,   0,   0, 150,   7,
		 16,   0,   0,   0,   0,   0,
		 70,   2,  16,   0,   2,   0,
		  0,   0,  21,   0,   0,   1,
		 21,   0,   0,   1,  43,   0,
		  0,   5, 130,  32,  16,   0,
		  0,   0,   0,   0,   1,  64,
		  0,   0,   1,   0,   0,   0,
		 54,   0,   0,   5, 114,  32,
		 16,   0,   0,   0,   0,   0,
		 70,   2,  16,   0,   1,   0,
		  0,   0,  62,   0,   0,   1,
		 83,  84,  65,  84, 148,   0,
		  0,   0,  77,   0,   0,   0,
		 12,   0,   0,   0,   0,   0,
		  0,   0,   2,   0,   0,   0,
		 16,   0,   0,   0,   2,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   4,   0,
		  0,   0,   0,   0,   0,   0,
		 11,   0,   0,   0,  14,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  8,   0,   0,   0,   1,   0,
		  0,   0,  25,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 83,  80,  68,  66,   0,  70,
		  0,   0,  77, 105,  99, 114,
		111, 115, 111, 102, 116,  32,
		 67,  47,  67,  43,  43,  32,
		 77,  83,  70,  32,  55,  46,
		 48,  48,  13,  10,  26,  68,
		 83,   0,   0,   0,   0,   2,
		  0,   0,   2,   0,   0,   0,
		 35,   0,   0,   0, 168,   0,
		  0,   0,   0,   0,   0,   0,
		 31,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 192, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		 56,   0,   0,   0, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255,   5,   0,   0,   0,
		 32,   0,   0,   0,  60,   0,
		  0,   0,   0,   0,   0,   0,
		255, 255, 255, 255,   0,   0,
		  0,   0,   6,   0,   0,   0,
		  5,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   3,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		148,  46,  49,   1, 250, 103,
		166,  92,   1,   0,   0,   0,
		187,  24, 125, 247, 150, 195,
		117,  70, 168, 206,  99, 144,
		125,  61,  40, 142,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0, 220,  81,  51,   1,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  11, 202,  49,   1,
		 56,   0,   0,   0,   0,  16,
		  0,   0,   1,  16,   0,   0,
		 24,   0,   0,   0,  11,   0,
		255, 255,   4,   0,   0,   0,
		255, 255,   3,   0,   0,   0,
		  0,   0,   4,   0,   0,   0,
		  4,   0,   0,   0,   8,   0,
		  0,   0,  12,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		  1,  22,   0,   0,   0,   0,
		  8,  16,   0,   0,  71, 101,
		116,  67, 111, 108, 111, 114,
		  0, 243, 242, 241,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 198,  90,
		  0,   0, 117, 131,   1,   0,
		178, 211,   1,   0,  65,  36,
		  1,   0, 137, 231,   1,   0,
		202,  10,   1,   0,  76, 232,
		  3,   0,  49, 251,   3,   0,
		176,  22,   1,   0, 240, 220,
		  1,   0, 104, 174,   1,   0,
		 73,  20,   1,   0, 153, 189,
		  3,   0,   0,  16,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 84, 101, 120, 116, 117, 114,
		101,  50,  68,  60, 105, 110,
		116,  62,  32, 116, 101, 120,
		 32,  58,  32, 114, 101, 103,
		105, 115, 116, 101, 114,  32,
		 40, 116,  48,  41,  59,  13,
		 10,  13,  10, 102, 108, 111,
		 97, 116,  51,  32,  71, 101,
		116,  67, 111, 108, 111, 114,
		 40, 105, 110, 116,  32,  99,
		111, 109, 112, 108, 101, 120,
		105, 116, 121,  41,  32, 123,
		 13,  10,  13,  10,   9, 105,
		102,  32,  40,  99, 111, 109,
		112, 108, 101, 120, 105, 116,
		121,  32,  61,  61,  32,  48,
		 41,  13,  10,   9,   9, 114,
		101, 116, 117, 114, 110,  32,
		102, 108, 111,  97, 116,  51,
		 40,  49,  44,  32,  49,  44,
		 32,  49,  41,  59,  13,  10,
		 13,  10,   9,  47,  47, 114,
		101, 116, 117, 114, 110,  32,
		102, 108, 111,  97, 116,  51,
		 40,  49,  44,  49,  44,  49,
		 41,  59,  13,  10,  13,  10,
		  9, 102, 108, 111,  97, 116,
		 32, 108, 101, 118, 101, 108,
		 32,  61,  32, 108, 111, 103,
		 50,  40,  99, 111, 109, 112,
		108, 101, 120, 105, 116, 121,
		 41,  59,  13,  10,   9, 102,
		108, 111,  97, 116,  51,  32,
		115, 116, 111, 112,  80, 111,
		105, 110, 116, 115,  91,  49,
		 49,  93,  32,  61,  32, 123,
		 13,  10,   9,   9, 102, 108,
		111,  97, 116,  51,  40,  48,
		 44,  48,  44,  48,  46,  53,
		 41,  44,  32,  47,  47,  32,
		 49,  13,  10,   9,   9, 102,
		108, 111,  97, 116,  51,  40,
		 48,  44,  48,  44,  49,  41,
		 44,  32,  47,  47,  32,  50,
		 13,  10,   9,   9, 102, 108,
		111,  97, 116,  51,  40,  48,
		 44,  48,  46,  53,  44,  49,
		 41,  44,  32,  47,  47,  32,
		 52,  13,  10,   9,   9, 102,
		108, 111,  97, 116,  51,  40,
		 48,  44,  49,  44,  49,  41,
		 44,  32,  47,  47,  32,  56,
		 13,  10,   9,   9, 102, 108,
		111,  97, 116,  51,  40,  48,
		 44,  49,  44,  48,  46,  53,
		 41,  44,  32,  47,  47,  32,
		 49,  54,  13,  10,   9,   9,
		102, 108, 111,  97, 116,  51,
		 40,  48,  44,  49,  44,  48,
		 41,  44,  32,  47,  47,  32,
		 51,  50,  13,  10,   9,   9,
		102, 108, 111,  97, 116,  51,
		 40,  48,  46,  53,  44,  49,
		 44,  48,  41,  44,  32,  47,
		 47,  32,  54,  52,  13,  10,
		  9,   9, 102, 108, 111,  97,
		116,  51,  40,  49,  44,  49,
		 44,  48,  41,  44,  32,  47,
		 47,  32,  49,  50,  56,  13,
		 10,   9,   9, 102, 108, 111,
		 97, 116,  51,  40,  49,  44,
		 48,  46,  53,  44,  48,  41,
		 44,  32,  47,  47,  32,  50,
		 53,  54,  13,  10,   9,   9,
		102, 108, 111,  97, 116,  51,
		 40,  49,  44,  48,  44,  48,
		 41,  44,  32,  47,  47,  32,
		 53,  49,  50,  13,  10,   9,
		  9, 102, 108, 111,  97, 116,
		 51,  40,  49,  44,  48,  44,
		 49,  41,  32,  47,  47,  32,
		 49,  48,  50,  52,  13,  10,
		  9, 125,  59,  13,  10,  13,
		 10,   9, 105, 102,  32,  40,
		108, 101, 118, 101, 108,  32,
		 62,  61,  32,  49,  48,  41,
		 13,  10,   9,   9, 114, 101,
		116, 117, 114, 110,  32, 115,
		116, 111, 112,  80, 111, 105,
		110, 116, 115,  91,  49,  48,
		 93,  59,  13,  10,  13,  10,
		  9, 114, 101, 116, 117, 114,
		110,  32, 108, 101, 114, 112,
		 40, 115, 116, 111, 112,  80,
		111, 105, 110, 116, 115,  91,
		 40, 105, 110, 116,  41, 108,
		101, 118, 101, 108,  93,  44,
		 32, 115, 116, 111, 112,  80,
		111, 105, 110, 116, 115,  91,
		 40, 105, 110, 116,  41, 108,
		101, 118, 101, 108,  32,  43,
		 32,  49,  93,  44,  32, 108,
		101, 118, 101, 108,  32,  37,
		 32,  49,  41,  59,  13,  10,
		125,  13,  10,  13,  10, 102,
		108, 111,  97, 116,  52,  32,
		109,  97, 105, 110,  40, 102,
		108, 111,  97, 116,  52,  32,
		112, 114, 111, 106,  32,  58,
		 32,  83,  86,  95,  80,  79,
		 83,  73,  84,  73,  79,  78,
		 44,  32, 102, 108, 111,  97,
		116,  50,  32,  67,  32,  58,
		 32,  84,  69,  88,  67,  79,
		 79,  82,  68,  41,  32,  58,
		 32,  83,  86,  95,  84,  65,
		 82,  71,  69,  84,  13,  10,
		123,  13,  10,   9, 105, 110,
		116,  32, 119, 105, 100, 116,
		104,  44,  32, 104, 101, 105,
		103, 104, 116,  59,  13,  10,
		  9, 116, 101, 120,  46,  71,
		101, 116,  68, 105, 109, 101,
		110, 115, 105, 111, 110, 115,
		 40, 119, 105, 100, 116, 104,
		 44,  32, 104, 101, 105, 103,
		104, 116,  41,  59,  13,  10,
		  9, 114, 101, 116, 117, 114,
		110,  32, 102, 108, 111,  97,
		116,  52,  40,  71, 101, 116,
		 67, 111, 108, 111, 114,  40,
		116, 101, 120,  91, 117, 105,
		110, 116,  50,  40, 119, 105,
		100, 116, 104,  42,  67,  46,
		120,  44,  32, 104, 101, 105,
		103, 104, 116,  42,  67,  46,
		121,  41,  93,  41,  44,  49,
		 41,  59,  13,  10, 125,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 254, 239,
		254, 239,   1,   0,   0,   0,
		210,   3,   0,   0,   0,  67,
		 58,  92,  85, 115, 101, 114,
		115,  92, 108, 108, 101, 111,
		110,  97, 114, 116,  92,  68,
		101, 115, 107, 116, 111, 112,
		 92,  80, 104,  68,  32,  80,
		114, 111, 106, 101,  99, 116,
		 92,  67,  65,  52,  71,  92,
		 83, 104,  97, 100, 101, 114,
		115,  92,  68, 114,  97, 119,
		 67, 111, 109, 112, 108, 101,
		120, 105, 116, 121,  95,  80,
		 83,  46, 104, 108, 115, 108,
		  0,   0,  99,  58,  92, 117,
		115, 101, 114, 115,  92, 108,
		108, 101, 111, 110,  97, 114,
		116,  92, 100, 101, 115, 107,
		116, 111, 112,  92, 112, 104,
		100,  32, 112, 114, 111, 106,
		101,  99, 116,  92,  99,  97,
		 52, 103,  92, 115, 104,  97,
		100, 101, 114, 115,  92, 100,
		114,  97, 119,  99, 111, 109,
		112, 108, 101, 120, 105, 116,
		121,  95, 112, 115,  46, 104,
		108, 115, 108,   0,  84, 101,
		120, 116, 117, 114, 101,  50,
		 68,  60, 105, 110, 116,  62,
		 32, 116, 101, 120,  32,  58,
		 32, 114, 101, 103, 105, 115,
		116, 101, 114,  32,  40, 116,
		 48,  41,  59,  13,  10,  13,
		 10, 102, 108, 111,  97, 116,
		 51,  32,  71, 101, 116,  67,
		111, 108, 111, 114,  40, 105,
		110, 116,  32,  99, 111, 109,
		112, 108, 101, 120, 105, 116,
		121,  41,  32, 123,  13,  10,
		 13,  10,   9, 105, 102,  32,
		 40,  99, 111, 109, 112, 108,
		101, 120, 105, 116, 121,  32,
		 61,  61,  32,  48,  41,  13,
		 10,   9,   9, 114, 101, 116,
		117, 114, 110,  32, 102, 108,
		111,  97, 116,  51,  40,  49,
		 44,  32,  49,  44,  32,  49,
		 41,  59,  13,  10,  13,  10,
		  9,  47,  47, 114, 101, 116,
		117, 114, 110,  32, 102, 108,
		111,  97, 116,  51,  40,  49,
		 44,  49,  44,  49,  41,  59,
		 13,  10,  13,  10,   9, 102,
		108, 111,  97, 116,  32, 108,
		101, 118, 101, 108,  32,  61,
		 32, 108, 111, 103,  50,  40,
		 99, 111, 109, 112, 108, 101,
		120, 105, 116, 121,  41,  59,
		 13,  10,   9, 102, 108, 111,
		 97, 116,  51,  32, 115, 116,
		111, 112,  80, 111, 105, 110,
		116, 115,  91,  49,  49,  93,
		 32,  61,  32, 123,  13,  10,
		  9,   9, 102, 108, 111,  97,
		116,  51,  40,  48,  44,  48,
		 44,  48,  46,  53,  41,  44,
		 32,  47,  47,  32,  49,  13,
		 10,   9,   9, 102, 108, 111,
		 97, 116,  51,  40,  48,  44,
		 48,  44,  49,  41,  44,  32,
		 47,  47,  32,  50,  13,  10,
		  9,   9, 102, 108, 111,  97,
		116,  51,  40,  48,  44,  48,
		 46,  53,  44,  49,  41,  44,
		 32,  47,  47,  32,  52,  13,
		 10,   9,   9, 102, 108, 111,
		 97, 116,  51,  40,  48,  44,
		 49,  44,  49,  41,  44,  32,
		 47,  47,  32,  56,  13,  10,
		  9,   9, 102, 108, 111,  97,
		116,  51,  40,  48,  44,  49,
		 44,  48,  46,  53,  41,  44,
		 32,  47,  47,  32,  49,  54,
		 13,  10,   9,   9, 102, 108,
		111,  97, 116,  51,  40,  48,
		 27, 226,  48,   1, 128,   0,
		  0,   0, 185,  96,  92, 102,
		 36, 235, 212,   1,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   1,   0,
		  0,   0,   2,   0,   0,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		 76,   0,   0,   0,  40,   0,
		  0,   0,  27, 226,  48,   1,
		 82, 214, 153,   9,  59,   3,
		  0,   0,   1,   0,   0,   0,
		 75,   0,   0,   0,  76,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   4,   0,   0,   0,
		 66,   0,  60,  17,  16,   1,
		  0,   0,   0,   1,  10,   0,
		  1,   0, 132,   0,  99,  69,
		 10,   0,   1,   0, 132,   0,
		 99,  69,  77, 105,  99, 114,
		111, 115, 111, 102, 116,  32,
		 40,  82,  41,  32,  72,  76,
		 83,  76,  32,  83, 104,  97,
		100, 101, 114,  32,  67, 111,
		109, 112, 105, 108, 101, 114,
		 32,  49,  48,  46,  49,   0,
		  0,   0,  54,   0,  61,  17,
		  1, 104, 108, 115, 108,  70,
		108,  97, 103, 115,   0,  48,
		120,  53,   0, 104, 108, 115,
		108,  84,  97, 114, 103, 101,
		116,   0, 112, 115,  95,  53,
		 95,  48,   0, 104, 108, 115,
		108,  69, 110, 116, 114, 121,
		  0, 109,  97, 105, 110,   0,
		  0,   0,   0,   0,  42,   0,
		 16,  17,   0,   0,   0,   0,
		 80,   5,   0,   0,   0,   0,
		  0,   0,  16,   7,   0,   0,
		  0,   0,   0,   0,  16,   7,
		  0,   0,   4,  16,   0,   0,
		 76,   0,   0,   0,   1,   0,
		160, 109,  97, 105, 110,   0,
		 42,   0,  62,  17,   0,  16,
		  0,   0,   9,   0, 112, 114,
		111, 106,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  22,   0,  80,  17,
		  1,   0,   5,   0,   0,   0,
		  4,   0,  76,   0,   0,   0,
		  1,   0,  16,   7,   0,   0,
		  0,   0,  22,   0,  80,  17,
		  1,   0,   5,   0,   4,   0,
		  4,   0,  76,   0,   0,   0,
		  1,   0,  16,   7,   4,   0,
		  0,   0,  22,   0,  80,  17,
		  1,   0,   5,   0,   8,   0,
		  4,   0,  76,   0,   0,   0,
		  1,   0,  16,   7,   8,   0,
		  0,   0,  22,   0,  80,  17,
		  1,   0,   5,   0,  12,   0,
		  4,   0,  76,   0,   0,   0,
		  1,   0,  16,   7,  12,   0,
		  0,   0,  42,   0,  62,  17,
		  1,  16,   0,   0,   9,   0,
		 67,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   1,   0,   5,   0,
		  0,   0,   4,   0,  76,   0,
		  0,   0,   1,   0,  16,   7,
		 16,   0,   0,   0,  22,   0,
		 80,  17,   1,   0,   5,   0,
		  4,   0,   4,   0,  76,   0,
		  0,   0,   1,   0,  16,   7,
		 20,   0,   0,   0,  58,   0,
		 62,  17,   3,  16,   0,   0,
		136,   0,  60, 109,  97, 105,
		110,  32, 114, 101, 116, 117,
		114, 110,  32, 118,  97, 108,
		117, 101,  62,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   2,   0,   5,   0,
		  0,   0,   4,   0,  76,   0,
		  0,   0,   1,   0,  16,   7,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   2,   0,   5,   0,
		  4,   0,   4,   0,  76,   0,
		  0,   0,   1,   0,  16,   7,
		  4,   0,   0,   0,  22,   0,
		 80,  17,   2,   0,   5,   0,
		  8,   0,   4,   0,  76,   0,
		  0,   0,   1,   0,  16,   7,
		  8,   0,   0,   0,  22,   0,
		 80,  17,   2,   0,   5,   0,
		 12,   0,   4,   0,  76,   0,
		  0,   0,   1,   0,  16,   7,
		 12,   0,   0,   0,  46,   0,
		 62,  17, 116,   0,   0,   0,
		  0,   0, 119, 105, 100, 116,
		104,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   0,   0,   1,   0,
		  0,   0,   4,   0, 132,   0,
		  0,   0,   1,   0,  44,   0,
		  0,   0,   0,   0,  46,   0,
		 62,  17, 116,   0,   0,   0,
		  0,   0, 104, 101, 105, 103,
		104, 116,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   0,   0,   1,   0,
		  0,   0,   4,   0, 152,   0,
		  0,   0,   1,   0,  72,   0,
		  4,   0,   0,   0,  34,   1,
		 77,  17, 128,   0,   0,   0,
		 76,   5,   0,   0,   0,  16,
		  0,   0,   7,   0,   9,   2,
		 13,  21,   6,   4,   3, 129,
		 48,   9,   3,  13,  25,   6,
		  2,   3,  40,   9,  25,   3,
		 32,   9,   2,  13,  32,   6,
		  8,   3,   4,   7,  12,  13,
		  3,   6,   2,   3,  52,   7,
		  0,  13,  17,   6,  28,   3,
		130, 204,   9,   3,  13,  24,
		  6,   2,   3,  60,   9,  24,
		  3,  24,   9,   2,  13,  76,
		 11,  68,   9,  76,  12,   8,
		130,   8,   8,   0,   9,   6,
		 13,  20,   1, 129, 124,   6,
		 47,   3,   0,   9,   2,  13,
		 21,   3,  28,   9,  10,  13,
		 24,  11,  44,   9,  25,  13,
		 25,   3,  32,   9,   2,  13,
		 32,   6,   8,   3,   4,   9,
		 16,  13,  31,   3,  12,   9,
		  3,  13,  17,   6,   4,   3,
		 40,  13,  15,   6,   2,   3,
		 32,  13,  17,   6,   2,   3,
		 32,  13,  15,   6,   2,   3,
		 32,  13,  17,   6,   2,   3,
		 32,  13,  15,   6,   2,   3,
		 32,  13,  17,   6,   2,   3,
		 32,  13,  15,   6,   2,   3,
		 32,  13,  17,   6,   2,   3,
		 32,  13,  15,   6,   2,   3,
		 32,   6,   2,   3,  32,   7,
		 12,   9,   9,  13,   2,   6,
		 23,   3,  32,   7,   0,   9,
		  6,  13,  16,   6,  28,   3,
		129, 108,   9,   2,  13,  17,
		  3,  48,   9,  10,  13,  23,
		 11,  44,   9,  24,  13,  24,
		  3,  24,   9,   2,  13,  76,
		 11,  68,   9,  25,  13,  34,
		  3,  12,   9,  14,  13,  35,
		  3,  20,   9,  49,  13,  58,
		  3,  28,  13,  62,   3,  20,
		  9,  38,  13,  63,   3,  28,
		  9,  66,  13,  74,   3,  28,
		  9,   9,  13,  75,   3, 129,
		 20,   9,  76,  13,  76,  12,
		  8, 108,  62,   0,  62,  17,
		  7,  16,   0,   0, 136,   0,
		 60,  71, 101, 116,  67, 111,
		108, 111, 114,  32, 114, 101,
		116, 117, 114, 110,  32, 118,
		 97, 108, 117, 101,  62,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 22,   0,  80,  17,   0,   0,
		  5,   0,   0,   0,   4,   0,
		200,   1,   0,   0,   1,   0,
		124,   5,  16,   0,   0,   0,
		 22,   0,  80,  17,   0,   0,
		  5,   0,   4,   0,   4,   0,
		200,   1,   0,   0,   1,   0,
		124,   5,  20,   0,   0,   0,
		 22,   0,  80,  17,   0,   0,
		  5,   0,   8,   0,   4,   0,
		200,   1,   0,   0,   1,   0,
		124,   5,  24,   0,   0,   0,
		 50,   0,  62,  17, 116,   0,
		  0,   0,   1,   0,  99, 111,
		109, 112, 108, 101, 120, 105,
		116, 121,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   0,   0,   1,   0,
		  0,   0,   4,   0, 124,   1,
		  0,   0,   1,   0, 128,   0,
		  0,   0,   0,   0,  46,   0,
		 62,  17,  64,   0,   0,   0,
		  0,   0, 108, 101, 118, 101,
		108,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   0,   0,   1,   0,
		  0,   0,   4,   0, 252,   1,
		  0,   0,   1,   0, 144,   4,
		  0,   0,   0,   0,  50,   0,
		 62,  17,   9,  16,   0,   0,
		  8,   0, 115, 116, 111, 112,
		 80, 111, 105, 110, 116, 115,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  22,   0,  80,  17,
		  3,   0,   1,   0,   0,   0,
		172,   0, 124,   1,   0,   0,
		  1,   0, 200,   5,   0,   0,
		  0,   0,   2,   0,  78,  17,
		  2,   0,   6,   0, 244,   0,
		  0,   0,  24,   0,   0,   0,
		  1,   0,   0,   0,  16,   1,
		207,  67,   4, 234, 230,  80,
		194, 105, 158,  89, 122,  58,
		209,  90,  26,  46,   0,   0,
		242,   0,   0,   0,  80,   7,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   1,   0,  92,   7,
		  0,   0,   0,   0,   0,   0,
		154,   0,   0,   0,  68,   7,
		  0,   0,  76,   0,   0,   0,
		 34,   0,   0, 128,  76,   0,
		  0,   0,  34,   0,   0,   0,
		112,   0,   0,   0,  34,   0,
		  0, 128, 112,   0,   0,   0,
		 34,   0,   0,   0, 132,   0,
		  0,   0,  34,   0,   0, 128,
		132,   0,   0,   0,  34,   0,
		  0,   0, 152,   0,   0,   0,
		 35,   0,   0, 128, 152,   0,
		  0,   0,  35,   0,   0,   0,
		156,   0,   0,   0,  35,   0,
		  0, 128, 156,   0,   0,   0,
		 35,   0,   0,   0, 176,   0,
		  0,   0,  35,   0,   0, 128,
		176,   0,   0,   0,  35,   0,
		  0,   0, 204,   0,   0,   0,
		 35,   0,   0, 128, 204,   0,
		  0,   0,  35,   0,   0,   0,
		224,   0,   0,   0,  35,   0,
		  0, 128, 224,   0,   0,   0,
		 35,   0,   0,   0, 252,   0,
		  0,   0,  35,   0,   0, 128,
		252,   0,   0,   0,  35,   0,
		  0,   0,  16,   1,   0,   0,
		 35,   0,   0, 128,  16,   1,
		  0,   0,  35,   0,   0,   0,
		 36,   1,   0,   0,  35,   0,
		  0, 128,  36,   1,   0,   0,
		 35,   0,   0,   0,  68,   1,
		  0,   0,  35,   0,   0, 128,
		 68,   1,   0,   0,  35,   0,
		  0,   0, 104,   1,   0,   0,
		 35,   0,   0, 128, 104,   1,
		  0,   0,  35,   0,   0,   0,
		124,   1,   0,   0,  35,   0,
		  0, 128, 124,   1,   0,   0,
		 35,   0,   0,   0, 152,   1,
		  0,   0,  35,   0,   0, 128,
		152,   1,   0,   0,  35,   0,
		  0,   0, 164,   1,   0,   0,
		 35,   0,   0, 128, 164,   1,
		  0,   0,  35,   0,   0,   0,
		196,   1,   0,   0,  35,   0,
		  0, 128, 196,   1,   0,   0,
		 35,   0,   0,   0, 200,   1,
		  0,   0,  35,   0,   0, 128,
		200,   1,   0,   0,  35,   0,
		  0,   0, 212,   1,   0,   0,
		 35,   0,   0, 128, 212,   1,
		  0,   0,  35,   0,   0,   0,
		232,   1,   0,   0,  35,   0,
		  0, 128, 232,   1,   0,   0,
		 35,   0,   0,   0, 252,   1,
		  0,   0,  35,   0,   0, 128,
		252,   1,   0,   0,  35,   0,
		  0,   0,  28,   2,   0,   0,
		 35,   0,   0, 128,  28,   2,
		  0,   0,  35,   0,   0,   0,
		 60,   2,   0,   0,  35,   0,
		  0, 128,  60,   2,   0,   0,
		 35,   0,   0,   0,  92,   2,
		  0,   0,  35,   0,   0, 128,
		 92,   2,   0,   0,  35,   0,
		  0,   0, 124,   2,   0,   0,
		 35,   0,   0, 128, 124,   2,
		  0,   0,  35,   0,   0,   0,
		156,   2,   0,   0,  35,   0,
		  0, 128, 156,   2,   0,   0,
		 35,   0,   0,   0, 188,   2,
		  0,   0,  35,   0,   0, 128,
		188,   2,   0,   0,  35,   0,
		  0,   0, 220,   2,   0,   0,
		 35,   0,   0, 128, 220,   2,
		  0,   0,  35,   0,   0,   0,
		252,   2,   0,   0,  35,   0,
		  0, 128, 252,   2,   0,   0,
		 35,   0,   0,   0,  28,   3,
		  0,   0,  35,   0,   0, 128,
		 28,   3,   0,   0,  35,   0,
		  0,   0,  60,   3,   0,   0,
		 35,   0,   0, 128,  60,   3,
		  0,   0,  35,   0,   0,   0,
		 92,   3,   0,   0,  35,   0,
		  0, 128,  92,   3,   0,   0,
		 35,   0,   0,   0, 112,   3,
		  0,   0,  35,   0,   0, 128,
		112,   3,   0,   0,  35,   0,
		  0,   0, 136,   3,   0,   0,
		 35,   0,   0, 128, 136,   3,
		  0,   0,  35,   0,   0,   0,
		160,   3,   0,   0,  35,   0,
		  0, 128, 160,   3,   0,   0,
		 35,   0,   0,   0, 180,   3,
		  0,   0,  35,   0,   0, 128,
		180,   3,   0,   0,  35,   0,
		  0,   0, 204,   3,   0,   0,
		 35,   0,   0, 128, 204,   3,
		  0,   0,  35,   0,   0,   0,
		228,   3,   0,   0,  35,   0,
		  0, 128, 228,   3,   0,   0,
		 35,   0,   0,   0, 248,   3,
		  0,   0,  35,   0,   0, 128,
		248,   3,   0,   0,  35,   0,
		  0,   0,  16,   4,   0,   0,
		 35,   0,   0, 128,  16,   4,
		  0,   0,  35,   0,   0,   0,
		 40,   4,   0,   0,  35,   0,
		  0, 128,  40,   4,   0,   0,
		 35,   0,   0,   0,  60,   4,
		  0,   0,  35,   0,   0, 128,
		 60,   4,   0,   0,  35,   0,
		  0,   0,  84,   4,   0,   0,
		 35,   0,   0, 128,  84,   4,
		  0,   0,  35,   0,   0,   0,
		108,   4,   0,   0,  35,   0,
		  0, 128, 108,   4,   0,   0,
		 35,   0,   0,   0, 128,   4,
		  0,   0,  35,   0,   0, 128,
		128,   4,   0,   0,  35,   0,
		  0,   0, 152,   4,   0,   0,
		 35,   0,   0, 128, 152,   4,
		  0,   0,  35,   0,   0,   0,
		176,   4,   0,   0,  35,   0,
		  0, 128, 176,   4,   0,   0,
		 35,   0,   0,   0, 200,   4,
		  0,   0,  35,   0,   0, 128,
		200,   4,   0,   0,  35,   0,
		  0,   0, 220,   4,   0,   0,
		 35,   0,   0, 128, 220,   4,
		  0,   0,  35,   0,   0,   0,
		248,   4,   0,   0,  35,   0,
		  0, 128, 248,   4,   0,   0,
		 35,   0,   0,   0,   4,   5,
		  0,   0,  35,   0,   0, 128,
		  4,   5,   0,   0,  35,   0,
		  0,   0,  28,   5,   0,   0,
		 35,   0,   0, 128,  28,   5,
		  0,   0,  35,   0,   0,   0,
		 32,   5,   0,   0,  35,   0,
		  0, 128,  32,   5,   0,   0,
		 35,   0,   0,   0,  44,   5,
		  0,   0,  35,   0,   0, 128,
		 44,   5,   0,   0,  35,   0,
		  0,   0,  64,   5,   0,   0,
		 35,   0,   0, 128,  64,   5,
		  0,   0,  35,   0,   0,   0,
		 92,   5,   0,   0,  35,   0,
		  0, 128,  92,   5,   0,   0,
		 35,   0,   0,   0, 112,   5,
		  0,   0,  35,   0,   0, 128,
		112,   5,   0,   0,  35,   0,
		  0,   0, 140,   5,   0,   0,
		 35,   0,   0, 128, 140,   5,
		  0,   0,  35,   0,   0,   0,
		168,   5,   0,   0,  35,   0,
		  0, 128, 168,   5,   0,   0,
		 35,   0,   0,   0, 188,   5,
		  0,   0,  35,   0,   0, 128,
		188,   5,   0,   0,  35,   0,
		  0,   0, 216,   5,   0,   0,
		 35,   0,   0, 128, 216,   5,
		  0,   0,  35,   0,   0,   0,
		240,   5,   0,   0,  35,   0,
		  0, 128, 240,   5,   0,   0,
		 35,   0,   0,   0,  12,   6,
		  0,   0,  35,   0,   0, 128,
		 12,   6,   0,   0,  35,   0,
		  0,   0,  36,   6,   0,   0,
		 35,   0,   0, 128,  36,   6,
		  0,   0,  35,   0,   0,   0,
		 72,   6,   0,   0,  35,   0,
		  0, 128,  72,   6,   0,   0,
		 35,   0,   0,   0, 112,   6,
		  0,   0,  35,   0,   0, 128,
		112,   6,   0,   0,  35,   0,
		  0,   0, 140,   6,   0,   0,
		 35,   0,   0, 128, 140,   6,
		  0,   0,  35,   0,   0,   0,
		160,   6,   0,   0,  35,   0,
		  0, 128, 160,   6,   0,   0,
		 35,   0,   0,   0, 188,   6,
		  0,   0,  35,   0,   0, 128,
		188,   6,   0,   0,  35,   0,
		  0,   0, 212,   6,   0,   0,
		 35,   0,   0, 128, 212,   6,
		  0,   0,  35,   0,   0,   0,
		240,   6,   0,   0,  35,   0,
		  0, 128, 240,   6,   0,   0,
		 35,   0,   0,   0,  12,   7,
		  0,   0,  35,   0,   0, 128,
		 12,   7,   0,   0,  35,   0,
		  0,   0,  40,   7,   0,   0,
		 35,   0,   0, 128,  40,   7,
		  0,   0,  35,   0,   0,   0,
		 44,   7,   0,   0,  35,   0,
		  0, 128,  44,   7,   0,   0,
		 35,   0,   0,   0,  48,   7,
		  0,   0,  35,   0,   0, 128,
		 48,   7,   0,   0,  35,   0,
		  0,   0,  68,   7,   0,   0,
		 35,   0,   0, 128,  68,   7,
		  0,   0,  35,   0,   0,   0,
		 88,   7,   0,   0,  35,   0,
		  0, 128,  88,   7,   0,   0,
		 35,   0,   0,   0,   2,   0,
		 34,   0,   2,   0,  33,   0,
		  2,   0,  34,   0,   2,   0,
		 33,   0,   2,   0,  34,   0,
		  2,   0,  33,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  35,   0,
		 43,   0,   2,   0,  62,   0,
		 35,   0,  43,   0,   2,   0,
		 62,   0,  46,   0,  55,   0,
		  2,   0,  62,   0,  46,   0,
		 55,   0,   2,   0,  62,   0,
		 29,   0,  56,   0,   2,   0,
		 62,   0,  29,   0,  56,   0,
		  2,   0,  62,   0,  25,   0,
		 57,   0,   2,   0,  62,   0,
		 25,   0,  57,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		 16,   0,  58,   0,   2,   0,
		 62,   0,  16,   0,  58,   0,
		  2,   0,  62,   0,  16,   0,
		 58,   0,   2,   0,  62,   0,
		  9,   0,  61,   0,   2,   0,
		 62,   0,   2,   0,  62,   0,
		  2,   0,  62,   0,   2,   0,
		 62,   0, 246,   0,   0,   0,
		 16,   0,   0,   0,   0,   0,
		  0,   0,   0,  16,   0,   0,
		  0,   0,   0,   0,   3,   0,
		  0,   0,   8,   0,   0,   0,
		  0,   0,   0,   0,  20,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  11, 202,
		 49,   1,  56,   0,   0,   0,
		  0,  16,   0,   0,  13,  16,
		  0,   0, 216,   0,   0,   0,
		 10,   0, 255, 255,   4,   0,
		  0,   0, 255, 255,   3,   0,
		  0,   0,   0,   0,  52,   0,
		  0,   0,  52,   0,   0,   0,
		  8,   0,   0,   0,  60,   0,
		  0,   0,   0,   0,   0,   0,
		 22,   0,  27,  21,  64,   0,
		  0,   0,   4,   0,   0,   0,
		 16,   0, 102, 108, 111,  97,
		116,  52,   0, 243, 242, 241,
		 22,   0,  27,  21,  64,   0,
		  0,   0,   2,   0,   0,   0,
		  8,   0, 102, 108, 111,  97,
		116,  50,   0, 243, 242, 241,
		 14,   0,   1,  18,   2,   0,
		  0,   0,   0,  16,   0,   0,
		  1,  16,   0,   0,  10,   0,
		 24,  21,   0,  16,   0,   0,
		  1,   0,   1,   0,  14,   0,
		  8,  16,   3,  16,   0,   0,
		 23,   0,   2,   0,   2,  16,
		  0,   0,  10,   0,   1,  18,
		  1,   0,   0,   0, 116,   0,
		  0,   0,  22,   0,  27,  21,
		 64,   0,   0,   0,   3,   0,
		  0,   0,  12,   0, 102, 108,
		111,  97, 116,  51,   0, 243,
		242, 241,  10,   0,  24,  21,
		  6,  16,   0,   0,   1,   0,
		  1,   0,  14,   0,   8,  16,
		  7,  16,   0,   0,  23,   0,
		  1,   0,   5,  16,   0,   0,
		 18,   0,  22,  21,   6,  16,
		  0,   0,  34,   0,   0,   0,
		 16,   0,   0,   0, 172,   0,
		  0, 241,  14,   0,  23,  21,
		116,   0,   0,   0,   3,   2,
		  0,   0,   0,   0, 242, 241,
		 10,   0,  24,  21,  10,  16,
		  0,   0,   1,   0,   1,   0,
		 10,   0,  24,  21,  11,  16,
		  0,   0,   1,   0,   0,   2,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 43,  73,   0,   0,   0,  16,
		  0,   0,   0,   0,   0,   0,
		  0,  16,   0,   0,   0,   0,
		  0,   0, 255, 255, 255, 255,
		  4,   0,   0,   0, 255, 255,
		  3,   0,   0,   0,   0,   0,
		255, 255, 255, 255,   0,   0,
		  0,   0, 255, 255, 255, 255,
		  0,   0,   0,   0, 255, 255,
		255, 255,  22,   0,  27,  21,
		 64,   0,   0,   0,   4,   0,
		  0,   0,  16,   0, 102, 108,
		111,  97, 116,  52,   0, 243,
		242, 241,  22,   0,  27,  21,
		 64,   0,   0,   0,   2,   0,
		  0,   0,   8,   0, 102, 108,
		111,  97, 116,  50,   0, 243,
		242, 241,  14,   0,   1,  18,
		  2,   0,   0,   0,   0,  16,
		  0,   0,   1,  16,   0,   0,
		 10,   0,  24,  21,   0,  16,
		  0,   0,   1,   0,   1,   0,
		 14,   0,   8,  16,   3,  16,
		  0,   0,  23,   0,   2,   0,
		  2,  16,   0,   0,  10,   0,
		  1,  18,   1,   0,   0,   0,
		116,   0,   0,   0,  22,   0,
		 27,  21,  64,   0,   0,   0,
		  3,   0,   0,   0,  12,   0,
		102, 108, 111,  97, 116,  51,
		  0, 243, 242, 241,  10,   0,
		 24,  21,   6,  16,   0,   0,
		  1,   0,   1,   0,  14,   0,
		  8,  16,   7,  16,   0,   0,
		 23,   0,   1,   0,   5,  16,
		  0,   0,  18,   0,  22,  21,
		  6,  16,   0,   0,  34,   0,
		  0,   0,  16,   0,   0,   0,
		172,   0,   0, 241,  14,   0,
		 23,  21, 116,   0,   0,   0,
		  3,   2,   0,   0,   0,   0,
		242, 241,  10,   0,  24,  21,
		 10,  16,   0,   0,   1,   0,
		  1,   0,  10,   0,  24,  21,
		 11,  16,   0,   0,   1,   0,
		  0,   2,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  68,  51,  68,  83,
		 72,  68,  82,   0,  92,   7,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  32,   0,   0,  96,
		  4,   0,   0,   0,   8,   0,
		  0,   0,  12,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		  1,  22,   0,   0,   0,   0,
		  8,  16,   0,   0,  71, 101,
		116,  67, 111, 108, 111, 114,
		  0, 243, 242, 241,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 255, 255,
		255, 255,  26,   9,  47, 241,
		 16,   0,   0,   0,  12,   2,
		  0,   0,  21,   0,   0,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  16,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  32,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  12,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  18,   0,  37,  17,
		  0,   0,   0,   0, 128,   0,
		  0,   0,   1,   0, 109,  97,
		105, 110,   0,   0,  22,   0,
		 81,  17,  12,  16,   0,   0,
		  7,   0, 255, 255, 255, 255,
		  0,   0, 255, 255, 255, 255,
		116, 101, 120,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  16,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0, 255, 255, 255, 255,
		 26,   9,  47, 241,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		255, 255, 255, 255, 119,   9,
		 49,   1,   1,   0,   0,   0,
		 13,   0,   0, 142,  14,   0,
		 63,  92,  15,   0,   0,   0,
		 76,   0,   0,   0,  32,   0,
		  0,   0,  44,   0,   0,   0,
		 88,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 22,   0,   0,   0,  25,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,  92,   7,
		  0,   0,  32,   0,   0,  96,
		  0,   0,  40, 142,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  2,   0,   9,   0,  84,   5,
		  0,   0,   0,   0,   0,   0,
		144,   7,   0,   0,   1,   0,
		  0,   0, 112,  26, 151,   2,
		  0,   0,   0,   0,   0,   0,
		  0,   0, 109,  97, 105, 110,
		  0, 110, 111, 110, 101,   0,
		  0,   0,  45, 186,  46, 241,
		  1,   0,   0,   0,   0,   0,
		  0,   0,  92,   7,   0,   0,
		 32,   0,   0,  96,   0,   0,
		 40, 142,   0,   0,   0,   0,
		  0,   0,   0,   0,   2,   0,
		  2,   0,   7,   0,   0,   0,
		  0,   0,   1,   0, 255, 255,
		255, 255,   0,   0,   0,   0,
		 92,   7,   0,   0,   8,   2,
		  0,   0,   0,   0,   0,   0,
		255, 255, 255, 255,   0,   0,
		  0,   0, 255, 255, 255, 255,
		  1,   0,   1,   0,   0,   0,
		  1,   0,   0,   0,   0,   0,
		 67,  58,  92,  85, 115, 101,
		114, 115,  92, 108, 108, 101,
		111, 110,  97, 114, 116,  92,
		 68, 101, 115, 107, 116, 111,
		112,  92,  80, 104,  68,  32,
		 80, 114, 111, 106, 101,  99,
		116,  92,  67,  65,  52,  71,
		 92,  83, 104,  97, 100, 101,
		114, 115,  92,  68, 114,  97,
		119,  67, 111, 109, 112, 108,
		101, 120, 105, 116, 121,  95,
		 80,  83,  46, 104, 108, 115,
		108,   0,   0,   0, 254, 239,
		254, 239,   1,   0,   0,   0,
		  1,   0,   0,   0,   0,   1,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255,  12,   0, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  44,  49,  44,  48,
		 41,  44,  32,  47,  47,  32,
		 51,  50,  13,  10,   9,   9,
		102, 108, 111,  97, 116,  51,
		 40,  48,  46,  53,  44,  49,
		 44,  48,  41,  44,  32,  47,
		 47,  32,  54,  52,  13,  10,
		  9,   9, 102, 108, 111,  97,
		116,  51,  40,  49,  44,  49,
		 44,  48,  41,  44,  32,  47,
		 47,  32,  49,  50,  56,  13,
		 10,   9,   9, 102, 108, 111,
		 97, 116,  51,  40,  49,  44,
		 48,  46,  53,  44,  48,  41,
		 44,  32,  47,  47,  32,  50,
		 53,  54,  13,  10,   9,   9,
		102, 108, 111,  97, 116,  51,
		 40,  49,  44,  48,  44,  48,
		 41,  44,  32,  47,  47,  32,
		 53,  49,  50,  13,  10,   9,
		  9, 102, 108, 111,  97, 116,
		 51,  40,  49,  44,  48,  44,
		 49,  41,  32,  47,  47,  32,
		 49,  48,  50,  52,  13,  10,
		  9, 125,  59,  13,  10,  13,
		 10,   9, 105, 102,  32,  40,
		108, 101, 118, 101, 108,  32,
		 62,  61,  32,  49,  48,  41,
		 13,  10,   9,   9, 114, 101,
		116, 117, 114, 110,  32, 115,
		116, 111, 112,  80, 111, 105,
		110, 116, 115,  91,  49,  48,
		 93,  59,  13,  10,  13,  10,
		  9, 114, 101, 116, 117, 114,
		110,  32, 108, 101, 114, 112,
		 40, 115, 116, 111, 112,  80,
		111, 105, 110, 116, 115,  91,
		 40, 105, 110, 116,  41, 108,
		101, 118, 101, 108,  93,  44,
		 32, 115, 116, 111, 112,  80,
		111, 105, 110, 116, 115,  91,
		 40, 105, 110, 116,  41, 108,
		101, 118, 101, 108,  32,  43,
		 32,  49,  93,  44,  32, 108,
		101, 118, 101, 108,  32,  37,
		 32,  49,  41,  59,  13,  10,
		125,  13,  10,  13,  10, 102,
		108, 111,  97, 116,  52,  32,
		109,  97, 105, 110,  40, 102,
		108, 111,  97, 116,  52,  32,
		112, 114, 111, 106,  32,  58,
		 32,  83,  86,  95,  80,  79,
		 83,  73,  84,  73,  79,  78,
		 44,  32, 102, 108, 111,  97,
		116,  50,  32,  67,  32,  58,
		 32,  84,  69,  88,  67,  79,
		 79,  82,  68,  41,  32,  58,
		 32,  83,  86,  95,  84,  65,
		 82,  71,  69,  84,  13,  10,
		123,  13,  10,   9, 105, 110,
		116,  32, 119, 105, 100, 116,
		104,  44,  32, 104, 101, 105,
		103, 104, 116,  59,  13,  10,
		  9, 116, 101, 120,  46,  71,
		101, 116,  68, 105, 109, 101,
		110, 115, 105, 111, 110, 115,
		 40, 119, 105, 100, 116, 104,
		 44,  32, 104, 101, 105, 103,
		104, 116,  41,  59,  13,  10,
		  9, 114, 101, 116, 117, 114,
		110,  32, 102, 108, 111,  97,
		116,  52,  40,  71, 101, 116,
		 67, 111, 108, 111, 114,  40,
		116, 101, 120,  91, 117, 105,
		110, 116,  50,  40, 119, 105,
		100, 116, 104,  42,  67,  46,
		120,  44,  32, 104, 101, 105,
		103, 104, 116,  42,  67,  46,
		121,  41,  93,  41,  44,  49,
		 41,  59,  13,  10, 125,   0,
		  7,   0,   0,   0,   0,   0,
		  0,   0,  75,   0,   0,   0,
		150,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		 76,   0,   0,   0,   0,   0,
		  0,   0,   4,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		148,  46,  49,   1, 250, 103,
		166,  92,   1,   0,   0,   0,
		187,  24, 125, 247, 150, 195,
		117,  70, 168, 206,  99, 144,
		125,  61,  40, 142, 119,   0,
		  0,   0,  47,  76, 105, 110,
		107,  73, 110, 102, 111,   0,
		 47, 110,  97, 109, 101, 115,
		  0,  47, 115, 114,  99,  47,
		104, 101,  97, 100, 101, 114,
		 98, 108, 111,  99, 107,   0,
		 47, 115, 114,  99,  47, 102,
		105, 108, 101, 115,  47,  99,
		 58,  92, 117, 115, 101, 114,
		115,  92, 108, 108, 101, 111,
		110,  97, 114, 116,  92, 100,
		101, 115, 107, 116, 111, 112,
		 92, 112, 104, 100,  32, 112,
		114, 111, 106, 101,  99, 116,
		 92,  99,  97,  52, 103,  92,
		115, 104,  97, 100, 101, 114,
		115,  92, 100, 114,  97, 119,
		 99, 111, 109, 112, 108, 101,
		120, 105, 116, 121,  95, 112,
		115,  46, 104, 108, 115, 108,
		  0,   4,   0,   0,   0,   6,
		  0,   0,   0,   1,   0,   0,
		  0,  27,   0,   0,   0,   0,
		  0,   0,   0,  34,   0,   0,
		  0,   8,   0,   0,   0,  17,
		  0,   0,   0,   7,   0,   0,
		  0,  10,   0,   0,   0,   6,
		  0,   0,   0,   0,   0,   0,
		  0,   5,   0,   0,   0,   0,
		  0,   0,   0, 220,  81,  51,
		  1,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  16,   0,   0,   0,
		 32,   0,   0,   0, 211,   0,
		  0,   0,  16,   1,   0,   0,
		 95,   1,   0,   0,  80,   0,
		  0,   0,   0,   0,   0,   0,
		  2,   4,   0,   0, 128,   0,
		  0,   0,  59,   3,   0,   0,
		240,  12,   0,   0,  60,   0,
		  0,   0,  12,   0,   0,   0,
		 40,   0,   0,   0,  44,   2,
		  0,   0,  44,   0,   0,   0,
		 44,   0,   0,   0,   3,   0,
		  0,   0,  29,   0,   0,   0,
		 19,   0,   0,   0,  26,   0,
		  0,   0,   6,   0,   0,   0,
		 10,   0,   0,   0,  27,   0,
		  0,   0,  28,   0,   0,   0,
		 11,   0,   0,   0,   8,   0,
		  0,   0,   9,   0,   0,   0,
		 12,   0,   0,   0,  13,   0,
		  0,   0,  14,   0,   0,   0,
		 15,   0,   0,   0,  16,   0,
		  0,   0,  17,   0,   0,   0,
		 18,   0,   0,   0,   7,   0,
		  0,   0,  20,   0,   0,   0,
		 21,   0,   0,   0,  22,   0,
		  0,   0,  23,   0,   0,   0,
		 25,   0,   0,   0,  24,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  30,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0
	};
#if 0
	//
	// Generated by Microsoft (R) HLSL Shader Compiler 10.1
	//
	//
	// Resource Bindings:
	//
	// Name                                 Type  Format         Dim      HLSL Bind  Count
	// ------------------------------ ---------- ------- ----------- -------------- ------
	// tex                               texture  float4          2d             t0      1 
	//
	//
	//
	// Input signature:
	//
	// Name                 Index   Mask Register SysValue  Format   Used
	// -------------------- ----- ------ -------- -------- ------- ------
	// SV_POSITION              0   xyzw        0      POS   float       
	// TEXCOORD                 0   xy          1     NONE   float   xy  
	//
	//
	// Output signature:
	//
	// Name                 Index   Mask Register SysValue  Format   Used
	// -------------------- ----- ------ -------- -------- ------- ------
	// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
	//
	ps_5_0
		dcl_globalFlags refactoringAllowed | skipOptimization
		dcl_resource_texture2d(float, float, float, float) t0
		dcl_input_ps linear v1.xy
		dcl_output o0.xyzw
		dcl_temps 2
		//
		// Initial variable locations:
		//   v0.x <- proj.x; v0.y <- proj.y; v0.z <- proj.z; v0.w <- proj.w; 
		//   v1.x <- C.x; v1.y <- C.y; 
		//   o0.x <- <main return value>.x; o0.y <- <main return value>.y; o0.z <- <main return value>.z; o0.w <- <main return value>.w
		//
#line 10 "C:\Users\lleonart\Desktop\PhD Project\CA4G\Shaders\DrawScreen_PS.hlsl"
		resinfo_indexable(texture2d)(float, float, float, float)_uint r0.xy, l(0), t0.xyzw
		mov r0.x, r0.x  // r0.x <- w
		mov r0.y, r0.y  // r0.y <- h

#line 12
		utof r1.x, r0.x
		utof r1.y, r0.y
		mul r0.xy, r1.xyxx, v1.xyxx
		ftou r0.xy, r0.xyxx
		mov r0.zw, l(0, 0, 0, 0)
		ld_indexable(texture2d)(float, float, float, float) o0.xyzw, r0.xyzw, t0.xyzw
		ret
		// Approximately 10 instruction slots used
#endif

		const BYTE cso_DrawScreen_PS[] =
	{
		 68,  88,  66,  67,  85,  59,
		251, 103,   7,  91, 135,  16,
		163,  42, 196, 152, 113, 122,
		 91, 124,   1,   0,   0,   0,
		 40,  57,   0,   0,   6,   0,
		  0,   0,  56,   0,   0,   0,
		200,   0,   0,   0,  32,   1,
		  0,   0,  84,   1,   0,   0,
		132,   2,   0,   0,  32,   3,
		  0,   0,  82,  68,  69,  70,
		136,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  1,   0,   0,   0,  60,   0,
		  0,   0,   0,   5, 255, 255,
		  5,   1,   0,   0,  96,   0,
		  0,   0,  82,  68,  49,  49,
		 60,   0,   0,   0,  24,   0,
		  0,   0,  32,   0,   0,   0,
		 40,   0,   0,   0,  36,   0,
		  0,   0,  12,   0,   0,   0,
		  0,   0,   0,   0,  92,   0,
		  0,   0,   2,   0,   0,   0,
		  5,   0,   0,   0,   4,   0,
		  0,   0, 255, 255, 255, 255,
		  0,   0,   0,   0,   1,   0,
		  0,   0,  13,   0,   0,   0,
		116, 101, 120,   0,  77, 105,
		 99, 114, 111, 115, 111, 102,
		116,  32,  40,  82,  41,  32,
		 72,  76,  83,  76,  32,  83,
		104,  97, 100, 101, 114,  32,
		 67, 111, 109, 112, 105, 108,
		101, 114,  32,  49,  48,  46,
		 49,   0,  73,  83,  71,  78,
		 80,   0,   0,   0,   2,   0,
		  0,   0,   8,   0,   0,   0,
		 56,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  3,   0,   0,   0,   0,   0,
		  0,   0,  15,   0,   0,   0,
		 68,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  3,   0,   0,   0,   1,   0,
		  0,   0,   3,   3,   0,   0,
		 83,  86,  95,  80,  79,  83,
		 73,  84,  73,  79,  78,   0,
		 84,  69,  88,  67,  79,  79,
		 82,  68,   0, 171, 171, 171,
		 79,  83,  71,  78,  44,   0,
		  0,   0,   1,   0,   0,   0,
		  8,   0,   0,   0,  32,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   3,   0,
		  0,   0,   0,   0,   0,   0,
		 15,   0,   0,   0,  83,  86,
		 95,  84,  65,  82,  71,  69,
		 84,   0, 171, 171,  83,  72,
		 69,  88,  40,   1,   0,   0,
		 80,   0,   0,   0,  74,   0,
		  0,   0, 106, 136,   0,   1,
		 88,  24,   0,   4,   0, 112,
		 16,   0,   0,   0,   0,   0,
		 85,  85,   0,   0,  98,  16,
		  0,   3,  50,  16,  16,   0,
		  1,   0,   0,   0, 101,   0,
		  0,   3, 242,  32,  16,   0,
		  0,   0,   0,   0, 104,   0,
		  0,   2,   2,   0,   0,   0,
		 61,  16,   0, 137, 194,   0,
		  0, 128,  67,  85,  21,   0,
		 50,   0,  16,   0,   0,   0,
		  0,   0,   1,  64,   0,   0,
		  0,   0,   0,   0,  70, 126,
		 16,   0,   0,   0,   0,   0,
		 54,   0,   0,   5,  18,   0,
		 16,   0,   0,   0,   0,   0,
		 10,   0,  16,   0,   0,   0,
		  0,   0,  54,   0,   0,   5,
		 34,   0,  16,   0,   0,   0,
		  0,   0,  26,   0,  16,   0,
		  0,   0,   0,   0,  86,   0,
		  0,   5,  18,   0,  16,   0,
		  1,   0,   0,   0,  10,   0,
		 16,   0,   0,   0,   0,   0,
		 86,   0,   0,   5,  34,   0,
		 16,   0,   1,   0,   0,   0,
		 26,   0,  16,   0,   0,   0,
		  0,   0,  56,   0,   0,   7,
		 50,   0,  16,   0,   0,   0,
		  0,   0,  70,   0,  16,   0,
		  1,   0,   0,   0,  70,  16,
		 16,   0,   1,   0,   0,   0,
		 28,   0,   0,   5,  50,   0,
		 16,   0,   0,   0,   0,   0,
		 70,   0,  16,   0,   0,   0,
		  0,   0,  54,   0,   0,   8,
		194,   0,  16,   0,   0,   0,
		  0,   0,   2,  64,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  45,   0,
		  0, 137, 194,   0,   0, 128,
		 67,  85,  21,   0, 242,  32,
		 16,   0,   0,   0,   0,   0,
		 70,  14,  16,   0,   0,   0,
		  0,   0,  70, 126,  16,   0,
		  0,   0,   0,   0,  62,   0,
		  0,   1,  83,  84,  65,  84,
		148,   0,   0,   0,  10,   0,
		  0,   0,   2,   0,   0,   0,
		  0,   0,   0,   0,   2,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   3,   0,   0,   0,
		  0,   0,   0,   0,   3,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  83,  80,  68,  66,
		  0,  54,   0,   0,  77, 105,
		 99, 114, 111, 115, 111, 102,
		116,  32,  67,  47,  67,  43,
		 43,  32,  77,  83,  70,  32,
		 55,  46,  48,  48,  13,  10,
		 26,  68,  83,   0,   0,   0,
		  0,   2,   0,   0,   2,   0,
		  0,   0,  27,   0,   0,   0,
		136,   0,   0,   0,   0,   0,
		  0,   0,  23,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		192, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255,  56,   0,   0, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255, 255, 255, 255,   5,   0,
		  0,   0,  32,   0,   0,   0,
		 60,   0,   0,   0,   0,   0,
		  0,   0, 255, 255, 255, 255,
		  0,   0,   0,   0,   6,   0,
		  0,   0,   5,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  3,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0, 148,  46,  49,   1,
		251, 103, 166,  92,   1,   0,
		  0,   0, 190, 221,   2,  60,
		168,  64, 148,  64, 179, 104,
		 16, 246, 117, 235, 191, 175,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   1,   0,   0,   0,
		  1,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 220,  81,
		 51,   1,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  68,  51,
		 68,  83,  72,  68,  82,   0,
		 40,   1,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  32,   0,
		  0,  96,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		198,  90,   0,   0, 117, 131,
		  1,   0, 178, 211,   1,   0,
		 65,  36,   1,   0, 137, 231,
		  1,   0,   2, 209,   1,   0,
		109,  24,   1,   0,   9, 241,
		  2,   0,   0,  16,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  84, 101, 120, 116,
		117, 114, 101,  50,  68,  60,
		102, 108, 111,  97, 116,  52,
		 62,  32, 116, 101, 120,  32,
		 58,  32, 114, 101, 103, 105,
		115, 116, 101, 114,  32,  40,
		116,  48,  41,  59,  13,  10,
		 13,  10,  47,  47,  99,  98,
		117, 102, 102, 101, 114,  32,
		 84, 114,  97, 110, 115, 102,
		111, 114, 109, 105, 110, 103,
		 32,  58,  32, 114, 101, 103,
		105, 115, 116, 101, 114,  32,
		 40,  98,  48,  41,  32, 123,
		 13,  10,  47,  47,   9, 114,
		111, 119,  95, 109,  97, 106,
		111, 114,  32, 109,  97, 116,
		114, 105, 120,  32,  84, 114,
		 97, 110, 115, 102, 111, 114,
		109,  59,  13,  10,  47,  47,
		125,  13,  10,  13,  10, 102,
		108, 111,  97, 116,  52,  32,
		109,  97, 105, 110,  40, 102,
		108, 111,  97, 116,  52,  32,
		112, 114, 111, 106,  32,  58,
		 32,  83,  86,  95,  80,  79,
		 83,  73,  84,  73,  79,  78,
		 44,  32, 102, 108, 111,  97,
		116,  50,  32,  67,  32,  58,
		 32,  84,  69,  88,  67,  79,
		 79,  82,  68,  41,  32,  58,
		 32,  83,  86,  95,  84,  65,
		 82,  71,  69,  84,  13,  10,
		123,  13,  10,   9, 117, 105,
		110, 116,  32, 119,  44, 104,
		 59,  13,  10,   9, 116, 101,
		120,  46,  71, 101, 116,  68,
		105, 109, 101, 110, 115, 105,
		111, 110, 115,  40, 119,  44,
		 32, 104,  41,  59,  13,  10,
		  9,  47,  47, 114, 101, 116,
		117, 114, 110,  32, 109, 117,
		108,  40, 116, 101, 120,  46,
		 83,  97, 109, 112, 108, 101,
		 40,  83,  97, 109, 112,  44,
		 32,  67,  41,  44,  32,  84,
		114,  97, 110, 115, 102, 111,
		114, 109,  41,  59,  13,  10,
		  9, 114, 101, 116, 117, 114,
		110,  32, 116, 101, 120,  91,
		117, 105, 110, 116,  50,  40,
		119,  44, 104,  41,  42,  67,
		 93,  59,  13,  10, 125,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 254, 239,
		254, 239,   1,   0,   0,   0,
		202,   1,   0,   0,   0,  67,
		 58,  92,  85, 115, 101, 114,
		115,  92, 108, 108, 101, 111,
		110,  97, 114, 116,  92,  68,
		101, 115, 107, 116, 111, 112,
		 92,  80, 104,  68,  32,  80,
		114, 111, 106, 101,  99, 116,
		 92,  67,  65,  52,  71,  92,
		 83, 104,  97, 100, 101, 114,
		115,  92,  68, 114,  97, 119,
		 83,  99, 114, 101, 101, 110,
		 95,  80,  83,  46, 104, 108,
		115, 108,   0,   0,  99,  58,
		 92, 117, 115, 101, 114, 115,
		 92, 108, 108, 101, 111, 110,
		 97, 114, 116,  92, 100, 101,
		115, 107, 116, 111, 112,  92,
		112, 104, 100,  32, 112, 114,
		111, 106, 101,  99, 116,  92,
		 99,  97,  52, 103,  92, 115,
		104,  97, 100, 101, 114, 115,
		 92, 100, 114,  97, 119, 115,
		 99, 114, 101, 101, 110,  95,
		112, 115,  46, 104, 108, 115,
		108,   0,  84, 101, 120, 116,
		117, 114, 101,  50,  68,  60,
		102, 108, 111,  97, 116,  52,
		 62,  32, 116, 101, 120,  32,
		 58,  32, 114, 101, 103, 105,
		115, 116, 101, 114,  32,  40,
		116,  48,  41,  59,  13,  10,
		 13,  10,  47,  47,  99,  98,
		117, 102, 102, 101, 114,  32,
		 84, 114,  97, 110, 115, 102,
		111, 114, 109, 105, 110, 103,
		 32,  58,  32, 114, 101, 103,
		105, 115, 116, 101, 114,  32,
		 40,  98,  48,  41,  32, 123,
		 13,  10,  47,  47,   9, 114,
		111, 119,  95, 109,  97, 106,
		111, 114,  32, 109,  97, 116,
		114, 105, 120,  32,  84, 114,
		 97, 110, 115, 102, 111, 114,
		109,  59,  13,  10,  47,  47,
		125,  13,  10,  13,  10, 102,
		108, 111,  97, 116,  52,  32,
		109,  97, 105, 110,  40, 102,
		108, 111,  97, 116,  52,  32,
		112, 114, 111, 106,  32,  58,
		 32,  83,  86,  95,  80,  79,
		 83,  73,  84,  73,  79,  78,
		 44,  32, 102, 108, 111,  97,
		116,  50,  32,  67,  32,  58,
		 32,  84,  69,  88,  67,  79,
		 79,  82,  68,  41,  32,  58,
		 32,  83,  86,  95,  84,  65,
		 82,  71,  69,  84,  13,  10,
		123,  13,  10,   9, 117, 105,
		110, 116,  32, 119,  44, 104,
		 59,  13,  10,   9, 116, 101,
		120,  46,  71, 101, 116,  68,
		105, 109, 101, 110, 115, 105,
		111, 110, 115,  40, 119,  44,
		 32, 104,  41,  59,  13,  10,
		  9,  47,  47, 114, 101, 116,
		117, 114, 110,  32, 109, 117,
		108,  40, 116, 101, 120,  46,
		 83,  97, 109, 112, 108, 101,
		 40,  83,  97, 109, 112,  44,
		 32,  67,  41,  44,  32,  84,
		114,  97, 110, 115, 102, 111,
		114, 109,  41,  59,  13,  10,
		  9, 114, 101, 116, 117, 114,
		110,  32, 116, 101, 120,  91,
		117, 105, 110, 116,  50,  40,
		119,  44, 104,  41,  42,  67,
		 93,  59,  13,  10, 125,   0,
		  7,   0,   0,   0,   0,   0,
		  0,   0,  71,   0,   0,   0,
		  0,   0,   0,   0,   1,   0,
		  0,   0,  72,   0,   0,   0,
		142,   0,   0,   0,   0,   0,
		  0,   0,   4,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 27, 226,  48,   1, 128,   0,
		  0,   0,  18,  54, 150, 102,
		 36, 235, 212,   1,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   1,   0,
		  0,   0,   2,   0,   0,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		 72,   0,   0,   0,  40,   0,
		  0,   0,  27, 226,  48,   1,
		103, 155, 154, 125,  59,   1,
		  0,   0,   1,   0,   0,   0,
		 71,   0,   0,   0,  72,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   4,   0,   0,   0,
		 66,   0,  60,  17,  16,   1,
		  0,   0,   0,   1,  10,   0,
		  1,   0, 132,   0,  99,  69,
		 10,   0,   1,   0, 132,   0,
		 99,  69,  77, 105,  99, 114,
		111, 115, 111, 102, 116,  32,
		 40,  82,  41,  32,  72,  76,
		 83,  76,  32,  83, 104,  97,
		100, 101, 114,  32,  67, 111,
		109, 112, 105, 108, 101, 114,
		 32,  49,  48,  46,  49,   0,
		  0,   0,  54,   0,  61,  17,
		  1, 104, 108, 115, 108,  70,
		108,  97, 103, 115,   0,  48,
		120,  53,   0, 104, 108, 115,
		108,  84,  97, 114, 103, 101,
		116,   0, 112, 115,  95,  53,
		 95,  48,   0, 104, 108, 115,
		108,  69, 110, 116, 114, 121,
		  0, 109,  97, 105, 110,   0,
		  0,   0,   0,   0,  42,   0,
		 16,  17,   0,   0,   0,   0,
		184,   2,   0,   0,   0,   0,
		  0,   0, 236,   0,   0,   0,
		  0,   0,   0,   0, 236,   0,
		  0,   0,   4,  16,   0,   0,
		 60,   0,   0,   0,   1,   0,
		160, 109,  97, 105, 110,   0,
		 42,   0,  62,  17,   0,  16,
		  0,   0,   9,   0, 112, 114,
		111, 106,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  22,   0,  80,  17,
		  1,   0,   5,   0,   0,   0,
		  4,   0,  60,   0,   0,   0,
		  1,   0, 236,   0,   0,   0,
		  0,   0,  22,   0,  80,  17,
		  1,   0,   5,   0,   4,   0,
		  4,   0,  60,   0,   0,   0,
		  1,   0, 236,   0,   4,   0,
		  0,   0,  22,   0,  80,  17,
		  1,   0,   5,   0,   8,   0,
		  4,   0,  60,   0,   0,   0,
		  1,   0, 236,   0,   8,   0,
		  0,   0,  22,   0,  80,  17,
		  1,   0,   5,   0,  12,   0,
		  4,   0,  60,   0,   0,   0,
		  1,   0, 236,   0,  12,   0,
		  0,   0,  42,   0,  62,  17,
		  1,  16,   0,   0,   9,   0,
		 67,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   1,   0,   5,   0,
		  0,   0,   4,   0,  60,   0,
		  0,   0,   1,   0, 236,   0,
		 16,   0,   0,   0,  22,   0,
		 80,  17,   1,   0,   5,   0,
		  4,   0,   4,   0,  60,   0,
		  0,   0,   1,   0, 236,   0,
		 20,   0,   0,   0,  58,   0,
		 62,  17,   3,  16,   0,   0,
		136,   0,  60, 109,  97, 105,
		110,  32, 114, 101, 116, 117,
		114, 110,  32, 118,  97, 108,
		117, 101,  62,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   2,   0,   5,   0,
		  0,   0,   4,   0,  60,   0,
		  0,   0,   1,   0, 236,   0,
		  0,   0,   0,   0,  22,   0,
		 80,  17,   2,   0,   5,   0,
		  4,   0,   4,   0,  60,   0,
		  0,   0,   1,   0, 236,   0,
		  4,   0,   0,   0,  22,   0,
		 80,  17,   2,   0,   5,   0,
		  8,   0,   4,   0,  60,   0,
		  0,   0,   1,   0, 236,   0,
		  8,   0,   0,   0,  22,   0,
		 80,  17,   2,   0,   5,   0,
		 12,   0,   4,   0,  60,   0,
		  0,   0,   1,   0, 236,   0,
		 12,   0,   0,   0,  42,   0,
		 62,  17, 117,   0,   0,   0,
		  0,   0, 119,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 22,   0,  80,  17,   0,   0,
		  1,   0,   0,   0,   4,   0,
		116,   0,   0,   0,   1,   0,
		180,   0,   0,   0,   0,   0,
		 42,   0,  62,  17, 117,   0,
		  0,   0,   0,   0, 104,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  22,   0,  80,  17,
		  0,   0,   1,   0,   0,   0,
		  4,   0, 136,   0,   0,   0,
		  1,   0, 160,   0,   4,   0,
		  0,   0,   2,   0,   6,   0,
		244,   0,   0,   0,  24,   0,
		  0,   0,   1,   0,   0,   0,
		 16,   1, 185,  17,  16, 126,
		216,  30, 223,  43,  12,  49,
		229, 143, 134, 240, 181,  38,
		  0,   0, 242,   0,   0,   0,
		  8,   1,   0,   0,   0,   0,
		  0,   0,   1,   0,   1,   0,
		 40,   1,   0,   0,   0,   0,
		  0,   0,  20,   0,   0,   0,
		252,   0,   0,   0,  60,   0,
		  0,   0,  10,   0,   0, 128,
		 60,   0,   0,   0,  10,   0,
		  0,   0,  96,   0,   0,   0,
		 10,   0,   0, 128,  96,   0,
		  0,   0,  10,   0,   0,   0,
		116,   0,   0,   0,  10,   0,
		  0, 128, 116,   0,   0,   0,
		 10,   0,   0,   0, 136,   0,
		  0,   0,  12,   0,   0, 128,
		136,   0,   0,   0,  12,   0,
		  0,   0, 156,   0,   0,   0,
		 12,   0,   0, 128, 156,   0,
		  0,   0,  12,   0,   0,   0,
		176,   0,   0,   0,  12,   0,
		  0, 128, 176,   0,   0,   0,
		 12,   0,   0,   0, 204,   0,
		  0,   0,  12,   0,   0, 128,
		204,   0,   0,   0,  12,   0,
		  0,   0, 224,   0,   0,   0,
		 12,   0,   0, 128, 224,   0,
		  0,   0,  12,   0,   0,   0,
		  0,   1,   0,   0,  12,   0,
		  0, 128,   0,   1,   0,   0,
		 12,   0,   0,   0,  36,   1,
		  0,   0,  12,   0,   0, 128,
		 36,   1,   0,   0,  12,   0,
		  0,   0,   2,   0,  25,   0,
		  2,   0,  24,   0,   2,   0,
		 25,   0,   2,   0,  24,   0,
		  2,   0,  25,   0,   2,   0,
		 24,   0,   2,   0,  26,   0,
		 13,   0,  24,   0,   2,   0,
		 26,   0,  13,   0,  24,   0,
		  2,   0,  26,   0,  13,   0,
		 24,   0,   2,   0,  26,   0,
		  9,   0,  25,   0,   2,   0,
		 26,   0,   9,   0,  25,   0,
		  2,   0,  26,   0,   9,   0,
		 25,   0,   2,   0,  26,   0,
		  2,   0,  26,   0, 246,   0,
		  0,   0,   4,   0,   0,   0,
		  0,   0,   0,   0,   8,   0,
		  0,   0,   0,   0,   0,   0,
		 20,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  11, 202,  49,   1,
		 56,   0,   0,   0,   0,  16,
		  0,   0,   8,  16,   0,   0,
		132,   0,   0,   0,  10,   0,
		255, 255,   4,   0,   0,   0,
		255, 255,   3,   0,   0,   0,
		  0,   0,  32,   0,   0,   0,
		 32,   0,   0,   0,   8,   0,
		  0,   0,  40,   0,   0,   0,
		  0,   0,   0,   0,  22,   0,
		 27,  21,  64,   0,   0,   0,
		  4,   0,   0,   0,  16,   0,
		102, 108, 111,  97, 116,  52,
		  0, 243, 242, 241,  22,   0,
		 27,  21,  64,   0,   0,   0,
		  2,   0,   0,   0,   8,   0,
		102, 108, 111,  97, 116,  50,
		  0, 243, 242, 241,  14,   0,
		  1,  18,   2,   0,   0,   0,
		  0,  16,   0,   0,   1,  16,
		  0,   0,  10,   0,  24,  21,
		  0,  16,   0,   0,   1,   0,
		  1,   0,  14,   0,   8,  16,
		  3,  16,   0,   0,  23,   0,
		  2,   0,   2,  16,   0,   0,
		 14,   0,  23,  21,   0,  16,
		  0,   0,   3,   2, 144,   2,
		  0,   0, 242, 241,  10,   0,
		 24,  21,   5,  16,   0,   0,
		  1,   0,   1,   0,  10,   0,
		 24,  21,   6,  16,   0,   0,
		  1,   0,   0,   2,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  11, 202,
		 49,   1,  56,   0,   0,   0,
		  0,  16,   0,   0,   0,  16,
		  0,   0,   0,   0,   0,   0,
		 11,   0, 255, 255,   4,   0,
		  0,   0, 255, 255,   3,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		255, 255, 255, 255,  26,   9,
		 47, 241,  16,   0,   0,   0,
		 12,   2,   0,   0,  21,   0,
		  0,   0,   1,   0,   0,   0,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  16,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  32,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 12,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,  18,   0,
		 37,  17,   0,   0,   0,   0,
		128,   0,   0,   0,   1,   0,
		109,  97, 105, 110,   0,   0,
		 22,   0,  81,  17,   7,  16,
		  0,   0,   7,   0, 255, 255,
		255, 255,   0,   0, 255, 255,
		255, 255, 116, 101, 120,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 16,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 255, 255,
		255, 255,  26,   9,  47, 241,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0, 255, 255, 255, 255,
		119,   9,  49,   1,   1,   0,
		  0,   0,  13,   0,   0, 142,
		 14,   0,  63,  92,  15,   0,
		  0,   0,  76,   0,   0,   0,
		 32,   0,   0,   0,  44,   0,
		  0,   0,  84,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  22,   0,   0,   0,
		 25,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   1,   0,
		  0,   0,   0,   0,   0,   0,
		 40,   1,   0,   0,  32,   0,
		  0,  96,   0,   0, 191, 175,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   2,   0,   9,   0,
		188,   2,   0,   0,   0,   0,
		  0,   0,  60,   1,   0,   0,
		  1,   0,   0,   0, 120, 200,
		204,   2,   0,   0,   0,   0,
		  0,   0,   0,   0, 109,  97,
		105, 110,   0, 110, 111, 110,
		101,   0,   0,   0,  45, 186,
		 46, 241,   1,   0,   0,   0,
		  0,   0,   0,   0,  40,   1,
		  0,   0,  32,   0,   0,  96,
		  0,   0, 191, 175,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  2,   0,   2,   0,   7,   0,
		  0,   0,   0,   0,   1,   0,
		255, 255, 255, 255,   0,   0,
		  0,   0,  40,   1,   0,   0,
		  8,   2,   0,   0,   0,   0,
		  0,   0, 255, 255, 255, 255,
		  0,   0,   0,   0, 255, 255,
		255, 255,   1,   0,   1,   0,
		  0,   0,   1,   0,   0,   0,
		  0,   0,  67,  58,  92,  85,
		115, 101, 114, 115,  92, 108,
		108, 101, 111, 110,  97, 114,
		116,  92,  68, 101, 115, 107,
		116, 111, 112,  92,  80, 104,
		 68,  32,  80, 114, 111, 106,
		101,  99, 116,  92,  67,  65,
		 52,  71,  92,  83, 104,  97,
		100, 101, 114, 115,  92,  68,
		114,  97, 119,  83,  99, 114,
		101, 101, 110,  95,  80,  83,
		 46, 104, 108, 115, 108,   0,
		  0,   0, 254, 239, 254, 239,
		  1,   0,   0,   0,   1,   0,
		  0,   0,   0,   1,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255,  12,   0, 255, 255, 255,
		255, 255, 255, 255, 255, 255,
		255,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0, 148,  46,
		 49,   1, 251, 103, 166,  92,
		  1,   0,   0,   0, 190, 221,
		  2,  60, 168,  64, 148,  64,
		179, 104,  16, 246, 117, 235,
		191, 175, 115,   0,   0,   0,
		 47,  76, 105, 110, 107,  73,
		110, 102, 111,   0,  47, 110,
		 97, 109, 101, 115,   0,  47,
		115, 114,  99,  47, 104, 101,
		 97, 100, 101, 114,  98, 108,
		111,  99, 107,   0,  47, 115,
		114,  99,  47, 102, 105, 108,
		101, 115,  47,  99,  58,  92,
		117, 115, 101, 114, 115,  92,
		108, 108, 101, 111, 110,  97,
		114, 116,  92, 100, 101, 115,
		107, 116, 111, 112,  92, 112,
		104, 100,  32, 112, 114, 111,
		106, 101,  99, 116,  92,  99,
		 97,  52, 103,  92, 115, 104,
		 97, 100, 101, 114, 115,  92,
		100, 114,  97, 119, 115,  99,
		114, 101, 101, 110,  95, 112,
		115,  46, 104, 108, 115, 108,
		  0,   4,   0,   0,   0,   6,
		  0,   0,   0,   1,   0,   0,
		  0,  58,   0,   0,   0,   0,
		  0,   0,   0,  17,   0,   0,
		  0,   7,   0,   0,   0,  10,
		  0,   0,   0,   6,   0,   0,
		  0,   0,   0,   0,   0,   5,
		  0,   0,   0,  34,   0,   0,
		  0,   8,   0,   0,   0,   0,
		  0,   0,   0, 220,  81,  51,
		  1,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		 16,   0,   0,   0,  32,   0,
		  0,   0, 207,   0,   0,   0,
		188,   0,   0,   0,  91,   1,
		  0,   0,  56,   0,   0,   0,
		  0,   0,   0,   0, 250,   1,
		  0,   0, 128,   0,   0,   0,
		 59,   1,   0,   0,   4,   4,
		  0,   0,  40,   0,   0,   0,
		  0,   0,   0,   0,  40,   0,
		  0,   0,  44,   2,   0,   0,
		 44,   0,   0,   0,  44,   0,
		  0,   0,   3,   0,   0,   0,
		 21,   0,   0,   0,  14,   0,
		  0,   0,  20,   0,   0,   0,
		 15,   0,   0,   0,   9,   0,
		  0,   0,  10,   0,   0,   0,
		  8,   0,   0,   0,  11,   0,
		  0,   0,  12,   0,   0,   0,
		 13,   0,   0,   0,   7,   0,
		  0,   0,   6,   0,   0,   0,
		 16,   0,   0,   0,  17,   0,
		  0,   0,  19,   0,   0,   0,
		 18,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,  22,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0
	};



#pragma endregion

	class ShaderLoader {

	public:
		static D3D12_SHADER_BYTECODE DrawScreenVS() {
			return FromMemory(cso_DrawScreen_VS);
		}

		static D3D12_SHADER_BYTECODE DrawTexturePS() {
			return FromMemory(cso_DrawScreen_PS);
		}

		static D3D12_SHADER_BYTECODE DrawComplexityPS() {
			return FromMemory(cso_DrawComplexity_PS);
		}

		// Use this method to load a bytecode
		static D3D12_SHADER_BYTECODE FromFile(const char* bytecodeFilePath);

		// Use this method to load a bytecode from a memory buffer
		static D3D12_SHADER_BYTECODE FromMemory(const byte* bytecodeData, int count);

		// Use this method to load a bytecode from a memory buffer
		template<int count>
		static D3D12_SHADER_BYTECODE FromMemory(const byte(&bytecodeData)[count])
		{
			return FromMemory(bytecodeData, count);
		}
	};

	// Represents the shader stage a resource is bound to.
	enum class ShaderType : int {
		// Resource is bound to pixel and vertex stages in the same slot.
		Any = D3D12_SHADER_VISIBILITY_ALL,
		// Resource is bound to vertex stage.
		Vertex = D3D12_SHADER_VISIBILITY_VERTEX,
		// Resource is bound to geometry stage.
		Geometry = D3D12_SHADER_VISIBILITY_GEOMETRY,
		// Resource is bound to pixel stage.
		Pixel = D3D12_SHADER_VISIBILITY_PIXEL,
		// Resource is bound to hull stage.
		Hull = D3D12_SHADER_VISIBILITY_HULL,
		// Resource is bound to domain stage.
		Domain = D3D12_SHADER_VISIBILITY_DOMAIN
	};

	// Represents the element type of a vertex field.
	enum class VertexElementType {
		// Each component of this field is a signed integer
		Int = 0,
		// Each component of this field is an unsigned integer
		UInt = 1,
		// Each component of this field is a floating value
		Float = 2
	};

#pragma region Vertex Description

	static DXGI_FORMAT TYPE_VS_COMPONENTS_FORMATS[3][4]{
		/*Int*/{ DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32B32_SINT,DXGI_FORMAT_R32G32B32A32_SINT },
		/*Unt*/{ DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32B32_UINT,DXGI_FORMAT_R32G32B32A32_UINT },
		/*Float*/{ DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT },
	};

	enum class VertexElementSemantic
	{
		Custom,
		Position,
		Normal,
		TexCoord,
		Tangent,
		Binormal,
		Color
	};

	constexpr static LPCSTR GetSemanticText(VertexElementSemantic s) {
		switch (s) {
		case VertexElementSemantic::Position:
			return "POSITION";
		case VertexElementSemantic::Normal:
			return "NORMAL";
		case VertexElementSemantic::TexCoord:
			return "TEXCOORD";
		case VertexElementSemantic::Tangent:
			return "TANGENT";
		case VertexElementSemantic::Binormal:
			return "BINORMAL";
		case VertexElementSemantic::Color:
			return "COLOR";
		}
		throw Exception::FromError(Errors::Invalid_Operation, "Standard semantic can not be custom.");
	}

	// Basic struct for constructing vertex element descriptions
	struct VertexElement {
		// Type for each field component
		const VertexElementType Type;
		// Number of components
		const int Components;

		VertexElementSemantic const Semantic;
		// String with the semantic
		LPCSTR const SemanticText;
		// Index for indexed semantics
		const int SemanticIndex;
		// Buffer slot this field will be contained.
		const int Slot;
	public:

		constexpr VertexElement(
			VertexElementType Type,
			int Components,
			VertexElementSemantic standardSemantic,
			int SemanticIndex = 0,
			int Slot = 0
		) :Type(Type), Components(Components), SemanticText(GetSemanticText(standardSemantic)), SemanticIndex(SemanticIndex), Slot(Slot),
			Semantic(standardSemantic)
		{
		}
		constexpr VertexElement(
			VertexElementType Type,
			int Components,
			LPCSTR const customSemantic,
			int SemanticIndex = 0,
			int Slot = 0
		) : Type(Type), Components(Components), SemanticText(customSemantic), SemanticIndex(SemanticIndex), Slot(Slot),
			Semantic(VertexElementSemantic::Custom)
		{
		}

		constexpr DXGI_FORMAT Format() const {
			return TYPE_VS_COMPONENTS_FORMATS[(int)Type][Components - 1];
		}

		// Creates a Dx12 description using this information.
		D3D12_INPUT_ELEMENT_DESC createDesc(int offset, int& size) const {
			D3D12_INPUT_ELEMENT_DESC d = {};
			d.AlignedByteOffset = offset;
			d.Format = TYPE_VS_COMPONENTS_FORMATS[(int)Type][Components - 1];
			d.InputSlot = Slot;
			d.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			d.SemanticIndex = SemanticIndex;
			d.SemanticName = SemanticText;
			size += 4 * Components;
			return d;
		}
	};

#pragma endregion

#pragma region Shader handles

	class ProgramHandle {
		friend RaytracingPipeline;
		friend RaytracingProgramBase;
		friend wProgram;

		LPCWSTR shaderHandle;
		void* cachedShaderIdentifier = nullptr;
	protected:
		ProgramHandle(LPCWSTR handle) :shaderHandle(handle) {}
	public:
		ProgramHandle() :shaderHandle(nullptr) {}
		ProgramHandle(const ProgramHandle& other) {
			this->shaderHandle = other.shaderHandle;
		}
		inline bool IsNull() { return shaderHandle == nullptr; }
	};

	class MissHandle : public ProgramHandle {
		friend RaytracingPipeline;
		friend RaytracingProgramBase;

		MissHandle(LPCWSTR shaderHandle) : ProgramHandle(shaderHandle) { }
	public:
		MissHandle() : ProgramHandle() {}
		MissHandle(const MissHandle& other) : ProgramHandle(other) { }
	};

	class RayGenerationHandle : public ProgramHandle {
		friend RaytracingPipeline;
		friend RaytracingProgramBase;

		RayGenerationHandle(LPCWSTR shaderHandle) : ProgramHandle(shaderHandle) {}
	public:
		RayGenerationHandle() : ProgramHandle() { }
		RayGenerationHandle(const RayGenerationHandle& other) : ProgramHandle(other) { }
	};

	class AnyHitHandle : public ProgramHandle {
		friend RaytracingPipeline;
		friend RaytracingProgramBase;

		AnyHitHandle(LPCWSTR shaderHandle) : ProgramHandle(shaderHandle) {}
	public:
		AnyHitHandle() : ProgramHandle() { }
		AnyHitHandle(const AnyHitHandle& other) : ProgramHandle(other) { }
	};

	class ClosestHitHandle : public ProgramHandle {
		friend RaytracingPipeline;
		friend RaytracingProgramBase;

		ClosestHitHandle(LPCWSTR shaderHandle) : ProgramHandle(shaderHandle) {}
	public:
		ClosestHitHandle() : ProgramHandle() { }
		ClosestHitHandle(const ClosestHitHandle& other) : ProgramHandle(other) { }
	};

	class IntersectionHandle : public ProgramHandle {
		friend RaytracingPipeline;
		friend RaytracingProgramBase;

		IntersectionHandle(LPCWSTR shaderHandle) : ProgramHandle(shaderHandle) {}
	public:
		IntersectionHandle() : ProgramHandle() { }
		IntersectionHandle(const IntersectionHandle& other) : ProgramHandle(other) { }
	};

	class HitGroupHandle : public ProgramHandle {
		friend RaytracingPipeline;
		friend RaytracingProgramBase;

		static LPCWSTR CreateAutogeneratedName() {
			static int ID = 0;

			wchar_t* label = new wchar_t[100];
			label[0] = 0;
			wcscat_s(label, 100, TEXT("HITGROUP"));
			wsprintf(&label[8], L"%d",
				ID);
			ID++;

			return label;
		}

		gObj<AnyHitHandle> anyHit;
		gObj<ClosestHitHandle> closestHit;
		gObj<IntersectionHandle> intersection;

		HitGroupHandle(gObj<ClosestHitHandle> closestHit, gObj<AnyHitHandle> anyHit, gObj<IntersectionHandle> intersection)
			: ProgramHandle(CreateAutogeneratedName()), anyHit(anyHit), closestHit(closestHit), intersection(intersection) {}
	public:
		HitGroupHandle() :ProgramHandle() {}
		HitGroupHandle(const HitGroupHandle& other) :ProgramHandle(other) {
			this->anyHit = other.anyHit;
			this->closestHit = other.closestHit;
			this->intersection = other.intersection;
		}
	};

#pragma endregion

	/// <summary>
	/// Represents a base class for all types of pipeline settings objects.
	/// Compute and Graphics pipelines will extend StaticPipeline and
	/// Raytracing pipeline will extend DynamicPipeline.
	/// </summary>
	class Pipeline {
		friend DeviceManager;
		friend ComputeManager;
		friend GraphicsManager;
		friend RaytracingManager;
	protected:
		virtual Engine GetEngine() = 0;
		virtual void OnCreate(wDevice* w_device) = 0;
		virtual void OnSet(wCmdList* w_cmdList) = 0;
		virtual void OnDispatch(wCmdList* w_cmdList) = 0;
	};

#pragma region Binders

	class ComputeBinder {
		friend StaticPipelineBase;
		friend RaytracingPipeline;
		friend wPipelineState;
		friend wProgram;
		friend RaytracingProgramBase;
		friend RaytracingManager;
		friend class DX_RTProgram;

		void CreateSignature(
			DX_Device device,
			D3D12_ROOT_SIGNATURE_FLAGS flags,
			DX_RootSignature& rootSignature,
			int& rootSignatureSize
		);

	protected:
		wBindings* __InternalBindingObject;
	
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
		int space = 0;
		bool collectGlobal = true;

		void AddConstant(int slot, void* data, int size);

		void AddDescriptorRange(int slot, D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_RESOURCE_DIMENSION dimension, void* resource);

		void AddDescriptorRange(int initialSlot, D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_RESOURCE_DIMENSION dimension, void* resourceArray, int* count);

		void AddStaticSampler(int slot, const Sampler& sampler);
	
	public:
		ComputeBinder();
		virtual ~ComputeBinder() {
			delete __InternalBindingObject;
		}

		/// <summary>
		/// Specifies next bindings are activated when the pipeline is set.
		/// </summary>
		void Bindings_OnSet() {
			this->collectGlobal = true;
		}

		/// <summary>
		/// Specifies next bindings are activated when dispatching
		/// </summary>
		void Bindings_OnDispatch() {
			this->collectGlobal = false;
		}

		// Change the space for the next bindings
		void Space(int space) { this->space = space; }

		void CBV(int slot, gObj<Buffer>& const buffer)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
				D3D12_RESOURCE_DIMENSION_BUFFER,
				(void*)&buffer);
		}

		template<typename T>
		void CBV(int slot, T& data) {
			AddConstant(slot, (void*)&data, ((sizeof(T) - 1) / 4) + 1);
		}

		void SMP_Static(int slot, const Sampler& sampler) {
			AddStaticSampler(slot, sampler);
		}

		void SMP(int slot, gObj<Sampler>& const sampler) {
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
				D3D12_RESOURCE_DIMENSION_UNKNOWN,
				(void*)&sampler);
		}

		void SRV(int slot, gObj<Buffer>& const buffer)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				D3D12_RESOURCE_DIMENSION_BUFFER,
				(void*)&buffer);
		}
		void SRV(int slot, gObj<Texture1D>& const texture)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				D3D12_RESOURCE_DIMENSION_TEXTURE1D,
				(void*)&texture);
		}
		void SRV(int slot, gObj<Texture2D>& const texture)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				(void*)&texture);
		}
		void SRV(int slot, gObj<Texture3D>& const texture)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				D3D12_RESOURCE_DIMENSION_TEXTURE3D,
				(void*)&texture);
		}

		void UAV(int slot, gObj<Buffer>& const buffer)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				D3D12_RESOURCE_DIMENSION_BUFFER,
				(void*)&buffer);
		}
		void UAV(int slot, gObj<Texture1D>& const texture)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				D3D12_RESOURCE_DIMENSION_TEXTURE1D,
				(void*)&texture);
		}
		void UAV(int slot, gObj<Texture2D>& const texture)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				(void*)&texture);
		}
		void UAV(int slot, gObj<Texture3D>& const texture)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				D3D12_RESOURCE_DIMENSION_TEXTURE3D,
				(void*)&texture);
		}

		void SMP_Array(int slot, gObj<Sampler>*& const samplers, int& const count) {
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
				D3D12_RESOURCE_DIMENSION_UNKNOWN,
				(void*)&samplers, &count);
		}

		void SRV_Array(int slot, gObj<Buffer>*& const buffers, int& const count)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				D3D12_RESOURCE_DIMENSION_BUFFER,
				(void*)&buffers, &count);
		}
		void SRV_Array(int slot, gObj<Texture1D>*& const textures, int& const count)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				D3D12_RESOURCE_DIMENSION_TEXTURE1D,
				(void*)&textures, &count);
		}
		void SRV_Array(int slot, gObj<Texture2D>*& const textures, int& const count)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				(void*)&textures, &count);
		}
		void SRV_Array(int slot, gObj<Texture3D>*& const textures, int& const count)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				D3D12_RESOURCE_DIMENSION_TEXTURE3D,
				(void*)&textures, &count);
		}

		void UAV_Array(int slot, gObj<Buffer>*& const buffers, int& const count)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				D3D12_RESOURCE_DIMENSION_BUFFER,
				(void*)&buffers, &count);
		}
		void UAV_Array(int slot, gObj<Texture1D>*& const textures, int& const count)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				D3D12_RESOURCE_DIMENSION_TEXTURE1D,
				(void*)&textures, &count);
		}
		void UAV_Array(int slot, gObj<Texture2D>*& const textures, int& const count)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				(void*)&textures, &count);
		}
		void UAV_Array(int slot, gObj<Texture3D>*& const textures, int& const count)
		{
			AddDescriptorRange(slot, D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				D3D12_RESOURCE_DIMENSION_TEXTURE3D,
				(void*)&textures, &count);
		}

		bool HasSomeBindings();
	};

	class GraphicsBinder : public ComputeBinder {
	protected:
		void SetRenderTarget(int slot, gObj<Texture2D>& const resource);
		void SetDSV(gObj<Texture2D>& const resource);
	public:
		GraphicsBinder();

		void Bindings_All() {
			this->visibility = D3D12_SHADER_VISIBILITY_ALL;
		}

		void Bindings_VertexShader() {
			this->visibility = D3D12_SHADER_VISIBILITY_VERTEX;
		}

		void Bindings_PixelShader() {
			this->visibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}

		void Bindings_GeometryShader() {
			this->visibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
		}

		void Bindings_HullShader() {
			this->visibility = D3D12_SHADER_VISIBILITY_HULL;
		}

		void Bindings_DomainShader() {
			this->visibility = D3D12_SHADER_VISIBILITY_DOMAIN;
		}

		void RTV(int slot, gObj<Texture2D>& const texture) {
			SetRenderTarget(slot, texture);
		}

		void DSV(gObj<Texture2D>& const texture) {
			SetDSV(texture);
		}
	};

	class RaytracingBinder : public ComputeBinder {
	protected:
	public:
		RaytracingBinder();
		void ADS(int slot, gObj<InstanceCollection>& const scene);
	};

#pragma endregion

#pragma region Static Pipelines

#pragma region Pipeline Subobject states

	struct DefaultStateValue {

		operator D3D12_RASTERIZER_DESC () {
			D3D12_RASTERIZER_DESC d = {};
			d.FillMode = D3D12_FILL_MODE_SOLID;
			d.CullMode = D3D12_CULL_MODE_NONE;
			d.FrontCounterClockwise = FALSE;
			d.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			d.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			d.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			d.DepthClipEnable = TRUE;
			d.MultisampleEnable = FALSE;
			d.AntialiasedLineEnable = FALSE;
			d.ForcedSampleCount = 0;
			d.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
			return d;
		}

		operator D3D12_DEPTH_STENCIL_DESC () {
			D3D12_DEPTH_STENCIL_DESC d = { };

			d.DepthEnable = FALSE;
			d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			d.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			d.StencilEnable = FALSE;
			d.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
			d.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
			const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
			{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
			d.FrontFace = defaultStencilOp;
			d.BackFace = defaultStencilOp;

			return d;
		}

		operator D3D12_BLEND_DESC () {
			D3D12_BLEND_DESC d = { };
			d.AlphaToCoverageEnable = FALSE;
			d.IndependentBlendEnable = FALSE;
			const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
			{
				FALSE,FALSE,
				D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
				D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL,
			};
			for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
				d.RenderTarget[i] = defaultRenderTargetBlendDesc;
			return d;
		}

		operator DXGI_SAMPLE_DESC() {
			DXGI_SAMPLE_DESC d = { };
			d.Count = 1;
			d.Quality = 0;
			return d;
		}

		operator D3D12_DEPTH_STENCIL_DESC1 () {
			D3D12_DEPTH_STENCIL_DESC1 _Description = { };
			_Description.DepthEnable = TRUE;
			_Description.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			_Description.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			_Description.StencilEnable = FALSE;
			_Description.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
			_Description.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
			const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
			{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
			_Description.FrontFace = defaultStencilOp;
			_Description.BackFace = defaultStencilOp;
			_Description.DepthBoundsTestEnable = false;
			return _Description;
		}
	};

	struct DefaultSampleMask {
		operator UINT()
		{
			return UINT_MAX;
		}
	};

	struct DefaultTopology {
		operator D3D12_PRIMITIVE_TOPOLOGY_TYPE() {
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		}
	};

	// Adapted from d3dx12 class
	template<typename Description, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type, typename DefaultValue = Description>
	struct alignas(void*) PipelineSubobjectState {
		template<typename ...A> friend class StaticPipeline;

		D3D12_PIPELINE_STATE_SUBOBJECT_TYPE _Type;
		Description _Description;

		static D3D12_PIPELINE_STATE_SUBOBJECT_TYPE getType() { return Type; }

		inline Description& getDescription() {
			return _Description;
		}

		PipelineSubobjectState() noexcept : _Type(Type), _Description(DefaultValue()) {}
		PipelineSubobjectState(Description const& d) : _Type(Type), _Description(d) {}
		PipelineSubobjectState& operator=(Description const& i) { _Description = i; return *this; }
		operator Description() const { return _Description; }
		operator Description& () { return _Description; }

		static const D3D12_PIPELINE_STATE_SUBOBJECT_TYPE PipelineState_Type = Type;
	};

	struct DebugStateManager : public PipelineSubobjectState< D3D12_PIPELINE_STATE_FLAGS, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS> {
		void Debug() { _Description = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG; }
		void NoDebug() { _Description = D3D12_PIPELINE_STATE_FLAG_NONE; }
	};

	struct NodeMaskStateManager : public PipelineSubobjectState< UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK> {
		void ExecutionAt(int node) {
			_Description = 1 << node;
		}
		void ExecutionSingleAdapter() {
			_Description = 0;
		}
	};

	struct RootSignatureStateManager : public PipelineSubobjectState< ID3D12RootSignature*, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE> {
		template<typename ...PSS> friend class StaticPipeline;
	private:
		void SetRootSignature(ID3D12RootSignature* rootSignature) {
			_Description = rootSignature;
		}
	};

	struct InputLayoutStateManager : public PipelineSubobjectState< D3D12_INPUT_LAYOUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT> {
		void InputLayout(std::initializer_list<VertexElement> elements) {
			if (_Description.pInputElementDescs != nullptr)
				delete[] _Description.pInputElementDescs;
			auto layout = new D3D12_INPUT_ELEMENT_DESC[elements.size()];
			int offset = 0;
			auto current = elements.begin();
			for (int i = 0; i < elements.size(); i++)
			{
				int size = 0;
				layout[i] = current->createDesc(offset, size);
				offset += size;
				current++;
			}
			_Description.pInputElementDescs = layout;
			_Description.NumElements = elements.size();
		}

		template<int N>
		void InputLayout(VertexElement(&elements)[N]) {
			if (_Description.pInputElementDescs != nullptr)
				delete[] _Description.pInputElementDescs;
			auto layout = new D3D12_INPUT_ELEMENT_DESC[N];
			int offset = 0;
			for (int i = 0; i < N; i++)
			{
				int size = 0;
				layout[i] = elements[i]->createDesc(offset, size);
				offset += size;
			}
			_Description.pInputElementDescs = layout;
			_Description.NumElements = N;
		}
	};

	struct IndexBufferStripStateManager : public PipelineSubobjectState< D3D12_INDEX_BUFFER_STRIP_CUT_VALUE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE> {
		void DisableIndexBufferCut() {
			_Description = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		}
		void IndexBufferCutAt32BitMAX() {
			_Description = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF;
		}
		void IndexBufferCutAt64BitMAX() {
			_Description = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
		}
	};

	struct PrimitiveTopologyStateManager : public PipelineSubobjectState< D3D12_PRIMITIVE_TOPOLOGY_TYPE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY, DefaultTopology> {
		void CustomTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type) {
			_Description = type;
		}
		void TrianglesTopology() {
			CustomTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}
		void PointsTopology() {
			CustomTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
		}
		void LinesTopology() {
			CustomTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
		}
	};

	template<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type>
	struct ShaderStageStateManager : public PipelineSubobjectState< D3D12_SHADER_BYTECODE, Type> {
	protected:
		void FromBytecode(const D3D12_SHADER_BYTECODE& shaderBytecode) {
			this->_Description = shaderBytecode;
		}
	};

	struct VertexShaderStageStateManager : public ShaderStageStateManager <D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS> {
		void VertexShader(const D3D12_SHADER_BYTECODE& bytecode) {
			ShaderStageStateManager::FromBytecode(bytecode);
		}
	};

	struct PixelShaderStageStateManager : public ShaderStageStateManager <D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS> {
		void PixelShader(const D3D12_SHADER_BYTECODE& bytecode) {
			ShaderStageStateManager::FromBytecode(bytecode);
		}
	};

	struct GeometryShaderStageStateManager : public ShaderStageStateManager <D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS> {
		void GeometryShader(const D3D12_SHADER_BYTECODE& bytecode) {
			ShaderStageStateManager::FromBytecode(bytecode);
		}
	};

	struct HullShaderStageStateManager : public ShaderStageStateManager <D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS> {
		void HullShader(const D3D12_SHADER_BYTECODE& bytecode) {
			ShaderStageStateManager::FromBytecode(bytecode);
		}
	};

	struct DomainShaderStageStateManager : public ShaderStageStateManager <D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS> {
		void DomainShader(const D3D12_SHADER_BYTECODE& bytecode) {
			ShaderStageStateManager::FromBytecode(bytecode);
		}
	};

	struct ComputeShaderStageStateManager : public ShaderStageStateManager <D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS> {
		void ComputeShader(const D3D12_SHADER_BYTECODE& bytecode) {
			ShaderStageStateManager::FromBytecode(bytecode);
		}
	};

	struct StreamOutputStateManager : public PipelineSubobjectState< D3D12_STREAM_OUTPUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT> {
	};

	struct BlendingStateManager : public PipelineSubobjectState< D3D12_BLEND_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND, DefaultStateValue> {
		void AlphaToCoverage(bool enable = true) {
			_Description.AlphaToCoverageEnable = enable;
		}
		void IndependentBlend(bool enable = true) {
			_Description.IndependentBlendEnable = enable;
		}
		void BlendAtRenderTarget(
			int renderTarget = 0,
			bool enable = true,
			D3D12_BLEND_OP operation = D3D12_BLEND_OP_ADD,
			D3D12_BLEND src = D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND dst = D3D12_BLEND_INV_SRC_ALPHA,
			D3D12_BLEND_OP alphaOperation = D3D12_BLEND_OP_MAX,
			D3D12_BLEND srcAlpha = D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND dstAlpha = D3D12_BLEND_DEST_ALPHA,
			bool enableLogicOperation = false,
			D3D12_LOGIC_OP logicOperation = D3D12_LOGIC_OP_COPY
		) {
			D3D12_RENDER_TARGET_BLEND_DESC d;
			d.BlendEnable = enable;
			d.BlendOp = operation;
			d.BlendOpAlpha = alphaOperation;
			d.SrcBlend = src;
			d.DestBlend = dst;
			d.SrcBlendAlpha = srcAlpha;
			d.DestBlendAlpha = dstAlpha;
			d.LogicOpEnable = enableLogicOperation;
			d.LogicOp = logicOperation;
			d.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			_Description.RenderTarget[renderTarget] = d;
		}
		void BlendForAllRenderTargets(
			bool enable = true,
			D3D12_BLEND_OP operation = D3D12_BLEND_OP_ADD,
			D3D12_BLEND src = D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND dst = D3D12_BLEND_INV_SRC_ALPHA,
			D3D12_BLEND_OP alphaOperation = D3D12_BLEND_OP_MAX,
			D3D12_BLEND srcAlpha = D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND dstAlpha = D3D12_BLEND_DEST_ALPHA,
			bool enableLogicOperation = false,
			D3D12_LOGIC_OP logicOperation = D3D12_LOGIC_OP_COPY
		) {
			D3D12_RENDER_TARGET_BLEND_DESC d;
			d.BlendEnable = enable;
			d.BlendOp = operation;
			d.BlendOpAlpha = alphaOperation;
			d.SrcBlend = src;
			d.DestBlend = dst;
			d.SrcBlendAlpha = srcAlpha;
			d.DestBlendAlpha = dstAlpha;
			d.LogicOpEnable = enableLogicOperation;
			d.LogicOp = logicOperation;
			d.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
				_Description.RenderTarget[i] = d;
		}
		void BlendDisabledAtRenderTarget(int renderTarget) {
			BlendAtRenderTarget(renderTarget, false);
		}
		void BlendDisabled() {
			BlendForAllRenderTargets(false);
		}
	};

	struct DepthStencilStateManager : public PipelineSubobjectState< D3D12_DEPTH_STENCIL_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL, DefaultStateValue> {
		void NoDepthTest() {
			_Description = {};
		}
		void DepthTest(bool enable = true, bool writeDepth = true, D3D12_COMPARISON_FUNC comparison = D3D12_COMPARISON_FUNC_LESS_EQUAL) {
			_Description.DepthEnable = enable;
			_Description.DepthWriteMask = writeDepth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			_Description.DepthFunc = comparison;
		}
		void StencilTest(bool enable = true, byte readMask = 0xFF, byte writeMask = 0xFF) {
			_Description.StencilEnable = enable;
			_Description.StencilReadMask = readMask;
			_Description.StencilWriteMask = writeMask;
		}
		void StencilOperationAtFront(D3D12_STENCIL_OP fail = D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP depthFail = D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP pass = D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC function = D3D12_COMPARISON_FUNC_ALWAYS) {
			_Description.FrontFace.StencilFailOp = fail;
			_Description.FrontFace.StencilDepthFailOp = depthFail;
			_Description.FrontFace.StencilPassOp = pass;
			_Description.FrontFace.StencilFunc = function;
		}
		void StencilOperationAtBack(D3D12_STENCIL_OP fail = D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP depthFail = D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP pass = D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC function = D3D12_COMPARISON_FUNC_ALWAYS) {
			_Description.BackFace.StencilFailOp = fail;
			_Description.BackFace.StencilDepthFailOp = depthFail;
			_Description.BackFace.StencilPassOp = pass;
			_Description.BackFace.StencilFunc = function;
		}
	};

	struct DepthStencilWithDepthBoundsStateManager : public PipelineSubobjectState< D3D12_DEPTH_STENCIL_DESC1, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1, DefaultStateValue> {
		void DepthTest(bool enable = true, bool writeDepth = true, D3D12_COMPARISON_FUNC comparison = D3D12_COMPARISON_FUNC_LESS_EQUAL) {
			_Description.DepthEnable = enable;
			_Description.DepthWriteMask = writeDepth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			_Description.DepthFunc = comparison;
		}
		void StencilTest(bool enable = true, byte readMask = 0xFF, byte writeMask = 0xFF) {
			_Description.StencilEnable = enable;
			_Description.StencilReadMask = readMask;
			_Description.StencilWriteMask = writeMask;
		}
		void StencilOperationAtFront(D3D12_STENCIL_OP fail = D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP depthFail = D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP pass = D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC function = D3D12_COMPARISON_FUNC_ALWAYS) {
			_Description.FrontFace.StencilFailOp = fail;
			_Description.FrontFace.StencilDepthFailOp = depthFail;
			_Description.FrontFace.StencilPassOp = pass;
			_Description.FrontFace.StencilFunc = function;
		}
		void StencilOperationAtBack(D3D12_STENCIL_OP fail = D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP depthFail = D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP pass = D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC function = D3D12_COMPARISON_FUNC_ALWAYS) {
			_Description.BackFace.StencilFailOp = fail;
			_Description.BackFace.StencilDepthFailOp = depthFail;
			_Description.BackFace.StencilPassOp = pass;
			_Description.BackFace.StencilFunc = function;
		}
		void DepthBoundsTest(bool enable) {
			_Description.DepthBoundsTestEnable = enable;
		}
	};

	struct DepthStencilFormatStateManager : public PipelineSubobjectState< DXGI_FORMAT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT> {
		void DepthStencilFormat(DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT) {
			_Description = format;
		}
	};

	struct RasterizerStateManager : public PipelineSubobjectState< D3D12_RASTERIZER_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER, DefaultStateValue> {
		void AntialiasedLine(bool enable = true, bool multisample = true) {
			_Description.AntialiasedLineEnable = false;
			_Description.MultisampleEnable = multisample;
		}
		void ConservativeRasterization(bool enable = true) {
			_Description.ConservativeRaster = enable ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		}
		void CullMode(D3D12_CULL_MODE mode = D3D12_CULL_MODE_NONE) {
			_Description.CullMode = mode;
		}
		void FillMode(D3D12_FILL_MODE mode = D3D12_FILL_MODE_SOLID) {
			_Description.FillMode = mode;
		}
		void DepthBias(int depthBias = 1, float slopeScale = 0, float clamp = D3D12_FLOAT32_MAX)
		{
			_Description.DepthBias = depthBias;
			_Description.SlopeScaledDepthBias = slopeScale;
			_Description.DepthBiasClamp = clamp;
		}
		void ForcedSampleCount(UINT value) {
			_Description.ForcedSampleCount = value;
		}
	};

	struct RenderTargetFormatsStateManager : public PipelineSubobjectState< D3D12_RT_FORMAT_ARRAY, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS> {
		void RenderTargetFormatAt(int slot, DXGI_FORMAT format) {
			if (slot >= _Description.NumRenderTargets)
				_Description.NumRenderTargets = slot + 1;

			_Description.RTFormats[slot] = format;
		}
		void AllRenderTargets(int numberOfRTs, DXGI_FORMAT format) {
			_Description.NumRenderTargets = numberOfRTs;
			for (int i = 0; i < numberOfRTs; i++)
				_Description.RTFormats[i] = format;
		}
	};

	struct MultisamplingStateManager : public PipelineSubobjectState< DXGI_SAMPLE_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC, DefaultStateValue> {
		void Multisampling(int count = 1, int quality = 0) {
			_Description.Count = count;
			_Description.Quality = quality;
		}
	};

	struct BlendSampleMaskStateManager : public PipelineSubobjectState< UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK, DefaultSampleMask> {
		void BlendSampleMask(UINT value = UINT_MAX) {
			_Description = value;
		}
	};

	struct MultiViewInstancingStateManager : public PipelineSubobjectState< D3D12_VIEW_INSTANCING_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING, DefaultStateValue> {
		void MultipleViews(std::initializer_list<D3D12_VIEW_INSTANCE_LOCATION> views, D3D12_VIEW_INSTANCING_FLAGS flags = D3D12_VIEW_INSTANCING_FLAG_NONE) {
			_Description.ViewInstanceCount = views.size();
			auto newViews = new D3D12_VIEW_INSTANCE_LOCATION[views.size()];
			auto current = views.begin();
			for (int i = 0; i < views.size(); i++) {
				newViews[i] = *current;
				current++;
			}
			if (_Description.pViewInstanceLocations != nullptr)
				delete[] _Description.pViewInstanceLocations;
			_Description.pViewInstanceLocations = newViews;
		}
	};

	//typedef PipelineSubobjectState< D3D12_CACHED_PIPELINE_STATE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO>                        CD3DX12_PIPELINE_STATE_STREAM_CACHED_PSO;

#pragma endregion

	class StaticPipelineBase : public Pipeline {
		friend DeviceManager;
	protected:

		wPipelineState* w_pipeline;

		virtual void* getSettingObject() = 0;

		virtual int getSettingObjectSize() = 0;

		virtual D3D12_ROOT_SIGNATURE_FLAGS getRootFlags() {
			return D3D12_ROOT_SIGNATURE_FLAG_NONE;
		}

		virtual void CompleteStateObject(DX_RootSignature rootSignature) = 0;

		virtual void OnCreate(wDevice* w_device) override;

		virtual void OnSet(wCmdList* w_cmdList) override;

		virtual void OnDispatch(wCmdList* w_cmdList) override;

		virtual gObj<ComputeBinder> OnCollectBindings() = 0;

		// When implemented by users, this method will setup the pipeline object after created
		// Use this method to specify how to load shaders and set other default pipeline settings
		virtual void Setup() = 0;
	};

	// -- Abstract pipeline object (can be Graphics or Compute)
	// Allows creation of root signatures and leaves abstract the pipeline state object creation
	template<typename ...PSS>
	class StaticPipeline : public StaticPipelineBase {

		void* getSettingObject() { return set; }

		int getSettingObjectSize() { return sizeof(*set); }

		template<typename S>
		bool HasSubobjectState() {
			return ((S*)set)->_Type == S::getType();
		}

#pragma region Query to know if some stage is active
		bool Using_VS() {
			return HasSubobjectState<VertexShaderStageStateManager>()
				&& ((VertexShaderStageStateManager*)set)->getDescription().pShaderBytecode != nullptr;
		}
		bool Using_PS() {
			return HasSubobjectState<PixelShaderStageStateManager>()
				&& ((PixelShaderStageStateManager*)set)->getDescription().pShaderBytecode != nullptr;
		}
		bool Using_GS() {
			return HasSubobjectState<GeometryShaderStageStateManager>()
				&& ((GeometryShaderStageStateManager*)set)->getDescription().pShaderBytecode != nullptr;
		}
		bool Using_HS() {
			return HasSubobjectState<HullShaderStageStateManager>()
				&& ((HullShaderStageStateManager*)set)->getDescription().pShaderBytecode != nullptr;
		}
		bool Using_DS() {
			return HasSubobjectState<DomainShaderStageStateManager>()
				&& ((DomainShaderStageStateManager*)set)->getDescription().pShaderBytecode != nullptr;
		}
		bool Using_CS() {
			return HasSubobjectState<ComputeShaderStageStateManager>()
				&& ((ComputeShaderStageStateManager*)set)->_Description.pShaderBytecode != nullptr;
		}

		Engine GetEngine() {
			if (Using_CS())
				return Engine::Compute;
			return Engine::Direct;
		}

		D3D12_ROOT_SIGNATURE_FLAGS getRootFlags() {
			auto flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
			if (!Using_CS()) flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			if (!Using_VS()) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
			if (!Using_PS()) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
			if (!Using_GS()) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
			if (!Using_HS()) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
			if (!Using_DS()) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			return flags;
		}

		void CompleteStateObject(DX_RootSignature rootSignature) {
			if (HasSubobjectState<RootSignatureStateManager>())
			{
				((RootSignatureStateManager*)set)->SetRootSignature(rootSignature);
			}
			if (HasSubobjectState<RenderTargetFormatsStateManager>())
			{
				RenderTargetFormatsStateManager* rtfsm = (RenderTargetFormatsStateManager*)set;
				if (rtfsm->_Description.NumRenderTargets == 0)
					rtfsm->AllRenderTargets(8, DXGI_FORMAT_R8G8B8A8_UNORM);
			}
			if (HasSubobjectState<DepthStencilFormatStateManager>())
			{
				DepthStencilFormatStateManager* dsfsm = (DepthStencilFormatStateManager*)set;
				dsfsm->_Description = DXGI_FORMAT_D32_FLOAT;
			}
		}

#pragma endregion
	protected:

		// Object used to set all possible states of this pipeline
		// inheriting from different state managers.
		// This object is the pipeline settings object in DX12.
		struct PipelineStateStreamDescription : PSS...{
		}* const set;

		StaticPipeline() : set(new PipelineStateStreamDescription()) {
		}

	};

	// Used as a common compute pipeline binding object
	struct ComputePipeline : public StaticPipeline<
		DebugStateManager,
		ComputeShaderStageStateManager,
		NodeMaskStateManager,
		RootSignatureStateManager
	> {

	private:
		gObj<ComputeBinder> OnCollectBindings() {
			gObj<ComputeBinder> binder = new ComputeBinder();
			Bindings(binder);
			return binder;
		}

	protected:
		virtual void Bindings(gObj<ComputeBinder> binder) { }
	};

	// Used as a common graphics pipeline binding object
	struct GraphicsPipeline : public StaticPipeline<
		DebugStateManager,
		VertexShaderStageStateManager,
		PixelShaderStageStateManager,
		DomainShaderStageStateManager,
		HullShaderStageStateManager,
		GeometryShaderStageStateManager,
		StreamOutputStateManager,
		BlendingStateManager,
		BlendSampleMaskStateManager,
		RasterizerStateManager,
		DepthStencilStateManager,
		InputLayoutStateManager,
		IndexBufferStripStateManager,
		PrimitiveTopologyStateManager,
		RenderTargetFormatsStateManager,
		DepthStencilFormatStateManager,
		MultisamplingStateManager,
		NodeMaskStateManager,
		RootSignatureStateManager
	> {
	protected:
		gObj<ComputeBinder> OnCollectBindings() override {
			gObj<ComputeBinder> binder = new GraphicsBinder();
			Bindings(binder.Dynamic_Cast<GraphicsBinder>());
			return binder;
		}

		virtual void Bindings(gObj<GraphicsBinder> binder) { }
	};

	// Used to show complexity
	class ShowComplexityPipeline : public GraphicsPipeline {
	public:
		// UAV to output the complexity
		gObj<Texture2D> Complexity;

		// Render Target
		gObj<Texture2D> RenderTarget;

	protected:
		void Setup() {
			set->VertexShader(ShaderLoader::FromMemory(cso_DrawScreen_VS));
			set->PixelShader(ShaderLoader::FromMemory(cso_DrawComplexity_PS));
			set->InputLayout({
					VertexElement(VertexElementType::Float, 2, VertexElementSemantic::Position)
				});
		}

		void Bindings(gObj<GraphicsBinder> binder)
		{
			binder->Bindings_OnSet();
			binder->Bindings_PixelShader();
			binder->RTV(0, RenderTarget);
			binder->SRV(0, Complexity);
		}
	};

#pragma endregion

#pragma region Dynamic Pipelines (RTX)

	// Represents a raytracing program.
	// Internally represents a set of bindings for programs
	// and shader tables to represent accessible programs during tracing.
	class RaytracingProgramBase {
		friend RaytracingPipeline;
		friend RaytracingManager;

		wProgram* wrapper;

		// Collect bindings, create signatures and shader tables.
		void OnLoad(wDevice* dxWrapper);

	protected:
		RaytracingProgramBase() {
		}

		virtual void Setup() = 0;

		// Collects global bindings for all programs.
		virtual void Bindings(gObj<RaytracingBinder> binder) {
		}

		// Collects local bindings for raygeneration program.
		virtual void RayGeneration_Bindings(gObj<RaytracingBinder> binder) {
		}

		// Collects local bindings for miss programs.
		virtual void Miss_Bindings(gObj<RaytracingBinder> binder) {
		}

		// Collects local bindings for hit group programs.
		virtual void HitGroup_Bindings(gObj<RaytracingBinder> binder) {
		}

	public:

		virtual ~RaytracingProgramBase() {}

		// Defines the maximum payload size for all raytracing recursive programs.
		void Payload(int sizeInBytes);
		// Defines the size of the shader table used to handle hit groups
		void MaxHitGroupIndex(int index);
		// Defines the maximum stack size for recursive shaders.
		void StackSize(int maxDeep);
		// Defines the attribute size for intersection feed back.
		void Attribute(int sizeInBytes);

		/// <summary>
		/// Loads a raygeneration shader in this rt program
		/// </summary>
		void Shader(gObj<RayGenerationHandle>& shader);
		/// <summary>
		/// Loads a miss shader in this rt program
		/// </summary>
		void Shader(gObj<MissHandle>& shader);
		/// <summary>
		/// Loads a hit group in this rt program
		/// </summary>
		void Shader(gObj<HitGroupHandle>& shader);
	};

	template <typename C>
	class RaytracingProgram : public RaytracingProgramBase {
		friend RaytracingPipeline;

		gObj<C> __Pipeline;
		void BindContext(RaytracingPipeline* bindings) {
			__Pipeline = dynamic_cast<C*>(bindings);
			if (__Pipeline.isNull())
				throw Exception::FromError(Errors::Invalid_Operation, "Error loading program. Program should use this type as context type.");
		}
	protected:
		RaytracingProgram() {}
	public:
		inline gObj<C>& Context() {
			return __Pipeline;
		}
	};

	class DynamicStateBindings {

		friend RaytracingPipeline;
		template<typename S, D3D12_STATE_SUBOBJECT_TYPE Type> friend class DynamicStateBindingOf;

		D3D12_STATE_SUBOBJECT* dynamicStates;

		void AllocateStates(int length) {
			dynamicStates = new D3D12_STATE_SUBOBJECT[length];
		}

		template<typename D>
		D* GetState(int index) {
			return (D*)dynamicStates[index].pDesc;
		}

		template<typename D>
		void SetState(int index, D3D12_STATE_SUBOBJECT_TYPE type, D* state) {
			dynamicStates[index].Type = type;
			dynamicStates[index].pDesc = state;
		}

	public:
		DynamicStateBindings() {}
	};

	template<typename S, D3D12_STATE_SUBOBJECT_TYPE Type>
	class DynamicStateBindingOf : public virtual DynamicStateBindings {
	protected:
		S* GetState(int index) {
			return GetState<S>(index);
		}

		S* GetAfterCreate(int index) {
			S* state = new S{ };
			SetState(index, Type, state);
			return state;
		}
	public:
	};

#pragma region Dynamic States

	struct GlobalRootSignatureManager : public DynamicStateBindingOf<D3D12_GLOBAL_ROOT_SIGNATURE, D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE> {
		void SetGlobalRootSignature(int index, ID3D12RootSignature* rootSignature) {
			auto state = GetAfterCreate(index);
			state->pGlobalRootSignature = rootSignature;
		}
	};

	struct LocalRootSignatureManager : public DynamicStateBindingOf<D3D12_LOCAL_ROOT_SIGNATURE, D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE> {
		void SetLocalRootSignature(int index, ID3D12RootSignature* rootSignature) {
			auto state = GetAfterCreate(index);
			state->pLocalRootSignature = rootSignature;
		}
	};

	struct HitGroupManager : public virtual DynamicStateBindingOf<D3D12_HIT_GROUP_DESC, D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP> {
		void SetTriangleHitGroup(int index, LPCWSTR exportName, LPCWSTR anyHit, LPCWSTR closestHit) {
			auto hg = GetAfterCreate(index);
			hg->AnyHitShaderImport = anyHit;
			hg->ClosestHitShaderImport = closestHit;
			hg->IntersectionShaderImport = nullptr;
			hg->Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
			hg->HitGroupExport = exportName;
		}
		void SetProceduralGeometryHitGroup(int index, LPCWSTR exportName, LPCWSTR anyHit, LPCWSTR closestHit, LPCWSTR intersection) {
			auto hg = GetAfterCreate(index);
			hg->AnyHitShaderImport = anyHit;
			hg->ClosestHitShaderImport = closestHit;
			hg->IntersectionShaderImport = intersection;
			hg->Type = D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;
			hg->HitGroupExport = exportName;
		}
	};

	struct DXILManager : public virtual DynamicStateBindingOf<D3D12_DXIL_LIBRARY_DESC, D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY> {
		void SetDXIL(int index, D3D12_SHADER_BYTECODE bytecode, list<D3D12_EXPORT_DESC>& exports) {
			auto dxil = GetAfterCreate(index);
			dxil->DXILLibrary = bytecode;
			dxil->NumExports = exports.size();
			dxil->pExports = &exports.first();
		}
	};

	struct RTShaderConfig : public virtual DynamicStateBindingOf<D3D12_RAYTRACING_SHADER_CONFIG, D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG>
	{
		void SetRTSizes(int index, int maxAttributeSize, int maxPayloadSize) {
			auto state = GetAfterCreate(index);
			state->MaxAttributeSizeInBytes = maxAttributeSize;
			state->MaxPayloadSizeInBytes = maxPayloadSize;
		}
	};

	struct RTPipelineConfig : public virtual DynamicStateBindingOf<D3D12_RAYTRACING_PIPELINE_CONFIG, D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG>
	{
		void SetMaxRTRecursion(int index, int maxRecursion) {
			auto state = GetAfterCreate(index);
			state->MaxTraceRecursionDepth = maxRecursion;
		}
	};

	struct ExportsManager : public virtual DynamicStateBindingOf<D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION, D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION>
	{
		void SetExportsAssociations(int index, D3D12_STATE_SUBOBJECT* ptrToSubobject, const table<LPCWSTR>& exports) {
			auto state = GetAfterCreate(index);
			state->pSubobjectToAssociate = ptrToSubobject;
			state->NumExports = exports.size();
			state->pExports = &exports.first();
		}
	};

#pragma endregion

	struct RaytracingPipeline :
		public Pipeline,
		public virtual DynamicStateBindings,
		DXILManager,
		GlobalRootSignatureManager,
		LocalRootSignatureManager,
		HitGroupManager,
		RTShaderConfig,
		RTPipelineConfig,
		ExportsManager
	{
		friend DeviceManager;
		friend ComputeManager;
		friend RaytracingManager;
	private:

		wStateObject* wrapper;

		// Will be executed when this pipeline manager were loaded
		void OnCreate(wDevice* dxWrapper);

		// Called when a pipeline is active into the pipeline
		void OnSet(wCmdList* cmdWrapper);

		// Called when a pipeline is used to gen rays.
		void OnDispatch(wCmdList* cmdWrapper);

		void AppendCode(const D3D12_SHADER_BYTECODE& code);
		void LoadProgram(gObj<RaytracingProgramBase> program);
		void DeclareExport(LPCWSTR shader);

	protected:


		// Loads the code of a dxil library.
		// After this several shaders can be created for handling programs.
		// Use __create Handle<>(...) to export the shader handler used by your programs. 
		// This method can be called several times to load different library files but the shaders should be created inbetween.
		void Code(const D3D12_SHADER_BYTECODE& code) {
			AppendCode(code);
		}

		// Loads a specific program into a field.
		// This method ask the program to setup and create shader tables, signatures, etc.
		template<typename P>
		void RTProgram(gObj<P>& program) {
			program = new P();
			program->BindContext(this);
			LoadProgram(program);
		}

		template<typename S>
		void Shader(gObj<S>& handle, LPCWSTR shader) {
			handle = new S(shader);
			DeclareExport(shader);
		}
		// Creates a handler to a Hit Group of shaders
		void HitGroup(gObj<HitGroupHandle> &hitGroup, gObj<ClosestHitHandle> closest, gObj<AnyHitHandle> anyHit, gObj<IntersectionHandle> intersection);

		// When implemented, configures the pipeline object loading libraries,
		// creating shader handles and defining program settings.
		virtual void Setup() = 0;

		Engine GetEngine() { return Engine::Compute; }

		RaytracingPipeline()
		{
		}
	};


#pragma endregion

#pragma endregion

#pragma region Command List Management

	

	/// Base type for all types of engines
	class CommandListManager {
		friend wScheduler;
		Tagging __Tag; // Allows the scheduler to 'plant' a value for async processes to access to it.
	protected:
		wCmdList* w_cmdList;
	public:
		virtual ~CommandListManager() { }
		// Tag data when the process was enqueue
		template<typename T>
		inline T Tag() { return __Tag.getData<T>(); }
	};

	class CopyManager : public CommandListManager {
		friend wScheduler;
	protected:
		CopyManager() {
		}
	public:

		/// <summary>
		/// Clears a resource on the gpu as UAV
		/// </summary>
		void Clear_UAV(gObj<ResourceView> uav, const FLOAT values[4]);
		/// <summary>
		/// Clears a resource on the gpu as UAV
		/// </summary>
		void Clear_UAV(gObj<ResourceView> uav, const unsigned int values[4]);
		/// <summary>
		/// Clears a resource on the gpu as UAV
		/// </summary>
		void Clear_UAV(gObj<ResourceView> uav, const float4& value) {
			float v[4]{ value.x, value.y, value.z, value.w };
			Clear_UAV(uav, v);
		}
		/// <summary>
		/// Clears a resource on the gpu as UAV
		/// </summary>
		inline void Clear_UAV(gObj<ResourceView> uav, const unsigned int& value) {
			unsigned int v[4]{ value, value, value, value };
			Clear_UAV(uav, v);
		}
		/// <summary>
		/// Clears a resource on the gpu as UAV
		/// </summary>
		inline void Clear_UAV(gObj<ResourceView> uav, const uint4& value) {
			unsigned int v[4]{ value.x, value.y, value.z, value.w };
			Clear_UAV(uav, v);
		}
		/// <summary>
		/// Updates all subresources from mapped memory to the gpu
		/// </summary>
		void Load_AllToGPU(gObj<ResourceView> resource);
		/// <summary>
		/// Updates all subresources from the gpu to mapped memory
		/// </summary>
		void Load_AllFromGPU(gObj<ResourceView> resource);
		/// <summary>
		/// Updates a region of a single subresource from mapped memory to the gpu
		/// </summary>
		void Load_RegionToGPU(gObj<ResourceView> singleSubresource, const D3D12_BOX& region);
		/// <summary>
		/// Updates a region of a single subresource from the gpu to mapped memory
		/// </summary>
		void Load_RegionFromGPU(gObj<ResourceView> singleSubresource, const D3D12_BOX& region);
		/// <summary>
		/// Copies the content from one resource to another. Both resources must be compatible.
		/// </summary>
		void Copy_Resource(gObj<Texture2D> dst, gObj<Texture2D> src);
	};

	class ComputeManager : public CopyManager
	{
		friend wScheduler;
	protected:
		ComputeManager() : CopyManager()
		{
		}
	public:

		// Sets a graphics pipeline
		void Set_Pipeline(gObj<Pipeline> pipeline);

		// Dispatches a specific number of threads to execute current compute shader set.
		void Dispatch_Threads(int dimx, int dimy = 1, int dimz = 1);
	};

	class GraphicsManager : public ComputeManager {
		friend wScheduler;
	protected:
		GraphicsManager() :ComputeManager()
		{
		}
	public:
		// Changes the viewport
		void Set_Viewport(float width, float height, float maxDepth = 1.0f, float x = 0, float y = 0, float minDepth = 0.0f);

		// Sets a vertex buffer in a specific slot
		void Set_VertexBuffer(gObj<Buffer> buffer, int slot = 0);

		// Sets an index buffer
		void Set_IndexBuffer(gObj<Buffer> buffer);

		void Clear_RT(gObj<Texture2D> rt, const FLOAT values[4]);
		inline void Clear_RT(gObj<Texture2D> rt, const float4& value) {
			float v[4]{ value.x, value.y, value.z, value.w };
			Clear_RT(rt, v);
		}
		// Clears the render target accessed by the Texture2D View with a specific float3 value
		inline void Clear_RT(gObj<Texture2D> rt, const float3& value) {
			float v[4]{ value.x, value.y, value.z, 1.0f };
			Clear_RT(rt, v);
		}
		void Clear_DepthStencil(gObj<Texture2D> depthStencil, float depth, UINT8 stencil, D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
		inline void Clear_Depth(gObj<Texture2D> depthStencil, float depth = 1.0f) {
			Clear_DepthStencil(depthStencil, depth, 0, D3D12_CLEAR_FLAG_DEPTH);
		}
		inline void Clear_Stencil(gObj<Texture2D> depthStencil, UINT8 stencil) {
			Clear_DepthStencil(depthStencil, 1, stencil, D3D12_CLEAR_FLAG_STENCIL);
		}

		// Draws a primitive using a specific number of vertices
		void Dispatch_Primitive(D3D_PRIMITIVE_TOPOLOGY topology, int count, int start = 0);

		// Draws a primitive using a specific number of vertices
		void Dispatch_IndexedPrimitive(D3D_PRIMITIVE_TOPOLOGY topology, int count, int start = 0);

		// Draws triangles using a specific number of vertices
		void Dispatch_Triangles(int count, int start = 0) {
			Dispatch_Primitive(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, count, start);
		}

		void Dispatch_IndexedTriangles(int count, int start = 0) {
			Dispatch_IndexedPrimitive(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, count, start);
		}
	};

	struct DX_BakedGeometry;
	struct DX_BakedScene;
	struct DX_GeometryCollectionWrapper;
	struct DX_InstanceCollectionWrapper;

	enum class CollectionState {
		// The collection has not been built yet, there is not any GPU version of it.
		NotBuilt,
		// The collection has been built and doesnt need any update
		UpToDate,
		// The collection has a GPU version but some elements might be updated.
		NeedsUpdate,
		// The collection has a GPU version with a different structure.
		// Needs to be rebuilt to update dirty elements.
		NeedsRebuilt
	};

	class InstanceCollection;
	class GeometryCollection {
		friend RaytracingManager;
		friend InstanceCollection;
	protected:
		DX_GeometryCollectionWrapper* wrapper = nullptr;
		GeometryCollection() {}
	public:
		virtual ~GeometryCollection() {
			delete wrapper;
		}

		CollectionState State();

		void ForceState(CollectionState state);

		int Count();

		// Clears this collection
		void Clear();
	};

	class TriangleGeometryCollection : public GeometryCollection {
		friend RaytracingManager;
		TriangleGeometryCollection() : GeometryCollection() {}

		void __SetInputLayout(VertexElement* elements, int count);

	public:
		void Load_Vertices(int geometryID, gObj<Buffer> newVertices);

		void Load_Transform(int geometryID, int transformIndex);

		int Create_Geometry(gObj<Buffer> vertices, int transformIndex = -1);

		int Create_Geometry(gObj<Buffer> vertices, gObj<Buffer> indices, int transformIndex = -1);

		template<int count>
		void Set_VertexLayout(VertexElement(&layout)[count]) {
			__SetInputLayout((VertexElement*)&layout, count);
		}
		void Set_VertexLayout(std::initializer_list<VertexElement> layout) {
			__SetInputLayout((VertexElement*)layout.begin(), layout.size());
		}

		void Set_Transforms(gObj<Buffer> transforms);
	};

	class ProceduralGeometryCollection : public GeometryCollection {
		friend RaytracingManager;
		ProceduralGeometryCollection() :GeometryCollection() {}
	public:
		void Load_Boxes(int start, gObj<Buffer> newBoxes);

		int Create_Geometry(gObj<Buffer> boxes);
	};

	class InstanceCollection {
		friend RaytracingManager;
		friend wBindings;

		DX_InstanceCollectionWrapper* wrapper = nullptr;
		InstanceCollection() {}
	public:
		// Updates the geometry attribute for a specific instance
		void Load_InstanceGeometry(int instance, gObj<GeometryCollection> geometries);
		// Updates the mask attribute for a specific instance
		void Load_InstanceMask(int instance, UINT mask);
		// Updates the contribution attribute for a specific instance
		void Load_InstanceContribution(int instance, int instanceContribution);
		// Updates the instance ID attribute for a specific instance
		void Load_InstanceID(int instance, UINT instanceID);
		// Updates the transform attribute for a specific instance
		void Load_InstanceTransform(int instance, float4x3 transform);

		// Adds a new instance to the collection.
		// Returns the number of instances.
		int Create_Instance(gObj<GeometryCollection> geometry,
			UINT mask = 255U,
			int contribution = 0,
			UINT instanceID = INTSAFE_UINT_MAX,
			float4x3 transform = float4x3(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0));

		int Count();

		CollectionState State();

		void ForceState(CollectionState state);

		// Clear this collection
		void Clear();
	};

	class RaytracingManager : public GraphicsManager {
		friend wScheduler;
	protected:
		RaytracingManager() : GraphicsManager()
		{
		}
	public:
		gObj<TriangleGeometryCollection> Create_TriangleGeometries();
		gObj<ProceduralGeometryCollection> Create_ProceduralGeometries();
		gObj<InstanceCollection> Create_Intances();

		gObj<GeometryCollection> Attach(gObj<GeometryCollection> collection);

		// Depending on collection state, builds, updates or rebuilds current collection into GPU buffers.
		void Load_Geometry(gObj<GeometryCollection> geometries,
			bool allowUpdate = false, bool preferFastRaycasting = true);
		// Depending on collection state, builds, updates or rebuilds current collection into GPU buffers.
		void Load_Scene(gObj<InstanceCollection> instances,
			bool allowUpdate = false, bool preferFastRaycasting = true);

		// Activate this program to be used when dispatching rays.
		void Set_Program(gObj<RaytracingProgramBase> program);

		// Commit all local bindings for this ray generation shader
		void Set_RayGeneration(gObj<RayGenerationHandle> shader);

		// Commit all local bindings for this miss shader
		void Set_Miss(gObj<MissHandle> shader, int index);

		// Commit all local bindings for this shader group
		void Set_HitGroup(gObj<HitGroupHandle> group, int geometryIndex,
			int rayContribution = 0, int multiplier = 1, int instanceContribution = 0);

		void Dispatch_Rays(int width, int height, int depth = 1);
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

	struct InternalDXInfo {
		HWND hWnd;
		DX_Device device = nullptr;
		DX_SwapChain swapChain = nullptr;
		DX_CommandList mainCmdList = nullptr;
		int Buffers;
		DXGI_FORMAT RenderTargetFormat;
		D3D12_CPU_DESCRIPTOR_HANDLE* RenderTargets;
	};

	class DeviceManager {
		friend Presenter;
		friend Technique;

		wDevice* device = nullptr;

		wResource* CreateResource(
			const D3D12_RESOURCE_DESC& desc,
			int elementStride,
			int elementCount,
			D3D12_RESOURCE_STATES initialState,
			D3D12_CLEAR_VALUE* clearing,
			CPUAccessibility accessibility
		);

		template<typename T>
		gObj<T> CreateResourceView(const D3D12_RESOURCE_DESC& desc,
			int elementStride,
			int elementCount,
			D3D12_RESOURCE_STATES initialState,
			D3D12_CLEAR_VALUE* clearing = nullptr,
			CPUAccessibility accessibility = CPUAccessibility::None)
		{
			wResource* resource = CreateResource(desc, elementStride, elementCount, initialState, clearing, accessibility);

			return new T(device, resource, nullptr);
		}

		int MaxMipsFor(int dimension) {
			int mips = 0;
			while (dimension > 0)
			{
				mips++;
				dimension >>= 1;
			}
			return mips;
		}

		void FillTexture3DDescription(DXGI_FORMAT format, D3D12_RESOURCE_DESC& desc, long width, int height, int slices, int mips, D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE) {
			if (mips == 0) // compute maximum possible value
				mips = 100000;

			desc.Width = width;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			desc.Flags = flag;
			desc.Format = format;
			desc.Height = height;
			desc.Alignment = 0;
			desc.DepthOrArraySize = slices;
			desc.MipLevels = min(mips, MaxMipsFor(min(min(width, height), slices)));
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.SampleDesc = { 1, 0 };
		}

		void FillTexture2DDescription(DXGI_FORMAT format, D3D12_RESOURCE_DESC& desc, long width, int height, int mips, int arrayLength, D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE) {
			if (mips == 0) // compute maximum possible value
				mips = 100000;

			desc.Width = width;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Flags = flag;
			desc.Format = format;
			desc.Height = height;
			desc.Alignment = 0;
			desc.DepthOrArraySize = arrayLength;
			desc.MipLevels = min(mips, MaxMipsFor(min(width, height)));
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.SampleDesc = { 1, 0 };
		}

		void FillTexture1DDescription(DXGI_FORMAT format, D3D12_RESOURCE_DESC& desc, long width, int mips, int arrayLength, D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE) {
			if (mips == 0) // compute maximum possible value
				mips = 100000;
			desc.Width = width;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			desc.Flags = flag;
			desc.Format = format;
			desc.Height = 1;
			desc.Alignment = 0;
			desc.DepthOrArraySize = arrayLength;
			desc.MipLevels = min(mips, MaxMipsFor(width));
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.SampleDesc = { 1, 0 };
		}

		void FillBufferDescription(D3D12_RESOURCE_DESC& desc, long width, D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE) {
			desc.Width = width;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Flags = flag;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Height = 1;
			desc.Alignment = 0;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.SampleDesc = { 1, 0 };
		}

	protected:

		DeviceManager() {}

	public:

		gObj<Texture2D> CurrentRenderTarget();
		
		/// <summary>
		/// Used to load techniques or pipelines. This method associates the current object to the device manager used by their parent.
		/// </summary>
		template<typename T, typename ...A>
		inline void Load(gObj<T>& obj, A... args) {
			if (obj.isNull())
				obj = new T(args...);
			obj->OnCreate(device);
		}

#pragma region Creating

		// Creates a buffer to be used as a constant buffer in a shader.
		// Dynamic buffers are created in a upload heap so, they can be written directly from the cpu.
		gObj<Buffer> Create_Buffer_CB(int elementStride, bool dynamic = false);
		// Creates a buffer to be used as a constant buffer in a shader.
		// Dynamic buffers are created in a upload heap so, they can be written directly from the cpu.
		template <typename T>
		gObj<Buffer> Create_Buffer_CB(bool dynamic = false) {
			return Create_Buffer_CB(sizeof(T), dynamic);
		}

		// Creates a buffer to store modifiable acceleration datastructure geometry and instance buffers.
		gObj<Buffer> Create_Buffer_ADS(int elementStride, int count);
		// Creates a buffer to store modifiable acceleration datastructure geometry and instance buffers.
		template <typename T>
		gObj<Buffer> Create_Buffer_ADS(int count = 1) {
			return Create_Buffer_ADS(sizeof(T), count);
		}

		// Creates a buffer to be used as a StructuredBuffer in a shader.
		gObj<Buffer> Create_Buffer_SRV(int elementStride, int count);
		// Creates a buffer to be used as a StructuredBuffer in a shader.
		template <typename T>
		gObj<Buffer> Create_Buffer_SRV(int count = 1) {
			return Create_Buffer_SRV(sizeof(T), count);
		}

		// Creates a buffer to be binded as a vertex buffer.
		gObj<Buffer> Create_Buffer_VB(int elementStride, int count);
		// Creates a buffer to be binded as a vertex buffer.
		template <typename T>
		gObj<Buffer> Create_Buffer_VB(int count) {
			return Create_Buffer_VB(sizeof(T), count);
		}

		// Creates a buffer to be binded as an index buffer.
		gObj<Buffer> Create_Buffer_IB(int elementStride, int count);
		// Creates a buffer to be binded as an index buffer.
		template <typename T>
		gObj<Buffer> Create_Buffer_IB(int count) {
			throw Exception::FromError(Errors::UnsupportedFormat, "Error in type");
		}
		// Creates a buffer to be binded as an index buffer.
		template <>
		gObj<Buffer> Create_Buffer_IB<int>(int count) {
			return Create_Buffer_IB(4, count);
		}
		// Creates a buffer to be binded as an index buffer.
		template <>
		gObj<Buffer> Create_Buffer_IB<unsigned int>(int count) {
			return Create_Buffer_IB(4, count);
		}
		// Creates a buffer to be binded as an index buffer.
		template <>
		gObj<Buffer> Create_Buffer_IB<short>(int count) {
			return Create_Buffer_IB(2, count);
		}
		// Creates a buffer to be binded as an index buffer.
		template <>
		gObj<Buffer> Create_Buffer_IB<unsigned short>(int count) {
			return Create_Buffer_IB(2, count);
		}

		// Creates a buffer to be used as a RWStructuredBuffer in a shader.
		gObj<Buffer> Create_Buffer_UAV(int elementStride, int count);
		// Creates a buffer to be used as a RWStructuredBuffer in a shader.
		template <typename T>
		gObj<Buffer> Create_Buffer_UAV(int count = 1) {
			return Create_Buffer_UAV(sizeof(T), count);
		}

		// Creates a onedimensional texture to be used as a Texture1D in a shader.
		gObj<Texture1D> Create_Texture1D_SRV(DXGI_FORMAT format, int width, int mips = 1, int arrayLength = 1);
		// Creates a onedimensional texture to be used as a Texture1D in a shader.
		template<typename T>
		gObj<Texture1D> Create_Texture1D_SRV(int width, int mips = 1, int arrayLength = 1) {
			return Create_Texture1D_SRV(Formats<T>::format, width, mips, arrayLength);
		}

		// Creates a onedimensional texture to be used as a RWTexture1D in a shader.
		// if mips == 0 all possible mips are allocated.
		gObj<Texture1D> Create_Texture1D_UAV(DXGI_FORMAT format, int width, int mips = 0, int arrayLength = 1);
		// Creates a onedimensional texture to be used as a RWTexture1D in a shader.
		// if mips == 0 all possible mips are allocated.
		template<typename T>
		gObj<Texture1D> Create_Texture1D_UAV(int width, int mips = 0, int arrayLength = 1) {
			return Create_Texture1D_UAV(Formats<T>::Value, width, mips, arrayLength);
		}

		// Creates a bidimensional texture to be used as a Texture2D in a shader.
		// if mips == 0 all possible mips are allocated.
		gObj<Texture2D> Create_Texture2D_SRV(DXGI_FORMAT format, int width, int height, int mips = 0, int arrayLength = 1);
		// Creates a bidimensional texture to be used as a Texture2D in a shader.
		// if mips == 0 all possible mips are allocated.
		template<typename T>
		gObj<Texture2D> Create_Texture2D_SRV(int width, int height, int mips = 0, int arrayLength = 1) {
			return Create_Texture2D_SRV(Formats<T>::Value, width, height, mips, arrayLength);
		}
		/// <summary>
		/// Loads the content of a file into a texture2D.
		/// </summary>
		gObj<Texture2D> Create_Texture2D_SRV(dx4xb::string filePath);

		/// <summary>
		///	Saves the content of a texture to a file.
		/// </summary>
		void Create_File(dx4xb::string filePath, gObj<Texture2D> texture);

		// Creates a bidimensional texture to be used as a RWTexture2D in a shader or a render target.
		// if mips == 0 all possible mips are allocated.
		gObj<Texture2D> Create_Texture2D_UAV(DXGI_FORMAT format, int width, int height, int mips = 1, int arrayLength = 1);
		// Creates a bidimensional texture to be used as a RWTexture2D in a shader.
		// if mips == 0 all possible mips are allocated.
		template<typename T>
		gObj<Texture2D> Create_Texture2D_UAV(int width, int height, int mips = 1, int arrayLength = 1) {
			return Create_Texture2D_UAV(Formats<T>::Value, width, height, mips, arrayLength);
		}

		// Creates a bidimensional texture to be used exclusively as a render target.
		// if mips == 0 all possible mips are allocated.
		gObj<Texture2D> Create_Texture2D_RT(DXGI_FORMAT format, int width, int height, int mips = 1, int arrayLength = 1);
		// Creates a bidimensional texture to be used exclusively as a render target.
		// if mips == 0 all possible mips are allocated.
		template<typename T>
		gObj<Texture2D> Create_Texture2D_RT(int width, int height, int mips = 1, int arrayLength = 1) {
			return Create_Texture2D_RT(Formats<T>::Value, width, height, mips, arrayLength);
		}

		// Creates a bidimensional texture to be used as exclusively as DepthStencil Buffer.
		gObj<Texture2D> Create_Texture2D_DSV(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);

		// Creates a threedimensional texture to be used as a Texture3D in a shader.
		// if mips == 0 all possible mips are allocated.
		gObj<Texture3D> Create_Texture3D_SRV(DXGI_FORMAT format, int width, int height, int depth, int mips = 1);
		// Creates a threedimensional texture to be used as a Texture3D in a shader.
		// if mips == 0 all possible mips are allocated.
		template<typename T>
		gObj<Texture3D> Create_Texture3D_SRV(int width, int height, int depth, int mips = 1) {
			return Create_Texture3D_SRV(Formats<T>::Value, width, height, depth, mips);
		}

		// Creates a threedimensional texture to be used as a RWTexture3D in a shader.
		gObj<Texture3D> Create_Texture3D_UAV(DXGI_FORMAT format, int width, int height, int depth, int mips = 1);
		// Creates a threedimensional texture to be used as a RWTexture3D in a shader.
		template<typename T>
		gObj<Texture3D> Create_Texture3D_UAV(int width, int height, int depth, int mips = 1) {
			return Create_Texture3D_UAV(Formats<T>::Value, width, height, depth, mips);
		}

#pragma endregion

#pragma region Dispatching

		template<typename T>
		inline void Dispatch_Technique(gObj<T> technique) {
			technique->OnDispatch();
		}

		void Dispatch_Process(gObj<GPUProcess> process);

		void Dispatch_Process_Async(gObj<GPUProcess> process);

		template<typename T>
		void Dispatch_Method(T* instance, typename MethodAsGPUProcess<T, CopyManager>::Member member) {
			Dispatch_Process(new MethodAsGPUProcess<T, CopyManager>(instance, member));
		}

		template<typename T>
		void Dispatch_Method(T* instance, typename MethodAsGPUProcess<T, ComputeManager>::Member member) {
			Dispatch_Process(new MethodAsGPUProcess<T, ComputeManager>(instance, member));
		}

		template<typename T>
		void Dispatch_Method(T* instance, typename MethodAsGPUProcess<T, GraphicsManager>::Member member) {
			Dispatch_Process(new MethodAsGPUProcess<T, GraphicsManager>(instance, member));
		}

		template<typename T>
		void Dispatch_Method(T* instance, typename MethodAsGPUProcess<T, RaytracingManager>::Member member) {
			Dispatch_Process(new MethodAsGPUProcess<T, RaytracingManager>(instance, member));
		}

		template<typename T>
		void Dispatch_Method_Async(T* instance, typename MethodAsGPUProcess<T, CopyManager>::Member member) {
			Dispatch_Process_Async(new MethodAsGPUProcess<T, CopyManager>(instance, member));
		}

		template<typename T>
		void Dispatch_Method_Async(T* instance, typename MethodAsGPUProcess<T, ComputeManager>::Member member) {
			Dispatch_Process_Async(new MethodAsGPUProcess<T, ComputeManager>(instance, member));
		}

		template<typename T>
		void Dispatch_Method_Async(T* instance, typename MethodAsGPUProcess<T, GraphicsManager>::Member member) {
			Dispatch_Process_Async(new MethodAsGPUProcess<T, GraphicsManager>(instance, member));
		}

		template<typename T>
		void Dispatch_Method_Async(T* instance, typename MethodAsGPUProcess<T, RaytracingManager>::Member member) {
			Dispatch_Process_Async(new MethodAsGPUProcess<T, RaytracingManager>(instance, member));
		}
	
#pragma endregion
	
#pragma region Sync

		Signal Flush(EngineMask mask = EngineMask::All);

#pragma endregion

	};

	class Technique : protected DeviceManager {
		friend DeviceManager;

		void OnCreate(wDevice* w_device);

	protected:
		virtual void OnLoad() = 0;

		virtual void OnDispatch() = 0;
	};

	class Presenter : public DeviceManager
	{
		Presenter() {}
	public:
		static gObj<Presenter> Create(const PresenterDescription& desc);

		void GetInternalDXInfo(InternalDXInfo& info);

		void BeginFrame();

		void EndFrame();

		~Presenter();
	};


#pragma endregion

}


#endif
