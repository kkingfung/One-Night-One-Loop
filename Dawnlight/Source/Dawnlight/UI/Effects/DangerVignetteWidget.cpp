// Copyright Epic Games, Inc. All Rights Reserved.

#include "DangerVignetteWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

UDangerVignetteWidget::UDangerVignetteWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetVignetteIntensity(0.3f)
	, CurrentVignetteIntensity(0.3f)
	, TargetVignetteColor(FLinearColor::Black)
	, CurrentVignetteColor(FLinearColor::Black)
	, CurrentDangerLevel(0.0f)
	, CurrentSurveillanceLevel(0.0f)
	, bIsHiding(false)
	, FlashTimer(0.0f)
	, FlashColor(FLinearColor::Black)
	, FlashIntensity(0.0f)
	, bIsBreathing(false)
	, BreathingTimer(0.0f)
	, HeartbeatTimer(0.0f)
{
}

void UDangerVignetteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 初期値設定
	TargetVignetteIntensity = BaseVignetteIntensity;
	CurrentVignetteIntensity = BaseVignetteIntensity;
	TargetVignetteColor = NormalColor;
	CurrentVignetteColor = NormalColor;

	// マテリアルインスタンス作成
	CreateMaterialInstance();
}

void UDangerVignetteWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 目標値を計算
	CalculateTargetValues();

	// ビネット更新
	UpdateVignette(InDeltaTime);

	// フラッシュ更新
	UpdateFlash(InDeltaTime);

	// 呼吸パルス更新
	UpdateBreathing(InDeltaTime);

	// マテリアルパラメータ更新
	UpdateMaterialParameters();
}

void UDangerVignetteWidget::SetDangerLevel(float Level)
{
	CurrentDangerLevel = FMath::Clamp(Level, 0.0f, 1.0f);
}

void UDangerVignetteWidget::SetSurveillanceLevel(float Level)
{
	CurrentSurveillanceLevel = FMath::Clamp(Level, 0.0f, 1.0f);
}

void UDangerVignetteWidget::SetHidingState(bool bNewIsHiding)
{
	bIsHiding = bNewIsHiding;

	// 隠れ始めたら安全フラッシュ
	if (bIsHiding)
	{
		TriggerSafeFlash();
	}
}

