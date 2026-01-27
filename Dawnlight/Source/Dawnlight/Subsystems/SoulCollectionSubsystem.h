// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/SoulDataAsset.h"
#include "Data/SoulTypes.h"
#include "SoulCollectionSubsystem.generated.h"

class USoulDataAsset;
class UDawnlightAttributeSet;

/**
 * コンボキル情報
 */
USTRUCT(BlueprintType)
struct DAWNLIGHT_API FComboKillInfo
{
	GENERATED_BODY()

	/** 現在のコンボ数 */
	UPROPERTY(BlueprintReadOnly, Category = "コンボ")
	int32 CurrentCombo = 0;

	/** 最大コンボ数（このセッション） */
	UPROPERTY(BlueprintReadOnly, Category = "コンボ")
	int32 MaxCombo = 0;

	/** 最後のキル時刻 */
	UPROPERTY(BlueprintReadOnly, Category = "コンボ")
	float LastKillTime = 0.0f;

	/** コンボボーナスで獲得した追加魂数 */
	UPROPERTY(BlueprintReadOnly, Category = "コンボ")
	int32 BonusSoulsFromCombo = 0;
};

/**
 * 魂収集イベントデータ
 */
USTRUCT(BlueprintType)
struct DAWNLIGHT_API FSoulCollectedEventData
{
	GENERATED_BODY()

	/** 収集した魂のデータ（読み取り専用参照） */
	UPROPERTY(BlueprintReadOnly, Category = "魂")
	TObjectPtr<USoulDataAsset> SoulData = nullptr;

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
	// コンボキルシステム
	// ========================================================================

	/**
	 * キルを記録（コンボ判定）
	 * @param KillLocation キル位置
	 * @return コンボボーナスで追加される魂数
	 */
	UFUNCTION(BlueprintCallable, Category = "コンボ")
	int32 RecordKill(const FVector& KillLocation);

	/** 現在のコンボ情報を取得 */
	UFUNCTION(BlueprintPure, Category = "コンボ")
	const FComboKillInfo& GetComboInfo() const { return ComboInfo; }

	/** コンボをリセット */
	UFUNCTION(BlueprintCallable, Category = "コンボ")
	void ResetCombo();

	/** コンボタイムアウト時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "コンボ|設定")
	float ComboTimeout = 2.0f;

	/** コンボボーナス閾値とボーナス魂数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "コンボ|設定")
	TMap<int32, int32> ComboThresholds;

	// ========================================================================
	// ソウルセットボーナス
	// ========================================================================

	/**
	 * アクティブなセットボーナスを取得
	 * @param SoulTag 魂タグ
	 * @return アクティブなセットボーナスのリスト
	 */
	UFUNCTION(BlueprintPure, Category = "セットボーナス")
	TArray<FSoulSetBonus> GetActiveSetBonuses(const FGameplayTag& SoulTag) const;

	/**
	 * 全てのアクティブなセットボーナスを取得
	 * @return 全てのアクティブなセットボーナス（魂タグ→ボーナスリスト）
	 */
	UFUNCTION(BlueprintPure, Category = "セットボーナス")
	TMap<FGameplayTag, FSoulSetBonus> GetAllActiveSetBonuses() const;

	/**
	 * セットボーナスを登録
	 * @param SoulTag 対象の魂タグ
	 * @param Bonus ボーナス定義
	 */
	UFUNCTION(BlueprintCallable, Category = "セットボーナス")
	void RegisterSetBonus(const FGameplayTag& SoulTag, const FSoulSetBonus& Bonus);

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

	/** コンボ更新時のデリゲート */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComboUpdated, int32, ComboCount, int32, BonusSouls);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnComboUpdated OnComboUpdated;

	/** セットボーナス達成時のデリゲート */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetBonusAchieved, const FGameplayTag&, SoulTag, const FSoulSetBonus&, Bonus);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnSetBonusAchieved OnSetBonusAchieved;

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

	/** コンボ情報 */
	UPROPERTY()
	FComboKillInfo ComboInfo;

	/** セットボーナス定義（魂タグ→ボーナスリスト） - UPROPERTYは使用不可（TArrayがTMapの値のため） */
	TMap<FGameplayTag, TArray<FSoulSetBonus>> SetBonusDefinitions;

	/** 達成済みセットボーナス（再通知防止用） */
	UPROPERTY()
	TSet<FString> AchievedSetBonuses;

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** バフ効果を属性セットに適用 */
	void ApplyBuffEffect(UDawnlightAttributeSet* AttributeSet, const FSoulBuffEffect& Buff);

	/** バフ効果を属性セットから除去 */
	void RemoveBuffEffect(UDawnlightAttributeSet* AttributeSet, const FSoulBuffEffect& Buff);

	/** デフォルトのコンボ閾値を初期化 */
	void InitializeDefaultComboThresholds();

	/** デフォルトのセットボーナスを初期化 */
	void InitializeDefaultSetBonuses();

	/** セットボーナスの達成をチェック */
	void CheckSetBonusAchievement(const FGameplayTag& SoulTag, int32 NewCount);

	/** セットボーナスキーを生成（再通知防止用） */
	FString MakeSetBonusKey(const FGameplayTag& SoulTag, int32 RequiredCount) const;
};
