#pragma once
#include <d3d12.h>
#include <wrl.h>
#include<string>

#include"externals/DirectXTex/DirectXTex.h"
#include <Vector4.h>
#include <Vector2.h>
#include "Vector3.h"
#include <Matrix4x4.h>



class SpriteCommon;
class Sprite
	

{

public:

	//頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	//マテリアルデータ
	struct Material {
		Vector4 color; //色
		int32_t enableLighting; //ライティング有効フラグ
		float padding[3]; //パディング
		Matrix4x4 uvTransform; //UV変換行列
	};


	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	struct Transform
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};


	Matrix4x4 Multiply(const Matrix4x4& mat1, const Matrix4x4& mat2) {
		Matrix4x4 result = {};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = 0.0f;
				for (int k = 0; k < 4; ++k) {
					result.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
				}
			}
		}
		return result;
	}




public:


	//初期化
	void Initialize(SpriteCommon* spriteCommon,std::string textureFilePath);

	void Update();

	void Draw();

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);


	//getter

	const Vector2& GetPosition() const { return position; }
	const Vector2& GetSize() const { return size; }
	const Vector2& GetAnchorPoint() const { return anchorPoint; }
	const Vector4& GetColor() const { return materialDataPtr->color; }
	const BOOL& GetIsFlipX()const { return isFlipX_; }
	const BOOL& GetIsFlipY()const { return isFlipY_; }
	const Vector2& GetTextureLeftTop()const { return textureLeftTop; }
	const Vector2& GetTextureSize() { return textureSize; }

	float GetRotation() const { return rotation; }

	

	//setter
	void SetPosition(const Vector2& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetColor(const Vector4& color) { materialDataPtr->color = color; }
	void SetSize(const Vector2& size) { this->size = size; }
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }
	void SetIsFlipX(const BOOL& isFlipX_) { this->isFlipX_ = isFlipX_; }
	void SetIsFlipY(const BOOL& isFlipY_) { this->isFlipY_ = isFlipY_; }
	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }



private:


	SpriteCommon* spriteCommon = nullptr;


	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = nullptr;


	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	//バッファリソース内のデータを指すポインタ
	VertexData* vertexDataPtr = nullptr;
	uint32_t* indexDataPtr = nullptr;
	Material* materialDataPtr = nullptr;
	TransformationMatrix* transformationMatrixDataPtr = nullptr;
	VertexData* vertexDataSprite = nullptr;
	
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU{};

	
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform transformSprite{ {1.0f,1.0f,1.0f,},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };


	Transform uvTransform{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f} 
	};


	void CreateVertexData();

	void CreateMaterialData();

	void CreateTransformationMatrixData();



	bool useMonsterBall = false;

	Vector2 position = { 0.0f, 0.0f };
	Vector2 size = { 640.0f, 360.0f };
	Vector2 anchorPoint = { 0.0f, 0.0f };
	Vector2 textureLeftTop = { 0.0f, 0.0f };
	Vector2 textureSize = { 100.0f, 100.0f };

	float rotation = 0.0f;

	uint32_t textureIndex = 0;

	//左右フリップ
	bool isFlipX_ = false;

	//上下フリップ
	bool isFlipY_ = false;

	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();


};

