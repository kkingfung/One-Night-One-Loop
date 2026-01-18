// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhotographyComponent.h"
#include "Dawnlight.h"
#include "DawnlightTags.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayTagAssetInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UPhotographyComponent::UPhotographyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// デフォルト設定
	MaxPhotos = 10;
	PhotoDuration = 1.5f;
	ShutterSoundRadius = 500.0f;
	bMakeShutterSound = true;
	ViewfinderFOV = 60.0f;
	MaxPhotographDistance = 1500.0f;

	// 状態初期化
	bIsPhotographing = false;
	CurrentPhotoCount = 0;
}

void UPhotographyComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogDawnlight, Log, TEXT("PhotographyComponent: %s で開始しました（最大撮影数: %d）"),
		*GetOwner()->GetName(), MaxPhotos);
}

bool UPhotographyComponent::StartPhotograph()
{
	if (!CanPhotograph())
	{
		UE_LOG(LogDawnlight, Warning, TEXT("PhotographyComponent: 撮影できません"));
		OnPhotographFailed.Broadcast(TEXT("撮影不可"));
		return false;
	}

	bIsPhotographing = true;

	// GASタグを付与
	if (AActor* Owner = GetOwner())
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				ASC->AddLooseGameplayTag(DawnlightTags::State_Player_Photographing);
				UE_LOG(LogDawnlight, Verbose, TEXT("PhotographyComponent: State.Player.Photographing タグを付与"));
			}
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("PhotographyComponent: 撮影開始"));
	OnPhotographStarted.Broadcast();

	// 撮影時間後に完了処理
	GetWorld()->GetTimerManager().SetTimer(
		PhotoTimerHandle,
		this,
		&UPhotographyComponent::OnPhotoTimerComplete,
		PhotoDuration,
		false
	);

	return true;
}

void UPhotographyComponent::CancelPhotograph()
{
	if (!bIsPhotographing)
	{
		return;
	}

	// タイマーをクリア
	GetWorld()->GetTimerManager().ClearTimer(PhotoTimerHandle);

	bIsPhotographing = false;

	// GASタグを削除
	if (AActor* Owner = GetOwner())
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				ASC->RemoveLooseGameplayTag(DawnlightTags::State_Player_Photographing);
				UE_LOG(LogDawnlight, Verbose, TEXT("PhotographyComponent: State.Player.Photographing タグを削除"));
			}
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("PhotographyComponent: 撮影キャンセル"));
	OnPhotographCancelled.Broadcast();
}

FPhotographData UPhotographyComponent::ExecutePhotograph()
{
	FPhotographData PhotoData;

	if (!bIsPhotographing)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("PhotographyComponent: 撮影中ではありません"));
		return PhotoData;
	}

	// フィルム残量チェック
	if (CurrentPhotoCount >= MaxPhotos)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("PhotographyComponent: フィルム切れ"));
		OnPhotographFailed.Broadcast(TEXT("フィルム切れ"));
		bIsPhotographing = false;
		return PhotoData;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return PhotoData;
	}

	// 撮影データを設定
	PhotoData.CaptureLocation = Owner->GetActorLocation();
	PhotoData.CaptureRotation = Owner->GetActorRotation();
	PhotoData.CaptureTime = GetWorld()->GetTimeSeconds();

	// ビューファインダー内の対象を検出
	TArray<AActor*> Subjects = DetectSubjectsInViewfinder();

	// 撮影対象のタグを収集
	for (AActor* Subject : Subjects)
	{
		if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Subject))
		{
			FGameplayTagContainer SubjectTags;
			TagInterface->GetOwnedGameplayTags(SubjectTags);
			PhotoData.CapturedSubjectTags.AppendTags(SubjectTags);
		}
	}

	// 証拠価値を計算
	PhotoData.EvidenceValue = CalculateEvidenceValue(Subjects);
	PhotoData.bIsValidEvidence = PhotoData.EvidenceValue > 0.0f;

	// シャッター音
	if (bMakeShutterSound)
	{
		EmitShutterSound();
	}

	// 撮影データを保存
	Photographs.Add(PhotoData);
	CurrentPhotoCount++;

	UE_LOG(LogDawnlight, Log, TEXT("PhotographyComponent: 撮影完了（証拠価値: %.1f, 残り: %d）"),
		PhotoData.EvidenceValue, GetRemainingPhotos());

	bIsPhotographing = false;

	// GASタグを削除
	if (AActor* OwnerActor = GetOwner())
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerActor))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				ASC->RemoveLooseGameplayTag(DawnlightTags::State_Player_Photographing);
				UE_LOG(LogDawnlight, Verbose, TEXT("PhotographyComponent: State.Player.Photographing タグを削除"));
			}
		}
	}

	OnPhotographCompleted.Broadcast(PhotoData);

	return PhotoData;
}

