// Copyright Epic Games, Inc. All Rights Reserved.

#include "ImGuiTestActor.h"
#include "UnrealImGui.h"

// Sets default values
AImGuiTestActor::AImGuiTestActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void AImGuiTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ImGui::GetCurrentContext()) //FCS TODO:
	{
		ImGui::ShowDemoWindow();
	}
}

