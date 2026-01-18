// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NightProgressSubsystem.generated.h"

/**
 * 夜の進行サブシステム
 *
 * 夜の時間進行を管理
 * - 残り時間のトラッキング
 * - フェーズ進行の通知
 * - 夜明け（強制終了）のトリガー
 */
UCLASS()
class DAWNLIGHT_API UNightProgressSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// ========================================================================
	// USubsystem インターフェース
	// ========================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// ========================================================================
	// 夜の開始/停止
	// ========================================================================

	/** 夜を開始 */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void StartNight(float Duration);

	/** 夜を停止 */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void StopNight();

	/** 一時停止 */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void PauseNight();

	/** 再開 */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void ResumeNight();

	// ========================================================================
	// 状態取得
	// ========================================================================

	/** 夜が進行中かどうか */
	UFUNCTION(BlueprintPure, Category = "夜進行|状態")
	bool IsNightActive() const { return bNightActive; }

	/** 一時停止中かどうか */
	UFUNCTION(BlueprintPure, Category = "夜進行|状態")
	bool IsNightPaused() const { return bNightPaused; }

	/** 残り時間を取得（秒） */
	UFUNCTION(BlueprintPure, Category = "夜進行|状態")
	float GetRemainingTime() const { return RemainingTime; }

	/** 経過時間を取得（秒） */
	UFUNCTION(BlueprintPure, Category = "夜進行|状態")
	float GetElapsedTime() const { return TotalDuration - RemainingTime; }

	/** 夜の進行度を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "夜進行|状態")
	float GetNightProgress() const;

	/** 残り時間をフォーマット済み文字列で取得 */
	UFUNCTION(BlueprintPure, Category = "夜進行|状態")
	FString GetFormattedRemainingTime() const;

	/** 夜明けが近いかどうか */
	UFUNCTION(BlueprintPure, Category = "夜進行|状態")
	bool IsDawnApproaching() const;

	/** 現在のフェーズを取得（0=導入, 1=緩和, 2=クライマックス） */
	UFUNCTION(BlueprintPure, Category = "夜進行|状態")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 夜開始時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightStarted);
	UPROPERTY(BlueprintAssignable, Category = "夜進行|イベント")
	FOnNightStarted OnNightStarted;

	/** 夜明け時（強制終了） */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDawnTriggered);
	UPROPERTY(BlueprintAssignable, Category = "夜進行|イベント")
	FOnDawnTriggered OnDawnTriggered;

	/** フェーズ変更時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhaseChanged, int32, OldPhase, int32, NewPhase);
	UPROPERTY(BlueprintAssignable, Category = "夜進行|イベント")
	FOnPhaseChanged OnPhaseChanged;

	/** 夜明けが近づいた時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDawnApproaching);
	UPROPERTY(BlueprintAssignable, Category = "夜進行|イベント")
	FOnDawnApproaching OnDawnApproaching;

protected:
	// ========================================================================
	// 設定
	// ========================================================================

	/** フェーズ移行の閾値（進行度 0-1） */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	TArray<float> PhaseThresholds;

	/** 夜明け警告の閾値（残り時間の割合） */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float DawnWarningThreshold;

private:
	// ========================================================================
	// 状態
	// ========================================================================

	/** 夜が進行中かどうか */
	bool bNightActive;

	/** 一時停止中かどうか */
	bool bNightPaused;

	/** 夜明け警告が発行されたかどうか */
	bool bDawnWarningIssued;

	/** 残り時間（秒） */
	float RemainingTime;

	/** 総時間（秒） */
	float TotalDuration;

	/** 現在のフェーズ */
	int32 CurrentPhase;

	/** フェーズ進行を確認 */
	void CheckPhaseProgression();
};
