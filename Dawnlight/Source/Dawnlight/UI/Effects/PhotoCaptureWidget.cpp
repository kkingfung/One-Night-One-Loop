// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhotoCaptureWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

UPhotoCaptureWidget::UPhotoCaptureWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsViewfinderActive(false)
	, bIsFocused(false)
	, bShowDanger(false)
	, FlashTimer(0.0f)
	, ViewfinderFadeTimer(0.0f)
	, ShutterAnimTimer(0.0f)
	, ResultTextTimer(0.0f)
	, PulseTimer(0.0f)
{
}

void UPhotoCaptureWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 初期状態は非表示
	if (FlashOverlay)
	{
		FlashOverlay->SetColorAndOpacity(FLinearColor::Transparent);
		FlashOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ViewfinderFrame)
	{
		ViewfinderFrame->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (FocusIndicator)
	{
		FocusIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Crosshair)
	{
		Crosshair->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (DangerIndicator)
	{
		DangerIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ResultText)
	{
		ResultText->SetVisibility(ESlateVisibility::Collapsed);
	}

	// コーナーフレームを非表示
	if (CornerTopLeft) CornerTopLeft->SetVisibility(ESlateVisibility::Collapsed);
	if (CornerTopRight) CornerTopRight->SetVisibility(ESlateVisibility::Collapsed);
	if (CornerBottomLeft) CornerBottomLeft->SetVisibility(ESlateVisibility::Collapsed);
	if (CornerBottomRight) CornerBottomRight->SetVisibility(ESlateVisibility::Collapsed);
}

void UPhotoCaptureWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// パルスタイマー更新
	PulseTimer += InDeltaTime;

	// 各エフェクトを更新
	UpdateFlash(InDeltaTime);
	UpdateViewfinder(InDeltaTime);
	UpdateFocusIndicator(InDeltaTime);
	UpdateDangerIndicator(InDeltaTime);
	UpdateResultText(InDeltaTime);
	UpdateShutterAnimation(InDeltaTime);
}