void UDangerVignetteWidget::SetVignetteIntensity(float Intensity)
{
	TargetVignetteIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UDangerVignetteWidget::SetVignetteColor(const FLinearColor& Color)
{
	TargetVignetteColor = Color;
}

void UDangerVignetteWidget::TriggerDamageFlash(float Intensity)
{
	FlashTimer = FlashDuration;
	FlashColor = DamageFlashColor;
	FlashIntensity = Intensity;
}

void UDangerVignetteWidget::TriggerDetectionFlash()
{
	FlashTimer = FlashDuration * 0.5f;
	FlashColor = DetectionFlashColor;
	FlashIntensity = 1.0f;
}

void UDangerVignetteWidget::TriggerSafeFlash()
{
	FlashTimer = FlashDuration * 0.7f;
	FlashColor = SafeFlashColor;
	FlashIntensity = 0.8f;
}

void UDangerVignetteWidget::StartBreathingPulse()
{
	bIsBreathing = true;
	BreathingTimer = 0.0f;
}

void UDangerVignetteWidget::StopBreathingPulse()
{
	bIsBreathing = false;
}

void UDangerVignetteWidget::TriggerHeartbeatPulse()
{
	HeartbeatTimer = 0.5f;
}

void UDangerVignetteWidget::CreateMaterialInstance()
{
	if (VignetteMaterial && VignetteImage)
	{
		VignetteMID = UMaterialInstanceDynamic::Create(VignetteMaterial, this);
		if (VignetteMID)
		{
			VignetteImage->SetBrushFromMaterial(VignetteMID);
		}
	}
}

void UDangerVignetteWidget::CalculateTargetValues()
{
	// 隠れ状態
	if (bIsHiding)
	{
		TargetVignetteIntensity = HidingVignetteIntensity;
		TargetVignetteColor = HidingColor;
		return;
	}

	// 危険度と監視レベルの最大値を使用
	const float ThreatLevel = FMath::Max(CurrentDangerLevel, CurrentSurveillanceLevel);

	// 強度を計算
	TargetVignetteIntensity = FMath::Lerp(BaseVignetteIntensity, MaxVignetteIntensity, ThreatLevel);

	// 色を計算
	TargetVignetteColor = FLinearColor::LerpUsingHSV(NormalColor, DangerColor, ThreatLevel);
}

void UDangerVignetteWidget::UpdateVignette(float DeltaTime)
{
	// スムージング
	CurrentVignetteIntensity = FMath::FInterpTo(CurrentVignetteIntensity, TargetVignetteIntensity, DeltaTime, SmoothSpeed);

	// 色のスムージング
	CurrentVignetteColor.R = FMath::FInterpTo(CurrentVignetteColor.R, TargetVignetteColor.R, DeltaTime, SmoothSpeed);
	CurrentVignetteColor.G = FMath::FInterpTo(CurrentVignetteColor.G, TargetVignetteColor.G, DeltaTime, SmoothSpeed);
	CurrentVignetteColor.B = FMath::FInterpTo(CurrentVignetteColor.B, TargetVignetteColor.B, DeltaTime, SmoothSpeed);
	CurrentVignetteColor.A = FMath::FInterpTo(CurrentVignetteColor.A, TargetVignetteColor.A, DeltaTime, SmoothSpeed);
}

void UDangerVignetteWidget::UpdateFlash(float DeltaTime)
{
	if (FlashTimer > 0.0f)
	{
		FlashTimer -= DeltaTime;

		if (FlashImage)
		{
			// フラッシュの減衰
			const float FlashAlpha = (FlashTimer / FlashDuration) * FlashIntensity;
			FLinearColor DisplayFlashColor = FlashColor;
			DisplayFlashColor.A *= FlashAlpha;
			FlashImage->SetColorAndOpacity(DisplayFlashColor);
		}
	}
	else if (FlashImage)
	{
		FlashImage->SetColorAndOpacity(FLinearColor::Transparent);
	}
}

void UDangerVignetteWidget::UpdateBreathing(float DeltaTime)
{
	if (!bIsBreathing)
	{
		return;
	}

	BreathingTimer += DeltaTime * BreathingSpeed;

	// 呼吸のような波形（吸う->止める->吐く->止める）
	const float BreathCycle = FMath::Sin(BreathingTimer * PI * 2.0f);
	const float BreathPulse = (BreathCycle * 0.5f + 0.5f) * BreathingAmplitude;

	// 強度に加算
	CurrentVignetteIntensity += BreathPulse;
}

void UDangerVignetteWidget::UpdateMaterialParameters()
{
	// 心拍パルスの追加効果
	float FinalIntensity = CurrentVignetteIntensity;
	if (HeartbeatTimer > 0.0f)
	{
		HeartbeatTimer -= GetWorld()->GetDeltaSeconds();
		// ダブルビート（ドクドク）
		const float Beat1 = FMath::Max(0.0f, FMath::Sin(HeartbeatTimer * HeartbeatSpeed * PI * 2.0f));
		const float Beat2 = FMath::Max(0.0f, FMath::Sin((HeartbeatTimer - 0.15f) * HeartbeatSpeed * PI * 2.0f));
		const float Heartbeat = FMath::Max(Beat1, Beat2) * 0.15f;
		FinalIntensity += Heartbeat;
	}

	// マテリアルパラメータ更新
	if (VignetteMID)
	{
		VignetteMID->SetScalarParameterValue(TEXT("Intensity"), FinalIntensity);
		VignetteMID->SetScalarParameterValue(TEXT("Radius"), VignetteRadius);
		VignetteMID->SetScalarParameterValue(TEXT("Softness"), VignetteSoftness);
		VignetteMID->SetVectorParameterValue(TEXT("Color"), CurrentVignetteColor);
	}

	// イメージに直接適用（マテリアルがない場合のフォールバック）
	if (VignetteImage && !VignetteMID)
	{
		FLinearColor DisplayColor = CurrentVignetteColor;
		DisplayColor.A = FinalIntensity;
		VignetteImage->SetColorAndOpacity(DisplayColor);
	}
}
