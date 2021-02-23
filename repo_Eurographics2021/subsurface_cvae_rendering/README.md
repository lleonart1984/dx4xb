# dx4xb

The dx4xb project (*DirectX For eXpert Beginners*) project allows to abstract to developers from the specific details for driving DX12-based applications. This programming can be bothersome for simple tasks such as resources handling, synchronization, and other graphic-related tasks. This library serves as a façade for accessing to such functionalities and at the same time, uses good practices for DX managing guided using most of the Do’s and Don’ts NVidia advices.

Some of the highlighted features added so far includes:

**Triple buffering support:** dx4xb can use a tree Backbuffer swap chain and uses synchronization and allocators to exploit CPU and GPU workloads efficiently.

**Basic synchronization tools:** dx4xb hides all proposed DX12 synchronization mechanisms and just exposes a Signal concept. Signals can be trigger by a technique and can be waitable for CPU programs to wait for GPU termination. Also, expose functions to flush pending CPU work to the GPU. All resource access barrier are managed by the engine.

**Multi-engine support:** dx4xb encapsulates the graphics pipeline usage through `Technique` concept and execution of process. Processes are methods receiving a command list manager. Depending on the type of the command list (Copy, Compute, Graphics, Raytracing) a different engine is used (i.e. Command Queue).

**Multi-threading support:** dx4xb device manager has several threads for command population. All asynchronous processes are queued in producer-consumer queue that deploys process execution across all threads. It exposes functions to flush all pending work to the GPU and wait (on the CPU) for their termination.

**Typed Resource View:** DX12 unifies resource concept in just one type (`ID3D12Resource`). Nevertheless, dx4xb wraps this concept internally and only exposes resource views. Resource views are typed versions of a resource that has the information of the “view” (Descriptors) and other external functionalities for sub-resource managing, slicing, among others. Sub-resources can be treated the same way of resources.

Typed resource view helps in other aspects such as binding, root descriptor construction and static type-check of resource handling.

**Customizable Pipeline state objects:** In dx4xb there is the `Pipeline` concept with two roles. Representing bindings to the pipeline (resources, constants, samplers, render targets, depth buffers) used to construct the Root Signature object and, in the other hand, manage all settable states to the pipeline exposing functionalities to setup, such as Input Layout, Depth Tests, Rasterizer state, shader stages, etc., used to create the `ID3D12PipelineState` or the `ID3D12StateObject`.

**DXR support:** dx4xb can access to DXR engine in DX12. DXR can be used through the framework in a friendly manner hiding bothersome tasks and settings.

The type `DeviceManager` encapsulates the DX12 Device, the Swap Chains and the scheduler that manages allocators, command lists and command queues (including signaling and synchronization).

There is two concepts that can be used and extend `DeviceManager` functionalities.


## Presenter Class

Presenter object is the "factory" class proposed for rendering through DX12. This class allows to load Techniques, manage triple buffering and other specific tasks for render output presenting.

Use `Load` method to load and initialize a technique, and the use `Present` method to execute the graphic process represented in the technique. Most of the objects defined in dx4xb needs to be managed using smart pointers. Pointers to dx4xb graphic objects are managed using `gObj<T>` wrapper.

Next code shows a presenter object creation using a handle to the rendered window. Additional parameters may be included for full screen, number of buffers in swap chain and a to indicate if a warp device should be used instead of hardware.

```c++
PresenterDescription pDesc;
pDesc.hWnd = hwnd;
gObj<Presenter> presenter = Presenter::Create(pDesc);
```

​	The graphic loop presenting a specific technique can be as follow:

```c++
gObj<MyTechnique> technique;
presenter->Load(technique);

while(true){
	presenter->BeginFrame();
	presenter->ExecuteTechnique(technique);
	presenter->EndFrame();
}
```

## Technique class

A technique is a class representing the developer's usage intention of the DX12 API. The technique has three special methods:

**Constructor**: Use the constructor of a technique to initialize the object with required parameters. Mostly will be used with constants for the technique (will never change). Creational parameters can be passed to the Technique with the additional variadic parameters of `Load` method.

