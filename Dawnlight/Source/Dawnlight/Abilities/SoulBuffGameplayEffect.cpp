// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulBuffGameplayEffect.h"
#include "DawnlightAttributeSet.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

// ============================================================================
// USoulBuffGameplayEffect
// ============================================================================

USoulBuffGameplayEffect::USoulBuffGameplayEffect()
{
	BuffType = ESoulBuffType::Damage;  // デフォルト値
	EffectPerSoul = 5.0f;
	MaxStacks = 99;

	// 永続効果（Dawn Phase終了まで）
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// スタック可能
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = MaxStacks;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
}

// ============================================================================
// USoulBuff_Power（攻撃力UP）
// ============================================================================

USoulBuff_Power::USoulBuff_Power()
{
	BuffType = ESoulBuffType::Damage;
	EffectPerSoul = 5.0f; // 1個あたり5%

	// DamageMultiplier を加算
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UDawnlightAttributeSet::GetDamageMultiplierAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(0.05f)); // 5%

	Modifiers.Add(ModifierInfo);
}

// ============================================================================
// USoulBuff_Speed（移動速度UP）
// ============================================================================

USoulBuff_Speed::USoulBuff_Speed()
{
	BuffType = ESoulBuffType::Speed;
	EffectPerSoul = 5.0f;

	// SpeedMultiplier を加算
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UDawnlightAttributeSet::GetSpeedMultiplierAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(0.05f));

	Modifiers.Add(ModifierInfo);
}

// ============================================================================
// USoulBuff_Guard（防御力UP）
// ============================================================================

USoulBuff_Guard::USoulBuff_Guard()
{
	BuffType = ESoulBuffType::Defense;
	EffectPerSoul = 5.0f;

	// DefenseBonus を加算
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UDawnlightAttributeSet::GetDefenseBonusAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(5.0f)); // 5%ダメージ軽減

	Modifiers.Add(ModifierInfo);
}

// ============================================================================
// USoulBuff_Luck（クリティカル率UP）
// ============================================================================

USoulBuff_Luck::USoulBuff_Luck()
{
	BuffType = ESoulBuffType::Luck;
	EffectPerSoul = 3.0f; // 1個あたり3%クリティカル率

	// Luck を加算
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UDawnlightAttributeSet::GetLuckAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(3.0f));

	Modifiers.Add(ModifierInfo);
}

// ============================================================================
// USoulBuff_Regen（HP回復）
// ============================================================================

USoulBuff_Regen::USoulBuff_Regen()
{
	BuffType = ESoulBuffType::ReaperGauge;  // Regenは存在しないのでReaperGaugeに代用
	EffectPerSoul = 1.0f; // 1個あたり1HP/秒

	// 周期的にHealthを回復
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = 1.0f; // 1秒ごと

	// Health を加算（回復）
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UDawnlightAttributeSet::GetHealthAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.0f));

	Modifiers.Add(ModifierInfo);
}

// ============================================================================
// UReaperModeGameplayEffect
// ============================================================================

UReaperModeGameplayEffect::UReaperModeGameplayEffect()
{
	// 有限期間（リーパーモード持続時間）
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(10.0f)); // 10秒

	// ダメージ2倍
	{
		FGameplayModifierInfo ModifierInfo;
		ModifierInfo.Attribute = UDawnlightAttributeSet::GetDamageMultiplierAttribute();
		ModifierInfo.ModifierOp = EGameplayModOp::Multiplicitive;
		ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(2.0f));
		Modifiers.Add(ModifierInfo);
	}

	// 無敵タグを付与（TargetTagsGameplayEffectComponentを使用）
	// Note: UE5.3以降では、タグはGameplayEffectComponentsを通じて設定する必要がある
}

// ============================================================================
// UDamageGameplayEffect
// ============================================================================

UDamageGameplayEffect::UDamageGameplayEffect()
{
	// 即時効果
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// IncomingDamage を設定（AttributeSetのPostGameplayEffectExecuteで処理）
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UDawnlightAttributeSet::GetIncomingDamageAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;

	// SetByCallerで実際のダメージ値を設定
	// 使用時: EffectContext->SetMagnitude() または SetByCallerTagMagnitudes
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(10.0f)); // デフォルト10ダメージ

	Modifiers.Add(ModifierInfo);
}
