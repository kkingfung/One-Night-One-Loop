// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UpgradeTypes.h"
#include "SoulTypes.h"
#include "UpgradeDataAsset.generated.h"

/**
 * アップグレードデータアセット
 *
 * ローグライトシステムで出現するアップグレードの定義
 * AssetManagerで管理され、ゲーム開始時にロードされる
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API UUpgradeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 基本情報
	// ========================================================================

	/** 一意の識別子 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "基本情報")
	FName UpgradeID;

	/** アップグレード名（日本語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "基本情報")
	FText DisplayName;

	/** アップグレード名（英語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "基本情報")
	FString DisplayNameEN;

	/** 説明文（日本語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "基本情報", meta = (MultiLine = true))
	FText Description;

	/** アイコン */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "基本情報")
	TSoftObjectPtr<UTexture2D> Icon;

	// ========================================================================
	// 分類
	// ========================================================================

	/** カテゴリ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "分類")
	EUpgradeCategory Category = EUpgradeCategory::Passive;

	/** レアリティ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "分類")
	EUpgradeRarity Rarity = EUpgradeRarity::Common;

	/** 関連するソウルタイプ（セットボーナス用） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "分類")
	ESoulType RelatedSoulType = ESoulType::None;

	// ========================================================================
	// 効果
	// ========================================================================

	/** ステータス修正値 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "効果")
	TArray<FStatModifier> StatModifiers;

	/** ビジュアル効果 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "効果")
	FVisualModifier VisualModifier;

	/** スキル効果（カテゴリがSkillの場合） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "効果", meta = (EditCondition = "Category == EUpgradeCategory::Skill"))
	FSkillEffect SkillEffect;

	/** 武器データ（カテゴリがWeaponの場合） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "効果", meta = (EditCondition = "Category == EUpgradeCategory::Weapon"))
	FWeaponData WeaponData;

	// ========================================================================
	// 取得条件
	// ========================================================================

	/** 出現に必要な最小ウェーブ番号 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "条件", meta = (ClampMin = "1"))
	int32 MinWaveRequired = 1;

	/** 前提となるアップグレードID */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "条件")
	TArray<FName> PrerequisiteUpgradeIDs;

	/** 排他的アップグレードID（これを持っていると出現しない） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "条件")
	TArray<FName> ExclusiveUpgradeIDs;

	// ========================================================================
	// スタッキング
	// ========================================================================

	/** スタック可能か */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スタック")
	bool bStackable = false;

	/** 最大スタック数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スタック", meta = (EditCondition = "bStackable", ClampMin = "1", ClampMax = "10"))
	int32 MaxStacks = 1;

	// ========================================================================
	// UPrimaryDataAsset オーバーライド
	// ========================================================================

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType("Upgrade"), UpgradeID);
	}
};

/**
 * セットボーナス段階データ
 */
USTRUCT(BlueprintType)
struct FSetBonusTier
{
	GENERATED_BODY()

	/** この段階に必要なソウル数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "セットボーナス")
	int32 RequiredCount = 0;

	/** この段階で得られるステータス修正 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "セットボーナス")
	TArray<FStatModifier> Modifiers;
};

/**
 * ソウルセットボーナスデータアセット
 *
 * 同じタイプのソウルを集めると発動するボーナス
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API USoulSetBonusDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** ソウルタイプ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "セットボーナス")
	ESoulType SoulType = ESoulType::None;

	/** セットボーナス名 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "セットボーナス")
	FText SetName;

	/** 説明文 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "セットボーナス", meta = (MultiLine = true))
	FText Description;

	/** 段階ごとのボーナス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "セットボーナス")
	TArray<FSetBonusTier> BonusTiers;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		FString AssetName = FString::Printf(TEXT("SetBonus_%s"), *UEnum::GetValueAsString(SoulType));
		return FPrimaryAssetId(FPrimaryAssetType("SoulSetBonus"), FName(*AssetName));
	}
};

/**
 * 敵バリアントデータアセット
 *
 * ParagonMinionsの色バリアントを定義
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API UEnemyVariantDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** バリアント名 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	FName VariantID;

	/** 表示名 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	FText DisplayName;

	/** カラー名（Black, Blue, Green, Red, White） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	FString ColorVariant;

	/** ドロップするソウルタイプ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	ESoulType DropSoulType = ESoulType::None;

	/** ステータス倍率 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	float StatMultiplier = 1.0f;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType("EnemyVariant"), VariantID);
	}
};
