// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTable/DataTableManager.h"

UDataTableManager::UDataTableManager()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("DataTable'/Game/DataTable/DT_MyDataTable.DT_MyDataTable'"));
	if(DataTableAsset.Succeeded())
	{
		MyDataTable = DataTableAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableAsset(TEXT("DataTable'/Game/DataTable/DT_Weapon.DT_Weapon'"));
	if(WeaponDataTableAsset.Succeeded())
	{
		WeaponDataTable = WeaponDataTableAsset.Object;
	}
}

UDataTableManager* UDataTableManager::Get()
{
	static UDataTableManager* Singleton = NewObject<UDataTableManager>();

	return Singleton;
}

