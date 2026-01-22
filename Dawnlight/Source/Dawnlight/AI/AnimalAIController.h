// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AnimalAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

/**
 * 動物AI状態
 */
UENUM(BlueprintType)
enum class EAnimalAIState : uint8
{
	Idle		UMETA(DisplayName = "待機"),
	Wandering	UMETA(DisplayName = "徘徊"),
	Fleeing		UMETA(DisplayName = "逃走"),
	Alerted		UMETA(DisplayName = "警戒")
};

/**
 * 動物AIコントローラー
 *
 * Night Phaseでの動物AIを制御
 * - ランダムに徘徊
 * - プレイヤーを検知すると逃走
 * - Behavior Treeで行動制御
 */
UCLASS()
class DAWNLIGHT_API AAnimalAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAnimalAIController();

	// ========================================================================
	// AAIController インターフェース
	// ========================================================================

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;

	// ========================================================================
	// AI制御
	// ========================================================================

	/** 現在のAI状態を取得 */
	UFUNCTION(BlueprintPure, Category = "AI")
	EAnimalAIState GetCurrentState() const { return CurrentState; }

	/** 逃走先を取得 */
	UFUNCTION(BlueprintPure, Category = "AI")
	FVector GetFleeDestination() const;

	/** ランダムな徘徊先を取得 */
	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector GetRandomWanderLocation() const;

	/** プレイヤーを検知したか */
	UFUNCTION(BlueprintPure, Category = "AI")
	bool HasDetectedThreat() const { return bHasDetectedThreat; }

	/** 脅威からの距離を取得 */
	UFUNCTION(BlueprintPure, Category = "AI")
	float GetDistanceToThreat() const;

	// ========================================================================
	// Blackboard キー名
	// ========================================================================

	/** 現在のAI状態キー */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName AIStateKey;

	/** 移動先キー */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName MoveDestinationKey;

	/** 脅威アクターキー */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName ThreatActorKey;

	/** 脅威検知フラグキー */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName HasThreatKey;

protected:
	// ========================================================================
	// コンポーネント
	// ========================================================================

	/** Behavior Tree コンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	/** AI Perception コンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	/** 視覚センサー設定 */
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 使用するBehavior Tree */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	/** 徘徊半径 */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Wander", meta = (ClampMin = "100"))
	float WanderRadius;

	/** 逃走距離（脅威からこの距離まで逃げる） */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Flee", meta = (ClampMin = "100"))
	float FleeDistance;

	/** 安全距離（脅威からこの距離離れたら安全） */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Flee", meta = (ClampMin = "100"))
	float SafeDistance;

	/** 視野距離 */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "0"))
	float SightDistance;

	/** 視野角（度） */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "0", ClampMax = "180"))
	float SightAngle;

	// ========================================================================
	// 内部状態
	// ========================================================================

	/** 現在のAI状態 */
	EAnimalAIState CurrentState;

	/** 脅威を検知したか */
	bool bHasDetectedThreat;

	/** 脅威アクター */
	UPROPERTY()
	TWeakObjectPtr<AActor> ThreatActor;

	/** スポーン位置（徘徊の中心） */
	FVector SpawnLocation;

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** Perception更新時のコールバック */
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** 状態を設定 */
	void SetState(EAnimalAIState NewState);

	/** Blackboardを更新 */
	void UpdateBlackboard();

	/** Perceptionを設定 */
	void SetupPerception();

	/** 逃走方向を計算 */
	FVector CalculateFleeDirection() const;
};
