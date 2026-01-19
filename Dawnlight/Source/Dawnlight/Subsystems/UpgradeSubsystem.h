// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/UpgradeTypes.h"
#include "Data/SoulTypes.h"
#include "Data/UpgradeDataAsset.h"
#include "UpgradeSubsystem.generated.h"

/**
 * 取得済みアップグレードの情報
 */
USTRUCT(BlueprintType)
struct FAcquiredUpgrade
{
	GENERATED_BODY()

	/** アップグレードデータ参照 */
	UPROPERTY(BlueprintReadOnly, Category = "アップグレード")
	TObjectPtr<UUpgradeDataAsset> UpgradeData;

	/** 現在のスタック数 */
	UPROPERTY(BlueprintReadOnly, Category = "アップグレード")
	int32 StackCount = 1;

	/** 取得したウェーブ番号 */
	UPROPERTY(BlueprintReadOnly, Category = "アップグレード")
	int32 AcquiredAtWave = 0;
};

/**
 * アップグレード選択イベント
 * Note: Dynamic delegatesはTArrayやTMapの複雑な型をパラメータにできないため、
 *       シンプルな型のみ使用
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpgradeChoicesGenerated, int32, WaveNumber, int32, ChoiceCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpgradeAcquired, UUpgradeDataAsset*, Upgrade, int32, NewStackCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetBonusActivated, ESoulType, SoulType, int32, Tier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsRecalculated);

/**
 * アップグレードサブシステム
 *
 * ローグライト形式のアップグレードシステムを管理
 *
 * 機能:
 * - ウェーブクリア後のランダムアップグレード選択肢生成
 * - アップグレード取得・管理
 * - ソウルセットボーナス計算
 * - 最終ステータス計算
 */
