// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimalCharacter.h"
#include "Dawnlight.h"
#include "Data/SoulDataAsset.h"
#include "Subsystems/SoulCollectionSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AAnimalCharacter::AAnimalCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// デフォルト値
	CurrentHealth = 100.0f;
	BehaviorState = EAnimalBehaviorState::Idle;
	WanderSpeed = 100.0f;
	FleeSpeed = 400.0f;
	DetectionRadius = 600.0f;
	FleeRadius = 400.0f;
	WanderInterval = 3.0f;
	WanderRadius = 500.0f;

	// キャラクター移動設定
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = WanderSpeed;
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	}

	// プレイヤーのコントローラー回転を使用しない
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

void AAnimalCharacter::BeginPlay()
{
	Super::BeginPlay();

	// スポーン位置を記録
	SpawnLocation = GetActorLocation();

	// SoulDataからパラメータを初期化
	InitializeFromSoulData();

	// プレイヤーをキャッシュ
	CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

	// 最初の徘徊目的地を設定
	SetNewWanderTarget();

	// 徘徊タイマーを開始
	GetWorld()->GetTimerManager().SetTimer(
		WanderTimerHandle,
		this,
		&AAnimalCharacter::SetNewWanderTarget,
		WanderInterval,
		true
	);

	// 徘徊状態で開始
	BehaviorState = EAnimalBehaviorState::Wandering;

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalCharacter] %s がスポーン HP: %.0f"), *GetName(), CurrentHealth);
}

void AAnimalCharacter::Tick(float DeltaTime)
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
	case EAnimalBehaviorState::Wandering:
		ProcessWandering(DeltaTime);
		break;

	case EAnimalBehaviorState::Fleeing:
		ProcessFleeing(DeltaTime);
		break;

	case EAnimalBehaviorState::Stunned:
		// スタン中は何もしない
		break;

	default:
		break;
	}
}

void AAnimalCharacter::InitializeFromSoulData()
{
	if (!SoulData)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[AnimalCharacter] %s: SoulDataが設定されていません"), *GetName());
		return;
	}

	// SoulDataからパラメータを取得
	CurrentHealth = SoulData->AnimalHealth;
	FleeRadius = SoulData->FleeDistance;

	// 移動速度を設定
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		WanderSpeed = SoulData->AnimalMoveSpeed * 0.5f;  // 徘徊時は半分の速度
		FleeSpeed = SoulData->AnimalMoveSpeed;
		Movement->MaxWalkSpeed = WanderSpeed;
	}

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalCharacter] %s: SoulData '%s' から初期化 HP: %.0f, FleeRadius: %.0f"),
		*GetName(), *SoulData->DisplayName.ToString(), CurrentHealth, FleeRadius);
}

void AAnimalCharacter::UpdateBehaviorState()
{
	if (BehaviorState == EAnimalBehaviorState::Dead || BehaviorState == EAnimalBehaviorState::Stunned)
	{
		return;
	}

	const float DistanceToPlayer = GetDistanceToPlayer();

	// プレイヤーが逃走距離内にいる場合
	if (DistanceToPlayer <= FleeRadius)
	{
		if (BehaviorState != EAnimalBehaviorState::Fleeing)
		{
			BehaviorState = EAnimalBehaviorState::Fleeing;

			// 逃走速度に変更
			if (UCharacterMovementComponent* Movement = GetCharacterMovement())
			{
				Movement->MaxWalkSpeed = FleeSpeed;
			}

			OnStartFleeing();
			UE_LOG(LogDawnlight, Verbose, TEXT("[AnimalCharacter] %s: 逃走開始"), *GetName());
		}
	}
	// プレイヤーが検知距離外に出た場合、徘徊に戻る
	else if (DistanceToPlayer > DetectionRadius)
	{
		if (BehaviorState == EAnimalBehaviorState::Fleeing)
		{
			BehaviorState = EAnimalBehaviorState::Wandering;

			// 徘徊速度に戻す
			if (UCharacterMovementComponent* Movement = GetCharacterMovement())
			{
				Movement->MaxWalkSpeed = WanderSpeed;
			}

			UE_LOG(LogDawnlight, Verbose, TEXT("[AnimalCharacter] %s: 徘徊に戻る"), *GetName());
		}
	}
}

