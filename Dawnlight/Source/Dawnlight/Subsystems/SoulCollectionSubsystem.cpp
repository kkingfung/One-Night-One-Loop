// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "SoulCollectionSubsystem.h"
#include "Dawnlight.h"
#include "Abilities/DawnlightAttributeSet.h"
#include "Characters/DawnlightCharacter.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

// ========================================================================
// UWorldSubsystem インターフェース
// ========================================================================

void USoulCollectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// デフォルト設定を初期化
	InitializeDefaultComboThresholds();
	InitializeDefaultSetBonuses();

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 初期化完了"));
}

void USoulCollectionSubsystem::Deinitialize()
{
	// クリーンアップ
	ClearSouls();
	SoulDataMap.Empty();
	AppliedBuffs.Empty();
	ComboInfo = FComboKillInfo();
	SetBonusDefinitions.Empty();
	AchievedSetBonuses.Empty();

	Super::Deinitialize();

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 終了"));
}

bool USoulCollectionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// ゲームワールドでのみ作成
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		return World->IsGameWorld();
	}
	return false;
}

// ========================================================================
// 魂収集
// ========================================================================

bool USoulCollectionSubsystem::CollectSoul(const FGameplayTag& SoulTag, const FVector& CollectionLocation)
{
	if (!SoulTag.IsValid())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: 無効な魂タグ"));
		return false;
	}

	// 魂データを取得
	const USoulDataAsset* SoulData = GetSoulDataByTag(SoulTag);
	if (!SoulData)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: 魂データが見つかりません: %s"), *SoulTag.ToString());
		return false;
	}

	return CollectSoulFromData(SoulData, CollectionLocation);
}

bool USoulCollectionSubsystem::CollectSoulFromData(const USoulDataAsset* SoulData, const FVector& CollectionLocation)
{
	if (!SoulData)
	{
		return false;
	}

	// 魂をコレクションに追加
	CollectedSouls.AddSoul(SoulData->SoulTag, 1);

	// 新しいカウントを取得
	const int32 NewCount = GetSoulCount(SoulData->SoulTag);

	// プレイヤーキャラクターのリーパーゲージを増加
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		if (ADawnlightCharacter* PlayerCharacter = Cast<ADawnlightCharacter>(PlayerPawn))
		{
			PlayerCharacter->AddReaperGauge(SoulData->ReaperGaugeGain);
		}
	}

	// セットボーナスの達成をチェック
	CheckSetBonusAchievement(SoulData->SoulTag, NewCount);

	// イベントデータを作成
	FSoulCollectedEventData EventData;
	EventData.SoulData = const_cast<USoulDataAsset*>(SoulData);
	EventData.CollectionLocation = CollectionLocation;
	EventData.TotalSoulCount = GetTotalSoulCount();

	// デリゲートを発火
	OnSoulCollected.Broadcast(EventData);

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 魂を収集 - %s (個数: %d, 総数: %d, ゲージ+%.0f)"),
		*SoulData->DisplayNameEN, NewCount, EventData.TotalSoulCount, SoulData->ReaperGaugeGain);

	return true;
}

int32 USoulCollectionSubsystem::GetSoulCount(const FGameplayTag& SoulTag) const
{
	return CollectedSouls.GetSoulCount(SoulTag);
}

int32 USoulCollectionSubsystem::GetTotalSoulCount() const
{
	return CollectedSouls.GetTotalSoulCount();
}

void USoulCollectionSubsystem::ClearSouls()
{
	CollectedSouls.Clear();
	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 魂コレクションをクリア"));
}

// ========================================================================
// 魂データ管理
// ========================================================================

void USoulCollectionSubsystem::RegisterSoulData(USoulDataAsset* SoulData)
{
	if (!SoulData || !SoulData->SoulTag.IsValid())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: 無効な魂データの登録試行"));
		return;
	}

	SoulDataMap.Add(SoulData->SoulTag, SoulData);
	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 魂データを登録 - %s"), *SoulData->DisplayNameEN);
}

