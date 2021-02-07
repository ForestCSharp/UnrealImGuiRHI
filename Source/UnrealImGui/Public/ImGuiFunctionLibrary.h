#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ImGuiFunctionLibrary.generated.h"

UCLASS()
class UImGuiFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiInitialize(const AActor* const ActorContext);

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiShutdown(const AActor* const ActorContext);

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiShowDemoWindow();

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiBegin(const FString& Label); //TODO: Window Flags

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiEnd();
	
	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiSliderFloat(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max);

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiCheckbox(const FString& Label, UPARAM(ref) bool& BoolRef);

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiButton(const FString& Label);

	//Helper to Pass in UObject, render all properties?
};