// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ItemManager.h"
#include "ABeltConveyor.generated.h"

UCLASS()
class RUNICFACTORY_API AABeltConveyor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABeltConveyor();

	UFUNCTION(BlueprintCallable, Category = "Belt")
	void AddItemOnBelt(UStaticMesh* mesh);


	UFUNCTION(BlueprintCallable, Category = "Belt")
	void MoveItemsOnBelt(float deltaTime);

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
private: 
	struct FBeltItem{
		FBeltItem() {}
		FBeltItem(UStaticMesh* mesh, int64 itemID, float distance) : mesh(mesh), itemID(itemID), distance(distance) {}
		int64 itemID;
		float distance;
		UStaticMesh* mesh;
	};
	TArray<FBeltItem> itemsOnBelt;
};
