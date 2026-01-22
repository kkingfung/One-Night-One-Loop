// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimalAIController.h"
#include "Dawnlight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

AAnimalAIController::AAnimalAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Behavior Tree コンポーネント
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	// Perception コンポーネント
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	// デフォルト値
	WanderRadius = 500.0f;
	FleeDistance = 800.0f;
	SafeDistance = 1200.0f;
	SightDistance = 1000.0f;
	SightAngle = 120.0f; // 視野角（全体）

	// Blackboard キー名
	AIStateKey = TEXT("AIState");
	MoveDestinationKey = TEXT("MoveDestination");
	ThreatActorKey = TEXT("ThreatActor");
	HasThreatKey = TEXT("HasThreat");

	CurrentState = EAnimalAIState::Idle;
	bHasDetectedThreat = false;
}

void AAnimalAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// スポーン位置を記録
	SpawnLocation = InPawn->GetActorLocation();

	// Perceptionをセットアップ
	SetupPerception();

	// Behavior Treeを開始
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		UE_LOG(LogDawnlight, Log, TEXT("[AnimalAI] Behavior Tree開始: %s"), *GetNameSafe(InPawn));
	}
	else
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[AnimalAI] Behavior Treeが設定されていません: %s"), *GetNameSafe(InPawn));
	}

	// 初期状態は徘徊
	SetState(EAnimalAIState::Wandering);
}

void AAnimalAIController::OnUnPossess()
{
	// Behavior Treeを停止
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree();
	}

	Super::OnUnPossess();
}

void AAnimalAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 逃走中の場合、安全距離まで離れたかチェック
	if (CurrentState == EAnimalAIState::Fleeing && bHasDetectedThreat)
	{
		float DistanceToThreat = GetDistanceToThreat();
		if (DistanceToThreat >= SafeDistance)
		{
			// 安全距離に達したら警戒状態に
			bHasDetectedThreat = false;
			ThreatActor.Reset();
			SetState(EAnimalAIState::Alerted);

			// 少し待ってから徘徊に戻る
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (CurrentState == EAnimalAIState::Alerted)
				{
					SetState(EAnimalAIState::Wandering);
				}
			}, 3.0f, false);

			UE_LOG(LogDawnlight, Log, TEXT("[AnimalAI] 安全距離に到達、警戒状態に移行"));
		}
	}

	// Blackboardを更新
	UpdateBlackboard();
}

FVector AAnimalAIController::GetFleeDestination() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return FVector::ZeroVector;
	}

	// 逃走方向を計算
	FVector FleeDirection = CalculateFleeDirection();

	// 逃走先を計算
	FVector FleeDestination = ControlledPawn->GetActorLocation() + FleeDirection * FleeDistance;

	// ナビゲーション可能な位置に調整
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		FNavLocation NavLocation;
		if (NavSystem->ProjectPointToNavigation(FleeDestination, NavLocation, FVector(500.0f, 500.0f, 500.0f)))
		{
			return NavLocation.Location;
		}
	}

	return FleeDestination;
}

FVector AAnimalAIController::GetRandomWanderLocation() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return SpawnLocation;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		return SpawnLocation;
	}

	// スポーン位置を中心にランダムな位置を取得
	FNavLocation RandomLocation;
	if (NavSystem->GetRandomPointInNavigableRadius(SpawnLocation, WanderRadius, RandomLocation))
	{
		return RandomLocation.Location;
	}

	return SpawnLocation;
}

float AAnimalAIController::GetDistanceToThreat() const
{
	APawn* ControlledPawn = GetPawn();
	AActor* Threat = ThreatActor.Get();

	if (!ControlledPawn || !Threat)
	{
		return FLT_MAX;
	}

	return FVector::Dist(ControlledPawn->GetActorLocation(), Threat->GetActorLocation());
}

void AAnimalAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	// プレイヤーかどうかチェック
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Actor != PlayerPawn)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// プレイヤーを検知 → 逃走開始
		bHasDetectedThreat = true;
		ThreatActor = Actor;
		SetState(EAnimalAIState::Fleeing);
		UE_LOG(LogDawnlight, Log, TEXT("[AnimalAI] プレイヤーを検知！逃走開始"));
	}
}

void AAnimalAIController::SetState(EAnimalAIState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	EAnimalAIState OldState = CurrentState;
	CurrentState = NewState;

	// Blackboardに状態を設定
	if (Blackboard)
	{
		Blackboard->SetValueAsEnum(AIStateKey, static_cast<uint8>(NewState));

		// 状態に応じた移動先を設定
		switch (NewState)
		{
		case EAnimalAIState::Wandering:
			Blackboard->SetValueAsVector(MoveDestinationKey, GetRandomWanderLocation());
			break;
		case EAnimalAIState::Fleeing:
			Blackboard->SetValueAsVector(MoveDestinationKey, GetFleeDestination());
			break;
		default:
			break;
		}
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[AnimalAI] 状態変更: %d → %d"),
		static_cast<int32>(OldState), static_cast<int32>(NewState));
}

void AAnimalAIController::UpdateBlackboard()
{
	if (!Blackboard)
	{
		return;
	}

	// 脅威情報を更新
	Blackboard->SetValueAsBool(HasThreatKey, bHasDetectedThreat);
	Blackboard->SetValueAsObject(ThreatActorKey, ThreatActor.Get());

	// 逃走中は移動先を更新
	if (CurrentState == EAnimalAIState::Fleeing && bHasDetectedThreat)
	{
		Blackboard->SetValueAsVector(MoveDestinationKey, GetFleeDestination());
	}
}

void AAnimalAIController::SetupPerception()
{
	if (!AIPerceptionComponent)
	{
		return;
	}

	// 視覚センサーを設定
	SightConfig = NewObject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));
	if (SightConfig)
	{
		SightConfig->SightRadius = SightDistance;
		SightConfig->LoseSightRadius = SightDistance * 1.5f;
		SightConfig->PeripheralVisionAngleDegrees = SightAngle * 0.5f;
		SightConfig->SetMaxAge(3.0f);

		// 全てのアクターを検知
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	// Perceptionコールバックを登録
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAnimalAIController::OnTargetPerceptionUpdated);

	UE_LOG(LogDawnlight, Log, TEXT("[AnimalAI] Perceptionセットアップ完了 - 視野距離: %.0f, 視野角: %.0f°"),
		SightDistance, SightAngle);
}

FVector AAnimalAIController::CalculateFleeDirection() const
{
	APawn* ControlledPawn = GetPawn();
	AActor* Threat = ThreatActor.Get();

	if (!ControlledPawn)
	{
		return FVector::ForwardVector;
	}

	if (!Threat)
	{
		// 脅威がない場合はランダム方向
		return FMath::VRand().GetSafeNormal2D();
	}

	// 脅威から逃げる方向
	FVector FleeDirection = ControlledPawn->GetActorLocation() - Threat->GetActorLocation();
	FleeDirection.Z = 0.0f; // 2D平面上
	return FleeDirection.GetSafeNormal();
}
