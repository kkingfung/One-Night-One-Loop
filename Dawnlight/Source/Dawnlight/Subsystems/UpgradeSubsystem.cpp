// Copyright Epic Games, Inc. All Rights Reserved.

#include "UpgradeSubsystem.h"
#include "Data/UpgradeDataAsset.h"
#include "Engine/AssetManager.h"
#include "Dawnlight.h"

// ========================================================================
// UWorldSubsystem インターフェース
// ========================================================================

void UUpgradeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// アップグレードアセットをロード
	LoadAllUpgradeAssets();

	// ステータスを初期化
	for (int32 i = 0; i < static_cast<int32>(EStatModifierType::Max); ++i)
	{
		CalculatedStats.Add(static_cast<EStatModifierType>(i), 0.0f);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSubsystem] 初期化完了 - %d個のアップグレード, %d個のセットボーナスをロード"),
		AllUpgrades.Num(), AllSetBonuses.Num());
}

void UUpgradeSubsystem::Deinitialize()
{
	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSubsystem] 終了処理"));
	Super::Deinitialize();
}

bool UUpgradeSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		return World->IsGameWorld();
	}
	return false;
}

// ========================================================================
// アップグレード選択
// ========================================================================

TArray<UUpgradeDataAsset*> UUpgradeSubsystem::GenerateUpgradeChoices(int32 WaveNumber, int32 ChoiceCount)
{
	TArray<UUpgradeDataAsset*> Choices;
	TSet<FName> UsedIDs;  // 重複防止

	for (int32 i = 0; i < ChoiceCount; ++i)
	{
		// レアリティをロール
		EUpgradeRarity Rarity = RollRarity(WaveNumber);

		// 候補を取得
		TArray<UUpgradeDataAsset*> Candidates = GetEligibleUpgrades(WaveNumber, Rarity);

		// 既に選ばれたものを除外
		Candidates.RemoveAll([&UsedIDs](UUpgradeDataAsset* Upgrade)
		{
			return UsedIDs.Contains(Upgrade->UpgradeID);
		});

		// 候補がない場合、レアリティを下げて再試行
		while (Candidates.Num() == 0 && Rarity > EUpgradeRarity::Common)
		{
			Rarity = static_cast<EUpgradeRarity>(static_cast<int32>(Rarity) - 1);
			Candidates = GetEligibleUpgrades(WaveNumber, Rarity);
			Candidates.RemoveAll([&UsedIDs](UUpgradeDataAsset* Upgrade)
			{
				return UsedIDs.Contains(Upgrade->UpgradeID);
			});
		}

		if (Candidates.Num() > 0)
		{
			// ランダムに1つ選択
			int32 Index = FMath::RandRange(0, Candidates.Num() - 1);
			UUpgradeDataAsset* Selected = Candidates[Index];
			Choices.Add(Selected);
			UsedIDs.Add(Selected->UpgradeID);
		}
	}

	// 最後に生成した選択肢を保存
	LastGeneratedChoices.Empty();
	for (UUpgradeDataAsset* Choice : Choices)
	{
		LastGeneratedChoices.Add(Choice);
	}

	// イベント発火
	OnUpgradeChoicesGenerated.Broadcast(WaveNumber, ChoiceCount);

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSubsystem] Wave %d: %d個のアップグレード選択肢を生成"),
		WaveNumber, Choices.Num());

	return Choices;
}

bool UUpgradeSubsystem::AcquireUpgrade(UUpgradeDataAsset* Upgrade, int32 WaveNumber)
{
	if (!Upgrade)
	{
		return false;
	}

	if (!CanAcquireUpgrade(Upgrade))
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[UpgradeSubsystem] アップグレード取得不可: %s"),
			*Upgrade->UpgradeID.ToString());
		return false;
	}

	// 既存のスタックを確認
	FAcquiredUpgrade* Existing = AcquiredUpgrades.FindByPredicate([Upgrade](const FAcquiredUpgrade& Acquired)
	{
		return Acquired.UpgradeData && Acquired.UpgradeData->UpgradeID == Upgrade->UpgradeID;
	});

	int32 NewStackCount = 1;

	if (Existing)
	{
		if (Upgrade->bStackable && Existing->StackCount < Upgrade->MaxStacks)
		{
			Existing->StackCount++;
			NewStackCount = Existing->StackCount;
		}
		else
		{
			UE_LOG(LogDawnlight, Warning, TEXT("[UpgradeSubsystem] スタック上限に達しています: %s"),
				*Upgrade->UpgradeID.ToString());
			return false;
		}
	}
	else
	{
		FAcquiredUpgrade NewAcquired;
		NewAcquired.UpgradeData = Upgrade;
		NewAcquired.StackCount = 1;
		NewAcquired.AcquiredAtWave = WaveNumber;
		AcquiredUpgrades.Add(NewAcquired);
	}

	// ステータスを再計算
	RecalculateStats();

	// イベント発火
	OnUpgradeAcquired.Broadcast(Upgrade, NewStackCount);

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSubsystem] アップグレード取得: %s (スタック: %d)"),
		*Upgrade->UpgradeID.ToString(), NewStackCount);

	return true;
}

