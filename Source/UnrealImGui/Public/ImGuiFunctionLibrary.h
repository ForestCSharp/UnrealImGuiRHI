#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ImGuiFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EImGuiClickResult : uint8
{
	Clicked,
    NotClicked
};

UENUM(BlueprintType)
enum class EImGuiComponentCount : uint8
{
	One,
	Two,
	Three,
	Four,
};

//TODO: Tooltips
//TODO: Automatically handle IDs in some intelligent way?

UCLASS()
class UImGuiFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiInitialize(const AActor* const ActorContext);

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiShowDemoWindow();

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiShowMetricsWindow();

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiBegin(const FString& Label); //TODO: Window Flags

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiEnd();

	//TODO: Child Windows

	//TODO: Separator
	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiSeparator();

	//TODO: Indent,Unindent,BeginGroup,EndGroup

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImGuiText(const FString& Text);

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiButton(const FString& Label);

	//TODO: SmallButton, ArrowButton

	//TODO: Image, ImageButton (make work w/ UE4 Textures?)
	
	UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiCheckbox(const FString& Label, UPARAM(ref) bool& BoolRef);

    UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "Branches", DisplayName="Imgui Checkbox (Branched)"))
    static void ImguiCheckboxBranched(const FString& Label, UPARAM(ref) bool& BoolRef, EImGuiClickResult& Branches);

	//TODO: RadioButton

	//TODO: ProgressBar

	//TODO: BeginCombo, Combo, EndCombo

	//TODO: DragFloat(1, Vec3) + Branched

	//TODO: DragInt(1, Vec3) + Branched

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiSliderFloat(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "Branches", DisplayName="Imgui Slider Float (Branched)"))
	static void ImguiSliderFloatBranched(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max, EImGuiClickResult& Branches);

	//TODO: Imgui Slider Float (Vec3)

	//TODO: SliderInt(1, Vec3) + Branched
	
	//TODO: InputText

	//TODO: InputFloat, InputFloat3

	//TODO: InputInt, InputInt3

	//TODO: ColorEdit4 (ColorEdit)

	//TODO: (Later) Trees
	//TODO: (Later) Menu Bar
	//TODO: (Later) Tooltip
	//TODO: (Later) Popups functionality
	//TODO: (Later) Tables, Columns, TabBar

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static void ImguiObject(UObject* InObject, const bool bOpenInNewWindow = true);

	//Helper to Pass in UObject, render all properties?
};