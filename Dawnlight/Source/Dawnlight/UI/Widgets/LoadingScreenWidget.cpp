// Copyright Epic Games, Inc. All Rights Reserved.

#include "LoadingScreenWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

ULoadingScreenWidget::ULoadingScreenWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentProgress(0.0f)
	, DisplayProgress(0.0f)
	, CurrentHintIndex(0)
	, HintTimer(0.0f)
	, bIsFading(false)
	, bIsFadingIn(true)
	, FadeProgress(0.0f)
{
}

void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// デフォルトヒントを設定
	SetupDefaultHints();

	// 初期ヒントを表示
	if (LoadingHints.Num() > 0)
	{
		ShowRandomHint();
	}

	// 初期状態を設定
	CurrentProgress = 0.0f;
	DisplayProgress = 0.0f;
	HintTimer = 0.0f;
}

void ULoadingScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// プログレスバーのスムージング
	UpdateProgressBar(InDeltaTime);

	// ローディングアイコンの回転
	UpdateLoadingIcon(InDeltaTime);

	// ヒントの自動切り替え
	UpdateHintTimer(InDeltaTime);

	// フェードアニメーション
	UpdateFade(InDeltaTime);
}

void ULoadingScreenWidget::SetProgress(float Progress)
{
	CurrentProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
}

void ULoadingScreenWidget::SetLoadingText(const FText& Text)
{
	if (LoadingText)
	{
		LoadingText->SetText(Text);
	}
}

void ULoadingScreenWidget::SetHintText(const FText& Text)
{
	if (HintText)
	{
		HintText->SetText(Text);
	}
}

void ULoadingScreenWidget::ShowWithFadeIn()
{
	SetVisibility(ESlateVisibility::Visible);
	bIsFading = true;
	bIsFadingIn = true;
	FadeProgress = 0.0f;
	SetRenderOpacity(0.0f);
}

void ULoadingScreenWidget::HideWithFadeOut()
{
	bIsFading = true;
	bIsFadingIn = false;
	FadeProgress = 0.0f;
}

void ULoadingScreenWidget::ShowImmediately()
{
	SetVisibility(ESlateVisibility::Visible);
	SetRenderOpacity(1.0f);
	bIsFading = false;
}

void ULoadingScreenWidget::HideImmediately()
{
	SetVisibility(ESlateVisibility::Collapsed);
	SetRenderOpacity(0.0f);
	bIsFading = false;
}

void ULoadingScreenWidget::ShowRandomHint()
{
	if (LoadingHints.Num() == 0)
	{
		return;
	}

	CurrentHintIndex = FMath::RandRange(0, LoadingHints.Num() - 1);
	SetHintText(LoadingHints[CurrentHintIndex]);
	HintTimer = 0.0f;
}

void ULoadingScreenWidget::ShowNextHint()
{
	if (LoadingHints.Num() == 0)
	{
		return;
	}

	CurrentHintIndex = (CurrentHintIndex + 1) % LoadingHints.Num();
	SetHintText(LoadingHints[CurrentHintIndex]);
	HintTimer = 0.0f;
}

void ULoadingScreenWidget::UpdateProgressBar(float DeltaTime)
{
	if (!ProgressBar)
	{
		return;
	}

	// スムージングを適用
	DisplayProgress = FMath::FInterpTo(DisplayProgress, CurrentProgress, DeltaTime, ProgressSmoothSpeed);

	// プログレスバーを更新
	ProgressBar->SetPercent(DisplayProgress);
}

void ULoadingScreenWidget::UpdateLoadingIcon(float DeltaTime)
{
	if (!LoadingIcon)
	{
		return;
	}

	// 回転角度を計算
	const float CurrentAngle = LoadingIcon->GetRenderTransformAngle();
	const float NewAngle = CurrentAngle + (IconRotationSpeed * DeltaTime);
	LoadingIcon->SetRenderTransformAngle(FMath::Fmod(NewAngle, 360.0f));
}

void ULoadingScreenWidget::UpdateHintTimer(float DeltaTime)
{
	if (LoadingHints.Num() <= 1)
	{
		return;
	}

	HintTimer += DeltaTime;
	if (HintTimer >= HintChangeInterval)
	{
		ShowNextHint();
	}
}

void ULoadingScreenWidget::UpdateFade(float DeltaTime)
{
	if (!bIsFading)
	{
		return;
	}

	FadeProgress += DeltaTime / FadeDuration;

	if (FadeProgress >= 1.0f)
	{
		// フェード完了
		FadeProgress = 1.0f;
		bIsFading = false;

		if (bIsFadingIn)
		{
			SetRenderOpacity(1.0f);
		}
		else
		{
			SetRenderOpacity(0.0f);
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		// フェード中
		const float Alpha = bIsFadingIn ? FadeProgress : (1.0f - FadeProgress);
		SetRenderOpacity(Alpha);
	}
}

void ULoadingScreenWidget::SetupDefaultHints()
{
	// デフォルトのローディングヒント（Soul Reaperテーマ）
	if (LoadingHints.Num() == 0)
	{
		LoadingHints.Add(FText::FromString(TEXT("夜の間に動物を狩り、魂を収集せよ")));
		LoadingHints.Add(FText::FromString(TEXT("収集した魂は夜明けに強力なバフに変換される")));
		LoadingHints.Add(FText::FromString(TEXT("リーパーゲージが満タンになったらSpaceでリーパーモード発動")));
		LoadingHints.Add(FText::FromString(TEXT("リーパーモード中はダメージが2倍になる")));
		LoadingHints.Add(FText::FromString(TEXT("Tigerの魂は攻撃力、Horseの魂は移動速度をアップ")));
		LoadingHints.Add(FText::FromString(TEXT("Dawnフェーズでは敵のウェーブを全滅させろ")));
		LoadingHints.Add(FText::FromString(TEXT("多くの魂を集めるほど、夜明けの戦いが有利になる")));
		LoadingHints.Add(FText::FromString(TEXT("夜に魂を集め、夜明けに力を解き放て")));
	}
}
