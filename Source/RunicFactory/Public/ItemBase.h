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
    Ressource,
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TObjectPtr<UTexture2D> Icone;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    E_Item_Type ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UStaticMesh* mesh;
};
