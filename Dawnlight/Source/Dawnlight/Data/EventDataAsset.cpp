// Copyright Epic Games, Inc. All Rights Reserved.

#include "EventDataAsset.h"

UEventDataAsset::UEventDataAsset()
{
	// デフォルト値
	EventType = EEventType::Random;
	TensionLevel = 0.5f;
	SuccessTensionChange = -0.1f;
	FailureTensionChange = 0.2f;
	FixedEventOrder = 0;
	bIsTutorial = false;
	SelectionWeight = 1.0f;
	MaxOccurrences = 0;
}

FPrimaryAssetId UEventDataAsset::GetPrimaryAssetId() const
{
	// EventDataAsset:イベント名 の形式でIDを返す
	return FPrimaryAssetId(TEXT("EventDataAsset"), GetFName());
}
