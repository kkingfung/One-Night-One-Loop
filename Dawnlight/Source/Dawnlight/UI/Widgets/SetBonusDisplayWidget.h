// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "Data/SoulTypes.h"
#include "SetBonusDisplayWidget.generated.h"

class UUpgradeSubsystem;
class USoulCollectionSubsystem;
class UVerticalBox;
class UHorizontalBox;
class UTextBlock;
class UImage;
class UProgressBar;

/**
 * 個別のセットボーナス進捗情報
 */
USTRUCT(BlueprintType)
struct FSetBonusProgressInfo
{
	GENERATED_BODY()

	/** ソウルタイプ */
	UPROPERTY(BlueprintReadOnly, Category = "セットボーナス")
	ESoulType SoulType = ESoulType::None;

	/** 現在の収集数 */
	UPROPERTY(BlueprintReadOnly, Category = "セットボーナス")
	int32 CurrentCount = 0;

	/** 次の段階に必要な数 */
	UPROPERTY(BlueprintReadOnly, Category = "セットボーナス")
	int32 NextTierCount = 3;

	/** 現在の段階（0 = まだボーナスなし） */
	UPROPERTY(BlueprintReadOnly, Category = "セットボーナス")
	int32 CurrentTier = 0;

	/** 最大段階 */
	UPROPERTY(BlueprintReadOnly, Category = "セットボーナス")
	int32 MaxTier = 3;

	/** 進捗率（0.0 - 1.0） */
	UPROPERTY(BlueprintReadOnly, Category = "セットボーナス")
	float Progress = 0.0f;

	/** ボーナスがアクティブか */
	UPROPERTY(BlueprintReadOnly, Category = "セットボーナス")
	bool bIsActive = false;
};

/**
 * セットボーナス表示ウィジェット
 *
 * HUDに表示されるセットボーナスの進捗状況
 * - ソウルタイプごとの収集数
 * - セットボーナスの達成状態
 * - 次の段階までの進捗
 *
 * Night Phase中に表示し、Dawn Phaseでは非表示
 */
UCLASS()
class DAWNLIGHT_API USetBonusDisplayWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 更新
	// ========================================================================

	/** セットボーナス情報を更新 */
	UFUNCTION(BlueprintCallable, Category = "セットボーナス")
	void RefreshDisplay();

	/** 特定のソウルタイプの進捗を取得 */
	UFUNCTION(BlueprintPure, Category = "セットボーナス")
	FSetBonusProgressInfo GetProgressForSoulType(ESoulType SoulType) const;

	/** 全てのソウルタイプの進捗を取得 */
	UFUNCTION(BlueprintPure, Category = "セットボーナス")
	TArray<FSetBonusProgressInfo> GetAllProgress() const;

	// ========================================================================
	// 表示制御
	// ========================================================================

	/** コンパクト表示モード（アイコンと数字のみ） */
	UFUNCTION(BlueprintCallable, Category = "セットボーナス|表示")
	void SetCompactMode(bool bCompact);

	/** 詳細表示モード（説明文も表示） */
	UFUNCTION(BlueprintCallable, Category = "セットボーナス|表示")
	void SetDetailedMode(bool bDetailed);

	// ========================================================================
	// アニメーション
	// ========================================================================

	/** セットボーナス達成時の演出 */
	UFUNCTION(BlueprintCallable, Category = "セットボーナス|アニメーション")
	void PlayBonusAchievedAnimation(ESoulType SoulType, int32 Tier);

	/** ソウル収集時の更新アニメーション */
	UFUNCTION(BlueprintCallable, Category = "セットボーナス|アニメーション")
	void PlaySoulCollectedAnimation(ESoulType SoulType);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** ボーナス項目のコンテナ */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> BonusItemContainer;

	/** タイトルテキスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 表示するソウルタイプ（空の場合は全て表示） */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	TArray<ESoulType> DisplaySoulTypes;

	/** ソウルタイプごとのアイコン */
	UPROPERTY(EditDefaultsOnly, Category = "設定|アイコン")
	TMap<ESoulType, TSoftObjectPtr<UTexture2D>> SoulTypeIcons;

	/** ソウルタイプごとの色 */
	UPROPERTY(EditDefaultsOnly, Category = "設定|色")
	TMap<ESoulType, FLinearColor> SoulTypeColors;

	/** セットボーナス達成に必要な数（段階ごと） */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	TArray<int32> BonusTierThresholds;

	/** コンパクトモード中か */
	UPROPERTY(BlueprintReadOnly, Category = "設定")
	bool bIsCompactMode = true;

	/** 詳細モード中か */
	UPROPERTY(BlueprintReadOnly, Category = "設定")
	bool bIsDetailedMode = false;

	/** 自動更新間隔（0で無効） */
	UPROPERTY(EditDefaultsOnly, Category = "設定", meta = (ClampMin = "0.0"))
	float AutoRefreshInterval = 0.5f;

private:
	// ========================================================================
	// 内部データ
	// ========================================================================

	/** UpgradeSubsystemへの参照 */
	UPROPERTY()
	TObjectPtr<UUpgradeSubsystem> UpgradeSubsystem;

	/** SoulCollectionSubsystemへの参照 */
	UPROPERTY()
	TObjectPtr<USoulCollectionSubsystem> SoulCollectionSubsystem;

	/** 進捗情報のキャッシュ */
	UPROPERTY()
	TMap<ESoulType, FSetBonusProgressInfo> ProgressCache;

	/** 生成されたUI要素のキャッシュ（ソウルタイプ→ウィジェット） */
	TMap<ESoulType, TWeakObjectPtr<UWidget>> ItemWidgetCache;

	/** 自動更新タイマー */
	float AutoRefreshTimer = 0.0f;

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** サブシステムをキャッシュ */
	void CacheSubsystems();

	/** デフォルト値を初期化 */
	void InitializeDefaults();

	/** 全てのボーナス項目UIを生成 */
	void CreateBonusItemWidgets();

	/** 単一のボーナス項目UIを生成 */
	UWidget* CreateSingleBonusItemWidget(ESoulType SoulType);

	/** ボーナス項目UIを更新 */
	void UpdateBonusItemWidget(ESoulType SoulType, const FSetBonusProgressInfo& Progress);

	/** 進捗情報を計算 */
	FSetBonusProgressInfo CalculateProgress(ESoulType SoulType) const;

	/** ソウルタイプ名を取得 */
	FText GetSoulTypeName(ESoulType SoulType) const;

	/** 段階名を取得 */
	FText GetTierName(int32 Tier) const;

	// ========================================================================
	// イベントハンドラ
	// ========================================================================

	/** ソウル収集時のコールバック */
	UFUNCTION()
	void OnSoulCollected(const FSoulCollectedEventData& EventData);

	/** セットボーナス達成時のコールバック */
	UFUNCTION()
	void OnSetBonusActivated(ESoulType SoulType, int32 Tier);
};
