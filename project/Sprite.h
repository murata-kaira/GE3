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
	void Initialize(SpriteCommon* spriteCommon);

	void Update();

	void Draw();

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);


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

	
	Transform transform{};
	Transform uvTransform{};

	bool useMonsterBall = false;
};

