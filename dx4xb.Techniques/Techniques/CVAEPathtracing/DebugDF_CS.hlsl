Texture3D<int> Head : register(t0);
//Texture3D<float> DF: register(t0);
RWTexture2D<int> Slice : register(u0);

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint Size, height, depth;
	Head.GetDimensions(Size, height, depth);
	
	int3 currentCell = int3(DTid.x % Size, DTid.x / Size % Size, 128);
	
	//if (Head[currentCell] == -1)
	//		Slice[currentCell.xy] = 100;
	Slice[currentCell.xy] = Head[currentCell];// 1;
}