// Copyright Epic Games, Inc. All Rights Reserved.

#include "DetectionGaugeWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

UDetectionGaugeWidget::UDetectionGaugeWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetDetectionLevel(0.0f)
	, CurrentDetectionLevel(0.0f)
	, CurrentState(EDetectionState::Safe)
	, PulseTimer(0.0f)
	, AutoHideTimer(0.0f)
	, bAlwaysVisible(false)
	, bIsVisible(false)
	, bIsFading(false)
	, bIsFadingIn(true)
	, FadeProgress(0.0f)
	, WarningPulseTimer(0.0f)
	, DetectedFlashTimer(0.0f)
{
}

void UDetectionGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 初期状態は非表示
	SetRenderOpacity(0.0f);
	bIsVisible = false;
}

void UDetectionGaugeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ゲージ更新
	UpdateGauge(InDeltaTime);

	// 状態更新
	UpdateState();

	// 色更新
	UpdateColors();

	// パルスエフェクト
	UpdatePulseEffects(InDeltaTime);

	// アイコン更新
	UpdateIcon(InDeltaTime);

	// フェード更新
	UpdateFade(InDeltaTime);

	// 自動非表示
	UpdateAutoHide(InDeltaTime);
}

void UDetectionGaugeWidget::SetDetectionLevel(float Level)
{
	TargetDetectionLevel = FMath::Clamp(Level, 0.0f, 1.0f);

	// 検知中は表示
	if (TargetDetectionLevel > 0.01f && !bIsVisible)
	{
		ShowGauge();
	}

	// 自動非表示タイマーリセット
	AutoHideTimer = 0.0f;
}

void UDetectionGaugeWidget::ResetGauge()
{
	TargetDetectionLevel = 0.0f;
	CurrentDetectionLevel = 0.0f;
	CurrentState = EDetectionState::Safe;
	PulseTimer = 0.0f;
}

void UDetectionGaugeWidget::ShowGauge()
{
	if (!bIsVisible || bIsFading)
	{
		bIsFading = true;
		bIsFadingIn = true;
		FadeProgress = GetRenderOpacity();
		bIsVisible = true;
	}
}

void UDetectionGaugeWidget::HideGauge()
{
	if (bIsVisible || bIsFading)
	{
		bIsFading = true;
		bIsFadingIn = false;
		FadeProgress = GetRenderOpacity();
	}
}

void UDetectionGaugeWidget::SetAlwaysVisible(bool bNewAlwaysVisible)
{
	bAlwaysVisible = bNewAlwaysVisible;
	if (bAlwaysVisible && !bIsVisible)
	{
		ShowGauge();
	}
}

void UDetectionGaugeWidget::TriggerWarningPulse()
{
	WarningPulseTimer = 0.5f;
}

void UDetectionGaugeWidget::TriggerDetectedFlash()
{
	DetectedFlashTimer = 0.3f;

	// 完全検知イベント
	OnFullyDetected.Broadcast();

	// サウンド再生
	if (FullyDetectedSound)
	{
		UGameplayStatics::PlaySound2D(this, FullyDetectedSound);
	}
}

void UDetectionGaugeWidget::UpdateGauge(float DeltaTime)
{
	// スムージング
	CurrentDetectionLevel = FMath::FInterpTo(CurrentDetectionLevel, TargetDetectionLevel, DeltaTime, GaugeSmoothSpeed);

	// プログレスバー更新
	if (MainProgressBar)
	{
		MainProgressBar->SetPercent(CurrentDetectionLevel);
	}

	// グロープログレスバー（少し先行）
	if (GlowProgressBar)
	{
		const float GlowLevel = FMath::Min(CurrentDetectionLevel + 0.05f, 1.0f);
		GlowProgressBar->SetPercent(GlowLevel);
	}
}

void UDetectionGaugeWidget::UpdateState()
{
	EDetectionState NewState;

	if (CurrentDetectionLevel >= CriticalThreshold)
	{
		NewState = EDetectionState::Critical;
	}
	else if (CurrentDetectionLevel >= DangerThreshold)
	{
		NewState = EDetectionState::Danger;
	}
	else if (CurrentDetectionLevel >= WarningThreshold)
	{
		NewState = EDetectionState::Warning;
	}
	else if (CurrentDetectionLevel >= CautionThreshold)
	{
		NewState = EDetectionState::Caution;
	}
	else
	{
		NewState = EDetectionState::Safe;
	}

	// 状態変化
	if (NewState != CurrentState)
	{
		EDetectionState OldState = CurrentState;
		CurrentState = NewState;

		// デリゲート発火
		OnDetectionStateChanged.Broadcast(NewState);

		// サウンド
		PlayStateChangeSound(NewState, OldState);

		// 臨界到達時
		if (NewState == EDetectionState::Critical && OldState != EDetectionState::Critical)
		{
			TriggerWarningPulse();
		}
	}

	// 完全検知チェック
	if (CurrentDetectionLevel >= 1.0f && TargetDetectionLevel >= 1.0f)
	{
		TriggerDetectedFlash();
	}
}

