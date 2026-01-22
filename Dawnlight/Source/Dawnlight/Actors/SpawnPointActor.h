// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SpawnPointActor.generated.h"

class UBillboardComponent;
class USphereComponent;

/**
 * スポーンポイントのタイプ
 */
UENUM(BlueprintType)
enum class ESpawnPointType : uint8
{
	Animal		UMETA(DisplayName = "動物"),
	Enemy		UMETA(DisplayName = "敵"),
	Both		UMETA(DisplayName = "両方")
};

/**
 * スポーンポイントアクター
 *
 * レベルに配置してスポーン位置を指定する
 * - BeginPlayで自動的にSubsystemに登録
 * - タイプ別にフィルタリング可能
 * - デバッグ表示機能
 */
UCLASS()
class DAWNLIGHT_API ASpawnPointActor : public AActor
{
	GENERATED_BODY()

public:
	ASpawnPointActor();

	// ========================================================================
	// スポーン位置取得
	// ========================================================================

	/** スポーン位置を取得（半径内のランダム位置） */
	UFUNCTION(BlueprintPure, Category = "スポーンポイント")
	FVector GetSpawnLocation() const;

	/** このスポーンポイントのタイプを取得 */
	UFUNCTION(BlueprintPure, Category = "スポーンポイント")
	ESpawnPointType GetSpawnPointType() const { return SpawnPointType; }

	/** 指定タイプで使用可能か */
	UFUNCTION(BlueprintPure, Category = "スポーンポイント")
	bool IsValidForType(ESpawnPointType Type) const;

	/** 有効かどうか */
	UFUNCTION(BlueprintPure, Category = "スポーンポイント")
	bool IsEnabled() const { return bEnabled; }

	/** 有効/無効を設定 */
	UFUNCTION(BlueprintCallable, Category = "スポーンポイント")
	void SetEnabled(bool bNewEnabled);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// ========================================================================
	// コンポーネント
	// ========================================================================

	/** ルートコンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント")
	TObjectPtr<USceneComponent> RootComp;

	/** デバッグ表示用スフィア */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント")
	TObjectPtr<USphereComponent> DebugSphere;

#if WITH_EDITORONLY_DATA
	/** エディタ用ビルボード */
	UPROPERTY(VisibleAnywhere, Category = "コンポーネント")
	TObjectPtr<UBillboardComponent> BillboardComponent;
#endif

	// ========================================================================
	// 設定
	// ========================================================================

	/** スポーンポイントのタイプ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スポーンポイント")
	ESpawnPointType SpawnPointType;

	/** スポーン半径（この範囲内でランダムスポーン） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スポーンポイント", meta = (ClampMin = "0"))
	float SpawnRadius;

	/** 有効かどうか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スポーンポイント")
	bool bEnabled;

	/** 追加タグ（フィルタリング用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "スポーンポイント")
	FGameplayTagContainer SpawnTags;

	/** デバッグ表示を有効にするか（エディタのみ） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "デバッグ")
	bool bShowDebug;

	/** デバッグ表示色 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "デバッグ", meta = (EditCondition = "bShowDebug"))
	FColor DebugColor;

private:
	/** Subsystemに登録 */
	void RegisterToSubsystems();

	/** Subsystemから登録解除 */
	void UnregisterFromSubsystems();

	/** デバッグ表示を更新 */
	void UpdateDebugVisualization();
};
