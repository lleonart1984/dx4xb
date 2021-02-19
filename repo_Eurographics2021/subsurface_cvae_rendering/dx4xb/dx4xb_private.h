#ifndef DX4XB_PRIVATE_H
#define DX4XB_PRIVATE_H

#include "pch.h"
#include "dx4xb.h"
#include "DirectXTex.h"

namespace dx4xb {

#pragma region Sync Tools

	class Semaphore {
		HANDLE handle;
#ifdef _DEBUG
		int currentCounter;
		int maxCount;
		bool waiting = false;
#endif
	public:

		Semaphore(int initialCount, int maxCount)
#ifdef _DEBUG
			:currentCounter(initialCount), maxCount(maxCount)
#endif
		{
			handle = CreateSemaphore(nullptr, initialCount, maxCount, nullptr);
		}
		~Semaphore() {
			CloseHandle(handle);
		}

		inline bool Wait() {
#ifdef _DEBUG
			waiting = true;
#endif
			auto result = WaitForSingleObject(handle, INFINITE);
#ifdef _DEBUG
			waiting = false;
			currentCounter--;
#endif
			return result == WAIT_OBJECT_0;
		}

		inline void Release() {
			ReleaseSemaphore(handle, 1, nullptr);
#ifdef _DEBUG
			currentCounter++;
#endif
		}
	};

	class Mutex {
		HANDLE handle;
#ifdef _DEBUG
		bool locked = false;
#endif
	public:
		inline Mutex() {
			handle = CreateMutex(nullptr, false, nullptr);
		}
		~Mutex() {
			CloseHandle(handle);
		}

		inline bool Acquire() {
#ifdef _DEBUG
			locked = true;
#endif
			return WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0;
		}

		inline void Release() {
			ReleaseMutex(handle);
#ifdef _DEBUG
			locked = false;
#endif
		}
	};

	class CountEvent {
		int count = 0;
		Mutex mutex;
		Semaphore goSemaphore;
	public:
		inline CountEvent() :mutex(Mutex()), goSemaphore(Semaphore(1, 1)) {
		}

		inline void Increment() {
			mutex.Acquire();
			if (this->count == 0)
				goSemaphore.Wait();
			this->count++;
			mutex.Release();
		}

		inline void Wait() {
			goSemaphore.Wait();
			goSemaphore.Release();
		}

		inline void Signal() {
			mutex.Acquire();
			this->count--;
			if (this->count == 0)
				goSemaphore.Release();
			mutex.Release();
		}
	};

	template<typename T>
	class ProducerConsumerQueue {
		T* elements;
		int elementsLength;
		int start;
		int count;
		Semaphore productsSemaphore;
		Semaphore spacesSemaphore;
		Mutex mutex;
		bool closed;
	public:
		ProducerConsumerQueue(int capacity) :
			productsSemaphore(Semaphore(0, capacity)),
			spacesSemaphore(Semaphore(capacity, capacity)),
			mutex(Mutex()) {
			elementsLength = capacity;
			elements = new T[elementsLength];
			start = 0;
			count = 0;
			closed = false;
		}

		~ProducerConsumerQueue() {
			delete[] elements;
		}
		inline int getCount() { return count; }

		template<typename T>
		bool TryConsume(T& element)
		{
			if (productsSemaphore.Wait())
			{
				// ensured there is an element to be consumed
				mutex.Acquire();
				element = elements[start];
				start = (start + 1) % elementsLength;
				count--;
				spacesSemaphore.Release();
				mutex.Release();
				return true;
			}
			return false;
		}

		template<typename T>
		bool TryProduce(T element) {
			if (spacesSemaphore.Wait())
			{
				mutex.Acquire();
				int enqueuePos = (start + count) % elementsLength;
				elements[enqueuePos] = element;
				count++;
				productsSemaphore.Release();
				mutex.Release();
				return true;
			}
			return false;
		}
	};

#pragma endregion

#pragma region Command List Wrapping

