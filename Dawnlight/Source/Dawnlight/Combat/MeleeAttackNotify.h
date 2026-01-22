// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MeleeAttackNotify.generated.h"

/**
 * 近接攻撃判定用AnimNotifyState
 *
 * アニメーション中の特定フレーム間で攻撃判定を行う
 * - Sphere Traceで範囲攻撃判定
 * - 複数ヒット防止のためのヒット済みアクター管理
 * - GASと連携してダメージ適用
 */
UCLASS(meta = (DisplayName = "Melee Attack Window"))
class DAWNLIGHT_API UMeleeAttackNotify : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UMeleeAttackNotify();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

protected:
	/** 攻撃範囲の半径 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "10"))
	float AttackRadius = 100.0f;

	/** 攻撃範囲のオフセット（キャラクター前方） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float ForwardOffset = 100.0f;

	/** 基本ダメージ値 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "0"))
	float BaseDamage = 10.0f;

	/** ダメージ倍率（重攻撃用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = "0.1"))
	float DamageMultiplier = 1.0f;

	/** ヒット時にノックバックを与えるか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	bool bApplyKnockback = true;

	/** ノックバック力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (EditCondition = "bApplyKnockback", ClampMin = "0"))
	float KnockbackForce = 500.0f;

	/** デバッグ表示 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebug = false;

private:
	/** この攻撃でヒットしたアクター（重複ヒット防止） */
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitActors;

	/** 攻撃判定を実行 */
	void PerformAttackTrace(AActor* Attacker);

	/** ダメージを適用 */
	void ApplyDamageToTarget(AActor* Attacker, AActor* Target, const FHitResult& HitResult);
};
