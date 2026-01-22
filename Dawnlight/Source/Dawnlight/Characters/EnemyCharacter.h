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
	// ボス設定
	// ========================================================================

	/** ボスかどうか */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|ボス")
	bool bIsBoss = false;

	/** ボスの現在フェーズ（1から開始） */
	UPROPERTY(BlueprintReadOnly, Category = "敵|ボス")
	int32 CurrentBossPhase = 1;

	/** ボスの最大フェーズ数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|ボス", meta = (ClampMin = "1", EditCondition = "bIsBoss"))
	int32 MaxBossPhases = 3;

	/** フェーズ移行するHP閾値（パーセント、0.0-1.0） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|ボス", meta = (EditCondition = "bIsBoss"))
	TArray<float> PhaseHealthThresholds;

	/** ボスの特殊攻撃クールダウン（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|ボス", meta = (ClampMin = "1.0", EditCondition = "bIsBoss"))
	float SpecialAttackCooldown = 10.0f;

	/** ボスの特殊攻撃ダメージ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|ボス", meta = (ClampMin = "0.0", EditCondition = "bIsBoss"))
	float SpecialAttackDamage = 50.0f;

	/** ボスの範囲攻撃半径 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "敵|ボス", meta = (ClampMin = "0.0", EditCondition = "bIsBoss"))
	float AreaAttackRadius = 300.0f;

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

	/** ボスフェーズ変更時のイベント（Blueprint実装可能） */
	UFUNCTION(BlueprintImplementableEvent, Category = "敵|イベント")
	void OnBossPhaseChanged(int32 NewPhase);

	/** ボス特殊攻撃時のイベント（Blueprint実装可能） */
	UFUNCTION(BlueprintImplementableEvent, Category = "敵|イベント")
	void OnBossSpecialAttack();

	/** 死亡時のC++デリゲート（WaveSpawner等で使用） */
	UPROPERTY(BlueprintAssignable, Category = "敵|イベント")
	FOnEnemyDeathDelegate OnEnemyDeathDelegate;

	// ========================================================================
	// ボス専用関数
	// ========================================================================

	/** ボスの特殊攻撃を実行 */
	UFUNCTION(BlueprintCallable, Category = "敵|ボス")
	void PerformBossSpecialAttack();

	/** 範囲攻撃を実行 */
	UFUNCTION(BlueprintCallable, Category = "敵|ボス")
	void PerformAreaAttack(FVector CenterLocation, float Radius, float Damage);

	/** ボスフェーズをチェックして更新 */
	UFUNCTION(BlueprintCallable, Category = "敵|ボス")
	void CheckBossPhaseTransition();

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

	// ========================================================================
	// ボス内部処理
	// ========================================================================

	/** ボス特殊攻撃のクールダウン中かどうか */
	bool bIsSpecialAttackOnCooldown;

	/** ボス特殊攻撃タイマー */
	FTimerHandle SpecialAttackCooldownTimerHandle;

	/** ボス特殊攻撃クールダウン終了 */
	void OnSpecialAttackCooldownEnd();

	/** ボス処理を実行（Tick内で呼び出し） */
	void ProcessBossLogic(float DeltaTime);

	/** デフォルトのフェーズ閾値を初期化 */
	void InitializeDefaultPhaseThresholds();
};