**OnLoad**: This method will be called once the first time the technique is presented. You may use of any DX12 functionality here (even drawing!), but normally will be used to load assets, preprocess scene, populate bundles, and load resource data. Create pipeline objects or other subtechniques objects.

**OnDispatch**: This method will be called every time the technique is executed in a presenter object or another parent technique. You may use this method to draw in the render target (accessible throught `CurrentRenderTarget()` method).

Both methods (`OnLoad` and `OnDispatch`) can access to all functions in `DeviceManager` type used to execute graphics processes. Any method receiving a `CommandListManager` subclass can be considered as a graphics process. The main role of `OnLoad` and `OnDispatch` methods is to call to execute other methods as graphic process. This is the common usage of `PopulateCommandList` method in other DX12 engines.

Graphics process indicates with the parameter type which kind of engine will process the command list. This is the way dx4xb supports multiple engines usages. Next code shows a OnLoad method of a technique using a Copy engine to load some assets and a Compute engine to perform some preprocess computation.

```c++
void LoadAssets (gObj<CopyManager> manager) { /*...*/ }

void ComputeSomePreprocessedData(gObj<ComputeManager> manager) { /*...*/ }

void OnLoad() {
	Execute_OnGPU(LoadAssets);
	Execute_OnGPU(ComputeSomePreprocessedData);
}
```
Notice the `Execute_OnGPU` macro hides internally the wrapping of the method as a GPUProcess. To perform this loading stage asynchronously you just need to enqueue the graphic process asynchronously.

```c++
void LoadAssets (gObj<CopyManager> manager) { ... }

void ComputeSomePreprocessedData(gObj<ComputeManager> manager) { ... }

void Startup() {
    Execute_OnGPU_Async(LoadAssets);
    Execute_OnGPU_Async(ComputeSomePreprocessedData);
}
```

dx4xb manages internally a fixed number (defined normally 8) of workers. The main thread host the synchronous worker, and the other threads will wait for async tasks to do. This is implemented using a producer consumer queue internally and threads are waiting for new task to be enqueue. That means that an Execute_OnGPU_Async doesn't represent that a new thread is created but that the async process is enqueue for future execution in some engine.

### Synchronizing CPU-GPU work

The proposed mechanism for synchronization in dx4xb is the Signal concept. Every time the user use Execute_OnGPU_Async to enqueue some graphics process asynchronously, this process is enqueued. Using `Flush` method will produce all pending work to be completed (finish populating all async pending works in queue) and executed by their respective command queue (engines).​		

```c++
Execute_OnGPU_Async(LoadAssets);
Execute_OnGPU_Async(ComputeSomePreprocessedData);
Flush(EngineMask::All);
```

Notice that this code means: "I want every pending work for every engine to be sent to the GPU and will wait until that occurs", but that doesn't mean the work was really finished on the GPU. Nevertheless, this command serves as a barrier for different graphic process you want to synchronize on the GPU.

For CPU-GPU synchronization you will need to send a signal through the GPU that will "trigger" an event object in CPU.

For this purpose you may use the `Signal` object returned by the `Flush` method.

```c++
auto signal = Flush(EngineMask::All);
```

This method sends a signal through the specific engines and return an object that has all needed values for fencing strategy. Notice you may signal through just one engine and not the others, or through all engines.

Then the CPU can wait for a specific signal object to trigger the event using the `WaitFor` method.

```c++
signal->WaitFor();
```

> All device manager methods for work submission and synchronization must be used only in `OnLoad` and `OnDispatch` methods. Never use those methods inside a graphics process method because they are executed by demand and only represents command list populating processes.

## GPU Processes

All GPU work is considered as a GPU process in dx4xb. DX12 unified all work submission to the GPU using Command lists of different engines. The `CommandListManager` class in dx4xb is designed for such purpose. This class has several subclasses, one for each different command list type. When a technique calls to perform a graphics process, is really saying "populate a command list with the commands in that method".

All possible command managers are implemented in a chain inheritance extending the funcitionalities. That means that a `RaytracingManager` is a `GraphicsManager`, that is a `ComputeManager`, that is a `CopyManager`. However, this inheritance is not only to increaase the inferface definition, but also decides wich engine will be used internally for the execution.

