// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SoulDataAsset.generated.h"

class UNiagaraSystem;

/**
 * 魂のレアリティ
 */
UENUM(BlueprintType)
enum class ESoulRarity : uint8
{
	Common      UMETA(DisplayName = "コモン"),      // Chicken
	Uncommon    UMETA(DisplayName = "アンコモン"),  // Dog
	Rare        UMETA(DisplayName = "レア"),        // Horse, Tiger
	Epic        UMETA(DisplayName = "エピック"),    // Deer
	Legendary   UMETA(DisplayName = "レジェンダリー") // Penguin, Kitty
};

/**
 * 魂が与えるバフの種類
 */
UENUM(BlueprintType)
enum class ESoulBuffType : uint8
{
	Damage          UMETA(DisplayName = "ダメージ"),
	Speed           UMETA(DisplayName = "スピード"),
	Defense         UMETA(DisplayName = "防御"),
	Cooldown        UMETA(DisplayName = "クールダウン"),
	Luck            UMETA(DisplayName = "ラック"),
	AllStats        UMETA(DisplayName = "全ステータス"),
	ReaperGauge     UMETA(DisplayName = "リーパーゲージ")
};

/**
 * 魂のバフ効果定義
 */
USTRUCT(BlueprintType)
struct DAWNLIGHT_API FSoulBuffEffect
{
	GENERATED_BODY()

	/** バフの種類 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バフ")
	ESoulBuffType BuffType = ESoulBuffType::Damage;

	/** バフ量（種類によって意味が異なる）
	 *  Damage/Speed: 倍率増加（0.1 = +10%）
	 *  Defense: 加算（5 = +5%軽減）
	 *  Cooldown: 減少率（5 = -5%）
	 *  Luck: 加算（5 = +5ラック）
	 *  ReaperGauge: 直接加算（20 = +20ゲージ）
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バフ", meta = (ClampMin = "0.0"))
	float BuffAmount = 0.1f;

	/** バフの持続時間（0 = 永続、Dawn Phase終了まで） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バフ", meta = (ClampMin = "0.0"))
	float Duration = 0.0f;
};

/**
 * 魂データアセット
 *
 * 各動物の魂の定義
 * - 識別情報
 * - レアリティ
 * - バフ効果
 * - ビジュアル/サウンド
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API USoulDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 識別情報
	// ========================================================================

	/** 魂の識別タグ（Soul.Type.Tiger など） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別")
	FGameplayTag SoulTag;

	/** 対応する動物タグ（Animal.Type.Tiger など） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別")
	FGameplayTag AnimalTag;

	/** 魂の名前（日本語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別")
	FText DisplayName;

	/** 魂の名前（英語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別")
	FString DisplayNameEN;

	/** 魂の説明 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "識別", meta = (MultiLine = true))
	FText Description;

	// ========================================================================
	// レアリティとスポーン
	// ========================================================================

	/** レアリティ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "レアリティ")
	ESoulRarity Rarity = ESoulRarity::Common;

	/** スポーン重み（高いほど出やすい） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "レアリティ", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float SpawnWeight = 50.0f;

	/** 最大同時スポーン数（1フェーズ内） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "レアリティ", meta = (ClampMin = "1", ClampMax = "10"))
	int32 MaxSpawnCount = 3;

	// ========================================================================
	// 動物の設定
	// ========================================================================

	/** 動物のHP（狩猟に必要なダメージ量） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物", meta = (ClampMin = "1.0"))
	float AnimalHealth = 50.0f;

	/** 動物の移動速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物", meta = (ClampMin = "0.0"))
	float AnimalMoveSpeed = 200.0f;

	/** 動物が逃げる距離（プレイヤーからこの距離で逃げ始める） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物", meta = (ClampMin = "0.0"))
	float FleeDistance = 500.0f;

	/** 動物のBlueprintクラス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "動物")
	TSoftClassPtr<AActor> AnimalBlueprintClass;

	// ========================================================================
	// バフ効果
	// ========================================================================

	/** この魂が与えるバフ効果（複数可） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バフ")
	TArray<FSoulBuffEffect> BuffEffects;

	/** 魂を獲得した時のリーパーゲージ増加量 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "バフ", meta = (ClampMin = "0.0"))
	float ReaperGaugeGain = 10.0f;

	// ========================================================================
	// ビジュアル
	// ========================================================================

	/** 魂アイコン（UI用） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビジュアル")
	TSoftObjectPtr<UTexture2D> SoulIcon;

	/** 魂の色（エフェクト用） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビジュアル")
	FLinearColor SoulColor = FLinearColor::White;

	/** 魂収集エフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビジュアル")
	TSoftObjectPtr<UParticleSystem> CollectEffect;

	/** 魂収集Niagaraエフェクト */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビジュアル")
	TSoftObjectPtr<UNiagaraSystem> CollectNiagaraEffect;

	// ========================================================================
	// サウンド
	// ========================================================================

	/** 動物の鳴き声（狩猟時） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TSoftObjectPtr<USoundBase> AnimalCrySound;

	/** 魂収集時のサウンド */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TSoftObjectPtr<USoundBase> CollectSound;

	// ========================================================================
	// UPrimaryDataAsset インターフェース
	// ========================================================================

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		// SoulDataとして登録（AssetManagerで管理）
		return FPrimaryAssetId(TEXT("SoulData"), GetFName());
	}

	// ========================================================================
	// 便利関数
	// ========================================================================

	/** レアリティに基づく色を取得 */
	UFUNCTION(BlueprintPure, Category = "魂")
	FLinearColor GetRarityColor() const;

	/** レアリティ名を取得 */
	UFUNCTION(BlueprintPure, Category = "魂")
	FText GetRarityDisplayName() const;

	/** バフの説明文を生成 */
	UFUNCTION(BlueprintPure, Category = "魂")
	FText GetBuffDescription() const;
};

/**
 * 魂コレクション（プレイヤーが収集した魂を管理）
 */
USTRUCT(BlueprintType)
struct DAWNLIGHT_API FSoulCollection
{
	GENERATED_BODY()

	/** 収集した魂の種類とカウント */
	UPROPERTY(BlueprintReadOnly, Category = "魂")
	TMap<FGameplayTag, int32> CollectedSouls;

	/** 魂を追加 */
	void AddSoul(const FGameplayTag& SoulTag, int32 Count = 1);

	/** 魂のカウントを取得 */
	int32 GetSoulCount(const FGameplayTag& SoulTag) const;

	/** 総魂数を取得 */
	int32 GetTotalSoulCount() const;

	/** コレクションをクリア */
	void Clear();
};