TArray<UUpgradeDataAsset*> UUpgradeSubsystem::RerollUpgradeChoices(int32 WaveNumber, int32 ChoiceCount)
{
	RerollCount++;

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSubsystem] リロール実行 (回数: %d)"), RerollCount);

	return GenerateUpgradeChoices(WaveNumber, ChoiceCount);
}

// ========================================================================
// クエリ
// ========================================================================

bool UUpgradeSubsystem::HasUpgrade(FName UpgradeID) const
{
	return AcquiredUpgrades.ContainsByPredicate([UpgradeID](const FAcquiredUpgrade& Acquired)
	{
		return Acquired.UpgradeData && Acquired.UpgradeData->UpgradeID == UpgradeID;
	});
}

int32 UUpgradeSubsystem::GetUpgradeStackCount(FName UpgradeID) const
{
	const FAcquiredUpgrade* Found = AcquiredUpgrades.FindByPredicate([UpgradeID](const FAcquiredUpgrade& Acquired)
	{
		return Acquired.UpgradeData && Acquired.UpgradeData->UpgradeID == UpgradeID;
	});

	return Found ? Found->StackCount : 0;
}

bool UUpgradeSubsystem::CanAcquireUpgrade(UUpgradeDataAsset* Upgrade) const
{
	if (!Upgrade)
	{
		return false;
	}

	// 前提条件チェック
	for (const FName& PrereqID : Upgrade->PrerequisiteUpgradeIDs)
	{
		if (!HasUpgrade(PrereqID))
		{
			return false;
		}
	}

	// 排他条件チェック
	for (const FName& ExclusiveID : Upgrade->ExclusiveUpgradeIDs)
	{
		if (HasUpgrade(ExclusiveID))
		{
			return false;
		}
	}

	// スタック可能性チェック
	if (HasUpgrade(Upgrade->UpgradeID))
	{
		if (!Upgrade->bStackable)
		{
			return false;
		}
		if (GetUpgradeStackCount(Upgrade->UpgradeID) >= Upgrade->MaxStacks)
		{
			return false;
		}
	}

	return true;
}

// ========================================================================
// ステータス計算
// ========================================================================

void UUpgradeSubsystem::RecalculateStats()
{
	// リセット
	for (auto& Pair : CalculatedStats)
	{
		Pair.Value = 0.0f;
	}

	// アップグレードからのステータス加算
	for (const FAcquiredUpgrade& Acquired : AcquiredUpgrades)
	{
		if (!Acquired.UpgradeData)
		{
			continue;
		}

		for (const FStatModifier& Mod : Acquired.UpgradeData->StatModifiers)
		{
			float* CurrentValue = CalculatedStats.Find(Mod.StatType);
			if (CurrentValue)
			{
				// スタック数を考慮
				*CurrentValue += (Mod.AdditiveValue + Mod.MultiplicativeValue) * Acquired.StackCount;
			}
		}
	}

	// セットボーナスを計算して加算
	CalculateSetBonuses();

	// イベント発火
	OnStatsRecalculated.Broadcast();

	UE_LOG(LogDawnlight, Verbose, TEXT("[UpgradeSubsystem] ステータス再計算完了"));
}

float UUpgradeSubsystem::GetStatValue(EStatModifierType StatType) const
{
	const float* Value = CalculatedStats.Find(StatType);
	return Value ? *Value : 0.0f;
}

// ========================================================================
// ソウルセットボーナス
// ========================================================================

void UUpgradeSubsystem::UpdateSoulCounts(const TMap<ESoulType, int32>& SoulCounts)
{
	CurrentSoulCounts = SoulCounts;
	RecalculateStats();
}

void UUpgradeSubsystem::CalculateSetBonuses()
{
	TMap<ESoulType, int32> OldTiers = ActiveSetBonusTiers;
	ActiveSetBonusTiers.Empty();

	// 各セットボーナスデータを確認
	for (const TObjectPtr<USoulSetBonusDataAsset>& SetBonus : AllSetBonuses)
	{
		if (!SetBonus)
		{
			continue;
		}

		// 該当するソウルタイプの収集数を取得
		const int32* CollectedCount = CurrentSoulCounts.Find(SetBonus->SoulType);
		if (!CollectedCount || *CollectedCount <= 0)
		{
			continue;
		}

		// 段階ごとに確認して、最高の段階を決定
		int32 HighestTier = 0;
		for (int32 TierIndex = 0; TierIndex < SetBonus->BonusTiers.Num(); TierIndex++)
		{
			const FSetBonusTier& Tier = SetBonus->BonusTiers[TierIndex];
			if (*CollectedCount >= Tier.RequiredCount)
			{
				HighestTier = TierIndex + 1;  // 1から始まる段階番号
			}
		}

		// 発動した段階を記録
		if (HighestTier > 0)
		{
			ActiveSetBonusTiers.Add(SetBonus->SoulType, HighestTier);
			UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSubsystem] セットボーナス発動: %s 段階 %d (収集数: %d)"),
				*SetBonus->SetName.ToString(), HighestTier, *CollectedCount);
		}
	}

	// 新しく発動したボーナスをイベント発火
	for (const auto& Pair : ActiveSetBonusTiers)
	{
		const int32* OldTier = OldTiers.Find(Pair.Key);
		if (!OldTier || *OldTier < Pair.Value)
		{
			OnSetBonusActivated.Broadcast(Pair.Key, Pair.Value);
		}
	}
}