Notice that in DX12, a COPY command queue is different from a COMPUTE command queue and therefore, a synchronization is needed to ensure some finished copy can be used in a compute shader. But, if you decide a ComputeManager in first place to make the copies, then you don't need to use any signaling because internally will be using the Compute engine.

```c++
void OnDispatch() {
	// Every frame perform a single process using a Graphics Engine
	Execute_OnGPU(SomeCopy);
	Flush(EngineMask::Copy).WaitFor();
	Execute_OnGPU(SomeComputeUsingTheCopy);
}

void SomeCopy(gObj<CopyManager> manager)  { ... }
void SomeComputeUsingTheCopy(gObj<ComputeManager> manager) { ... }
```
Equivalent solution using the same engine internally.
```c++
void OnDispatch() {
	// Every frame perform a single process using a Graphics Engine
	Execute_OnGPU(SomeCopy);
	Execute_OnGPU(SomeComputeUsingTheCopy);
}

void SomeCopy(gObj<ComputeManager> manager)  { ... }
void SomeComputeUsingTheCopy(gObj<ComputeManager> manager) { ... }
```

## Resource Management

DX12 proposes an unified resource handling through `ID3D12Resource` interface. However, in dx4xb it is used a typed version again. That means that there are several types for each kind of representation a resource may have. For instance, a Buffer, a Texture2D or even a Texture2DMS.

The idea is to hide the duality Resource-ResourceView from developers and export the managed object as a `ResourceView`. Resource views hides internally the real resource and exposes functions for querying description of the resource, slicing (gets a range of sub-resources), copying data to/from, among others functionalities.

Resources can be created by any `DeviceManager` object. Since technique is internally a device manager, a resource can be created any time. Every resource usage is protected via smart pointers using the `gObj<T>` wrapper.

```c++
gObj<Buffer> vertices = nullptr;
...
void OnLoad(){
	vertices = CreateBufferVB<VERTEX>(3);
}
```

Notice a convention in the naming, `Create(Type)(Usage)` generic in `<ElementType>`. The type can be `Buffer`, `Texture1D`, `Texture2D` or `Texture3D`. The usage, depending on the type: VB for vertex buffer, IB for index buffer, CB for constant buffer, SRV for a shader resource, UAV for an unordered access view , RT for render target, DSV for a depth-stencil buffer.

In order to fill a resource with data, it will be different if the resource is created with CPUAccessibility::Write or not. If that is the case, then the resource is in an Upload Heap and therefore updating the data with `Write` methods will be sufficent. If not, the resource internally manages a copy in a uploaded heap and we need to execute two methods, first `Write` to update the uploading version, then, using a CopyManager, call `ToGPU` method. The last function ensures that all modifications in the uploading version of a resource is copied on the gpu to the resource.

Next example shows how a vertex buffer can be created and copied to the resource on the gpu.

```c++
void OnLoad() {
	...	
	// Create a simple vertex buffer for a triangle
	vertices = CreateBufferVB<VERTEX>(3);
	vertices->Write({
		VERTEX(...),
		VERTEX(...),
		VERTEX(...)
	});
	// Performs a copying commanding execution for uploading data to resources
	Execute_OnGPU(UploadData);
}

// A copy engine can be used to populate buffers using GPU commands.
void UploadData(gObj<CopyManager> manager) {
	// Copies the uploading version to the gpu version
	manager->ToGPU(vertices);
}
```
Every resource view manages internally the different descriptors needed for each kind of view. For instance, a `Texture2D` used as a shader resource and then as a render target will cache both descriptors internally.

> This process is hidden from the developer, so you will never need to deal with descriptors, bindings, copying descriptors, descriptor heaps, null descriptors, visible and non-visible descriptor heaps, heaps ring-buffer allocations, resource barriers, resources uploading and readback versions, sub-resources footprints and other DX12 overwhelming stuff. You are very welcome.

`Write` and `Read` methods refers to the view slice, not the original resource. The next code allows to update only the vertices 6, 7 and 8 of a buffer.

