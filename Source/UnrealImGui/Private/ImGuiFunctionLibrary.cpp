#include "ImGuiFunctionLibrary.h"
#include "UnrealImGui.h"

//FCS TODO: should TCHAR_TO_ANSI be TCHAR_TO_UTF8

//Helper macro to check ImGui context then call an ImGui function
#define IMGUI_CALL(imgui_function) \
if (ImGui::GetCurrentContext())    \
{								   \
	(imgui_function);			   \
}								   \

//Helper for ImGui functions that return a bool to signify if a given item was clicked this frame or is active
#define IMGUI_CALL_WITH_RESULT(imgui_function) ImGui::GetCurrentContext() ? (imgui_function) : false;

void UImGuiFunctionLibrary::ImguiInitialize(const AActor* const ActorContext)
{
	if (UWorld* World = ActorContext ? ActorContext->GetWorld() : nullptr)
	{
		UnrealImGui::Initialize(World->GetGameViewport());
	}
}

void UImGuiFunctionLibrary::ImguiShowDemoWindow()
{
	IMGUI_CALL(ImGui::ShowDemoWindow());
}

void UImGuiFunctionLibrary::ImguiShowMetricsWindow()
{
	if (ImGui::GetCurrentContext())
	{
		ImGui::ShowMetricsWindow();
	}
}

bool UImGuiFunctionLibrary::ImguiBegin(const FString& Label)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::Begin(TCHAR_TO_ANSI(*Label), nullptr));
}

void UImGuiFunctionLibrary::ImguiEnd()
{
	IMGUI_CALL(ImGui::End());
}

void UImGuiFunctionLibrary::ImguiSeparator()
{
	IMGUI_CALL(ImGui::Separator());
}

void UImGuiFunctionLibrary::ImguiIndent()
{
	IMGUI_CALL(ImGui::Indent());
}

void UImGuiFunctionLibrary::ImguiUnindent()
{
	IMGUI_CALL(ImGui::Unindent());
}

void UImGuiFunctionLibrary::ImguiText(const FString& Text)
{
	IMGUI_CALL(ImGui::Text(TCHAR_TO_ANSI(*Text)));
}

struct FInputStringCallbackUserData
{
	FString* String;
};

static int InputStringCallback(ImGuiInputTextCallbackData* CallbackData)
{
	FInputStringCallbackUserData* UserData = static_cast<FInputStringCallbackUserData*>(CallbackData->UserData);
	FString* String = UserData->String;
	check(String != nullptr);
	
	if (CallbackData->EventFlag == ImGuiInputTextFlags_CallbackEdit)
	{
		*String = FString(CallbackData->Buf);
	}
	return 0;
}

bool UImGuiFunctionLibrary::ImguiInputString(const FString& Label, FString& InputString)
{
	//We require the CallbackResize flag so that STB_TEXTEDIT_INSERTCHARS will report we've edited the string
	//We don't need to implement the Resize in our callback, as DearImGui stores their own persistent buffer internally
	const ImGuiInputTextFlags InputTextFlags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackEdit;

	FInputStringCallbackUserData UserData;
	UserData.String = &InputString;

	return IMGUI_CALL_WITH_RESULT(ImGui::InputText(TCHAR_TO_ANSI(*Label), TCHAR_TO_ANSI(*InputString),
		InputString.Len() + 1, InputTextFlags, InputStringCallback, &UserData)
	);
}

void UImGuiFunctionLibrary::ImguiInputStringBranched(const FString& Label, FString& InputString, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiInputString(Label, InputString) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

bool UImGuiFunctionLibrary::ImguiButton(const FString& Label)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::Button(TCHAR_TO_ANSI(*Label)));
}

bool UImGuiFunctionLibrary::ImguiCheckbox(const FString& Label, UPARAM(ref) bool& BoolRef)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::Checkbox(TCHAR_TO_ANSI(*Label), &BoolRef));
}

void UImGuiFunctionLibrary::ImguiCheckboxBranched(const FString& Label, bool& BoolRef, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiCheckbox(Label, BoolRef) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

bool UImGuiFunctionLibrary::ImguiSliderFloat(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::SliderFloat(TCHAR_TO_ANSI(*Label), &FloatRef, Min, Max));
}

void UImGuiFunctionLibrary::ImguiSliderFloatBranched(const FString& Label, UPARAM(ref) float& FloatRef, float Min, float Max, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiSliderFloat(Label, FloatRef, Min, Max) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

bool UImGuiFunctionLibrary::ImguiSliderVector(const FString& Label, FVector& VectorRef, float Min, float Max)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::SliderFloat3(TCHAR_TO_ANSI(*Label), reinterpret_cast<float*>(&VectorRef), Min, Max));
}

void UImGuiFunctionLibrary::ImguiSliderVectorBranched(const FString& Label, FVector& VectorRef, float Min, float Max, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiSliderVector(Label, VectorRef, Min, Max) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

bool UImGuiFunctionLibrary::ImguiSliderInt(const FString& Label, int32& IntRef, int32 Min, int32 Max)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::SliderInt(TCHAR_TO_ANSI(*Label), &IntRef, Min, Max));
}

void UImGuiFunctionLibrary::ImguiSliderIntBranched(const FString& Label, int32& IntRef, int32 Min, int32 Max, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiSliderInt(Label, IntRef, Min, Max) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

bool UImGuiFunctionLibrary::ImguiSliderIntVector(const FString& Label, FIntVector& VectorRef, int32 Min, int32 Max)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::SliderInt3(TCHAR_TO_ANSI(*Label), reinterpret_cast<int32*>(&VectorRef), Min, Max));
}