// ========================================================================
// ビジュアル効果
// ========================================================================

TArray<FVisualModifier> UUpgradeSubsystem::GetActiveVisualModifiers() const
{
	TArray<FVisualModifier> Modifiers;

	for (const FAcquiredUpgrade& Acquired : AcquiredUpgrades)
	{
		if (Acquired.UpgradeData)
		{
			// ビジュアル効果がある場合のみ追加
			if (Acquired.UpgradeData->VisualModifier.GlowIntensity > 0.0f ||
				!Acquired.UpgradeData->VisualModifier.CharacterParticle.IsNull() ||
				!Acquired.UpgradeData->VisualModifier.WeaponParticle.IsNull())
			{
				Modifiers.Add(Acquired.UpgradeData->VisualModifier);
			}
		}
	}

	return Modifiers;
}

// ========================================================================
// リセット
// ========================================================================

void UUpgradeSubsystem::ResetForNewLoop()
{
	AcquiredUpgrades.Empty();
	LastGeneratedChoices.Empty();
	RerollCount = 0;
	CurrentSoulCounts.Empty();
	ActiveSetBonusTiers.Empty();

	RecalculateStats();

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSubsystem] 新ループ用にリセット完了"));
}

void UUpgradeSubsystem::FullReset()
{
	AcquiredUpgrades.Empty();
	LastGeneratedChoices.Empty();
	RerollCount = 0;
	CurrentSoulCounts.Empty();
	ActiveSetBonusTiers.Empty();

	for (auto& Pair : CalculatedStats)
	{
		Pair.Value = 0.0f;
	}

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSubsystem] 完全リセット完了"));
}

// ========================================================================
// 内部処理
// ========================================================================

EUpgradeRarity UUpgradeSubsystem::RollRarity(int32 WaveNumber) const
{
	// ウェーブが進むほどレア度が上がりやすい
	float LuckBonus = FMath::Min(WaveNumber * 2.0f, 20.0f);  // 最大20%ボーナス

	float TotalWeight = 0.0f;
	for (const auto& Pair : WeightSettings.RarityWeights)
	{
		float Weight = Pair.Value;

		// レア以上はウェーブボーナスを適用
		if (Pair.Key >= EUpgradeRarity::Rare)
		{
			Weight += LuckBonus * 0.1f;
		}

		TotalWeight += Weight;
	}

	float Roll = FMath::FRandRange(0.0f, TotalWeight);
	float CurrentWeight = 0.0f;

	for (const auto& Pair : WeightSettings.RarityWeights)
	{
		float Weight = Pair.Value;
		if (Pair.Key >= EUpgradeRarity::Rare)
		{
			Weight += LuckBonus * 0.1f;
		}

		CurrentWeight += Weight;
		if (Roll <= CurrentWeight)
		{
			return Pair.Key;
		}
	}

	return EUpgradeRarity::Common;
}

TArray<UUpgradeDataAsset*> UUpgradeSubsystem::GetEligibleUpgrades(int32 WaveNumber, EUpgradeRarity Rarity) const
{
	TArray<UUpgradeDataAsset*> Eligible;

	for (UUpgradeDataAsset* Upgrade : AllUpgrades)
	{
		if (!Upgrade)
		{
			continue;
		}

		// レアリティチェック
		if (Upgrade->Rarity != Rarity)
		{
			continue;
		}

		// ウェーブ要件チェック
		if (WaveNumber < Upgrade->MinWaveRequired)
		{
			continue;
		}

		// 取得可能かチェック
		if (!CanAcquireUpgrade(Upgrade))
		{
			continue;
		}

		Eligible.Add(Upgrade);
	}

	return Eligible;
}

void UUpgradeSubsystem::LoadAllUpgradeAssets()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	// アップグレードをロード
	TArray<FPrimaryAssetId> UpgradeAssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("Upgrade"), UpgradeAssetIds);

	for (const FPrimaryAssetId& AssetId : UpgradeAssetIds)
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
		if (UUpgradeDataAsset* Upgrade = Cast<UUpgradeDataAsset>(AssetPath.TryLoad()))
		{
			AllUpgrades.Add(Upgrade);
		}
	}

	// セットボーナスをロード
	TArray<FPrimaryAssetId> SetBonusAssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("SoulSetBonus"), SetBonusAssetIds);

	for (const FPrimaryAssetId& AssetId : SetBonusAssetIds)
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
		if (USoulSetBonusDataAsset* SetBonus = Cast<USoulSetBonusDataAsset>(AssetPath.TryLoad()))
		{
			AllSetBonuses.Add(SetBonus);
		}
	}
}
