// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NightProgressSubsystem.generated.h"

/**
 * Night進行サブシステム
 *
 * Night Phase中の時間進行と状態を管理する補助サブシステム
 * メインのフェーズ管理はADawnlightGameModeが行う
 *
 * 機能:
 * - 時間ベースのイベント発火
 * - 夜明け警告タイミング
 * - フェーズ内の緊張度管理
 */
UCLASS()
class DAWNLIGHT_API UNightProgressSubsystem : public UWorldSubsystem
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
	// 時間情報
	// ========================================================================

	/** Night Phaseの残り時間を設定（GameModeから呼び出し） */
	UFUNCTION(BlueprintCallable, Category = "Night進行")
	void SetRemainingTime(float Time);

	/** Night Phaseの残り時間を取得 */
	UFUNCTION(BlueprintPure, Category = "Night進行")
	float GetRemainingTime() const { return RemainingTime; }

	/** Night Phaseの進行率を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "Night進行")
	float GetNightProgress() const;

	/** 夜明けが近いかどうか（残り30秒以下） */
	UFUNCTION(BlueprintPure, Category = "Night進行")
	bool IsDawnApproaching() const;

	// ========================================================================
	// 緊張度
	// ========================================================================

	/** 現在の緊張度を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "Night進行")
	float GetTensionLevel() const { return TensionLevel; }

	/** 緊張度を設定 */
	UFUNCTION(BlueprintCallable, Category = "Night進行")
	void SetTensionLevel(float Level);

	/** 緊張度を加算 */
	UFUNCTION(BlueprintCallable, Category = "Night進行")
	void AddTension(float Amount);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 夜明けが近づいた時（残り30秒） */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDawnApproaching);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnDawnApproaching OnDawnApproaching;

	/** 緊張度が閾値を超えた時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTensionThresholdReached, float, TensionLevel);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnTensionThresholdReached OnTensionThresholdReached;

private:
	/** 残り時間 */
	float RemainingTime;

	/** 総Night Phase時間 */
	float TotalNightDuration;

	/** 現在の緊張度 */
	float TensionLevel;

	/** 夜明け警告を発行済みか */
	bool bDawnWarningIssued;
};
