// Copyright Epic Games, Inc. All Rights Reserved.

#include "GlitchEffectWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

UGlitchEffectWidget::UGlitchEffectWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsGlitching(false)
	, bIsContinuousGlitch(false)
	, CurrentGlitchType(EGlitchType::None)
	, CurrentGlitchIntensity(0.0f)
	, GlitchTimeRemaining(0.0f)
	, StaticBurstTimeRemaining(0.0f)
	, RGBShiftTimeRemaining(0.0f)
	, ScreenShiftTimeRemaining(0.0f)
	, NextGlitchTimer(0.0f)
	, CurrentRGBOffset(FVector2D::ZeroVector)
	, CurrentScreenShift(FVector2D::ZeroVector)
	, NoiseSeed(0.0f)
{
}

void UGlitchEffectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// マテリアルインスタンス作成
	CreateMaterialInstances();

	// 初期状態は非表示
	UpdateOverlayVisibility();
}

void UGlitchEffectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ノイズシード更新
	NoiseSeed += InDeltaTime * 60.0f;

	// グリッチ更新
	UpdateGlitch(InDeltaTime);

	// 継続グリッチ更新
	UpdateContinuousGlitch(InDeltaTime);

	// エフェクト更新
	UpdateEffects(InDeltaTime);

	// マテリアルパラメータ更新
	UpdateMaterialParameters();

	// オーバーレイ可視性更新
	UpdateOverlayVisibility();
}

void UGlitchEffectWidget::TriggerGlitch(EGlitchType Type, float Duration)
{
	if (Type == EGlitchType::None)
	{
		return;
	}

	bIsGlitching = true;
	CurrentGlitchType = Type;
	CurrentGlitchIntensity = GetIntensityForType(Type);
	GlitchTimeRemaining = Duration;

	// サウンド再生
	PlayGlitchSound(Type);

	// 追加効果をトリガー
	ApplyRandomGlitchEffect();
}

void UGlitchEffectWidget::TriggerSystemAwareness(float Duration)
{
	TriggerGlitch(EGlitchType::SystemAwareness, Duration);

	// システム覚醒専用の演出
	// 複数のグリッチ効果を重ねがけ
	TriggerRGBShift(Duration * 0.5f);
	TriggerScreenShift(Duration * 0.3f);

	// 特別なサウンド
	if (SystemAwarenessSound)
	{
		UGameplayStatics::PlaySound2D(this, SystemAwarenessSound);
	}
}

void UGlitchEffectWidget::StartContinuousGlitch(EGlitchType Type)
{
	bIsContinuousGlitch = true;
	CurrentGlitchType = Type;
	CurrentGlitchIntensity = GetIntensityForType(Type);
	NextGlitchTimer = MinGlitchInterval;
}

void UGlitchEffectWidget::StopContinuousGlitch()
{
	bIsContinuousGlitch = false;
	bIsGlitching = false;
	CurrentGlitchType = EGlitchType::None;
	CurrentGlitchIntensity = 0.0f;
}

