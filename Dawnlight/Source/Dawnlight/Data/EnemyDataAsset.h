// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EnemyDataAsset.generated.h"

class UNiagaraSystem;

/**
 * 敵の種類
 */
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	DawnMelee		UMETA(DisplayName = "黎明の近接兵"),		// 基本近接敵
	DawnRanged		UMETA(DisplayName = "黎明の射手"),		// 遠距離敵
	DawnElite		UMETA(DisplayName = "黎明の精鋭"),		// エリート敵
	DawnBoss		UMETA(DisplayName = "黎明の守護者")		// ボス敵
};

/**
 * 敵のカラーバリアント（色による特殊効果）
 *
 * ParagonMinionsのカラーバリエーションを活用
 */
UENUM(BlueprintType)
enum class EEnemyColorVariant : uint8
{
	Default		UMETA(DisplayName = "通常"),			// 基本ステータス
	Red			UMETA(DisplayName = "紅蓮"),			// 攻撃時に炎上（DoT）
	Blue		UMETA(DisplayName = "氷結"),			// 攻撃時に減速
	Green		UMETA(DisplayName = "毒霧"),			// 毒攻撃（持続ダメージ）
	Black		UMETA(DisplayName = "深淵"),			// HP・ダメージ1.5倍（エリート）
	White		UMETA(DisplayName = "聖光")			// HP2倍、高防御（タンク）
};

/**
 * カラーバリアント設定
 */
USTRUCT(BlueprintType)
struct DAWNLIGHT_API FEnemyVariantConfig
{
	GENERATED_BODY()

	/** バリアントタイプ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	EEnemyColorVariant Variant = EEnemyColorVariant::Default;

	/** HP倍率 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント", meta = (ClampMin = "0.5", ClampMax = "5.0"))
	float HealthMultiplier = 1.0f;

	/** ダメージ倍率 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント", meta = (ClampMin = "0.5", ClampMax = "5.0"))
	float DamageMultiplier = 1.0f;

	/** 防御倍率 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント", meta = (ClampMin = "0.5", ClampMax = "5.0"))
	float DefenseMultiplier = 1.0f;

	/** 特殊効果のダメージ（炎上、毒等） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント", meta = (ClampMin = "0.0"))
	float SpecialEffectDamage = 5.0f;

	/** 特殊効果の持続時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント", meta = (ClampMin = "0.0"))
	float SpecialEffectDuration = 3.0f;

	/** 確定ドロップする魂タグ（空なら通常ドロップ） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	FGameplayTag GuaranteedSoulDrop;

	/** マテリアルオーバーレイ色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	FLinearColor OverlayColor = FLinearColor::White;

	/** スポーン重み（0 = スポーンしない） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float SpawnWeight = 0.0f;

	/** 最小出現ウェーブ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント", meta = (ClampMin = "1"))
	int32 MinWaveToSpawn = 1;
};

/**
 * 敵データアセット
 *
 * Dawn Phaseで出現する敵の定義
 * - 識別情報
 * - ステータス
 * - AI設定
 * - ビジュアル/サウンド
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API UEnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 識別情報
	// ========================================================================

	/** 敵の識別タグ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別")
	FGameplayTag EnemyTag;

	/** 敵の種類 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別")
	EEnemyType EnemyType = EEnemyType::DawnMelee;

	/** 敵の名前（日本語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別")
	FText DisplayName;

	/** 敵の名前（英語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別")
	FString DisplayNameEN;

	// ========================================================================
	// ステータス
	// ========================================================================

	/** 最大HP */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ステータス", meta = (ClampMin = "1.0"))
	float MaxHealth = 50.0f;

	/** 攻撃力 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ステータス", meta = (ClampMin = "0.0"))
	float AttackDamage = 10.0f;

	/** 移動速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ステータス", meta = (ClampMin = "0.0"))
	float MoveSpeed = 300.0f;

	/** 攻撃クールダウン（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ステータス", meta = (ClampMin = "0.1"))
	float AttackCooldown = 1.5f;

	/** 攻撃範囲 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ステータス", meta = (ClampMin = "0.0"))
	float AttackRange = 150.0f;

	// ========================================================================
	// AI設定
	// ========================================================================

	/** プレイヤー検知距離 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (ClampMin = "0.0"))
	float DetectionRadius = 1000.0f;

	/** 攻撃時に立ち止まるかどうか */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	bool bStopWhileAttacking = true;

	// ========================================================================
	// スポーン設定
	// ========================================================================

	/** スポーン重み（高いほど出やすい） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スポーン", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float SpawnWeight = 50.0f;

	/** 出現開始ウェーブ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スポーン", meta = (ClampMin = "1"))
	int32 MinWaveToSpawn = 1;

	/** 敵のBlueprintクラス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スポーン")
	TSoftClassPtr<AActor> EnemyBlueprintClass;

	// ========================================================================
	// カラーバリアント設定
	// ========================================================================

	/** カラーバリアント設定（色ごとの特殊効果） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バリアント")
	TArray<FEnemyVariantConfig> ColorVariants;

	// ========================================================================
	// ビジュアル
	// ========================================================================

	/** 敵の色（エフェクト用） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビジュアル")
	FLinearColor EnemyColor = FLinearColor::Red;

	/** 死亡エフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビジュアル")
	TObjectPtr<UNiagaraSystem> DeathEffect;

	/** スポーンエフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビジュアル")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	// ========================================================================
	// サウンド
	// ========================================================================

	/** 攻撃サウンド */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TSoftObjectPtr<USoundBase> AttackSound;

	/** 死亡サウンド */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TSoftObjectPtr<USoundBase> DeathSound;

	// ========================================================================
	// UPrimaryDataAsset インターフェース
	// ========================================================================

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("EnemyData"), GetFName());
	}

	// ========================================================================
	// 便利関数
	// ========================================================================

	/** 指定バリアントの設定を取得（見つからなければデフォルトを返す） */
	UFUNCTION(BlueprintPure, Category = "敵")
	FEnemyVariantConfig GetVariantConfig(EEnemyColorVariant Variant) const;

	/** ウェーブに応じたランダムなバリアントを選択 */
	UFUNCTION(BlueprintPure, Category = "敵")
	EEnemyColorVariant SelectRandomVariant(int32 CurrentWave) const;

	/** バリアントの表示名を取得 */
	UFUNCTION(BlueprintPure, Category = "敵")
	static FText GetVariantDisplayName(EEnemyColorVariant Variant);

	/** バリアントの色を取得 */
	UFUNCTION(BlueprintPure, Category = "敵")
	static FLinearColor GetVariantColor(EEnemyColorVariant Variant);
};
