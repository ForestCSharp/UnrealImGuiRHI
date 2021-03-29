#include "ImGuiFunctionLibrary.h"
#include "UnrealImGui.h"

void UImGuiFunctionLibrary::ImguiInitialize(const AActor* const ActorContext)
{
	if (UWorld* World = ActorContext ? ActorContext->GetWorld() : nullptr)
	{
		UnrealImGui::Initialize(World->GetGameViewport());
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

void UImGuiFunctionLibrary::ImguiObject(UObject* InObject)
{
	if (ImGui::GetCurrentContext() && InObject != nullptr)
	{
		//FCS TODO: If in a "Begin", just start adding UI. Otherwise, create window named InObject's name
		//
		//FCS TODO: Look at my old Ofx property editor (listed names first)
		//FCS TODO: Sort by categories
	
		for (FProperty* Property : TFieldRange<FProperty>(InObject->GetClass()))
		{
			const char* PropertyName = TCHAR_TO_ANSI(*Property->GetName());
			void* PropertyAddress = Property->ContainerPtrToValuePtr<void>(InObject);
			
			if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
			{
				float* FloatPtr = FloatProperty->GetPropertyValuePtr(PropertyAddress);
				ImGui::InputFloat(PropertyName, FloatPtr);
				//TODO: Sliders, using SliderMin, SliderMax
			}
			else if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
			{
				int32* IntPtr = IntProperty->GetPropertyValuePtr(PropertyAddress);
				ImGui::InputInt(PropertyName, IntPtr);
				//TODO: Sliders, using SliderMin, SliderMax
			}
			else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
			{
				bool BoolValue = BoolProperty->GetPropertyValue(PropertyAddress);
				if (ImGui::Checkbox(PropertyName, &BoolValue))
				{
					BoolProperty->SetPropertyValue(PropertyAddress, BoolValue);
				}
			}
			else if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
			{
				FNumericProperty* UnderlyingProperty = EnumProperty->GetUnderlyingProperty();
				UEnum* Enum = EnumProperty->GetEnum();
				if (UnderlyingProperty && Enum)
				{
					const int64 CurrentValue = UnderlyingProperty->GetSignedIntPropertyValue(PropertyAddress);
					const int32 CurrentIndex = Enum->GetIndexByValue(CurrentValue);
					FString CurrentValueName = Enum->GetDisplayNameTextByIndex(CurrentIndex).ToString();

					if (ImGui::BeginCombo(PropertyName, TCHAR_TO_ANSI(*CurrentValueName)))
					{
						for (int32 i = 0; i < Enum->NumEnums(); ++i)
						{
							const bool bIsSelected = i == CurrentIndex;
							if (ImGui::Selectable(TCHAR_TO_ANSI(*Enum->GetDisplayNameTextByIndex(i).ToString()), bIsSelected))
							{
								const int64 NewValue = Enum->GetValueByIndex(i);
								UnderlyingProperty->SetIntPropertyValue(PropertyAddress, NewValue);
							}  
                            if (bIsSelected)
                            {
                            	// Set the initial focus when opening the combo
                            	ImGui::SetItemDefaultFocus();
                            }
						}
						ImGui::EndCombo();
					}
				}
			}

			//TODO: FStructProperty
			//TODO: FEnumProperty
			//TODO: ObjectProperty (recursive call to ImguiObject)
		}
	}
}