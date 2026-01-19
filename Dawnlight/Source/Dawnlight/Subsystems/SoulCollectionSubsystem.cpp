// Copyright Epic Games, Inc. All Rights Reserved.

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

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 初期化完了"));
}

void USoulCollectionSubsystem::Deinitialize()
{
	// クリーンアップ
	ClearSouls();
	SoulDataMap.Empty();
	AppliedBuffs.Empty();

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

	// プレイヤーキャラクターのリーパーゲージを増加
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		if (ADawnlightCharacter* PlayerCharacter = Cast<ADawnlightCharacter>(PlayerPawn))
		{
			PlayerCharacter->AddReaperGauge(SoulData->ReaperGaugeGain);
		}
	}

	// イベントデータを作成
	FSoulCollectedEventData EventData;
	EventData.SoulData = SoulData;
	EventData.CollectionLocation = CollectionLocation;
	EventData.TotalSoulCount = GetTotalSoulCount();

	// デリゲートを発火
	OnSoulCollected.Broadcast(EventData);

	UE_LOG(LogDawnlight, Log, TEXT("SoulCollectionSubsystem: 魂を収集 - %s (総数: %d, ゲージ+%.0f)"),
		*SoulData->DisplayNameEN, EventData.TotalSoulCount, SoulData->ReaperGaugeGain);

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
