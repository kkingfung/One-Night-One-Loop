// Copyright Epic Games, Inc. All Rights Reserved.

#include "NightProgressWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

UNightProgressWidget::UNightProgressWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentProgress(0.0f)
	, DisplayProgress(0.0f)
	, CurrentPhase(ENightPhase::Introduction)
	, CurrentBarColor(IntroductionColor)
	, TargetBarColor(IntroductionColor)
	, GlowTimer(0.0f)
	, EventPulseTimer(0.0f)
	, PhaseTransitionTimer(0.0f)
	, bShowingWarning(false)
	, WarningTimer(0.0f)
{
}

void UNightProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 初期状態設定
	CurrentBarColor = IntroductionColor;
	TargetBarColor = IntroductionColor;

	// プログレスバー初期化
	if (ProgressBar)
	{
		ProgressBar->SetPercent(0.0f);
		ProgressBar->SetFillColorAndOpacity(CurrentBarColor);
	}

	// グロー初期化
	if (ProgressBarGlow)
	{
		ProgressBarGlow->SetColorAndOpacity(FLinearColor::Transparent);
	}

	// 警告初期化
	if (WarningOverlay)
	{
		WarningOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 太陽アイコン初期化（最初は非表示）
	if (SunIcon)
	{
		SunIcon->SetColorAndOpacity(FLinearColor::Transparent);
	}

	// フェーズテキスト初期化
	if (PhaseText)
	{
		PhaseText->SetText(GetPhaseName(CurrentPhase));
	}
}

void UNightProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// タイマー更新
	GlowTimer += InDeltaTime;

	// 各要素を更新
	UpdateProgressBar(InDeltaTime);
	UpdateColors(InDeltaTime);
	UpdateGlow(InDeltaTime);
	UpdateMoonIcon();
	UpdateEventPulse(InDeltaTime);
	UpdatePhaseTransition(InDeltaTime);
	UpdateWarning(InDeltaTime);
}

void UNightProgressWidget::SetNightProgress(float Progress)
{
	CurrentProgress = FMath::Clamp(Progress, 0.0f, 1.0f);

	// 色の目標値を更新
	TargetBarColor = CalculateColorForProgress(CurrentProgress);

	// 夜明け間近の警告チェック
	if (CurrentProgress >= DawnWarningThreshold && !bShowingWarning)
	{
		ShowDawnWarning();
	}
}

void UNightProgressWidget::SetCurrentPhase(ENightPhase Phase)
{
	if (CurrentPhase != Phase)
	{
		CurrentPhase = Phase;

		// フェーズテキスト更新
		if (PhaseText)
		{
			PhaseText->SetText(GetPhaseName(Phase));
		}

		// フェーズ移行演出
		TriggerPhaseTransition();
	}
}

void UNightProgressWidget::SetTimeText(const FText& Text)
{
	if (TimeText)
	{
		TimeText->SetText(Text);
	}
}

