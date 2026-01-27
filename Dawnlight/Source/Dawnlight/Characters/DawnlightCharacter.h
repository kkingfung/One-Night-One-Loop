// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "DawnlightCharacter.generated.h"

class UAbilitySystemComponent;
class UDawnlightAttributeSet;
class USpringArmComponent;
class UCameraComponent;
class UReaperModeComponent;

/**
 * Soul Reaper プレイヤーキャラクター
 *
 * トップダウン視点のアクションゲーム主人公（死神）
 * - GAS（Gameplay Ability System）対応
 * - Night Phase: 動物を狩って魂を収集
 * - Dawn Phase: 敵と戦闘、リーパーモード発動
 */
UCLASS()
class DAWNLIGHT_API ADawnlightCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADawnlightCharacter();

	// ========================================================================
	// IAbilitySystemInterface
	// ========================================================================

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** 属性セットを取得 */
	UFUNCTION(BlueprintPure, Category = "アビリティ")
	UDawnlightAttributeSet* GetDawnlightAttributeSet() const { return AttributeSet; }

	// ========================================================================
	// 移動
	// ========================================================================

	/** 移動入力を処理 */
	UFUNCTION(BlueprintCallable, Category = "移動")
	void HandleMoveInput(const FVector2D& MovementVector);

	/** 現在の移動速度を取得 */
	UFUNCTION(BlueprintPure, Category = "移動")
	float GetCurrentMoveSpeed() const;

	// ========================================================================
	// 戦闘
	// ========================================================================

	/** 通常攻撃（左クリック） */
	UFUNCTION(BlueprintCallable, Category = "戦闘")
	void PerformLightAttack();

	/** 強攻撃（右クリック） */
	UFUNCTION(BlueprintCallable, Category = "戦闘")
	void PerformHeavyAttack();

	/** 特殊攻撃（Q） */
	UFUNCTION(BlueprintCallable, Category = "戦闘")
	void PerformSpecialAttack();

	/** 攻撃中かどうか */
	UFUNCTION(BlueprintPure, Category = "戦闘")
	bool IsAttacking() const;

	// ========================================================================
	// リーパーモード
	// ========================================================================

	/** リーパーモードを発動 */
	UFUNCTION(BlueprintCallable, Category = "リーパー")
	void ActivateReaperMode();

	/** リーパーモード中かどうか */
	UFUNCTION(BlueprintPure, Category = "リーパー")
	bool IsInReaperMode() const;

	/** リーパーゲージの割合を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "リーパー")
	float GetReaperGaugePercent() const;

	/** リーパーモードが発動可能かどうか */
	UFUNCTION(BlueprintPure, Category = "リーパー")
	bool CanActivateReaperMode() const;

	/** リーパーゲージを追加 */
	UFUNCTION(BlueprintCallable, Category = "リーパー")
	void AddReaperGauge(float Amount);

	// ========================================================================
	// ダメージ
	// ========================================================================

	/** ダメージを受ける */
	UFUNCTION(BlueprintCallable, Category = "ダメージ")
	void TakeDamageAmount(float DamageAmount);

	/** 死亡しているかどうか */
	UFUNCTION(BlueprintPure, Category = "ダメージ")
	bool IsDead() const;

	/** 現在のHP割合を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "ダメージ")
	float GetHealthPercent() const;

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 死亡時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnPlayerDeath OnPlayerDeath;

	/** リーパーモード発動時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReaperModeActivated);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnReaperModeActivated OnReaperModeActivated;

	/** リーパーモード終了時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReaperModeDeactivated);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnReaperModeDeactivated OnReaperModeDeactivated;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

	// ========================================================================
	// コンポーネント
	// ========================================================================

	/** カメラブーム */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "カメラ", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** トップダウンカメラ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "カメラ", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCamera;

	/** アビリティシステムコンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "アビリティ", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/** 属性セット */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "アビリティ", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDawnlightAttributeSet> AttributeSet;

	/** リーパーモードコンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "リーパー", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UReaperModeComponent> ReaperModeComponent;

	// ========================================================================
	// 移動設定
	// ========================================================================

	/** 通常移動速度 */
	UPROPERTY(EditDefaultsOnly, Category = "移動")
	float NormalMoveSpeed;

	/** リーパーモード中の移動速度倍率 */
	UPROPERTY(EditDefaultsOnly, Category = "移動")
	float ReaperModeSpeedMultiplier;

	// ========================================================================
	// 戦闘設定
	// ========================================================================

	/** 基本攻撃力 */
	UPROPERTY(EditDefaultsOnly, Category = "戦闘")
	float BaseDamage;

	/** 強攻撃の倍率 */
	UPROPERTY(EditDefaultsOnly, Category = "戦闘")
	float HeavyAttackMultiplier;

	/** 特殊攻撃の倍率 */
	UPROPERTY(EditDefaultsOnly, Category = "戦闘")
	float SpecialAttackMultiplier;

	/** 通常攻撃モンタージュ */
	UPROPERTY(EditDefaultsOnly, Category = "戦闘|アニメーション")
	TObjectPtr<UAnimMontage> LightAttackMontage;

	/** 強攻撃モンタージュ */
	UPROPERTY(EditDefaultsOnly, Category = "戦闘|アニメーション")
	TObjectPtr<UAnimMontage> HeavyAttackMontage;

	/** 特殊攻撃モンタージュ */
	UPROPERTY(EditDefaultsOnly, Category = "戦闘|アニメーション")
	TObjectPtr<UAnimMontage> SpecialAttackMontage;

	/** リーパーモード発動モンタージュ（Roar） */
	UPROPERTY(EditDefaultsOnly, Category = "リーパー|アニメーション")
	TObjectPtr<UAnimMontage> ReaperActivationMontage;

	// ========================================================================
	// カメラ設定
	// ========================================================================

	/** カメラの高さ（SpringArmの長さ） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "カメラ", meta = (ClampMin = "200.0", ClampMax = "3000.0"))
	float CameraHeight;

	/** カメラの角度（ピッチ：-90で真上、-45で斜め） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "カメラ", meta = (ClampMin = "-90.0", ClampMax = "-30.0"))
	float CameraPitch;

	/** カメラのラグを有効にするか（滑らかな追従） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "カメラ")
	bool bEnableCameraLag;

	/** カメララグの速度（大きいほど素早く追従） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "カメラ", meta = (ClampMin = "0.0", ClampMax = "30.0", EditCondition = "bEnableCameraLag"))
	float CameraLagSpeed;

private:
	// ========================================================================
	// 内部状態
	// ========================================================================

	/** 攻撃中 */
	bool bIsAttacking;

	/** 死亡状態 */
	bool bIsDead;

	/** 現在のHP */
	float CurrentHealth;

	/** 最大HP */
	float MaxHealth;

	// ========================================================================
	// タイマーハンドル
	// ========================================================================

	/** 攻撃終了タイマー */
	FTimerHandle AttackEndTimerHandle;

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** GASを初期化 */
	void InitializeAbilitySystem();

	/** 現在のGameplayTagsを取得 */
	FGameplayTagContainer GetCurrentTags() const;

	/** リーパーモードコンポーネントのイベントに接続 */
	void BindReaperModeEvents();

	/** 死亡処理 */
	void HandleDeath();

	/** 攻撃終了処理（タイマーコールバック用） */
	void EndAttack();

	// ========================================================================
	// リーパーモードイベントコールバック
	// ========================================================================

	/** リーパーモード発動時のコールバック（コンポーネントからの転送用） */
	UFUNCTION()
	void OnReaperModeActivatedCallback();

	/** リーパーモード終了時のコールバック（コンポーネントからの転送用） */
	UFUNCTION()
	void OnReaperModeDeactivatedCallback();
};
