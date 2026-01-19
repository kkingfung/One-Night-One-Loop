// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h"
#include "DawnlightGameMode.generated.h"

class UNightProgressSubsystem;
class USoulCollectionSubsystem;
class UGameplayHUDWidget;
class UGameResultWidget;
class ADawnlightCharacter;
class UDawnlightAttributeSet;

/**
 * ゲームフェーズ
 */
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	None            UMETA(DisplayName = "なし"),
	Night           UMETA(DisplayName = "Night Phase"),     // 魂狩猟フェーズ
	DawnTransition  UMETA(DisplayName = "Dawn Transition"), // 夜明け移行演出
	Dawn            UMETA(DisplayName = "Dawn Phase"),      // 戦闘フェーズ
	LoopEnd         UMETA(DisplayName = "Loop End")         // ループ終了
};

/**
 * Soul Reaper ゲームモード
 *
 * 2フェーズ構成のゲームフローを管理
 * - Night Phase: 動物を狩って魂を収集（3分）
 * - Dawn Phase: 敵と戦闘（Wave制）
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

	/** ゲームを開始（Night Phaseから） */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void StartGame();

	/** Night Phaseを開始 */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void StartNightPhase();

	/** Dawn Phaseを開始（Night Phase終了後自動的に呼ばれる） */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void StartDawnPhase();

	/** 現在のフェーズを取得 */
	UFUNCTION(BlueprintPure, Category = "ゲームフロー")
	EGamePhase GetCurrentPhase() const { return CurrentPhase; }

	/** Night Phase中かどうか */
	UFUNCTION(BlueprintPure, Category = "ゲームフロー")
	bool IsInNightPhase() const { return CurrentPhase == EGamePhase::Night; }

	/** Dawn Phase中かどうか */
	UFUNCTION(BlueprintPure, Category = "ゲームフロー")
	bool IsInDawnPhase() const { return CurrentPhase == EGamePhase::Dawn; }

	/** Night Phase残り時間を取得 */
	UFUNCTION(BlueprintPure, Category = "ゲームフロー")
	float GetNightPhaseTimeRemaining() const { return NightPhaseTimeRemaining; }

	/** Night Phase時間を設定 */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void SetNightPhaseDuration(float Duration) { NightPhaseDuration = Duration; }

	// ========================================================================
	// Wave管理
	// ========================================================================

	/** 次のWaveを開始 */
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartNextWave();

	/** 現在のWave番号を取得（1から開始） */
	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetCurrentWave() const { return CurrentWave; }

	/** 総Wave数を取得 */
	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetTotalWaves() const { return TotalWaves; }

	/** 残り敵数を取得 */
	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetRemainingEnemies() const { return RemainingEnemies; }

	/** 敵を倒した時に呼び出す */
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void OnEnemyKilled();

	// ========================================================================
	// ゲーム終了
	// ========================================================================

	/** プレイヤー死亡時 */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void OnPlayerDeath();

	/** ゲームクリア時（全Wave突破） */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void OnGameCleared();

	/** リスタート */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void RestartGame();

	/** メインメニューに戻る */
	UFUNCTION(BlueprintCallable, Category = "ゲームフロー")
	void ReturnToMainMenu();

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** フェーズ変更時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhaseChanged, EGamePhase, OldPhase, EGamePhase, NewPhase);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnPhaseChanged OnPhaseChanged;

	/** Wave開始時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnWaveStarted OnWaveStarted;

	/** Wave完了時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveCompleted, int32, WaveNumber);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnWaveCompleted OnWaveCompleted;

	/** ゲームオーバー時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnGameOver OnGameOver;

	/** ゲームクリア時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameClear);
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnGameClear OnGameClear;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ========================================================================
	// イベント（BP実装可能）
	// ========================================================================

	/** Night Phase開始時に呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void BP_OnNightPhaseStarted();

	/** Dawn Phase開始時に呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void BP_OnDawnPhaseStarted();

	/** Wave開始時にBPで呼ばれる（敵スポーンなど） */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void BP_OnWaveStarted(int32 WaveNumber, int32 EnemyCount);

	/** Wave完了時にBPで呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void BP_OnWaveCompleted(int32 WaveNumber);

	/** ゲームオーバー時にBPで呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void BP_OnGameOver();

	/** ゲームクリア時にBPで呼ばれる */
	UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
	void BP_OnGameClear();

	// ========================================================================
	// UI
	// ========================================================================

	/** HUDを表示 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameplayHUD();

	/** HUDを非表示 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameplayHUD();

	/** 結果画面を表示 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowResultScreen(bool bVictory);

	// ========================================================================
	// 設定
	// ========================================================================

	/** Night Phaseの時間（秒）- デフォルト3分 */
	UPROPERTY(EditDefaultsOnly, Category = "設定|Night Phase")
	float NightPhaseDuration;

	/** Night Phase中に動物をスポーンするか */
	UPROPERTY(EditDefaultsOnly, Category = "設定|Night Phase")
	bool bSpawnAnimals;

	/** 動物スポーン間隔（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定|Night Phase", meta = (EditCondition = "bSpawnAnimals"))
	float AnimalSpawnInterval;

	/** 最大動物数 */
	UPROPERTY(EditDefaultsOnly, Category = "設定|Night Phase", meta = (EditCondition = "bSpawnAnimals"))
	int32 MaxAnimalCount;

	/** 総Wave数 */
	UPROPERTY(EditDefaultsOnly, Category = "設定|Dawn Phase")
	int32 TotalWaves;

	/** 各Waveの敵数 */
	UPROPERTY(EditDefaultsOnly, Category = "設定|Dawn Phase")
	TArray<int32> EnemiesPerWave;

	/** Wave間のインターバル（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定|Dawn Phase")
	float WaveInterval;

	/** Dawn Transition演出時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定|Dawn Phase")
	float DawnTransitionDuration;

	/** ゲーム開始時に自動でNight Phaseを開始するか */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	bool bAutoStart;

	/** 自動開始までの遅延（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float AutoStartDelay;

	/** ゲームプレイHUDウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayHUDWidget> GameplayHUDWidgetClass;

	/** 結果画面ウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameResultWidget> ResultWidgetClass;

private:
	// ========================================================================
	// 内部処理
	// ========================================================================

	/** フェーズを設定 */
	void SetPhase(EGamePhase NewPhase);

	/** Night Phase終了処理 */
	void EndNightPhase();

	/** Dawn Transition演出 */
	void StartDawnTransition();

	/** Dawn Transition完了 */
	void OnDawnTransitionComplete();

	/** Wave完了チェック */
	void CheckWaveCompletion();

	/** サブシステムを初期化 */
	void InitializeSubsystems();

	/** 動物スポーン処理 */
	void SpawnAnimal();

	/** 収集した魂のバフを適用 */
	void ApplyCollectedSoulBuffs();

	// ========================================================================
	// 状態
	// ========================================================================

	/** 現在のフェーズ */
	UPROPERTY(VisibleInstanceOnly, Category = "状態")
	EGamePhase CurrentPhase;

	/** 現在のWave番号（1から開始） */
	UPROPERTY(VisibleInstanceOnly, Category = "状態")
	int32 CurrentWave;

	/** 残り敵数 */
	UPROPERTY(VisibleInstanceOnly, Category = "状態")
	int32 RemainingEnemies;

	/** Night Phase残り時間 */
	UPROPERTY(VisibleInstanceOnly, Category = "状態")
	float NightPhaseTimeRemaining;

	/** 現在の動物数 */
	UPROPERTY(VisibleInstanceOnly, Category = "状態")
	int32 CurrentAnimalCount;

	// ========================================================================
	// サブシステム参照
	// ========================================================================

	UPROPERTY()
	TWeakObjectPtr<UNightProgressSubsystem> NightProgressSubsystem;

	UPROPERTY()
	TWeakObjectPtr<USoulCollectionSubsystem> SoulCollectionSubsystem;

	// ========================================================================
	// ウィジェット参照
	// ========================================================================

	UPROPERTY()
	TObjectPtr<UGameplayHUDWidget> GameplayHUDWidget;

	UPROPERTY()
	TObjectPtr<UGameResultWidget> ResultWidget;

	// ========================================================================
	// タイマー
	// ========================================================================

	FTimerHandle AutoStartTimerHandle;
	FTimerHandle NightPhaseTimerHandle;
	FTimerHandle DawnTransitionTimerHandle;
	FTimerHandle WaveIntervalTimerHandle;
	FTimerHandle AnimalSpawnTimerHandle;
};
