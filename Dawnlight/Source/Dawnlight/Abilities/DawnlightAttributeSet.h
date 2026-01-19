// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DawnlightAttributeSet.generated.h"

// マクロ定義：属性アクセサ
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Soul Reaper 属性セット
 *
 * GAS用の属性定義
 * - HP/戦闘関連
 * - リーパーモード
 * - バフシステム
 */
UCLASS()
class DAWNLIGHT_API UDawnlightAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDawnlightAttributeSet();

	// ========================================================================
	// UAttributeSet インターフェース
	// ========================================================================

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// ========================================================================
	// HP属性
	// ========================================================================

	/** 現在のHP */
	UPROPERTY(BlueprintReadOnly, Category = "HP", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, Health)

	/** 最大HP */
	UPROPERTY(BlueprintReadOnly, Category = "HP", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, MaxHealth)

	// ========================================================================
	// 戦闘属性
	// ========================================================================

	/** 基本攻撃力 */
	UPROPERTY(BlueprintReadOnly, Category = "戦闘", ReplicatedUsing = OnRep_BaseDamage)
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, BaseDamage)

	/** 移動速度 */
	UPROPERTY(BlueprintReadOnly, Category = "戦闘", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, MoveSpeed)

	/** 攻撃速度（倍率） */
	UPROPERTY(BlueprintReadOnly, Category = "戦闘", ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, AttackSpeed)

	/** 防御力（ダメージ軽減率 0-100%） */
	UPROPERTY(BlueprintReadOnly, Category = "戦闘", ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, Defense)

	// ========================================================================
	// リーパーモード属性
	// ========================================================================

	/** リーパーゲージ（0-100） */
	UPROPERTY(BlueprintReadOnly, Category = "リーパー", ReplicatedUsing = OnRep_ReaperGauge)
	FGameplayAttributeData ReaperGauge;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, ReaperGauge)

	/** リーパーゲージ最大値 */
	UPROPERTY(BlueprintReadOnly, Category = "リーパー", ReplicatedUsing = OnRep_MaxReaperGauge)
	FGameplayAttributeData MaxReaperGauge;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, MaxReaperGauge)

	// ========================================================================
	// バフ属性（魂から得られるバフ）
	// ========================================================================

	/** ダメージバフ倍率（1.0 = 100%、1.5 = 150%） */
	UPROPERTY(BlueprintReadOnly, Category = "バフ", ReplicatedUsing = OnRep_DamageMultiplier)
	FGameplayAttributeData DamageMultiplier;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, DamageMultiplier)

	/** スピードバフ倍率（1.0 = 100%） */
	UPROPERTY(BlueprintReadOnly, Category = "バフ", ReplicatedUsing = OnRep_SpeedMultiplier)
	FGameplayAttributeData SpeedMultiplier;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, SpeedMultiplier)

	/** 防御バフ加算（直接Defense属性に加算される） */
	UPROPERTY(BlueprintReadOnly, Category = "バフ", ReplicatedUsing = OnRep_DefenseBonus)
	FGameplayAttributeData DefenseBonus;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, DefenseBonus)

	/** クールダウン短縮率（0-50%） */
	UPROPERTY(BlueprintReadOnly, Category = "バフ", ReplicatedUsing = OnRep_CooldownReduction)
	FGameplayAttributeData CooldownReduction;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, CooldownReduction)

	/** ラック（クリティカル率やドロップ率に影響、0-100） */
	UPROPERTY(BlueprintReadOnly, Category = "バフ", ReplicatedUsing = OnRep_Luck)
	FGameplayAttributeData Luck;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, Luck)

	// ========================================================================
	// メタ属性（一時的なダメージ計算用）
	// ========================================================================

	/** 受けるダメージ（一時的、計算後にHealthに適用） */
	UPROPERTY(BlueprintReadOnly, Category = "メタ")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, IncomingDamage)

	/** 与えるダメージ（一時的、計算後にターゲットに適用） */
	UPROPERTY(BlueprintReadOnly, Category = "メタ")
	FGameplayAttributeData OutgoingDamage;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, OutgoingDamage)

	// ========================================================================
	// 便利関数
	// ========================================================================

	/** 最終ダメージを計算（バフ込み） */
	UFUNCTION(BlueprintPure, Category = "戦闘")
	float GetFinalDamage() const;

	/** 最終移動速度を計算（バフ込み） */
	UFUNCTION(BlueprintPure, Category = "戦闘")
	float GetFinalMoveSpeed() const;

	/** 最終防御力を計算（バフ込み） */
	UFUNCTION(BlueprintPure, Category = "戦闘")
	float GetFinalDefense() const;

	/** HP割合を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "HP")
	float GetHealthPercent() const;

	/** リーパーゲージ割合を取得（0-1） */
	UFUNCTION(BlueprintPure, Category = "リーパー")
	float GetReaperGaugePercent() const;

	/** リーパーモード発動可能かどうか */
	UFUNCTION(BlueprintPure, Category = "リーパー")
	bool CanActivateReaperMode() const;

protected:
	// ========================================================================
	// レプリケーション
	// ========================================================================

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// HP
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	// 戦闘
	UFUNCTION()
	virtual void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Defense(const FGameplayAttributeData& OldValue);

	// リーパー
	UFUNCTION()
	virtual void OnRep_ReaperGauge(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxReaperGauge(const FGameplayAttributeData& OldValue);

	// バフ
	UFUNCTION()
	virtual void OnRep_DamageMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SpeedMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_DefenseBonus(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CooldownReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Luck(const FGameplayAttributeData& OldValue);

private:
	/** 属性値をクランプ */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
