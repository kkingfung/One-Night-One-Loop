// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UDawnlightAttributeSet::UDawnlightAttributeSet()
{
	// デフォルト値を設定
	InitDetectionLevel(0.0f);
	InitSurveillanceDanger(0.0f);
	InitTensionLevel(0.0f);
	InitNightProgress(0.0f);
	InitPhotoCount(0.0f);
	InitMaxPhotoCount(10.0f);  // デフォルトで10枚まで撮影可能
}

void UDawnlightAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 属性値をクランプ
	ClampAttribute(Attribute, NewValue);
}

void UDawnlightAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// エフェクト適用後の処理
	// 必要に応じてイベントを発火

	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;

	// 検知レベルが閾値を超えた場合の処理
	if (Attribute == GetDetectionLevelAttribute())
	{
		// 検知レベルに基づいて監視危険度を更新
		// 80%以上で危険状態
		if (GetDetectionLevel() >= 80.0f)
		{
			SetSurveillanceDanger(100.0f);
		}
		else
		{
			SetSurveillanceDanger(GetDetectionLevel() * 1.25f);
		}
	}

	// 撮影カウントが最大値を超えないように
	if (Attribute == GetPhotoCountAttribute())
	{
		const float CurrentCount = GetPhotoCount();
		const float MaxCount = GetMaxPhotoCount();
		if (CurrentCount > MaxCount)
		{
			SetPhotoCount(MaxCount);
		}
	}
}

void UDawnlightAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 全ての属性をレプリケート
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, DetectionLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, SurveillanceDanger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, TensionLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, NightProgress, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, PhotoCount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, MaxPhotoCount, COND_None, REPNOTIFY_Always);
}

void UDawnlightAttributeSet::OnRep_DetectionLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, DetectionLevel, OldValue);
}

void UDawnlightAttributeSet::OnRep_SurveillanceDanger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, SurveillanceDanger, OldValue);
}

void UDawnlightAttributeSet::OnRep_TensionLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, TensionLevel, OldValue);
}

void UDawnlightAttributeSet::OnRep_NightProgress(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, NightProgress, OldValue);
}

void UDawnlightAttributeSet::OnRep_PhotoCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, PhotoCount, OldValue);
}

void UDawnlightAttributeSet::OnRep_MaxPhotoCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, MaxPhotoCount, OldValue);
}

void UDawnlightAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 検知レベル: 0-100
	if (Attribute == GetDetectionLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}
	// 監視危険度: 0-100
	else if (Attribute == GetSurveillanceDangerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}
	// 緊張度: 0-100
	else if (Attribute == GetTensionLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}
	// 夜の進行度: 0-1
	else if (Attribute == GetNightProgressAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
	// 撮影カウント: 0以上
	else if (Attribute == GetPhotoCountAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	// 最大撮影数: 1以上
	else if (Attribute == GetMaxPhotoCountAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
