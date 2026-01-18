// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PhotographyComponent.generated.h"

class UTextureRenderTarget2D;

/**
 * 撮影データ構造体
 *
 * 撮影した証拠の情報を保持
 */
USTRUCT(BlueprintType)
struct FPhotographData
{
	GENERATED_BODY()

	/** 撮影時刻（ゲーム内時間） */
	UPROPERTY(BlueprintReadOnly, Category = "撮影")
	float CaptureTime = 0.0f;

	/** 撮影位置 */
	UPROPERTY(BlueprintReadOnly, Category = "撮影")
	FVector CaptureLocation = FVector::ZeroVector;

	/** 撮影方向 */
	UPROPERTY(BlueprintReadOnly, Category = "撮影")
	FRotator CaptureRotation = FRotator::ZeroRotator;

	/** 撮影対象のタグ */
	UPROPERTY(BlueprintReadOnly, Category = "撮影")
	FGameplayTagContainer CapturedSubjectTags;

	/** 証拠価値（スコア） */
	UPROPERTY(BlueprintReadOnly, Category = "撮影")
	float EvidenceValue = 0.0f;

	/** 撮影が成功したか */
	UPROPERTY(BlueprintReadOnly, Category = "撮影")
	bool bIsValidEvidence = false;

	/** レンダーターゲット（サムネイル用） */
	UPROPERTY(BlueprintReadOnly, Category = "撮影")
	TObjectPtr<UTextureRenderTarget2D> Thumbnail;
};

/**
 * 撮影コンポーネント
 *
 * プレイヤーの撮影機能を提供
 * - 撮影の開始/終了
 * - 証拠の記録
 * - 撮影中の状態管理
 */
UCLASS(ClassGroup=(Dawnlight), meta=(BlueprintSpawnableComponent))
class DAWNLIGHT_API UPhotographyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPhotographyComponent();

	// ========================================================================
	// 撮影操作
	// ========================================================================

	/** 撮影を開始 */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	bool StartPhotograph();

	/** 撮影をキャンセル */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	void CancelPhotograph();

	/** 撮影を実行（シャッターを切る） */
	UFUNCTION(BlueprintCallable, Category = "撮影")
	FPhotographData ExecutePhotograph();

	// ========================================================================
	// 状態取得
	// ========================================================================

	/** 撮影中かどうか */
	UFUNCTION(BlueprintPure, Category = "撮影")
	bool IsPhotographing() const { return bIsPhotographing; }

	/** 撮影可能かどうか */
	UFUNCTION(BlueprintPure, Category = "撮影")
	bool CanPhotograph() const;

	/** 残り撮影可能回数 */
	UFUNCTION(BlueprintPure, Category = "撮影")
	int32 GetRemainingPhotos() const { return MaxPhotos - CurrentPhotoCount; }

	/** 撮影データを取得 */
	UFUNCTION(BlueprintPure, Category = "撮影")
	const TArray<FPhotographData>& GetPhotographs() const { return Photographs; }

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 撮影開始時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotographStarted);
	UPROPERTY(BlueprintAssignable, Category = "撮影|イベント")
	FOnPhotographStarted OnPhotographStarted;

	/** 撮影完了時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotographCompleted, const FPhotographData&, PhotoData);
	UPROPERTY(BlueprintAssignable, Category = "撮影|イベント")
	FOnPhotographCompleted OnPhotographCompleted;

	/** 撮影キャンセル時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotographCancelled);
	UPROPERTY(BlueprintAssignable, Category = "撮影|イベント")
	FOnPhotographCancelled OnPhotographCancelled;

	/** 撮影失敗時（フィルム切れ等） */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotographFailed, FString, Reason);
	UPROPERTY(BlueprintAssignable, Category = "撮影|イベント")
	FOnPhotographFailed OnPhotographFailed;

protected:
	virtual void BeginPlay() override;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 最大撮影可能回数 */
	UPROPERTY(EditDefaultsOnly, Category = "撮影|設定")
	int32 MaxPhotos;

	/** 撮影にかかる時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "撮影|設定")
	float PhotoDuration;

	/** シャッター音の検知範囲 */
	UPROPERTY(EditDefaultsOnly, Category = "撮影|設定")
	float ShutterSoundRadius;

	/** シャッター音を発生させるか */
	UPROPERTY(EditDefaultsOnly, Category = "撮影|設定")
	bool bMakeShutterSound;

	/** ビューファインダーの視野角 */
	UPROPERTY(EditDefaultsOnly, Category = "撮影|設定")
	float ViewfinderFOV;

	/** 撮影可能な最大距離 */
	UPROPERTY(EditDefaultsOnly, Category = "撮影|設定")
	float MaxPhotographDistance;

	/** 撮影対象として認識するタグ */
	UPROPERTY(EditDefaultsOnly, Category = "撮影|設定")
	FGameplayTagContainer ValidSubjectTags;

private:
	// ========================================================================
	// 状態
	// ========================================================================

	/** 撮影中フラグ */
	bool bIsPhotographing;

	/** 現在の撮影カウント */
	int32 CurrentPhotoCount;

	/** 撮影データ配列 */
	UPROPERTY()
	TArray<FPhotographData> Photographs;

	/** 撮影タイマーハンドル */
	FTimerHandle PhotoTimerHandle;

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** ビューファインダー内の対象を検出 */
	TArray<AActor*> DetectSubjectsInViewfinder() const;

	/** 撮影対象の証拠価値を計算 */
	float CalculateEvidenceValue(const TArray<AActor*>& Subjects) const;

	/** シャッター音を発生させる */
	void EmitShutterSound();

	/** 撮影完了処理 */
	void OnPhotoTimerComplete();
};
