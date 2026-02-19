#include "Sprite.h"
#include <SpriteCommon.h>
#include <TextureManager.h>
using namespace MatrixMath;



void Sprite::Initialize(SpriteCommon* spriteCommon, std::string textureFilePath)
	{
	this->spriteCommon = spriteCommon;


	CreateVertexData();

	CreateMaterialData();

	CreateTransformationMatrixData();

	TextureManager::GetInstance()->LoadTexture(textureFilePath);


	//単位行列を書き込んでおく
	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);

	AdjustTextureSize();
}


void Sprite::Update()
{

	transform.translate = { position.x, position.y, 0.0f }; //座標
	transform.rotate = { 0.0f, 0.0f, rotation }; //回転
	transform.scale = { size.x, size.y, 1.0f }; //サイズ

	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;


	//左右反転
	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}

	//上下反転
	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}


	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureIndex);
	float tex_Left = textureLeftTop.x / metadata.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
	float tex_top = textureLeftTop.y / metadata.height;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;




	
	// 左下
	vertexDataPtr[0].position = { left,bottom,0.0f,1.0f };
	vertexDataPtr[0].texcoord = { tex_Left,tex_bottom };
	vertexDataPtr[0].normal = { 0.0f,0.0f,-1.0f };

	//左上
	vertexDataPtr[1].position = { left,top,0.0f,1.0f };
	vertexDataPtr[1].texcoord = { tex_Left,tex_top };
	vertexDataPtr[1].normal = { 0.0f,0.0f,-1.0f };

	//右下
	vertexDataPtr[2].position = { right,bottom,0.0f,1.0f };
	vertexDataPtr[2].texcoord = { tex_right,tex_bottom };
	vertexDataPtr[2].normal = { 0.0f,0.0f,-1.0f };

	//右上
	vertexDataPtr[3].position = { right,top,0.0f,1.0f };
	vertexDataPtr[3].texcoord = { tex_right,tex_top };
	vertexDataPtr[3].normal = { 0.0f,0.0f,-1.0f };


	indexDataPtr[0] = 0; indexDataPtr[1] = 1; indexDataPtr[2] = 2;
	indexDataPtr[3] = 1;	indexDataPtr[4] = 3; indexDataPtr[5] = 2;

	
	Matrix4x4 worldMatrix = MakeAffine(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrixSprite = MatrixMath::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = Orthographic(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multipty(worldMatrix, Multipty(viewMatrixSprite, projectionMatrixSprite));

	transform.translate = { position.x,position.y,0.0f };
	transform.rotate = { 0.0f,0.0f,rotation };
	transformationMatrixDataPtr->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixDataPtr->World = worldMatrix;
	transform.rotate = { 0.0f,0.0f,rotation };
	transform.scale = { size.x,size.y,1.0f };

	
}

void Sprite::Draw()
{

	// Spriteの描画
	spriteCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
	spriteCommon->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);//IBVを設定

	// マテリアルCBuffer
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex));

	spriteCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);


}

DirectX::ScratchImage Sprite::LoadTexture(const std::string& filePath)
{
	return DirectX::ScratchImage();
}

void Sprite::CreateVertexData()
{	
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
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexDataPtr));
}

void Sprite::CreateMaterialData()
{
	materialResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDataPtr));
	materialDataPtr->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataPtr->enableLighting = true;
	materialDataPtr->uvTransform = MatrixMath::MakeIdentity4x4();
}

void Sprite::CreateTransformationMatrixData()
{
	transformationMatrixResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	// 書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataPtr));

	transformationMatrixDataPtr->WVP = MakeIdentity4x4();
	transformationMatrixDataPtr->World = MakeIdentity4x4();
}

void Sprite::AdjustTextureSize()
{

	//テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureIndex);
	textureSize.x = static_cast<float>(metadata.width);
	textureSize.y = static_cast<float>(metadata.height);
	//画像サイズをテクスチャサイズに合わせる
	size = textureSize;

}

