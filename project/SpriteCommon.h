#pragma once
#include <DirectXCommon.h>
#include <wrl.h>  
#include <d3d12.h>  

class SpriteCommon
{


public:
	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//共通描画設定
	void SpriteDeCommon();


	DirectXCommon* GetDxCommon() const { return dxCommon; }

	
private:

	void RootSignature();

	void GraphicsPipeline();


private:

	DirectXCommon* dxCommon;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};

