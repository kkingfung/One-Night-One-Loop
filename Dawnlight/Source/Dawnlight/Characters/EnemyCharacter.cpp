// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnemyCharacter.h"
#include "Dawnlight.h"
#include "Data/EnemyDataAsset.h"
#include "Characters/DawnlightCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// デフォルト値
	CurrentHealth = 50.0f;
	MaxHealth = 50.0f;
	BehaviorState = EEnemyBehaviorState::Idle;
	ChaseSpeed = 300.0f;
	DetectionRadius = 1000.0f;
	AttackRange = 150.0f;
	AttackCooldown = 1.5f;
	AttackDamage = 10.0f;
	bIsAttackOnCooldown = false;

	// キャラクター移動設定
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = ChaseSpeed;
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	}

	// プレイヤーのコントローラー回転を使用しない
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// EnemyDataからパラメータを初期化
	InitializeFromEnemyData();

	// プレイヤーをキャッシュ
	CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

	// 追跡状態で開始
	BehaviorState = EEnemyBehaviorState::Chasing;

	UE_LOG(LogDawnlight, Log, TEXT("[EnemyCharacter] %s がスポーン HP: %.0f"), *GetName(), CurrentHealth);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive())
	{
		return;
	}

	// 行動状態を更新
	UpdateBehaviorState();

	// 状態に応じた処理
	switch (BehaviorState)
	{
	case EEnemyBehaviorState::Chasing:
		ProcessChasing(DeltaTime);
		break;

	case EEnemyBehaviorState::Attacking:
		ProcessAttacking(DeltaTime);
		break;

	case EEnemyBehaviorState::Stunned:
		// スタン中は何もしない
		break;

	default:
		break;
	}
}

void AEnemyCharacter::InitializeFromEnemyData()
{
	if (!EnemyData)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[EnemyCharacter] %s: EnemyDataが設定されていません"), *GetName());
		return;
	}

	// EnemyDataからパラメータを取得
	MaxHealth = EnemyData->MaxHealth;
	CurrentHealth = MaxHealth;
	AttackDamage = EnemyData->AttackDamage;
	ChaseSpeed = EnemyData->MoveSpeed;
	AttackCooldown = EnemyData->AttackCooldown;
	AttackRange = EnemyData->AttackRange;
	DetectionRadius = EnemyData->DetectionRadius;

	// 移動速度を設定
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = ChaseSpeed;
	}

	// 死亡エフェクトを設定
	DeathEffect = EnemyData->DeathEffect;

	UE_LOG(LogDawnlight, Log, TEXT("[EnemyCharacter] %s: EnemyData '%s' から初期化 HP: %.0f, Damage: %.0f"),
		*GetName(), *EnemyData->DisplayName.ToString(), CurrentHealth, AttackDamage);
}

void AEnemyCharacter::UpdateBehaviorState()
{
	if (BehaviorState == EEnemyBehaviorState::Dead || BehaviorState == EEnemyBehaviorState::Stunned)
	{
		return;
	}

	const float DistanceToPlayer = GetDistanceToPlayer();

	// 攻撃範囲内にいる場合
	if (DistanceToPlayer <= AttackRange)
	{
		if (BehaviorState != EEnemyBehaviorState::Attacking)
		{
			BehaviorState = EEnemyBehaviorState::Attacking;
			UE_LOG(LogDawnlight, Verbose, TEXT("[EnemyCharacter] %s: 攻撃状態に移行"), *GetName());
		}
	}
	// 検知範囲内の場合は追跡
	else if (DistanceToPlayer <= DetectionRadius)
	{
		if (BehaviorState != EEnemyBehaviorState::Chasing)
		{
			BehaviorState = EEnemyBehaviorState::Chasing;
			UE_LOG(LogDawnlight, Verbose, TEXT("[EnemyCharacter] %s: 追跡状態に移行"), *GetName());
		}
	}
	// 検知範囲外の場合は待機（ただし通常は常に追跡）
	else
	{
		// Dawn Phaseでは常にプレイヤーを追跡するため、実質的にはここには来ない
		BehaviorState = EEnemyBehaviorState::Chasing;
	}
}

