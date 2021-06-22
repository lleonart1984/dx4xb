#ifndef pi
#define pi 3.1415926535897932384626433832795
#endif

static const float invPDF[] = {
-1.        , -0.98231404, -0.95349017, -0.91490359, -0.87097013,
	   -0.79390977, -0.56714868, -0.46876006, -0.2960549 , -0.18378139,
	   -0.10359053, -0.01734251,  0.09067203,  0.17330505,  0.22743219,
		0.26979452,  0.30702644,  0.34242521,  0.37846251,  0.41810321,
		0.46673304,  0.53252807,  0.59305736,  0.63050919,  0.65666263,
		0.67725714,  0.69465512,  0.71002431,  0.72403314,  0.73711072,
		0.74956167,  0.76162529,  0.77351178,  0.78543112,  0.79762557,
		0.81042218,  0.82434729,  0.84046173,  0.86193905,  0.93629957,
		0.95769547,  0.96713289,  0.97366463,  0.97876912,  0.98300484,
		0.98664897,  0.98986106,  0.99274208,  0.99536034,  0.99776437,
		1.
};

float invertcdf(float xi) {


	int index = xi * 50;
	float alpha = (xi * 50) % 1;
	//return invPDF[index];

	return lerp(invPDF[index], invPDF[index + 1], alpha);
}
static const float lmPDF[50] =
{
	0.8487863547791894, 0.5235445473753759, 0.4718880009559122, 0.36675921186104893, 0.20963047815457195, 0.07521298095037061, 0.012304367318623385, 0.024413169247927528, 0.08377303265133451, 0.15297397190381537, 0.20196286647207723, 0.2164633732173325, 0.19843950748663855, 0.1612680848539045, 0.12271463078918945, 0.0983178200419308, 0.09687936979861367, 0.1187714586006942, 0.1569261541143932, 0.19977461001880858, 0.23509567520839017, 0.25369747504433676, 0.25203924119126114, 0.2332321052209363, 0.20625786519225378, 0.1836386147293225, 0.17811373591355023, 0.19908703184353965, 0.24966850506661423, 0.3250460906307284, 0.41269603721811915, 0.49460745386349764, 0.5513011831764276, 0.5670188438534183, 0.535101636767372, 0.4623264404643771, 0.37087036883324387, 0.2966796752218362, 0.28336544155560617, 0.3713793848895897, 0.5831952949524398, 0.9066278118691614, 1.2804215131395589, 1.5891159061607125, 1.678392631265756, 1.4083651142654479, 0.7716881007368852, 0.11759149049260904, 0.5443575079579972, 4.554719266774118
};
float calcpdf(float cosTheta) {
	float xi = cosTheta * 0.5 + 0.5;
	int index = xi * 49;
	float alpha = (xi * 49) % 1;
	return lerp(lmPDF[index], lmPDF[index + 1], alpha);
	//return lmPDF[int(49.999 * (cosTheta * 0.5 + 0.5))];
}

float EvalPhase(float3 D, float3 L) {
	float cosTheta = dot(normalize(D), normalize(L));
	return calcpdf(cosTheta) / (2 * pi);
}

void CreateOrthonormalBasis(float3 D, out float3 B, out float3 T) {
	D = normalize(D);
	float3 other = abs(D.z) >= 0.999 ? float3(1, 0, 0) : float3(0, 0, 1);
	B = normalize(cross(other, D));
	T = normalize(cross(D, B));
}

//float random();

void GeneratePhase(float3 D, out float3 L, out float pdf) {
	float phi = random() * 2 * pi;
	float cosTheta = invertcdf(random());
	float sinTheta = sqrt(max(0, 1.0f - cosTheta * cosTheta));

	float3 t0, t1;
	CreateOrthonormalBasis(D, t0, t1);

	L = (sinTheta * sin(phi) * t0 + sinTheta * cos(phi) * t1 +
		cosTheta * D);

	pdf = calcpdf(cosTheta);
}

float3 ImportanceSamplePhase(float GFactor, float3 D) {
	float phi = random() * 2 * pi;
	float cosTheta = invertcdf(random());
	float sinTheta = sqrt(max(0, 1.0f - cosTheta * cosTheta));

	float3 t0, t1;
	CreateOrthonormalBasis(D, t0, t1);

	return (sinTheta * sin(phi) * t0 + sinTheta * cos(phi) * t1 +
		cosTheta * D);
}

