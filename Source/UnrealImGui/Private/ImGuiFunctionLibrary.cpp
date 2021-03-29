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

void UImGuiFunctionLibrary::ImguiObject(UObject* InObject, const bool bOpenInNewWindow)
{
	if (ImGui::GetCurrentContext() && InObject != nullptr)
	{
		if (bOpenInNewWindow) { ImGui::Begin(TCHAR_TO_ANSI(*InObject->GetName())); }
		
		//FCS TODO: Sort by categories (use collapsing headers)
		{
			int32 PropertyIndex = 0;
			for (FProperty* Property : TFieldRange<FProperty>(InObject->GetClass()))
			{
				//Ignored Properties
				if (   Property->IsA<FDelegateProperty>()
					|| Property->IsA<FMulticastDelegateProperty>()
					|| Property->IsA<FMulticastInlineDelegateProperty>()
					|| Property->IsA<FMulticastSparseDelegateProperty>()
					|| Property->IsA<FInterfaceProperty>()
					//TODO: Ignore FLazyObjectProperty, FSoftObjectProperty, FClassProperty, FSoftClassProperty
					)
				{
					continue;
				}
				
				FString IDString = FString::FromInt(InObject->GetUniqueID()) + TEXT("_") + FString::FromInt(PropertyIndex++);
				ImGui::PushID(TCHAR_TO_ANSI(*IDString));

				const char* PropertyName = TCHAR_TO_ANSI(*Property->GetName());
				void* PropertyAddress = Property->ContainerPtrToValuePtr<void>(InObject);

				//TODO: FStructProperty, FSetProperty, FMapProperty, FArrayProperty, FWeakObjectProperty, FNameProperty, FStrProperty
				
				if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
				{
					UObject* ChildObject = ObjectProperty->GetObjectPropertyValue(PropertyAddress);
					if (ChildObject != nullptr)
					{
						const float Indentation = 16.0f;
						if (ImGui::CollapsingHeader(PropertyName))
						{
							ImGui::Indent(Indentation);
							ImguiObject(ChildObject, false);
							ImGui::Unindent(Indentation);
						}
					}
				}
				else if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable))
				{
					ImGui::TableNextRow();
					
					ImGui::TableSetColumnIndex(0);
					{
						
						ImGui::AlignTextToFramePadding();
						ImGui::Text(PropertyName);
					}

	                ImGui::TableSetColumnIndex(1);
					{
						if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
						{
							float* FloatPtr = FloatProperty->GetPropertyValuePtr(PropertyAddress);
							ImGui::DragFloat("##FloatValue", FloatPtr);
							//TODO: use SliderMin/Max if available, use ClampMin/Max if available
						}
						else if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
						{
							int32* IntPtr = IntProperty->GetPropertyValuePtr(PropertyAddress);
							ImGui::DragInt("##IntValue", IntPtr);
							//TODO: use SliderMin/Max if available, use ClampMin/Max if available
						}
						else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
						{
							bool BoolValue = BoolProperty->GetPropertyValue(PropertyAddress);
							if (ImGui::Checkbox("##BoolValue", &BoolValue))
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
						
								if (ImGui::BeginCombo("##EnumCombo", TCHAR_TO_ANSI(*CurrentValueName)))
								{
									//FCS TODO: Don't display the "MAX" entry
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
					}
					ImGui::EndTable();
				}

				ImGui::PopID();
			}
		}

		if (bOpenInNewWindow) { ImGui::End(); }
	}
}