void UGlitchEffectWidget::SetGlitchIntensity(float Intensity)
{
	CurrentGlitchIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UGlitchEffectWidget::TriggerStaticBurst(float Duration)
{
	StaticBurstTimeRemaining = Duration;
}

void UGlitchEffectWidget::TriggerRGBShift(float Duration)
{
	RGBShiftTimeRemaining = Duration;

	// ランダムなオフセット方向
	CurrentRGBOffset.X = FMath::RandRange(-MaxRGBOffset, MaxRGBOffset);
	CurrentRGBOffset.Y = FMath::RandRange(-MaxRGBOffset, MaxRGBOffset);
}

void UGlitchEffectWidget::TriggerScreenShift(float Duration)
{
	ScreenShiftTimeRemaining = Duration;

	// ランダムなシフト方向
	CurrentScreenShift.X = FMath::RandRange(-MaxScreenShift, MaxScreenShift);
	CurrentScreenShift.Y = FMath::RandRange(-MaxScreenShift * 0.5f, MaxScreenShift * 0.5f);
}

void UGlitchEffectWidget::CreateMaterialInstances()
{
	// グリッチMID
	if (GlitchMaterial && GlitchOverlay)
	{
		GlitchMID = UMaterialInstanceDynamic::Create(GlitchMaterial, this);
		if (GlitchMID)
		{
			GlitchOverlay->SetBrushFromMaterial(GlitchMID);
		}
	}

	// ノイズMID
	if (NoiseMaterial && NoiseOverlay)
	{
		NoiseMID = UMaterialInstanceDynamic::Create(NoiseMaterial, this);
		if (NoiseMID)
		{
			NoiseOverlay->SetBrushFromMaterial(NoiseMID);
		}
	}

	// RGBシフトMID
	if (RGBShiftMaterial && RGBShiftOverlay)
	{
		RGBShiftMID = UMaterialInstanceDynamic::Create(RGBShiftMaterial, this);
		if (RGBShiftMID)
		{
			RGBShiftOverlay->SetBrushFromMaterial(RGBShiftMID);
		}
	}
}

void UGlitchEffectWidget::UpdateGlitch(float DeltaTime)
{
	if (GlitchTimeRemaining > 0.0f)
	{
		GlitchTimeRemaining -= DeltaTime;

		if (GlitchTimeRemaining <= 0.0f)
		{
			GlitchTimeRemaining = 0.0f;

			if (!bIsContinuousGlitch)
			{
				bIsGlitching = false;
				CurrentGlitchType = EGlitchType::None;
				CurrentGlitchIntensity = 0.0f;
			}
		}
	}
}

void UGlitchEffectWidget::UpdateContinuousGlitch(float DeltaTime)
{
	if (!bIsContinuousGlitch)
	{
		return;
	}

	NextGlitchTimer -= DeltaTime;

	if (NextGlitchTimer <= 0.0f)
	{
		// 次のグリッチまでの時間をリセット
		NextGlitchTimer = FMath::RandRange(MinGlitchInterval, MinGlitchInterval * 3.0f);

		// 確率でグリッチ効果を適用
		if (FMath::FRand() < GlitchProbability)
		{
			bIsGlitching = true;
			GlitchTimeRemaining = FMath::RandRange(0.02f, 0.1f);
			ApplyRandomGlitchEffect();
		}
	}
}

void UGlitchEffectWidget::UpdateEffects(float DeltaTime)
{
	// スタティックバースト
	if (StaticBurstTimeRemaining > 0.0f)
	{
		StaticBurstTimeRemaining -= DeltaTime;
	}

	// RGBシフト
	if (RGBShiftTimeRemaining > 0.0f)
	{
		RGBShiftTimeRemaining -= DeltaTime;

		// シフトを徐々に減衰
		const float ShiftMultiplier = RGBShiftTimeRemaining > 0.0f ? 1.0f : 0.0f;
		CurrentRGBOffset *= FMath::Lerp(1.0f, 0.0f, DeltaTime * 10.0f);
	}
	else
	{
		CurrentRGBOffset = FVector2D::ZeroVector;
	}

	// 画面シフト
	if (ScreenShiftTimeRemaining > 0.0f)
	{
		ScreenShiftTimeRemaining -= DeltaTime;

		// ランダムな揺れを追加
		if (bIsGlitching)
		{
			CurrentScreenShift.X += FMath::RandRange(-5.0f, 5.0f);
			CurrentScreenShift.Y += FMath::RandRange(-2.0f, 2.0f);
		}
	}
	else
	{
		CurrentScreenShift = FMath::Lerp(CurrentScreenShift, FVector2D::ZeroVector, DeltaTime * 10.0f);
	}
}

void UGlitchEffectWidget::UpdateMaterialParameters()
{
	const bool bShowEffects = bIsGlitching || StaticBurstTimeRemaining > 0.0f || RGBShiftTimeRemaining > 0.0f;

	// グリッチマテリアル
	if (GlitchMID)
	{
		GlitchMID->SetScalarParameterValue(TEXT("Intensity"), bIsGlitching ? CurrentGlitchIntensity : 0.0f);
		GlitchMID->SetScalarParameterValue(TEXT("Time"), NoiseSeed);
		GlitchMID->SetScalarParameterValue(TEXT("ScreenShiftX"), CurrentScreenShift.X);
		GlitchMID->SetScalarParameterValue(TEXT("ScreenShiftY"), CurrentScreenShift.Y);
	}

	// ノイズマテリアル
	if (NoiseMID)
	{
		const float NoiseIntensity = StaticBurstTimeRemaining > 0.0f ? 0.8f : (bIsGlitching ? CurrentGlitchIntensity * 0.5f : 0.0f);
		NoiseMID->SetScalarParameterValue(TEXT("Intensity"), NoiseIntensity);
		NoiseMID->SetScalarParameterValue(TEXT("Seed"), NoiseSeed);
	}

	// RGBシフトマテリアル
	if (RGBShiftMID)
	{
		const float RGBIntensity = RGBShiftTimeRemaining > 0.0f ? 1.0f : (bIsGlitching ? CurrentGlitchIntensity * 0.3f : 0.0f);
		RGBShiftMID->SetScalarParameterValue(TEXT("Intensity"), RGBIntensity);
		RGBShiftMID->SetScalarParameterValue(TEXT("OffsetX"), CurrentRGBOffset.X);
		RGBShiftMID->SetScalarParameterValue(TEXT("OffsetY"), CurrentRGBOffset.Y);
	}
}

float UGlitchEffectWidget::GetIntensityForType(EGlitchType Type) const
{
	switch (Type)
	{
	case EGlitchType::Minor:
		return MinorIntensity;
	case EGlitchType::Moderate:
		return ModerateIntensity;
	case EGlitchType::Severe:
		return SevereIntensity;
	case EGlitchType::SystemAwareness:
		return SystemAwarenessIntensity;
	default:
		return 0.0f;
	}
}

void UGlitchEffectWidget::ApplyRandomGlitchEffect()
{
	const int32 EffectType = FMath::RandRange(0, 3);

	switch (EffectType)
	{
	case 0:
		TriggerStaticBurst(FMath::RandRange(0.02f, 0.1f));
		break;
	case 1:
		TriggerRGBShift(FMath::RandRange(0.05f, 0.2f));
		break;
	case 2:
		TriggerScreenShift(FMath::RandRange(0.03f, 0.15f));
		break;
	case 3:
		// 複合効果
		TriggerStaticBurst(0.05f);
		TriggerRGBShift(0.1f);
		break;
	}
}

void UGlitchEffectWidget::UpdateOverlayVisibility()
{
	const bool bShouldShow = bIsGlitching || StaticBurstTimeRemaining > 0.0f || RGBShiftTimeRemaining > 0.0f || ScreenShiftTimeRemaining > 0.0f;

	if (GlitchOverlay)
	{
		GlitchOverlay->SetVisibility(bShouldShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (NoiseOverlay)
	{
		const bool bShowNoise = bIsGlitching || StaticBurstTimeRemaining > 0.0f;
		NoiseOverlay->SetVisibility(bShowNoise ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (RGBShiftOverlay)
	{
		const bool bShowRGB = RGBShiftTimeRemaining > 0.0f || (bIsGlitching && CurrentGlitchIntensity > 0.3f);
		RGBShiftOverlay->SetVisibility(bShowRGB ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (BlockNoiseOverlay)
	{
		const bool bShowBlock = bIsGlitching && CurrentGlitchIntensity > 0.5f;
		BlockNoiseOverlay->SetVisibility(bShowBlock ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UGlitchEffectWidget::PlayGlitchSound(EGlitchType Type)
{
	USoundBase* SoundToPlay = nullptr;

	switch (Type)
	{
	case EGlitchType::Minor:
	case EGlitchType::Moderate:
		SoundToPlay = GlitchSound;
		break;
	case EGlitchType::Severe:
		SoundToPlay = StaticSound;
		break;
	case EGlitchType::SystemAwareness:
		SoundToPlay = SystemAwarenessSound;
		break;
	default:
		break;
	}

	if (SoundToPlay)
	{
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
	}
}