	struct wCmdList {
		wDevice* w_device;
		DX_CommandList cmdList = nullptr;
		gObj<Pipeline> currentPipeline = nullptr;
		wProgram* activeProgram = nullptr;
		list<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors = {};
		list<D3D12_CPU_DESCRIPTOR_HANDLE> dstDescriptors = {};
		list<unsigned int> dstDescriptorRangeLengths = {};
		int vertexBufferSliceOffset = 0;
		int indexBufferSliceOffset = 0;
	};

#pragma endregion

#pragma region Command Queue and Allocators (Scheduling)

	struct wCommandQueue {
		DX_CommandQueue dxQueue;
		long fenceValue;
		DX_Fence fence;
		HANDLE fenceEvent;
		D3D12_COMMAND_LIST_TYPE type;

		wCommandQueue(DX_Device device, D3D12_COMMAND_LIST_TYPE type) {
			D3D12_COMMAND_QUEUE_DESC d = {};
			d.Type = type;
			device->CreateCommandQueue(&d, IID_PPV_ARGS(&dxQueue));

			fenceValue = 1;
			device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
			fenceEvent = CreateEventW(nullptr, false, false, nullptr);
		}

		// Executes a command list set in this engine
		inline void Commit(DX_CommandList* cmdLists, int count) {
			dxQueue->ExecuteCommandLists(count, (ID3D12CommandList**)cmdLists);
		}

		// Send a signals through this queue
		inline long Signal() {
			dxQueue->Signal(fence, fenceValue);
			return fenceValue++;
		}

		// Triggers the event of reaching some fence value.
		inline HANDLE TriggerEvent(long rallyPoint) {
			fence->SetEventOnCompletion(rallyPoint, fenceEvent);
			return fenceEvent;
		}
	};

	// This object allows the scheduler to know what cmd list is used by thread, and if it has some commands to commit or not.
	struct PerThreadInfo {
		DX_CommandList cmdList;
		gObj<CommandListManager> manager;
		bool isActive;
		// Prepares this command list for recording. This method resets the command list to use the desiredAllocator.
		void Activate(DX_CommandAllocator desiredAllocator)
		{
			if (isActive)
				return;
			cmdList->Reset(desiredAllocator, nullptr);
			isActive = true;
		}

		// Prepares this cmd list for flushing to the GPU
		void Close() {
			if (!isActive)
				return;
			isActive = false;
			cmdList->Close();
		}
	};

	// Information needed by the scheduler for each frame on each engine
	struct PerFrameInfo {
		// Allocators for each thread.
		DX_CommandAllocator* allocatorSet;
		int allocatorsUsed;

		inline DX_CommandAllocator RequireAllocator(int index) {
			allocatorsUsed = max(allocatorsUsed, index + 1);
			return allocatorSet[index];
		}

		void ResetUsedAllocators()
		{
			for (int i = 0; i < allocatorsUsed; i++)
				allocatorSet[i]->Reset();
			allocatorsUsed = 0;
		}
	};

	// Information needed by the scheduler for each engine.
	struct PerEngineInfo {
		// The queue representing this engine.
		wCommandQueue* queue;

		// An array of command lists used per thread
		PerThreadInfo* threadInfos;
		int threadCount;

		// An array of allocators used per frame
		PerFrameInfo* frames;
		int framesCount;
	};

	// Struct pairing a process with the tag in the moment of enqueue
	// Used for defered command list population in async mode.
	struct TagProcess {
		gObj<GPUProcess> process;
		Tagging Tag;
	};

	struct wScheduler {

		wDevice* w_device;

		PerEngineInfo Engines[3] = { }; // 0- direct, 1- compute, 2 - copy

		// Struct for threading parameters
		struct GPUWorkerInfo {
			int Index;
			wScheduler* Scheduler;
		} *workers;

		ProducerConsumerQueue<TagProcess>* processQueue;

		// Threads on CPU to collect commands from GPU Processes
		HANDLE* threads;
		int threadsCount;

		// Counting sync object to wait for flushing all processes in processQueue.
		CountEvent* counting;

		// Gets or sets when this scheduler is closed.
		bool IsClosed = false;

		// Gets the index of the current frame
		int CurrentFrameIndex = 0;

		// Tag data will be associated to any command list manager for a process in the time they are enqueue
		Tagging Tag;

		// Gets whenever there is an async cmd list pendent.
		// This is used to know when to force a flush before changing render state at cmd list 0 to Present
		bool AsyncWorkPending = false;