```c++
gObj<Buffer> slice = vertices->Slice(6, 3);
slice->Write( { ... });
manager->ToGPU(slice);
```

### Textures

Texture objects can be created by different ways. First, a normal texture to represented an image.

```c++
myTexture = LoadTexture2D("c:\\Users\\...\\Desktop\\Models\\skybox.dds");
manager->ToGPU(myTexture);
```

This method creates the texture object using the data loaded from an image path. Imaging functionalities of dx4xb uses a project published in GitHub. DirectXTex. (link here).

If you need an empty texture you may use:

```c++
auto myTexture = CreateTexture2DUAV<float4>(512, 512);
```

This texture object is ready for UAV and Render Target purposes. You may indicate how many mipmaps to allocate an the initial state for the internal resource.

Depending on the resource kind, you may access to sub-resources and treat them as resources. Updating data to a resource and sub-resources, binding resources and sub-resources, are treated the same in dx4xb.

Imaging you have a texture array created representing a `CubeTexture`. You can use each different cube face texture as a render target in some process.

```c++
for (int i=0; i<6; i++)
{
	gObj<Texture2D> face = myCubeTexture->SliceArray(i, 1);
	RenderFace(i, face, manager);
}			
```

Next sample shows how to populate an array with all sub-resources of a Texture2D.

```c++
// Compute number of subresources
subresourcesCount = Texture->Mips()*Texture->ArrayLength();
// Array of subresources views
Subresources = new gObj<Texture2D>[subresourcesCount];

int index = 0;
for (int j = 0; j < Texture->ArrayLength(); j++)
	for (int i = 0; i < Texture->Mips(); i++)
		// Creates a subresource view for each posible array slice and mip slice.
        Subresources[index++] = Texture->Subresource(i, j);
```

Texture pixel format can be specified using regular types, vector types (`float2`, `float3`,`float4`, ...) and other types representing color components in an unsigned integer using different distributions, `RGBA` and `ARGB` structs. Scalar types such as `int`, `unsigned int`, `short`, `byte`, ..., are supported as well.

## Pipeline objects and resource bindings

DX proposes a new concept to setup most of the render states required during draw calls and dispatching, the `ID3D12PipelineState`. This object is constructed with all data regarded to states that will be set on the GPU to setup the graphics pipeline. These states include shaders, rasterizer states, depth test, stencil tests, input layout description, information about bound render targets, depth buffers, etc.

Other states needs to be setup outside pipeline state object, such as viewport, scissor rectangle, vertex buffer and index buffer bindings.

On the other hand, resources bindings to the GPU for shaders accessibility is obtained by creating descriptor heaps, and copying needed descriptors to that memory, manage a root signature to know how pipeline shaders can access to such resources, and setup the offset of each entry in a root signature to the descriptor heap.

In dx4xb there is a class designed to manage everything related to the pipeline object and also includes the binding purpose. So, all the work described below is hidden for the developer.

For dx4xb, a `StaticPipeline` type is a base class that builds a setting object as a mixin of all render state manager object can be set (Notice that depending on the pipeline usage you will require some states or others...). Nevertheless, a common usable version of `StaticPipeline` will be a `GraphicsPipeline` and we will start explaining from it.

### Pipeline Bindings for Graphics purposes

Every draw call needs a pipeline set before with the information about what shaders to set, rasterization options, etc.. You may define such settings in a single typed object inheriting from `GraphicsPipeline`.

This subclass has two methods to override:

**Setup**: This method will be invoked to set all values proposed for each pipeline state. I.e. rasterizer states (cull mode, fill mode). In this method, used shader codes must be set as well.

**Bindings**: This method will be used to collect all resource bindings proposed for this pipeline. All resources and samplers fields declared as bound in this method will be updated once the pipeline object is set or any dispatch/draw method is invoked.

Next code shows a sample of pipeline bindings implementation.

