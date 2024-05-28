// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTableManager.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data")
	FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data")
	TSubclassOf<UAnimInstance> AnimLayerClass;
};

/**
 * 
 */
UCLASS()
class MYRPG_API UDataTableManager : public UObject
{
	GENERATED_BODY()

public:
	static UDataTableManager* Get();
	UDataTable* GetWeaponDataTable() const { return WeaponDataTable; }

private:
	UDataTableManager();

	UPROPERTY()
	UDataTable* WeaponDataTable;
};