void UDetectionGaugeWidget::UpdateColors()
{
	FLinearColor CurrentColor = GetStateColor();

	// 警告パルス中は白くフラッシュ
	if (WarningPulseTimer > 0.0f)
	{
		const float PulseAlpha = FMath::Sin(WarningPulseTimer * 20.0f) * 0.5f + 0.5f;
		CurrentColor = FLinearColor::LerpUsingHSV(CurrentColor, FLinearColor::White, PulseAlpha * 0.5f);
	}

	// 検知フラッシュ中
	if (DetectedFlashTimer > 0.0f)
	{
		CurrentColor = FLinearColor::LerpUsingHSV(CurrentColor, FLinearColor::White, DetectedFlashTimer / 0.3f);
	}

	// プログレスバーに色適用
	if (MainProgressBar)
	{
		MainProgressBar->SetFillColorAndOpacity(CurrentColor);
	}

	// グローバーに色適用
	if (GlowProgressBar)
	{
		FLinearColor GlowBarColor = CurrentColor;
		GlowBarColor.A = GlowColor.A;
		GlowProgressBar->SetFillColorAndOpacity(GlowBarColor);
	}

	// アイコンに色適用
	if (EyeIcon)
	{
		EyeIcon->SetColorAndOpacity(CurrentColor);
	}

	// ステータステキスト更新
	if (StatusText)
	{
		FText StateText;
		switch (CurrentState)
		{
		case EDetectionState::Safe:
			StateText = FText::FromString(TEXT(""));
			break;
		case EDetectionState::Caution:
			StateText = FText::FromString(TEXT("注意"));
			break;
		case EDetectionState::Warning:
			StateText = FText::FromString(TEXT("警戒"));
			break;
		case EDetectionState::Danger:
			StateText = FText::FromString(TEXT("危険"));
			break;
		case EDetectionState::Critical:
			StateText = FText::FromString(TEXT("検知！"));
			break;
		}
		StatusText->SetText(StateText);
		StatusText->SetColorAndOpacity(FSlateColor(CurrentColor));
	}
}

void UDetectionGaugeWidget::UpdatePulseEffects(float DeltaTime)
{
	// パルスタイマー更新
	PulseTimer += DeltaTime * CriticalPulseSpeed;

	// 警告パルスタイマー減少
	if (WarningPulseTimer > 0.0f)
	{
		WarningPulseTimer -= DeltaTime;
	}

	// 検知フラッシュタイマー減少
	if (DetectedFlashTimer > 0.0f)
	{
		DetectedFlashTimer -= DeltaTime;
	}

	// 臨界状態のパルス
	if (CurrentState == EDetectionState::Critical && GlowProgressBar)
	{
		const float PulseValue = (FMath::Sin(PulseTimer) * 0.5f + 0.5f) * GlowAmplitude;
		GlowProgressBar->SetRenderOpacity(0.5f + PulseValue);
	}
	else if (GlowProgressBar)
	{
		// 通常時のグロー
		const float GlowValue = CurrentDetectionLevel * GlowAmplitude;
		GlowProgressBar->SetRenderOpacity(GlowValue);
	}
}

void UDetectionGaugeWidget::UpdateIcon(float DeltaTime)
{
	if (!EyeIcon)
	{
		return;
	}

	// 危険/臨界状態では揺れる
	if (CurrentState == EDetectionState::Danger || CurrentState == EDetectionState::Critical)
	{
		const float ShakeX = FMath::RandRange(-IconShakeIntensity, IconShakeIntensity);
		const float ShakeY = FMath::RandRange(-IconShakeIntensity * 0.5f, IconShakeIntensity * 0.5f);
		EyeIcon->SetRenderTranslation(FVector2D(ShakeX, ShakeY));

		// 臨界時はスケールもパルス
		if (CurrentState == EDetectionState::Critical)
		{
			const float ScalePulse = 1.0f + FMath::Sin(PulseTimer * 2.0f) * 0.1f;
			EyeIcon->SetRenderScale(FVector2D(ScalePulse, ScalePulse));
		}
	}
	else
	{
		EyeIcon->SetRenderTranslation(FVector2D::ZeroVector);
		EyeIcon->SetRenderScale(FVector2D::UnitVector);
	}
}

void UDetectionGaugeWidget::UpdateFade(float DeltaTime)
{
	if (!bIsFading)
	{
		return;
	}

	if (bIsFadingIn)
	{
		FadeProgress += DeltaTime / FadeDuration;
		if (FadeProgress >= 1.0f)
		{
			FadeProgress = 1.0f;
			bIsFading = false;
		}
	}
	else
	{
		FadeProgress -= DeltaTime / FadeDuration;
		if (FadeProgress <= 0.0f)
		{
			FadeProgress = 0.0f;
			bIsFading = false;
			bIsVisible = false;
		}
	}

	SetRenderOpacity(FadeProgress);
}

void UDetectionGaugeWidget::UpdateAutoHide(float DeltaTime)
{
	if (bAlwaysVisible || !bIsVisible || bIsFading)
	{
		return;
	}

	// 安全状態で一定時間経過したら非表示
	if (CurrentState == EDetectionState::Safe && TargetDetectionLevel < 0.01f)
	{
		AutoHideTimer += DeltaTime;
		if (AutoHideTimer >= SafeHideDelay)
		{
			HideGauge();
		}
	}
	else
	{
		AutoHideTimer = 0.0f;
	}
}

FLinearColor UDetectionGaugeWidget::GetStateColor() const
{
	switch (CurrentState)
	{
	case EDetectionState::Safe:
		return SafeColor;
	case EDetectionState::Caution:
		return CautionColor;
	case EDetectionState::Warning:
		return WarningColor;
	case EDetectionState::Danger:
		return DangerColor;
	case EDetectionState::Critical:
		return CriticalColor;
	default:
		return SafeColor;
	}
}

void UDetectionGaugeWidget::PlayStateChangeSound(EDetectionState NewState, EDetectionState OldState)
{
	// 状態が上昇した場合
	if (static_cast<int32>(NewState) > static_cast<int32>(OldState))
	{
		if (StateIncreaseSound)
		{
			UGameplayStatics::PlaySound2D(this, StateIncreaseSound);
		}
	}

	// 臨界状態に入った
	if (NewState == EDetectionState::Critical && CriticalLoopSound)
	{
		// ループ音開始（実装はオーディオコンポーネントで行う）
	}
}