void AAnimalCharacter::ProcessWandering(float DeltaTime)
{
	// 目的地への方向
	const FVector CurrentLocation = GetActorLocation();
	FVector Direction = CurrentWanderTarget - CurrentLocation;
	Direction.Z = 0.0f;

	const float DistanceToTarget = Direction.Size();

	// 目的地に到達したら待機
	if (DistanceToTarget < 50.0f)
	{
		AddMovementInput(FVector::ZeroVector);
		return;
	}

	// 目的地に向かって移動
	Direction.Normalize();
	AddMovementInput(Direction, 1.0f);
}

void AAnimalCharacter::ProcessFleeing(float DeltaTime)
{
	const FVector FleeDirection = GetFleeDirection();

	if (!FleeDirection.IsNearlyZero())
	{
		AddMovementInput(FleeDirection, 1.0f);
	}
}

void AAnimalCharacter::SetNewWanderTarget()
{
	if (BehaviorState == EAnimalBehaviorState::Fleeing || !IsAlive())
	{
		return;
	}

	// スポーン位置を中心にランダムな位置を選択
	const float RandomAngle = FMath::FRandRange(0.0f, 360.0f);
	const float RandomDistance = FMath::FRandRange(WanderRadius * 0.3f, WanderRadius);

	CurrentWanderTarget = SpawnLocation + FVector(
		FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
		FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
		0.0f
	);
}

float AAnimalCharacter::GetDistanceToPlayer() const
{
	if (!CachedPlayer.IsValid())
	{
		return MAX_FLT;
	}

	return FVector::Dist(GetActorLocation(), CachedPlayer->GetActorLocation());
}

FVector AAnimalCharacter::GetFleeDirection() const
{
	if (!CachedPlayer.IsValid())
	{
		return FVector::ZeroVector;
	}

	FVector Direction = GetActorLocation() - CachedPlayer->GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();

	return Direction;
}

void AAnimalCharacter::TakeDamageFromPlayer(float DamageAmount, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalCharacter] %s がダメージを受けた: %.0f (残りHP: %.0f)"),
		*GetName(), DamageAmount, CurrentHealth);

	// ダメージイベント
	OnDamageTaken(DamageAmount, CurrentHealth);

	// 死亡判定
	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
	else
	{
		// ダメージを受けたら即座に逃走
		if (BehaviorState != EAnimalBehaviorState::Fleeing)
		{
			BehaviorState = EAnimalBehaviorState::Fleeing;

			if (UCharacterMovementComponent* Movement = GetCharacterMovement())
			{
				Movement->MaxWalkSpeed = FleeSpeed;
			}

			OnStartFleeing();
		}
	}
}

void AAnimalCharacter::Die()
{
	if (BehaviorState == EAnimalBehaviorState::Dead)
	{
		return;
	}

	BehaviorState = EAnimalBehaviorState::Dead;

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalCharacter] %s が死亡"), *GetName());

	// 移動を停止
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	// タイマーを停止
	GetWorld()->GetTimerManager().ClearTimer(WanderTimerHandle);

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

	// 魂をドロップ
	DropSoul();

	// 死亡イベント（Blueprint実装可能）
	OnDeath();

	// 少し待ってから削除
	SetLifeSpan(2.0f);
}

void AAnimalCharacter::DropSoul()
{
	if (!SoulData)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[AnimalCharacter] %s: SoulDataが設定されていないため、魂をドロップできません"), *GetName());
		return;
	}

	// SoulCollectionSubsystemに魂を追加
	if (UWorld* World = GetWorld())
	{
		if (USoulCollectionSubsystem* SoulSystem = World->GetSubsystem<USoulCollectionSubsystem>())
		{
			SoulSystem->CollectSoul(SoulData->SoulTag, GetActorLocation());

			UE_LOG(LogDawnlight, Log, TEXT("[AnimalCharacter] %s: 魂 '%s' をドロップ"),
				*GetName(), *SoulData->DisplayName.ToString());

			// 魂放出エフェクト
			if (SoulReleaseEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					this,
					SoulReleaseEffect,
					GetActorLocation() + FVector(0.0f, 0.0f, 50.0f),
					FRotator::ZeroRotator
				);
			}
		}
	}
}

float AAnimalCharacter::GetHealthPercent() const
{
	if (!SoulData || SoulData->AnimalHealth <= 0.0f)
	{
		return 0.0f;
	}

	return CurrentHealth / SoulData->AnimalHealth;
}
