// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameResultWidget.h"
#include "Dawnlight.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"

void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// デフォルト値を設定
	VictoryColor = FLinearColor(1.0f, 0.84f, 0.0f, 1.0f);  // 金色
	DefeatColor = FLinearColor(0.8f, 0.2f, 0.2f, 1.0f);    // 赤色

	VictoryTitleText = FText::FromString(TEXT("VICTORY"));
	VictorySubtitleText = FText::FromString(TEXT("You survived the dawn!"));
	DefeatTitleText = FText::FromString(TEXT("DEFEAT"));
	DefeatSubtitleText = FText::FromString(TEXT("The night has claimed you..."));

	// ボタンイベントをバインド
	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameResultWidget::OnRestartButtonClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UGameResultWidget::OnMainMenuButtonClicked);
	}

	// 初期状態は非表示
	SetVisibility(ESlateVisibility::Collapsed);
}

void UGameResultWidget::ShowResult(EGameResult Result, int32 TotalSouls, int32 WavesCleared, int32 TotalWaves)
{
	CurrentResult = Result;

	// UIを設定
	SetupResultUI(Result);
	SetupStats(TotalSouls, WavesCleared, TotalWaves);

	// 表示
	Show();

	UE_LOG(LogDawnlight, Log, TEXT("[GameResultWidget] 結果画面を表示: %s（魂: %d, Wave: %d/%d）"),
		Result == EGameResult::Victory ? TEXT("勝利") : TEXT("敗北"),
		TotalSouls, WavesCleared, TotalWaves);
}

void UGameResultWidget::SetupResultUI(EGameResult Result)
{
	if (!ResultTitleText)
	{
		return;
	}

	if (Result == EGameResult::Victory)
	{
		ResultTitleText->SetText(VictoryTitleText);
		ResultTitleText->SetColorAndOpacity(FSlateColor(VictoryColor));

		if (ResultSubtitleText)
		{
			ResultSubtitleText->SetText(VictorySubtitleText);
		}
	}
	else if (Result == EGameResult::Defeat)
	{
		ResultTitleText->SetText(DefeatTitleText);
		ResultTitleText->SetColorAndOpacity(FSlateColor(DefeatColor));

		if (ResultSubtitleText)
		{
			ResultSubtitleText->SetText(DefeatSubtitleText);
		}
	}
}

void UGameResultWidget::SetupStats(int32 TotalSouls, int32 WavesCleared, int32 TotalWaves)
{
	// 魂数を設定
	if (SoulCountText)
	{
		SoulCountText->SetText(FText::FromString(
			FString::Printf(TEXT("Souls Collected: %d"), TotalSouls)));
	}

	// Wave進捗を設定
	if (WaveProgressText)
	{
		WaveProgressText->SetText(FText::FromString(
			FString::Printf(TEXT("Waves Cleared: %d / %d"), WavesCleared, TotalWaves)));
	}
}

void UGameResultWidget::OnRestartButtonClicked()
{
	UE_LOG(LogDawnlight, Log, TEXT("[GameResultWidget] リスタートボタンがクリックされました"));

	// デリゲートを発火
	OnRestartRequested.Broadcast();
}

void UGameResultWidget::OnMainMenuButtonClicked()
{
	UE_LOG(LogDawnlight, Log, TEXT("[GameResultWidget] メインメニューボタンがクリックされました"));

	// デリゲートを発火
	OnMainMenuRequested.Broadcast();
}
