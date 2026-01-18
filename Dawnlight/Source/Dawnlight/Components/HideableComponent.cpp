// Copyright Epic Games, Inc. All Rights Reserved.

#include "HideableComponent.h"
#include "Dawnlight.h"
#include "DawnlightCharacter.h"
#include "DawnlightTags.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UHideableComponent::UHideableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// デフォルト設定
	MaxOccupants = 1;
	HideTransitionTime = 0.5f;
	UnhideTransitionTime = 0.3f;
	HideLocationOffset = FVector::ZeroVector;
	HiddenRotation = FRotator::ZeroRotator;
	DiscoveryChance = 0.3f;
	bIsPerfectHideSpot = false;
	InteractionRadius = 150.0f;

	// 状態初期化
	bIsAvailable = true;
}

void UHideableComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogDawnlight, Log, TEXT("HideableComponent: %s で開始しました（最大収容: %d人）"),
		*GetOwner()->GetName(), MaxOccupants);
}

bool UHideableComponent::HidePlayer(ADawnlightCharacter* Player)
{
	if (!Player)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("HideableComponent: 無効なプレイヤー"));
		return false;
	}

	if (!CanHide())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("HideableComponent: 隠れることができません（満員または無効）"));
		return false;
	}

	if (IsPlayerHidden(Player))
	{
		UE_LOG(LogDawnlight, Warning, TEXT("HideableComponent: プレイヤーは既に隠れています"));
		return false;
	}

	// プレイヤーを隠れ位置に移動
	MovePlayerToHideLocation(Player);

	// 隠れ状態を適用
	ApplyHiddenState(Player);

	// リストに追加
	HiddenPlayers.Add(Player);

	UE_LOG(LogDawnlight, Log, TEXT("HideableComponent: %s が %s に隠れました"),
		*Player->GetName(), *GetOwner()->GetName());

	OnPlayerHidden.Broadcast(Player);

	return true;
}

bool UHideableComponent::UnhidePlayer(ADawnlightCharacter* Player)
{
	if (!Player)
	{
		return false;
	}

	if (!IsPlayerHidden(Player))
	{
		UE_LOG(LogDawnlight, Warning, TEXT("HideableComponent: プレイヤーは隠れていません"));
		return false;
	}

	// リストから削除
	HiddenPlayers.RemoveAll([Player](const TWeakObjectPtr<ADawnlightCharacter>& WeakPlayer)
	{
		return WeakPlayer.Get() == Player;
	});

	// 隠れ状態を解除
	RemoveHiddenState(Player);

	// プレイヤーを元の位置に戻す
	RestorePlayerPosition(Player);

	UE_LOG(LogDawnlight, Log, TEXT("HideableComponent: %s が %s から出ました"),
		*Player->GetName(), *GetOwner()->GetName());

	OnPlayerUnhidden.Broadcast(Player);

	return true;
}

void UHideableComponent::ForceUnhideAll()
{
	// コピーを作成してイテレート（元配列が変更されるため）
	TArray<TWeakObjectPtr<ADawnlightCharacter>> PlayersCopy = HiddenPlayers;

	for (const TWeakObjectPtr<ADawnlightCharacter>& WeakPlayer : PlayersCopy)
	{
		if (ADawnlightCharacter* Player = WeakPlayer.Get())
		{
			UnhidePlayer(Player);
		}
	}

	HiddenPlayers.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("HideableComponent: %s から全員が強制退去しました"),
		*GetOwner()->GetName());
}

bool UHideableComponent::CanHide() const
{
	// 使用不可
	if (!bIsAvailable)
	{
		return false;
	}

	// 満員
	if (HiddenPlayers.Num() >= MaxOccupants)
	{
		return false;
	}

	return true;
}

bool UHideableComponent::IsPlayerHidden(ADawnlightCharacter* Player) const
{
	if (!Player)
	{
		return false;
	}

	for (const TWeakObjectPtr<ADawnlightCharacter>& WeakPlayer : HiddenPlayers)
	{
		if (WeakPlayer.Get() == Player)
		{
			return true;
		}
	}

	return false;
}

FVector UHideableComponent::GetHideLocation() const
{
	if (AActor* Owner = GetOwner())
	{
		return Owner->GetActorLocation() + Owner->GetActorRotation().RotateVector(HideLocationOffset);
	}
	return FVector::ZeroVector;
}

void UHideableComponent::DisableHideSpot()
{
	if (!bIsAvailable)
	{
		return;
	}

	bIsAvailable = false;

	// 隠れている全員を出す
	ForceUnhideAll();

	UE_LOG(LogDawnlight, Log, TEXT("HideableComponent: %s が無効化されました"),
		*GetOwner()->GetName());

	OnHideSpotDisabled.Broadcast();
}

void UHideableComponent::EnableHideSpot()
{
	bIsAvailable = true;

	UE_LOG(LogDawnlight, Log, TEXT("HideableComponent: %s が有効化されました"),
		*GetOwner()->GetName());
}

void UHideableComponent::MovePlayerToHideLocation(ADawnlightCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	// 隠れ位置に瞬間移動（将来的にはアニメーション遷移）
	const FVector HideLocation = GetHideLocation();
	Player->SetActorLocation(HideLocation);
	Player->SetActorRotation(HiddenRotation);
}

void UHideableComponent::RestorePlayerPosition(ADawnlightCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	// 隠れ場所の前に出す
	AActor* Owner = GetOwner();
	if (Owner)
	{
		const FVector ExitLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * InteractionRadius;
		Player->SetActorLocation(ExitLocation);
	}
}

void UHideableComponent::ApplyHiddenState(ADawnlightCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	// 移動を無効化
	if (UCharacterMovementComponent* MovementComp = Player->GetCharacterMovement())
	{
		MovementComp->DisableMovement();
	}

	// コリジョンを無効化（敵に当たらないように）
	Player->SetActorEnableCollision(false);

	// 不可視にする
	Player->SetActorHiddenInGame(true);

	// Gameplay Tag を付与
	if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
	{
		ASC->AddLooseGameplayTag(DawnlightTags::State_Player_Hidden);
		UE_LOG(LogDawnlight, Verbose, TEXT("HideableComponent: State.Player.Hidden タグを付与しました"));
	}
}

void UHideableComponent::RemoveHiddenState(ADawnlightCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	// 移動を有効化
	if (UCharacterMovementComponent* MovementComp = Player->GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_Walking);
	}

	// コリジョンを有効化
	Player->SetActorEnableCollision(true);

	// 可視にする
	Player->SetActorHiddenInGame(false);

	// Gameplay Tag を削除
	if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
	{
		ASC->RemoveLooseGameplayTag(DawnlightTags::State_Player_Hidden);
		UE_LOG(LogDawnlight, Verbose, TEXT("HideableComponent: State.Player.Hidden タグを削除しました"));
	}
}
