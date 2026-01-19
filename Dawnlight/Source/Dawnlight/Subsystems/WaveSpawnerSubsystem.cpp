// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaveSpawnerSubsystem.h"
#include "Dawnlight.h"
#include "Data/EnemyDataAsset.h"
#include "Characters/EnemyCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UWaveSpawnerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentWaveNumber = 0;
	CurrentWaveState = EWaveState::NotStarted;
	EnemiesSpawnedThisWave = 0;

	UE_LOG(LogDawnlight, Log, TEXT("[WaveSpawnerSubsystem] 初期化完了"));
}

void UWaveSpawnerSubsystem::Deinitialize()
{
	StopAllWaves();
	Super::Deinitialize();
}

void UWaveSpawnerSubsystem::InitializeWaveSystem(const TArray<FWaveConfig>& InWaveConfigs)
{
	WaveConfigs = InWaveConfigs;
	CurrentWaveNumber = 0;
	CurrentWaveState = EWaveState::NotStarted;
	EnemiesSpawnedThisWave = 0;
	AliveEnemies.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("[WaveSpawnerSubsystem] ウェーブシステム初期化: %d ウェーブ"), WaveConfigs.Num());
}

void UWaveSpawnerSubsystem::StartFirstWave()
{
	if (WaveConfigs.Num() == 0)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[WaveSpawnerSubsystem] ウェーブ設定がありません"));
		return;
	}

	CurrentWaveNumber = 1;
	EnemiesSpawnedThisWave = 0;
	CurrentWaveState = EWaveState::InProgress;

	const FWaveConfig* Config = GetCurrentWaveConfig();
	if (!Config)
	{
		return;
	}

	UE_LOG(LogDawnlight, Log, TEXT("[WaveSpawnerSubsystem] ウェーブ %d 開始 (敵: %d体, 同時: %d体)"),
		CurrentWaveNumber, Config->TotalEnemies, Config->MaxConcurrentEnemies);

	// ウェーブ開始イベント
	OnWaveStarted.Broadcast(CurrentWaveNumber);

	// スポーンタイマーを開始
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&UWaveSpawnerSubsystem::SpawnEnemy,
		Config->SpawnInterval,
		true,
		0.5f  // 最初のスポーンは少し遅延
	);
}

void UWaveSpawnerSubsystem::StartNextWave()
{
	if (CurrentWaveNumber >= WaveConfigs.Num())
	{
		UE_LOG(LogDawnlight, Log, TEXT("[WaveSpawnerSubsystem] 全ウェーブ完了"));
		OnAllWavesCompleted.Broadcast();
		return;
	}

	CurrentWaveNumber++;
	EnemiesSpawnedThisWave = 0;
	CurrentWaveState = EWaveState::InProgress;

	const FWaveConfig* Config = GetCurrentWaveConfig();
	if (!Config)
	{
		return;
	}

	UE_LOG(LogDawnlight, Log, TEXT("[WaveSpawnerSubsystem] ウェーブ %d 開始 (敵: %d体, 同時: %d体)"),
		CurrentWaveNumber, Config->TotalEnemies, Config->MaxConcurrentEnemies);

	// ウェーブ開始イベント
	OnWaveStarted.Broadcast(CurrentWaveNumber);

	// スポーンタイマーを開始
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&UWaveSpawnerSubsystem::SpawnEnemy,
		Config->SpawnInterval,
		true,
		0.5f
	);
}

void UWaveSpawnerSubsystem::EndCurrentWave(bool bSuccess)
{
	// スポーンタイマーを停止
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

	CurrentWaveState = bSuccess ? EWaveState::Completed : EWaveState::Failed;

	UE_LOG(LogDawnlight, Log, TEXT("[WaveSpawnerSubsystem] ウェーブ %d 終了 (%s)"),
		CurrentWaveNumber, bSuccess ? TEXT("成功") : TEXT("失敗"));

	// ウェーブ完了イベント
	OnWaveCompleted.Broadcast(CurrentWaveNumber, bSuccess);

	// 全ウェーブ完了判定
	if (bSuccess && CurrentWaveNumber >= WaveConfigs.Num())
	{
		OnAllWavesCompleted.Broadcast();
	}
}

void UWaveSpawnerSubsystem::StopAllWaves()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

	// 生存中の敵を全て削除
	for (const TWeakObjectPtr<AEnemyCharacter>& Enemy : AliveEnemies)
	{
		if (Enemy.IsValid())
		{
			Enemy->Destroy();
		}
	}
	AliveEnemies.Empty();

	CurrentWaveState = EWaveState::NotStarted;

	UE_LOG(LogDawnlight, Log, TEXT("[WaveSpawnerSubsystem] 全ウェーブ停止"));
}

void UWaveSpawnerSubsystem::AddSpawnPoint(const FVector& Location)
{
	SpawnPoints.Add(Location);
}

void UWaveSpawnerSubsystem::ClearSpawnPoints()
{
	SpawnPoints.Empty();
}

void UWaveSpawnerSubsystem::SetDefaultEnemyData(UEnemyDataAsset* EnemyData)
{
	DefaultEnemyData = EnemyData;
}

int32 UWaveSpawnerSubsystem::GetRemainingEnemiesInWave() const
{
	const FWaveConfig* Config = GetCurrentWaveConfig();
	if (!Config)
	{
		return 0;
	}

	// まだスポーンしていない敵 + 生存中の敵
	const int32 NotYetSpawned = Config->TotalEnemies - EnemiesSpawnedThisWave;
	return NotYetSpawned + AliveEnemies.Num();
}

bool UWaveSpawnerSubsystem::AreAllWavesCompleted() const
{
	return CurrentWaveNumber >= WaveConfigs.Num() && CurrentWaveState == EWaveState::Completed;
}

