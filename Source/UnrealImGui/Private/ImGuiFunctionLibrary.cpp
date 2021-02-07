#include "ImGuiFunctionLibrary.h"
#include "UnrealImGui.h"

void UImGuiFunctionLibrary::ImguiInitialize(const AActor* const ActorContext)
{
	if (UWorld* World = ActorContext ? ActorContext->GetWorld() : nullptr)
	{
		UnrealImGui::Initialize(World->GetGameViewport());
	}
}

void UImGuiFunctionLibrary::ImguiShutdown(const AActor* const ActorContext)
{
	if (UWorld* World = ActorContext ? ActorContext->GetWorld() : nullptr)
	{
		UnrealImGui::Shutdown(World->GetGameViewport());
	}
}

void UImGuiFunctionLibrary::ImguiShowDemoWindow()
{
	if (ImGui::GetCurrentContext())
	{
		ImGui::ShowDemoWindow();
	}
}

bool UImGuiFunctionLibrary::ImguiBegin(const FString& Label)
{
	return ImGui::GetCurrentContext() ? ImGui::Begin(TCHAR_TO_ANSI(*Label), nullptr) : false;
}

void UImGuiFunctionLibrary::ImguiEnd()
{
	if (ImGui::GetCurrentContext())
	{
		ImGui::End();
	}
}

bool UImGuiFunctionLibrary::ImguiSliderFloat(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max)
{
	return ImGui::GetCurrentContext() ? ImGui::SliderFloat(TCHAR_TO_ANSI(*Label), &FloatRef, Min, Max) : false;
}

bool UImGuiFunctionLibrary::ImguiCheckbox(const FString& Label, UPARAM(ref) bool& BoolRef)
{
	return ImGui::GetCurrentContext() ? ImGui::Checkbox(TCHAR_TO_ANSI(*Label), &BoolRef) : false;
}

bool UImGuiFunctionLibrary::ImguiButton(const FString& Label)
{
	return ImGui::GetCurrentContext() ? ImGui::Button(TCHAR_TO_ANSI(*Label)) : false;
}