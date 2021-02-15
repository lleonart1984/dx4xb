#ifndef RANDOMS_H
#define RANDOMS_H

static uint4 rng_state;

uint TausStep(uint z, int S1, int S2, int S3, uint M)
{
	uint b = (((z << S1) ^ z) >> S2);
	return ((z & M) << S3) ^ b;
}
uint LCGStep(uint z, uint A, uint C)
{
	return A * z + C;
}

float HybridTaus()
{
	rng_state.x = TausStep(rng_state.x, 13, 19, 12, 4294967294);
	rng_state.y = TausStep(rng_state.y, 2, 25, 4, 4294967288);
	rng_state.z = TausStep(rng_state.z, 3, 11, 17, 4294967280);
	rng_state.w = LCGStep(rng_state.w, 1664525, 1013904223);

	return 2.3283064365387e-10 * (rng_state.x ^ rng_state.y ^ rng_state.z ^ rng_state.w);
}

float random() {
	return HybridTaus();
}

void StartRandomSeedForRay(uint2 gridDimensions, int maxBounces, uint2 raysIndex, int bounce, int frame) {
	uint index = 0;
	uint dim = 1;
	index += raysIndex.x * dim;
	dim *= gridDimensions.x;
	index += raysIndex.y * dim;
	dim *= gridDimensions.y;
	index += bounce * dim;
	dim *= maxBounces;
	index += frame * dim;

	rng_state = uint4(index, index, index, index);

	for (int i = 0; i < 23 + index % 13; i++)
		random();
}

uint4 getRNG() {
	return rng_state;
}

void setRNG(uint4 state) {
	rng_state = state;
}

float3 randomHSDirection(float3 N, out float NdotD)
{
	float r1 = random();
	float r2 = random() * 2 - 1;
	float sqrR2 = r2 * r2;
	float two_pi_by_r1 = two_pi * r1;
	float sqrt_of_one_minus_sqrR2 = sqrt(1.0 - sqrR2);
	float x = cos(two_pi_by_r1) * sqrt_of_one_minus_sqrR2;
	float y = sin(two_pi_by_r1) * sqrt_of_one_minus_sqrR2;
	float z = r2;
	float3 d = float3(x, y, z);
	NdotD = dot(N, d);
	d *= (NdotD < 0 ? -1 : 1);
	NdotD *= NdotD < 0 ? -1 : 1;
	return d;
}

void CreateOrthonormalBasis2(float3 D, out float3 B, out float3 T) {
	float3 other = abs(D.z) >= 0.999 ? float3(1, 0, 0) : float3(0, 0, 1);
	B = normalize(cross(other, D));
	T = normalize(cross(D, B));
}


float3 randomDirection(float3 D) {
	float r1 = random();
	float r2 = random() * 2 - 1;
	float sqrR2 = r2 * r2;
	float two_pi_by_r1 = two_pi * r1;
	float sqrt_of_one_minus_sqrR2 = sqrt(1.0 - sqrR2);
	float x = cos(two_pi_by_r1) * sqrt_of_one_minus_sqrR2;
	float y = sin(two_pi_by_r1) * sqrt_of_one_minus_sqrR2;
	float z = r2;

	float3 t0, t1;
	CreateOrthonormalBasis2(D, t0, t1);

	return t0 * x + t1 * y + D * z;
}

float3 randomDirectionWrong()
{
	while (true)
	{
		float3 x = float3(random(), random(), random()) * 2 - 1;
		float lsqr = dot(x, x);
		float s;
		if (lsqr > 0.01 && lsqr < 1)
			return normalize(x);// randomHSDirection(x / sqrt(lsqr), s);
	}
	/*
	float r1 = random();
	float r2 = random() * 2 - 1;
	float sqrR2 = r2 * r2;
	float two_pi_by_r1 = two_pi * r1;
	float sqrt_of_one_minus_sqrR2 = sqrt(max(0, 1.0 - sqrR2));
	float x = cos(two_pi_by_r1) * sqrt_of_one_minus_sqrR2;
	float y = sin(two_pi_by_r1) * sqrt_of_one_minus_sqrR2;
	float z = r2;
	return float3(x, y, z);*/
}

float2 BM_2() {
	float u1 = 1.0 - random(); //uniform(0,1] random doubles
	float u2 = 1.0 - random();
	float r = sqrt(-2.0 * log(max(0.0000000001, u1)));
	float t = 2.0 * pi * u2;
	return r * float2(cos(t), sin(t));
}

float4 BM_4() {
	float2 u1 = 1.0 - float2(random(), random()); //uniform(0,1] random doubles
	float2 u2 = 1.0 - float2(random(), random());

	float2 r = sqrt(-2.0 * log(max(0.0000000001, u1)));
	float2 t = 2.0 * pi * u2;
	return float4(r.x * float2(cos(t.x), sin(t.x)), r.y * float2(cos(t.y), sin(t.y)));
}

float randomStdNormal() {
	return BM_2().x;
}

float3 randomStdNormal3() {
	return BM_4().xyz;
}

float2 randomStdNormal2() {
	return BM_2();
}

float4 randomStdNormal4() {
	return BM_4();
}

float gauss(float mu = 0, float sigma = 1) {
	return mu + sigma * randomStdNormal(); //random normal(mean,stdDev^2)
}

#endif