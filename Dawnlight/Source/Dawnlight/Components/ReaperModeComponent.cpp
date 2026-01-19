// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReaperModeComponent.h"
#include "Dawnlight.h"
#include "Abilities/DawnlightAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"

UReaperModeComponent::UReaperModeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// デフォルト値
	ReaperModeDuration = 10.0f;
	ReaperDamageMultiplier = 2.0f;
	ReaperSpeedMultiplier = 1.3f;
	bIsReaperModeActive = false;
	OriginalDamageMultiplier = 1.0f;
	OriginalSpeedMultiplier = 1.0f;
}

void UReaperModeComponent::BeginPlay()
{
	Super::BeginPlay();

	// AttributeSetとASCをキャッシュ
	if (AActor* Owner = GetOwner())
	{
		CachedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
		if (CachedASC.IsValid())
		{
			// GetAttributeSetはconst UAttributeSet*を返すため、const_castが必要
			const UAttributeSet* AttributeSet = CachedASC->GetAttributeSet(UDawnlightAttributeSet::StaticClass());
			CachedAttributeSet = const_cast<UDawnlightAttributeSet*>(Cast<const UDawnlightAttributeSet>(AttributeSet));
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[ReaperModeComponent] 初期化完了"));
}

void UReaperModeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// タイマーをクリア
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	// エフェクトを停止
	if (ActiveEffectComponent)
	{
		ActiveEffectComponent->DestroyComponent();
		ActiveEffectComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

UDawnlightAttributeSet* UReaperModeComponent::GetAttributeSet() const
{
	return CachedAttributeSet.Get();
}

UAbilitySystemComponent* UReaperModeComponent::GetASC() const
{
	return CachedASC.Get();
}

bool UReaperModeComponent::CanActivateReaperMode() const
{
	if (bIsReaperModeActive)
	{
		return false;
	}

	if (UDawnlightAttributeSet* AttributeSet = GetAttributeSet())
	{
		return AttributeSet->CanActivateReaperMode();
	}

	return false;
}

bool UReaperModeComponent::ActivateReaperMode()
{
	if (!CanActivateReaperMode())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[ReaperModeComponent] リーパーモードを発動できません（ゲージ不足または発動中）"));
		return false;
	}

	bIsReaperModeActive = true;

	UE_LOG(LogDawnlight, Log, TEXT("[ReaperModeComponent] リーパーモード発動！（持続時間: %.1f秒）"), ReaperModeDuration);

	// バフを適用
	ApplyReaperBuffs();

	// ゲージをリセット
	ResetReaperGauge();

	// 発動エフェクト
	if (ActivationEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ActivationEffect,
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorRotation()
		);
	}

	// 常時エフェクト開始
	if (ActiveEffect)
	{
		ActiveEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			ActiveEffect,
			Cast<ACharacter>(GetOwner())->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}

	// 持続時間タイマー開始
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DurationTimerHandle,
			this,
			&UReaperModeComponent::OnReaperModeDurationEnd,
			ReaperModeDuration,
			false
		);
	}

	// イベント発火
	OnReaperModeActivated.Broadcast();

	return true;
}

void UReaperModeComponent::DeactivateReaperMode()
{
	if (!bIsReaperModeActive)
	{
		return;
	}

	bIsReaperModeActive = false;

	UE_LOG(LogDawnlight, Log, TEXT("[ReaperModeComponent] リーパーモード終了"));

	// タイマーをクリア
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	// バフを解除
	RemoveReaperBuffs();

	// エフェクトを停止
	if (ActiveEffectComponent)
	{
		ActiveEffectComponent->DestroyComponent();
		ActiveEffectComponent = nullptr;
	}

	// イベント発火
	OnReaperModeDeactivated.Broadcast();
}

void UReaperModeComponent::OnReaperModeDurationEnd()
{
	DeactivateReaperMode();
}

void UReaperModeComponent::ApplyReaperBuffs()
{
	UDawnlightAttributeSet* AttributeSet = GetAttributeSet();
	if (!AttributeSet)
	{
		return;
	}

	// 現在の値を保存
	OriginalDamageMultiplier = AttributeSet->GetDamageMultiplier();
	OriginalSpeedMultiplier = AttributeSet->GetSpeedMultiplier();

	// リーパーモードのバフを適用
	AttributeSet->SetDamageMultiplier(OriginalDamageMultiplier * ReaperDamageMultiplier);
	AttributeSet->SetSpeedMultiplier(OriginalSpeedMultiplier * ReaperSpeedMultiplier);

	UE_LOG(LogDawnlight, Log, TEXT("[ReaperModeComponent] バフ適用: ダメージ %.1fx → %.1fx, 速度 %.1fx → %.1fx"),
		OriginalDamageMultiplier, AttributeSet->GetDamageMultiplier(),
		OriginalSpeedMultiplier, AttributeSet->GetSpeedMultiplier());
}

void UReaperModeComponent::RemoveReaperBuffs()
{
	UDawnlightAttributeSet* AttributeSet = GetAttributeSet();
	if (!AttributeSet)
	{
		return;
	}

	// 元の値に復元
	AttributeSet->SetDamageMultiplier(OriginalDamageMultiplier);
	AttributeSet->SetSpeedMultiplier(OriginalSpeedMultiplier);

	UE_LOG(LogDawnlight, Log, TEXT("[ReaperModeComponent] バフ解除: ダメージ %.1fx, 速度 %.1fx"),
		OriginalDamageMultiplier, OriginalSpeedMultiplier);
}

void UReaperModeComponent::AddReaperGauge(float Amount)
{
	if (bIsReaperModeActive)
	{
		// リーパーモード中はゲージを増やさない
		return;
	}

	UDawnlightAttributeSet* AttributeSet = GetAttributeSet();
	if (!AttributeSet)
	{
		return;
	}

	const float CurrentValue = AttributeSet->GetReaperGauge();
	const float MaxValue = AttributeSet->GetMaxReaperGauge();
	const float NewValue = FMath::Clamp(CurrentValue + Amount, 0.0f, MaxValue);

	AttributeSet->SetReaperGauge(NewValue);

	UE_LOG(LogDawnlight, Verbose, TEXT("[ReaperModeComponent] ゲージ追加: %.0f → %.0f / %.0f"),
		CurrentValue, NewValue, MaxValue);

	// イベント発火
	OnReaperGaugeChanged.Broadcast(NewValue, MaxValue);
}

float UReaperModeComponent::GetReaperGaugePercent() const
{
	UDawnlightAttributeSet* AttributeSet = GetAttributeSet();
	if (!AttributeSet)
	{
		return 0.0f;
	}

	return AttributeSet->GetReaperGaugePercent();
}

void UReaperModeComponent::ResetReaperGauge()
{
	UDawnlightAttributeSet* AttributeSet = GetAttributeSet();
	if (!AttributeSet)
	{
		return;
	}

	AttributeSet->SetReaperGauge(0.0f);

	UE_LOG(LogDawnlight, Log, TEXT("[ReaperModeComponent] ゲージリセット"));

	// イベント発火
	OnReaperGaugeChanged.Broadcast(0.0f, AttributeSet->GetMaxReaperGauge());
}
