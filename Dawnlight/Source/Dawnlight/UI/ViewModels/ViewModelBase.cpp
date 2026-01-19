// Copyright Epic Games, Inc. All Rights Reserved.

#include "ViewModelBase.h"
#include "Dawnlight.h"

UViewModelBase::UViewModelBase()
	: bIsInitialized(false)
{
}

UViewModelBase::~UViewModelBase()
{
	// デストラクタでDeinitializeが呼ばれていない場合のフォールバック
	if (bIsInitialized)
	{
		Deinitialize();
	}
}

void UViewModelBase::Initialize(UWorld* InWorld)
{
	if (bIsInitialized)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[%s] 既に初期化されています"), *GetName());
		return;
	}

	WorldContext = InWorld;
	bIsInitialized = true;

	UE_LOG(LogDawnlight, Log, TEXT("[%s] ViewModel初期化完了"), *GetName());
}

void UViewModelBase::Deinitialize()
{
	if (!bIsInitialized)
	{
		return;
	}

	// デリゲートをクリア
	OnViewModelPropertyChanged.Clear();
	OnAllPropertiesChanged.Clear();

	WorldContext.Reset();
	bIsInitialized = false;

	UE_LOG(LogDawnlight, Log, TEXT("[%s] ViewModel破棄完了"), *GetName());
}

void UViewModelBase::NotifyPropertyChanged(FName PropertyName)
{
	if (!bIsInitialized)
	{
		return;
	}

	OnViewModelPropertyChanged.Broadcast(PropertyName);

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] プロパティ変更通知: %s"), *GetName(), *PropertyName.ToString());
}

void UViewModelBase::NotifyAllPropertiesChanged()
{
	if (!bIsInitialized)
	{
		return;
	}

	OnAllPropertiesChanged.Broadcast();

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] 全プロパティ変更通知"), *GetName());
}