```c++
struct MyBasicPipeline : public GraphicsPipeline 
{
	// Render target this pipeline will use
	gObj<Texture2D> renderTarget;
	// Setup method is overriden to load shader codes and set other default settings
	void Setup() {
		// Loads a compiled shader object and set as vertex shader
		auto vsBytecode = ShaderLoader::FromFile("NoTransforms_VS.cso");
		set->VertexShader(vsBytecode);
		// Loads a compiled shader object and set as pixel shader
		auto psBytecode = ShaderLoader::FromFile("SimpleColor_PS.cso");
		set->PixelShader(psBytecode);
		// Setting the input layout of the pipeline
		set->InputLayout({
			VertexElement { VertexElementType::Float, 3, VertexElementSemantic::Position},
			VertexElement { VertexElementType::Float, 3, VertexElementSemantic::Color}
		});
	}
	void Bindings(gObj<GraphicsBinder> binder) {
		// Only one binding when this pipeline state object is set.
		binder->OnSet();
		binder->RTV(0, renderTarget);
	}
}
```

> `binder->OnSet` and `binder->OnDispatch` are used to indicate the next bindings will activate when set or dispatch respectively. Those methods will be only called during initialization and used to create root signature for the pipeline object state. Every time the pipeline object needs to be set or draw call requires for locals, the fields of the object are requested (accessing through the stored reference pointer) and bind the real resource.
>
> Therefore, it is more likely to have an implementation error in the `Bindings` method if we use conditionals or loops that can be referring to a value will change after initialization.

For binding use the following methods:

`OnSet`: Next bindings are bound when the pipeline is set.

`OnDispatch`: Next bindings are bound whenever a dispatch (compute, graphics or raytracing) is performeed.

`VertexShader`,...,`PixelShader`: Next bindings are specific for such stages.

`All`: Next bindings are valid for all stages. 

`Space(s)`: Next bindings use the specified space.

`CBV(slot, &resource)`: Specifies a constant buffer must be bound to the slot of a specific shader stage.

`SRV(slot, &resource)`: Specifies a shader resource view must be bound to the slot of a specific shader stage.

`SRV_Array(slot, &resources, &count)`: Specifies a range of shader resource views of certain length (count) must be bound to the slot of a specific shader stage.

`UAV(...)`: Bounds a resource as an UAV.

`RTV(...)`: Bounds a texture as a render target.

`DSV(...)`: Bounds a texture as depth stencil view.

`Static_SMP(...)`: Bounds a sampler object as a static sampler in the root signature. The sampler object will be queried at initialization so it must be a constant value.

### Building Custom Pipeline State Object

Pipeline state objects are built from small pieces named pipeline subobject states. Each subobject type has a flag indicating the type and the specific value to set after it. DX12 supports a flexible mechanism for building customs pipeline state object designing a struct to describe the stream with all pipeline object setup.

In dx4xb this feature is supported using a mixin strategy using inheritance and templates. In fact, the `GraphicsPipeline` type used in the example below is defined as follows.

> ```c++
> struct GraphicsPipeline : public StaticPipeline<
> 	DebugStateManager,
> 	VertexShaderStageStateManager,
> 	PixelShaderStageStateManager,
> 	DomainShaderStageStateManager,
> 	HullShaderStageStateManager,
> 	GeometryShaderStageStateManager,
> 	StreamOutputStateManager,
> 	BlendingStateManager,
> 	BlendSampleMaskStateManager,
> 	RasterizerStateManager,
> 	DepthStencilStateManager,
> 	InputLayoutStateManager,
> 	IndexBufferStripStateManager,
> 	PrimitiveTopologyStateManager,
> 	RenderTargetFormatsStateManager,
> 	DepthStencilFormatStateManager,
> 	MultisamplingStateManager,
> 	NodeMaskStateManager,
> 	RootSignatureStateManager
> > {
> };
> ```

Each state manager struct used here has two roles. First the expanded memory required by the DX12 pipeline object construction method. Second, each manager has specific tool methods to easily set/modify values for that specific state "slot". For instance, the `DepthStencilStateManager` has the functions shown next:

```c++
void NoDepthTest() { ... }
void DepthTest (enable, write, comparison) { ... }
void StencilTest(enable, readMask, writeMask) { ... }
void StencilOperationAtFront (...) { ... }
void StencilOperationAtBack (...) { ... }
```

