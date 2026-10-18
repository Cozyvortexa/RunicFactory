// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.h"
#include "Inventory.generated.h"


struct FItemIndex
{
    TArray<int32> Slots;      // tous les slots contenant l'item
    TArray<int32> PartialSlots;   // le stack non plein (ou -1)
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UInventoryComponent();

    UFUNCTION(BlueprintCallable)
    void Init();

    UFUNCTION(BlueprintCallable)
    const TArray<FInventorySlot>& GetInventorySlots() const { return inventorySlots; }

    UFUNCTION(BlueprintCallable)
    bool AddItem(UPARAM(ref) FInventorySlot& item);

    UFUNCTION(BlueprintCallable)
    bool MoveSlot(int32 from, int32 to);

    UFUNCTION(BlueprintCallable)
    void StackAll();
    UFUNCTION(BlueprintCallable)
    bool RemoveItemAt(int32 index, int32 amount, FInventorySlot& outRemoved);

    UFUNCTION(BlueprintCallable)
    int32 TransferSlot(int32 from, UInventoryComponent* target, int32 amount = -1);

    UFUNCTION(BlueprintCallable)
    int32 FindOutputSlot(bool bFromEnd = false) const;

    UFUNCTION(BlueprintCallable)
    bool TakeOutput(int32 amount, FInventorySlot& outRemoved, bool bFromEnd = false);


private:
    int SearchEmptySlot();
    void RebuildIndex();

    void ClearSlotInternal(int32 index);
    void SetSlotCount(int32 index, int32 newCount);

    void CreateStackInternal(int32 index, const FInventorySlot& proto, int32 count);

    TMap<FName, FItemIndex> itemRegister;
    TArray<FInventorySlot> inventorySlots;
    TSet<int32> EmptySlots;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 slotNbr = 1;
};
