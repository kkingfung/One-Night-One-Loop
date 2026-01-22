// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnemyAIController.h"
#include "Dawnlight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Behavior Tree コンポーネント
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	// Blackboard コンポーネント（AIController基底クラスで作成される）
	// ただし明示的にセットアップが必要

	// Perception コンポーネント
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	// デフォルト値
	AttackRange = 150.0f;
	SightRadius = 45.0f; // 視野角（半分）
	SightDistance = 2000.0f;
	LoseSightRadius = 2500.0f;

	// Blackboard キー名
	TargetActorKey = TEXT("TargetActor");
	TargetLocationKey = TEXT("TargetLocation");
	InAttackRangeKey = TEXT("InAttackRange");
	HasDetectedPlayerKey = TEXT("HasDetectedPlayer");

	bHasDetectedPlayer = false;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Perceptionをセットアップ
	SetupPerception();

	// Behavior Treeを開始
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		UE_LOG(LogDawnlight, Log, TEXT("[EnemyAI] Behavior Tree開始: %s"), *GetNameSafe(InPawn));
	}
	else
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[EnemyAI] Behavior Treeが設定されていません: %s"), *GetNameSafe(InPawn));
	}

	// 初期ターゲットとしてプレイヤーを設定
	if (AActor* Player = FindPlayer())
	{
		SetTargetActor(Player);
	}
}

void AEnemyAIController::OnUnPossess()
{
	// Behavior Treeを停止
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree();
	}

	Super::OnUnPossess();
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Blackboardを更新
	UpdateBlackboard();
}

void AEnemyAIController::SetTargetActor(AActor* NewTarget)
{
	CurrentTarget = NewTarget;

	if (Blackboard)
	{
		Blackboard->SetValueAsObject(TargetActorKey, NewTarget);

		if (NewTarget)
		{
			Blackboard->SetValueAsVector(TargetLocationKey, NewTarget->GetActorLocation());
		}
	}

	if (NewTarget)
	{
		bHasDetectedPlayer = true;
		UE_LOG(LogDawnlight, Verbose, TEXT("[EnemyAI] ターゲット設定: %s"), *GetNameSafe(NewTarget));
	}
}

AActor* AEnemyAIController::GetTargetActor() const
{
	return CurrentTarget.Get();
}

void AEnemyAIController::ClearTarget()
{
	CurrentTarget.Reset();

	if (Blackboard)
	{
		Blackboard->ClearValue(TargetActorKey);
		Blackboard->ClearValue(TargetLocationKey);
	}
}

bool AEnemyAIController::IsInAttackRange() const
{
	return GetDistanceToTarget() <= AttackRange;
}

float AEnemyAIController::GetDistanceToTarget() const
{
	APawn* ControlledPawn = GetPawn();
	AActor* Target = CurrentTarget.Get();

	if (!ControlledPawn || !Target)
	{
		return FLT_MAX;
	}

	return FVector::Dist(ControlledPawn->GetActorLocation(), Target->GetActorLocation());
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
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
		// プレイヤーを検知
		SetTargetActor(Actor);
		UE_LOG(LogDawnlight, Log, TEXT("[EnemyAI] プレイヤーを検知！"));
	}
	else
	{
		// プレイヤーを見失った（ただしすぐにはターゲットをクリアしない）
		// 最後に見た位置は保持
		if (Blackboard && Actor)
		{
			Blackboard->SetValueAsVector(TargetLocationKey, Actor->GetActorLocation());
		}
		UE_LOG(LogDawnlight, Log, TEXT("[EnemyAI] プレイヤーを見失った"));
	}
}

void AEnemyAIController::UpdateBlackboard()
{
	if (!Blackboard)
	{
		return;
	}

	AActor* Target = CurrentTarget.Get();

	// ターゲット位置を更新
	if (Target)
	{
		Blackboard->SetValueAsVector(TargetLocationKey, Target->GetActorLocation());
	}

	// 攻撃距離フラグを更新
	Blackboard->SetValueAsBool(InAttackRangeKey, IsInAttackRange());

	// 検知フラグを更新
	Blackboard->SetValueAsBool(HasDetectedPlayerKey, bHasDetectedPlayer);
}

AActor* AEnemyAIController::FindPlayer() const
{
	return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AEnemyAIController::SetupPerception()
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
		SightConfig->LoseSightRadius = LoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = SightRadius;
		SightConfig->SetMaxAge(5.0f); // 5秒間記憶

		// 検知対象を設定（敵対アクターのみ）
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	// Perceptionコールバックを登録
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);

	UE_LOG(LogDawnlight, Log, TEXT("[EnemyAI] Perceptionセットアップ完了 - 視野距離: %.0f, 視野角: %.0f°"),
		SightDistance, SightRadius * 2.0f);
}
