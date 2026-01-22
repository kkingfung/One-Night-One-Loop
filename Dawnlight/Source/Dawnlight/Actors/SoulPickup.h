// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SoulPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class USoulDataAsset;

/**
 * 魂ピックアップアクター
 *
 * 動物が倒された時にスポーンし、プレイヤーが接触すると収集される
 * - 浮遊アニメーション
 * - プレイヤー接近時に吸い寄せ
 * - 収集時にVFX再生
 */
UCLASS()
class DAWNLIGHT_API ASoulPickup : public AActor
{
	GENERATED_BODY()

public:
	ASoulPickup();

	virtual void Tick(float DeltaTime) override;

	// ========================================================================
	// 初期化
	// ========================================================================

	/** 魂データを設定してスポーン */
	UFUNCTION(BlueprintCallable, Category = "魂")
	void InitializeSoul(USoulDataAsset* InSoulData);

	/** 魂タイプタグで初期化（DataAssetなし） */
	UFUNCTION(BlueprintCallable, Category = "魂")
	void InitializeSoulByTag(FGameplayTag InSoulTypeTag);

	// ========================================================================
	// 状態取得
	// ========================================================================

	/** 魂データを取得 */
	UFUNCTION(BlueprintPure, Category = "魂")
	USoulDataAsset* GetSoulData() const { return SoulData; }

	/** 魂タイプタグを取得 */
	UFUNCTION(BlueprintPure, Category = "魂")
	FGameplayTag GetSoulTypeTag() const { return SoulTypeTag; }

protected:
	virtual void BeginPlay() override;

	// ========================================================================
	// コンポーネント
	// ========================================================================

	/** コリジョン */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント")
	TObjectPtr<USphereComponent> CollisionComponent;

	/** ビジュアルメッシュ（オプション） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** VFXコンポーネント */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント")
	TObjectPtr<UNiagaraComponent> VFXComponent;

	/** 吸い寄せ用コリジョン（プレイヤー検出） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント")
	TObjectPtr<USphereComponent> AttractionTrigger;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 魂データ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "魂")
	TObjectPtr<USoulDataAsset> SoulData;

	/** 魂タイプタグ（DataAssetがない場合に使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "魂")
	FGameplayTag SoulTypeTag;

	/** 浮遊の振幅 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動作", meta = (ClampMin = "0"))
	float FloatAmplitude = 20.0f;

	/** 浮遊の速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動作", meta = (ClampMin = "0"))
	float FloatSpeed = 2.0f;

	/** 回転速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動作")
	float RotationSpeed = 90.0f;

	/** 吸い寄せ半径 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動作", meta = (ClampMin = "0"))
	float AttractionRadius = 300.0f;

	/** 吸い寄せ速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動作", meta = (ClampMin = "0"))
	float AttractionSpeed = 800.0f;

	/** 収集半径 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動作", meta = (ClampMin = "0"))
	float CollectionRadius = 50.0f;

	/** 存在時間（0で無限） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "動作", meta = (ClampMin = "0"))
	float LifeTime = 30.0f;

	// ========================================================================
	// 内部状態
	// ========================================================================

	/** 初期位置 */
	FVector SpawnLocation;

	/** 経過時間（浮遊用） */
	float ElapsedTime;

	/** 吸い寄せ対象のプレイヤー */
	UPROPERTY()
	TWeakObjectPtr<AActor> AttractTarget;

	/** 収集済みフラグ */
	bool bCollected;

	// ========================================================================
	// イベントハンドラ
	// ========================================================================

	/** 吸い寄せトリガーにプレイヤーが入った */
	UFUNCTION()
	void OnAttractionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** コリジョンにプレイヤーが接触 */
	UFUNCTION()
	void OnCollectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 収集処理 */
	void CollectSoul(AActor* Collector);

	/** 浮遊更新 */
	void UpdateFloating(float DeltaTime);

	/** 吸い寄せ更新 */
	void UpdateAttraction(float DeltaTime);
};
