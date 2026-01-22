// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

/**
 * 敵AIコントローラー
 *
 * Dawn Phaseでの敵AIを制御
 * - プレイヤーを検知して追跡
 * - 攻撃距離内で攻撃
 * - Behavior Treeで行動制御
 */
UCLASS()
class DAWNLIGHT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	// ========================================================================
	// AAIController インターフェース
	// ========================================================================

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;

	// ========================================================================
	// AI制御
	// ========================================================================

	/** ターゲットを設定 */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget);

	/** 現在のターゲットを取得 */
	UFUNCTION(BlueprintPure, Category = "AI")
	AActor* GetTargetActor() const;

	/** ターゲットをクリア */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ClearTarget();

	/** 攻撃距離内かどうか */
	UFUNCTION(BlueprintPure, Category = "AI")
	bool IsInAttackRange() const;

	/** ターゲットまでの距離を取得 */
	UFUNCTION(BlueprintPure, Category = "AI")
	float GetDistanceToTarget() const;

	/** プレイヤーを発見したか */
	UFUNCTION(BlueprintPure, Category = "AI")
	bool HasDetectedPlayer() const { return bHasDetectedPlayer; }

	// ========================================================================
	// Blackboard キー名
	// ========================================================================

	/** ターゲットアクターキー */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName TargetActorKey;

	/** ターゲット位置キー */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName TargetLocationKey;

	/** 攻撃距離内フラグキー */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName InAttackRangeKey;

	/** プレイヤー検知フラグキー */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName HasDetectedPlayerKey;

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

	/** 攻撃距離 */
	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0"))
	float AttackRange;

	/** 視野角（度） */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "0", ClampMax = "180"))
	float SightRadius;

	/** 視野距離 */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "0"))
	float SightDistance;

	/** ロスト時間（プレイヤーを見失ってから追跡をやめるまでの時間） */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "0"))
	float LoseSightRadius;

	// ========================================================================
	// 内部状態
	// ========================================================================

	/** プレイヤーを検知したか */
	bool bHasDetectedPlayer;

	/** 現在のターゲット */
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;

	// ========================================================================
	// 内部処理
	// ========================================================================

	/** Perception更新時のコールバック */
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** Blackboardを更新 */
	void UpdateBlackboard();

	/** プレイヤーを検索 */
	AActor* FindPlayer() const;

	/** Perceptionを設定 */
	void SetupPerception();
};
