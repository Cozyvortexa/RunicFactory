// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ItemManager.h"
#include "ABeltConveyor.generated.h"


struct FBeltItem {
	FBeltItem() {}
	FBeltItem(UStaticMesh* mesh, int64 itemID, float distance) : mesh(mesh), itemID(itemID), distance(distance) {}
	int64 itemID;
	float distance;
	UStaticMesh* mesh;
};

UCLASS()
class RUNICFACTORY_API AABeltConveyor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABeltConveyor();

	UFUNCTION(BlueprintCallable, Category = "Belt")
	int64 AddItemOnBelt(UStaticMesh* mesh);


	UFUNCTION(BlueprintCallable, Category = "Belt")
	void MoveItemsOnBelt(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "Belt")
	void RemoveItemFromBelt(int32 index);


	UFUNCTION(BlueprintCallable, Category = "Belt")
	void TransferItem(AABeltConveyor* receiver, int64 index);

	void ReceiveItem(FBeltItem&& item);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* BeltSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float speed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemManager* itemManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 itemToAchieveTheEnd;

private: 
	TArray<FBeltItem> itemsOnBelt;
};
