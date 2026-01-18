// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScanlineEffectWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

UScanlineEffectWidget::UScanlineEffectWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentEffectIntensity(1.0f)
	, CurrentScanlineIntensity(0.15f)
	, CurrentNoiseIntensity(0.05f)
	, CurrentChromaticAberration(0.002f)
	, CurrentFlickerIntensity(0.02f)
	, CurrentVignetteIntensity(0.3f)
	, CurrentCRTCurve(0.0f)
	, ScanlineOffset(0.0f)
	, NoiseSeed(0.0f)
	, GlitchTimeRemaining(0.0f)
	, DangerPulseTimer(0.0f)
	, bIsDangerPulsing(false)
{
}

void UScanlineEffectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 初期値を基本値から設定
	CurrentScanlineIntensity = BaseScanlineIntensity;
	CurrentNoiseIntensity = BaseNoiseIntensity;
	CurrentChromaticAberration = BaseChromaticAberration;
	CurrentFlickerIntensity = BaseFlickerIntensity;

	// マテリアルインスタンスを作成
	CreateMaterialInstances();
}

void UScanlineEffectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// スキャンラインをスクロール
	ScanlineOffset += ScanlineScrollSpeed * InDeltaTime;
	if (ScanlineOffset > 1000.0f)
	{
		ScanlineOffset -= 1000.0f;
	}

	// ノイズシードを更新
	NoiseSeed += NoiseAnimationSpeed * InDeltaTime;

	// グリッチを更新
	UpdateGlitch(InDeltaTime);

	// 危険パルスを更新
	UpdateDangerPulse(InDeltaTime);

	// マテリアルパラメータを更新
	UpdateMaterialParameters();
}

