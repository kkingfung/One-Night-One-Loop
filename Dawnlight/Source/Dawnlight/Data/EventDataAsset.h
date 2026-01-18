// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EventDataAsset.generated.h"

/**
 * イベントタイプ
 */
UENUM(BlueprintType)
enum class EEventType : uint8
{
	Fixed    UMETA(DisplayName = "固定"),
	Random   UMETA(DisplayName = "ランダム")
};

/**
 * イベントデータアセット
 *
 * ゲーム内イベントの定義データ
 * - 固定イベント（3つ）
 * - ランダムイベント（プールから選択）
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API UEventDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UEventDataAsset();

	// ========================================================================
	// 基本情報
	// ========================================================================

	/** イベント識別タグ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
	FGameplayTag EventTag;

	/** イベント名（日本語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
	FText EventNameJP;

	/** イベント名（英語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
	FString EventNameEN;

	/** イベント説明 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント", meta = (MultiLine = "true"))
	FText EventDescription;

	/** イベントタイプ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
	EEventType EventType;

	// ========================================================================
	// 難易度設定
	// ========================================================================

	/** 緊張度レベル（0.0 - 1.0） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float TensionLevel;

	/** 発生に必要なフェーズ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "設定")
	FGameplayTag RequiredPhase;

	/** 前提条件タグ（これらが満たされている必要がある） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "設定")
	FGameplayTagContainer Prerequisites;

	// ========================================================================
	// 報酬/ペナルティ
	// ========================================================================

	/** 成功時の緊張度変化 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "報酬")
	float SuccessTensionChange;

	/** 失敗時の緊張度変化 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "報酬")
	float FailureTensionChange;

	/** 成功時に付与されるタグ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "報酬")
	FGameplayTagContainer SuccessGrantedTags;

	// ========================================================================
	// 演出参照
	// ========================================================================

	/** イベント開始時に再生するサウンド */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "演出")
	TObjectPtr<USoundBase> StartSound;

	/** イベント完了時に再生するサウンド */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "演出")
	TObjectPtr<USoundBase> CompleteSound;

	// ========================================================================
	// 固定イベント専用
	// ========================================================================

	/** 固定イベントの順序（0から開始） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "固定イベント", meta = (EditCondition = "EventType == EEventType::Fixed"))
	int32 FixedEventOrder;

	/** チュートリアル要素を含むか */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "固定イベント", meta = (EditCondition = "EventType == EEventType::Fixed"))
	bool bIsTutorial;

	// ========================================================================
	// ランダムイベント専用
	// ========================================================================

	/** 選択重み（高いほど選ばれやすい） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ランダムイベント", meta = (EditCondition = "EventType == EEventType::Random", ClampMin = "0.1"))
	float SelectionWeight;

	/** 最大出現回数（0=無制限） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ランダムイベント", meta = (EditCondition = "EventType == EEventType::Random"))
	int32 MaxOccurrences;

	// ========================================================================
	// UPrimaryDataAsset インターフェース
	// ========================================================================

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