UCLASS()
class DAWNLIGHT_API UUpgradeSubsystem : public UWorldSubsystem
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
	// アップグレード選択
	// ========================================================================

	/**
	 * ランダムなアップグレード選択肢を生成
	 * @param WaveNumber 現在のウェーブ番号（高いほどレアが出やすい）
	 * @param ChoiceCount 生成する選択肢の数（通常3）
	 * @return 選択可能なアップグレードの配列
	 */
	UFUNCTION(BlueprintCallable, Category = "アップグレード")
	TArray<UUpgradeDataAsset*> GenerateUpgradeChoices(int32 WaveNumber, int32 ChoiceCount = 3);

	/**
	 * アップグレードを取得
	 * @param Upgrade 取得するアップグレード
	 * @param WaveNumber 取得時のウェーブ番号
	 * @return 成功したかどうか
	 */
	UFUNCTION(BlueprintCallable, Category = "アップグレード")
	bool AcquireUpgrade(UUpgradeDataAsset* Upgrade, int32 WaveNumber);

	/**
	 * アップグレードをリロール（選択肢を再生成）
	 * @param WaveNumber 現在のウェーブ番号
	 * @param ChoiceCount 生成する選択肢の数
	 * @return 新しい選択肢
	 */
	UFUNCTION(BlueprintCallable, Category = "アップグレード")
	TArray<UUpgradeDataAsset*> RerollUpgradeChoices(int32 WaveNumber, int32 ChoiceCount = 3);

	/** 最後に生成した選択肢を取得 */
	UFUNCTION(BlueprintPure, Category = "アップグレード")
	TArray<UUpgradeDataAsset*> GetLastGeneratedChoices() const { return LastGeneratedChoices; }

	// ========================================================================
	// クエリ
	// ========================================================================

	/** 取得済みアップグレード一覧を取得 */
	UFUNCTION(BlueprintPure, Category = "アップグレード")
	TArray<FAcquiredUpgrade> GetAcquiredUpgrades() const { return AcquiredUpgrades; }

	/** 特定のアップグレードを持っているか確認 */
	UFUNCTION(BlueprintPure, Category = "アップグレード")
	bool HasUpgrade(FName UpgradeID) const;

	/** 特定のアップグレードのスタック数を取得 */
	UFUNCTION(BlueprintPure, Category = "アップグレード")
	int32 GetUpgradeStackCount(FName UpgradeID) const;

	/** アップグレードが取得可能か確認（前提条件チェック） */
	UFUNCTION(BlueprintPure, Category = "アップグレード")
	bool CanAcquireUpgrade(UUpgradeDataAsset* Upgrade) const;

	// ========================================================================
	// ステータス計算
	// ========================================================================

	/**
	 * 最終ステータス修正値を計算
	 * 全アップグレード + セットボーナスを合算
	 */
	UFUNCTION(BlueprintCallable, Category = "ステータス")
	void RecalculateStats();

	/** 特定のステータスの最終値を取得 */
	UFUNCTION(BlueprintPure, Category = "ステータス")
	float GetStatValue(EStatModifierType StatType) const;

	// ========================================================================
	// ソウルセットボーナス
	// ========================================================================

	/** ソウル収集数を更新（SoulCollectionSubsystemから呼び出し） */
	UFUNCTION(BlueprintCallable, Category = "セットボーナス")
	void UpdateSoulCounts(const TMap<ESoulType, int32>& SoulCounts);

	// ========================================================================
	// ビジュアル効果
	// ========================================================================

	/** 現在適用中のビジュアル効果を取得 */
	UFUNCTION(BlueprintPure, Category = "ビジュアル")
	TArray<FVisualModifier> GetActiveVisualModifiers() const;

	// ========================================================================
	// リセット
	// ========================================================================

	/** ループ終了時のリセット（一時アップグレードをクリア） */
	UFUNCTION(BlueprintCallable, Category = "管理")
	void ResetForNewLoop();

	/** 完全リセット（新規ゲーム開始時） */
	UFUNCTION(BlueprintCallable, Category = "管理")
	void FullReset();

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** アップグレード選択肢が生成された時 */
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnUpgradeChoicesGenerated OnUpgradeChoicesGenerated;

	/** アップグレードを取得した時 */
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnUpgradeAcquired OnUpgradeAcquired;

	/** セットボーナスが発動した時 */
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnSetBonusActivated OnSetBonusActivated;

	/** ステータスが再計算された時 */
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnStatsRecalculated OnStatsRecalculated;

protected:
	// ========================================================================
	// 内部処理
	// ========================================================================

	/** レアリティに基づいてアップグレードを選択 */
	EUpgradeRarity RollRarity(int32 WaveNumber) const;

	/** 条件を満たすアップグレード候補を取得 */
	TArray<UUpgradeDataAsset*> GetEligibleUpgrades(int32 WaveNumber, EUpgradeRarity Rarity) const;

	/** セットボーナスを計算 */
	void CalculateSetBonuses();

	/** 登録されている全アップグレードをロード */
	void LoadAllUpgradeAssets();

private:
	/** 全アップグレードデータ（AssetManagerからロード） */
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeDataAsset>> AllUpgrades;

	/** 全セットボーナスデータ */
	UPROPERTY()
	TArray<TObjectPtr<USoulSetBonusDataAsset>> AllSetBonuses;

	/** 取得済みアップグレード */
	UPROPERTY()
	TArray<FAcquiredUpgrade> AcquiredUpgrades;

	/** 最後に生成した選択肢 */
	UPROPERTY()
	TArray<TObjectPtr<UUpgradeDataAsset>> LastGeneratedChoices;

	/** 計算済みステータス */
	UPROPERTY()
	TMap<EStatModifierType, float> CalculatedStats;

	/** 現在のソウル収集数 */
	UPROPERTY()
	TMap<ESoulType, int32> CurrentSoulCounts;

	/** アクティブなセットボーナスの段階 */
	UPROPERTY()
	TMap<ESoulType, int32> ActiveSetBonusTiers;

	/** リロール回数（コスト計算用） */
	int32 RerollCount = 0;

	/** 重み設定 */
	FUpgradeWeight WeightSettings;
};
