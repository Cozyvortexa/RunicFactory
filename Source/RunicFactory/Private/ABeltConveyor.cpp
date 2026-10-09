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

bool AABeltConveyor::AddItemOnBelt(const FItemBase& item)
{
    if (itemsOnBelt.Num() >= MaxItemNbrOnBelt)
        return false;


	int64 itemID = itemManager->AddItem(item.mesh, GetActorTransform());
	FBeltItem currentItem(item.mesh, itemID, 0);
	itemsOnBelt.Add(currentItem);


    ItemIndexToItem.Add(itemID, item);
    return true;
}


void AABeltConveyor::RemoveItemFromBelt(int32 itemIndex) {
    int64 itemID = itemsOnBelt[itemIndex].itemID;
    ItemIndexToItem.Remove(itemID);
    itemManager->RemoveItem(itemsOnBelt[itemIndex].mesh, itemsOnBelt[itemIndex].itemID);
    itemsOnBelt.RemoveAtSwap(itemIndex);
}


bool AABeltConveyor::GetItemFromBelt(int32 itemIndex, FItemBase& outItem) {
    if (itemIndex < 0 || itemIndex > itemsOnBelt.Num() - 1)
        return false;

    FItemBase* Found = ItemIndexToItem.Find(itemsOnBelt[itemIndex].itemID);
    if (!Found)
        return false;


    outItem = *Found;
    return true;
}


//bool AABeltConveyor::GetAndRemoveItemFromBelt(int32 itemIndex, FItemBase& outItem) {
//    if (!GetItemFromBelt(itemIndex, outItem))
//        return false;
//
//    RemoveItemFromBelt(itemIndex);
//
//    return true;
//}


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
            itemIndexToAchieveTheEnd = currentIndex;
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


//Transfert to a another Belt
bool AABeltConveyor::ReceiveItem(FBeltItem&& beltItem, FItemBase&& item) {
    if (itemsOnBelt.Num() >= MaxItemNbrOnBelt) //To many items, declined the item
        return false;


    itemsOnBelt.Add(MoveTemp(beltItem));
    ItemIndexToItem.Add(beltItem.itemID, MoveTemp(item));
    return true;
}

bool AABeltConveyor::TransferItem(AABeltConveyor* receiver, int32 itemIndex) {
    if (itemIndex < 0 || itemsOnBelt.Num() < itemIndex)
        return false;

    int64 itemID = itemsOnBelt[itemIndex].itemID;
    bool result = receiver->ReceiveItem(MoveTemp(itemsOnBelt[itemIndex]), MoveTemp(ItemIndexToItem[itemID]));
    if (!result)
        return false;

    itemsOnBelt.RemoveAtSwap(itemIndex);
    ItemIndexToItem.Remove(itemID);

    return true;
}