		// Array to collect active cmd lists during flushing.
		// This array needs capacity to store a cmd list x thread x engine type.
		DX_CommandList* ActiveCmdLists;

		Signal* perFrameFinishedSignal;

		static DWORD WINAPI __WORKER_TODO(LPVOID param);

		wScheduler(wDevice* w_device, int frames, int threads);

		void Enqueue(gObj<GPUProcess> process);

		void EnqueueAsync(gObj<GPUProcess> process);

		void PopulateCmdListWithProcess(TagProcess tagProcess, int threadIndex);

		Signal FlushAndSignal(EngineMask mask);

		void WaitFor(const Signal& signal);

		void SetupFrame(int frame);

		void PrepareRenderTarget(D3D12_RESOURCE_STATES state);

		void FinishFrame();

	};

#pragma endregion

#pragma region Descriptor Manager

	struct Allocator {
		struct Node {
			int index;
			Node* next;
		};

		int allocated = 0;
		Node* free = nullptr;

		Mutex mutex;

		~Allocator() {
			while (free != nullptr) {
				Node* toDelete = free;
				free = free->next;
				delete toDelete;
			}
		}

		int Allocate() {
			int index;
			mutex.Acquire();

			if (free != nullptr) {
				Node* toDelete = free;
				free = free->next;
				index = toDelete->index;
				delete toDelete;
			}
			else
				index = allocated++;

			mutex.Release();

			return index;
		}

		void Free(int index) {
			mutex.Acquire();

			free = new Node{
				index,
				free
			};

			mutex.Release();
		}
	};

	class CPUDescriptorHeapManager {
		DX_DescriptorHeap heap;
		unsigned int size;
		Allocator* allocator;
		SIZE_T startCPU;
	public:
		CPUDescriptorHeapManager(DX_Device device, D3D12_DESCRIPTOR_HEAP_TYPE type, int capacity);
		~CPUDescriptorHeapManager() {
			delete allocator;
		}
		// Allocates a new index for a resource descriptor
		inline int AllocateNewHandle() {
			return allocator->Allocate();
		}
		// Releases a specific index of a resource is being released.
		inline void Free(int index) {
			allocator->Free(index);
		}
		inline D3D12_CPU_DESCRIPTOR_HANDLE getCPUVersion(int index) { return D3D12_CPU_DESCRIPTOR_HANDLE{ startCPU + index * size }; }
		inline DX_DescriptorHeap getInnerHeap() { return heap; }
	};

	class GPUDescriptorHeapManager {
		DX_DescriptorHeap heap;
		unsigned int size;
		SIZE_T startCPU;
		UINT64 startGPU;

		// Current Avail
		volatile long mallocOffset = 0;

		// Total Capacity of the heap.
		int capacity;
		// Capacity of the heap for each frame.
		int frameCapacity;

		// Marks the capacity of the heap for a specific frame.
		int lastAvailableInBlock;

		// Allocator used for handles persistent during all execution.
		Allocator* persistentAllocator;

	public:

		// Resets the per-frame allocator to the initial values of a specific frame
		inline void RestartAllocatorForFrame(int frameIndex) {
			mallocOffset = frameIndex * frameCapacity;
			lastAvailableInBlock = (frameIndex + 1) * frameCapacity;
		}

		// Allocate some descriptors in current frame.
		inline long AllocateInFrame(int descriptors) {
			auto result = InterlockedExchangeAdd(&mallocOffset, descriptors);
			if (mallocOffset >= lastAvailableInBlock)
				throw Exception::FromError(Errors::RunOutOfMemory, "Descriptor heap");
			return result;
		}

		inline int AllocatePersistent() {
			return capacity - 1 - persistentAllocator->Allocate();
		}

		inline void FreePersistent(int index) {
			persistentAllocator->Free(capacity - 1 - index);
		}

		GPUDescriptorHeapManager(
			DX_Device device,
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			int capacity,
			int persistentCapacity,
			int buffers);

