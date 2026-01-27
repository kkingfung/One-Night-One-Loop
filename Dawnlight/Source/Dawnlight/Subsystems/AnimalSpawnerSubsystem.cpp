// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "AnimalSpawnerSubsystem.h"
#include "Dawnlight.h"
#include "Data/SoulDataAsset.h"
#include "Characters/AnimalCharacter.h"
#include "Engine/World.h"

void UAnimalSpawnerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SpawnAreaCenter = FVector::ZeroVector;
	SpawnAreaRadius = 1000.0f;
	bUseSpawnArea = false;
	TotalSpawnedCount = 0;
	KilledAnimalCount = 0;

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalSpawnerSubsystem] 初期化完了"));
}

void UAnimalSpawnerSubsystem::Deinitialize()
{
	DespawnAllAnimals();
	Super::Deinitialize();
}

void UAnimalSpawnerSubsystem::InitializeAnimalSpawner(const TArray<FAnimalSpawnConfig>& InSpawnConfigs)
{
	SpawnConfigs = InSpawnConfigs;
	TotalSpawnedCount = 0;
	KilledAnimalCount = 0;
	AliveAnimals.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalSpawnerSubsystem] 動物スポーナー初期化: %d 種類"), SpawnConfigs.Num());
}

void UAnimalSpawnerSubsystem::SpawnAllAnimals()
{
	if (SpawnConfigs.Num() == 0)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[AnimalSpawnerSubsystem] スポーン設定がありません"));
		return;
	}

	// 各設定に基づいてスポーン
	for (const FAnimalSpawnConfig& Config : SpawnConfigs)
	{
		if (!Config.SoulData)
		{
			UE_LOG(LogDawnlight, Warning, TEXT("[AnimalSpawnerSubsystem] SoulDataが設定されていないスポーン設定をスキップ"));
			continue;
		}

		for (int32 i = 0; i < Config.SpawnCount; ++i)
		{
			const FVector SpawnLocation = GetRandomSpawnLocation();
			AAnimalCharacter* Animal = SpawnAnimal(Config.SoulData, SpawnLocation);

			// カスタムクラスが指定されていれば使用
			if (Animal && Config.CustomAnimalClass)
			{
				// カスタムクラスは既にSpawnAnimal内で処理済み
			}
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalSpawnerSubsystem] 全動物スポーン完了: %d体"), TotalSpawnedCount);
}

AAnimalCharacter* UAnimalSpawnerSubsystem::SpawnAnimal(USoulDataAsset* SoulData, const FVector& Location)
{
	if (!SoulData)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[AnimalSpawnerSubsystem] SoulDataがnullのためスポーンできません"));
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	// 動物クラスを決定
	UClass* AnimalClass = nullptr;

	// スポーン設定からカスタムクラスを探す
	for (const FAnimalSpawnConfig& Config : SpawnConfigs)
	{
		if (Config.SoulData == SoulData && Config.CustomAnimalClass)
		{
			AnimalClass = Config.CustomAnimalClass;
			break;
		}
	}

	// SoulDataからBlueprintクラスを取得
	if (!AnimalClass && SoulData->AnimalBlueprintClass.IsValid())
	{
		AnimalClass = SoulData->AnimalBlueprintClass.LoadSynchronous();
	}

	// デフォルトのAnimalCharacterを使用
	if (!AnimalClass)
	{
		AnimalClass = AAnimalCharacter::StaticClass();
	}

	// スポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAnimalCharacter* NewAnimal = World->SpawnActor<AAnimalCharacter>(
		AnimalClass,
		Location,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (NewAnimal)
	{
		// SoulDataを設定
		NewAnimal->SoulData = SoulData;

		// 追跡リストに追加
		AliveAnimals.Add(NewAnimal);
		TotalSpawnedCount++;

		UE_LOG(LogDawnlight, Log, TEXT("[AnimalSpawnerSubsystem] 動物スポーン: %s (%d体目)"),
			*SoulData->DisplayName.ToString(), TotalSpawnedCount);

		// スポーンイベント
		OnAnimalSpawned.Broadcast(NewAnimal);
	}

	return NewAnimal;
}

void UAnimalSpawnerSubsystem::DespawnAllAnimals()
{
	// 生存中の動物を全て削除
	for (const TWeakObjectPtr<AAnimalCharacter>& Animal : AliveAnimals)
	{
		if (Animal.IsValid())
		{
			Animal->Destroy();
		}
	}
	AliveAnimals.Empty();

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalSpawnerSubsystem] 全動物削除"));
}

