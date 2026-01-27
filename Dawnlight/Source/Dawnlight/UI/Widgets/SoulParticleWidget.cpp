// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "SoulParticleWidget.h"
#include "Dawnlight.h"
#include "Rendering/DrawElements.h"

USoulParticleWidget::USoulParticleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MaxParticles(50)
	, EmissionRate(5.0f)
	, MinParticleSize(3.0f)
	, MaxParticleSize(12.0f)
	, MinLifetime(3.0f)
	, MaxLifetime(8.0f)
	, MinRiseSpeed(20.0f)
	, MaxRiseSpeed(60.0f)
	, SwayStrength(30.0f)
	, MinPulseSpeed(1.0f)
	, MaxPulseSpeed(3.0f)
	, PrimaryColor(0.5f, 0.2f, 0.7f, 1.0f)       // 紫
	, SecondaryColor(1.0f, 0.85f, 0.0f, 1.0f)    // 金
	, GoldenParticleChance(0.1f)
	, EmissionTimer(0.0f)
	, bEmitting(false)
	, TotalTime(0.0f)
{
}

void USoulParticleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// パーティクル配列を予約
	Particles.Reserve(MaxParticles);

	// 自動的に発生開始
	StartEmission();

	UE_LOG(LogDawnlight, Log, TEXT("[SoulParticleWidget] 初期化完了"));
}

void USoulParticleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TotalTime += InDeltaTime;

	// パーティクル発生
	if (bEmitting)
	{
		EmissionTimer += InDeltaTime;
		const float EmissionInterval = 1.0f / EmissionRate;

		while (EmissionTimer >= EmissionInterval && Particles.Num() < MaxParticles)
		{
			SpawnParticle();
			EmissionTimer -= EmissionInterval;
		}
	}

	// パーティクル更新
	UpdateParticles(InDeltaTime);
}

int32 USoulParticleWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
	int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// 親の描画
	LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements,
		LayerId, InWidgetStyle, bParentEnabled);

	// 各パーティクルを描画
	for (const FSoulParticle& Particle : Particles)
	{
		if (Particle.bActive)
		{
			DrawParticle(Particle, AllottedGeometry, OutDrawElements, LayerId);
		}
	}

	return LayerId + 1;
}

void USoulParticleWidget::StartEmission()
{
	bEmitting = true;
	EmissionTimer = 0.0f;
}

void USoulParticleWidget::StopEmission()
{
	bEmitting = false;
}

void USoulParticleWidget::ClearParticles()
{
	Particles.Empty();
}

void USoulParticleWidget::EmitBurst(int32 Count)
{
	for (int32 i = 0; i < Count && Particles.Num() < MaxParticles; ++i)
	{
		SpawnParticle();
	}
}

void USoulParticleWidget::SpawnParticle()
{
	FSoulParticle NewParticle;

	// 位置（画面下端からランダムな横位置）
	NewParticle.Position.X = FMath::FRand() * 100.0f;  // パーセント
	NewParticle.Position.Y = 100.0f + FMath::FRand() * 10.0f;  // 画面下端

	// 速度（上向き＋若干の横ずれ）
	NewParticle.Velocity.X = (FMath::FRand() - 0.5f) * 10.0f;
	NewParticle.Velocity.Y = -FMath::FRandRange(MinRiseSpeed, MaxRiseSpeed);

	// サイズ
	NewParticle.Size = FMath::FRandRange(MinParticleSize, MaxParticleSize);

	// 色（確率で金色）
	if (FMath::FRand() < GoldenParticleChance)
	{
		NewParticle.Color = SecondaryColor;
		NewParticle.Size *= 1.5f;  // 金色は少し大きく
	}
	else
	{
		// 紫のバリエーション
		float Hue = FMath::FRandRange(0.7f, 0.9f);  // 紫～青紫
		NewParticle.Color = FLinearColor::LerpUsingHSV(
			PrimaryColor,
			FLinearColor(0.3f, 0.1f, 0.6f, 1.0f),
			FMath::FRand()
		);
	}

	// 透明度とパルス
	NewParticle.Alpha = 0.0f;  // フェードインから開始
	NewParticle.PulseTimer = FMath::FRand() * PI * 2.0f;  // ランダムなフェーズで開始
	NewParticle.PulseSpeed = FMath::FRandRange(MinPulseSpeed, MaxPulseSpeed);

	// ライフタイム
	NewParticle.Lifetime = 0.0f;
	NewParticle.MaxLifetime = FMath::FRandRange(MinLifetime, MaxLifetime);

	NewParticle.bActive = true;

	Particles.Add(NewParticle);
}