		~GPUDescriptorHeapManager() {
			delete persistentAllocator;
		}
		// Gets the cpu descriptor handle to access to a specific index slot of this descriptor heap
		inline D3D12_GPU_DESCRIPTOR_HANDLE getGPUVersion(int index) { return D3D12_GPU_DESCRIPTOR_HANDLE{ startGPU + index * size }; }
		// Gets the gpu descriptor handle to access to a specific index slot of this descriptor heap
		inline D3D12_CPU_DESCRIPTOR_HANDLE getCPUVersion(int index) { return D3D12_CPU_DESCRIPTOR_HANDLE{ startCPU + index * size }; }

		inline DX_DescriptorHeap getInnerHeap() { return heap; }
	};

#pragma endregion

#pragma region Resources

	/// <summary>
	/// Internal wrapper of a DX 12 Resource.
	/// </summary>
	struct wResource {
		static int SizeOfFormatElement(DXGI_FORMAT format);

		wResource(
			DX_Device device,
			DX_Resource resource,
			const D3D12_RESOURCE_DESC& desc,
			int elementStride,
			int elmeentCount,
			D3D12_RESOURCE_STATES initialState,
			CPUAccessibility cpuAccessibility);

		~wResource() {
			if (resource == nullptr)
				return;

			delete[] pLayouts;
			delete[] pNumRows;
			delete[] pRowSizesInBytes;
		}

		DX_Device device;

		// Resource being wrapped
		DX_Resource resource;
		// Resourve version for uploading purposes (in case the CPU can not write directly).
		DX_Resource uploading;
		// Resource version for downloading purposes (in case the CPU can not read directly).
		DX_Resource downloading;

		// Resource description at creation
		D3D12_RESOURCE_DESC desc;
		// Gets the original element stride width of the resource
		int elementStride = 0;
		// Gets the original element count for this resource
		int elementCount = 1;
		// Last used state of this resource on the GPU
		D3D12_RESOURCE_STATES LastUsageState;

		// For uploading/downloading data, a permanent CPU mapped version is cache to avoid several map overheads...
		byte* permanentUploadingMap = nullptr;
		byte* permanentDownloadingMap = nullptr;

		int subresources;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts;
		unsigned int* pNumRows;
		UINT64* pRowSizesInBytes;
		UINT64 pTotalSizes;
		CPUAccessibility cpuaccess;

		// Used to synchronize access to the resource will mapping
		Mutex mutex;

		// How many resource views are referencing this resource. This resource is automatically released by the last view using it
		// when no view is referencing it.
		int references = 0;

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

		// Resolves an uploading CPU-Writable version for this resource
		void __ResolveUploading();

		// Resolves a downloading CPU-Readable version for this resource
		void __ResolveDownloading();

		// Writes data in specific subresources at mapped uploading memory
		void WriteToMapped(byte* data, wResourceView* view, bool flipRows = false);

		// copies de specific range of subresources from uploading to the gpu resource
		void UpdateResourceFromMapped(DX_CommandList cmdList, wResourceView* view);

		// Reads data from specific subresources at mapped downloading memory
		void ReadFromMapped(byte* data, wResourceView* view, bool flipRows = false);

		// copies de specific range of subresources from gpu resource to the downloading version
		void UpdateMappedFromResource(DX_CommandList cmdList, wResourceView* view);

		// Writes data in the first resource at specific region of the mapped data...
		void WriteRegionToMappedSubresource(byte* data, wResourceView* view, const D3D12_BOX& region, bool flipRows = false);

		// copies de specific region of the single-subresource from uploading to the gpu resource
		void UpdateRegionFromUploading(DX_CommandList cmdList, wResourceView* view, const D3D12_BOX& region);

		// Reads data from the first resource from specific region...
		void ReadRegionFromMappedSubresource(byte* data, wResourceView* view, const D3D12_BOX& region, bool flipRows = false);

		void UpdateRegionToDownloading(DX_CommandList cmdList, wResourceView* view, const D3D12_BOX& region);

		// Creates a barrier in a cmd list to trasition usage states for this resource.
		void AddBarrier(DX_CommandList cmdList, D3D12_RESOURCE_STATES dst);

		// Creates a barrier in a cmd list to trasition usage states for this resource.
		void AddUAVBarrier(DX_CommandList cmdList);
	};

