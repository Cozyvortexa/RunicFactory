// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInteractionComponent.h"

// Sets default values for this component's properties
UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerInteractionComponent::UseItem(FName name) {
	const FItemBase* ItemData = itemDataTable->FindRow<FItemBase>(name, TEXT("UseItem"));
	if (!ItemData) return;


    switch (ItemData->ItemType)
    {
    case E_Item_Type::Resource:
        // No action
        break;

    //case E_Item_Type::Tool:
    //    // On passe les données au système d'équipement
    //    EquipmentComponent->EquipTool(ItemData->EquipmentActorClass);
    //    break;

    //case E_Item_Type::Building:
    //    // On passe les données au système de construction
    //    BuildComponent->StartPlacement(ItemData->BuildingActorClass, ItemData->BuildingHologramMesh);
    //    break;
    }

}

