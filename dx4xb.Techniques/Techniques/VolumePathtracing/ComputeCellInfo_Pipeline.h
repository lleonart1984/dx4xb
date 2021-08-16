/// <summary>
/// Proxy class to setup Compute Majorant compute shader.
/// </summary>
struct ComputeCellInfo : public ComputePipeline {
	void Setup() {
		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumePathtracing\\ComputeCellInfo_CS.cso"));
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

//struct ComputingCellInfo : public Technique {
//
//	gObj<ComputeCellInfo> pipeline;
//
//	gObj<Texture3D> Grid;
//
//	ComputingCellInfo(gObj<Texture3D> Grid) {
//		this->Grid = Grid;
//	}
//
//	~ComputingCellInfo() {}
//
//	virtual void OnLoad() override {
//		Load(pipeline);
//
//		pipeline->Grid = this->Grid;
//
//	}
//};
//
