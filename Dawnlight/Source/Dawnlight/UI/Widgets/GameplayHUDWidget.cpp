// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayHUDWidget.h"
#include "Dawnlight.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Subsystems/NightProgressSubsystem.h"
#include "Subsystems/SurveillanceSubsystem.h"
#include "Components/PhotographyComponent.h"
#include "Components/SurveillanceDetectorComponent.h"
#include "Kismet/GameplayStatics.h"

void UGameplayHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// デフォルト色を設定
	DetectionGaugeNormalColor = FLinearColor(0.2f, 0.6f, 0.2f, 1.0f);  // 緑
	DetectionGaugeWarningColor = FLinearColor(0.8f, 0.6f, 0.1f, 1.0f); // オレンジ
	DetectionGaugeDangerColor = FLinearColor(0.8f, 0.1f, 0.1f, 1.0f);  // 赤

	// サブシステムをキャッシュ
	CacheSubsystems();

	// 初期状態を設定
	if (DetectionWarningPanel)
	{
		DetectionWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (DawnWarningPanel)
	{
		DawnWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (PhotographingIndicator)
	{
		PhotographingIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (HiddenIndicator)
	{
		HiddenIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDWidget] 初期化完了"));
}

void UGameplayHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 夜進行サブシステムから残り時間を更新
	if (NightProgressSubsystem.IsValid() && NightProgressSubsystem->IsNightActive())
	{
		UpdateRemainingTime(NightProgressSubsystem->GetRemainingTime());
		UpdatePhaseDisplay(NightProgressSubsystem->GetCurrentPhase());

		// 夜明け警告
		if (NightProgressSubsystem->IsDawnApproaching())
		{
			ShowDawnWarning();
		}
	}

	// 検知コンポーネントから検知ゲージを更新
	if (PlayerDetectorComponent.IsValid())
	{
		UpdateDetectionGauge(PlayerDetectorComponent->GetDetectionLevel());

		// 撮影中・隠れ中インジケーター
		// TODO: GASタグから取得するか、コンポーネントから取得
	}

	// 撮影コンポーネントからカウントを更新
	if (PlayerPhotographyComponent.IsValid())
	{
		UpdatePhotoCount(
			PlayerPhotographyComponent->GetRemainingPhotos(),
			10 // MaxPhotos - TODO: コンポーネントから取得
		);

		// 撮影中インジケーター
		ShowPhotographingIndicator(PlayerPhotographyComponent->IsPhotographing());
	}
}

void UGameplayHUDWidget::CacheSubsystems()
{
	if (UWorld* World = GetWorld())
	{
		NightProgressSubsystem = World->GetSubsystem<UNightProgressSubsystem>();
		SurveillanceSubsystem = World->GetSubsystem<USurveillanceSubsystem>();
	}

	// プレイヤーのコンポーネントを取得
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			PlayerPhotographyComponent = Pawn->FindComponentByClass<UPhotographyComponent>();
			PlayerDetectorComponent = Pawn->FindComponentByClass<USurveillanceDetectorComponent>();
		}
	}
}

void UGameplayHUDWidget::UpdateDetectionGauge(float NormalizedValue)
{
	if (!DetectionGauge)
	{
		return;
	}

	// ゲージ値を設定
	DetectionGauge->SetPercent(FMath::Clamp(NormalizedValue, 0.0f, 1.0f));

	// 色を更新
	UpdateDetectionGaugeColor(NormalizedValue);

	// 閾値を超えたら警告表示
	if (NormalizedValue >= DangerThreshold)
	{
		ShowDetectionWarning();
	}
	else
	{
		HideDetectionWarning();
	}
}

void UGameplayHUDWidget::UpdateDetectionGaugeColor(float NormalizedValue)
{
	if (!DetectionGauge)
	{
		return;
	}

	FLinearColor GaugeColor;

	if (NormalizedValue >= DangerThreshold)
	{
		GaugeColor = DetectionGaugeDangerColor;
	}
	else if (NormalizedValue >= WarningThreshold)
	{
		// 警告と危険の間を補間
		float Alpha = (NormalizedValue - WarningThreshold) / (DangerThreshold - WarningThreshold);
		GaugeColor = FMath::Lerp(DetectionGaugeWarningColor, DetectionGaugeDangerColor, Alpha);
	}
	else
	{
		// 通常と警告の間を補間
		float Alpha = NormalizedValue / WarningThreshold;
		GaugeColor = FMath::Lerp(DetectionGaugeNormalColor, DetectionGaugeWarningColor, Alpha);
	}

	DetectionGauge->SetFillColorAndOpacity(GaugeColor);
}

