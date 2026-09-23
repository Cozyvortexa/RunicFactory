// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Components/StaticMeshComponent.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "ItemManager.generated.h"

USTRUCT()
struct FInstancedMeshEntry
{
	GENERATED_BODY()

public:
	FInstancedMeshEntry() {};
	FInstancedMeshEntry(UInstancedStaticMeshComponent* ISM);

	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> ISM;
	UPROPERTY()
	TMap<int64, int32> ItemIdToInstanceIndex;
	UPROPERTY()
	TMap<int32, int64> InstanceIndexToItemId;
};

UCLASS()
class RUNICFACTORY_API UItemManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UItemManager();

	UFUNCTION(BlueprintCallable, Category = "ItemManager")
	int64 AddItem(UStaticMesh* mesh, const FTransform& transform);

	UFUNCTION(BlueprintCallable, Category = "ItemManager")
	bool RemoveItem(UStaticMesh* mesh, int64 itemID);

	UFUNCTION(BlueprintCallable, Category = "ItemManager")
	bool MoveItem(UStaticMesh* mesh, int64 itemID, FTransform& transform);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	UPROPERTY()
	TMap<TObjectPtr<UStaticMesh>, FInstancedMeshEntry> ISM_List;

	UPROPERTY()
	TObjectPtr<AActor> containerActor;

	int64 GetNewId();
	int64 Create_MeshEntry_Correspondance(FInstancedMeshEntry* meshEntry, int32 key);
};