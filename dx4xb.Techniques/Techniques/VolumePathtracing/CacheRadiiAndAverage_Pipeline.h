/// <summary>
/// Proxy class to setup Compute Majorant compute shader.
/// </summary>
struct CacheRadiiAndAverage : public ComputePipeline {
	void Setup() {
		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumePathtracing\\CacheRadiiAndAverage_CS.cso"));
	}

	gObj<Texture3D> Grid;
	gObj<Texture3D> Average;
	gObj<Texture3D> Radii;
	gObj<Texture3D> SphereGrid;

	virtual void Bindings(gObj<ComputeBinder> binder) {
		binder->SRV(0, Grid);
		binder->SRV(1, Average);
		binder->SRV(2, Radii);

		binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));

		binder->UAV(0, SphereGrid);
	}
};