const USoulDataAsset* USoulCollectionSubsystem::GetSoulDataByTag(const FGameplayTag& SoulTag) const
{
	if (const TObjectPtr<USoulDataAsset>* Found = SoulDataMap.Find(SoulTag))
	{
		return Found->Get();
	}
	return nullptr;
}

TArray<USoulDataAsset*> USoulCollectionSubsystem::GetAllSoulData() const
{
	TArray<USoulDataAsset*> Result;
	for (const auto& Pair : SoulDataMap)
	{
		if (Pair.Value)
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}

// ========================================================================
// バフ適用
// ========================================================================

void USoulCollectionSubsystem::ApplyCollectedBuffs(UDawnlightAttributeSet* TargetAttributeSet)
{
	if (!TargetAttributeSet)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: TargetAttributeSetがnull"));
		return;
	}

	// 現在のバフをクリア
	AppliedBuffs.Empty();

	// 収集した魂ごとにバフを適用
	for (const auto& SoulPair : CollectedSouls.CollectedSouls)
	{
		const FGameplayTag& SoulTag = SoulPair.Key;
		const int32 Count = SoulPair.Value;

		const USoulDataAsset* SoulData = GetSoulDataByTag(SoulTag);
		if (!SoulData)
		{
			continue;
		}

		// 魂の数だけバフを適用
		for (int32 i = 0; i < Count; ++i)
		{
			for (const FSoulBuffEffect& Buff : SoulData->BuffEffects)
			{
				ApplyBuffEffect(TargetAttributeSet, Buff);
				AppliedBuffs.Add(Buff);
			}
		}

		UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: バフ適用 - %s x%d"),
			*SoulData->DisplayNameEN, Count);
	}

	// デリゲートを発火
	OnBuffsApplied.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 全てのバフを適用完了 (合計: %d効果)"),
		AppliedBuffs.Num());
}

void USoulCollectionSubsystem::ClearAppliedBuffs(UDawnlightAttributeSet* TargetAttributeSet)
{
	if (!TargetAttributeSet)
	{
		return;
	}

	// 適用したバフを逆順で除去
	for (int32 i = AppliedBuffs.Num() - 1; i >= 0; --i)
	{
		RemoveBuffEffect(TargetAttributeSet, AppliedBuffs[i]);
	}

	AppliedBuffs.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 全てのバフをクリア"));
}

void USoulCollectionSubsystem::ApplyBuffEffect(UDawnlightAttributeSet* AttributeSet, const FSoulBuffEffect& Buff)
{
	if (!AttributeSet)
	{
		return;
	}

	switch (Buff.BuffType)
	{
	case ESoulBuffType::Damage:
		// ダメージ倍率を加算
		AttributeSet->SetDamageMultiplier(AttributeSet->GetDamageMultiplier() + Buff.BuffAmount);
		break;

	case ESoulBuffType::Speed:
		// スピード倍率を加算
		AttributeSet->SetSpeedMultiplier(AttributeSet->GetSpeedMultiplier() + Buff.BuffAmount);
		break;

	case ESoulBuffType::Defense:
		// 防御ボーナスを加算
		AttributeSet->SetDefenseBonus(AttributeSet->GetDefenseBonus() + Buff.BuffAmount);
		break;

	case ESoulBuffType::Cooldown:
		// クールダウン短縮率を加算
		AttributeSet->SetCooldownReduction(AttributeSet->GetCooldownReduction() + Buff.BuffAmount);
		break;

	case ESoulBuffType::Luck:
		// ラックを加算
		AttributeSet->SetLuck(AttributeSet->GetLuck() + Buff.BuffAmount);
		break;

	case ESoulBuffType::AllStats:
		// 全ステータスに適用
		AttributeSet->SetDamageMultiplier(AttributeSet->GetDamageMultiplier() + Buff.BuffAmount);
		AttributeSet->SetSpeedMultiplier(AttributeSet->GetSpeedMultiplier() + Buff.BuffAmount);
		AttributeSet->SetDefenseBonus(AttributeSet->GetDefenseBonus() + Buff.BuffAmount * 10.0f);
		break;

	case ESoulBuffType::ReaperGauge:
		// リーパーゲージを直接増加
		AttributeSet->SetReaperGauge(AttributeSet->GetReaperGauge() + Buff.BuffAmount);
		break;

	default:
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: 未処理のバフタイプ: %d"), static_cast<int32>(Buff.BuffType));
		break;
	}
}

