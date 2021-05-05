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

//TODO: Proper "Blueprint Demo" in example project
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

	//TODO: Indent,Unindent
	UFUNCTION(BlueprintCallable, Category = ImGui)
    static void ImguiIndent();

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static void ImguiUnindent();

	//TODO: BeginGroup (locks horizontal starting pos),EndGroup

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static void ImguiText(const FString& Text);

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

	//TODO: DragInt(1) + Branched

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiSliderFloat(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "Branches", DisplayName="Imgui Slider Float (Branched)"))
	static void ImguiSliderFloatBranched(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max, EImGuiClickResult& Branches);

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiSliderVector(const FString& Label, UPARAM(ref) FVector& VectorRef, float Min, float Max);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "Branches", DisplayName="Imgui Slider Vector (Branched)"))
    static void ImguiSliderVectorBranched(const FString& Label, UPARAM(ref) FVector& VectorRef, float Min, float Max, EImGuiClickResult& Branches);

	//TODO: SliderInt(1, Vec3) + Branched
	
	//TODO: InputText + Branched

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiInputFloat(const FString& Label, UPARAM(ref) float& FloatRef);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "Branches", DisplayName="Imgui Input Float (Branched)"))
    static void ImguiInputFloatBranched(const FString& Label, UPARAM(ref) float& FloatRef, EImGuiClickResult& Branches);

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiInputVector(const FString& Label, UPARAM(ref) FVector& VectorRef);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "Branches", DisplayName="Imgui Input Vector (Branched)"))
    static void ImguiInputVectorBranched(const FString& Label, UPARAM(ref) FVector& VectorRef, EImGuiClickResult& Branches);

	//TODO: InputInt (+ Branched) , InputInt3 (+ Branched)

	//TODO: FColor version of ImguiColorEdit (will need to convert to LinearColor?)
	
	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiLinearColorEdit(const FString& Label, UPARAM(ref) FLinearColor& ColorRef);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "Branches", DisplayName="Imgui Linear Color Edit (Branched)"))
    static void ImguiLinearColorEditBranched(const FString& Label, UPARAM(ref) FLinearColor& ColorRef, EImGuiClickResult& Branches);

	//TODO: (Later) Trees
	//TODO: (Later) Menu Bar
	//TODO: (Later) Tooltip
	//TODO: (Later) Popups functionality
	//TODO: (Later) Tables, Columns, TabBar

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static void ImguiObject(UObject* InObject, const bool bOpenInNewWindow = true);

	//Helper to Pass in UObject, render all properties?
};