// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "Data/UpgradeTypes.h"
#include "UpgradeCardWidget.generated.h"

class UUpgradeDataAsset;
class UImage;
class UTextBlock;
class UButton;
class UBorder;
class UVerticalBox;

/**
 * アップグレードカード選択時のデリゲート
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeCardSelected, UUpgradeDataAsset*, SelectedUpgrade);

/**
 * アップグレードカードウィジェット
 *
 * Wave終了後のアップグレード選択画面で使用する個別カード
 * レアリティに応じた見た目、アニメーション対応
 *
 * 機能:
 * - アップグレードデータの表示
 * - レアリティに応じた色・枠
 * - 選択時のアニメーション
 * - ホバー効果
 */
UCLASS()
class DAWNLIGHT_API UUpgradeCardWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 初期化
	// ========================================================================

	/**
	 * アップグレードデータを設定
	 * @param InUpgradeData 表示するアップグレードデータ
	 */
	UFUNCTION(BlueprintCallable, Category = "アップグレードカード")
	void SetUpgradeData(UUpgradeDataAsset* InUpgradeData);

	/** 現在のアップグレードデータを取得 */
	UFUNCTION(BlueprintPure, Category = "アップグレードカード")
	UUpgradeDataAsset* GetUpgradeData() const { return UpgradeData; }

	// ========================================================================
	// 状態制御
	// ========================================================================

	/** カードを選択可能に設定 */
	UFUNCTION(BlueprintCallable, Category = "アップグレードカード")
	void SetSelectable(bool bSelectable);

	/** カードを選択状態に設定 */
	UFUNCTION(BlueprintCallable, Category = "アップグレードカード")
	void SetSelected(bool bSelected);

	/** カードが選択状態か */
	UFUNCTION(BlueprintPure, Category = "アップグレードカード")
	bool IsSelected() const { return bIsSelected; }

	// ========================================================================
	// アニメーション
	// ========================================================================

	/** 登場アニメーションを再生 */
	UFUNCTION(BlueprintCallable, Category = "アップグレードカード|アニメーション")
	void PlayRevealAnimation(float Delay = 0.0f);

	/** 選択アニメーションを再生 */
	UFUNCTION(BlueprintCallable, Category = "アップグレードカード|アニメーション")
	void PlaySelectAnimation();

	/** 非選択（消える）アニメーションを再生 */
	UFUNCTION(BlueprintCallable, Category = "アップグレードカード|アニメーション")
	void PlayDismissAnimation();

	// ========================================================================
	// イベント
	// ========================================================================

	/** カード選択時のデリゲート */
	UPROPERTY(BlueprintAssignable, Category = "イベント")
	FOnUpgradeCardSelected OnCardSelected;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ========================================================================
	// UI要素（Blueprintでバインド）
	// ========================================================================

	/** カード全体の枠 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CardBorder;

	/** カード背景 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> CardBackground;

	/** アイコン表示 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UpgradeIcon;

	/** アップグレード名 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeName;

	/** 説明文 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeDescription;

	/** レアリティ表示 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RarityText;

	/** カテゴリ表示 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CategoryText;

	/** ステータス効果リスト */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> StatModifiersBox;

	/** 選択ボタン */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;

	/** グロー効果（レジェンダリー用） */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> GlowEffect;

	// ========================================================================
	// スタイル設定
	// ========================================================================

	/** レアリティごとの枠色 */
	UPROPERTY(EditDefaultsOnly, Category = "スタイル|レアリティ")
	TMap<EUpgradeRarity, FLinearColor> RarityBorderColors;

	/** レアリティごとの背景色 */
	UPROPERTY(EditDefaultsOnly, Category = "スタイル|レアリティ")
	TMap<EUpgradeRarity, FLinearColor> RarityBackgroundColors;

	/** レアリティごとのテキスト色 */
	UPROPERTY(EditDefaultsOnly, Category = "スタイル|レアリティ")
	TMap<EUpgradeRarity, FLinearColor> RarityTextColors;

	/** デフォルトアイコン（アイコンがない場合） */
	UPROPERTY(EditDefaultsOnly, Category = "スタイル")
	TObjectPtr<UTexture2D> DefaultIcon;

	/** 選択時のスケール */
	UPROPERTY(EditDefaultsOnly, Category = "スタイル|アニメーション")
	float SelectedScale = 1.1f;

	/** ホバー時のスケール */
	UPROPERTY(EditDefaultsOnly, Category = "スタイル|アニメーション")
	float HoverScale = 1.05f;

private:
	// ========================================================================
	// 内部データ
	// ========================================================================

	/** 現在のアップグレードデータ */
	UPROPERTY()
	TObjectPtr<UUpgradeDataAsset> UpgradeData;

	/** 選択状態 */
	bool bIsSelected = false;

	/** 選択可能状態 */
	bool bIsSelectable = true;

	// ========================================================================
	// 内部関数
	// ========================================================================

	/** UIを更新 */
	void UpdateUI();

	/** レアリティに応じたスタイルを適用 */
	void ApplyRarityStyle(EUpgradeRarity Rarity);

	/** ステータス修正表示を更新 */
	void UpdateStatModifiers();

	/** デフォルトのレアリティ色を初期化 */
	void InitializeDefaultColors();

	/** レアリティ名を取得 */
	FText GetRarityDisplayName(EUpgradeRarity Rarity) const;

	/** カテゴリ名を取得 */
	FText GetCategoryDisplayName(EUpgradeCategory Category) const;

	/** ステータスタイプ名を取得 */
	FText GetStatTypeName(EStatModifierType StatType) const;

	// ========================================================================
	// イベントハンドラ
	// ========================================================================

	UFUNCTION()
	void OnSelectButtonClicked();

	UFUNCTION()
	void OnSelectButtonHovered();

	UFUNCTION()
	void OnSelectButtonUnhovered();
};
