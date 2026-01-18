// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurveillanceLightComponent.generated.h"

class USurveillanceSubsystem;

/**
 * 監視光源コンポーネント
 *
 * このコンポーネントをアクターに追加すると、自動的にSurveillanceSubsystemに登録される
 * 光源として機能し、プレイヤーの検知に寄与する
 */
UCLASS(ClassGroup=(Dawnlight), meta=(BlueprintSpawnableComponent))
class DAWNLIGHT_API USurveillanceLightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurveillanceLightComponent();

	// ========================================================================
	// 設定
	// ========================================================================

	/** 光の半径 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "光源設定", meta = (ClampMin = "50.0", ClampMax = "5000.0"))
	float LightRadius;

	/** 光の強度（0-1、検知速度に影響） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "光源設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LightIntensity;

	/** 検知に寄与するかどうか（環境光など装飾目的の光は false） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "光源設定")
	bool bContributesToDetection;

	/** ゲーム開始時に有効化するか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "光源設定")
	bool bEnabledOnStart;

	// ========================================================================
	// 制御
	// ========================================================================

	/** 光源を有効化 */
	UFUNCTION(BlueprintCallable, Category = "光源")
	void EnableLight();

	/** 光源を無効化 */
	UFUNCTION(BlueprintCallable, Category = "光源")
	void DisableLight();

	/** 光源の有効/無効をトグル */
	UFUNCTION(BlueprintCallable, Category = "光源")
	void ToggleLight();

	/** 光源が有効かどうか */
	UFUNCTION(BlueprintPure, Category = "光源")
	bool IsLightEnabled() const { return bIsEnabled; }

	/** 光の半径を設定 */
	UFUNCTION(BlueprintCallable, Category = "光源")
	void SetLightRadius(float NewRadius);

	/** 光の強度を設定 */
	UFUNCTION(BlueprintCallable, Category = "光源")
	void SetLightIntensity(float NewIntensity);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** 現在有効かどうか */
	UPROPERTY()
	bool bIsEnabled;

	/** キャッシュされたサブシステム参照 */
	UPROPERTY()
	TWeakObjectPtr<USurveillanceSubsystem> CachedSubsystem;

	/** サブシステムに登録 */
	void RegisterWithSubsystem();

	/** サブシステムから解除 */
	void UnregisterFromSubsystem();

	/** サブシステムを取得 */
	USurveillanceSubsystem* GetSurveillanceSubsystem();
};
