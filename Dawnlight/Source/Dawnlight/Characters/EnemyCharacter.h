// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "EnemyCharacter.generated.h"

class UEnemyDataAsset;
class UNiagaraSystem;
class AEnemyCharacter;

/** 敵死亡時のデリゲート */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeathDelegate, AEnemyCharacter*, DeadEnemy);

/**
 * 敵の行動状態
 */
UENUM(BlueprintType)
enum class EEnemyBehaviorState : uint8
{
	Idle		UMETA(DisplayName = "待機"),
	Chasing		UMETA(DisplayName = "追跡"),
	Attacking	UMETA(DisplayName = "攻撃"),
	Stunned		UMETA(DisplayName = "スタン"),
	Dead		UMETA(DisplayName = "死亡")
};

/**
 * 敵キャラクターの基底クラス
 *
 * Dawn Phaseで出現する敵
 * - プレイヤーを追跡
 * - 近接攻撃を行う
 * - 倒されると経験値/アイテムをドロップ
 */
UCLASS(Blueprintable)
class DAWNLIGHT_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// ========================================================================
	// 設定
	// ========================================================================

	/** この敵のデータアセット */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|設定")
	TObjectPtr<UEnemyDataAsset> EnemyData;

	/** 現在のHP */
	UPROPERTY(BlueprintReadOnly, Category = "敵|ステータス")
	float CurrentHealth;

	/** 最大HP */
	UPROPERTY(BlueprintReadOnly, Category = "敵|ステータス")
	float MaxHealth;

	/** 現在の行動状態 */
	UPROPERTY(BlueprintReadOnly, Category = "敵|ステータス")
	EEnemyBehaviorState BehaviorState;

	// ========================================================================
	// AI設定
	// ========================================================================

	/** 追跡時の移動速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|AI", meta = (ClampMin = "0.0"))
	float ChaseSpeed = 300.0f;

	/** プレイヤーを検知する距離 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|AI", meta = (ClampMin = "0.0"))
	float DetectionRadius = 1000.0f;

	/** 攻撃を開始する距離 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|AI", meta = (ClampMin = "0.0"))
	float AttackRange = 150.0f;

	/** 攻撃のクールダウン（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|AI", meta = (ClampMin = "0.1"))
	float AttackCooldown = 1.5f;

	/** 攻撃ダメージ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|AI", meta = (ClampMin = "0.0"))
	float AttackDamage = 10.0f;

	// ========================================================================
	// エフェクト
	// ========================================================================

	/** 死亡時のNiagaraエフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|エフェクト")
	TObjectPtr<UNiagaraSystem> DeathEffect;

	/** ヒット時のエフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|エフェクト")
	TObjectPtr<UNiagaraSystem> HitEffect;

	// ========================================================================
	// 公開関数
	// ========================================================================

	/** ダメージを受ける */
	UFUNCTION(BlueprintCallable, Category = "敵")
	void TakeDamageFromPlayer(float DamageAmount, AActor* DamageCauser);

	/** 死亡処理 */
	UFUNCTION(BlueprintCallable, Category = "敵")
	void Die();

	/** HPの割合を取得 */
	UFUNCTION(BlueprintPure, Category = "敵")
	float GetHealthPercent() const;

	/** 生存中かどうか */
	UFUNCTION(BlueprintPure, Category = "敵")
	bool IsAlive() const { return BehaviorState != EEnemyBehaviorState::Dead; }

	/** 攻撃可能かどうか */
	UFUNCTION(BlueprintPure, Category = "敵")
	bool CanAttack() const;

	/** 攻撃を実行 */
	UFUNCTION(BlueprintCallable, Category = "敵")
	void PerformAttack();

	// ========================================================================
	// イベント
	// ========================================================================

	/** 死亡時のイベント（Blueprint実装可能） */
	UFUNCTION(BlueprintImplementableEvent, Category = "敵|イベント")
	void OnDeath();

	/** ダメージ受けた時のイベント（Blueprint実装可能） */
	UFUNCTION(BlueprintImplementableEvent, Category = "敵|イベント")
	void OnDamageTaken(float DamageAmount, float RemainingHealth);

	/** 攻撃時のイベント（Blueprint実装可能） */
	UFUNCTION(BlueprintImplementableEvent, Category = "敵|イベント")
	void OnAttack();

	/** 死亡時のC++デリゲート（WaveSpawner等で使用） */
	UPROPERTY(BlueprintAssignable, Category = "敵|イベント")
	FOnEnemyDeathDelegate OnEnemyDeathDelegate;

protected:
	// ========================================================================
	// 内部処理
	// ========================================================================

	/** プレイヤーへの参照（キャッシュ） */
	UPROPERTY()
	TWeakObjectPtr<AActor> CachedPlayer;

	/** 攻撃クールダウン中かどうか */
	bool bIsAttackOnCooldown;

	/** 攻撃タイマー */
	FTimerHandle AttackCooldownTimerHandle;

	/** 行動状態を更新 */
	void UpdateBehaviorState();

	/** 追跡処理 */
	void ProcessChasing(float DeltaTime);

	/** 攻撃処理 */
	void ProcessAttacking(float DeltaTime);

	/** プレイヤーとの距離を取得 */
	float GetDistanceToPlayer() const;

	/** プレイヤーへの方向を取得 */
	FVector GetDirectionToPlayer() const;

	/** 攻撃クールダウン終了 */
	void OnAttackCooldownEnd();

	/** EnemyDataからパラメータを初期化 */
	void InitializeFromEnemyData();
};