bool UAnimalSpawnerSubsystem::SpawnRandomAnimal()
{
	if (SpawnConfigs.Num() == 0)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[AnimalSpawnerSubsystem] SpawnRandomAnimal: スポーン設定がありません"));
		return false;
	}

	// 有効な設定からランダムに選択
	TArray<const FAnimalSpawnConfig*> ValidConfigs;
	for (const FAnimalSpawnConfig& Config : SpawnConfigs)
	{
		if (Config.SoulData)
		{
			ValidConfigs.Add(&Config);
		}
	}

	if (ValidConfigs.Num() == 0)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[AnimalSpawnerSubsystem] SpawnRandomAnimal: 有効なスポーン設定がありません"));
		return false;
	}

	// ランダムに設定を選択
	const int32 RandomIndex = FMath::RandRange(0, ValidConfigs.Num() - 1);
	const FAnimalSpawnConfig* SelectedConfig = ValidConfigs[RandomIndex];

	// ランダムな位置にスポーン
	const FVector SpawnLocation = GetRandomSpawnLocation();
	AAnimalCharacter* SpawnedAnimal = SpawnAnimal(SelectedConfig->SoulData, SpawnLocation);

	return SpawnedAnimal != nullptr;
}

void UAnimalSpawnerSubsystem::AddSpawnPoint(const FVector& Location)
{
	SpawnPoints.Add(Location);
}

void UAnimalSpawnerSubsystem::SetSpawnArea(const FVector& Center, float Radius)
{
	SpawnAreaCenter = Center;
	SpawnAreaRadius = Radius;
	bUseSpawnArea = true;

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalSpawnerSubsystem] スポーンエリア設定: 中心(%s), 半径(%.0f)"),
		*Center.ToString(), Radius);
}

void UAnimalSpawnerSubsystem::ClearSpawnPoints()
{
	SpawnPoints.Empty();
	bUseSpawnArea = false;
}

int32 UAnimalSpawnerSubsystem::GetAliveAnimalCount() const
{
	// 無効な参照を除外してカウント
	int32 Count = 0;
	for (const TWeakObjectPtr<AAnimalCharacter>& Animal : AliveAnimals)
	{
		if (Animal.IsValid() && Animal->IsAlive())
		{
			Count++;
		}
	}
	return Count;
}

bool UAnimalSpawnerSubsystem::AreAllAnimalsKilled() const
{
	return TotalSpawnedCount > 0 && GetAliveAnimalCount() == 0;
}

TArray<AAnimalCharacter*> UAnimalSpawnerSubsystem::GetAliveAnimals() const
{
	TArray<AAnimalCharacter*> Result;

	for (const TWeakObjectPtr<AAnimalCharacter>& Animal : AliveAnimals)
	{
		if (Animal.IsValid() && Animal->IsAlive())
		{
			Result.Add(Animal.Get());
		}
	}

	return Result;
}

FVector UAnimalSpawnerSubsystem::GetRandomSpawnLocation() const
{
	// スポーンポイントがあればそこからランダム選択
	if (SpawnPoints.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
		return SpawnPoints[Index];
	}

	// スポーンエリアが設定されていればその範囲内でランダム
	if (bUseSpawnArea)
	{
		const float RandomAngle = FMath::FRandRange(0.0f, 360.0f);
		const float RandomDistance = FMath::FRandRange(0.0f, SpawnAreaRadius);

		return SpawnAreaCenter + FVector(
			FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
			FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
			0.0f
		);
	}

	// プレイヤーの周囲にスポーン（デフォルト）
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (APawn* Player = PC->GetPawn())
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
	}

	return FVector::ZeroVector;
}

void UAnimalSpawnerSubsystem::OnAnimalDied(AAnimalCharacter* Animal)
{
	if (!Animal)
	{
		return;
	}

	// リストから削除
	AliveAnimals.RemoveAll([Animal](const TWeakObjectPtr<AAnimalCharacter>& A)
	{
		return A.Get() == Animal;
	});

	KilledAnimalCount++;

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalSpawnerSubsystem] 動物撃破: %s (残り: %d体)"),
		*Animal->GetName(), GetAliveAnimalCount());

	// 撃破イベント
	OnAnimalKilled.Broadcast(Animal);

	// 全滅判定
	if (AreAllAnimalsKilled())
	{
		UE_LOG(LogDawnlight, Log, TEXT("[AnimalSpawnerSubsystem] 全動物撃破完了"));
		OnAllAnimalsKilled.Broadcast();
	}
}

void UAnimalSpawnerSubsystem::CleanupInvalidReferences()
{
	AliveAnimals.RemoveAll([](const TWeakObjectPtr<AAnimalCharacter>& Animal)
	{
		return !Animal.IsValid();
	});
}
