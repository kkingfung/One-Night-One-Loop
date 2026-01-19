// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UpgradeTypes.generated.h"

class UNiagaraSystem;
class UMaterialInterface;
class UStaticMesh;
class UAnimMontage;

/**
 * アップグレードカテゴリ
 */
UENUM(BlueprintType)
enum class EUpgradeCategory : uint8
{
	Weapon			UMETA(DisplayName = "武器"),			// 武器関連
	Skill			UMETA(DisplayName = "スキル"),		// アビリティ関連
	Passive			UMETA(DisplayName = "パッシブ"),		// 常時効果
	SoulAffinity	UMETA(DisplayName = "魂親和"),		// ソウル収集ボーナス
	Visual			UMETA(DisplayName = "ビジュアル"),	// 見た目変化

	Max				UMETA(Hidden)
};

/**
 * アップグレードレアリティ
 */
UENUM(BlueprintType)
enum class EUpgradeRarity : uint8
{
	Common			UMETA(DisplayName = "コモン"),		// 50% 出現率
	Uncommon		UMETA(DisplayName = "アンコモン"),	// 30% 出現率
	Rare			UMETA(DisplayName = "レア"),			// 15% 出現率
	Epic			UMETA(DisplayName = "エピック"),		// 4% 出現率
	Legendary		UMETA(DisplayName = "レジェンダリー"),	// 1% 出現率

	Max				UMETA(Hidden)
};

/**
 * 武器タイプ
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Scythe			UMETA(DisplayName = "大鎌"),			// デフォルト、広範囲
	Staff			UMETA(DisplayName = "杖"),			// 長射程、速攻

	Max				UMETA(Hidden)
};

/**
 * ステータス修正タイプ
 */
UENUM(BlueprintType)
enum class EStatModifierType : uint8
{
	// === 攻撃系 ===
	AttackDamage		UMETA(DisplayName = "攻撃力"),
	AttackSpeed			UMETA(DisplayName = "攻撃速度"),
	AttackRange			UMETA(DisplayName = "攻撃範囲"),
	CriticalChance		UMETA(DisplayName = "クリティカル率"),
	CriticalDamage		UMETA(DisplayName = "クリティカルダメージ"),

	// === 防御系 ===
	MaxHealth			UMETA(DisplayName = "最大HP"),
	HealthRegen			UMETA(DisplayName = "HP回復"),
	DamageReduction		UMETA(DisplayName = "ダメージ軽減"),
	DodgeChance			UMETA(DisplayName = "回避率"),

	// === 移動系 ===
	MoveSpeed			UMETA(DisplayName = "移動速度"),
	DashCooldown		UMETA(DisplayName = "ダッシュCD"),
	DashDistance		UMETA(DisplayName = "ダッシュ距離"),

	// === リソース系 ===
	SoulGainMultiplier	UMETA(DisplayName = "ソウル獲得量"),
	SoulDuration		UMETA(DisplayName = "ソウル持続時間"),
	ReaperGaugeRate		UMETA(DisplayName = "リーパーゲージ上昇率"),

	// === スキル系 ===
	SkillCooldown		UMETA(DisplayName = "スキルCD"),
	SkillDamage			UMETA(DisplayName = "スキルダメージ"),
	SkillRange			UMETA(DisplayName = "スキル範囲"),

	Max					UMETA(Hidden)
};

/**
 * ステータス修正値
 */
USTRUCT(BlueprintType)
struct FStatModifier
{
	GENERATED_BODY()

	/** 修正するステータス */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ステータス")
	EStatModifierType StatType = EStatModifierType::AttackDamage;

	/** 加算値（固定値として追加） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ステータス")
	float AdditiveValue = 0.0f;

	/** 乗算値（パーセント、0.2 = +20%） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ステータス")
	float MultiplicativeValue = 0.0f;
};

/**
 * ビジュアル変更データ
 */
USTRUCT(BlueprintType)
struct FVisualModifier
{
	GENERATED_BODY()

	/** キャラクターに適用するパーティクル */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ビジュアル")
	TSoftObjectPtr<UNiagaraSystem> CharacterParticle;

	/** 武器に適用するパーティクル */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ビジュアル")
	TSoftObjectPtr<UNiagaraSystem> WeaponParticle;

	/** マテリアルオーバーレイ（グロー効果など） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ビジュアル")
	TSoftObjectPtr<UMaterialInterface> OverlayMaterial;

	/** カラーティント */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ビジュアル")
	FLinearColor ColorTint = FLinearColor::White;

	/** グロー強度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ビジュアル", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float GlowIntensity = 0.0f;

	/** スケール変更（1.0 = 変更なし） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ビジュアル", meta = (ClampMin = "0.5", ClampMax = "2.0"))
	float ScaleMultiplier = 1.0f;
};

/**
 * スキル効果データ
 */
USTRUCT(BlueprintType)
struct FSkillEffect
{
	GENERATED_BODY()

	/** スキル識別タグ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スキル")
	FGameplayTag SkillTag;

	/** スキル名 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スキル")
	FText SkillName;

	/** 使用するアニメーション */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スキル")
	TSoftObjectPtr<UAnimMontage> SkillMontage;

	/** スキル発動時のVFX */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スキル")
	TSoftObjectPtr<UNiagaraSystem> SkillVFX;

	/** クールダウン（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スキル", meta = (ClampMin = "0.0"))
	float Cooldown = 10.0f;

	/** ダメージ倍率 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スキル", meta = (ClampMin = "0.0"))
	float DamageMultiplier = 1.0f;

	/** 効果範囲 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スキル", meta = (ClampMin = "0.0"))
	float EffectRadius = 300.0f;
};

/**
 * 武器データ
 */
USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	/** 武器タイプ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器")
	EWeaponType WeaponType = EWeaponType::Scythe;

	/** 武器メッシュ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器")
	TSoftObjectPtr<UStaticMesh> WeaponMesh;

	/** 基本ダメージ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器")
	float BaseDamage = 10.0f;

	/** 攻撃速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器")
	float AttackSpeed = 1.0f;

	/** 攻撃範囲 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器")
	float AttackRange = 200.0f;

	/** コンボ段数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器")
	int32 ComboCount = 4;
};

/**
 * アップグレード選択肢の重み付け
 */
USTRUCT(BlueprintType)
struct FUpgradeWeight
{
	GENERATED_BODY()

	/** レアリティごとの出現重み */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "重み")
	TMap<EUpgradeRarity, float> RarityWeights;

	FUpgradeWeight()
	{
		// デフォルト重み設定
		RarityWeights.Add(EUpgradeRarity::Common, 50.0f);
		RarityWeights.Add(EUpgradeRarity::Uncommon, 30.0f);
		RarityWeights.Add(EUpgradeRarity::Rare, 15.0f);
		RarityWeights.Add(EUpgradeRarity::Epic, 4.0f);
		RarityWeights.Add(EUpgradeRarity::Legendary, 1.0f);
	}
};
