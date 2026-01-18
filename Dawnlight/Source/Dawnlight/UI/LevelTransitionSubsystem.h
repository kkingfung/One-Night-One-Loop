// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelTransitionSubsystem.generated.h"

class UUserWidget;

// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelTransitionStarted, const FString&, LevelName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelTransitionCompleted, const FString&, LevelName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingProgressChanged, float, Progress);

/**
 * レベル遷移管理サブシステム
 *
 * シーン/レベルの遷移を管理
 * - ローディング画面の表示
 * - 非同期レベルロード
 * - 遷移アニメーション
 */
UCLASS()
class DAWNLIGHT_API ULevelTransitionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ========================================================================
	// サブシステムライフサイクル
	// ========================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========================================================================
	// レベル遷移
	// ========================================================================

	/** メインメニューに遷移 */
	UFUNCTION(BlueprintCallable, Category = "レベル遷移")
	void TransitionToMainMenu();

	/** ゲームレベルに遷移 */
	UFUNCTION(BlueprintCallable, Category = "レベル遷移")
	void TransitionToGameLevel(const FString& LevelName);

	/** 指定レベルに遷移（汎用） */
	UFUNCTION(BlueprintCallable, Category = "レベル遷移")
	void TransitionToLevel(const FName& LevelName, bool bShowLoadingScreen = true);

	/** 現在のレベルをリスタート */
	UFUNCTION(BlueprintCallable, Category = "レベル遷移")
	void RestartCurrentLevel();

	/** 遷移中かどうか */
	UFUNCTION(BlueprintPure, Category = "レベル遷移")
	bool IsTransitioning() const { return bIsTransitioning; }

	// ========================================================================
	// ローディング画面
	// ========================================================================

	/** ローディング画面を表示 */
	UFUNCTION(BlueprintCallable, Category = "レベル遷移|ローディング")
	void ShowLoadingScreen();

	/** ローディング画面を非表示 */
	UFUNCTION(BlueprintCallable, Category = "レベル遷移|ローディング")
	void HideLoadingScreen();

	/** ローディング進捗を設定 (0.0 - 1.0) */
	UFUNCTION(BlueprintCallable, Category = "レベル遷移|ローディング")
	void SetLoadingProgress(float Progress);

	// ========================================================================
	// 設定
	// ========================================================================

	/** メインメニューのレベル名 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "レベル遷移|設定")
	FName MainMenuLevelName = TEXT("L_MainMenu");

	/** デフォルトのゲームレベル名 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "レベル遷移|設定")
	FName DefaultGameLevelName = TEXT("L_TestSurveillance");

	/** ローディング画面ウィジェットクラス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "レベル遷移|設定")
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

	/** 最小ローディング表示時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "レベル遷移|設定", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float MinLoadingDisplayTime = 1.0f;

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** レベル遷移が開始された */
	UPROPERTY(BlueprintAssignable, Category = "レベル遷移|イベント")
	FOnLevelTransitionStarted OnLevelTransitionStarted;

	/** レベル遷移が完了した */
	UPROPERTY(BlueprintAssignable, Category = "レベル遷移|イベント")
	FOnLevelTransitionCompleted OnLevelTransitionCompleted;

	/** ローディング進捗が変更された */
	UPROPERTY(BlueprintAssignable, Category = "レベル遷移|イベント")
	FOnLoadingProgressChanged OnLoadingProgressChanged;

private:
	/** 遷移中フラグ */
	bool bIsTransitioning;

	/** ローディング画面インスタンス */
	UPROPERTY()
	TObjectPtr<UUserWidget> LoadingScreenWidget;

	/** ローディング表示開始時間 */
	double LoadingStartTime;

	/** 遷移先レベル名 */
	FName PendingLevelName;

	/** 非同期ロード完了ハンドラ */
	void OnAsyncLoadComplete();

	/** 実際のレベル遷移を実行 */
	void ExecuteLevelTransition(const FName& LevelName);

	/** 最小表示時間経過後にローディングを終了 */
	void FinishLoadingAfterMinTime();

	/** タイマーハンドル */
	FTimerHandle LoadingTimerHandle;
};