void UNightProgressWidget::ShowDawnWarning()
{
	bShowingWarning = true;
	WarningTimer = 0.0f;

	if (WarningOverlay)
	{
		WarningOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UNightProgressWidget::TriggerEventPulse()
{
	EventPulseTimer = EventPulseDuration;
}

void UNightProgressWidget::TriggerPhaseTransition()
{
	PhaseTransitionTimer = PhaseTransitionDuration;
}

void UNightProgressWidget::UpdateProgressBar(float DeltaTime)
{
	// スムージング
	DisplayProgress = FMath::FInterpTo(DisplayProgress, CurrentProgress, DeltaTime, ProgressSmoothSpeed);

	// プログレスバー更新
	if (ProgressBar)
	{
		ProgressBar->SetPercent(DisplayProgress);
		ProgressBar->SetFillColorAndOpacity(CurrentBarColor);
	}
}

void UNightProgressWidget::UpdateColors(float DeltaTime)
{
	// 色のスムージング
	CurrentBarColor.R = FMath::FInterpTo(CurrentBarColor.R, TargetBarColor.R, DeltaTime, ProgressSmoothSpeed);
	CurrentBarColor.G = FMath::FInterpTo(CurrentBarColor.G, TargetBarColor.G, DeltaTime, ProgressSmoothSpeed);
	CurrentBarColor.B = FMath::FInterpTo(CurrentBarColor.B, TargetBarColor.B, DeltaTime, ProgressSmoothSpeed);
	CurrentBarColor.A = FMath::FInterpTo(CurrentBarColor.A, TargetBarColor.A, DeltaTime, ProgressSmoothSpeed);

	// バー背景にも色を適用（暗めに）
	if (ProgressBarBackground)
	{
		FLinearColor BgColor = CurrentBarColor;
		BgColor.R *= 0.2f;
		BgColor.G *= 0.2f;
		BgColor.B *= 0.2f;
		BgColor.A = 0.8f;
		ProgressBarBackground->SetColorAndOpacity(BgColor);
	}
}

void UNightProgressWidget::UpdateGlow(float DeltaTime)
{
	if (!ProgressBarGlow)
	{
		return;
	}

	// 基本グローパルス
	float GlowIntensity = (FMath::Sin(GlowTimer * GlowPulseSpeed * PI * 2.0f) * 0.5f + 0.5f) * 0.3f;

	// イベントパルス中は強調
	if (EventPulseTimer > 0.0f)
	{
		GlowIntensity += 0.5f;
	}

	// フェーズ移行中は強調
	if (PhaseTransitionTimer > 0.0f)
	{
		const float TransitionIntensity = PhaseTransitionTimer / PhaseTransitionDuration;
		GlowIntensity += TransitionIntensity * 0.4f;
	}

	// 夜明け間近は常に光る
	if (CurrentProgress >= DawnWarningThreshold)
	{
		GlowIntensity = FMath::Max(GlowIntensity, 0.5f);
	}

	// グロー色を設定
	FLinearColor CurrentGlowColor = GlowColor;

	// イベントパルス中は赤みを追加
	if (EventPulseTimer > 0.0f)
	{
		CurrentGlowColor = FLinearColor::LerpUsingHSV(GlowColor, EventPulseColor, EventPulseTimer / EventPulseDuration);
	}

	// 夜明け間近はオレンジに
	if (CurrentProgress >= DawnWarningThreshold)
	{
		const float DawnProgress = (CurrentProgress - DawnWarningThreshold) / (1.0f - DawnWarningThreshold);
		CurrentGlowColor = FLinearColor::LerpUsingHSV(GlowColor, DawnColor, DawnProgress);
	}

	CurrentGlowColor.A = GlowIntensity;
	ProgressBarGlow->SetColorAndOpacity(CurrentGlowColor);
}

void UNightProgressWidget::UpdateMoonIcon()
{
	if (!MoonIcon)
	{
		return;
	}

	// 月のアルファ（夜明けに近づくと薄くなる）
	const float MoonAlpha = 1.0f - FMath::Clamp((CurrentProgress - 0.8f) / 0.2f, 0.0f, 1.0f);

	FLinearColor MoonColor = FLinearColor::White;
	MoonColor.A = MoonAlpha;
	MoonIcon->SetColorAndOpacity(MoonColor);

	// 太陽アイコン（夜明けに近づくと表示）
	if (SunIcon)
	{
		const float SunAlpha = FMath::Clamp((CurrentProgress - 0.85f) / 0.15f, 0.0f, 1.0f);
		FLinearColor SunColor = DawnColor;
		SunColor.A = SunAlpha;
		SunIcon->SetColorAndOpacity(SunColor);
	}
}

void UNightProgressWidget::UpdateEventPulse(float DeltaTime)
{
	if (EventPulseTimer <= 0.0f)
	{
		return;
	}

	EventPulseTimer -= DeltaTime;
}

void UNightProgressWidget::UpdatePhaseTransition(float DeltaTime)
{
	if (PhaseTransitionTimer <= 0.0f)
	{
		return;
	}

	PhaseTransitionTimer -= DeltaTime;

	// フェーズテキストのパルス効果
	if (PhaseText)
	{
		const float Pulse = FMath::Sin(PhaseTransitionTimer * 10.0f) * 0.3f + 0.7f;
		FLinearColor TextColor = FLinearColor::White;
		TextColor.A = Pulse;
		PhaseText->SetColorAndOpacity(TextColor);
	}

	// フェーズマーカーの点灯
	const float MarkerIntensity = PhaseTransitionTimer / PhaseTransitionDuration;

	if (PhaseMarker1 && CurrentPhase >= ENightPhase::Relaxation)
	{
		FLinearColor MarkerColor = RelaxationColor;
		MarkerColor.A = 0.5f + MarkerIntensity * 0.5f;
		PhaseMarker1->SetColorAndOpacity(MarkerColor);
	}

	if (PhaseMarker2 && CurrentPhase >= ENightPhase::Climax)
	{
		FLinearColor MarkerColor = ClimaxColor;
		MarkerColor.A = 0.5f + MarkerIntensity * 0.5f;
		PhaseMarker2->SetColorAndOpacity(MarkerColor);
	}
}

void UNightProgressWidget::UpdateWarning(float DeltaTime)
{
	if (!bShowingWarning || !WarningOverlay)
	{
		return;
	}

	WarningTimer += DeltaTime;

	// 警告のパルス
	const float Pulse = (FMath::Sin(WarningTimer * WarningPulseSpeed * PI * 2.0f) * 0.5f + 0.5f);
	FLinearColor CurrentWarningColor = WarningColor;
	CurrentWarningColor.A = Pulse * 0.4f;

	WarningOverlay->SetColorAndOpacity(CurrentWarningColor);
}

FLinearColor UNightProgressWidget::CalculateColorForProgress(float Progress) const
{
	// 進行度に基づいて色を補間
	if (Progress < Phase1Boundary)
	{
		// 導入フェーズ
		return IntroductionColor;
	}
	else if (Progress < Phase2Boundary)
	{
		// 導入→緩和の補間
		const float LocalProgress = (Progress - Phase1Boundary) / (Phase2Boundary - Phase1Boundary);
		return FLinearColor::LerpUsingHSV(IntroductionColor, RelaxationColor, LocalProgress);
	}
	else if (Progress < DawnWarningThreshold)
	{
		// 緩和→締めの補間
		const float LocalProgress = (Progress - Phase2Boundary) / (DawnWarningThreshold - Phase2Boundary);
		return FLinearColor::LerpUsingHSV(RelaxationColor, ClimaxColor, LocalProgress);
	}
	else
	{
		// 締め→夜明けの補間
		const float LocalProgress = (Progress - DawnWarningThreshold) / (1.0f - DawnWarningThreshold);
		return FLinearColor::LerpUsingHSV(ClimaxColor, DawnColor, LocalProgress);
	}
}

FText UNightProgressWidget::GetPhaseName(ENightPhase Phase) const
{
	switch (Phase)
	{
	case ENightPhase::Introduction:
		return IntroductionPhaseName;
	case ENightPhase::Relaxation:
		return RelaxationPhaseName;
	case ENightPhase::Climax:
		return ClimaxPhaseName;
	default:
		return FText::GetEmpty();
	}
}
