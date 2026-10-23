// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

// Sets default values
UInventoryComponent::UInventoryComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

}

void UInventoryComponent::RebuildIndex()
{
	EmptySlots.Reset();
	itemRegister.Reset();
	EmptySlots.Reserve(inventorySlots.Num());

	// Parcours à l'envers : Pop() renverra alors les petits indices en premier
	for (int32 i = inventorySlots.Num() - 1; i >= 0; i--)
	{
		const FInventorySlot& slot = inventorySlots[i];

		if (slot.isEmpty)
		{
			EmptySlots.Add(i);
		}
		else
		{
			FItemIndex& reg = itemRegister.FindOrAdd(slot.item.ItemName);
			reg.Slots.Add(i);
			if (slot.number < slot.item.MaxStackSize)
				reg.PartialSlots.Add(i);
		}
	}
}

void UInventoryComponent::StackAll()
{
	for (auto& Pair : itemRegister)
	{
		TArray<int32> slots = Pair.Value.Slots;
		if (slots.Num() < 2)
			continue;
		slots.Sort();

		const int32 maxStack = inventorySlots[slots[0]].item.MaxStackSize;
		int32 total = 0;
		for (int32 idx : slots)
			total += inventorySlots[idx].number;

		for (int32 idx : slots)
		{
			if (total > 0)
			{
				const int32 n = FMath::Min(total, maxStack);
				inventorySlots[idx].number = n;
				total -= n;
			}
			else
			{
				inventorySlots[idx] = FInventorySlot(); // slot libéré
			}
		}
	}

	RebuildIndex(); // reconstruit registre + EmptySlots
}

bool UInventoryComponent::RemoveItemAt(int32 index, int32 amount, FInventorySlot& outRemoved)
{
	if (!inventorySlots.IsValidIndex(index) || inventorySlots[index].isEmpty || amount <= 0)
		return false;

	const FInventorySlot& slot = inventorySlots[index];
	const int32 taken = FMath::Min(amount, slot.number);

	outRemoved.item = slot.item;
	outRemoved.number = taken;
	outRemoved.isEmpty = false;

	SetSlotCount(index, slot.number - taken);
	return true;
}

int32 UInventoryComponent::TransferSlot(int32 from, UInventoryComponent* target, int32 amount)
{
	if (!target || target == this
		|| !inventorySlots.IsValidIndex(from) || inventorySlots[from].isEmpty)
		return 0;

	const FInventorySlot& src = inventorySlots[from];

	FInventorySlot temp = src;
	temp.number = (amount < 0) ? src.number : FMath::Min(amount, src.number);
	const int32 requested = temp.number;

	target->AddItem(temp); // temp.number = ce qui n'a pas pu entrer

	const int32 moved = requested - temp.number;
	if (moved > 0)
		SetSlotCount(from, src.number - moved);

	return moved;
}

int32 UInventoryComponent::FindOutputSlot(bool bFromEnd) const
{
	const int32 num = inventorySlots.Num();

	// Stockage entièrement vide : réponse immédiate
	if (EmptySlots.Num() == num)
		return INDEX_NONE;

	if (!bFromEnd)
	{
		for (int32 i = 0; i < num; i++)
			if (!inventorySlots[i].isEmpty) return i;
	}
	else
	{
		for (int32 i = num - 1; i >= 0; i--)
			if (!inventorySlots[i].isEmpty) return i;
	}
	return INDEX_NONE;
}

// Raccourci pour les convoyeurs : prend directement dans le bon slot
bool UInventoryComponent::TakeOutput(int32 amount, FInventorySlot& outRemoved, bool bFromEnd)
{
	const int32 idx = FindOutputSlot(bFromEnd);
	return idx != INDEX_NONE && RemoveItemAt(idx, amount, outRemoved);
}

// Vide un slot et met à jour le registre + EmptySlots
void UInventoryComponent::ClearSlotInternal(int32 index)
{
	const FName name = inventorySlots[index].item.ItemName;

	if (FItemIndex* reg = itemRegister.Find(name))
	{
		reg->Slots.RemoveSingleSwap(index);
		reg->PartialSlots.RemoveSingleSwap(index);
		if (reg->Slots.Num() == 0)
			itemRegister.Remove(name);
	}

	inventorySlots[index] = FInventorySlot(); // suppose isEmpty = true et number = 0 par défaut
	EmptySlots.Add(index);
}

// Change la quantité d'un slot NON vide et met à jour l'état partiel/plein
void UInventoryComponent::SetSlotCount(int32 index, int32 newCount)
{
	if (newCount <= 0)
	{
		ClearSlotInternal(index);
		return;
	}

	FInventorySlot& slot = inventorySlots[index];
	slot.number = newCount;

	if (FItemIndex* reg = itemRegister.Find(slot.item.ItemName))
	{
		if (newCount < slot.item.MaxStackSize)
			reg->PartialSlots.AddUnique(index);
		else
			reg->PartialSlots.RemoveSingleSwap(index);
	}
}

