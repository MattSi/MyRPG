// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTableManager.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FWeaponDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data")
	FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data")
	TSubclassOf<UAnimInstance> AnimLayerClass;
};

USTRUCT(BlueprintType)
struct FAIActionListDataStruct: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Comments;
	
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
