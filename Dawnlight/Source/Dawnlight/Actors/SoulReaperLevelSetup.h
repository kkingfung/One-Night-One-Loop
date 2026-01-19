// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/AnimalSpawnerSubsystem.h"
#include "Subsystems/WaveSpawnerSubsystem.h"
#include "SoulReaperLevelSetup.generated.h"

class USoulDataAsset;
class UEnemyDataAsset;
class UBoxComponent;

/**
 * Soul Reaperレベルセットアップアクター
 *
 * テストレベルやゲームレベルに配置して、
 * 動物スポーンとウェーブ設定を管理するアクター
 *
 * 配置するだけでNight/Dawn Phaseのセットアップが完了する
 */
UCLASS(Blueprintable)
class DAWNLIGHT_API ASoulReaperLevelSetup : public AActor
{
	GENERATED_BODY()

public:
	ASoulReaperLevelSetup();

protected:
	virtual void BeginPlay() override;

	// ========================================================================
	// Night Phase設定（動物スポーン）
	// ========================================================================

	/** 動物スポーン設定リスト */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Night Phase|動物")
	TArray<FAnimalSpawnConfig> AnimalSpawnConfigs;

	/** 動物スポーンエリアの中心オフセット */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Night Phase|動物")
	FVector AnimalSpawnAreaOffset = FVector::ZeroVector;

	/** 動物スポーンエリアの半径 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Night Phase|動物", meta = (ClampMin = "100.0"))
	float AnimalSpawnAreaRadius = 2000.0f;

	/** Night Phase継続時間（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Night Phase|時間", meta = (ClampMin = "30.0"))
	float NightPhaseDuration = 180.0f;

	// ========================================================================
	// Dawn Phase設定（ウェーブ）
	// ========================================================================

	/** ウェーブ設定リスト */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dawn Phase|ウェーブ")
	TArray<FWaveConfig> WaveConfigs;

	/** デフォルト敵データ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dawn Phase|敵")
	TObjectPtr<UEnemyDataAsset> DefaultEnemyData;

	/** 敵スポーンポイント（複数配置可能） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dawn Phase|敵")
	TArray<FVector> EnemySpawnPoints;

	/** 敵スポーンエリアの半径（スポーンポイントがない場合に使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dawn Phase|敵", meta = (ClampMin = "100.0"))
	float EnemySpawnAreaRadius = 1000.0f;

	// ========================================================================
	// ビジュアライゼーション
	// ========================================================================

	/** エディタでスポーンエリアを表示 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "デバッグ")
	bool bShowSpawnAreas = true;

	/** 動物スポーンエリアの色 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "デバッグ")
	FColor AnimalAreaColor = FColor::Green;

	/** 敵スポーンエリアの色 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "デバッグ")
	FColor EnemyAreaColor = FColor::Red;

	// ========================================================================
	// デフォルト設定
	// ========================================================================

	/** デフォルトのウェーブ設定を使用するか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "設定")
	bool bUseDefaultWaveConfigs = true;

	/** ゲーム開始時に自動セットアップするか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "設定")
	bool bAutoSetupOnBeginPlay = true;

public:
	// ========================================================================
	// 公開関数
	// ========================================================================

	/** サブシステムをセットアップ */
	UFUNCTION(BlueprintCallable, Category = "セットアップ")
	void SetupSubsystems();

	/** Night Phaseを開始 */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void StartNightPhase();

	/** Dawn Phaseを開始 */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void StartDawnPhase();

	/** デフォルトのウェーブ設定を生成 */
	UFUNCTION(BlueprintCallable, Category = "設定")
	void GenerateDefaultWaveConfigs();

protected:
	/** デバッグ描画 */
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	/** サブシステム参照 */
	UPROPERTY()
	TWeakObjectPtr<UAnimalSpawnerSubsystem> AnimalSpawner;

	UPROPERTY()
	TWeakObjectPtr<UWaveSpawnerSubsystem> WaveSpawner;

	/** Night Phase終了時の処理 */
	void OnNightPhaseEnd();
};
