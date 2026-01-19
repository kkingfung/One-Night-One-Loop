// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "AnimalCharacter.generated.h"

class USoulDataAsset;
class UNiagaraSystem;

/**
 * 動物の行動状態
 */
UENUM(BlueprintType)
enum class EAnimalBehaviorState : uint8
{
	Idle		UMETA(DisplayName = "待機"),
	Wandering	UMETA(DisplayName = "徘徊"),
	Fleeing		UMETA(DisplayName = "逃走"),
	Stunned		UMETA(DisplayName = "スタン"),
	Dead		UMETA(DisplayName = "死亡")
};

/**
 * 動物キャラクターの基底クラス
 *
 * Night Phaseで狩猟対象となる動物
 * - 徘徊AIで自然な動き
 * - プレイヤーが近づくと逃げる
 * - 倒されると魂をドロップ
 */
UCLASS(Blueprintable)
class DAWNLIGHT_API AAnimalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAnimalCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// ========================================================================
	// 設定
	// ========================================================================

	/** この動物のソウルデータ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|設定")
	TObjectPtr<USoulDataAsset> SoulData;

	/** 現在のHP */
	UPROPERTY(BlueprintReadOnly, Category = "動物|ステータス")
	float CurrentHealth;

	/** 現在の行動状態 */
	UPROPERTY(BlueprintReadOnly, Category = "動物|ステータス")
	EAnimalBehaviorState BehaviorState;

	// ========================================================================
	// AI設定
	// ========================================================================

	/** 徘徊時の移動速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|AI", meta = (ClampMin = "0.0"))
	float WanderSpeed = 100.0f;

	/** 逃走時の移動速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|AI", meta = (ClampMin = "0.0"))
	float FleeSpeed = 400.0f;

	/** プレイヤーを検知する距離 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|AI", meta = (ClampMin = "0.0"))
	float DetectionRadius = 600.0f;

	/** 逃走を開始する距離 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|AI", meta = (ClampMin = "0.0"))
	float FleeRadius = 400.0f;

	/** 徘徊の目的地変更間隔（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|AI", meta = (ClampMin = "0.1"))
	float WanderInterval = 3.0f;

	/** 徘徊範囲（スポーン地点からの半径） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|AI", meta = (ClampMin = "0.0"))
	float WanderRadius = 500.0f;

	// ========================================================================
	// エフェクト
	// ========================================================================

	/** 死亡時のNiagaraエフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|エフェクト")
	TObjectPtr<UNiagaraSystem> DeathEffect;

	/** 魂放出エフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物|エフェクト")
	TObjectPtr<UNiagaraSystem> SoulReleaseEffect;

	// ========================================================================
	// 公開関数
	// ========================================================================

	/** ダメージを受ける */
	UFUNCTION(BlueprintCallable, Category = "動物")
	void TakeDamageFromPlayer(float DamageAmount, AActor* DamageCauser);

	/** 死亡処理 */
	UFUNCTION(BlueprintCallable, Category = "動物")
	void Die();

	/** HPの割合を取得 */
	UFUNCTION(BlueprintPure, Category = "動物")
	float GetHealthPercent() const;

	/** 生存中かどうか */
	UFUNCTION(BlueprintPure, Category = "動物")
	bool IsAlive() const { return BehaviorState != EAnimalBehaviorState::Dead; }

	// ========================================================================
	// イベント
	// ========================================================================

	/** 死亡時のイベント（Blueprint実装可能） */
	UFUNCTION(BlueprintImplementableEvent, Category = "動物|イベント")
	void OnDeath();

	/** ダメージ受けた時のイベント（Blueprint実装可能） */
	UFUNCTION(BlueprintImplementableEvent, Category = "動物|イベント")
	void OnDamageTaken(float DamageAmount, float RemainingHealth);

	/** 逃走開始時のイベント（Blueprint実装可能） */
	UFUNCTION(BlueprintImplementableEvent, Category = "動物|イベント")
	void OnStartFleeing();

protected:
	// ========================================================================
	// 内部処理
	// ========================================================================

	/** スポーン位置 */
	FVector SpawnLocation;

	/** 現在の徘徊目的地 */
	FVector CurrentWanderTarget;

	/** 徘徊タイマー */
	FTimerHandle WanderTimerHandle;

	/** プレイヤーへの参照（キャッシュ） */
	UPROPERTY()
	TWeakObjectPtr<AActor> CachedPlayer;

	/** 行動状態を更新 */
	void UpdateBehaviorState();

	/** 徘徊処理 */
	void ProcessWandering(float DeltaTime);

	/** 逃走処理 */
	void ProcessFleeing(float DeltaTime);

	/** 新しい徘徊目的地を設定 */
	void SetNewWanderTarget();

	/** プレイヤーとの距離を取得 */
	float GetDistanceToPlayer() const;

	/** プレイヤーから離れる方向を取得 */
	FVector GetFleeDirection() const;

	/** ソウルをドロップ */
	void DropSoul();

	/** SoulDataからパラメータを初期化 */
	void InitializeFromSoulData();
};
