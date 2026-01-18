// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DawnlightGameMode.generated.h"

class UEventDirectorSubsystem;
class UNightProgressSubsystem;
class USurveillanceSubsystem;
class UGameplayHUDWidget;
class UNightCompleteWidget;

/**
 * Dawnlight ゲームモード
 *
 * Night 1のゲームフローを管理するゲームモード
 * - 夜の開始/終了処理
 * - フェーズ進行管理
 * - HUD管理
 * - 夜明け演出
 */
UCLASS()
class DAWNLIGHT_API ADawnlightGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADawnlightGameMode();

	// ========================================================================
	// ゲームフロー
	// ========================================================================

	/** 夜を開始する */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void StartNight();

	/** 夜明けによる強制終了 */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void TriggerDawn();

	/** 夜が進行中かどうか */
	UFUNCTION(BlueprintPure, Category = "ゲームフロー")
	bool IsNightActive() const { return bNightActive; }

	/** ゲームをリスタート */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void RestartNight();

	/** メインメニューに戻る */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void ReturnToMainMenu();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ========================================================================
	// イベント（BP実装可能）
	// ========================================================================

	/** 夜開始時に呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void OnNightStarted();

	/** 夜明け時に呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void OnDawnTriggered();

	/** フェーズ変更時に呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void OnPhaseChanged(int32 OldPhase, int32 NewPhase);

	/** 夜明け演出完了時に呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void OnNightCompleteSequenceFinished();

	// ========================================================================
	// UI
	// ========================================================================

	/** HUDを表示 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameplayHUD();

	/** HUDを非表示 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameplayHUD();

	/** Night Complete演出を表示 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowNightCompleteScreen();

	// ========================================================================
	// ウィジェットクラス設定
	// ========================================================================

	/** ゲームプレイHUDウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayHUDWidget> GameplayHUDWidgetClass;

	/** Night Complete画面ウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> NightCompleteWidgetClass;

private:
	// ========================================================================
	// サブシステムイベントハンドラ
	// ========================================================================

	/** NightProgressSubsystemの夜明けイベントハンドラ */
	UFUNCTION()
	void HandleDawnTriggered();

	/** NightProgressSubsystemのフェーズ変更イベントハンドラ */
	UFUNCTION()
	void HandlePhaseChanged(int32 OldPhase, int32 NewPhase);

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** サブシステムを初期化 */
	void InitializeSubsystems();

	/** サブシステムイベントをバインド */
	void BindSubsystemEvents();

	/** サブシステムイベントをアンバインド */
	void UnbindSubsystemEvents();

	/** 夜明け演出を開始 */
	void StartDawnSequence();

	/** 夜明け演出のタイマー完了 */
	void OnDawnSequenceComplete();

	// ========================================================================
	// 状態
	// ========================================================================

	/** 夜が進行中かどうか */
	UPROPERTY(VisibleInstanceOnly, Category = "状態")
	bool bNightActive;

	/** 現在のフェーズ（0=導入, 1=緩和, 2=クライマックス） */
	UPROPERTY(VisibleInstanceOnly, Category = "状態")
	int32 CurrentPhase;

	/** 夜明け演出中かどうか */
	UPROPERTY(VisibleInstanceOnly, Category = "状態")
	bool bDawnSequenceActive;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 夜の総時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float NightDuration;

	/** 夜明け演出の時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float DawnSequenceDuration;

	/** ゲーム開始時に自動で夜を開始するか */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	bool bAutoStartNight;

	/** 自動開始までの遅延（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float AutoStartDelay;

	// ========================================================================
	// サブシステム参照
	// ========================================================================

	UPROPERTY()
	TWeakObjectPtr<UNightProgressSubsystem> NightProgressSubsystem;

	UPROPERTY()
	TWeakObjectPtr<USurveillanceSubsystem> SurveillanceSubsystem;

	// ========================================================================
	// ウィジェット参照
	// ========================================================================

	UPROPERTY()
	TObjectPtr<UGameplayHUDWidget> GameplayHUDWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> NightCompleteWidget;

	// ========================================================================
	// タイマー
	// ========================================================================

	FTimerHandle AutoStartTimerHandle;
	FTimerHandle DawnSequenceTimerHandle;
};
