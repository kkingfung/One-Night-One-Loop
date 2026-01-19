// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/SoulDataAsset.h"
#include "SoulCollectionSubsystem.generated.h"

class USoulDataAsset;
class UDawnlightAttributeSet;

/**
 * 魂収集イベントデータ
 */
USTRUCT(BlueprintType)
struct DAWNLIGHT_API FSoulCollectedEventData
{
	GENERATED_BODY()

	/** 収集した魂のデータ */
	UPROPERTY(BlueprintReadOnly, Category = "魂")
	TObjectPtr<const USoulDataAsset> SoulData = nullptr;

	/** 収集位置 */
	UPROPERTY(BlueprintReadOnly, Category = "魂")
	FVector CollectionLocation = FVector::ZeroVector;

	/** 現在の総魂数 */
	UPROPERTY(BlueprintReadOnly, Category = "魂")
	int32 TotalSoulCount = 0;
};

/**
 * 魂収集サブシステム
 *
 * Night Phase中の魂収集を管理
 * - 動物のスポーン
 * - 魂の収集とカウント
 * - バフの適用
 */
UCLASS()
class DAWNLIGHT_API USoulCollectionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// ========================================================================
	// UWorldSubsystem インターフェース
	// ========================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// ========================================================================
	// 魂収集
	// ========================================================================

	/**
	 * 魂を収集
	 * @param SoulTag 収集する魂のタグ
	 * @param CollectionLocation 収集した場所
	 * @return 収集に成功したか
	 */
	UFUNCTION(BlueprintCallable, Category = "魂")
	bool CollectSoul(const FGameplayTag& SoulTag, const FVector& CollectionLocation);

	/**
	 * 魂データから魂を収集
	 * @param SoulData 魂データアセット
	 * @param CollectionLocation 収集した場所
	 * @return 収集に成功したか
	 */
	UFUNCTION(BlueprintCallable, Category = "魂")
	bool CollectSoulFromData(const USoulDataAsset* SoulData, const FVector& CollectionLocation);

	/** 収集した魂を取得 */
	UFUNCTION(BlueprintPure, Category = "魂")
	const FSoulCollection& GetCollectedSouls() const { return CollectedSouls; }

	/** 特定の魂のカウントを取得 */
	UFUNCTION(BlueprintPure, Category = "魂")
	int32 GetSoulCount(const FGameplayTag& SoulTag) const;

	/** 総魂数を取得 */
	UFUNCTION(BlueprintPure, Category = "魂")
	int32 GetTotalSoulCount() const;

	/** 魂コレクションをクリア */
	UFUNCTION(BlueprintCallable, Category = "魂")
	void ClearSouls();

	// ========================================================================
	// 魂データ管理
	// ========================================================================

	/**
	 * 魂データを登録
	 * @param SoulData 登録する魂データアセット
	 */
	UFUNCTION(BlueprintCallable, Category = "魂")
	void RegisterSoulData(USoulDataAsset* SoulData);

	/**
	 * タグから魂データを取得
	 * @param SoulTag 魂のタグ
	 * @return 対応する魂データ（見つからない場合はnullptr）
	 */
	UFUNCTION(BlueprintPure, Category = "魂")
	const USoulDataAsset* GetSoulDataByTag(const FGameplayTag& SoulTag) const;

	/** 登録されている全ての魂データを取得 */
	UFUNCTION(BlueprintPure, Category = "魂")
	TArray<USoulDataAsset*> GetAllSoulData() const;

	// ========================================================================
	// バフ適用
	// ========================================================================

	/**
	 * 収集した魂のバフをプレイヤーに適用
	 * Dawn Phase開始時に呼び出される
	 * @param TargetAttributeSet バフを適用する属性セット
	 */
	UFUNCTION(BlueprintCallable, Category = "バフ")
	void ApplyCollectedBuffs(UDawnlightAttributeSet* TargetAttributeSet);

	/**
	 * 適用されたバフをクリア
	 * ループ終了時に呼び出される
	 * @param TargetAttributeSet バフをクリアする属性セット
	 */
	UFUNCTION(BlueprintCallable, Category = "バフ")
	void ClearAppliedBuffs(UDawnlightAttributeSet* TargetAttributeSet);

	// ========================================================================
	// 動物スポーン
	// ========================================================================

	/**
	 * Night Phase用の動物をスポーン
	 * @param SpawnLocation スポーン位置
	 * @param SoulTag スポーンする動物の魂タグ
	 * @return スポーンしたアクター
	 */
	UFUNCTION(BlueprintCallable, Category = "動物")
	AActor* SpawnAnimal(const FVector& SpawnLocation, const FGameplayTag& SoulTag);

	/**
	 * ランダムな動物をスポーン
	 * @param SpawnLocation スポーン位置
	 * @return スポーンしたアクター
	 */
	UFUNCTION(BlueprintCallable, Category = "動物")
	AActor* SpawnRandomAnimal(const FVector& SpawnLocation);

	/**
	 * レアリティに基づいてランダムな魂データを取得
	 * @return 選択された魂データ
	 */
	UFUNCTION(BlueprintPure, Category = "動物")
	const USoulDataAsset* GetRandomSoulData() const;

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 魂収集時のデリゲート */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulCollected, const FSoulCollectedEventData&, EventData);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnSoulCollected OnSoulCollected;

	/** バフ適用時のデリゲート */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuffsApplied);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnBuffsApplied OnBuffsApplied;

protected:
	// ========================================================================
	// 内部データ
	// ========================================================================

	/** 収集した魂 */
	UPROPERTY()
	FSoulCollection CollectedSouls;

	/** 登録された魂データ（タグ→データのマップ） */
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<USoulDataAsset>> SoulDataMap;

	/** 現在適用されているバフ（リセット用に保存） */
	UPROPERTY()
	TArray<FSoulBuffEffect> AppliedBuffs;

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** バフ効果を属性セットに適用 */
	void ApplyBuffEffect(UDawnlightAttributeSet* AttributeSet, const FSoulBuffEffect& Buff);

	/** バフ効果を属性セットから除去 */
	void RemoveBuffEffect(UDawnlightAttributeSet* AttributeSet, const FSoulBuffEffect& Buff);
};
