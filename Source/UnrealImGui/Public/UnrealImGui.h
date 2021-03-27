// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "../../RenderCore/Public/ShaderParameters.h"
#include "../../RHI/Public/RHIResources.h"
#include "Runtime/RenderCore/Public/GlobalShader.h"

#define UNREAL_IMGUI_API DLLEXPORT
#define IMGUI_API DLLEXPORT
#include "ThirdParty/ImGui/misc/single_file/imgui_single_file.h"

class FUnrealImGuiModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

class FImGuiVS : public FGlobalShader
{
    DECLARE_SHADER_TYPE(FImGuiVS, Global);

    FImGuiVS() { }
    FImGuiVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
    : FGlobalShader(Initializer)
    {
    	ImGuiProjectionMatrix.Bind(Initializer.ParameterMap, TEXT("ImGuiProjectionMatrix"), SPF_Mandatory);
    }

    static bool ShouldCache(EShaderPlatform Platform)
    {
        return true;
    }


    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }

	void SetProjectionMatrix(FRHICommandList& RHICmdList, const FMatrix& InMatrix, ERHIFeatureLevel::Type FeatureLevel)
    {
    	const auto GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
    	const TShaderMapRef<FImGuiVS> VertexShader(GlobalShaderMap); //FCS FIXME: Shouldn't need to do this
    	SetShaderValue(RHICmdList, VertexShader.GetVertexShader(), ImGuiProjectionMatrix, InMatrix);
    }

private:

	LAYOUT_FIELD(FShaderParameter, ImGuiProjectionMatrix);
};

class FImGuiPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImGuiPS, Global);

	FImGuiPS() { }
	FImGuiPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
     : FGlobalShader(Initializer)
	{
		ImGuiFontTexture.Bind(Initializer.ParameterMap, TEXT("ImGuiFontTexture"), SPF_Mandatory);
		ImGuiFontSampler.Bind(Initializer.ParameterMap, TEXT("ImGuiFontSampler"), SPF_Mandatory);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		// Add your own defines for the shader code
		OutEnvironment.SetDefine(TEXT("MY_DEFINE"), 1);
	}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	void SetFontTexture(FRHICommandList& RHICmdList, const FTexture2DRHIRef& InTexture2D, const FSamplerStateRHIRef& InSamplerState, ERHIFeatureLevel::Type FeatureLevel)
	{
		const auto GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		const TShaderMapRef<FImGuiPS> PixelShader(GlobalShaderMap); //FCS FIXME: Shouldn't need to do this
		SetTextureParameter(RHICmdList, PixelShader.GetPixelShader(), ImGuiFontTexture, ImGuiFontSampler, InSamplerState, InTexture2D);
	}

private:

	// LAYOUT_FIELD(FShaderResourceParameter, FontTextureParameter);

	LAYOUT_FIELD(FShaderResourceParameter, ImGuiFontTexture);
	LAYOUT_FIELD(FShaderResourceParameter, ImGuiFontSampler);
};

namespace UnrealImGui
{
	struct FUnrealImGuiDrawData
	{
		TArray<ImDrawList> CmdLists; 			// CmdList Array (explicitly copied into a tarray so we can pass to the Render Thread)
		int             TotalIdxCount;          // For convenience, sum of all ImDrawList's IdxBuffer.Size
		int             TotalVtxCount;          // For convenience, sum of all ImDrawList's VtxBuffer.Size
		ImVec2          DisplayPos;             // Upper-left position of the viewport to render (== upper-left of the orthogonal projection matrix to use)
		ImVec2          DisplaySize;            // Size of the viewport to render (== io.DisplaySize for the main viewport) (DisplayPos + DisplaySize == lower-right of the orthogonal projection matrix to use)
		ImVec2          FramebufferScale;       // Amount of pixels for each unit of DisplaySize. Based on io.DisplayFramebufferScale. Generally (1,1) on normal display, (2,2) on OSX with Retina display.
	};

	UNREAL_IMGUI_API int32 ShowImGui = 1;
	static FAutoConsoleVariableRef CVarShowImGui = FAutoConsoleVariableRef(
        TEXT("imgui.show"),
        ShowImGui,
        TEXT("If enabled, shows ImGui Debug UI\n")
        TEXT("0: Disable, 1: Show"),
        ECVF_Cheat
    );
	
	void UNREAL_IMGUI_API Initialize(UGameViewportClient* InGameViewportClient);
	void UNREAL_IMGUI_API Shutdown(UGameViewportClient* InGameViewportClient);

	void Initialize_RenderThread(FRHICommandListImmediate& RHICmdList, const TArray<unsigned char>& FontTextureData, int32 Width, int32 Height);
	
	void Render(const FViewport* const Viewport);
	void RenderImGui_RenderThread(FRHICommandListImmediate& RHICmdList, ERHIFeatureLevel::Type FeatureLevel, const FUnrealImGuiDrawData& ImGuiDrawData, const
	                              FTexture2DRHIRef& RenderTargetTexture);
	
	void ShutdownImGui_RenderThread();
}