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
	// デフォルトのローディングヒント（Dawnlightテーマ）
	if (LoadingHints.Num() == 0)
	{
		LoadingHints.Add(FText::FromString(TEXT("光を避けて移動することで検知を防げます")));
		LoadingHints.Add(FText::FromString(TEXT("物陰に隠れると安全ですが、時間を消費します")));
		LoadingHints.Add(FText::FromString(TEXT("シャッター音は周囲に響きます。撮影のタイミングに注意")));
		LoadingHints.Add(FText::FromString(TEXT("監視レベルが上がるとイベントの難易度も上昇します")));
		LoadingHints.Add(FText::FromString(TEXT("夜明けまで生き延びることが最優先です")));
		LoadingHints.Add(FText::FromString(TEXT("証拠を記録することで真実に近づけます")));
		LoadingHints.Add(FText::FromString(TEXT("監視されている...常に何かがあなたを見ています")));
		LoadingHints.Add(FText::FromString(TEXT("光と影の境界線を見極めてください")));
	}
}