void USoulCollectionSubsystem::RemoveBuffEffect(UDawnlightAttributeSet* AttributeSet, const FSoulBuffEffect& Buff)
{
	if (!AttributeSet)
	{
		return;
	}

	switch (Buff.BuffType)
	{
	case ESoulBuffType::Damage:
		AttributeSet->SetDamageMultiplier(FMath::Max(1.0f, AttributeSet->GetDamageMultiplier() - Buff.BuffAmount));
		break;

	case ESoulBuffType::Speed:
		AttributeSet->SetSpeedMultiplier(FMath::Max(1.0f, AttributeSet->GetSpeedMultiplier() - Buff.BuffAmount));
		break;

	case ESoulBuffType::Defense:
		AttributeSet->SetDefenseBonus(FMath::Max(0.0f, AttributeSet->GetDefenseBonus() - Buff.BuffAmount));
		break;

	case ESoulBuffType::Cooldown:
		AttributeSet->SetCooldownReduction(FMath::Max(0.0f, AttributeSet->GetCooldownReduction() - Buff.BuffAmount));
		break;

	case ESoulBuffType::Luck:
		AttributeSet->SetLuck(FMath::Max(0.0f, AttributeSet->GetLuck() - Buff.BuffAmount));
		break;

	case ESoulBuffType::AllStats:
		AttributeSet->SetDamageMultiplier(FMath::Max(1.0f, AttributeSet->GetDamageMultiplier() - Buff.BuffAmount));
		AttributeSet->SetSpeedMultiplier(FMath::Max(1.0f, AttributeSet->GetSpeedMultiplier() - Buff.BuffAmount));
		AttributeSet->SetDefenseBonus(FMath::Max(0.0f, AttributeSet->GetDefenseBonus() - Buff.BuffAmount * 10.0f));
		break;

	case ESoulBuffType::ReaperGauge:
		// リーパーゲージは除去しない（一度増えたら減らない）
		break;

	default:
		// 未知のバフタイプは無視
		break;
	}
}

// ========================================================================
// 動物スポーン
// ========================================================================

AActor* USoulCollectionSubsystem::SpawnAnimal(const FVector& SpawnLocation, const FGameplayTag& SoulTag)
{
	const USoulDataAsset* SoulData = GetSoulDataByTag(SoulTag);
	if (!SoulData)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: 動物スポーン失敗 - 魂データが見つかりません: %s"),
			*SoulTag.ToString());
		return nullptr;
	}

	// Blueprintクラスをロード
	if (!SoulData->AnimalBlueprintClass.IsValid())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: 動物スポーン失敗 - Blueprintクラスが設定されていません: %s"),
			*SoulData->DisplayNameEN);
		return nullptr;
	}

	UClass* AnimalClass = SoulData->AnimalBlueprintClass.LoadSynchronous();
	if (!AnimalClass)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: 動物スポーン失敗 - Blueprintクラスのロードに失敗: %s"),
			*SoulData->DisplayNameEN);
		return nullptr;
	}

	// スポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedAnimal = GetWorld()->SpawnActor<AActor>(AnimalClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (SpawnedAnimal)
	{
		UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 動物をスポーン - %s at %s"),
			*SoulData->DisplayNameEN, *SpawnLocation.ToString());
	}

	return SpawnedAnimal;
}

AActor* USoulCollectionSubsystem::SpawnRandomAnimal(const FVector& SpawnLocation)
{
	const USoulDataAsset* RandomSoul = GetRandomSoulData();
	if (!RandomSoul)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("SoulCollectionSubsystem: ランダム動物スポーン失敗 - 魂データが登録されていません"));
		return nullptr;
	}

	return SpawnAnimal(SpawnLocation, RandomSoul->SoulTag);
}

