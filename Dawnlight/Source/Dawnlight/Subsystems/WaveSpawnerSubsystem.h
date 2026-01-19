// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "WaveSpawnerSubsystem.generated.h"

class UEnemyDataAsset;
class AEnemyCharacter;

/**
 * ウェーブ設定
 */
USTRUCT(BlueprintType)
struct FWaveConfig
{
	GENERATED_BODY()

	/** ウェーブ番号 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ウェーブ")
	int32 WaveNumber = 1;

	/** このウェーブで出現する敵の総数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ウェーブ", meta = (ClampMin = "1"))
	int32 TotalEnemies = 5;

	/** 同時に存在できる敵の最大数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ウェーブ", meta = (ClampMin = "1"))
	int32 MaxConcurrentEnemies = 3;

	/** 敵のスポーン間隔（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ウェーブ", meta = (ClampMin = "0.1"))
	float SpawnInterval = 2.0f;

	/** 敵のHP倍率 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ウェーブ", meta = (ClampMin = "0.1"))
	float HealthMultiplier = 1.0f;

	/** 敵のダメージ倍率 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ウェーブ", meta = (ClampMin = "0.1"))
	float DamageMultiplier = 1.0f;

	/** 使用可能な敵データ（空の場合はデフォルトを使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ウェーブ")
	TArray<TObjectPtr<UEnemyDataAsset>> AvailableEnemies;
};

/**
 * ウェーブ状態
 */
UENUM(BlueprintType)
enum class EWaveState : uint8
{
	NotStarted		UMETA(DisplayName = "未開始"),
	InProgress		UMETA(DisplayName = "進行中"),
	Completed		UMETA(DisplayName = "完了"),
	Failed			UMETA(DisplayName = "失敗")
};

/**
 * ウェーブ開始/終了デリゲート
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveCompleted, int32, WaveNumber, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllWavesCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemySpawned, AEnemyCharacter*, Enemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyKilled, AEnemyCharacter*, Enemy);

/**
 * ウェーブスポーナーサブシステム
 *
 * Dawn Phaseでの敵ウェーブ管理
 * - ウェーブ設定に基づいて敵をスポーン
 * - 敵の生存数を追跡
 * - ウェーブクリア判定
 */
UCLASS()
class DAWNLIGHT_API UWaveSpawnerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// ========================================================================
	// UWorldSubsystem インターフェース
	// ========================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// ========================================================================
	// ウェーブ管理
	// ========================================================================

	/** ウェーブシステムを初期化 */
	UFUNCTION(BlueprintCallable, Category = "ウェーブ")
	void InitializeWaveSystem(const TArray<FWaveConfig>& InWaveConfigs);

	/** 最初のウェーブを開始 */
	UFUNCTION(BlueprintCallable, Category = "ウェーブ")
	void StartFirstWave();

	/** 次のウェーブを開始 */
	UFUNCTION(BlueprintCallable, Category = "ウェーブ")
	void StartNextWave();

	/** 現在のウェーブを強制終了 */
	UFUNCTION(BlueprintCallable, Category = "ウェーブ")
	void EndCurrentWave(bool bSuccess);

	/** 全てのウェーブを停止 */
	UFUNCTION(BlueprintCallable, Category = "ウェーブ")
	void StopAllWaves();

	// ========================================================================
	// スポーン設定
	// ========================================================================

	/** スポーンポイントを追加 */
	UFUNCTION(BlueprintCallable, Category = "ウェーブ|スポーン")
	void AddSpawnPoint(const FVector& Location);

	/** スポーンポイントをクリア */
	UFUNCTION(BlueprintCallable, Category = "ウェーブ|スポーン")
	void ClearSpawnPoints();

	/** デフォルト敵データを設定 */
	UFUNCTION(BlueprintCallable, Category = "ウェーブ|スポーン")
	void SetDefaultEnemyData(UEnemyDataAsset* EnemyData);

	// ========================================================================
	// 状態取得
	// ========================================================================

	/** 現在のウェーブ番号を取得 */
	UFUNCTION(BlueprintPure, Category = "ウェーブ")
	int32 GetCurrentWaveNumber() const { return CurrentWaveNumber; }

	/** 総ウェーブ数を取得 */
	UFUNCTION(BlueprintPure, Category = "ウェーブ")
	int32 GetTotalWaveCount() const { return WaveConfigs.Num(); }

	/** 現在のウェーブ状態を取得 */
	UFUNCTION(BlueprintPure, Category = "ウェーブ")
	EWaveState GetCurrentWaveState() const { return CurrentWaveState; }

	/** 現在のウェーブで残っている敵の数 */
	UFUNCTION(BlueprintPure, Category = "ウェーブ")
	int32 GetRemainingEnemiesInWave() const;

	/** 現在生存している敵の数 */
	UFUNCTION(BlueprintPure, Category = "ウェーブ")
	int32 GetAliveEnemyCount() const { return AliveEnemies.Num(); }

	/** 全ウェーブが完了したかどうか */
	UFUNCTION(BlueprintPure, Category = "ウェーブ")
	bool AreAllWavesCompleted() const;

	// ========================================================================
	// イベント
	// ========================================================================

	/** ウェーブ開始時 */
	UPROPERTY(BlueprintAssignable, Category = "ウェーブ|イベント")
	FOnWaveStarted OnWaveStarted;

	/** ウェーブ完了時 */
	UPROPERTY(BlueprintAssignable, Category = "ウェーブ|イベント")
	FOnWaveCompleted OnWaveCompleted;

	/** 全ウェーブ完了時 */
	UPROPERTY(BlueprintAssignable, Category = "ウェーブ|イベント")
	FOnAllWavesCompleted OnAllWavesCompleted;

	/** 敵スポーン時 */
	UPROPERTY(BlueprintAssignable, Category = "ウェーブ|イベント")
	FOnEnemySpawned OnEnemySpawned;

	/** 敵撃破時 */
	UPROPERTY(BlueprintAssignable, Category = "ウェーブ|イベント")
	FOnEnemyKilled OnEnemyKilled;

protected:
	// ========================================================================
	// 内部データ
	// ========================================================================

	/** ウェーブ設定リスト */
	UPROPERTY()
	TArray<FWaveConfig> WaveConfigs;

	/** 現在のウェーブ番号（1始まり） */
	int32 CurrentWaveNumber;

	/** 現在のウェーブ状態 */
	EWaveState CurrentWaveState;

	/** このウェーブでスポーンした敵の数 */
	int32 EnemiesSpawnedThisWave;

	/** スポーンポイント */
	TArray<FVector> SpawnPoints;

	/** 生存中の敵 */
	UPROPERTY()
	TArray<TWeakObjectPtr<AEnemyCharacter>> AliveEnemies;

	/** デフォルト敵データ */
	UPROPERTY()
	TObjectPtr<UEnemyDataAsset> DefaultEnemyData;

	/** スポーンタイマー */
	FTimerHandle SpawnTimerHandle;

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** 敵をスポーン */
	void SpawnEnemy();

	/** スポーン位置を取得（ランダム） */
	FVector GetRandomSpawnLocation() const;

	/** 敵が倒された時の処理 */
	UFUNCTION()
	void OnEnemyDied(AEnemyCharacter* Enemy);

	/** ウェーブクリア判定 */
	void CheckWaveCompletion();

	/** 現在のウェーブ設定を取得 */
	const FWaveConfig* GetCurrentWaveConfig() const;

	/** 敵データを選択 */
	UEnemyDataAsset* SelectEnemyData() const;
};
