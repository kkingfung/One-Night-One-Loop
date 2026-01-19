// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReaperModeComponent.generated.h"

class UDawnlightAttributeSet;
class UAbilitySystemComponent;
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * リーパーモードコンポーネント
 *
 * プレイヤーのリーパーモード（必殺技）を管理
 * - ゲージが満タンでスペースキーで発動
 * - 発動中はダメージ2倍
 * - 一定時間後に終了
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DAWNLIGHT_API UReaperModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UReaperModeComponent();

	// ========================================================================
	// 発動
	// ========================================================================

	/** リーパーモードを発動 */
	UFUNCTION(BlueprintCallable, Category = "リーパーモード")
	bool ActivateReaperMode();

	/** リーパーモードを終了 */
	UFUNCTION(BlueprintCallable, Category = "リーパーモード")
	void DeactivateReaperMode();

	/** リーパーモードが発動可能かどうか */
	UFUNCTION(BlueprintPure, Category = "リーパーモード")
	bool CanActivateReaperMode() const;

	/** リーパーモードが発動中かどうか */
	UFUNCTION(BlueprintPure, Category = "リーパーモード")
	bool IsReaperModeActive() const { return bIsReaperModeActive; }

	// ========================================================================
	// ゲージ管理
	// ========================================================================

	/** リーパーゲージを追加 */
	UFUNCTION(BlueprintCallable, Category = "リーパーモード")
	void AddReaperGauge(float Amount);

	/** リーパーゲージの割合を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "リーパーモード")
	float GetReaperGaugePercent() const;

	/** リーパーゲージをリセット */
	UFUNCTION(BlueprintCallable, Category = "リーパーモード")
	void ResetReaperGauge();

	// ========================================================================
	// イベント
	// ========================================================================

	/** リーパーモード発動時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReaperModeActivated);
	UPROPERTY(BlueprintAssignable, Category = "リーパーモード|イベント")
	FOnReaperModeActivated OnReaperModeActivated;

	/** リーパーモード終了時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReaperModeDeactivated);
	UPROPERTY(BlueprintAssignable, Category = "リーパーモード|イベント")
	FOnReaperModeDeactivated OnReaperModeDeactivated;

	/** リーパーゲージ変更時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReaperGaugeChanged, float, NewValue, float, MaxValue);
	UPROPERTY(BlueprintAssignable, Category = "リーパーモード|イベント")
	FOnReaperGaugeChanged OnReaperGaugeChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ========================================================================
	// 設定
	// ========================================================================

	/** リーパーモードの持続時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "リーパーモード|設定", meta = (ClampMin = "1.0"))
	float ReaperModeDuration = 10.0f;

	/** リーパーモード中のダメージ倍率 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "リーパーモード|設定", meta = (ClampMin = "1.0"))
	float ReaperDamageMultiplier = 2.0f;

	/** リーパーモード中の移動速度倍率 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "リーパーモード|設定", meta = (ClampMin = "1.0"))
	float ReaperSpeedMultiplier = 1.3f;

	/** 発動時のNiagaraエフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "リーパーモード|エフェクト")
	TObjectPtr<UNiagaraSystem> ActivationEffect;

	/** 発動中の常時エフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "リーパーモード|エフェクト")
	TObjectPtr<UNiagaraSystem> ActiveEffect;

private:
	// ========================================================================
	// 状態
	// ========================================================================

	/** リーパーモード発動中かどうか */
	bool bIsReaperModeActive;

	/** 発動中のエフェクトコンポーネント */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveEffectComponent;

	/** 発動前のダメージ倍率（復元用） */
	float OriginalDamageMultiplier;

	/** 発動前の移動速度倍率（復元用） */
	float OriginalSpeedMultiplier;

	/** 持続時間タイマー */
	FTimerHandle DurationTimerHandle;

	// ========================================================================
	// キャッシュ
	// ========================================================================

	UPROPERTY()
	TWeakObjectPtr<UDawnlightAttributeSet> CachedAttributeSet;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** AttributeSetを取得 */
	UDawnlightAttributeSet* GetAttributeSet() const;

	/** AbilitySystemComponentを取得 */
	UAbilitySystemComponent* GetASC() const;

	/** リーパーモード終了処理 */
	void OnReaperModeDurationEnd();

	/** バフを適用 */
	void ApplyReaperBuffs();

	/** バフを解除 */
	void RemoveReaperBuffs();
};
