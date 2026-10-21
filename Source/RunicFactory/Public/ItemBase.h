// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/Texture2D.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType)
enum class E_Item_Type : uint8
{
    Default,
    Resource,
    Tool,
    Building
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FItemBase : public FTableRowBase
{
    GENERATED_BODY()

public:
    FItemBase()
        : MaxStackSize(1)
        , ItemType(E_Item_Type::Default)
        , Icone(nullptr)
        , mesh(nullptr)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    FName ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    int32 MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    TObjectPtr<UTexture2D> Icone;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
    E_Item_Type ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UStaticMesh* mesh;
    //TSoftObjectPtr<UStaticMesh>


    //Conditionnal value
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool Logic", meta = (EditCondition = "ItemType == E_Item_Type::Tool", EditConditionHides))
    TSoftClassPtr<AActor> EquipmentActorClass;

    // --- SPECIFIQUE BÂTIMENT ---
    // Visibles SEULEMENT si ItemType == Building
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Logic", meta = (EditCondition = "ItemType == E_Item_Type::Building", EditConditionHides))
    TSoftClassPtr<AActor> BuildingActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Logic", meta = (EditCondition = "ItemType == E_Item_Type::Building", EditConditionHides))
    TSoftObjectPtr<UStaticMesh> BuildingHologramMesh;
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

public:
    FInventorySlot()
        : number(0)
        , isEmpty(true)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FItemBase item;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 number;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool isEmpty;
};