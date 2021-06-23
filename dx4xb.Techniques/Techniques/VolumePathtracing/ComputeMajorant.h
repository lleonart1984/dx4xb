/// <summary>
/// Proxy class to setup Compute Majorant compute shader.
/// </summary>
struct ComputeMajorant : public ComputePipeline {
	void Setup() {
		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumePathtracing\\ComputeMajorant_CS.cso"));
	}

	gObj<Texture3D> Grid;
	gObj<Texture3D> Majorant;
	gObj<Texture3D> Minorant;
	gObj<Texture3D> Average;

	virtual void Bindings(gObj<ComputeBinder> binder) {
		binder->SRV(0, Grid);
		binder->UAV(0, Majorant);
		binder->UAV(1, Minorant);
		binder->UAV(2, Average);
	}
};
