

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "RaveBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Rave|Ability")
	static float GetSetByCallerMagnitude(const FGameplayEffectSpecHandle& EffectSpecHandle, FGameplayTag DataTag, bool WarnIfNotFound = false, float DefaultIfNotFound = 0.f);

	UFUNCTION(BlueprintPure, Category = "Rave|Team")
	static ETeamAttitude::Type GetTeamAttitude(const AActor* Source, const AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Rave|Equipment", Meta = (DefaultToSelf = "Actor"))
	static class URaveEquipmentComponent* GetEquipmentComponent(const AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "Rave|Equipment", Meta = (DefaultToSelf = "Actor"))
	static class UMeshComponent* GetEquipmentMesh(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Rave|Collision", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "Cone Overlap Actors"))
	static bool ConeOverlapActors(const UObject* WorldContextObject, const FVector ConePos, float ConeRadius, const FVector ConeDirection, float ConeHalfAngleDegree, const TArray<TEnumAsByte<EObjectTypeQuery> >& ObjectTypes, UClass* ActorClassFilter, const TArray<AActor*>& ActorsToIgnore, TArray<class AActor*>& OutActors);

	UFUNCTION(BlueprintCallable, Category = "Rave|Collision", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "Cone Overlap Components"))
	static bool ConeOverlapComponents(const UObject* WorldContextObject, const FVector ConePos, float ConeRadius, const FVector ConeDirection, float ConeHalfAngleDegree, const TArray<TEnumAsByte<EObjectTypeQuery> >& ObjectTypes, UClass* ComponentClassFilter, const TArray<AActor*>& ActorsToIgnore, TArray<class UPrimitiveComponent*>& OutComponents);
};