const USoulDataAsset* USoulCollectionSubsystem::GetRandomSoulData() const
{
	if (SoulDataMap.IsEmpty())
	{
		return nullptr;
	}

	// 重み付きランダム選択
	float TotalWeight = 0.0f;
	for (const auto& Pair : SoulDataMap)
	{
		if (Pair.Value)
		{
			TotalWeight += Pair.Value->SpawnWeight;
		}
	}

	if (TotalWeight <= 0.0f)
	{
		// 重みがない場合は均等に選択
		TArray<TObjectPtr<USoulDataAsset>> Values;
		SoulDataMap.GenerateValueArray(Values);
		const int32 RandomIndex = FMath::RandRange(0, Values.Num() - 1);
		return Values[RandomIndex];
	}

	// 重み付き選択
	float RandomValue = FMath::FRand() * TotalWeight;
	for (const auto& Pair : SoulDataMap)
	{
		if (Pair.Value)
		{
			RandomValue -= Pair.Value->SpawnWeight;
			if (RandomValue <= 0.0f)
			{
				return Pair.Value;
			}
		}
	}

	// フォールバック
	TArray<TObjectPtr<USoulDataAsset>> Values;
	SoulDataMap.GenerateValueArray(Values);
	return Values.Num() > 0 ? Values[0] : nullptr;
}

// ========================================================================
// コンボキルシステム
// ========================================================================

int32 USoulCollectionSubsystem::RecordKill(const FVector& KillLocation)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0;
	}

	const float CurrentTime = World->GetTimeSeconds();
	const float TimeSinceLastKill = CurrentTime - ComboInfo.LastKillTime;

	// コンボがタイムアウトしていないかチェック
	if (ComboInfo.CurrentCombo > 0 && TimeSinceLastKill > ComboTimeout)
	{
		// コンボリセット
		ResetCombo();
	}

	// コンボを増加
	ComboInfo.CurrentCombo++;
	ComboInfo.LastKillTime = CurrentTime;

	// 最大コンボを更新
	if (ComboInfo.CurrentCombo > ComboInfo.MaxCombo)
	{
		ComboInfo.MaxCombo = ComboInfo.CurrentCombo;
	}

	// コンボボーナスを計算
	int32 BonusSouls = 0;
	for (const auto& Threshold : ComboThresholds)
	{
		if (ComboInfo.CurrentCombo >= Threshold.Key)
		{
			BonusSouls = FMath::Max(BonusSouls, Threshold.Value);
		}
	}

	// ボーナス魂を記録
	ComboInfo.BonusSoulsFromCombo += BonusSouls;

	// デリゲートを発火
	OnComboUpdated.Broadcast(ComboInfo.CurrentCombo, BonusSouls);

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: キル記録 - コンボ: %d, ボーナス魂: %d"),
		ComboInfo.CurrentCombo, BonusSouls);

	return BonusSouls;
}

void USoulCollectionSubsystem::ResetCombo()
{
	if (ComboInfo.CurrentCombo > 0)
	{
		UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: コンボリセット (最終コンボ: %d, 累計ボーナス: %d)"),
			ComboInfo.CurrentCombo, ComboInfo.BonusSoulsFromCombo);
	}

	ComboInfo.CurrentCombo = 0;
	// MaxComboとBonusSoulsFromComboはセッション中維持
}

// ========================================================================
// ソウルセットボーナス
// ========================================================================

TArray<FSoulSetBonus> USoulCollectionSubsystem::GetActiveSetBonuses(const FGameplayTag& SoulTag) const
{
	TArray<FSoulSetBonus> ActiveBonuses;

	const TArray<FSoulSetBonus>* Bonuses = SetBonusDefinitions.Find(SoulTag);
	if (!Bonuses)
	{
		return ActiveBonuses;
	}

	const int32 CurrentCount = GetSoulCount(SoulTag);

	for (const FSoulSetBonus& Bonus : *Bonuses)
	{
		if (CurrentCount >= Bonus.RequiredCount)
		{
			ActiveBonuses.Add(Bonus);
		}
	}

	return ActiveBonuses;
}

