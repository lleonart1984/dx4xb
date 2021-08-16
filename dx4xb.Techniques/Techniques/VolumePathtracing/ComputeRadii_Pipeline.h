/// <summary>
/// Proxy class to setup Compute Radii compute shader.
/// </summary>
struct ComputeRadii : public ComputePipeline {
	void Setup() {
		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumePathtracing\\ComputeRadii_CS.cso"));
	}

	gObj<Texture3D> Grid;
	gObj<Texture3D> Majorant;
	gObj<Texture3D> Minorant;
	gObj<Texture3D> Average;

	gObj<Texture3D> Radii;

	virtual void Bindings(gObj<ComputeBinder> binder) {
		binder->SRV(0, Grid);

		binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));

		binder->SRV(1, Majorant);
		binder->SMP_Static(1, Sampler::Maximum(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));

		binder->SRV(2, Minorant);
		binder->SMP_Static(2, Sampler::Minimum(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));

		binder->SRV(3, Average);

		binder->UAV(0, Radii);
	}
};