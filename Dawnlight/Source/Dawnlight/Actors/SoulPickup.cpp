// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulPickup.h"
#include "Dawnlight.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Data/SoulDataAsset.h"
#include "Subsystems/SoulCollectionSubsystem.h"

ASoulPickup::ASoulPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	// ルートコンポーネント
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->SetGenerateOverlapEvents(true);
	RootComponent = CollisionComponent;

	// メッシュ（オプション）
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// VFXコンポーネント
	VFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFXComponent"));
	VFXComponent->SetupAttachment(RootComponent);

	// 吸い寄せトリガー
	AttractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("AttractionTrigger"));
	AttractionTrigger->SetupAttachment(RootComponent);
	AttractionTrigger->SetSphereRadius(300.0f);
	AttractionTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	AttractionTrigger->SetGenerateOverlapEvents(true);

	// デフォルト値
	FloatAmplitude = 20.0f;
	FloatSpeed = 2.0f;
	RotationSpeed = 90.0f;
	AttractionRadius = 300.0f;
	AttractionSpeed = 800.0f;
	CollectionRadius = 50.0f;
	LifeTime = 30.0f;

	ElapsedTime = 0.0f;
	bCollected = false;
}

void ASoulPickup::BeginPlay()
{
	Super::BeginPlay();

	// 初期位置を保存
	SpawnLocation = GetActorLocation();

	// 吸い寄せトリガーの半径を設定
	AttractionTrigger->SetSphereRadius(AttractionRadius);

	// コリジョンの半径を設定
	CollisionComponent->SetSphereRadius(CollectionRadius);

	// オーバーラップイベントをバインド
	AttractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASoulPickup::OnAttractionBeginOverlap);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASoulPickup::OnCollectionBeginOverlap);

	// 存在時間でDestroy
	if (LifeTime > 0.0f)
	{
		SetLifeSpan(LifeTime);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulPickup] スポーン: %s"), *SoulTypeTag.ToString());
}

void ASoulPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCollected)
	{
		return;
	}

	ElapsedTime += DeltaTime;

	// 浮遊更新
	UpdateFloating(DeltaTime);

	// 吸い寄せ更新
	UpdateAttraction(DeltaTime);
}

void ASoulPickup::InitializeSoul(USoulDataAsset* InSoulData)
{
	SoulData = InSoulData;

	if (SoulData)
	{
		SoulTypeTag = SoulData->SoulTag;

		// VFXを設定（CollectNiagaraEffectをIdleにも使用）
		if (SoulData->CollectNiagaraEffect.IsValid() && VFXComponent)
		{
			UNiagaraSystem* NiagaraAsset = SoulData->CollectNiagaraEffect.LoadSynchronous();
			if (NiagaraAsset)
			{
				VFXComponent->SetAsset(NiagaraAsset);
				VFXComponent->Activate();
			}
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulPickup] 初期化: %s"),
		SoulData ? *SoulData->DisplayName.ToString() : TEXT("Unknown"));
}

void ASoulPickup::InitializeSoulByTag(FGameplayTag InSoulTypeTag)
{
	SoulTypeTag = InSoulTypeTag;
	SoulData = nullptr;

	// TODO: タグからDataAssetを検索する処理

	UE_LOG(LogDawnlight, Log, TEXT("[SoulPickup] タグで初期化: %s"), *SoulTypeTag.ToString());
}

void ASoulPickup::OnAttractionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCollected)
	{
		return;
	}

	// プレイヤーかどうかチェック
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (OtherActor != PlayerPawn)
	{
		return;
	}

	// 吸い寄せ対象を設定
	AttractTarget = OtherActor;

	UE_LOG(LogDawnlight, Verbose, TEXT("[SoulPickup] プレイヤー接近、吸い寄せ開始"));
}

void ASoulPickup::OnCollectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCollected)
	{
		return;
	}

	// プレイヤーかどうかチェック
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (OtherActor != PlayerPawn)
	{
		return;
	}

	// 収集処理
	CollectSoul(OtherActor);
}

void ASoulPickup::CollectSoul(AActor* Collector)
{
	if (bCollected)
	{
		return;
	}

	bCollected = true;

	// SoulCollectionSubsystemに通知
	if (UWorld* World = GetWorld())
	{
		if (USoulCollectionSubsystem* SoulSystem = World->GetSubsystem<USoulCollectionSubsystem>())
		{
			SoulSystem->CollectSoul(SoulTypeTag, GetActorLocation());
		}
	}

	// 収集VFXを再生
	if (SoulData && SoulData->CollectNiagaraEffect.IsValid())
	{
		UNiagaraSystem* NiagaraAsset = SoulData->CollectNiagaraEffect.LoadSynchronous();
		if (NiagaraAsset)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				NiagaraAsset,
				GetActorLocation()
			);
		}
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SoulPickup] 収集完了: %s → %s"),
		*SoulTypeTag.ToString(), *Collector->GetName());

	// アクターを削除
	Destroy();
}

void ASoulPickup::UpdateFloating(float DeltaTime)
{
	// 吸い寄せ中は浮遊しない
	if (AttractTarget.IsValid())
	{
		return;
	}

	// Sin波で上下に浮遊
	float FloatOffset = FMath::Sin(ElapsedTime * FloatSpeed) * FloatAmplitude;
	FVector NewLocation = SpawnLocation;
	NewLocation.Z += FloatOffset;
	SetActorLocation(NewLocation);

	// 回転
	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
}

void ASoulPickup::UpdateAttraction(float DeltaTime)
{
	if (!AttractTarget.IsValid())
	{
		return;
	}

	AActor* Target = AttractTarget.Get();
	if (!Target)
	{
		AttractTarget.Reset();
		return;
	}

	// ターゲットに向かって移動
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

	FVector NewLocation = CurrentLocation + Direction * AttractionSpeed * DeltaTime;
	SetActorLocation(NewLocation);
}
