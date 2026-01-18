// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HideableComponent.generated.h"

class ADawnlightCharacter;

/**
 * 隠れ場所コンポーネント
 *
 * 物陰など、プレイヤーが隠れることができる場所に付与
 * - 隠れる/出る操作
 * - 収容人数管理
 * - 視覚的フィードバック
 */
UCLASS(ClassGroup=(Dawnlight), meta=(BlueprintSpawnableComponent))
class DAWNLIGHT_API UHideableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHideableComponent();

	// ========================================================================
	// 隠れる操作
	// ========================================================================

	/** プレイヤーを隠す */
	UFUNCTION(BlueprintCallable, Category = "隠れ場所")
	bool HidePlayer(ADawnlightCharacter* Player);

	/** プレイヤーを出す */
	UFUNCTION(BlueprintCallable, Category = "隠れ場所")
	bool UnhidePlayer(ADawnlightCharacter* Player);

	/** 強制的に全員を出す */
	UFUNCTION(BlueprintCallable, Category = "隠れ場所")
	void ForceUnhideAll();

	// ========================================================================
	// 状態取得
	// ========================================================================

	/** 隠れることができるか */
	UFUNCTION(BlueprintPure, Category = "隠れ場所")
	bool CanHide() const;

	/** 空いているスロット数 */
	UFUNCTION(BlueprintPure, Category = "隠れ場所")
	int32 GetAvailableSlots() const { return MaxOccupants - HiddenPlayers.Num(); }

	/** 隠れている人数 */
	UFUNCTION(BlueprintPure, Category = "隠れ場所")
	int32 GetOccupantCount() const { return HiddenPlayers.Num(); }

	/** 特定のプレイヤーが隠れているか */
	UFUNCTION(BlueprintPure, Category = "隠れ場所")
	bool IsPlayerHidden(ADawnlightCharacter* Player) const;

	/** 誰か隠れているか */
	UFUNCTION(BlueprintPure, Category = "隠れ場所")
	bool IsOccupied() const { return HiddenPlayers.Num() > 0; }

	/** 隠れ場所が使用可能か（破壊されていないか等） */
	UFUNCTION(BlueprintPure, Category = "隠れ場所")
	bool IsAvailable() const { return bIsAvailable; }

	/** 隠れ位置を取得 */
	UFUNCTION(BlueprintPure, Category = "隠れ場所")
	FVector GetHideLocation() const;

	// ========================================================================
	// 設定操作
	// ========================================================================

	/** 隠れ場所を無効化（発見された等） */
	UFUNCTION(BlueprintCallable, Category = "隠れ場所")
	void DisableHideSpot();

	/** 隠れ場所を再有効化 */
	UFUNCTION(BlueprintCallable, Category = "隠れ場所")
	void EnableHideSpot();

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** プレイヤーが隠れた時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHidden, ADawnlightCharacter*, Player);
	UPROPERTY(BlueprintAssignable, Category = "隠れ場所|イベント")
	FOnPlayerHidden OnPlayerHidden;

	/** プレイヤーが出た時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerUnhidden, ADawnlightCharacter*, Player);
	UPROPERTY(BlueprintAssignable, Category = "隠れ場所|イベント")
	FOnPlayerUnhidden OnPlayerUnhidden;

	/** 隠れ場所が発見された時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideSpotDiscovered);
	UPROPERTY(BlueprintAssignable, Category = "隠れ場所|イベント")
	FOnHideSpotDiscovered OnHideSpotDiscovered;

	/** 隠れ場所が無効化された時 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideSpotDisabled);
	UPROPERTY(BlueprintAssignable, Category = "隠れ場所|イベント")
	FOnHideSpotDisabled OnHideSpotDisabled;

protected:
	virtual void BeginPlay() override;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 最大収容人数 */
	UPROPERTY(EditDefaultsOnly, Category = "隠れ場所|設定", meta = (ClampMin = "1"))
	int32 MaxOccupants;

	/** 隠れるのにかかる時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "隠れ場所|設定")
	float HideTransitionTime;

	/** 出るのにかかる時間（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "隠れ場所|設定")
	float UnhideTransitionTime;

	/** 隠れ位置のオフセット（アクター中心からの相対位置） */
	UPROPERTY(EditDefaultsOnly, Category = "隠れ場所|設定")
	FVector HideLocationOffset;

	/** 隠れ中の視線方向 */
	UPROPERTY(EditDefaultsOnly, Category = "隠れ場所|設定")
	FRotator HiddenRotation;

	/** 発見されやすさ（0-1、高いほど見つかりやすい） */
	UPROPERTY(EditDefaultsOnly, Category = "隠れ場所|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DiscoveryChance;

	/** 完全に安全な隠れ場所か（発見されない） */
	UPROPERTY(EditDefaultsOnly, Category = "隠れ場所|設定")
	bool bIsPerfectHideSpot;

	/** インタラクション範囲 */
	UPROPERTY(EditDefaultsOnly, Category = "隠れ場所|設定")
	float InteractionRadius;

private:
	// ========================================================================
	// 状態
	// ========================================================================

	/** 隠れているプレイヤーのリスト */
	UPROPERTY()
	TArray<TWeakObjectPtr<ADawnlightCharacter>> HiddenPlayers;

	/** 使用可能フラグ */
	bool bIsAvailable;

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** プレイヤーを隠れ位置に移動 */
	void MovePlayerToHideLocation(ADawnlightCharacter* Player);

	/** プレイヤーを元の位置に戻す */
	void RestorePlayerPosition(ADawnlightCharacter* Player);

	/** 隠れ状態を適用 */
	void ApplyHiddenState(ADawnlightCharacter* Player);

	/** 隠れ状態を解除 */
	void RemoveHiddenState(ADawnlightCharacter* Player);
};
