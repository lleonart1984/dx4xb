enum class PoolingType {
	MaxFloat,
	MinFloat,
	AveFloat,
	AveFloat4
};

/// <summary>
/// Proxy class to setup Pooling compute shader.
/// </summary>
struct Pooling3DPipeline : public ComputePipeline {

	PoolingType type;
	Pooling3DPipeline(PoolingType type) {
		this->type = type;
	}

	void Setup() {
		switch (this->type)
		{
		case PoolingType::MaxFloat:
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\ML\\Pooling\\MaxPooling3DFloat_CS.cso"));
			break;
		case PoolingType::MinFloat:
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\ML\\Pooling\\MinPooling3DFloat_CS.cso"));
			break;
		case PoolingType::AveFloat:
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\ML\\Pooling\\AvePooling3DFloat_CS.cso"));
			break;
		case PoolingType::AveFloat4:
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\ML\\Pooling\\AvePooling3DFloat4_CS.cso"));
			break;
		}
	}

	gObj<Texture3D> Src;
	gObj<Texture3D> Dst;

	virtual void Bindings(gObj<ComputeBinder> binder) {
		binder->OnDispatch();
		binder->SRV(0, Src);
		binder->UAV(0, Dst);
	}
};
