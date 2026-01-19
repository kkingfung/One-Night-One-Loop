// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightCharacter.h"
#include "Dawnlight.h"
#include "DawnlightTags.h"
#include "DawnlightAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADawnlightCharacter::ADawnlightCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ========================================================================
	// 移動設定
	// ========================================================================
	NormalMoveSpeed = 400.0f;
	ReaperModeSpeedMultiplier = 1.3f;

	// ========================================================================
	// 戦闘設定
	// ========================================================================
	BaseDamage = 25.0f;
	HeavyAttackMultiplier = 2.0f;
	SpecialAttackMultiplier = 1.5f;

	// ========================================================================
	// リーパーモード設定
	// ========================================================================
	ReaperModeDuration = 10.0f;
	ReaperDamageMultiplier = 2.0f;
	ReaperAttackSpeedMultiplier = 1.5f;

	// ========================================================================
	// HP設定
	// ========================================================================
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	ReaperGauge = 0.0f;

	// ========================================================================
	// 状態初期化
	// ========================================================================
	bIsInReaperMode = false;
	bIsAttacking = false;
	bIsDead = false;

	// ========================================================================
	// カメラ設定（デフォルト値 - BPで調整可能）
	// ========================================================================
	CameraHeight = 1200.0f;
	CameraPitch = -60.0f;
	bEnableCameraLag = true;
	CameraLagSpeed = 10.0f;

	// カメラブームを作成
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CameraHeight;
	CameraBoom->SetRelativeRotation(FRotator(CameraPitch, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	// カメララグ設定（滑らかな追従）
	CameraBoom->bEnableCameraLag = bEnableCameraLag;
	CameraBoom->CameraLagSpeed = CameraLagSpeed;

	// トップダウンカメラを作成
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false;

	// アビリティシステムコンポーネントを作成
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false); // シングルプレイヤー

	// 属性セットを作成
	AttributeSet = CreateDefaultSubobject<UDawnlightAttributeSet>(TEXT("AttributeSet"));

	// ========================================================================
	// CharacterMovement設定
	// ========================================================================
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		MovementComp->MaxWalkSpeed = NormalMoveSpeed;

		// 地上歩行のみ
		MovementComp->SetMovementMode(MOVE_Walking);
		MovementComp->DefaultLandMovementMode = MOVE_Walking;
		MovementComp->NavAgentProps.bCanFly = false;
		MovementComp->NavAgentProps.bCanJump = false;
		MovementComp->GravityScale = 1.0f;
	}

	// コントローラーの回転を使わない（トップダウンカメラ用）
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void ADawnlightCharacter::BeginPlay()
{
	Super::BeginPlay();

	// HPを最大値にリセット
	CurrentHealth = MaxHealth;

	UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: BeginPlay - HP: %f/%f"), CurrentHealth, MaxHealth);
}

void ADawnlightCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 状態に応じた移動速度の更新
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = GetCurrentMoveSpeed();
	}
}

void ADawnlightCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// GASを初期化
	InitializeAbilitySystem();
}

UAbilitySystemComponent* ADawnlightCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// ============================================================================
// 移動
// ============================================================================

void ADawnlightCharacter::HandleMoveInput(const FVector2D& MovementVector)
{
	// 死亡中/攻撃中は移動不可
	if (bIsDead || bIsAttacking)
	{
		return;
	}

	// 入力がない場合は早期リターン
	if (MovementVector.IsNearlyZero())
	{
		return;
	}

	// トップダウンビュー用の移動方向を計算
	// W（画面上）→ +X, S（画面下）→ -X
	// D（画面右）→ +Y, A（画面左）→ -Y
	const FVector MoveDirection = FVector(MovementVector.Y, MovementVector.X, 0.0f).GetSafeNormal();

	AddMovementInput(MoveDirection, 1.0f, false);
}

float ADawnlightCharacter::GetCurrentMoveSpeed() const
{
	if (bIsDead || bIsAttacking)
	{
		return 0.0f;
	}

	float Speed = NormalMoveSpeed;

	// リーパーモード中は速度アップ
	if (bIsInReaperMode)
	{
		Speed *= ReaperModeSpeedMultiplier;
	}

	return Speed;
}

// ============================================================================
// 戦闘
// ============================================================================

void ADawnlightCharacter::PerformLightAttack()
{
	if (bIsDead || bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;

	// モンタージュ再生（設定されていれば）
	if (LightAttackMontage)
	{
		const float PlayRate = bIsInReaperMode ? ReaperAttackSpeedMultiplier : 1.0f;
		PlayAnimMontage(LightAttackMontage, PlayRate);
	}

	UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: Light Attack performed"));

	// TODO: AnimNotifyでbIsAttacking = falseにする
	// 暫定: タイマーで攻撃終了
	FTimerHandle AttackTimerHandle;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, [this]()
	{
		bIsAttacking = false;
	}, 0.5f, false);
}

