// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoulDataAsset.h"  // ESoulRarityをここからインポート
#include "SoulTypes.generated.h"

/**
 * ソウルタイプ - 敵を倒した時に得られる魂の種類
 *
 * Animals_Freeアセットの動物をベースに、
 * それぞれ異なるセットボーナスを提供する
 */
UENUM(BlueprintType)
enum class ESoulType : uint8
{
	None			UMETA(DisplayName = "なし"),

	// === 動物系ソウル（セットボーナス対象） ===
	Tiger			UMETA(DisplayName = "虎魂 - 獰猛"),		// 攻撃速度
	Horse			UMETA(DisplayName = "馬魂 - 疾走"),		// 移動速度
	Dog				UMETA(DisplayName = "犬魂 - 忠誠"),		// ソウル持続時間
	Cat				UMETA(DisplayName = "猫魂 - 俊敏"),		// 回避率
	Deer			UMETA(DisplayName = "鹿魂 - 優雅"),		// スタミナ回復
	Wolf			UMETA(DisplayName = "狼魂 - 群狼"),		// 範囲ダメージ

	// === 特殊ソウル ===
	Golden			UMETA(DisplayName = "金魂 - 希少"),		// レア、経験値ボーナス
	Corrupted		UMETA(DisplayName = "穢魂 - 呪い"),		// リスク＆リワード

	Max				UMETA(Hidden)
};

// ESoulRarity は SoulDataAsset.h で定義済み（重複を避けるためここでは定義しない）

/**
 * ソウルデータ - 収集したソウルの情報
 */
USTRUCT(BlueprintType)
struct FSoulData
{
	GENERATED_BODY()

	/** ソウルの種類 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ソウル")
	ESoulType Type = ESoulType::None;

	/** レアリティ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ソウル")
	ESoulRarity Rarity = ESoulRarity::Common;

	/** 収集数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ソウル")
	int32 Count = 0;

	/** このタイプのソウル価値（経験値換算） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ソウル")
	int32 Value = 1;
};

/**
 * セットボーナス定義
 */
USTRUCT(BlueprintType)
struct FSoulSetBonus
{
	GENERATED_BODY()

	/** ボーナス名 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "セットボーナス")
	FText BonusName;

	/** ボーナス説明 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "セットボーナス")
	FText Description;

	/** 必要なソウルタイプ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "セットボーナス")
	ESoulType RequiredSoulType = ESoulType::None;

	/** 発動に必要な数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "セットボーナス")
	int32 RequiredCount = 3;

	/** ボーナス効果値（パーセント、0.2 = 20%） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "セットボーナス", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float BonusValue = 0.2f;

	/** 対応するGameplayTag（GASで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "セットボーナス")
	FGameplayTag BonusTag;
};
