// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Data/SoulDataAsset.h"  // ESoulBuffTypeを参照
#include "SoulBuffGameplayEffect.generated.h"

/**
 * 魂バフGameplayEffect基底クラス
 *
 * 魂収集で得られるバフ効果のベースクラス
 * - 各魂タイプに対応したAttribute修正
 * - スタック可能（収集数に応じて効果増加）
 */
UCLASS(Abstract, Blueprintable)
class DAWNLIGHT_API USoulBuffGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	USoulBuffGameplayEffect();

	/** バフタイプ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "魂バフ")
	ESoulBuffType BuffType;

	/** 対応する魂タグ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "魂バフ")
	FGameplayTag SoulTypeTag;

	/** 1個あたりの効果量（%） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "魂バフ", meta = (ClampMin = "0"))
	float EffectPerSoul;

	/** 最大スタック数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "魂バフ", meta = (ClampMin = "1"))
	int32 MaxStacks;
};

/**
 * パワーバフ（攻撃力UP）
 */
UCLASS()
class DAWNLIGHT_API USoulBuff_Power : public USoulBuffGameplayEffect
{
	GENERATED_BODY()

public:
	USoulBuff_Power();
};

/**
 * スピードバフ（移動速度UP）
 */
UCLASS()
class DAWNLIGHT_API USoulBuff_Speed : public USoulBuffGameplayEffect
{
	GENERATED_BODY()

public:
	USoulBuff_Speed();
};

/**
 * ガードバフ（防御力UP）
 */
UCLASS()
class DAWNLIGHT_API USoulBuff_Guard : public USoulBuffGameplayEffect
{
	GENERATED_BODY()

public:
	USoulBuff_Guard();
};

/**
 * ラックバフ（クリティカル率UP）
 */
UCLASS()
class DAWNLIGHT_API USoulBuff_Luck : public USoulBuffGameplayEffect
{
	GENERATED_BODY()

public:
	USoulBuff_Luck();
};

/**
 * リジェネバフ（HP回復）
 */
UCLASS()
class DAWNLIGHT_API USoulBuff_Regen : public USoulBuffGameplayEffect
{
	GENERATED_BODY()

public:
	USoulBuff_Regen();
};

/**
 * リーパーモードバフ
 *
 * リーパーモード発動中の一時的なバフ
 * - ダメージ2倍
 * - 無敵
 */
UCLASS()
class DAWNLIGHT_API UReaperModeGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UReaperModeGameplayEffect();
};

/**
 * ダメージGameplayEffect
 *
 * ダメージ適用用のGameplayEffect
 */
UCLASS()
class DAWNLIGHT_API UDamageGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UDamageGameplayEffect();
};