Once the `PipelineBindings` subtype is defined a setting inner type is built with all the mixin and all function to read/set/modify states are accessible from it. Thus, the way to setup a pipeline binding object is via:

```c++
set->DepthTest(true, false, D3D12_COMPARISON_FUNC_GREATER);
```

You may use a custom pipeline object for reducing the complexity (memory) of the pipeline state object in your technique or simply to select subobject types with new functionalities available in DX12 such as the `DepthStencilWithDepthBoundsStateManager` object.

### Using a `Pipeline` object in an example

Once you have defined a specialization of a Pipeline according to your needs (normally in `OnLoad` method of your technique via `Load` method), you need to bind all required fields with the resources.

Some fields can be left `nullptr` if you are intending to bind a null descriptor to such slot. 

Global fields must be set before pipeline binding object is set on the pipeline. Locals fields must be set before each draw call.

```c++
void GraphicProcess(gObj<GraphicsManager> manager) {
	float4x4 view, proj;
	Camera->GetMatrices(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), view, proj);
	
	// Updates camera buffer each frame
	cameraCB->Write(Globals{ proj, view });
	manager->ToGPU(cameraCB);
	
	// Setting up per frame bindings
	pipeline->cameraCB = cameraCB;
	// this is necessary every frame because 3 different render targets 
	// might be used for triple-buffering support.
	pipeline->renderTarget = CurrentRenderTarget(); 
	
	manager->ClearRenderTarget(CurrentRenderTarget(), Backcolor);
	
	// Set the pipeline state object
	manager->SetPipeline(pipeline);
	// Set the viewport to the dimensions of the Backbuffer
	manager->Viewport(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
	// Set the vertex buffer object to the pipeline
	manager->VertexBuffer(vertices);
	// Set the index buffer to the pipeline
	manager->IndexBuffer(indices);
	
	for (int i = 0; i < subresourcesCount; i++)
	{
	    // Locals bindings
	    pipeline->transformCB = transforms[i];
	    pipeline->Texture = Subresources[i];
	
	    // Draw a quad with 4 vertices and 6 indices
	    manager->DrawIndexedTriangles(6);
	}
}
```



# Raytracing support

One of the main ideas behind dx4xb is to fully support all capabilities of DirectX12 API. Every technique that might be implemented over DirectX12 should be possible to implement using this facade. DXR is not an exception.

In dx4xb is presented a new command list manager named `RaytracingManager`. This command list manager works internally over a DIRECT-type command list (can be cast to `GraphicsManager` if necessary). Similarly to a `GraphicsManager` setup (setting a pipeline object), DXR works setting a `RaytracingPipeline` object into the pipeline. Then, developer can decide what ray-tracing program to activate and how to setup shaders (ray-generations, miss and hit groups) to obtain the desired ray-trace process.

To explain step-by-step proposed DXR support in dx4xb will be used the first example from the DirectX Samples repository. In this example a basic library is defined with three shaders: a shader to generate parallel rays in a normalized viewport (`MyRaygenShader`), a miss shader to paint a solid color when ray misses the geometry (`MyMissShader`), and a closest hit shader (`MyClosestHitShader`) to color ray payload with the barycentric coordinates of the intersection.

On the other hand, the applications needs to setup a single bottom level acceleration data structure with a simple triangle geometry, and a top level acceleration data structure with a single instance to this bottom level geometry. Then a global signature is used to refer to the Scene object (a shader resource view), and the output target (an UAV Texture2D). Only for illustration, a local signature is used to refer to a constant with some other application parameter (the viewports).

The shader was lightly modified from DirectX Samples and is shown next.