	/// <summary>
	/// Internal wrapper of a DX 12 Resource View
	/// </summary>
	struct wResourceView {
		wResource* w_resource;
		wDevice* w_device;

		wResourceView(wDevice* w_device, wResource* w_resource) :w_device(w_device), w_resource(w_resource) {}

		// Mip start of this view slice
		int mipStart = 0;
		// Mip count of this view slice
		int mipCount = 1;
		// Array start of this view slice.
		// If current view is a Buffer, then this variable represents the first element
		int arrayStart = 0;
		// Array count of this view slice
		// If current view is a Buffer, then this variable represents the number of elements.
		int arrayCount = 1;

		// Represents the element stride in bytes of the underlaying resource in case of a Buffer.
		int elementStride = 0;

		// Cached cpu handle created for Shader resource view
		int srv_cached_handle = 0;
		// Cached cpu handle created for Unordered Access view
		int uav_cached_handle = 0;
		// Cached cpu hanlde created for Constant Buffer view
		int cbv_cached_handle = 0;
		// Cached cpu handle created for a Render Target View
		int rtv_cached_handle = 0;
		// Cached cpu handle created for DepthStencil buffer view
		int dsv_cached_handle = 0;
		// Mask of valid cached handles 1-srv, 2-uav, 4-cbv, 8-rtv, 16-dsv
		unsigned int handle_mask = 0;

		// Mutex object used to synchronize handle creation.
		Mutex mutex;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleFor(D3D12_DESCRIPTOR_RANGE_TYPE type);

#pragma region Creating view handles and caching

		void CreateRTVDesc(D3D12_RENDER_TARGET_VIEW_DESC& d);

		void CreateSRVDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& d);

		void CreateUAVDesc(D3D12_UNORDERED_ACCESS_VIEW_DESC& d);

		void CreateVBV(D3D12_VERTEX_BUFFER_VIEW& desc);

		void CreateIBV(D3D12_INDEX_BUFFER_VIEW& desc);

		void CreateDSVDesc(D3D12_DEPTH_STENCIL_VIEW_DESC& d);

		void CreateCBVDesc(D3D12_CONSTANT_BUFFER_VIEW_DESC& d);

		int getSRV();

		int getUAV();

		int getCBV();

		int getRTV();

		int getDSV();

		D3D12_CPU_DESCRIPTOR_HANDLE getDSVHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE getRTVHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE getSRVHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE getCBVHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE getUAVHandle();

#pragma endregion

		// Gets the current view dimension of the resource.
		D3D12_RESOURCE_DIMENSION ViewDimension = D3D12_RESOURCE_DIMENSION_UNKNOWN;

		wResourceView* createSlicedClone(
			int mipOffset, int mipNewCount,
			int arrayOffset, int arrayNewCount);
	};

#pragma endregion

#pragma region Imaging

	struct TextureData {
		int const Width;
		int const Height;
		int const MipMaps;
		union {
			int const ArraySlices;
			int const Depth;
		};
		DXGI_FORMAT const Format;
		byte* const Data;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT* footprints;

		long long const DataSize;
		bool const FlipY;
		D3D12_RESOURCE_DIMENSION Dimension;

		TextureData(int width, int height, int mipMaps, int slices, DXGI_FORMAT format, byte* data, long long dataSize, bool flipY = false);

		TextureData(int width, int height, int depth, DXGI_FORMAT format, byte* data, long long dataSize, bool flipY = false);

		~TextureData();

		static gObj<TextureData> CreateEmpty(int width, int height, int mipMaps, int slices, DXGI_FORMAT format);

		static gObj<TextureData> CreateEmpty(int width, int height, int depth, DXGI_FORMAT format);

		static gObj<TextureData> LoadFromFile(const char* filename);

		static void SaveToFile(gObj<TextureData> data, const char* filename);

	private:
		int pixelStride;

	};

#pragma endregion