void UImGuiFunctionLibrary::ImguiSliderIntVectorBranched(const FString& Label, FIntVector& VectorRef, int32 Min, int32 Max, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiSliderIntVector(Label, VectorRef, Min, Max) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

bool UImGuiFunctionLibrary::ImguiInputFloat(const FString& Label, float& FloatRef)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::InputFloat(TCHAR_TO_ANSI(*Label), &FloatRef));
}

void UImGuiFunctionLibrary::ImguiInputFloatBranched(const FString& Label, float& FloatRef, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiInputFloat(Label, FloatRef) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

bool UImGuiFunctionLibrary::ImguiInputVector(const FString& Label, FVector& VectorRef)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::InputFloat(TCHAR_TO_ANSI(*Label), reinterpret_cast<float*>(&VectorRef)));
}

void UImGuiFunctionLibrary::ImguiInputVectorBranched(const FString& Label, FVector& VectorRef, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiInputVector(Label, VectorRef) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

bool UImGuiFunctionLibrary::ImguiLinearColorEdit(const FString& Label, FLinearColor& ColorRef)
{
	return IMGUI_CALL_WITH_RESULT(ImGui::ColorEdit4(TCHAR_TO_ANSI(*Label), &ColorRef.R));
}

void UImGuiFunctionLibrary::ImguiLinearColorEditBranched(const FString& Label, FLinearColor& ColorRef, EImGuiClickResult& OutBranches)
{
	OutBranches = ImguiLinearColorEdit(Label, ColorRef) ? EImGuiClickResult::Clicked : EImGuiClickResult::NotClicked;
}

void UImGuiFunctionLibrary::ImguiObject(UObject* InObject, const bool bOpenInNewWindow)
{
#if WITH_EDITORONLY_DATA
	if (ImGui::GetCurrentContext() && InObject != nullptr)
	{
		if (bOpenInNewWindow) { ImGui::Begin(TCHAR_TO_ANSI(*InObject->GetName())); }

		ImGui::PushID(InObject->GetUniqueID());

		const float Indentation = 16.0f;

		//Display SubObjects first
		if (ImGui::CollapsingHeader("SubObjects"))
		{
			ImGui::Indent(Indentation);
			for (FObjectProperty* ObjectProperty : TFieldRange<FObjectProperty>(InObject->GetClass()))
			{
				const char* PropertyName = TCHAR_TO_ANSI(*ObjectProperty->GetName());
				void* PropertyAddress = ObjectProperty->ContainerPtrToValuePtr<void>(InObject);
                
                ImGui::PushID(PropertyAddress);
			
				UObject* ChildObject = ObjectProperty->GetObjectPropertyValue(PropertyAddress);
				if (ChildObject != nullptr)
				{
					if (ImGui::CollapsingHeader(PropertyName))
					{
						ImGui::Indent(Indentation);
						ImguiObject(ChildObject, false);
						ImGui::Unindent(Indentation);
					}
				}

				ImGui::PopID();
			}
			ImGui::Unindent(Indentation);
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Properties"))
		{
			TMap<FString, TArray<FProperty*>> Categories;
			for (FProperty* Property : TFieldRange<FProperty>(InObject->GetClass()))
			{
				//Ignored Properties
				if (   Property->IsA<FDelegateProperty>()
                    || Property->IsA<FMulticastDelegateProperty>()
                    || Property->IsA<FMulticastInlineDelegateProperty>()
                    || Property->IsA<FMulticastSparseDelegateProperty>()
                    || Property->IsA<FInterfaceProperty>()
                    || Property->IsA<FObjectProperty>() //Handled Above
                    //TODO: Ignore FLazyObjectProperty, FSoftObjectProperty, FClassProperty, FSoftClassProperty
                    )
				{
					continue;
				}

				//Don't show "EditDefaultsOnly" properties
				if (Property->HasMetaData(TEXT("EditDefaultsOnly")))
				{
					continue;
				}

				//FCS TODO: Option to only show "EditAnywhere"
				const FString& Category = Property->GetMetaData(TEXT("Category"));
				Categories.FindOrAdd(Category).Add(Property);
			}

			for (TTuple<FString, TArray<FProperty*>> Category : Categories)
			{
				const FString CategoryName = Category.Key.IsEmpty() ? TEXT("Uncategorized") : Category.Key;
				
				if (Category.Value.Num() > 0 && ImGui::CollapsingHeader(TCHAR_TO_ANSI(*CategoryName)))
				{
					ImGui::Indent(Indentation);
					if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable))
					{
						for (FProperty* Property : Category.Value)
						{
							const char* PropertyName = TCHAR_TO_ANSI(*Property->GetName());
							void* PropertyAddress = Property->ContainerPtrToValuePtr<void>(InObject);

							ImGui::PushID(PropertyAddress);

							//TODO: FStructProperty, FSetProperty, FMapProperty, FArrayProperty, FWeakObjectProperty, FNameProperty, FStrProperty
					
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
							ImGui::PopID();
						}
						ImGui::EndTable();
					}
					ImGui::Unindent(Indentation);
				}
			}
		}

		ImGui::PopID();
		if (bOpenInNewWindow) { ImGui::End(); }
	}
#endif
}