void UPhotoCaptureWidget::ShowViewfinder()
{
	if (bIsViewfinderActive)
	{
		return;
	}

	bIsViewfinderActive = true;
	ViewfinderFadeTimer = ViewfinderFadeInDuration;

	// ファインダー要素を表示開始
	if (ViewfinderFrame)
	{
		ViewfinderFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
		FLinearColor FrameColor = ViewfinderColor;
		FrameColor.A = 0.0f;
		ViewfinderFrame->SetColorAndOpacity(FrameColor);
	}

	if (Crosshair)
	{
		Crosshair->SetVisibility(ESlateVisibility::HitTestInvisible);
		FLinearColor CrosshairColor = ViewfinderColor;
		CrosshairColor.A = 0.0f;
		Crosshair->SetColorAndOpacity(CrosshairColor);
	}

	if (FocusIndicator)
	{
		FocusIndicator->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	// コーナーフレーム表示
	if (CornerTopLeft) CornerTopLeft->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (CornerTopRight) CornerTopRight->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (CornerBottomLeft) CornerBottomLeft->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (CornerBottomRight) CornerBottomRight->SetVisibility(ESlateVisibility::HitTestInvisible);

	// フォーカス音を再生
	if (FocusSound)
	{
		UGameplayStatics::PlaySound2D(this, FocusSound);
	}
}

void UPhotoCaptureWidget::HideViewfinder()
{
	if (!bIsViewfinderActive)
	{
		return;
	}

	bIsViewfinderActive = false;
	bIsFocused = false;
	bShowDanger = false;

	// 全要素を非表示
	if (ViewfinderFrame)
	{
		ViewfinderFrame->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Crosshair)
	{
		Crosshair->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (FocusIndicator)
	{
		FocusIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (DangerIndicator)
	{
		DangerIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}

	// コーナーフレーム非表示
	if (CornerTopLeft) CornerTopLeft->SetVisibility(ESlateVisibility::Collapsed);
	if (CornerTopRight) CornerTopRight->SetVisibility(ESlateVisibility::Collapsed);
	if (CornerBottomLeft) CornerBottomLeft->SetVisibility(ESlateVisibility::Collapsed);
	if (CornerBottomRight) CornerBottomRight->SetVisibility(ESlateVisibility::Collapsed);
}

void UPhotoCaptureWidget::TakePhoto()
{
	// フラッシュを開始
	FlashTimer = FlashDuration;

	if (FlashOverlay)
	{
		FlashOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		FlashOverlay->SetColorAndOpacity(FlashColor);
	}

	// シャッターアニメーション開始
	ShutterAnimTimer = ShutterAnimDuration;

	// シャッター音を再生
	if (ShutterSound)
	{
		UGameplayStatics::PlaySound2D(this, ShutterSound);
	}

	// デリゲート発火
	OnPhotoTaken.Broadcast();
}

void UPhotoCaptureWidget::ShowPhotoSuccess()
{
	if (ResultText)
	{
		ResultText->SetVisibility(ESlateVisibility::HitTestInvisible);
		ResultText->SetText(FText::FromString(TEXT("CAPTURED")));
		ResultText->SetColorAndOpacity(SuccessTextColor);
	}

	ResultTextTimer = ResultTextDuration;

	// 成功音を再生
	if (SuccessSound)
	{
		UGameplayStatics::PlaySound2D(this, SuccessSound);
	}

	// クールダウン完了を通知
	OnPhotoCooldownComplete.Broadcast(true);
}

void UPhotoCaptureWidget::ShowPhotoFailed(const FText& Reason)
{
	if (ResultText)
	{
		ResultText->SetVisibility(ESlateVisibility::HitTestInvisible);
		ResultText->SetText(Reason);
		ResultText->SetColorAndOpacity(FailedTextColor);
	}

	ResultTextTimer = ResultTextDuration;

	// 失敗音を再生
	if (FailedSound)
	{
		UGameplayStatics::PlaySound2D(this, FailedSound);
	}

	// クールダウン完了を通知（失敗）
	OnPhotoCooldownComplete.Broadcast(false);
}

void UPhotoCaptureWidget::SetFocusState(bool bNewIsFocused)
{
	if (bIsFocused == bNewIsFocused)
	{
		return;
	}

	bIsFocused = bNewIsFocused;

	// フォーカス音を再生
	if (bIsFocused && FocusSound)
	{
		UGameplayStatics::PlaySound2D(this, FocusSound, 0.5f);
	}
}

void UPhotoCaptureWidget::SetDangerIndicator(bool bShow)
{
	bShowDanger = bShow;

	if (DangerIndicator)
	{
		DangerIndicator->SetVisibility(bShow && bIsViewfinderActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UPhotoCaptureWidget::UpdateFlash(float DeltaTime)
{
	if (FlashTimer <= 0.0f)
	{
		return;
	}

	FlashTimer -= DeltaTime;

	if (FlashOverlay)
	{
		// フラッシュの減衰
		const float Alpha = FMath::Clamp(FlashTimer / FlashDuration, 0.0f, 1.0f);
		FLinearColor CurrentFlashColor = FlashColor;
		CurrentFlashColor.A *= Alpha;
		FlashOverlay->SetColorAndOpacity(CurrentFlashColor);

		if (FlashTimer <= 0.0f)
		{
			FlashOverlay->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UPhotoCaptureWidget::UpdateViewfinder(float DeltaTime)
{
	if (!bIsViewfinderActive)
	{
		return;
	}

	// フェードイン処理
	if (ViewfinderFadeTimer > 0.0f)
	{
		ViewfinderFadeTimer -= DeltaTime;
		const float FadeProgress = 1.0f - FMath::Clamp(ViewfinderFadeTimer / ViewfinderFadeInDuration, 0.0f, 1.0f);

		// コーナーのアニメーション
		AnimateCorners(FadeProgress);

		// ファインダーフレームのフェードイン
		if (ViewfinderFrame)
		{
			FLinearColor FrameColor = ViewfinderColor;
			FrameColor.A *= FadeProgress;
			ViewfinderFrame->SetColorAndOpacity(FrameColor);
		}

		if (Crosshair)
		{
			FLinearColor CrosshairColor = ViewfinderColor;
			CrosshairColor.A *= FadeProgress;
			Crosshair->SetColorAndOpacity(CrosshairColor);
		}
	}
}

void UPhotoCaptureWidget::UpdateFocusIndicator(float DeltaTime)
{
	if (!bIsViewfinderActive || !FocusIndicator)
	{
		return;
	}

	// フォーカス状態に応じた色
	const FLinearColor TargetColor = bIsFocused ? FocusedColor : UnfocusedColor;

	// パルスエフェクト
	const float Pulse = FMath::Sin(PulseTimer * FocusPulseSpeed) * 0.2f + 0.8f;
	FLinearColor PulsedColor = TargetColor;
	PulsedColor.A *= Pulse;

	FocusIndicator->SetColorAndOpacity(PulsedColor);
}

void UPhotoCaptureWidget::UpdateDangerIndicator(float DeltaTime)
{
	if (!bShowDanger || !DangerIndicator)
	{
		return;
	}

	// 危険インジケーターの点滅
	const float Pulse = FMath::Abs(FMath::Sin(PulseTimer * DangerPulseSpeed));
	FLinearColor PulsedColor = DangerColor;
	PulsedColor.A *= Pulse;

	DangerIndicator->SetColorAndOpacity(PulsedColor);
}

void UPhotoCaptureWidget::UpdateResultText(float DeltaTime)
{
	if (ResultTextTimer <= 0.0f)
	{
		return;
	}

	ResultTextTimer -= DeltaTime;

	if (ResultText)
	{
		// フェードアウト（最後の0.5秒）
		if (ResultTextTimer < 0.5f)
		{
			const float Alpha = ResultTextTimer / 0.5f;
			FLinearColor TextColor = ResultText->GetColorAndOpacity().GetSpecifiedColor();
			TextColor.A = Alpha;
			ResultText->SetColorAndOpacity(TextColor);
		}

		if (ResultTextTimer <= 0.0f)
		{
			ResultText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UPhotoCaptureWidget::UpdateShutterAnimation(float DeltaTime)
{
	if (ShutterAnimTimer <= 0.0f)
	{
		return;
	}

	ShutterAnimTimer -= DeltaTime;

	// シャッターアニメーション（コーナーが一瞬縮む）
	const float Progress = 1.0f - FMath::Clamp(ShutterAnimTimer / ShutterAnimDuration, 0.0f, 1.0f);

	// シャッター効果：コーナーが内側に動いて戻る
	const float ShutterCurve = FMath::Sin(Progress * PI);
	AnimateCorners(1.0f - ShutterCurve * 0.2f);
}

void UPhotoCaptureWidget::AnimateCorners(float Alpha)
{
	// コーナーの透明度とスケールをアニメーション
	// Alpha: 0.0 = 閉じた状態、1.0 = 開いた状態

	const float CornerAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	FLinearColor CornerColor = ViewfinderColor;
	CornerColor.A = CornerAlpha;

	// 各コーナーに色を適用
	if (CornerTopLeft)
	{
		CornerTopLeft->SetColorAndOpacity(CornerColor);
	}

	if (CornerTopRight)
	{
		CornerTopRight->SetColorAndOpacity(CornerColor);
	}

	if (CornerBottomLeft)
	{
		CornerBottomLeft->SetColorAndOpacity(CornerColor);
	}

	if (CornerBottomRight)
	{
		CornerBottomRight->SetColorAndOpacity(CornerColor);
	}
}
