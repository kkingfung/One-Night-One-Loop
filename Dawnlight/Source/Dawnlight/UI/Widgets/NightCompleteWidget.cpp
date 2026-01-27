// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "NightCompleteWidget.h"
#include "Dawnlight.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UNightCompleteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentNightNumber = 1;

	// 初期テキストを設定
	UpdateNightText();

	// 証拠サマリーを非表示（デフォルト）
	if (PhotoCountText)
	{
		PhotoCountText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (EvidenceValueText)
	{
		EvidenceValueText->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[NightCompleteWidget] 初期化完了"));
}

void UNightCompleteWidget::ShowWithNightNumber(int32 NightNumber)
{
	CurrentNightNumber = NightNumber;
	UpdateNightText();

	// アニメーション付きで表示
	Show();

	UE_LOG(LogDawnlight, Log, TEXT("[NightCompleteWidget] Night %d Complete を表示"), NightNumber);
}

void UNightCompleteWidget::SetEvidenceSummary(int32 PhotosTaken, float TotalEvidenceValue)
{
	if (PhotoCountText)
	{
		FString PhotoText = FString::Printf(TEXT("Photos: %d"), PhotosTaken);
		PhotoCountText->SetText(FText::FromString(PhotoText));
		PhotoCountText->SetVisibility(ESlateVisibility::Visible);
	}

	if (EvidenceValueText)
	{
		FString EvidenceText = FString::Printf(TEXT("Evidence: %.0f"), TotalEvidenceValue);
		EvidenceValueText->SetText(FText::FromString(EvidenceText));
		EvidenceValueText->SetVisibility(ESlateVisibility::Visible);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[NightCompleteWidget] 証拠サマリー: 撮影 %d枚, 価値 %.0f"), PhotosTaken, TotalEvidenceValue);
}

void UNightCompleteWidget::UpdateNightText()
{
	if (NightNumberText)
	{
		FString NightText = FString::Printf(TEXT("NIGHT %d"), CurrentNightNumber);
		NightNumberText->SetText(FText::FromString(NightText));
	}

	if (CompleteText)
	{
		CompleteText->SetText(FText::FromString(TEXT("COMPLETE")));
	}
}
