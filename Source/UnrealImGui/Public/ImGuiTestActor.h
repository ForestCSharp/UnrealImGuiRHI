// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ImGuiTestActor.generated.h"

UCLASS()
class AImGuiTestActor : public AActor
{
	GENERATED_BODY()
public:
	AImGuiTestActor();
	virtual void Tick(float DeltaTime) override;
};
