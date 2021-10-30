float3 GetColor(int complexity) {

	if (complexity == 0)
		return float3(0, 0, 0);

	//return float3(1,1,1);

	float level = (log2(complexity));

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


