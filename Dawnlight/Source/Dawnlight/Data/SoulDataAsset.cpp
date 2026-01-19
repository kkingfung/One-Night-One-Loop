// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulDataAsset.h"

// ========================================================================
// USoulDataAsset
// ========================================================================

FLinearColor USoulDataAsset::GetRarityColor() const
{
	switch (Rarity)
	{
	case ESoulRarity::Common:
		return FLinearColor(0.7f, 0.7f, 0.7f, 1.0f);  // グレー

	case ESoulRarity::Uncommon:
		return FLinearColor(0.2f, 0.8f, 0.2f, 1.0f);  // 緑

	case ESoulRarity::Rare:
		return FLinearColor(0.2f, 0.5f, 1.0f, 1.0f);  // 青

	case ESoulRarity::Epic:
		return FLinearColor(0.6f, 0.2f, 0.9f, 1.0f);  // 紫

	case ESoulRarity::Legendary:
		return FLinearColor(1.0f, 0.7f, 0.0f, 1.0f);  // 金

	default:
		return FLinearColor::White;
	}
}

FText USoulDataAsset::GetRarityDisplayName() const
{
	switch (Rarity)
	{
	case ESoulRarity::Common:
		return FText::FromString(TEXT("コモン"));

	case ESoulRarity::Uncommon:
		return FText::FromString(TEXT("アンコモン"));

	case ESoulRarity::Rare:
		return FText::FromString(TEXT("レア"));

	case ESoulRarity::Epic:
		return FText::FromString(TEXT("エピック"));

	case ESoulRarity::Legendary:
		return FText::FromString(TEXT("レジェンダリー"));

	default:
		return FText::FromString(TEXT("不明"));
	}
}

FText USoulDataAsset::GetBuffDescription() const
{
	FString ResultDescription;

	for (const FSoulBuffEffect& Buff : BuffEffects)
	{
		if (!ResultDescription.IsEmpty())
		{
			ResultDescription += TEXT("\n");
		}

		FString BuffName;
		FString ValueFormat;

		switch (Buff.BuffType)
		{
		case ESoulBuffType::Damage:
			BuffName = TEXT("ダメージ");
			ValueFormat = FString::Printf(TEXT("+%.0f%%"), Buff.BuffAmount * 100.0f);
			break;

		case ESoulBuffType::Speed:
			BuffName = TEXT("スピード");
			ValueFormat = FString::Printf(TEXT("+%.0f%%"), Buff.BuffAmount * 100.0f);
			break;

		case ESoulBuffType::Defense:
			BuffName = TEXT("防御");
			ValueFormat = FString::Printf(TEXT("+%.0f"), Buff.BuffAmount);
			break;

		case ESoulBuffType::Cooldown:
			BuffName = TEXT("クールダウン");
			ValueFormat = FString::Printf(TEXT("-%.0f%%"), Buff.BuffAmount);
			break;

		case ESoulBuffType::Luck:
			BuffName = TEXT("ラック");
			ValueFormat = FString::Printf(TEXT("+%.0f"), Buff.BuffAmount);
			break;

		case ESoulBuffType::AllStats:
			BuffName = TEXT("全ステータス");
			ValueFormat = FString::Printf(TEXT("+%.0f%%"), Buff.BuffAmount * 100.0f);
			break;

		case ESoulBuffType::ReaperGauge:
			BuffName = TEXT("リーパーゲージ");
			ValueFormat = FString::Printf(TEXT("+%.0f"), Buff.BuffAmount);
			break;
		}

		ResultDescription += FString::Printf(TEXT("%s: %s"), *BuffName, *ValueFormat);

		// 持続時間がある場合
		if (Buff.Duration > 0.0f)
		{
			ResultDescription += FString::Printf(TEXT(" (%.0f秒)"), Buff.Duration);
		}
	}

	if (ResultDescription.IsEmpty())
	{
		ResultDescription = TEXT("効果なし");
	}

	return FText::FromString(ResultDescription);
}

// ========================================================================
// FSoulCollection
// ========================================================================

void FSoulCollection::AddSoul(const FGameplayTag& SoulTag, int32 Count)
{
	if (!SoulTag.IsValid() || Count <= 0)
	{
		return;
	}

	if (int32* ExistingCount = CollectedSouls.Find(SoulTag))
	{
		*ExistingCount += Count;
	}
	else
	{
		CollectedSouls.Add(SoulTag, Count);
	}
}

int32 FSoulCollection::GetSoulCount(const FGameplayTag& SoulTag) const
{
	if (const int32* Count = CollectedSouls.Find(SoulTag))
	{
		return *Count;
	}
	return 0;
}

int32 FSoulCollection::GetTotalSoulCount() const
{
	int32 Total = 0;
	for (const auto& Pair : CollectedSouls)
	{
		Total += Pair.Value;
	}
	return Total;
}

void FSoulCollection::Clear()
{
	CollectedSouls.Empty();
}
