// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "ImGuiGameViewportClient.generated.h"

UCLASS(MinimalAPI)
class UImGuiGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
public:
	virtual void Init(FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice) override;
};
