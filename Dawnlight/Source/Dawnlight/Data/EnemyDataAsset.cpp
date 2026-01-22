// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnemyDataAsset.h"

FEnemyVariantConfig UEnemyDataAsset::GetVariantConfig(EEnemyColorVariant Variant) const
{
	// 指定されたバリアントを検索
	for (const FEnemyVariantConfig& Config : ColorVariants)
	{
		if (Config.Variant == Variant)
		{
			return Config;
		}
	}

	// 見つからなければデフォルト設定を返す
	FEnemyVariantConfig DefaultConfig;
	DefaultConfig.Variant = EEnemyColorVariant::Default;
	DefaultConfig.HealthMultiplier = 1.0f;
	DefaultConfig.DamageMultiplier = 1.0f;
	DefaultConfig.DefenseMultiplier = 1.0f;
	DefaultConfig.OverlayColor = EnemyColor;
	return DefaultConfig;
}

EEnemyColorVariant UEnemyDataAsset::SelectRandomVariant(int32 CurrentWave) const
{
	// 有効なバリアントとその重みを収集
	TArray<TPair<EEnemyColorVariant, float>> ValidVariants;
	float TotalWeight = 0.0f;

	// デフォルトは常に候補（重み100）
	ValidVariants.Add(TPair<EEnemyColorVariant, float>(EEnemyColorVariant::Default, 100.0f));
	TotalWeight += 100.0f;

	// 各バリアントをチェック
	for (const FEnemyVariantConfig& Config : ColorVariants)
	{
		if (Config.SpawnWeight > 0.0f && CurrentWave >= Config.MinWaveToSpawn)
		{
			ValidVariants.Add(TPair<EEnemyColorVariant, float>(Config.Variant, Config.SpawnWeight));
			TotalWeight += Config.SpawnWeight;
		}
	}

	// 重み付き乱数選択
	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
	float AccumulatedWeight = 0.0f;

	for (const auto& Pair : ValidVariants)
	{
		AccumulatedWeight += Pair.Value;
		if (RandomValue <= AccumulatedWeight)
		{
			return Pair.Key;
		}
	}

	return EEnemyColorVariant::Default;
}

FText UEnemyDataAsset::GetVariantDisplayName(EEnemyColorVariant Variant)
{
	switch (Variant)
	{
	case EEnemyColorVariant::Default:
		return FText::FromString(TEXT("通常"));
	case EEnemyColorVariant::Red:
		return FText::FromString(TEXT("紅蓮"));
	case EEnemyColorVariant::Blue:
		return FText::FromString(TEXT("氷結"));
	case EEnemyColorVariant::Green:
		return FText::FromString(TEXT("毒霧"));
	case EEnemyColorVariant::Black:
		return FText::FromString(TEXT("深淵"));
	case EEnemyColorVariant::White:
		return FText::FromString(TEXT("聖光"));
	default:
		return FText::FromString(TEXT("不明"));
	}
}

FLinearColor UEnemyDataAsset::GetVariantColor(EEnemyColorVariant Variant)
{
	switch (Variant)
	{
	case EEnemyColorVariant::Default:
		return FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);   // グレー
	case EEnemyColorVariant::Red:
		return FLinearColor(1.0f, 0.2f, 0.1f, 1.0f);   // 赤
	case EEnemyColorVariant::Blue:
		return FLinearColor(0.2f, 0.4f, 1.0f, 1.0f);   // 青
	case EEnemyColorVariant::Green:
		return FLinearColor(0.2f, 0.9f, 0.2f, 1.0f);   // 緑
	case EEnemyColorVariant::Black:
		return FLinearColor(0.1f, 0.1f, 0.15f, 1.0f);  // 黒
	case EEnemyColorVariant::White:
		return FLinearColor(1.0f, 1.0f, 0.9f, 1.0f);   // 白
	default:
		return FLinearColor::White;
	}
}
