#include "Sprite.h"
#include <SpriteCommon.h>
using namespace MatrixMath;



void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	this->spriteCommon = spriteCommon;



	DirectX::ScratchImage mipImages = spriteCommon->GetDxCommon()->LoadTexture("resources/uvChecker.png");

	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャー
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	textureResource = spriteCommon->GetDxCommon()->CreateTextureResource(metadata).Get();
	Microsoft::WRL::ComPtr<ID3D12Resource> intermadiaiteResource = spriteCommon->GetDxCommon()->UploadTextureData(textureResource, mipImages);

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = spriteCommon->GetDxCommon()->GetSRVCPUDescriptorHandle(1);

	textureSrvHandleGPU = spriteCommon->GetDxCommon()->GetSRVGPUDescriptorHandle(1);

	//SRVを生成
	spriteCommon->GetDxCommon()->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);


	//頂点リソースを作る
	vertexResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 4);

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースを作る
	indexResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 6);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;


	//頂点リソース
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataPtr));


	// 左下
	vertexDataPtr[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexDataPtr[0].texcoord = { 0.0f,1.0f };
	vertexDataPtr[0].normal = { 0.0f,0.0f,-1.0f };

	//左上
	vertexDataPtr[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataPtr[1].texcoord = { 0.0f,0.0f };
	vertexDataPtr[1].normal = { 0.0f,0.0f,-1.0f };

	//右下
	vertexDataPtr[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataPtr[2].texcoord = { 1.0f,1.0f };
	vertexDataPtr[2].normal = { 0.0f,0.0f,-1.0f };

	//右上
	vertexDataPtr[3].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexDataPtr[3].texcoord = { 1.0f,0.0f };
	vertexDataPtr[3].normal = { 0.0f,0.0f,-1.0f };

	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexDataPtr));
	indexDataPtr[0] = 0; indexDataPtr[1] = 1; indexDataPtr[2] = 2;
	indexDataPtr[3] = 1;	indexDataPtr[4] = 3; indexDataPtr[5] = 2;

	materialResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));

	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDataPtr));
	materialDataPtr->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataPtr->enableLighting = true;
	materialDataPtr->uvTransform = MatrixMath::MakeIdentity4x4();

	transformationMatrixResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	// 書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataPtr));

	transform.scale = Vector3(1.0f, 1.0f, 1.0f);
	Update();
}

void Sprite::Update()
{
	Matrix4x4 worldMatrix = MakeAffine(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrixSprite = MatrixMath::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = Orthographic(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multipty(worldMatrix, Multipty(viewMatrixSprite, projectionMatrixSprite));

	transformationMatrixDataPtr->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixDataPtr->World = worldMatrix;
}

void Sprite::Draw()
{



	// Spriteの描画
	spriteCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
	spriteCommon->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);//IBVを設定


	// マテリアルCBuffer
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);



	spriteCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);


}

DirectX::ScratchImage Sprite::LoadTexture(const std::string& filePath)
{
	return DirectX::ScratchImage();
}
