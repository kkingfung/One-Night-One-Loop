// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "AnimalSpawnerSubsystem.generated.h"

class USoulDataAsset;
class AAnimalCharacter;

/**
 * 動物スポーン設定
 */
USTRUCT(BlueprintType)
struct FAnimalSpawnConfig
{
	GENERATED_BODY()

	/** この動物のソウルデータ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動物")
	TObjectPtr<USoulDataAsset> SoulData;

	/** スポーンする数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動物", meta = (ClampMin = "1"))
	int32 SpawnCount = 3;

	/** カスタムBlueprintクラス（未設定の場合はSoulDataから取得） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動物")
	TSubclassOf<AAnimalCharacter> CustomAnimalClass;
};

/**
 * 動物スポーンデリゲート
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimalSpawned, AAnimalCharacter*, Animal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimalKilled, AAnimalCharacter*, Animal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllAnimalsKilled);

/**
 * 動物スポーナーサブシステム
 *
 * Night Phaseでの動物スポーン管理
 * - 設定に基づいて動物をスポーン
 * - 動物の生存数を追跡
 * - 動物が全滅したかの判定
 */
UCLASS()
class DAWNLIGHT_API UAnimalSpawnerSubsystem : public UWorldSubsystem
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
	// スポーン管理
	// ========================================================================

	/** 動物スポーンシステムを初期化 */
	UFUNCTION(BlueprintCallable, Category = "動物スポーン")
	void InitializeAnimalSpawner(const TArray<FAnimalSpawnConfig>& InSpawnConfigs);

	/** 全ての動物をスポーン */
	UFUNCTION(BlueprintCallable, Category = "動物スポーン")
	void SpawnAllAnimals();

	/** 指定された動物をスポーン */
	UFUNCTION(BlueprintCallable, Category = "動物スポーン")
	AAnimalCharacter* SpawnAnimal(USoulDataAsset* SoulData, const FVector& Location);

	/** 全ての動物を削除 */
	UFUNCTION(BlueprintCallable, Category = "動物スポーン")
	void DespawnAllAnimals();

	/**
	 * ランダムな動物を1体スポーン
	 * @return スポーン成功時 true
	 * @note SpawnConfigsが設定されている必要があります
	 */
	UFUNCTION(BlueprintCallable, Category = "動物スポーン")
	bool SpawnRandomAnimal();

	// ========================================================================
	// スポーンポイント
	// ========================================================================

	/** スポーンポイントを追加 */
	UFUNCTION(BlueprintCallable, Category = "動物スポーン|スポーンポイント")
	void AddSpawnPoint(const FVector& Location);

	/** スポーンエリアを設定（中心と半径） */
	UFUNCTION(BlueprintCallable, Category = "動物スポーン|スポーンポイント")
	void SetSpawnArea(const FVector& Center, float Radius);

	/** スポーンポイントをクリア */
	UFUNCTION(BlueprintCallable, Category = "動物スポーン|スポーンポイント")
	void ClearSpawnPoints();

	// ========================================================================
	// 状態取得
	// ========================================================================

	/** 生存中の動物数を取得 */
	UFUNCTION(BlueprintPure, Category = "動物スポーン")
	int32 GetAliveAnimalCount() const;

	/** 総スポーン数を取得 */
	UFUNCTION(BlueprintPure, Category = "動物スポーン")
	int32 GetTotalSpawnedCount() const { return TotalSpawnedCount; }

	/** 撃破された動物数を取得 */
	UFUNCTION(BlueprintPure, Category = "動物スポーン")
	int32 GetKilledAnimalCount() const { return KilledAnimalCount; }

	/** 全ての動物が倒されたか */
	UFUNCTION(BlueprintPure, Category = "動物スポーン")
	bool AreAllAnimalsKilled() const;

	/** 生存中の動物リストを取得 */
	UFUNCTION(BlueprintPure, Category = "動物スポーン")
	TArray<AAnimalCharacter*> GetAliveAnimals() const;

	// ========================================================================
	// イベント
	// ========================================================================

	/** 動物スポーン時 */
	UPROPERTY(BlueprintAssignable, Category = "動物スポーン|イベント")
	FOnAnimalSpawned OnAnimalSpawned;

	/** 動物撃破時 */
	UPROPERTY(BlueprintAssignable, Category = "動物スポーン|イベント")
	FOnAnimalKilled OnAnimalKilled;

	/** 全動物撃破時 */
	UPROPERTY(BlueprintAssignable, Category = "動物スポーン|イベント")
	FOnAllAnimalsKilled OnAllAnimalsKilled;

protected:
	// ========================================================================
	// 内部データ
	// ========================================================================

	/** スポーン設定リスト */
	UPROPERTY()
	TArray<FAnimalSpawnConfig> SpawnConfigs;

	/** スポーンポイント */
	TArray<FVector> SpawnPoints;

	/** スポーンエリア中心 */
	FVector SpawnAreaCenter;

	/** スポーンエリア半径 */
	float SpawnAreaRadius;

	/** スポーンエリアを使用するか */
	bool bUseSpawnArea;

	/** 生存中の動物 */
	UPROPERTY()
	TArray<TWeakObjectPtr<AAnimalCharacter>> AliveAnimals;

	/** 総スポーン数 */
	int32 TotalSpawnedCount;

	/** 撃破数 */
	int32 KilledAnimalCount;

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** スポーン位置を取得（ランダム） */
	FVector GetRandomSpawnLocation() const;

	/** 動物が倒された時の処理 */
	void OnAnimalDied(AAnimalCharacter* Animal);

	/** 無効な参照をクリーンアップ */
	void CleanupInvalidReferences();
};