bool UInventoryComponent::MoveSlot(int32 from, int32 to)
{
	if (from == to
		|| !inventorySlots.IsValidIndex(from) || !inventorySlots.IsValidIndex(to)
		|| inventorySlots[from].isEmpty)
		return false;

	FInventorySlot& src = inventorySlots[from];
	FInventorySlot& dst = inventorySlots[to];

	// Case cible vide : on déplace tout
	if (dst.isEmpty)
	{
		const FInventorySlot moved = src; // copie
		ClearSlotInternal(from);          // ajoute `from` dans EmptySlots

		EmptySlots.Remove(to);            // retire `to` des slots vides
		inventorySlots[to] = moved;

		FItemIndex& reg = itemRegister.FindOrAdd(moved.item.ItemName);
		reg.Slots.Add(to);
		if (moved.number < moved.item.MaxStackSize)
			reg.PartialSlots.Add(to);
		return true;
	}

	// Case cible occupée par le même item : on empile
	if (dst.item.ItemName == src.item.ItemName)
	{
		const int32 space = dst.item.MaxStackSize - dst.number;
		if (space <= 0)
			return false;

		const int32 toMove = FMath::Min(space, src.number);
		const int32 srcLeft = src.number - toMove;
		SetSlotCount(to, dst.number + toMove);
		SetSlotCount(from, srcLeft); // vide le slot si srcLeft == 0
		return true;
	}

	return false;
}

// Place un nouveau stack dans un slot vide déjà réservé via SearchEmptySlot()
void UInventoryComponent::CreateStackInternal(int32 index, const FInventorySlot& proto, int32 count)
{
	FInventorySlot& slot = inventorySlots[index];
	slot.item = proto.item;
	slot.number = count;
	slot.isEmpty = false;

	FItemIndex& reg = itemRegister.FindOrAdd(slot.item.ItemName);
	reg.Slots.Add(index);
	if (count < slot.item.MaxStackSize)
		reg.PartialSlots.Add(index);
}

void UInventoryComponent::Init() {
	const int32 size = FMath::Max(slotNbr, 1);

	inventorySlots.SetNum(size);
	itemRegister.Reserve(size);

	RebuildIndex();
}

// Called when the game starts or when spawned
void UInventoryComponent::BeginPlay()
{	
	Init();
}


int32 UInventoryComponent::SearchEmptySlot()
{
	if (EmptySlots.Num() == 0)
		return INDEX_NONE;

	TSet<int32>::TIterator It(EmptySlots); // premier élément du set
	const int32 idx = *It;
	It.RemoveCurrent();                    // le consomme, O(1)
	return idx;
}

// Retourne true si TOUT a été inséré.
// En cas d'espace insuffisant, ce qui a pu entrer est inséré,
// et itemSlot.number contient le reste (à garder/rendre par l'appelant).
bool UInventoryComponent::AddItem(UPARAM(ref) FInventorySlot& itemSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Item slot number: %d"), itemSlot.number);
	UE_LOG(LogTemp, Warning, TEXT("itemSlot is Empty: %d"), itemSlot.isEmpty);
	UE_LOG(LogTemp, Warning, TEXT("itemSlot.item.MaxStackSize : %d"), itemSlot.item.MaxStackSize <= 0);

	if (itemSlot.number <= 0 || itemSlot.isEmpty || itemSlot.item.MaxStackSize <= 0)
		return false;

	const FName itemName = itemSlot.item.ItemName;
	const int32 maxStack = itemSlot.item.MaxStackSize;
	int32 remaining = itemSlot.number;

	// 1. Remplir les stacks partiels existants
	// Find (et non FindOrAdd) : on ne crée rien dans le registre ici
	while (remaining > 0)
	{
		const FItemIndex* reg = itemRegister.Find(itemName);
		if (!reg || reg->PartialSlots.Num() == 0)
			break;

		const int32 slotIndex = reg->PartialSlots.Last();
		const int32 current = inventorySlots[slotIndex].number;
		const int32 toAdd = FMath::Min(remaining, maxStack - current);

		// Met à jour la quantité, et retire le slot de PartialSlots s'il devient plein
		SetSlotCount(slotIndex, current + toAdd);
		remaining -= toAdd;
	}

	// 2. Créer de nouveaux stacks tant qu'il reste des items et des slots vides
	while (remaining > 0)
	{
		const int32 openSlot = SearchEmptySlot();
		if (openSlot == INDEX_NONE)
			break; // plus de place

		const int32 toAdd = FMath::Min(remaining, maxStack);
		CreateStackInternal(openSlot, itemSlot, toAdd);
		remaining -= toAdd;
	}

	// 3. Mettre à jour la source
	itemSlot.number = remaining;
	itemSlot.isEmpty = (remaining == 0);
	//UE_LOG(LogTemp, Warning, TEXT("Hello"));

	return remaining == 0;
}


FInventorySlot& UInventoryComponent::GetItemAt(int32 index, bool& success) {
	success = false;
	if (index > inventorySlots.Num() || inventorySlots[index].isEmpty) {
		return inventorySlots[0];
	}

	success = true;
	return inventorySlots[index];
}