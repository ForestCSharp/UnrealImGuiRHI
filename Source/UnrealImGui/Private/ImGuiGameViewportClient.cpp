// Copyright Epic Games, Inc. All Rights Reserved.


#include "ImGuiGameViewportClient.h"
#include "UnrealImGui.h"

void UImGuiGameViewportClient::Init(FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	UnrealImGui::Initialize(this);
}