void USoulParticleWidget::UpdateParticles(float DeltaTime)
{
	for (int32 i = Particles.Num() - 1; i >= 0; --i)
	{
		FSoulParticle& Particle = Particles[i];

		if (!Particle.bActive)
		{
			Particles.RemoveAt(i);
			continue;
		}

		// ライフタイム更新
		Particle.Lifetime += DeltaTime;
		if (Particle.Lifetime >= Particle.MaxLifetime)
		{
			Particle.bActive = false;
			continue;
		}

		// 位置更新
		Particle.Position += Particle.Velocity * DeltaTime * 0.1f;  // パーセント単位

		// 横揺れ（サイン波）
		float SwayOffset = FMath::Sin(TotalTime * 2.0f + Particle.PulseTimer) * SwayStrength * 0.01f;
		Particle.Position.X += SwayOffset * DeltaTime;

		// 画面外に出たら非アクティブ化
		if (Particle.Position.Y < -10.0f || Particle.Position.X < -10.0f || Particle.Position.X > 110.0f)
		{
			Particle.bActive = false;
			continue;
		}

		// パルスタイマー更新
		Particle.PulseTimer += Particle.PulseSpeed * DeltaTime;

		// 透明度の計算（フェードイン→持続→フェードアウト）
		float LifeProgress = Particle.Lifetime / Particle.MaxLifetime;
		float FadeInDuration = 0.2f;
		float FadeOutStart = 0.7f;

		if (LifeProgress < FadeInDuration)
		{
			// フェードイン
			Particle.Alpha = LifeProgress / FadeInDuration;
		}
		else if (LifeProgress > FadeOutStart)
		{
			// フェードアウト
			Particle.Alpha = 1.0f - (LifeProgress - FadeOutStart) / (1.0f - FadeOutStart);
		}
		else
		{
			// 持続
			Particle.Alpha = 1.0f;
		}

		// パルス効果を適用
		float PulseMultiplier = 0.5f + 0.5f * FMath::Sin(Particle.PulseTimer);
		Particle.Alpha *= FMath::Lerp(0.6f, 1.0f, PulseMultiplier);
	}
}

void USoulParticleWidget::DrawParticle(const FSoulParticle& Particle, const FGeometry& AllottedGeometry,
	FSlateWindowElementList& OutDrawElements, int32 LayerId) const
{
	// パーセント位置を実際の位置に変換
	FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	FVector2D Position(
		Particle.Position.X * LocalSize.X * 0.01f,
		Particle.Position.Y * LocalSize.Y * 0.01f
	);

	// パーティクルの四角形を定義
	float HalfSize = Particle.Size * 0.5f;
	TArray<FSlateVertex> Vertices;
	TArray<SlateIndex> Indices;

	// 色を設定
	FLinearColor FinalColor = Particle.Color;
	FinalColor.A = Particle.Alpha * 0.8f;  // 最大透明度を80%に
	FColor VertexColor = FinalColor.ToFColor(true);

	// 頂点を追加（円形に見せるために角を丸める代わりに、シンプルな四角形で光る効果）
	// 中心から外側に向かってグラデーション
	FVector2D Center = Position;

	// 光るエフェクトのための外側のグロー
	float GlowSize = Particle.Size * 2.0f;

	// 内側の明るい部分
	FColor CoreColor = FColor::White;
	CoreColor.A = static_cast<uint8>(Particle.Alpha * 200);

	// Box（シンプルな四角形として描画）
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(
			FVector2D(Center.X - HalfSize, Center.Y - HalfSize),
			FVector2D(Particle.Size, Particle.Size)
		),
		FCoreStyle::Get().GetBrush("GenericWhiteBox"),
		ESlateDrawEffect::None,
		FinalColor.ToFColor(true)
	);

	// 中心のより明るい点
	float CoreSize = Particle.Size * 0.4f;
	float CoreHalfSize = CoreSize * 0.5f;
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(
			FVector2D(Center.X - CoreHalfSize, Center.Y - CoreHalfSize),
			FVector2D(CoreSize, CoreSize)
		),
		FCoreStyle::Get().GetBrush("GenericWhiteBox"),
		ESlateDrawEffect::None,
		CoreColor
	);
}
