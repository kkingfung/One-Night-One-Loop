// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "EventDirectorSubsystem.generated.h"

class UEventDataAsset;

/**
 * フェーズタイプ
 */
UENUM(BlueprintType)
enum class EPhaseType : uint8
{
	Introduction  UMETA(DisplayName = "導入"),
	Relaxation    UMETA(DisplayName = "緩和"),
	Climax        UMETA(DisplayName = "クライマックス")
};

/**
 * イベント選択結果
 */
USTRUCT(BlueprintType)
struct FEventSelectionResult
{
	GENERATED_BODY()

	/** 選択されたイベント */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UEventDataAsset> SelectedEvent;

	/** 選択理由 */
	UPROPERTY(BlueprintReadOnly)
	FString SelectionReason;

	/** 現在の緊張度（選択時） */
	UPROPERTY(BlueprintReadOnly)
	float TensionAtSelection;

	FEventSelectionResult()
		: SelectedEvent(nullptr)
		, TensionAtSelection(0.0f)
	{}
};

/**
 * イベントディレクターサブシステム
 *
 * イベントの選択と進行を管理するAIシステム
 * - 固定イベントの進行管理
 * - ランダムイベントの選択
 * - 難易度曲線の調整
 */
UCLASS()
class DAWNLIGHT_API UEventDirectorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ========================================================================
	// USubsystem インターフェース
	// ========================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	// ========================================================================
	// フェーズ管理
	// ========================================================================

	/** 現在のフェーズを取得 */
	UFUNCTION(BlueprintPure, Category = "イベント|フェーズ")
	EPhaseType GetCurrentPhase() const { return CurrentPhase; }

	/** フェーズのGameplayTagを取得 */
	UFUNCTION(BlueprintPure, Category = "イベント|フェーズ")
	FGameplayTag GetCurrentPhaseTag() const;

	/** フェーズを設定 */
	UFUNCTION(BlueprintCallable, Category = "イベント|フェーズ")
	void SetPhase(EPhaseType NewPhase);

	/** フェーズを進める */
	UFUNCTION(BlueprintCallable, Category = "イベント|フェーズ")
	void AdvancePhase();

	// ========================================================================
	// 固定イベント
	// ========================================================================

	/** 固定イベントを登録 */
	UFUNCTION(BlueprintCallable, Category = "イベント|固定")
	void RegisterFixedEvent(UEventDataAsset* EventAsset, int32 Order);

	/** 固定イベントの完了を記録 */
	UFUNCTION(BlueprintCallable, Category = "イベント|固定")
	void MarkFixedEventCompleted(FGameplayTag EventTag);

	/** 固定イベントが完了しているか */
	UFUNCTION(BlueprintPure, Category = "イベント|固定")
	bool IsFixedEventCompleted(FGameplayTag EventTag) const;

	/** 次の固定イベントを取得 */
	UFUNCTION(BlueprintPure, Category = "イベント|固定")
	UEventDataAsset* GetNextFixedEvent() const;

	// ========================================================================
	// ランダムイベント
	// ========================================================================

	/** ランダムイベントプールを設定 */
	UFUNCTION(BlueprintCallable, Category = "イベント|ランダム")
	void SetRandomEventPool(const TArray<UEventDataAsset*>& EventPool);

	/** ランダムイベントを選択 */
	UFUNCTION(BlueprintCallable, Category = "イベント|ランダム")
	FEventSelectionResult SelectRandomEvent();

	/** ランダムイベント使用回数を記録 */
	UFUNCTION(BlueprintCallable, Category = "イベント|ランダム")
	void RecordRandomEventUsage(FGameplayTag EventTag);

	// ========================================================================
	// 緊張度管理
	// ========================================================================

	/** 現在の緊張度を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "イベント|緊張度")
	float GetCurrentTension() const { return CurrentTension; }

	/** 緊張度を設定 */
	UFUNCTION(BlueprintCallable, Category = "イベント|緊張度")
	void SetTension(float NewTension);

	/** 緊張度を加算 */
	UFUNCTION(BlueprintCallable, Category = "イベント|緊張度")
	void AddTension(float Amount);

	// ========================================================================
	// ログ/デバッグ
	// ========================================================================

	/** イベント選択ログを取得 */
	UFUNCTION(BlueprintPure, Category = "イベント|デバッグ")
	TArray<FString> GetEventSelectionLog() const { return EventSelectionLog; }

	/** ログをクリア */
	UFUNCTION(BlueprintCallable, Category = "イベント|デバッグ")
	void ClearSelectionLog();

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** フェーズ変更時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhaseChanged, EPhaseType, OldPhase, EPhaseType, NewPhase);
	UPROPERTY(BlueprintAssignable, Category = "イベント|イベント")
	FOnPhaseChanged OnPhaseChanged;

	/** イベント選択時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventSelected, const FEventSelectionResult&, Result);
	UPROPERTY(BlueprintAssignable, Category = "イベント|イベント")
	FOnEventSelected OnEventSelected;

protected:
	// ========================================================================
	// 内部データ
	// ========================================================================

	/** 現在のフェーズ */
	UPROPERTY()
	EPhaseType CurrentPhase;

	/** 現在の緊張度 */
	UPROPERTY()
	float CurrentTension;

	/** 固定イベントリスト（順序付き） */
	UPROPERTY()
	TArray<TObjectPtr<UEventDataAsset>> FixedEvents;

	/** 固定イベント完了フラグ */
	UPROPERTY()
	TSet<FGameplayTag> CompletedFixedEvents;

	/** ランダムイベントプール */
	UPROPERTY()
	TArray<TObjectPtr<UEventDataAsset>> RandomEventPool;

	/** ランダムイベント使用回数 */
	UPROPERTY()
	TMap<FGameplayTag, int32> RandomEventUsageCount;

	/** イベント選択ログ */
	TArray<FString> EventSelectionLog;

private:
	/** ログを追加 */
	void AddToLog(const FString& Message);

	/** イベント選択スコアを計算 */
	float CalculateEventScore(const UEventDataAsset* EventAsset) const;
};