void AEnemyCharacter::ProcessChasing(float DeltaTime)
{
	const FVector Direction = GetDirectionToPlayer();

	if (!Direction.IsNearlyZero())
	{
		AddMovementInput(Direction, 1.0f);
	}
}

void AEnemyCharacter::ProcessAttacking(float DeltaTime)
{
	// 攻撃可能なら攻撃
	if (CanAttack())
	{
		PerformAttack();
	}

	// 攻撃中でも追跡を続ける（設定による）
	if (EnemyData && !EnemyData->bStopWhileAttacking)
	{
		const FVector Direction = GetDirectionToPlayer();
		if (!Direction.IsNearlyZero())
		{
			AddMovementInput(Direction, 0.5f);  // 攻撃中は速度半減
		}
	}
}

float AEnemyCharacter::GetDistanceToPlayer() const
{
	if (!CachedPlayer.IsValid())
	{
		return MAX_FLT;
	}

	return FVector::Dist(GetActorLocation(), CachedPlayer->GetActorLocation());
}

FVector AEnemyCharacter::GetDirectionToPlayer() const
{
	if (!CachedPlayer.IsValid())
	{
		return FVector::ZeroVector;
	}

	FVector Direction = CachedPlayer->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();

	return Direction;
}

bool AEnemyCharacter::CanAttack() const
{
	return !bIsAttackOnCooldown && IsAlive();
}

void AEnemyCharacter::PerformAttack()
{
	if (!CanAttack())
	{
		return;
	}

	UE_LOG(LogDawnlight, Log, TEXT("[EnemyCharacter] %s: 攻撃実行 (Damage: %.0f)"), *GetName(), AttackDamage);

	// クールダウン開始
	bIsAttackOnCooldown = true;
	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		this,
		&AEnemyCharacter::OnAttackCooldownEnd,
		AttackCooldown,
		false
	);

	// プレイヤーにダメージを与える
	if (CachedPlayer.IsValid())
	{
		if (ADawnlightCharacter* PlayerChar = Cast<ADawnlightCharacter>(CachedPlayer.Get()))
		{
			// プレイヤーがまだ攻撃範囲内にいるか確認
			if (GetDistanceToPlayer() <= AttackRange * 1.2f)  // 少し余裕を持たせる
			{
				// プレイヤーにダメージを与える
				PlayerChar->TakeDamageAmount(AttackDamage);
				UE_LOG(LogDawnlight, Log, TEXT("[EnemyCharacter] %s: プレイヤーに %.0f ダメージを与えた"), *GetName(), AttackDamage);
			}
		}
	}

	// 攻撃イベント（Blueprint実装可能）
	OnAttack();
}

void AEnemyCharacter::OnAttackCooldownEnd()
{
	bIsAttackOnCooldown = false;
}

void AEnemyCharacter::TakeDamageFromPlayer(float DamageAmount, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	UE_LOG(LogDawnlight, Log, TEXT("[EnemyCharacter] %s がダメージを受けた: %.0f (残りHP: %.0f)"),
		*GetName(), DamageAmount, CurrentHealth);

	// ヒットエフェクト
	if (HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			HitEffect,
			GetActorLocation() + FVector(0.0f, 0.0f, 50.0f),
			FRotator::ZeroRotator
		);
	}

	// ダメージイベント
	OnDamageTaken(DamageAmount, CurrentHealth);

	// 死亡判定
	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void AEnemyCharacter::Die()
{
	if (BehaviorState == EEnemyBehaviorState::Dead)
	{
		return;
	}

	BehaviorState = EEnemyBehaviorState::Dead;

	UE_LOG(LogDawnlight, Log, TEXT("[EnemyCharacter] %s が死亡"), *GetName());

	// 移動を停止
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	// タイマーを停止
	GetWorld()->GetTimerManager().ClearTimer(AttackCooldownTimerHandle);

	// 死亡エフェクト
	if (DeathEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			DeathEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// C++デリゲートを発火（WaveSpawner等で使用）
	OnEnemyDeathDelegate.Broadcast(this);

	// 死亡イベント（Blueprint実装可能）
	OnDeath();

	// 少し待ってから削除
	SetLifeSpan(2.0f);
}

float AEnemyCharacter::GetHealthPercent() const
{
	if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}

	return CurrentHealth / MaxHealth;
}
