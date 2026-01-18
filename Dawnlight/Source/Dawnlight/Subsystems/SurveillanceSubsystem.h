// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "SurveillanceSubsystem.generated.h"

class AActor;

/**
 * 監視レベルの列挙型
 */
UENUM(BlueprintType)
enum class ESurveillanceLevel : uint8
{
	Low        UMETA(DisplayName = "低"),
	Medium     UMETA(DisplayName = "中"),
	High       UMETA(DisplayName = "高"),
	Critical   UMETA(DisplayName = "危険")
};

/**
 * 光源情報構造体
 */
USTRUCT(BlueprintType)
struct FLightSourceInfo
{
	GENERATED_BODY()

	/** 光源アクター */
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> LightActor;

	/** 光の半径 */
	UPROPERTY(BlueprintReadOnly)
	float Radius;

	/** 光の強度（0-1） */
	UPROPERTY(BlueprintReadOnly)
	float Intensity;

	/** 検知に寄与するかどうか */
	UPROPERTY(BlueprintReadOnly)
	bool bContributesToDetection;

	FLightSourceInfo()
		: Radius(500.0f)
		, Intensity(1.0f)
		, bContributesToDetection(true)
	{}
};

/**
 * 監視サブシステム
 *
 * 光源の管理と監視レベルの計算を担当
 * - 光源の登録/解除
 * - 位置が光の中にあるかの判定
 * - グローバルな監視レベルの管理
 */
UCLASS()
class DAWNLIGHT_API USurveillanceSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// ========================================================================
	// USubsystem インターフェース
	// ========================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// ========================================================================
	// 光源管理
	// ========================================================================

	/** 光源を登録 */
	UFUNCTION(BlueprintCallable, Category = "監視|光源")
	void RegisterLightSource(AActor* LightActor, float Radius, float Intensity = 1.0f, bool bContributesToDetection = true);

	/** 光源を解除 */
	UFUNCTION(BlueprintCallable, Category = "監視|光源")
	void UnregisterLightSource(AActor* LightActor);

	/** 全ての光源を取得 */
	UFUNCTION(BlueprintPure, Category = "監視|光源")
	TArray<FLightSourceInfo> GetAllLightSources() const { return RegisteredLightSources; }

	// ========================================================================
	// 検知判定
	// ========================================================================

	/** 指定位置が光の中にあるかどうか */
	UFUNCTION(BlueprintPure, Category = "監視|検知")
	bool IsLocationInLight(const FVector& Location) const;

	/** 指定位置の光の強度を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "監視|検知")
	float GetLightIntensityAtLocation(const FVector& Location) const;

	/** 指定位置が遮蔽されているかどうか */
	UFUNCTION(BlueprintPure, Category = "監視|検知")
	bool IsLocationOccluded(const FVector& Location, const FVector& LightLocation) const;

	// ========================================================================
	// 監視レベル
	// ========================================================================

	/** 現在の監視レベルを取得 */
	UFUNCTION(BlueprintPure, Category = "監視|レベル")
	ESurveillanceLevel GetCurrentSurveillanceLevel() const { return CurrentSurveillanceLevel; }

	/** 監視レベルのGameplayTagを取得 */
	UFUNCTION(BlueprintPure, Category = "監視|レベル")
	FGameplayTag GetSurveillanceLevelTag() const;

	/** 監視レベルを設定 */
	UFUNCTION(BlueprintCallable, Category = "監視|レベル")
	void SetSurveillanceLevel(ESurveillanceLevel NewLevel);

	/** 監視レベルを1段階上げる */
	UFUNCTION(BlueprintCallable, Category = "監視|レベル")
	void IncreaseSurveillanceLevel();

	/** 監視レベルをリセット */
	UFUNCTION(BlueprintCallable, Category = "監視|レベル")
	void ResetSurveillanceLevel();

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 監視レベル変更時のデリゲート */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSurveillanceLevelChanged, ESurveillanceLevel, OldLevel, ESurveillanceLevel, NewLevel);

	UPROPERTY(BlueprintAssignable, Category = "監視|イベント")
	FOnSurveillanceLevelChanged OnSurveillanceLevelChanged;

protected:
	// ========================================================================
	// 内部データ
	// ========================================================================

	/** 登録された光源リスト */
	UPROPERTY()
	TArray<FLightSourceInfo> RegisteredLightSources;

	/** 現在の監視レベル */
	UPROPERTY()
	ESurveillanceLevel CurrentSurveillanceLevel;

private:
	/** 無効な光源を削除 */
	void CleanupInvalidLightSources();
};
