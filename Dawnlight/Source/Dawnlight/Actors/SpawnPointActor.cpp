// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpawnPointActor.h"
#include "Dawnlight.h"
#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"
#include "Subsystems/AnimalSpawnerSubsystem.h"
#include "Subsystems/WaveSpawnerSubsystem.h"
#include "UObject/ConstructorHelpers.h"

ASpawnPointActor::ASpawnPointActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// ルートコンポーネント
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;

	// デバッグ用スフィア
	DebugSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DebugSphere"));
	DebugSphere->SetupAttachment(RootComponent);
	DebugSphere->SetSphereRadius(100.0f);
	DebugSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugSphere->SetHiddenInGame(true);
	DebugSphere->bIsEditorOnly = true;
	DebugSphere->SetLineThickness(2.0f);

#if WITH_EDITORONLY_DATA
	// ビルボード（エディタのみ）
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(RootComponent);
	BillboardComponent->bIsEditorOnly = true;

	// デフォルトのスプライトを設定
	static ConstructorHelpers::FObjectFinder<UTexture2D> SpriteFinder(TEXT("/Engine/EditorResources/S_TargetPoint"));
	if (SpriteFinder.Succeeded())
	{
		BillboardComponent->SetSprite(SpriteFinder.Object);
	}
#endif

	// デフォルト値
	SpawnPointType = ESpawnPointType::Both;
	SpawnRadius = 100.0f;
	bEnabled = true;
	bShowDebug = true;
	DebugColor = FColor::Green;

	// ゲーム中は非表示
	SetActorHiddenInGame(true);
}

void ASpawnPointActor::BeginPlay()
{
	Super::BeginPlay();

	if (bEnabled)
	{
		RegisterToSubsystems();
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SpawnPointActor] スポーンポイント登録: %s (タイプ: %d, 半径: %.0f)"),
		*GetName(), static_cast<int32>(SpawnPointType), SpawnRadius);
}

void ASpawnPointActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterFromSubsystems();
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void ASpawnPointActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateDebugVisualization();
}
#endif

FVector ASpawnPointActor::GetSpawnLocation() const
{
	FVector BaseLocation = GetActorLocation();

	if (SpawnRadius <= 0.0f)
	{
		return BaseLocation;
	}

	// 半径内でランダムな位置を取得
	const float RandomAngle = FMath::FRandRange(0.0f, 360.0f);
	const float RandomDistance = FMath::FRandRange(0.0f, SpawnRadius);

	return BaseLocation + FVector(
		FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
		FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
		0.0f
	);
}

bool ASpawnPointActor::IsValidForType(ESpawnPointType Type) const
{
	if (!bEnabled)
	{
		return false;
	}

	// Bothは両方に有効
	if (SpawnPointType == ESpawnPointType::Both)
	{
		return true;
	}

	return SpawnPointType == Type;
}

void ASpawnPointActor::SetEnabled(bool bNewEnabled)
{
	if (bEnabled == bNewEnabled)
	{
		return;
	}

	bEnabled = bNewEnabled;

	if (bEnabled)
	{
		RegisterToSubsystems();
	}
	else
	{
		UnregisterFromSubsystems();
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SpawnPointActor] %s: %s"),
		*GetName(), bEnabled ? TEXT("有効化") : TEXT("無効化"));
}

void ASpawnPointActor::RegisterToSubsystems()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector Location = GetActorLocation();

	// 動物スポーナーに登録
	if (IsValidForType(ESpawnPointType::Animal))
	{
		if (UAnimalSpawnerSubsystem* AnimalSpawner = World->GetSubsystem<UAnimalSpawnerSubsystem>())
		{
			AnimalSpawner->AddSpawnPoint(Location);
		}
	}

	// 敵スポーナーに登録
	if (IsValidForType(ESpawnPointType::Enemy))
	{
		if (UWaveSpawnerSubsystem* WaveSpawner = World->GetSubsystem<UWaveSpawnerSubsystem>())
		{
			WaveSpawner->AddSpawnPoint(Location);
		}
	}
}

void ASpawnPointActor::UnregisterFromSubsystems()
{
	// 注意: Subsystemは単純なFVector配列なので、個別の登録解除は未実装
	// 必要に応じてSubsystem側にRemoveSpawnPoint()を追加
}

void ASpawnPointActor::UpdateDebugVisualization()
{
#if WITH_EDITOR
	if (!DebugSphere)
	{
		return;
	}

	// 半径を更新
	DebugSphere->SetSphereRadius(SpawnRadius > 0.0f ? SpawnRadius : 50.0f);

	// 色を更新
	DebugSphere->ShapeColor = DebugColor;

	// 表示/非表示
	DebugSphere->SetVisibility(bShowDebug);
#endif
}
