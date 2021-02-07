// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealImGui.h"
#include "Interfaces/IPluginManager.h"
// #include "Core/Public/Misc/CoreDelegates.h"

#define IMGUI_IMPLEMENTATION
#include "ThirdParty/ImGui/misc/single_file/imgui_single_file.h"

#define LOCTEXT_NAMESPACE "FUnrealImGuiModule"

void FUnrealImGuiModule::StartupModule()
{
	const FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("UnrealImGui"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/UnrealImGui"), PluginShaderDir);
}

void FUnrealImGuiModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealImGuiModule, UnrealImGui)

IMPLEMENT_SHADER_TYPE(, FImGuiVS, TEXT("/Plugin/UnrealImGui/Private/ImGui.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FImGuiPS, TEXT("/Plugin/UnrealImGui/Private/ImGui.usf"), TEXT("MainPS"), SF_Pixel);

//GameThread Globals
TWeakObjectPtr<UGameViewportClient> OwningGameViewportClient = nullptr;
ImGuiContext* ImGuiContextPtr = nullptr;
FDelegateHandle ViewportRenderedDelegateHandle;

//RenderThread Globals
FVertexBufferRHIRef ImguiVertexBuffer;
FIndexBufferRHIRef ImguiIndexBuffer;
FTexture2DRHIRef ImGuiFontTexture;
FSamplerStateRHIRef ImGuiFontSampler;

void UnrealImGui::Initialize(UGameViewportClient* InGameViewportClient)
{
	if (ImGuiContextPtr != nullptr)
	{
		//TODO: Warn about double init
	}

	ImGuiContextPtr = ImGui::CreateContext();
	OwningGameViewportClient = InGameViewportClient;
	
	ImGuiIO& IO = ImGui::GetIO();
	
	unsigned char* FontTexSrc = nullptr;
	int32 Width,Height,BytesPerPixel;
	IO.Fonts->GetTexDataAsRGBA32(&FontTexSrc, &Width, &Height, &BytesPerPixel);

	const TArray<unsigned char> FontTextureData(FontTexSrc, Width * Height * BytesPerPixel);

	ENQUEUE_RENDER_COMMAND(InitImGuiCmd)(
		[FontTextureData, Width, Height](FRHICommandListImmediate& RHICmdList)
		{
			Initialize_RenderThread(RHICmdList, FontTextureData, Width, Height);
		}
	);

	//Call New Frame Once Here to ensure we're properly initialized
	ImGui::NewFrame();
	
	ViewportRenderedDelegateHandle = InGameViewportClient->OnViewportRendered().AddLambda([](FViewport* InViewport)
	{
		if (UnrealImGui::ShowImGui)
		{
			UnrealImGui::Render(InViewport);
			ImGui::NewFrame();
		}
	});

	//TODO: OnInputKey?

	//TODO: OnCloseRequested?
}

void UnrealImGui::Shutdown(UGameViewportClient* InGameViewportClient)
{
	ImGui::DestroyContext();
	ImGuiContextPtr = nullptr;
	OwningGameViewportClient.Reset();

	if (ViewportRenderedDelegateHandle.IsValid())
	{
		InGameViewportClient->OnViewportRendered().Remove(ViewportRenderedDelegateHandle);
	}

	ENQUEUE_RENDER_COMMAND(ShutdownImGuiCmd)(
        [](FRHICommandListImmediate& RHICmdList)
        {
            ShutdownImGui_RenderThread();
        }
    );
}

void UnrealImGui::Render(const FViewport* const Viewport)
{
	if (ImGuiContextPtr == nullptr)
	{
		//FCS FIXME: UE_LOG Warning
		return;
	}
	
	ImGuiIO& IO = ImGui::GetIO();
	const auto& ViewportSize = Viewport->GetRenderTargetTextureSizeXY();
	IO.DisplaySize.x = ViewportSize.X;
	IO.DisplaySize.y = ViewportSize.Y;
	//TODO: DPI Scale

	if (OwningGameViewportClient.IsValid())
	{
		if (UWorld* World = OwningGameViewportClient->GetWorld())
		{
			IO.DeltaTime = World->GetDeltaSeconds();
		}
	}

	IO.MousePos.x = Viewport->GetMouseX();
	IO.MousePos.y = Viewport->GetMouseY();
	IO.MouseDown[0] = Viewport->KeyState(EKeys::LeftMouseButton);
	IO.MouseDown[1] = Viewport->KeyState(EKeys::RightMouseButton);
	IO.MouseDown[2] = Viewport->KeyState(EKeys::MiddleMouseButton);
	//FCS TODO: More Key States

	//FCS TODO: Mouse Wheel Data
	
	ImGui::Render();
	
	ImDrawData* ImGuiDrawData = ImGui::GetDrawData();
	if (!ImGuiDrawData || ImGuiDrawData->TotalVtxCount == 0)
	{
		return;
	}

	//Create a Copy of most of ImGuiDrawData (stored in FUnrealImGuiDrawData, which owns its CmdLists) to be passed to the render thread
	FUnrealImGuiDrawData UnrealImGuiDrawData;
	
	UnrealImGuiDrawData.CmdLists.Reserve(ImGuiDrawData->CmdListsCount);
	for (int32 i = 0; i < ImGuiDrawData->CmdListsCount; ++i)
	{
		UnrealImGuiDrawData.CmdLists.Add(*ImGuiDrawData->CmdLists[i]);
	}
	UnrealImGuiDrawData.TotalIdxCount = ImGuiDrawData->TotalIdxCount;
	UnrealImGuiDrawData.TotalVtxCount = ImGuiDrawData->TotalVtxCount;
	UnrealImGuiDrawData.DisplayPos = ImGuiDrawData->DisplayPos;
	UnrealImGuiDrawData.DisplaySize = ImGuiDrawData->DisplaySize;
	UnrealImGuiDrawData.FramebufferScale = ImGuiDrawData->FramebufferScale;

	const auto FeatureLevel = ERHIFeatureLevel::SM5; //FCS FIXME:
	
	ENQUEUE_RENDER_COMMAND(RenderImGuiCmd)(
	    [UnrealImGuiDrawData, FeatureLevel](FRHICommandListImmediate& RHICmdList)
		{
		    RenderImGui_RenderThread(RHICmdList, FeatureLevel, UnrealImGuiDrawData);
		}
	);
}

void UnrealImGui::Initialize_RenderThread(FRHICommandListImmediate& RHICmdList, const TArray<unsigned char>& FontTextureData, int32 Width, int32 Height)
{	
	const size_t UploadSize = FontTextureData.Num() * sizeof(char);
	FRHIResourceCreateInfo FontTextureCreateInfo;
	FontTextureCreateInfo.DebugName = TEXT("ImGuiFontTexture");
	ImGuiFontTexture = RHICmdList.CreateTexture2D(Width, Height, PF_R8G8B8A8, 1, 1, TexCreate_ShaderResource, FontTextureCreateInfo);

	uint32 _DestStride;
	unsigned char* TexDst = static_cast<unsigned char*>(RHICmdList.LockTexture2D(ImGuiFontTexture, 0, RLM_WriteOnly, _DestStride, false));
	FMemory::Memcpy(TexDst, FontTextureData.GetData(), UploadSize);
	RHICmdList.UnlockTexture2D(ImGuiFontTexture, 0, false);

	FSamplerStateInitializerRHI SamplerStateCreateInfo;
	SamplerStateCreateInfo.Filter = SF_Trilinear;
	SamplerStateCreateInfo.AddressU = AM_Wrap;
	SamplerStateCreateInfo.AddressV = AM_Wrap;
	SamplerStateCreateInfo.AddressW = AM_Wrap;
	SamplerStateCreateInfo.MipBias = 1.0f;
	SamplerStateCreateInfo.MinMipLevel = 0;
	SamplerStateCreateInfo.MaxMipLevel = 0;
	SamplerStateCreateInfo.MaxAnisotropy = 1.0f;
	SamplerStateCreateInfo.SamplerComparisonFunction = SCF_Never;
	ImGuiFontSampler = RHICmdList.CreateSamplerState(SamplerStateCreateInfo);
}

void UnrealImGui::RenderImGui_RenderThread(FRHICommandListImmediate& RHICmdList, ERHIFeatureLevel::Type FeatureLevel, const FUnrealImGuiDrawData& ImGuiDrawData)
{
	SCOPED_DRAW_EVENT(RHICmdList, ImGui)
	
	//Create/Resize Vertex Buffer
	const int32 VertexCount = ImGuiDrawData.TotalVtxCount;
	size_t VertexBufferSize = VertexCount * sizeof(ImDrawVert);
	FRHIResourceCreateInfo VertexBufferCreateInfo;
	VertexBufferCreateInfo.DebugName = TEXT("ImGuiVertexBuffer");
	ImguiVertexBuffer = RHICmdList.CreateVertexBuffer(VertexBufferSize, BUF_Dynamic, VertexBufferCreateInfo);
	
	//Create/Resize Index Buffer
	const int32 IndexCount = ImGuiDrawData.TotalIdxCount;
	size_t IndexBufferSize = IndexCount * sizeof(ImDrawIdx);
	FRHIResourceCreateInfo IndexBufferCreateInfo;
	IndexBufferCreateInfo.DebugName = TEXT("ImGuiIndexBuffer");
	ImguiIndexBuffer = RHICmdList.CreateIndexBuffer(sizeof(ImDrawIdx), IndexBufferSize, BUF_Dynamic, IndexBufferCreateInfo);

	{
		ImDrawVert* VtxDst = static_cast<ImDrawVert*>(RHICmdList.LockVertexBuffer(ImguiVertexBuffer, 0, VertexBufferSize, RLM_WriteOnly));
		ImDrawIdx*  IdxDst = static_cast<ImDrawIdx*>(RHICmdList.LockIndexBuffer(ImguiIndexBuffer, 0, IndexBufferSize, RLM_WriteOnly));

		for (const auto& CmdList : ImGuiDrawData.CmdLists)
		{
			FMemory::Memcpy(VtxDst, CmdList.VtxBuffer.Data, CmdList.VtxBuffer.Size * sizeof(ImDrawVert));
			FMemory::Memcpy(IdxDst, CmdList.IdxBuffer.Data, CmdList.IdxBuffer.Size * sizeof(ImDrawIdx));
			
			VtxDst += CmdList.VtxBuffer.Size;
			IdxDst += CmdList.IdxBuffer.Size;
		}
		
		RHICmdList.UnlockVertexBuffer(ImguiVertexBuffer);
		RHICmdList.UnlockIndexBuffer(ImguiIndexBuffer);
	}
	
	// Get the collection of Global Shaders
	auto ShaderMap = GetGlobalShaderMap(FeatureLevel);
	// Get the actual shader instances off the ShaderMap
	TShaderMapRef<FImGuiVS> MyVS(ShaderMap);
	TShaderMapRef<FImGuiPS> MyPS(ShaderMap);

	// Declare a pipeline state object that holds all the rendering state
	FGraphicsPipelineStateInitializer PSOInitializer;

	//FCS FIXME: Is there a Debug Render target that draws over everything? (RHICmdList.BeginRenderPass?) see below
	// FRHIRenderPassInfo RPInfo(DstPfmMesh.GetTargetableTexture(), ERenderTargetActions::Load_Store);
	// RHICmdList.BeginRenderPass(RPInfo, TEXT("DisplayClusterPFMExporterShader"));
	RHICmdList.ApplyCachedRenderTargets(PSOInitializer);

	PSOInitializer.PrimitiveType = PT_TriangleList;

	FVertexDeclarationElementList Elements;
	uint32 Stride = sizeof(ImDrawVert);
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(ImDrawVert, pos), VET_Float2, 0, Stride));
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(ImDrawVert, uv), VET_Float2, 1, Stride));
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(ImDrawVert, col), VET_UByte4N, 2, Stride));
	
	PSOInitializer.BoundShaderState.VertexDeclarationRHI = PipelineStateCache::GetOrCreateVertexDeclaration(Elements);
	PSOInitializer.BoundShaderState.VertexShaderRHI = MyVS.GetVertexShader();
	PSOInitializer.BoundShaderState.PixelShaderRHI = MyPS.GetPixelShader();
	PSOInitializer.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	PSOInitializer.BlendState = TStaticBlendState<
		/*EColorWriteMask RT0ColorWriteMask = */ CW_RGBA,
	    /*EBlendOperation RT0ColorBlendOp = */ BO_Add,
	    /*EBlendFactor    RT0ColorSrcBlend = */ BF_SourceAlpha,
	    /*EBlendFactor    RT0ColorDestBlend = */ BF_InverseSourceAlpha,
	    /*EBlendOperation RT0AlphaBlendOp = */ BO_Add,
	    /*EBlendFactor    RT0AlphaSrcBlend = */ BF_InverseSourceAlpha,
	    /*EBlendFactor    RT0AlphaDestBlend = */ BF_Zero
	>::GetRHI();
	PSOInitializer.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	SetGraphicsPipelineState(RHICmdList, PSOInitializer);

	// Setup Our Parameters. This has to happen after SetGraphicsPipelineState
	{
		//Setup projection matrix	
		const float L = ImGuiDrawData.DisplayPos.x;
		const float R = ImGuiDrawData.DisplayPos.x + ImGuiDrawData.DisplaySize.x;
		const float T = ImGuiDrawData.DisplayPos.y;
		const float B = ImGuiDrawData.DisplayPos.y + ImGuiDrawData.DisplaySize.y;

		const FMatrix OrthographicProjection(
            FPlane(2.0f/(R-L),   0.0f,           0.0f,       0.0f),
            FPlane(0.0f,         2.0f/(T-B),     0.0f,       0.0f),
            FPlane(0.0f,         0.0f,           0.5f,       0.0f),
            FPlane((R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f)
        );

		MyVS->SetProjectionMatrix(RHICmdList, OrthographicProjection.GetTransposed(), FeatureLevel);

		//Setup Font Texture
		MyPS->SetFontTexture(RHICmdList, ImGuiFontTexture, ImGuiFontSampler, FeatureLevel);
	}

	//Cmd Bind Vertex Buffer
	RHICmdList.SetStreamSource(0, ImguiVertexBuffer, 0);
	
	int GlobalVtxOffset = 0;
	int GlobalIdxOffset = 0;
	ImVec2 ClipOff = ImGuiDrawData.DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 ClipScale = ImGuiDrawData.FramebufferScale; // (1,1) unless using retina display which are often (2,2)
	
    for (const auto& CmdList : ImGuiDrawData.CmdLists)
    {
        for (const auto& Cmd : CmdList.CmdBuffer)
        {
            if (Cmd.UserCallback != nullptr)
            {
                //FCS TODO:
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 ClipRect;
                ClipRect.x = (Cmd.ClipRect.x - ClipOff.x) * ClipScale.x;
                ClipRect.y = (Cmd.ClipRect.y - ClipOff.y) * ClipScale.y;
                ClipRect.z = (Cmd.ClipRect.z - ClipOff.x) * ClipScale.x;
                ClipRect.w = (Cmd.ClipRect.w - ClipOff.y) * ClipScale.y;

                if (ClipRect.x < ImGuiDrawData.DisplaySize.x && ClipRect.y < ImGuiDrawData.DisplaySize.y && ClipRect.z >= 0.0f && ClipRect.w >= 0.0f)
                {
                    // Negative offsets are illegal for vkCmdSetScissor
                    if (ClipRect.x < 0.0f) { ClipRect.x = 0.0f; }
                    if (ClipRect.y < 0.0f) { ClipRect.y = 0.0f; }

                    // // Apply scissor/clipping rectangle
                	RHICmdList.SetScissorRect(true, Cmd.ClipRect.x - ClipOff.x, Cmd.ClipRect.y - ClipOff.y, Cmd.ClipRect.z - ClipOff.x, Cmd.ClipRect.w - ClipOff.y);

                	uint32 NumVertices = Cmd.ElemCount;
                	uint32 NumPrimitives = Cmd.ElemCount / 3;
                	RHICmdList.DrawIndexedPrimitive(ImguiIndexBuffer, Cmd.VtxOffset + GlobalVtxOffset, 0, NumVertices, Cmd.IdxOffset + GlobalIdxOffset, NumPrimitives, 1);
                }
            }
        }
        GlobalIdxOffset += CmdList.IdxBuffer.Size;
        GlobalVtxOffset += CmdList.VtxBuffer.Size;
    }
}

void UnrealImGui::ShutdownImGui_RenderThread()
{
	if (ImguiVertexBuffer.IsValid())
	{
		ImguiVertexBuffer = nullptr;
	}
	
	if (ImguiIndexBuffer.IsValid())
	{
		ImguiIndexBuffer = nullptr;
	}
	
	if (ImGuiFontTexture.IsValid())
	{
		ImGuiFontTexture = nullptr;
	}
	
	if (ImGuiFontSampler.IsValid())
	{
		ImGuiFontSampler = nullptr;
	}
}