```c
struct Viewport
{
	float left;
	float top;
	float right;
	float bottom;
};

struct RayGenConstantBuffer {
	Viewport viewport;
	Viewport stencil;
};

typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
	float4 color;
};

RaytracingAccelerationStructure Scene : register(t0, space0);
RWTexture2D<float4> RenderTarget : register(u0);
ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b0);

bool IsInsideViewport(float2 p, Viewport viewport)
{
	return (p.x >= viewport.left && p.x <= viewport.right)
		&& (p.y >= viewport.top && p.y <= viewport.bottom);
}

[shader("raygeneration")]
void MyRaygenShader()
{
	float2 lerpValues = (float2)DispatchRaysIndex() / (float2)DispatchRaysDimensions();

// Orthographic projection since we're raytracing in screen space.
float3 rayDir = float3(0, 0, 1);
float3 origin = float3(lerpValues * 2 - 1, -1);

if (IsInsideViewport(origin.xy, g_rayGenCB.stencil))
{
	// Trace the ray.
	// Set the ray's extents.
	RayDesc ray;
	ray.Origin = origin;
	ray.Direction = rayDir;
	// Set TMin to a non-zero small value to avoid aliasing issues due to floating - point errors.
	// TMin should be kept small to prevent missing geometry at close contact areas.
	ray.TMin = 0.001;
	ray.TMax = 10000.0;
	RayPayload payload = { float4(0, 0, 1, 1) };
	TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);

	// Write the raytraced color to the output texture.
	RenderTarget[DispatchRaysIndex().xy] = payload.color;
}
else
{
	// Render interpolated DispatchRaysIndex outside the stencil window
	RenderTarget[DispatchRaysIndex().xy] = float4(lerpValues, 0, 1);
}

}

[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr)
{
	float3 barycentrics = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
	payload.color = float4(barycentrics, 1);
}

[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
	payload.color = float4(1, 0, 1, 1);
}
```

Next, we will explain how this example can be implemented using dx4xb concepts to support DXR caps.

## Ray-tracing Pipeline type

In dx4xb the `RaytracingPipeline` class is meant to be inherited specializing some methods to describe what DXIL libraries should loaded and which ray-tracing programs can be loaded from them. That means that it can be loaded 3 DXIL libraries and use all exported shaders to setup one single program.

The program's role is to stablish several shaders will be used in a dispatch rays call, therefore, shares the same global signature and local signatures by shader type.

Lets define for this example a new `RaytracingPipeline` subclass object named `DXRBasic`.

```c++
struct DXRBasic : public RaytracingPipeline {
...
}
```

This type must expose all shader objects (represented in dx4xb with a specific handle object), libraries and programs loaded.

### Loading a DXIL library

First, lets setup that our pipeline refers to a single DXIL library and uses its exports.

```c++
// Handles to store raytracing shaders
gObj<RayGenerationHandle> Generating;
gObj<HitGroupHandle> Hitting;
gObj<MissHandle> Missing;
// Setup method of a pipeline
void Setup() {
	// Load a library
	Code(ShaderLoader::FromFile("./Techniques/Examples/DX_RTX_Basic_RT.cso"));
	// Declare all shader exports and bind the name to the handle
	Shader(Generating, L"MyRaygenShader");
	Shader(Missing, L"MyMissShader");
	gObj<ClosestHitHandle> closestHit;
	Shader(closestHit, L"MyClosestHitShader");
	// Create a hit group with only the closest hit shader.
	HitGroup(Hitting, closestHit, nullptr, nullptr);
	...
}
```

Notice shaders are loaded (declared as exported by this library) and fill the handle present in pipeline object. 

### Defining a Ray Tracing Program

Once a library was loaded, a ray-tracing program can be loaded because required shaders will be available.

```c++
...// in Setup method
RTProgram(MainProgram);
```

Here, `MainProgram` is a handle used to refer to a program built with the shaders exposed in that library.

A posible way to accomplish this is to define the program type inside the pipeline definition.

```c++
struct Program : public RaytracingProgram<DXRBasic> {
	void Setup() {
		Payload(16);
		Shader(Context()->Generating);
		Shader(Context()->Missing);
		Shader(Context()->Hitting);
	}

	void Bindings(gObj<RaytracingBinder> binder) {
		binder->OnSet();
		binder->ADS(0, Context()->Scene); // Acceleration Data Structure
		binder->UAV(0, Context()->Output);
	}

	void RayGeneration_Bindings() override {
		CBV(0, Context()->RayGenConstantBuffer);
	}
};
gObj<Program> MainProgram;

// Resources used by the program/pipeline
gObj<InstanceCollection> Scene;
gObj<Texture2D> Output;
struct Viewport { float l, t, r, b; };
struct RayGenConstantBuffer {
	Viewport viewport;
	Viewport stencil;
} RayGenConstantBuffer;
``` 

