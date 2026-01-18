// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayTagContainer.h"
#include "DawnlightGameState.generated.h"

class UEventDataAsset;

/**
 * ゲーム進行状態
 */
UENUM(BlueprintType)
enum class EGameProgressState : uint8
{
	NotStarted   UMETA(DisplayName = "未開始"),
	InProgress   UMETA(DisplayName = "進行中"),
	Paused       UMETA(DisplayName = "一時停止"),
	Completed    UMETA(DisplayName = "完了"),
	Failed       UMETA(DisplayName = "失敗")
};

/**
 * イベント完了記録
 */
USTRUCT(BlueprintType)
struct FEventCompletionRecord
{
	GENERATED_BODY()

	/** イベントタグ */
	UPROPERTY(BlueprintReadOnly, Category = "イベント")
	FGameplayTag EventTag;

	/** 成功したか */
	UPROPERTY(BlueprintReadOnly, Category = "イベント")
	bool bWasSuccessful = false;

	/** 完了時の夜の進行度 */
	UPROPERTY(BlueprintReadOnly, Category = "イベント")
	float CompletionProgress = 0.0f;

	/** 獲得した証拠価値 */
	UPROPERTY(BlueprintReadOnly, Category = "イベント")
	float EvidenceGained = 0.0f;
};

/**
 * Dawnlight ゲームステート
 *
 * ゲーム全体の状態を管理
 * - 夜の進行状況
 * - イベント完了記録
 * - プレイヤー統計
 * - ゲーム結果
 */