void UScanlineEffectWidget::SetEffectIntensity(float Intensity)
{
	CurrentEffectIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UScanlineEffectWidget::SetScanlineIntensity(float Intensity)
{
	CurrentScanlineIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UScanlineEffectWidget::SetNoiseIntensity(float Intensity)
{
	CurrentNoiseIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UScanlineEffectWidget::SetChromaticAberration(float Intensity)
{
	CurrentChromaticAberration = FMath::Clamp(Intensity, 0.0f, 0.1f);
}

void UScanlineEffectWidget::SetFlickerIntensity(float Intensity)
{
	CurrentFlickerIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UScanlineEffectWidget::SetCRTCurve(float Intensity)
{
	CurrentCRTCurve = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UScanlineEffectWidget::SetVignetteIntensity(float Intensity)
{
	CurrentVignetteIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UScanlineEffectWidget::SetSurveillanceLevel(float Level)
{
	const float ClampedLevel = FMath::Clamp(Level, 0.0f, 1.0f);

	// 監視レベルに応じてエフェクトを強化
	const float Multiplier = 1.0f + (ClampedLevel * (SurveillanceEffectMultiplier - 1.0f));

	CurrentScanlineIntensity = BaseScanlineIntensity * Multiplier;
	CurrentNoiseIntensity = BaseNoiseIntensity * Multiplier;
	CurrentChromaticAberration = BaseChromaticAberration * Multiplier;
	CurrentFlickerIntensity = BaseFlickerIntensity * Multiplier;

	// 高レベルではビネットも強化
	CurrentVignetteIntensity = 0.3f + (ClampedLevel * 0.4f);
}

void UScanlineEffectWidget::TriggerDangerPulse()
{
	bIsDangerPulsing = true;
	DangerPulseTimer = 0.0f;
}

void UScanlineEffectWidget::TriggerGlitch(float Duration)
{
	GlitchTimeRemaining = Duration;
}

void UScanlineEffectWidget::CreateMaterialInstances()
{
	// スキャンラインMID
	if (ScanlineMaterial && ScanlineOverlay)
	{
		ScanlineMID = UMaterialInstanceDynamic::Create(ScanlineMaterial, this);
		if (ScanlineMID)
		{
			ScanlineOverlay->SetBrushFromMaterial(ScanlineMID);
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
}

void UScanlineEffectWidget::UpdateMaterialParameters()
{
	const float EffectMult = CurrentEffectIntensity;
	const float Flicker = CalculateFlicker();

	// グリッチ中は強いノイズ
	float NoiseIntensity = CurrentNoiseIntensity;
	float ChromaticAberration = CurrentChromaticAberration;
	if (GlitchTimeRemaining > 0.0f)
	{
		const float GlitchStrength = FMath::Sin(GlitchTimeRemaining * 50.0f) * 0.5f + 0.5f;
		NoiseIntensity = FMath::Lerp(CurrentNoiseIntensity, GlitchNoiseIntensity, GlitchStrength);
		ChromaticAberration = FMath::Lerp(CurrentChromaticAberration, 0.02f, GlitchStrength);
	}

	// スキャンラインマテリアル更新
	if (ScanlineMID)
	{
		ScanlineMID->SetScalarParameterValue(TEXT("Intensity"), CurrentScanlineIntensity * EffectMult * (1.0f + Flicker));
		ScanlineMID->SetScalarParameterValue(TEXT("Offset"), ScanlineOffset);
		ScanlineMID->SetScalarParameterValue(TEXT("ChromaticAberration"), ChromaticAberration * EffectMult);
		ScanlineMID->SetScalarParameterValue(TEXT("CRTCurve"), CurrentCRTCurve);
	}

	// ノイズマテリアル更新
	if (NoiseMID)
	{
		NoiseMID->SetScalarParameterValue(TEXT("Intensity"), NoiseIntensity * EffectMult);
		NoiseMID->SetScalarParameterValue(TEXT("Seed"), NoiseSeed);
	}

	// ビネットオーバーレイ更新
	if (VignetteOverlay)
	{
		// 危険パルス中は赤みを追加
		FLinearColor VignetteColor = FLinearColor(0.0f, 0.0f, 0.0f, CurrentVignetteIntensity * EffectMult);
		if (bIsDangerPulsing)
		{
			const float PulseAlpha = FMath::Sin(DangerPulseTimer * 10.0f) * 0.5f + 0.5f;
			VignetteColor = FLinearColor(0.3f * PulseAlpha, 0.0f, 0.0f, CurrentVignetteIntensity * EffectMult + PulseAlpha * 0.2f);
		}
		VignetteOverlay->SetColorAndOpacity(VignetteColor);
	}

	// スキャンラインオーバーレイの可視性
	if (ScanlineOverlay)
	{
		ScanlineOverlay->SetRenderOpacity(EffectMult);
	}

	// ノイズオーバーレイの可視性
	if (NoiseOverlay)
	{
		NoiseOverlay->SetRenderOpacity(EffectMult);
	}
}

float UScanlineEffectWidget::CalculateFlicker() const
{
	if (CurrentFlickerIntensity <= 0.0f)
	{
		return 0.0f;
	}

	// 複数の周波数を組み合わせて自然なフリッカー
	const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	const float Flicker1 = FMath::Sin(Time * 60.0f) * 0.3f;
	const float Flicker2 = FMath::Sin(Time * 120.0f + 1.5f) * 0.2f;
	const float Flicker3 = FMath::PerlinNoise1D(Time * 5.0f) * 0.5f;

	return (Flicker1 + Flicker2 + Flicker3) * CurrentFlickerIntensity;
}

void UScanlineEffectWidget::UpdateGlitch(float DeltaTime)
{
	if (GlitchTimeRemaining > 0.0f)
	{
		GlitchTimeRemaining -= DeltaTime;
		if (GlitchTimeRemaining < 0.0f)
		{
			GlitchTimeRemaining = 0.0f;
		}
	}
}

void UScanlineEffectWidget::UpdateDangerPulse(float DeltaTime)
{
	if (bIsDangerPulsing)
	{
		DangerPulseTimer += DeltaTime;

		// 2秒後に自動停止
		if (DangerPulseTimer > 2.0f)
		{
			bIsDangerPulsing = false;
			DangerPulseTimer = 0.0f;
		}
	}
}
