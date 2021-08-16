/// <summary>
/// Proxy class to setup Compute Mip Maps compute shader.
/// </summary>
struct VolMipMap : public ComputePipeline {
	void Setup() {
		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumePathtracing\\VolMipMap_CS.cso"));
	}

	gObj<Texture3D> Src_Majorant;
	gObj<Texture3D> Src_Minorant;
	gObj<Texture3D> Src_Average;

	gObj<Texture3D> Dst_Majorant;
	gObj<Texture3D> Dst_Minorant;
	gObj<Texture3D> Dst_Average;

	virtual void Bindings(gObj<ComputeBinder> binder) {
		binder->OnDispatch();

		binder->SRV(0, Src_Majorant);
		binder->SRV(1, Src_Minorant);
		binder->SRV(2, Src_Average);

		binder->UAV(0, Dst_Majorant);
		binder->UAV(1, Dst_Minorant);
		binder->UAV(2, Dst_Average);

	}
};
