// Fill out your copyright notice in the Description page of Project Settings.


#include "ABeltConveyor.h"

// Sets default values
AABeltConveyor::AABeltConveyor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AABeltConveyor::BeginPlay()
{
	Super::BeginPlay();

	itemManager = GetWorld()->GetSubsystem<UItemManager>();
}

// Called every frame
void AABeltConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABeltConveyor::AddItemOnBelt(UStaticMesh* mesh)
{
	int64 itemID = itemManager->AddItem(mesh, GetActorTransform());
	FBeltItem currentItem(mesh, itemID, 0);
	itemsOnBelt.Add(currentItem);
}


void AABeltConveyor::MoveItemsOnBelt(float deltaTime)
{
	float splineLength = BeltSpline->GetSplineLength();
	float distanceToAdd = deltaTime * speed;
	TArray<int32> indexToRemove;


    for (int32 i = 0; i < itemsOnBelt.Num(); i++)
    {
        itemsOnBelt[i].distance += distanceToAdd;

        if (itemsOnBelt[i].distance >= splineLength)
        {
            indexToRemove.Add(i);
        }
        else
        {
            FTransform transform = BeltSpline->GetTransformAtDistanceAlongSpline(
                itemsOnBelt[i].distance, ESplineCoordinateSpace::World);
            itemManager->MoveItem(itemsOnBelt[i].mesh, itemsOnBelt[i].itemID, transform);
        }
    }



	for (int64 index : indexToRemove)
		itemManager->RemoveItem(itemsOnBelt[index].mesh, itemsOnBelt[index].itemID);
}