The `Context()` method references to the RT pipeline that loaded this program. This is convenient to have only one object with the shader handles and required resources.

The Setup method of a program will load all required shaders and hit groups using those shaders. Also, all settings such as `StackSize`, `PayloadSize` and `AttributeSize`, must be fulfilled here if necessary.

The `Bindings` method will define all bindings required when this program is activated on the pipeline. This method is used to declare the global signature and all loaded shaders (and hit groups created) in this program are automatically associated to this root signature.

The method RayGeneration_Bindings needs to be overridden in order to specify this ray generation shader will use a local data store in the shader table (this is not necessary because only one ray-generation shader can be used in a dispatch ray but was used to illustrate how local signatures are created and associated to shaders).

This program creates a hit group with a closesthit shader and no intersection, neither anyhit shaders.

In next sections will be explained in detail the role of the `InstanceCollection` object. Now, consider this object as a reference (by any means necessary) to the top level acceleration data structure. 

The dx4xb façade hides internally in this object the construction of the DX12 state object. The other functionality of a program object is to handle internally shader tables and resource bindings (to the GPU or to the shader table entries).

## Building the Acceleration Structures

In dx4xb, the process of building the bottom level acceleration DS is handled by a type named `GeometryCollection`. This object can be created by a `RaytracingManager` and stores geometries (using `CreateGeometry` commands). After that, the GPU buffers can be built or updated using `ToGPU` method.

```c++
void BuildScene(gObj<RaytracingManager> manager) {
	// Build the Bottom Level ADS (Geometries)
	auto geometries = manager->CreateTriangleGeometries();
	geometries->CreateGeometry(vertices);
	// build bottom level ADS (false -> no update, true -> prefer fast raytracing)
	manager->ToGPU(geometries, false, true); 
    
	// Build the Top Level ADS (Instances)
	this->Scene = manager->CreateInstances();
	this->Scene->CreateInstance(geometries);
	// build top level ADS
	manager->ToGPU(this->Scene, false, true);
}
```

After this, the top level data structure can be created (because will reference the instances to the bottom level objects).

For this, the `InstanceCollection` object is used. This object can be created with the `CreateIntances` method in the `RaytracingManager` type.

Finally the `InstanceCollection` underlaying buffers (mostly top level acceleration data-structure) can be created using the `ToGPU` method.

## Dispatching rays

```c++
void Raytracing(gObj<RaytracingManager> manager) {
	pipeline->Output = rtRenderTarget;
	pipeline->Scene = this->Scene;
	pipeline->RayGenConstantBuffer = {
		{ -0.9, -0.9, 0.9, 0.9},
		{-0.9, -0.9, 0.9, 0.9}
	};
	manager->SetPipeline(pipeline);
	manager->SetProgram(pipeline->MainProgram);
	manager->SetRayGeneration(pipeline->Generating);
	manager->SetMiss(pipeline->Missing, 0);
	manager->SetHitGroup(pipeline->Hitting, 0);
	manager->DispatchRays(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
	manager->Copy(CurrentRenderTarget(), rtRenderTarget);
}
```

As shown in this code, the "rendering" process of the ray-tracing program can be as follows.

First, set all necessary resources bindings to the pipeline before set. Then set the pipeline object and after this activate (with `SetProgram`) the program to bind. Notice a program is not forced to have a single ray-generation shader, but it must be defined (set) later.

Set all necessary shaders into the pipeline (this will update necessary shader tables with the shader identifiers and bind local resources). Miss and HitGroup methods receives a way of indexing the current bindings. That means, if you have two different miss shaders you will need to assign for each all local bindings before calling to `SetMiss (..., index)`.

The dispatch rays command will use all shader tables of the active program and perform the ray-tracing process. Then a copy command can be used to save from the generated UAV object to the render target.