#pragma region Device and SwapChain Wrappers

	struct wDevice {

		DX_Device device;
		DX_SwapChain swapChain;
		ID3D12Debug* debugController;
		PresenterDescription desc;
		wScheduler* scheduler;

		gObj<Texture2D>* RenderTargets;
		D3D12_CPU_DESCRIPTOR_HANDLE* RenderTargetsRTV;

		wResource* CreateResource(
			const D3D12_RESOURCE_DESC& desc,
			int elementStride,
			int elementCount,
			D3D12_RESOURCE_STATES initialState,
			D3D12_CLEAR_VALUE* clearing = nullptr,
			CPUAccessibility accessibility = CPUAccessibility::None
		);

#pragma region Descriptor Heaps

		// -- Shader visible heaps --

		// Descriptor heap for gui windows and fonts
		gObj<GPUDescriptorHeapManager> gui_csu;

		// -- GPU heaps --

		// Descriptor heap for CBV, SRV and UAV
		gObj<GPUDescriptorHeapManager> gpu_csu;
		// Descriptor heap for sampler objects
		gObj<GPUDescriptorHeapManager> gpu_smp;

		// -- CPU heaps --

		// Descriptor heap for render targets views
		gObj<CPUDescriptorHeapManager> cpu_rt;
		// Descriptor heap for depth stencil views
		gObj<CPUDescriptorHeapManager> cpu_ds;
		// Descriptor heap for cbs, srvs and uavs in CPU memory
		gObj<CPUDescriptorHeapManager> cpu_csu;
		// Descriptor heap for sampler objects in CPU memory
		gObj<CPUDescriptorHeapManager> cpu_smp;

#pragma endregion

#pragma region Tool methods

		// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
		// If no such adapter can be found, *ppAdapter will be set to nullptr.
		_Use_decl_annotations_
			void GetHardwareAdapter(CComPtr<IDXGIFactory4> pFactory, CComPtr<IDXGIAdapter1>& ppAdapter)
		{
			IDXGIAdapter1* adapter;
			ppAdapter = nullptr;

			for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see if the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}

			ppAdapter = adapter;
		}

		// Enable experimental features required for compute-based raytracing fallback.
		// This will set active D3D12 devices to DEVICE_REMOVED state.
		// Returns bool whether the call succeeded and the device supports the feature.
		bool EnableComputeRaytracingFallback(CComPtr<IDXGIAdapter1> adapter)
		{
			ID3D12Device* testDevice;
			UUID experimentalFeatures[] = { D3D12ExperimentalShaderModels };

			return SUCCEEDED(D3D12EnableExperimentalFeatures(ARRAYSIZE(experimentalFeatures), experimentalFeatures, nullptr, nullptr))
				&& SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&testDevice)));
		}

		// Returns bool whether the device supports DirectX Raytracing tier.
		bool IsDirectXRaytracingSupported(IDXGIAdapter1* adapter)
		{
			ID3D12Device* testDevice;
			D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

			return SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&testDevice)))
				&& SUCCEEDED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData)))
				&& featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
		}

		gObj<ResourceView> ResolveNullView(wDevice* wrapper, D3D12_RESOURCE_DIMENSION dimension) {
			static gObj<ResourceView> nullBuffer = nullptr;
			static gObj<ResourceView> nullTexture1D = nullptr;
			static gObj<ResourceView> nullTexture2D = nullptr;
			static gObj<ResourceView> nullTexture3D = nullptr;

			switch (dimension)
			{
			case D3D12_RESOURCE_DIMENSION_BUFFER:
				return nullBuffer ? nullBuffer : nullBuffer = ResourceView::CreateNullView(wrapper, dimension);
			case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
				return nullTexture1D ? nullTexture1D : nullTexture1D = ResourceView::CreateNullView(wrapper, dimension);
			case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
				return nullTexture2D ? nullTexture2D : nullTexture2D = ResourceView::CreateNullView(wrapper, dimension);
			case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
				return nullTexture3D ? nullTexture3D : nullTexture3D = ResourceView::CreateNullView(wrapper, dimension);
			default:
				return nullptr;
			}
		}

		D3D12_CPU_DESCRIPTOR_HANDLE ResolveNullRTVDescriptor() {
			return ResolveNullView(this, D3D12_RESOURCE_DIMENSION_TEXTURE2D)->w_view->getRTVHandle();
		}



#pragma endregion

		void Initialize(const PresenterDescription& desc);
	};

#pragma endregion

}


#endif
