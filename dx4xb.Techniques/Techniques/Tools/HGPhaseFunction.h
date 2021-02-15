
#define one_minus_g2 (1.0 - (GFactor) * (GFactor))
#define one_plus_g2 (1.0 + (GFactor) * (GFactor))
#define one_over_2g (0.5 / (GFactor))

float EvalPhase(float GFactor, float3 D, float3 L) {
	if (abs(GFactor) < 0.001)
		return 0.25 / pi;
	float cosTheta = dot(D, L);
	return 0.25 / pi * (one_minus_g2) / pow(one_plus_g2 - 2 * GFactor * cosTheta, 1.5);
}

float invertcdf(float GFactor, float xi) {
	float t = (one_minus_g2) / (1.0f - GFactor + 2.0f * GFactor * xi);
	return one_over_2g * (one_plus_g2 - t * t);
}

void CreateOrthonormalBasis(float3 D, out float3 B, out float3 T) {
	float3 other = abs(D.z) >= 0.999 ? float3(1, 0, 0) : float3(0, 0, 1);
	B = normalize(cross(other, D));
	T = normalize(cross(D, B));
}

//float random();

float3 ImportanceSamplePhase(float GFactor, float3 D) {
	if (abs(GFactor) < 0.001) {
		return randomDirection(-D);
		//return randomDirectionWrong();
	}

	float phi = random() * 2 * pi;
	float cosTheta = invertcdf(GFactor, random());
	float sinTheta = sqrt(max(0, 1.0f - cosTheta * cosTheta));

	float3 t0, t1;
	CreateOrthonormalBasis(D, t0, t1);

	return sinTheta * sin(phi) * t0 + sinTheta * cos(phi) * t1 +
		cosTheta * D;
}