bool UPhotographyComponent::CanPhotograph() const
{
	// 既に撮影中
	if (bIsPhotographing)
	{
		return false;
	}

	// フィルム切れ
	if (CurrentPhotoCount >= MaxPhotos)
	{
		return false;
	}

	return true;
}

TArray<AActor*> UPhotographyComponent::DetectSubjectsInViewfinder() const
{
	TArray<AActor*> DetectedSubjects;

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return DetectedSubjects;
	}

	const FVector StartLocation = Owner->GetActorLocation();
	const FVector ForwardDirection = Owner->GetActorForwardVector();

	// コーントレースで視野内の対象を検出
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	// 複数のレイキャストで扇形に検出
	const int32 NumRays = 5;
	const float HalfFOV = ViewfinderFOV * 0.5f;

	for (int32 i = 0; i < NumRays; ++i)
	{
		const float Angle = FMath::Lerp(-HalfFOV, HalfFOV, static_cast<float>(i) / (NumRays - 1));
		const FVector Direction = ForwardDirection.RotateAngleAxis(Angle, FVector::UpVector);
		const FVector EndLocation = StartLocation + Direction * MaxPhotographDistance;

		FHitResult HitResult;
		const bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			ECC_Visibility
		);

		if (bHit && HitResult.GetActor())
		{
			AActor* HitActor = HitResult.GetActor();

			// 有効な撮影対象かチェック
			if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(HitActor))
			{
				FGameplayTagContainer ActorTags;
				TagInterface->GetOwnedGameplayTags(ActorTags);

				if (ActorTags.HasAny(ValidSubjectTags))
				{
					DetectedSubjects.AddUnique(HitActor);
				}
			}
		}
	}

	return DetectedSubjects;
}

float UPhotographyComponent::CalculateEvidenceValue(const TArray<AActor*>& Subjects) const
{
	if (Subjects.Num() == 0)
	{
		return 0.0f;
	}

	float TotalValue = 0.0f;

	for (const AActor* Subject : Subjects)
	{
		// 基本価値
		float SubjectValue = 10.0f;

		// 距離による減衰
		const float Distance = FVector::Distance(GetOwner()->GetActorLocation(), Subject->GetActorLocation());
		const float DistanceFactor = FMath::Clamp(1.0f - (Distance / MaxPhotographDistance), 0.2f, 1.0f);
		SubjectValue *= DistanceFactor;

		// タグによるボーナス（重要な証拠ほど高価値）
		// TODO: タグごとの価値を設定

		TotalValue += SubjectValue;
	}

	return TotalValue;
}

void UPhotographyComponent::EmitShutterSound()
{
	// シャッター音による検知リスク
	// TODO: サウンドイベントを発行し、周囲のAIに通知

	UE_LOG(LogDawnlight, Verbose, TEXT("PhotographyComponent: シャッター音（半径: %.0f）"), ShutterSoundRadius);

	// 周囲のアクターにサウンドイベントを通知
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(ShutterSoundRadius);

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetOwner()->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		SphereShape
	);

	// TODO: AIPerceptionSystemへの通知を実装
}

void UPhotographyComponent::OnPhotoTimerComplete()
{
	// 撮影時間経過後、自動的にシャッターを切る
	ExecutePhotograph();
}