void UGameplayHUDWidget::UpdateRemainingTime(float RemainingSeconds)
{
	if (!RemainingTimeText)
	{
		return;
	}

	RemainingTimeText->SetText(FormatTime(RemainingSeconds));
}

FText UGameplayHUDWidget::FormatTime(float Seconds) const
{
	int32 Minutes = FMath::FloorToInt(Seconds / 60.0f);
	int32 Secs = FMath::FloorToInt(FMath::Fmod(Seconds, 60.0f));

	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Secs));
}

void UGameplayHUDWidget::UpdatePhotoCount(int32 RemainingPhotos, int32 MaxPhotos)
{
	if (!PhotoCountText)
	{
		return;
	}

	FString CountString = FString::Printf(TEXT("%d / %d"), RemainingPhotos, MaxPhotos);
	PhotoCountText->SetText(FText::FromString(CountString));

	// フィルム切れ警告
	if (RemainingPhotos == 0)
	{
		ShowNoFilmWarning();
	}

	// カメラアイコンの色を変更
	if (CameraIcon)
	{
		if (RemainingPhotos == 0)
		{
			CameraIcon->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.5f)); // グレーアウト
		}
		else if (RemainingPhotos <= 3)
		{
			CameraIcon->SetColorAndOpacity(FLinearColor(0.8f, 0.6f, 0.1f, 1.0f)); // オレンジ警告
		}
		else
		{
			CameraIcon->SetColorAndOpacity(FLinearColor::White);
		}
	}
}

void UGameplayHUDWidget::UpdatePhaseDisplay(int32 PhaseIndex)
{
	if (!PhaseText)
	{
		return;
	}

	PhaseText->SetText(GetPhaseName(PhaseIndex));
}

FText UGameplayHUDWidget::GetPhaseName(int32 PhaseIndex) const
{
	switch (PhaseIndex)
	{
	case 0:
		return FText::FromString(TEXT("導入"));
	case 1:
		return FText::FromString(TEXT("緩和"));
	case 2:
		return FText::FromString(TEXT("クライマックス"));
	default:
		return FText::FromString(TEXT("---"));
	}
}

void UGameplayHUDWidget::ShowDetectionWarning()
{
	if (DetectionWarningPanel && DetectionWarningPanel->GetVisibility() != ESlateVisibility::Visible)
	{
		DetectionWarningPanel->SetVisibility(ESlateVisibility::Visible);
		PlayAttentionPulse(DetectionWarningPanel, true);
	}
}

void UGameplayHUDWidget::HideDetectionWarning()
{
	if (DetectionWarningPanel && DetectionWarningPanel->GetVisibility() == ESlateVisibility::Visible)
	{
		StopWidgetAnimation(DetectionWarningPanel);
		DetectionWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGameplayHUDWidget::ShowDawnWarning()
{
	if (DawnWarningPanel)
	{
		DawnWarningPanel->SetVisibility(ESlateVisibility::Visible);
		PlayAttentionPulse(DawnWarningPanel, true);

		UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDWidget] 夜明け警告を表示"));
	}
}

void UGameplayHUDWidget::ShowNoFilmWarning()
{
	// フィルム切れの一時的な警告表示
	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDWidget] フィルム切れ警告"));

	// TODO: 一時的なポップアップ表示
}

void UGameplayHUDWidget::ShowPhotographingIndicator(bool bShow)
{
	if (PhotographingIndicator)
	{
		if (bShow)
		{
			PhotographingIndicator->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			PhotographingIndicator->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UGameplayHUDWidget::ShowHiddenIndicator(bool bShow)
{
	if (HiddenIndicator)
	{
		if (bShow)
		{
			HiddenIndicator->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			HiddenIndicator->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
