// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulReaperLevelSetup.h"
#include "Dawnlight.h"
#include "Core/DawnlightGameMode.h"
#include "Data/EnemyDataAsset.h"
#include "Data/SoulDataAsset.h"
#include "Subsystems/AnimalSpawnerSubsystem.h"
#include "Subsystems/WaveSpawnerSubsystem.h"
#include "Subsystems/SoulCollectionSubsystem.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ASoulReaperLevelSetup::ASoulReaperLevelSetup()
{
	PrimaryActorTick.bCanEverTick = false;

	// ルートコンポーネント
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ASoulReaperLevelSetup::BeginPlay()
{
	Super::BeginPlay();

	// デフォルト設定を使用する場合は生成
	if (bUseDefaultWaveConfigs && WaveConfigs.Num() == 0)
	{
		GenerateDefaultWaveConfigs();
	}

	// 自動セットアップ
	if (bAutoSetupOnBeginPlay)
	{
		SetupSubsystems();
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperLevelSetup] レベルセットアップ完了"));
}

void ASoulReaperLevelSetup::SetupSubsystems()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// AnimalSpawnerSubsystemのセットアップ
	AnimalSpawner = World->GetSubsystem<UAnimalSpawnerSubsystem>();
	if (AnimalSpawner.IsValid())
	{
		// 動物スポーン設定を渡す
		AnimalSpawner->InitializeAnimalSpawner(AnimalSpawnConfigs);

		// スポーンエリアを設定
		const FVector SpawnCenter = GetActorLocation() + AnimalSpawnAreaOffset;
		AnimalSpawner->SetSpawnArea(SpawnCenter, AnimalSpawnAreaRadius);

		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperLevelSetup] AnimalSpawner設定完了: %d種類の動物"), AnimalSpawnConfigs.Num());
	}

	// WaveSpawnerSubsystemのセットアップ
	WaveSpawner = World->GetSubsystem<UWaveSpawnerSubsystem>();
	if (WaveSpawner.IsValid())
	{
		// ウェーブ設定を渡す
		WaveSpawner->InitializeWaveSystem(WaveConfigs);

		// デフォルト敵データを設定
		if (DefaultEnemyData)
		{
			WaveSpawner->SetDefaultEnemyData(DefaultEnemyData);
		}

		// スポーンポイントを設定
		WaveSpawner->ClearSpawnPoints();
		for (const FVector& SpawnPoint : EnemySpawnPoints)
		{
			WaveSpawner->AddSpawnPoint(GetActorLocation() + SpawnPoint);
		}

		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperLevelSetup] WaveSpawner設定完了: %d ウェーブ, %d スポーンポイント"),
			WaveConfigs.Num(), EnemySpawnPoints.Num());
	}

	// GameModeにNight Phase時間を設定
	if (ADawnlightGameMode* GameMode = Cast<ADawnlightGameMode>(UGameplayStatics::GetGameMode(World)))
	{
		GameMode->SetNightPhaseDuration(NightPhaseDuration);
		UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperLevelSetup] Night Phase時間設定: %.0f秒"), NightPhaseDuration);
	}
}

void ASoulReaperLevelSetup::StartNightPhase()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 動物をスポーン
	if (AnimalSpawner.IsValid())
	{
		AnimalSpawner->SpawnAllAnimals();
	}

	// GameModeでNight Phaseを開始
	if (ADawnlightGameMode* GameMode = Cast<ADawnlightGameMode>(UGameplayStatics::GetGameMode(World)))
	{
		GameMode->StartNightPhase();
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperLevelSetup] Night Phase開始"));
}

void ASoulReaperLevelSetup::StartDawnPhase()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 残っている動物を削除
	if (AnimalSpawner.IsValid())
	{
		AnimalSpawner->DespawnAllAnimals();
	}

	// ウェーブを開始
	if (WaveSpawner.IsValid())
	{
		WaveSpawner->StartFirstWave();
	}

	// GameModeでDawn Phaseを開始
	if (ADawnlightGameMode* GameMode = Cast<ADawnlightGameMode>(UGameplayStatics::GetGameMode(World)))
	{
		GameMode->StartDawnPhase();
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperLevelSetup] Dawn Phase開始"));
}

void ASoulReaperLevelSetup::GenerateDefaultWaveConfigs()
{
	WaveConfigs.Empty();

	// Wave 1: 導入（簡単）
	FWaveConfig Wave1;
	Wave1.WaveNumber = 1;
	Wave1.TotalEnemies = 5;
	Wave1.MaxConcurrentEnemies = 2;
	Wave1.SpawnInterval = 3.0f;
	Wave1.HealthMultiplier = 1.0f;
	Wave1.DamageMultiplier = 1.0f;
	WaveConfigs.Add(Wave1);

	// Wave 2: 中盤（通常）
	FWaveConfig Wave2;
	Wave2.WaveNumber = 2;
	Wave2.TotalEnemies = 8;
	Wave2.MaxConcurrentEnemies = 3;
	Wave2.SpawnInterval = 2.5f;
	Wave2.HealthMultiplier = 1.2f;
	Wave2.DamageMultiplier = 1.1f;
	WaveConfigs.Add(Wave2);

	// Wave 3: クライマックス（難しい）
	FWaveConfig Wave3;
	Wave3.WaveNumber = 3;
	Wave3.TotalEnemies = 12;
	Wave3.MaxConcurrentEnemies = 4;
	Wave3.SpawnInterval = 2.0f;
	Wave3.HealthMultiplier = 1.5f;
	Wave3.DamageMultiplier = 1.3f;
	WaveConfigs.Add(Wave3);

	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperLevelSetup] デフォルトウェーブ設定を生成: 3ウェーブ"));
}

void ASoulReaperLevelSetup::OnNightPhaseEnd()
{
	// Night Phase終了時の処理
	// Dawn Phaseへの移行はGameModeが管理
	UE_LOG(LogDawnlight, Log, TEXT("[SoulReaperLevelSetup] Night Phase終了"));
}

#if WITH_EDITOR
void ASoulReaperLevelSetup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// デバッグ表示が有効なら再描画をリクエスト
	if (bShowSpawnAreas)
	{
		// エディタでの視覚的フィードバック用
	}
}
#endif