TMap<FGameplayTag, FSoulSetBonus> USoulCollectionSubsystem::GetAllActiveSetBonuses() const
{
	TMap<FGameplayTag, FSoulSetBonus> AllActiveBonuses;

	for (const auto& Pair : SetBonusDefinitions)
	{
		const FGameplayTag& SoulTag = Pair.Key;
		const int32 CurrentCount = GetSoulCount(SoulTag);

		// 最も高いアクティブなボーナスを取得
		const FSoulSetBonus* HighestBonus = nullptr;
		for (const FSoulSetBonus& Bonus : Pair.Value)
		{
			if (CurrentCount >= Bonus.RequiredCount)
			{
				if (!HighestBonus || Bonus.RequiredCount > HighestBonus->RequiredCount)
				{
					HighestBonus = &Bonus;
				}
			}
		}

		if (HighestBonus)
		{
			AllActiveBonuses.Add(SoulTag, *HighestBonus);
		}
	}

	return AllActiveBonuses;
}

void USoulCollectionSubsystem::RegisterSetBonus(const FGameplayTag& SoulTag, const FSoulSetBonus& Bonus)
{
	TArray<FSoulSetBonus>& Bonuses = SetBonusDefinitions.FindOrAdd(SoulTag);
	Bonuses.Add(Bonus);

	// RequiredCount順にソート
	Bonuses.Sort([](const FSoulSetBonus& A, const FSoulSetBonus& B)
	{
		return A.RequiredCount < B.RequiredCount;
	});

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: セットボーナス登録 - %s (必要数: %d)"),
		*Bonus.BonusName.ToString(), Bonus.RequiredCount);
}

// ========================================================================
// 内部関数
// ========================================================================

void USoulCollectionSubsystem::InitializeDefaultComboThresholds()
{
	// GDD準拠: コンボキルボーナス
	// 3キルコンボ: +1魂
	// 5キルコンボ: +2魂
	// 10キルコンボ: +5魂
	ComboThresholds.Empty();
	ComboThresholds.Add(3, 1);
	ComboThresholds.Add(5, 2);
	ComboThresholds.Add(10, 5);

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: デフォルトコンボ閾値を初期化"));
}

void USoulCollectionSubsystem::InitializeDefaultSetBonuses()
{
	// GDD準拠: ソウルセットボーナス
	// 同じ種類の魂を3/5/8個集めるとボーナス発動

	// 注: 魂タグは実際のGameplayTagに依存
	// ここではデフォルトのセットボーナス効果を定義
	// 実際のボーナスはSoulDataAssetから登録されることを想定

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: デフォルトセットボーナスを初期化"));
}

void USoulCollectionSubsystem::CheckSetBonusAchievement(const FGameplayTag& SoulTag, int32 NewCount)
{
	const TArray<FSoulSetBonus>* Bonuses = SetBonusDefinitions.Find(SoulTag);
	if (!Bonuses)
	{
		return;
	}

	for (const FSoulSetBonus& Bonus : *Bonuses)
	{
		// ちょうど閾値に達した時のみ通知
		if (NewCount == Bonus.RequiredCount)
		{
			// 重複通知を防ぐ
			const FString BonusKey = MakeSetBonusKey(SoulTag, Bonus.RequiredCount);
			if (!AchievedSetBonuses.Contains(BonusKey))
			{
				AchievedSetBonuses.Add(BonusKey);

				// デリゲートを発火
				OnSetBonusAchieved.Broadcast(SoulTag, Bonus);

				UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: セットボーナス達成! %s - %s"),
					*SoulTag.ToString(), *Bonus.BonusName.ToString());
			}
		}
	}
}

FString USoulCollectionSubsystem::MakeSetBonusKey(const FGameplayTag& SoulTag, int32 RequiredCount) const
{
	return FString::Printf(TEXT("%s_%d"), *SoulTag.ToString(), RequiredCount);
}
