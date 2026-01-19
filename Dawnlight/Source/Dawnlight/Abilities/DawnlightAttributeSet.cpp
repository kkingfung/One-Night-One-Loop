// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UDawnlightAttributeSet::UDawnlightAttributeSet()
{
	// ========================================================================
	// デフォルト値を設定
	// ========================================================================

	// HP
	InitHealth(100.0f);
	InitMaxHealth(100.0f);

	// 戦闘
	InitBaseDamage(25.0f);
	InitMoveSpeed(400.0f);
	InitAttackSpeed(1.0f);
	InitDefense(0.0f);

	// リーパー
	InitReaperGauge(0.0f);
	InitMaxReaperGauge(100.0f);

	// バフ（デフォルト値は1.0倍 = 効果なし）
	InitDamageMultiplier(1.0f);
	InitSpeedMultiplier(1.0f);
	InitDefenseBonus(0.0f);
	InitCooldownReduction(0.0f);
	InitLuck(0.0f);

	// メタ属性
	InitIncomingDamage(0.0f);
	InitOutgoingDamage(0.0f);
}

void UDawnlightAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 属性値をクランプ
	ClampAttribute(Attribute, NewValue);
}

void UDawnlightAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;

	// ========================================================================
	// ダメージ処理
	// ========================================================================
	if (Attribute == GetIncomingDamageAttribute())
	{
		// 防御力を適用してダメージを計算
		const float FinalDefense = GetFinalDefense();
		const float DamageReduction = FMath::Clamp(FinalDefense / 100.0f, 0.0f, 0.9f); // 最大90%軽減
		const float RawDamage = GetIncomingDamage();
		const float ActualDamage = RawDamage * (1.0f - DamageReduction);

		// HPを減少
		const float NewHealth = FMath::Max(0.0f, GetHealth() - ActualDamage);
		SetHealth(NewHealth);

		// ダメージを受けた分だけリーパーゲージを増加（10%分）
		const float GaugeGain = ActualDamage * 0.1f;
		const float NewGauge = FMath::Min(GetReaperGauge() + GaugeGain, GetMaxReaperGauge());
		SetReaperGauge(NewGauge);

		// IncomingDamageをリセット
		SetIncomingDamage(0.0f);
	}

	// ========================================================================
	// リーパーゲージ上限チェック
	// ========================================================================
	if (Attribute == GetReaperGaugeAttribute())
	{
		const float MaxGauge = GetMaxReaperGauge();
		if (GetReaperGauge() > MaxGauge)
		{
			SetReaperGauge(MaxGauge);
		}
	}

	// ========================================================================
	// HP上限チェック
	// ========================================================================
	if (Attribute == GetHealthAttribute())
	{
		const float MaxHP = GetMaxHealth();
		if (GetHealth() > MaxHP)
		{
			SetHealth(MaxHP);
		}
	}
}

void UDawnlightAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// HP
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	// 戦闘
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, Defense, COND_None, REPNOTIFY_Always);

	// リーパー
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, ReaperGauge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, MaxReaperGauge, COND_None, REPNOTIFY_Always);

	// バフ
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, DamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, SpeedMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, DefenseBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, CooldownReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDawnlightAttributeSet, Luck, COND_None, REPNOTIFY_Always);
}

// ========================================================================
// レプリケーション通知
// ========================================================================

void UDawnlightAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, Health, OldValue);
}

void UDawnlightAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, MaxHealth, OldValue);
}

void UDawnlightAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, BaseDamage, OldValue);
}

void UDawnlightAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, MoveSpeed, OldValue);
}

void UDawnlightAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, AttackSpeed, OldValue);
}

void UDawnlightAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, Defense, OldValue);
}

void UDawnlightAttributeSet::OnRep_ReaperGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, ReaperGauge, OldValue);
}

void UDawnlightAttributeSet::OnRep_MaxReaperGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, MaxReaperGauge, OldValue);
}

void UDawnlightAttributeSet::OnRep_DamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, DamageMultiplier, OldValue);
}

void UDawnlightAttributeSet::OnRep_SpeedMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, SpeedMultiplier, OldValue);
}

void UDawnlightAttributeSet::OnRep_DefenseBonus(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, DefenseBonus, OldValue);
}

void UDawnlightAttributeSet::OnRep_CooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, CooldownReduction, OldValue);
}

void UDawnlightAttributeSet::OnRep_Luck(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDawnlightAttributeSet, Luck, OldValue);
}

// ========================================================================
// 便利関数
// ========================================================================

float UDawnlightAttributeSet::GetFinalDamage() const
{
	return GetBaseDamage() * GetDamageMultiplier();
}

float UDawnlightAttributeSet::GetFinalMoveSpeed() const
{
	return GetMoveSpeed() * GetSpeedMultiplier();
}

float UDawnlightAttributeSet::GetFinalDefense() const
{
	return GetDefense() + GetDefenseBonus();
}

float UDawnlightAttributeSet::GetHealthPercent() const
{
	const float MaxHP = GetMaxHealth();
	if (MaxHP <= 0.0f)
	{
		return 0.0f;
	}
	return FMath::Clamp(GetHealth() / MaxHP, 0.0f, 1.0f);
}

float UDawnlightAttributeSet::GetReaperGaugePercent() const
{
	const float MaxGauge = GetMaxReaperGauge();
	if (MaxGauge <= 0.0f)
	{
		return 0.0f;
	}
	return FMath::Clamp(GetReaperGauge() / MaxGauge, 0.0f, 1.0f);
}

bool UDawnlightAttributeSet::CanActivateReaperMode() const
{
	return GetReaperGauge() >= GetMaxReaperGauge();
}

// ========================================================================
// 内部関数
// ========================================================================

void UDawnlightAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// HP: 0 ～ MaxHealth
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	// MaxHealth: 1以上
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	// BaseDamage: 0以上
	else if (Attribute == GetBaseDamageAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	// MoveSpeed: 0以上
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	// AttackSpeed: 0.1 ～ 5.0
	else if (Attribute == GetAttackSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, 5.0f);
	}
	// Defense: 0 ～ 90（最大90%軽減）
	else if (Attribute == GetDefenseAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 90.0f);
	}
	// ReaperGauge: 0 ～ MaxReaperGauge
	else if (Attribute == GetReaperGaugeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxReaperGauge());
	}
	// MaxReaperGauge: 1以上
	else if (Attribute == GetMaxReaperGaugeAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	// DamageMultiplier: 0.1 ～ 10.0
	else if (Attribute == GetDamageMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, 10.0f);
	}
	// SpeedMultiplier: 0.1 ～ 5.0
	else if (Attribute == GetSpeedMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, 5.0f);
	}
	// DefenseBonus: 0 ～ 50
	else if (Attribute == GetDefenseBonusAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 50.0f);
	}
	// CooldownReduction: 0 ～ 50%
	else if (Attribute == GetCooldownReductionAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 50.0f);
	}
	// Luck: 0 ～ 100
	else if (Attribute == GetLuckAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}
}
