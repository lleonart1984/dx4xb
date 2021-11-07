#include "Parameters.h"

float3 SampleSkybox(float3 L) {

	float3 BG_COLORS[5] =
	{
		float3(0.1f, 0.05f, 0.01f), // GROUND DARKER BLUE
		float3(0.01f, 0.05f, 0.2f), // HORIZON GROUND DARK BLUE
		float3(0.8f, 0.9f, 1.0f), // HORIZON SKY WHITE
		float3(0.1f, 0.3f, 1.0f),  // SKY LIGHT BLUE
		float3(0.01f, 0.1f, 0.7f)  // SKY BLUE
	};

	// Sky like
	//float3 BG_COLORS[5] =
	//{
	//	float3(0.00f, 0.0f, 0.1f), // GROUND DARKER BLUE
	//	float3(0.01f, 0.05f, 0.2f), // HORIZON GROUND DARK BLUE
	//	float3(0.7f, 0.9f, 1.0f), // HORIZON SKY WHITE
	//	float3(0.1f, 0.3f, 1.0f),  // SKY LIGHT BLUE
	//	float3(0.01f, 0.1f, 0.7f)  // SKY BLUE
	//};

	float BG_DISTS[5] =
	{
		-1.0f,
		-0.1f,
		0.0f,
		0.4f,
		1.0f
	};

	float3 col = BG_COLORS[0];
	//for (int i = 1; i < 5; i++)
	col = lerp(col, BG_COLORS[1], smoothstep(BG_DISTS[0], BG_DISTS[1], L.y));
	col = lerp(col, BG_COLORS[2], smoothstep(BG_DISTS[1], BG_DISTS[2], L.y));
	col = lerp(col, BG_COLORS[3], smoothstep(BG_DISTS[2], BG_DISTS[3], L.y));
	col = lerp(col, BG_COLORS[4], smoothstep(BG_DISTS[3], BG_DISTS[4], L.y));

	// disney sky
	col = L.y > -0.2 ? float3(0.3, 0.6, 1)*0.6 : float3(0.2, 0.2, 0.1);

#ifdef USE_SKYBOX
	return col;
#else
	return 1;
#endif
}

float3 SampleSkybox(float3 P, float3 L) {

	float3 BG_COLORS[5] =
	{
		float3(0.1f, 0.05f, 0.01f), // GROUND DARKER BLUE
		float3(0.01f, 0.05f, 0.2f), // HORIZON GROUND DARK BLUE
		float3(0.8f, 0.9f, 1.0f), // HORIZON SKY WHITE
		float3(0.1f, 0.3f, 1.0f),  // SKY LIGHT BLUE
		float3(0.01f, 0.1f, 0.7f)  // SKY BLUE
	};

	// Sky like
	//float3 BG_COLORS[5] =
	//{
	//	float3(0.00f, 0.0f, 0.1f), // GROUND DARKER BLUE
	//	float3(0.01f, 0.05f, 0.2f), // HORIZON GROUND DARK BLUE
	//	float3(0.7f, 0.9f, 1.0f), // HORIZON SKY WHITE
	//	float3(0.1f, 0.3f, 1.0f),  // SKY LIGHT BLUE
	//	float3(0.01f, 0.1f, 0.7f)  // SKY BLUE
	//};

	float BG_DISTS[5] =
	{
		-1.0f,
		-0.1f,
		0.0f,
		0.4f,
		1.0f
	};

	float3 col = BG_COLORS[0];
	//for (int i = 1; i < 5; i++)
	col = lerp(col, BG_COLORS[1], smoothstep(BG_DISTS[0], BG_DISTS[1], L.y));
	col = lerp(col, BG_COLORS[2], smoothstep(BG_DISTS[1], BG_DISTS[2], L.y));
	col = lerp(col, BG_COLORS[3], smoothstep(BG_DISTS[2], BG_DISTS[3], L.y));
	col = lerp(col, BG_COLORS[4], smoothstep(BG_DISTS[3], BG_DISTS[4], L.y));

	float d = 10;// max(0, 0.4 - P.y) * 5;

	// disney sky
	//col = L.y > -0.2 ? float3(0.3, 0.4, 1) : float3(0.3, 0.35, 0.3)*0.5;

#ifdef USE_SKYBOX
	return col;
#else
	return 1.0;
#endif
}


float3 SampleLight(float3 L)
{
#ifdef USE_NARROW_LIGHTSOURCE
	int N = 480;
	//int N = 80;
#else
	int N = 10;
#endif
	float phongNorm = (N + 2) / (2 * 3.14159);
	return pow(max(0, dot(L, LightDirection)), N) * phongNorm * LightIntensity;
}