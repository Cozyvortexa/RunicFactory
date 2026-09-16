// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RUNICFACTORY_API UItemBase : public UObject
{
    GENERATED_BODY()

public:
    UItemBase();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 StackSize;

    UFUNCTION(BlueprintCallable, Category = "Item")
    void Use();
};
