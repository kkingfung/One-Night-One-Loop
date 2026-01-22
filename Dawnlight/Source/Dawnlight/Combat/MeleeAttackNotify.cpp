// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeleeAttackNotify.h"
#include "Dawnlight.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/DawnlightAttributeSet.h"
#include "Engine/DamageEvents.h"

UMeleeAttackNotify::UMeleeAttackNotify()
{
	AttackRadius = 100.0f;
	ForwardOffset = 100.0f;
	BaseDamage = 10.0f;
	DamageMultiplier = 1.0f;
	bApplyKnockback = true;
	KnockbackForce = 500.0f;
	bShowDebug = false;
}

void UMeleeAttackNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// ヒットリストをクリア
	HitActors.Empty();

	UE_LOG(LogDawnlight, Verbose, TEXT("[MeleeAttack] 攻撃判定開始 - Radius: %.0f, Damage: %.0f x %.1f"),
		AttackRadius, BaseDamage, DamageMultiplier);
}

void UMeleeAttackNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	// 攻撃判定を実行
	PerformAttackTrace(Owner);
}

void UMeleeAttackNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogDawnlight, Verbose, TEXT("[MeleeAttack] 攻撃判定終了 - ヒット数: %d"), HitActors.Num());

	// ヒットリストをクリア
	HitActors.Empty();
}

FString UMeleeAttackNotify::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Melee Attack (%.0f dmg)"), BaseDamage * DamageMultiplier);
}

void UMeleeAttackNotify::PerformAttackTrace(AActor* Attacker)
{
	if (!Attacker)
	{
		return;
	}

	UWorld* World = Attacker->GetWorld();
	if (!World)
	{
		return;
	}

	// 攻撃位置を計算（キャラクター前方）
	FVector AttackerLocation = Attacker->GetActorLocation();
	FVector ForwardVector = Attacker->GetActorForwardVector();
	FVector TraceCenter = AttackerLocation + ForwardVector * ForwardOffset;

	// Sphere Traceでヒット判定
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Attacker);

	// 既にヒットしたアクターも無視
	for (const TWeakObjectPtr<AActor>& HitActor : HitActors)
	{
		if (HitActor.IsValid())
		{
			ActorsToIgnore.Add(HitActor.Get());
		}
	}

	// トレースチャンネル（Pawnを検出）
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Pawn);

	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		World,
		TraceCenter,
		TraceCenter, // 同じ位置（Sweep距離0）
		AttackRadius,
		TraceChannel,
		false, // bTraceComplex
		ActorsToIgnore,
		bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true, // bIgnoreSelf
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);

	if (!bHit)
	{
		return;
	}

	// ヒットしたアクターにダメージを適用
	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor || HitActor == Attacker)
		{
			continue;
		}

		// 重複チェック
		bool bAlreadyHit = false;
		for (const TWeakObjectPtr<AActor>& PrevHit : HitActors)
		{
			if (PrevHit.Get() == HitActor)
			{
				bAlreadyHit = true;
				break;
			}
		}

		if (bAlreadyHit)
		{
			continue;
		}

		// ヒットリストに追加
		HitActors.Add(HitActor);

		// ダメージを適用
		ApplyDamageToTarget(Attacker, HitActor, HitResult);

		UE_LOG(LogDawnlight, Log, TEXT("[MeleeAttack] ヒット: %s"), *HitActor->GetName());
	}
}

void UMeleeAttackNotify::ApplyDamageToTarget(AActor* Attacker, AActor* Target, const FHitResult& HitResult)
{
	if (!Attacker || !Target)
	{
		return;
	}

	// 最終ダメージ計算
	float FinalDamage = BaseDamage * DamageMultiplier;

	// 攻撃者のダメージ倍率を取得（GAS）
	if (UAbilitySystemComponent* AttackerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Attacker))
	{
		// DamageMultiplier属性を取得
		bool bFound = false;
		float AttackerDamageMultiplier = AttackerASC->GetGameplayAttributeValue(
			UDawnlightAttributeSet::GetDamageMultiplierAttribute(),
			bFound
		);

		if (bFound && AttackerDamageMultiplier > 0.0f)
		{
			FinalDamage *= AttackerDamageMultiplier;
		}
	}

	// ターゲットにダメージを適用（GAS経由）
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		// IncomingDamage属性に設定
		TargetASC->SetNumericAttributeBase(
			UDawnlightAttributeSet::GetIncomingDamageAttribute(),
			FinalDamage
		);

		UE_LOG(LogDawnlight, Log, TEXT("[MeleeAttack] GASダメージ適用: %.0f → %s"),
			FinalDamage, *Target->GetName());
	}
	else
	{
		// GASがない場合はUE標準のダメージシステムを使用
		FDamageEvent DamageEvent;
		Target->TakeDamage(FinalDamage, DamageEvent, Attacker->GetInstigatorController(), Attacker);

		UE_LOG(LogDawnlight, Log, TEXT("[MeleeAttack] 標準ダメージ適用: %.0f → %s"),
			FinalDamage, *Target->GetName());
	}

	// ノックバック
	if (bApplyKnockback && KnockbackForce > 0.0f)
	{
		FVector KnockbackDirection = (Target->GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal();
		KnockbackDirection.Z = 0.3f; // 少し上向きに
		KnockbackDirection.Normalize();

		if (ACharacter* TargetCharacter = Cast<ACharacter>(Target))
		{
			TargetCharacter->LaunchCharacter(KnockbackDirection * KnockbackForce, true, true);
		}
	}
}
