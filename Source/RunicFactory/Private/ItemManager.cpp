// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManager.h"

static int64 itemIDCompteur;

FInstancedMeshEntry::FInstancedMeshEntry(UInstancedStaticMeshComponent* new_ISM) {
	ISM = new_ISM;
}

int64 UItemManager::GetNewId() {
	return itemIDCompteur++;
}

int64 UItemManager::Create_MeshEntry_Correspondance(FInstancedMeshEntry* meshEntry, int32 key) {
	int64 newID = GetNewId();
	meshEntry->ItemIdToInstanceIndex.Add(newID, key);
	meshEntry->InstanceIndexToItemId.Add(key, newID);
	return newID;
}

void UItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.ObjectFlags |= RF_Transient; // pas besoin de sauvegarder cet Actor
		containerActor = World->SpawnActor<AActor>(SpawnParams);

		USceneComponent* Root = NewObject<USceneComponent>(containerActor);
		containerActor->SetRootComponent(Root);
		Root->RegisterComponent();
	}
}

void UItemManager::Deinitialize()
{
	if (containerActor)
	{
		containerActor->Destroy();
		containerActor = nullptr;
	}
	Super::Deinitialize();

	Super::Deinitialize(); // convention : Super en dernier pour Deinitialize
}


UItemManager::UItemManager() {
	itemIDCompteur = 0;
}


int64 UItemManager::AddItem(UStaticMesh* mesh, const FTransform& transform) {

	TObjectPtr<UStaticMesh> currentMesh = TObjectPtr<UStaticMesh>(mesh);
	FInstancedMeshEntry* instanceMeshEntry = ISM_List.Find(currentMesh);
	int32 ISM_key;

	// Static mesh found
	if (instanceMeshEntry != nullptr) {
		ISM_key = instanceMeshEntry->ISM.Get()->AddInstance(transform, true);
	}
	else {
		UInstancedStaticMeshComponent* new_ISM = NewObject<UInstancedStaticMeshComponent>(containerActor);
		new_ISM->SetStaticMesh(mesh);
		new_ISM->SetupAttachment(containerActor->GetRootComponent());
		new_ISM->RegisterComponent();

		ISM_key = new_ISM->AddInstance(transform, true);

		FInstancedMeshEntry newEntry(new_ISM);
		instanceMeshEntry = &ISM_List.Add(currentMesh, MoveTemp(newEntry));
	}

	return Create_MeshEntry_Correspondance(instanceMeshEntry, ISM_key);
}


bool UItemManager::RemoveItem(UStaticMesh* mesh, int64 itemID) {

	TObjectPtr<UStaticMesh> currentMesh = TObjectPtr<UStaticMesh>(mesh);
	FInstancedMeshEntry* instanceMeshEntry = ISM_List.Find(currentMesh);

	if (instanceMeshEntry == nullptr) {
		return false;
	}

	int32* indexPtr = instanceMeshEntry->ItemIdToInstanceIndex.Find(itemID);
	if (indexPtr == nullptr) {
		return false;
	}

	int32 indexToRemove = *indexPtr;
	int32 lastIndex = instanceMeshEntry->ISM->GetInstanceCount() - 1;

	int64 itemIdAtLastIndex = instanceMeshEntry->InstanceIndexToItemId[lastIndex];


	instanceMeshEntry->ISM->RemoveInstance(indexToRemove);

	//Clear
	instanceMeshEntry->InstanceIndexToItemId.Remove(indexToRemove);
	instanceMeshEntry->ItemIdToInstanceIndex.Remove(itemID);

	if (indexToRemove != lastIndex) {
		instanceMeshEntry->ItemIdToInstanceIndex[itemIdAtLastIndex] = indexToRemove;
		instanceMeshEntry->InstanceIndexToItemId.Add(indexToRemove, itemIdAtLastIndex);
	}

	return true;
}

bool UItemManager::MoveItem(UStaticMesh* mesh, int64 itemID, FTransform& transform) {

	TObjectPtr<UStaticMesh> currentMesh = TObjectPtr<UStaticMesh>(mesh);
	FInstancedMeshEntry* instanceMeshEntry = ISM_List.Find(currentMesh);

	if (instanceMeshEntry == nullptr ) {
		return false;
	}

	int32* instanceIndex = instanceMeshEntry->ItemIdToInstanceIndex.Find(itemID);
	if (instanceIndex == nullptr) {
		return false;
	}

	instanceMeshEntry->ISM->UpdateInstanceTransform(*instanceIndex, transform, true, false, true);


	return true;
}