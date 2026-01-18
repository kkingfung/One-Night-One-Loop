// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurveillanceDetectorComponent.generated.h"

class USurveillanceSubsystem;

/**
 * 監視検知コンポーネント
 *
 * アクターに付与して光の検知機能を提供
 * - 光の中にいるかの判定
 * - 検知ゲージの管理
 * - 検知状態の通知
 */
UCLASS(ClassGroup=(Dawnlight), meta=(BlueprintSpawnableComponent))
class DAWNLIGHT_API USurveillanceDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurveillanceDetectorComponent();

	// ========================================================================
	// 検知状態
	// ========================================================================

	/** 光の中にいるかどうか */
	UFUNCTION(BlueprintPure, Category = "監視|検知")
	bool IsInLight() const { return bIsInLight; }

	/** 現在の検知レベル（0-1） */
	UFUNCTION(BlueprintPure, Category = "監視|検知")
	float GetDetectionLevel() const { return DetectionLevel; }

	/** 検知されているかどうか（閾値以上） */
	UFUNCTION(BlueprintPure, Category = "監視|検知")
	bool IsDetected() const { return DetectionLevel >= DetectionThreshold; }

	/** 現在位置の光の強度を取得 */
	UFUNCTION(BlueprintPure, Category = "監視|検知")
	float GetCurrentLightIntensity() const { return CurrentLightIntensity; }

	// ========================================================================
	// 検知レベル操作
	// ========================================================================

	/** 検知レベルをリセット */
	UFUNCTION(BlueprintCallable, Category = "監視|検知")
	void ResetDetectionLevel();

	/** 検知レベルを強制的に設定 */
	UFUNCTION(BlueprintCallable, Category = "監視|検知")
	void SetDetectionLevel(float NewLevel);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 光に入った時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnteredLight);
	UPROPERTY(BlueprintAssignable, Category = "監視|イベント")
	FOnEnteredLight OnEnteredLight;

	/** 光から出た時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitedLight);
	UPROPERTY(BlueprintAssignable, Category = "監視|イベント")
	FOnExitedLight OnExitedLight;

	/** 検知された時（閾値超え） */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDetected);
	UPROPERTY(BlueprintAssignable, Category = "監視|イベント")
	FOnDetected OnDetected;

	/** 検知状態が解除された時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDetectionCleared);
	UPROPERTY(BlueprintAssignable, Category = "監視|イベント")
	FOnDetectionCleared OnDetectionCleared;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 検知レベルの上昇速度（/秒） */
	UPROPERTY(EditDefaultsOnly, Category = "監視|設定")
	float DetectionIncreaseRate;

	/** 検知レベルの減衰速度（/秒） */
	UPROPERTY(EditDefaultsOnly, Category = "監視|設定")
	float DetectionDecayRate;

	/** 検知閾値（この値以上で検知状態） */
	UPROPERTY(EditDefaultsOnly, Category = "監視|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DetectionThreshold;

	/** 検知判定のオフセット（アクター中心からのズレ） */
	UPROPERTY(EditDefaultsOnly, Category = "監視|設定")
	FVector DetectionOffset;

	/** 更新頻度（秒） - パフォーマンス最適化用 */
	UPROPERTY(EditDefaultsOnly, Category = "監視|設定")
	float UpdateInterval;

private:
	// ========================================================================
	// 状態
	// ========================================================================

	/** 光の中にいるかどうか */
	bool bIsInLight;

	/** 前フレームで光の中にいたか */
	bool bWasInLight;

	/** 検知状態だったか */
	bool bWasDetected;

	/** 現在の検知レベル（0-1） */
	float DetectionLevel;

	/** 現在位置の光の強度 */
	float CurrentLightIntensity;

	/** 最後の更新からの経過時間 */
	float TimeSinceLastUpdate;

	/** サブシステム参照（キャッシュ） */
	TWeakObjectPtr<USurveillanceSubsystem> CachedSubsystem;

	/** 検知位置を取得 */
	FVector GetDetectionLocation() const;

	/** サブシステムを取得 */
	USurveillanceSubsystem* GetSurveillanceSubsystem();
};
