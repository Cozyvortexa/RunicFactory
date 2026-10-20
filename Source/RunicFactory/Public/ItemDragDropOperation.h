// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemBase.h"
#include "Templates/SharedPointer.h"
#include "ItemDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class RUNICFACTORY_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetSlotPtr(const FInventorySlot& slot) { slotPtr = MakeShared<FInventorySlot>(slot); }

	UFUNCTION(BlueprintCallable, Category = "Item")
	FInventorySlot& GetSlot() { return *slotPtr; }


private:
	TSharedPtr<FInventorySlot> slotPtr;
};
