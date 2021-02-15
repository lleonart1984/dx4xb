// Top level structure with the scene
RaytracingAccelerationStructure Scene : register(t0, space0);

RWTexture2D<float4> Output : register(u0);

cbuffer Transforms : register(b0)
{
	float4x4 FromProjectionToWorld;
};

struct RayPayload // Only used for raycasting
{
	int Index;
};

/// Use RTX TraceRay to detect a single intersection. No recursion is necessary
bool Intersect(float3 P, float3 D, out int tIndex) {
	RayPayload payload = (RayPayload)0;
	RayDesc ray;
	ray.Origin = P;
	ray.Direction = D;
	ray.TMin = 0;
	ray.TMax = 100.0;
	TraceRay(Scene, RAY_FLAG_FORCE_OPAQUE, 0xFF, 0, 0, 0, ray, payload);
	tIndex = payload.Index;
	return tIndex >= 0;
}

[shader("closesthit")]
void OnClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
	payload.Index = PrimitiveIndex();
}

[shader("miss")]
void OnMiss(inout RayPayload payload)
{
	payload.Index = -1;
}

float3 GetColor(int complexity) {

	if (complexity == 0)
		return float3(0, 0, 0);

	//return float3(1,1,1);

	float level = (complexity % 100) * 12 / 100.0;

	float3 stopPoints[13] = {
		float3(0, 0, 128) / 255.0, //1
		float3(49, 54, 149) / 255.0, //2
		float3(69, 117, 180) / 255.0, //4
		float3(116, 173, 209) / 255.0, //8
		float3(171, 217, 233) / 255.0, //16
		float3(224, 243, 248) / 255.0, //32
		float3(255, 255, 191) / 255.0, //64
		float3(254, 224, 144) / 255.0, //128
		float3(253, 174, 97) / 255.0, //256
		float3(244, 109, 67) / 255.0, //512
		float3(215, 48, 39) / 255.0, //1024
		float3(165, 0, 38) / 255.0,  //2048
		float3(200, 0, 175) / 255.0  //4096
	};

	if (level >= 12)
		return stopPoints[12];

	return lerp(stopPoints[(int)level], stopPoints[(int)level + 1], level % 1);
}

[shader("raygeneration")]
void RayGen() {
	uint2 raysIndex = DispatchRaysIndex();
	uint2 raysDimensions = DispatchRaysDimensions();
	float2 coord = (raysIndex.xy + 0.5) / raysDimensions;
	float4 ndcP = float4(2 * coord - 1, 0, 1);
	ndcP.y *= -1;
	float4 ndcT = ndcP + float4(0, 0, 1, 0);
	float4 viewP = mul(ndcP, FromProjectionToWorld);
	viewP.xyz /= viewP.w;
	float4 viewT = mul(ndcT, FromProjectionToWorld);
	viewT.xyz /= viewT.w;
	float3 P = viewP.xyz;
	float3 D = normalize(viewT.xyz - viewP.xyz);

	int index = -1;
	if (Intersect(P, D, index))
	{
		Output[raysIndex] = float4(GetColor(index), 1);
	}
	else
	{
		Output[raysIndex] = float4(D, 1);
	}
}