UCLASS()
class DAWNLIGHT_API ADawnlightGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADawnlightGameState();

	// ========================================================================
	// ゲーム状態
	// ========================================================================

	/** 現在のゲーム進行状態を取得 */
	UFUNCTION(BlueprintPure, Category = "ゲーム状態")
	EGameProgressState GetProgressState() const { return ProgressState; }

	/** ゲーム進行状態を設定 */
	UFUNCTION(BlueprintCallable, Category = "ゲーム状態")
	void SetProgressState(EGameProgressState NewState);

	/** ゲームが進行中か */
	UFUNCTION(BlueprintPure, Category = "ゲーム状態")
	bool IsGameInProgress() const { return ProgressState == EGameProgressState::InProgress; }

	// ========================================================================
	// 夜の進行
	// ========================================================================

	/** 現在の夜の進行度を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "夜の進行")
	float GetNightProgress() const { return NightProgress; }

	/** 夜の進行度を設定 */
	UFUNCTION(BlueprintCallable, Category = "夜の進行")
	void SetNightProgress(float Progress);

	/** 現在のフェーズを取得 */
	UFUNCTION(BlueprintPure, Category = "夜の進行")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	/** フェーズを設定 */
	UFUNCTION(BlueprintCallable, Category = "夜の進行")
	void SetCurrentPhase(int32 Phase);

	// ========================================================================
	// イベント記録
	// ========================================================================

	/** イベント完了を記録 */
	UFUNCTION(BlueprintCallable, Category = "イベント")
	void RecordEventCompletion(FGameplayTag EventTag, bool bWasSuccessful, float EvidenceGained = 0.0f);

	/** イベントが完了済みかチェック */
	UFUNCTION(BlueprintPure, Category = "イベント")
	bool IsEventCompleted(FGameplayTag EventTag) const;

	/** 完了したイベント数を取得 */
	UFUNCTION(BlueprintPure, Category = "イベント")
	int32 GetCompletedEventCount() const { return EventCompletionRecords.Num(); }

	/** イベント完了記録を取得 */
	UFUNCTION(BlueprintPure, Category = "イベント")
	const TArray<FEventCompletionRecord>& GetEventCompletionRecords() const { return EventCompletionRecords; }

	// ========================================================================
	// プレイヤー統計
	// ========================================================================

	/** 合計証拠価値を取得 */
	UFUNCTION(BlueprintPure, Category = "統計")
	float GetTotalEvidenceValue() const { return TotalEvidenceValue; }

	/** 証拠価値を追加 */
	UFUNCTION(BlueprintCallable, Category = "統計")
	void AddEvidenceValue(float Value);

	/** 検知回数を取得 */
	UFUNCTION(BlueprintPure, Category = "統計")
	int32 GetDetectionCount() const { return DetectionCount; }

	/** 検知回数をインクリメント */
	UFUNCTION(BlueprintCallable, Category = "統計")
	void IncrementDetectionCount();

	/** 撮影回数を取得 */
	UFUNCTION(BlueprintPure, Category = "統計")
	int32 GetPhotosTaken() const { return PhotosTaken; }

	/** 撮影回数をインクリメント */
	UFUNCTION(BlueprintCallable, Category = "統計")
	void IncrementPhotosTaken();

	/** 隠れた回数を取得 */
	UFUNCTION(BlueprintPure, Category = "統計")
	int32 GetTimesHidden() const { return TimesHidden; }

	/** 隠れた回数をインクリメント */
	UFUNCTION(BlueprintCallable, Category = "統計")
	void IncrementTimesHidden();

	// ========================================================================
	// 緊張度
	// ========================================================================

	/** 現在の緊張度を取得（0-100） */
	UFUNCTION(BlueprintPure, Category = "緊張度")
	float GetTensionLevel() const { return TensionLevel; }

	/** 緊張度を設定 */
	UFUNCTION(BlueprintCallable, Category = "緊張度")
	void SetTensionLevel(float NewLevel);

	/** 緊張度を変更 */
	UFUNCTION(BlueprintCallable, Category = "緊張度")
	void ModifyTensionLevel(float Delta);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** ゲーム状態変更時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameProgressState, NewState);
	UPROPERTY(BlueprintAssignable, Category = "ゲーム状態|イベント")
	FOnGameStateChanged OnGameStateChanged;

	/** フェーズ変更時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhaseChanged, int32, OldPhase, int32, NewPhase);
	UPROPERTY(BlueprintAssignable, Category = "夜の進行|イベント")
	FOnPhaseChanged OnPhaseChanged;

	/** 緊張度変更時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTensionChanged, float, OldTension, float, NewTension);
	UPROPERTY(BlueprintAssignable, Category = "緊張度|イベント")
	FOnTensionChanged OnTensionChanged;

protected:
	// ========================================================================
	// 状態変数
	// ========================================================================

	/** 現在のゲーム進行状態 */
	UPROPERTY(ReplicatedUsing = OnRep_ProgressState, BlueprintReadOnly, Category = "ゲーム状態")
	EGameProgressState ProgressState;

	/** 夜の進行度（0-1） */
	UPROPERTY(ReplicatedUsing = OnRep_NightProgress, BlueprintReadOnly, Category = "夜の進行")
	float NightProgress;

	/** 現在のフェーズ（0=導入, 1=緩和, 2=クライマックス） */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "夜の進行")
	int32 CurrentPhase;

	/** 緊張度（0-100） */
	UPROPERTY(ReplicatedUsing = OnRep_TensionLevel, BlueprintReadOnly, Category = "緊張度")
	float TensionLevel;

	// ========================================================================
	// 統計変数
	// ========================================================================

	/** 合計証拠価値 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "統計")
	float TotalEvidenceValue;

	/** 検知された回数 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "統計")
	int32 DetectionCount;

	/** 撮影した回数 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "統計")
	int32 PhotosTaken;

	/** 隠れた回数 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "統計")
	int32 TimesHidden;

	/** イベント完了記録 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "イベント")
	TArray<FEventCompletionRecord> EventCompletionRecords;

	// ========================================================================
	// レプリケーション
	// ========================================================================

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ProgressState();

	UFUNCTION()
	void OnRep_NightProgress();

	UFUNCTION()
	void OnRep_TensionLevel();
};
