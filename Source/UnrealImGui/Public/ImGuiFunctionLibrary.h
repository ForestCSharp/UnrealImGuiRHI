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

	/// Begins a new ImGui Window, all ImGui widgets called after this and before ImGuiEnd (or another ImGuiBegin) will live in this window
	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiBegin(const FString& Label); //TODO: Window Flags

	/// Ends the current ImGui Window
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
	static bool ImguiInputString(const FString& Label, UPARAM(ref) FString& InputString);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Input String (Branched)"))
    static void ImguiInputStringBranched(const FString& Label, UPARAM(ref) FString& InputString, EImGuiClickResult& OutBranches);

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiButton(const FString& Label);

	//TODO: SmallButton, ArrowButton

	//TODO: Image, ImageButton (make work w/ UE4 Textures?)
	
	UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiCheckbox(const FString& Label, UPARAM(ref) bool& BoolRef);

    UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Checkbox (Branched)"))
    static void ImguiCheckboxBranched(const FString& Label, UPARAM(ref) bool& BoolRef, EImGuiClickResult& OutBranches);

	//TODO: RadioButton

	//TODO: ProgressBar

	//TODO: BeginCombo, Combo, EndCombo

	//TODO: DragFloat(1, Vec3) + Branched

	//TODO: DragInt(1,3) + Branched

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiSliderFloat(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Slider Float (Branched)"))
	static void ImguiSliderFloatBranched(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max, EImGuiClickResult& OutBranches);

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiSliderVector(const FString& Label, UPARAM(ref) FVector& VectorRef, float Min, float Max);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Slider Vector (Branched)"))
    static void ImguiSliderVectorBranched(const FString& Label, UPARAM(ref) FVector& VectorRef, float Min, float Max, EImGuiClickResult& OutBranches);

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiSliderInt(const FString& Label, UPARAM(ref) int32& IntRef, int32 Min, int32 Max);

    UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Slider Int (Branched)"))
    static void ImguiSliderIntBranched(const FString& Label, UPARAM(ref) int32& IntRef, int32 Min, int32 Max, EImGuiClickResult& OutBranches);

    UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiSliderIntVector(const FString& Label, UPARAM(ref) FIntVector& VectorRef, int32 Min, int32 Max);

    UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Slider IntVector (Branched)"))
    static void ImguiSliderIntVectorBranched(const FString& Label, UPARAM(ref) FIntVector& VectorRef, int32 Min, int32 Max, EImGuiClickResult& OutBranches);

	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiInputFloat(const FString& Label, UPARAM(ref) float& FloatRef);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Input Float (Branched)"))
    static void ImguiInputFloatBranched(const FString& Label, UPARAM(ref) float& FloatRef, EImGuiClickResult& OutBranches);

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static bool ImguiInputVector(const FString& Label, UPARAM(ref) FVector& VectorRef);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Input Vector (Branched)"))
    static void ImguiInputVectorBranched(const FString& Label, UPARAM(ref) FVector& VectorRef, EImGuiClickResult& OutBranches);

	//TODO: InputInt (+ Branched) , InputInt3 (+ Branched)

	//TODO: FColor version of ImguiColorEdit (will need to convert to LinearColor?)
	
	UFUNCTION(BlueprintCallable, Category = ImGui)
	static bool ImguiLinearColorEdit(const FString& Label, UPARAM(ref) FLinearColor& ColorRef);

	UFUNCTION(BlueprintCallable, Category = ImGui, Meta = (ExpandEnumAsExecs = "OutBranches", DisplayName="Imgui Linear Color Edit (Branched)"))
    static void ImguiLinearColorEditBranched(const FString& Label, UPARAM(ref) FLinearColor& ColorRef, EImGuiClickResult& OutBranches);

	//TODO: (Later) Trees
	//TODO: (Later) Menu Bar
	//TODO: (Later) Tooltip
	//TODO: (Later) Popups functionality
	//TODO: (Later) Tables, Columns, TabBar

	UFUNCTION(BlueprintCallable, Category = ImGui)
    static void ImguiObject(UObject* InObject, const bool bOpenInNewWindow = true);

	//Helper to Pass in UObject, render all properties?
};