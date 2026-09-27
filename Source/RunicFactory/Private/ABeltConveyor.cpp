// Fill out your copyright notice in the Description page of Project Settings.


#include "ABeltConveyor.h"

static constexpr float ItemSpacing = 118.6f;

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

int64 AABeltConveyor::AddItemOnBelt(UStaticMesh* mesh)
{
	int64 itemID = itemManager->AddItem(mesh, GetActorTransform());
	FBeltItem currentItem(mesh, itemID, 0);
	itemsOnBelt.Add(currentItem);
    return itemID;
}


void AABeltConveyor::MoveItemsOnBelt(float deltaTime)
{
	float splineLength = BeltSpline->GetSplineLength();
	float distanceToAdd = deltaTime * speed;

    TArray<int32> SortedIndices;
    SortedIndices.Reserve(itemsOnBelt.Num());

    for (int32 i = 0; i < itemsOnBelt.Num(); ++i)
    {
        SortedIndices.Add(i);
    }

    SortedIndices.Sort([this](int32 A, int32 B) { return itemsOnBelt[A].distance < itemsOnBelt[B].distance; });

    for (int32 i = 0; i < itemsOnBelt.Num(); i++)
    {
        int32 currentIndex = SortedIndices[i];

        if (i + 1 < itemsOnBelt.Num())
        {
            int32 forwardIndex = SortedIndices[i + 1];
            float forwardItemDistance = itemsOnBelt[forwardIndex].distance;
            float distanceEstimation = itemsOnBelt[currentIndex].distance + distanceToAdd;

            // S'il y a un item devant lui
            if (distanceEstimation + ItemSpacing > forwardItemDistance)
            {
                itemsOnBelt[currentIndex].distance = FMath::Max(itemsOnBelt[currentIndex].distance, forwardItemDistance - ItemSpacing);

                FTransform transform = BeltSpline->GetTransformAtDistanceAlongSpline(itemsOnBelt[currentIndex].distance, ESplineCoordinateSpace::World);
                itemManager->MoveItem(itemsOnBelt[currentIndex].mesh, itemsOnBelt[currentIndex].itemID, transform);
                continue;
            }
            itemsOnBelt[currentIndex].distance = distanceEstimation;
        }
        else if (itemsOnBelt[currentIndex].distance >= splineLength)
        {
            itemToAchieveTheEnd = itemsOnBelt[currentIndex].itemID;
            continue;
        }
        else
        {
            //Last item
            itemsOnBelt[currentIndex].distance += distanceToAdd;
        }

        FTransform transform = BeltSpline->GetTransformAtDistanceAlongSpline(itemsOnBelt[currentIndex].distance, ESplineCoordinateSpace::World);
        itemManager->MoveItem(itemsOnBelt[currentIndex].mesh, itemsOnBelt[currentIndex].itemID, transform);
    }
}

void AABeltConveyor::RemoveItemFromBelt(int32 index) {
    itemManager->RemoveItem(itemsOnBelt[index].mesh, itemsOnBelt[index].itemID);
}


void AABeltConveyor::ReceiveItem(FBeltItem&& item) {
    itemsOnBelt.Add(MoveTemp(item));
}

void AABeltConveyor::TransferItem(AABeltConveyor* receiver, int64 index) {
    receiver->ReceiveItem(MoveTemp(itemsOnBelt[index]));
    itemsOnBelt.RemoveAtSwap(index);
}