void UWaveSpawnerSubsystem::SpawnEnemy()
{
	const FWaveConfig* Config = GetCurrentWaveConfig();
	if (!Config)
	{
		return;
	}

	// スポーン上限チェック
	if (EnemiesSpawnedThisWave >= Config->TotalEnemies)
	{
		// 全ての敵をスポーン済み
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	// 同時存在数チェック
	// 無効な参照を削除
	AliveEnemies.RemoveAll([](const TWeakObjectPtr<AEnemyCharacter>& Enemy)
	{
		return !Enemy.IsValid();
	});

	if (AliveEnemies.Num() >= Config->MaxConcurrentEnemies)
	{
		// 同時存在上限に達している
		return;
	}

	// スポーン位置を取得
	FVector SpawnLocation = GetRandomSpawnLocation();

	// 敵データを選択
	UEnemyDataAsset* EnemyData = SelectEnemyData();
	if (!EnemyData)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[WaveSpawnerSubsystem] 敵データがありません"));
		return;
	}

	// 敵クラスを取得
	UClass* EnemyClass = nullptr;
	if (EnemyData->EnemyBlueprintClass.IsValid())
	{
		EnemyClass = EnemyData->EnemyBlueprintClass.LoadSynchronous();
	}

	if (!EnemyClass)
	{
		// デフォルトのEnemyCharacterを使用
		EnemyClass = AEnemyCharacter::StaticClass();
	}

	// スポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(
		EnemyClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (NewEnemy)
	{
		// 敵データを設定
		NewEnemy->EnemyData = EnemyData;

		// ウェーブ倍率を適用
		NewEnemy->MaxHealth = EnemyData->MaxHealth * Config->HealthMultiplier;
		NewEnemy->CurrentHealth = NewEnemy->MaxHealth;
		NewEnemy->AttackDamage = EnemyData->AttackDamage * Config->DamageMultiplier;

		// 死亡時のデリゲートにバインド
		NewEnemy->OnEnemyDeathDelegate.AddDynamic(this, &UWaveSpawnerSubsystem::OnEnemyDied);

		// 追跡
		AliveEnemies.Add(NewEnemy);
		EnemiesSpawnedThisWave++;

		UE_LOG(LogDawnlight, Verbose, TEXT("[WaveSpawnerSubsystem] 敵スポーン: %s (%d/%d)"),
			*EnemyData->DisplayName.ToString(), EnemiesSpawnedThisWave, Config->TotalEnemies);

		// イベント
		OnEnemySpawned.Broadcast(NewEnemy);
	}
}

FVector UWaveSpawnerSubsystem::GetRandomSpawnLocation() const
{
	if (SpawnPoints.Num() == 0)
	{
		// スポーンポイントがない場合はプレイヤーの周囲にランダムスポーン
		if (UWorld* World = GetWorld())
		{
			if (APawn* Player = World->GetFirstPlayerController()->GetPawn())
			{
				const float SpawnDistance = 800.0f;
				const float RandomAngle = FMath::FRandRange(0.0f, 360.0f);

				return Player->GetActorLocation() + FVector(
					FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * SpawnDistance,
					FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * SpawnDistance,
					0.0f
				);
			}
		}
		return FVector::ZeroVector;
	}

	// ランダムなスポーンポイントを選択
	const int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
	return SpawnPoints[Index];
}

void UWaveSpawnerSubsystem::OnEnemyDied(AEnemyCharacter* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	// リストから削除
	AliveEnemies.RemoveAll([Enemy](const TWeakObjectPtr<AEnemyCharacter>& E)
	{
		return E.Get() == Enemy;
	});

	UE_LOG(LogDawnlight, Log, TEXT("[WaveSpawnerSubsystem] 敵撃破 (残り: %d体)"), GetRemainingEnemiesInWave());

	// イベント
	OnEnemyKilled.Broadcast(Enemy);

	// ウェーブクリア判定
	CheckWaveCompletion();
}

void UWaveSpawnerSubsystem::CheckWaveCompletion()
{
	if (CurrentWaveState != EWaveState::InProgress)
	{
		return;
	}

	const FWaveConfig* Config = GetCurrentWaveConfig();
	if (!Config)
	{
		return;
	}

	// 全ての敵をスポーン済みで、生存中の敵がいない場合
	if (EnemiesSpawnedThisWave >= Config->TotalEnemies && AliveEnemies.Num() == 0)
	{
		EndCurrentWave(true);
	}
}

const FWaveConfig* UWaveSpawnerSubsystem::GetCurrentWaveConfig() const
{
	if (CurrentWaveNumber < 1 || CurrentWaveNumber > WaveConfigs.Num())
	{
		return nullptr;
	}

	return &WaveConfigs[CurrentWaveNumber - 1];
}

UEnemyDataAsset* UWaveSpawnerSubsystem::SelectEnemyData() const
{
	const FWaveConfig* Config = GetCurrentWaveConfig();
	if (!Config)
	{
		return DefaultEnemyData;
	}

	// ウェーブ固有の敵リストがあればそこから選択
	if (Config->AvailableEnemies.Num() > 0)
	{
		// 重み付きランダム選択
		float TotalWeight = 0.0f;
		for (const TObjectPtr<UEnemyDataAsset>& Enemy : Config->AvailableEnemies)
		{
			if (Enemy)
			{
				TotalWeight += Enemy->SpawnWeight;
			}
		}

		float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
		float CurrentWeight = 0.0f;

		for (const TObjectPtr<UEnemyDataAsset>& Enemy : Config->AvailableEnemies)
		{
			if (Enemy)
			{
				CurrentWeight += Enemy->SpawnWeight;
				if (RandomValue <= CurrentWeight)
				{
					return Enemy;
				}
			}
		}
	}

	return DefaultEnemyData;
}
