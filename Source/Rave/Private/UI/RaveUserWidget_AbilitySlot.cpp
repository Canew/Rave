


#include "UI/RaveUserWidget_AbilitySlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "GameplayAbility/Abilities/RaveGameplayAbility_Chain.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RaveGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveUserWidget_AbilitySlot)

void URaveUserWidget_AbilitySlot::NativeConstruct()
{
	Super::NativeConstruct();

	// Create material instance dynamic
	if (UMaterialInterface* ImageMaterial = Cast<UMaterialInterface>(AbilitySlotImage->GetBrush().GetResourceObject()))
	{
		if (ImageMID = UMaterialInstanceDynamic::Create(ImageMaterial, this))
		{
			AbilitySlotImage->SetBrushFromMaterial(ImageMID);

			ImageMID->SetScalarParameterValue(TEXT("BorderShade"), 0.0f);
		}
	}

	if (URaveAbilitySystemComponent* RaveASC = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn())))
	{
		UpdateAbilitySlot();

		RaveASC->OnAbilityAdded.AddUObject(this, &ThisClass::HandleAbilityAdded);
		RaveASC->OnAbilityRemoved.AddUObject(this, &ThisClass::HandleAbilityRemoved);
	}

	AbilityCooldownTextBlock->SetVisibility(ESlateVisibility::Hidden);
}

void URaveUserWidget_AbilitySlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateAbilitySlot();
	UpdateCooldown();
}

void URaveUserWidget_AbilitySlot::UpdateAbilitySlot()
{
	if (!ImageMID)
	{
		return;
	}

	if (URaveAbilitySystemComponent* RaveASC = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn())))
	{
		SlotAbilitySpec = FGameplayAbilitySpec();
		for (const FGameplayAbilitySpec& ActivatableAbilitySpec : RaveASC->GetActivatableAbilities())
		{
			if (ActivatableAbilitySpec.DynamicAbilityTags.HasTag(AbilitySlotInputTag))
			{
				SlotAbilitySpec = ActivatableAbilitySpec;
				break;
			}
		}

		if (URaveGameplayAbility* RaveAbilityCDO = Cast<URaveGameplayAbility>(SlotAbilitySpec.Ability))
		{
			FRaveAbilityDescription AbilityDescription = RaveAbilityCDO->GetAbilityDescription();
			if (UTexture2D* AbilityIconTexture = AbilityDescription.Icon)
			{
				ImageMID->SetTextureParameterValue(TEXT("AbilityTexture"), AbilityIconTexture);
				ImageMID->SetScalarParameterValue(TEXT("Percent"), 0.0f);
				ImageMID->SetScalarParameterValue(TEXT("BorderOpacity"), 1.0f);
			}
		}
		else
		{
			ImageMID->SetTextureParameterValue(TEXT("AbilityTexture"), DefaultBackgroundTexture);
			ImageMID->SetScalarParameterValue(TEXT("Percent"), 0.0f);
			ImageMID->SetScalarParameterValue(TEXT("BorderOpacity"), 1.0f);
		}
	}
}

void URaveUserWidget_AbilitySlot::UpdateCooldown()
{
	URaveAbilitySystemComponent* RaveASC = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn()));
	if (!RaveASC)
	{
		return;
	}

	for (const FGameplayAbilitySpec& ActivatableAbilitySpec : RaveASC->GetActivatableAbilities())
	{
		if (ActivatableAbilitySpec.DynamicAbilityTags.HasTag(AbilitySlotInputTag))
		{
			SlotAbilitySpec = ActivatableAbilitySpec;
			break;
		}
	}

	if (URaveGameplayAbility* RaveAbilityInstance = Cast<URaveGameplayAbility>(SlotAbilitySpec.GetPrimaryInstance()))
	{
		// Get ability cooldown
		float TimeRemaining;
		float CooldownDuration;
		RaveAbilityInstance->GetCooldownTimeRemainingAndDuration(SlotAbilitySpec.Handle, RaveAbilityInstance->GetCurrentActorInfo(), TimeRemaining, CooldownDuration);

		float RemainingChainDurationRatio = 1.0f;
		if (URaveGameplayAbility_Chain* RaveChianAbilityInstance = Cast< URaveGameplayAbility_Chain>(RaveAbilityInstance))
		{
			float RemainingChainDuration = RaveChianAbilityInstance->GetRemainingChainDuration();
			float MaxChainInterval = RaveChianAbilityInstance->GetMaxChainInterval();
			
			RemainingChainDurationRatio = (MaxChainInterval == 0.0f) ? 1.0f : (RemainingChainDuration / MaxChainInterval);

			if (RemainingChainDuration > 0.0f)
			{
				ImageMID->SetScalarParameterValue(TEXT("BorderShade"), ChainBorderShade);
			}
			else
			{
				ImageMID->SetScalarParameterValue(TEXT("BorderShade"), 0.0f);
			}
		}

		if (ImageMID)
		{
			if (CooldownDuration == 0.0f)
			{
				ImageMID->SetScalarParameterValue(TEXT("Percent"), 0.0f);
				ImageMID->SetScalarParameterValue(TEXT("Shade"), 1.0f);
			}
			else
			{
				ImageMID->SetScalarParameterValue(TEXT("Percent"), 1.0f - (TimeRemaining / CooldownDuration));
				ImageMID->SetScalarParameterValue(TEXT("Shade"), 0.1f);

			}

			ImageMID->SetScalarParameterValue(TEXT("BorderPercent"), 1.0f - RemainingChainDurationRatio);
		}

		if (TimeRemaining == 0.0f)
		{

			AbilityCooldownTextBlock->SetText(FText());
			AbilityCooldownTextBlock->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			FString CooldownString = FString::SanitizeFloat(TimeRemaining);
			if (TimeRemaining >= 1.0f)
			{
				CooldownString = CooldownString.Left(CooldownString.Find(TEXT(".")));
			}
			else
			{
				CooldownString = CooldownString.Left(CooldownString.Find(TEXT(".")) + 2);
			}

			AbilityCooldownTextBlock->SetText(FText::FromString(CooldownString));
			AbilityCooldownTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		AbilityCooldownTextBlock->SetText(FText());
		AbilityCooldownTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URaveUserWidget_AbilitySlot::HandleAbilityAdded(FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag DynamicAbilityTag : AbilitySpec.DynamicAbilityTags)
	{
		if (DynamicAbilityTag == AbilitySlotInputTag)
		{
			UpdateAbilitySlot();
		}
	}
}

void URaveUserWidget_AbilitySlot::HandleAbilityRemoved(FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag DynamicAbilityTag : AbilitySpec.DynamicAbilityTags)
	{
		if (DynamicAbilityTag == AbilitySlotInputTag)
		{
			UpdateAbilitySlot();
		}
	}
}