void ADawnlightCharacter::PerformHeavyAttack()
{
	if (bIsDead || bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;

	// モンタージュ再生（設定されていれば）
	if (HeavyAttackMontage)
	{
		const float PlayRate = bIsInReaperMode ? ReaperAttackSpeedMultiplier : 1.0f;
		PlayAnimMontage(HeavyAttackMontage, PlayRate);
	}

	UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: Heavy Attack performed"));

	// 暫定: タイマーで攻撃終了
	FTimerHandle AttackTimerHandle;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, [this]()
	{
		bIsAttacking = false;
	}, 0.8f, false);
}

void ADawnlightCharacter::PerformSpecialAttack()
{
	if (bIsDead || bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;

	// モンタージュ再生（設定されていれば）
	if (SpecialAttackMontage)
	{
		const float PlayRate = bIsInReaperMode ? ReaperAttackSpeedMultiplier : 1.0f;
		PlayAnimMontage(SpecialAttackMontage, PlayRate);
	}

	UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: Special Attack performed"));

	// 暫定: タイマーで攻撃終了
	FTimerHandle AttackTimerHandle;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, [this]()
	{
		bIsAttacking = false;
	}, 0.7f, false);
}

bool ADawnlightCharacter::IsAttacking() const
{
	return bIsAttacking;
}

// ============================================================================
// リーパーモード
// ============================================================================

void ADawnlightCharacter::ActivateReaperMode()
{
	if (!CanActivateReaperMode())
	{
		return;
	}

	bIsInReaperMode = true;
	ReaperGauge = 0.0f; // ゲージをリセット

	// 発動アニメーション再生
	if (ReaperActivationMontage)
	{
		PlayAnimMontage(ReaperActivationMontage);
	}

	// タイマー設定（持続時間後に終了）
	GetWorldTimerManager().SetTimer(
		ReaperModeTimerHandle,
		this,
		&ADawnlightCharacter::DeactivateReaperMode,
		ReaperModeDuration,
		false
	);

	// デリゲート発火
	OnReaperModeActivated.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: REAPER MODE ACTIVATED! Duration: %f seconds"), ReaperModeDuration);
}

void ADawnlightCharacter::DeactivateReaperMode()
{
	if (!bIsInReaperMode)
	{
		return;
	}

	bIsInReaperMode = false;

	// タイマーをクリア
	GetWorldTimerManager().ClearTimer(ReaperModeTimerHandle);

	// デリゲート発火
	OnReaperModeDeactivated.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: Reaper Mode deactivated"));
}

bool ADawnlightCharacter::IsInReaperMode() const
{
	return bIsInReaperMode;
}

float ADawnlightCharacter::GetReaperGaugePercent() const
{
	return FMath::Clamp(ReaperGauge / 100.0f, 0.0f, 1.0f);
}

bool ADawnlightCharacter::CanActivateReaperMode() const
{
	return !bIsDead && !bIsInReaperMode && ReaperGauge >= 100.0f;
}

void ADawnlightCharacter::AddReaperGauge(float Amount)
{
	// リーパーモード中はゲージを増やさない
	if (bIsInReaperMode || bIsDead)
	{
		return;
	}

	const float OldGauge = ReaperGauge;
	ReaperGauge = FMath::Clamp(ReaperGauge + Amount, 0.0f, 100.0f);

	UE_LOG(LogDawnlight, Verbose, TEXT("SoulReaper: リーパーゲージ追加: %.1f → %.1f / 100"), OldGauge, ReaperGauge);

	// ゲージが満タンになったらログ出力
	if (OldGauge < 100.0f && ReaperGauge >= 100.0f)
	{
		UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: リーパーゲージ満タン！スペースキーで発動可能"));
	}
}

// ============================================================================
// ダメージ
// ============================================================================

void ADawnlightCharacter::TakeDamageAmount(float DamageAmount)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: Took %f damage. HP: %f/%f"), DamageAmount, CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		HandleDeath();
	}
}

bool ADawnlightCharacter::IsDead() const
{
	return bIsDead;
}

float ADawnlightCharacter::GetHealthPercent() const
{
	if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}
	return FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
}

void ADawnlightCharacter::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	// リーパーモードを強制終了
	DeactivateReaperMode();

	// デリゲート発火
	OnPlayerDeath.Broadcast();

	UE_LOG(LogDawnlight, Warning, TEXT("SoulReaper: PLAYER DIED!"));
}

// ============================================================================
// 内部関数
// ============================================================================

void ADawnlightCharacter::InitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// 属性セットをアビリティシステムに登録
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	UE_LOG(LogDawnlight, Log, TEXT("SoulReaper: アビリティシステムを初期化しました"));
}

FGameplayTagContainer ADawnlightCharacter::GetCurrentTags() const
{
	FGameplayTagContainer OwnedTags;
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
	}
	return OwnedTags;
}
