// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "ConfirmationDialogWidget.h"
#include "WidgetUtilities.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Dawnlight.h"

UConfirmationDialogWidget::UConfirmationDialogWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UConfirmationDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// BindWidgetで見つからなかった場合、手動で検索
	if (!TitleText)
	{
		TitleText = FindWidgetByBaseName<UTextBlock>(this, TEXT("TitleText"));
	}
	if (!MessageText)
	{
		MessageText = FindWidgetByBaseName<UTextBlock>(this, TEXT("MessageText"));
	}
	if (!ConfirmButton)
	{
		ConfirmButton = FindWidgetByBaseName<UButton>(this, TEXT("ConfirmButton"));
	}
	if (!ConfirmButtonText)
	{
		ConfirmButtonText = FindWidgetByBaseName<UTextBlock>(this, TEXT("ConfirmButtonText"));
	}
	if (!CancelButton)
	{
		CancelButton = FindWidgetByBaseName<UButton>(this, TEXT("CancelButton"));
	}
	if (!CancelButtonText)
	{
		CancelButtonText = FindWidgetByBaseName<UTextBlock>(this, TEXT("CancelButtonText"));
	}
	if (!BackgroundOverlay)
	{
		BackgroundOverlay = FindWidgetByBaseName<UImage>(this, TEXT("BackgroundOverlay"));
	}

	BindButtonEvents();

	// 初期状態は非表示
	SetVisibility(ESlateVisibility::Collapsed);

	UE_LOG(LogDawnlight, Log, TEXT("[ConfirmationDialogWidget] 初期化完了"));
}

void UConfirmationDialogWidget::NativeDestruct()
{
	UnbindButtonEvents();

	Super::NativeDestruct();
}

FReply UConfirmationDialogWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// Escキーでキャンセル
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnCancelClicked();
		return FReply::Handled();
	}

	// Enterキーで確認
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		OnConfirmClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UConfirmationDialogWidget::ShowDialog(
	const FText& Title,
	const FText& Message,
	EConfirmationDialogType DialogType)
{
	// タイトル設定
	if (TitleText)
	{
		TitleText->SetText(Title);
	}

	// メッセージ設定
	if (MessageText)
	{
		MessageText->SetText(Message);
	}

	// ボタンテキストをタイプに応じて設定
	SetButtonTextForType(DialogType);

	// 表示
	Show();

	UE_LOG(LogDawnlight, Log, TEXT("[ConfirmationDialogWidget] ダイアログ表示: %s"), *Title.ToString());
}

void UConfirmationDialogWidget::ShowDialogCustom(
	const FText& Title,
	const FText& Message,
	const FText& ConfirmText,
	const FText& CancelText,
	bool bShowCancelButton)
{
	// タイトル設定
	if (TitleText)
	{
		TitleText->SetText(Title);
	}

	// メッセージ設定
	if (MessageText)
	{
		MessageText->SetText(Message);
	}

	// ボタンテキスト設定
	if (ConfirmButtonText)
	{
		ConfirmButtonText->SetText(ConfirmText);
	}

	if (CancelButtonText)
	{
		CancelButtonText->SetText(CancelText);
	}

	// キャンセルボタンの表示/非表示
	if (CancelButton)
	{
		CancelButton->SetVisibility(bShowCancelButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// 表示
	Show();

	UE_LOG(LogDawnlight, Log, TEXT("[ConfirmationDialogWidget] カスタムダイアログ表示: %s"), *Title.ToString());
}

void UConfirmationDialogWidget::CloseDialog()
{
	Hide();
}

void UConfirmationDialogWidget::BindButtonEvents()
{
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UConfirmationDialogWidget::OnConfirmClicked);
		ApplyButtonHoverAnimation(ConfirmButton);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UConfirmationDialogWidget::OnCancelClicked);
		ApplyButtonHoverAnimation(CancelButton);
	}
}

void UConfirmationDialogWidget::UnbindButtonEvents()
{
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.RemoveDynamic(this, &UConfirmationDialogWidget::OnConfirmClicked);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.RemoveDynamic(this, &UConfirmationDialogWidget::OnCancelClicked);
	}
}

void UConfirmationDialogWidget::SetButtonTextForType(EConfirmationDialogType DialogType)
{
	FText ConfirmText;
	FText CancelText;
	bool bShowCancel = true;

	switch (DialogType)
	{
	case EConfirmationDialogType::YesNo:
		ConfirmText = FText::FromString(TEXT("はい"));
		CancelText = FText::FromString(TEXT("いいえ"));
		break;

	case EConfirmationDialogType::OkCancel:
		ConfirmText = FText::FromString(TEXT("OK"));
		CancelText = FText::FromString(TEXT("キャンセル"));
		break;

	case EConfirmationDialogType::Ok:
		ConfirmText = FText::FromString(TEXT("OK"));
		bShowCancel = false;
		break;

	case EConfirmationDialogType::Custom:
		// カスタムの場合は変更しない
		return;
	}

	if (ConfirmButtonText)
	{
		ConfirmButtonText->SetText(ConfirmText);
	}

	if (CancelButtonText)
	{
		CancelButtonText->SetText(CancelText);
	}

	if (CancelButton)
	{
		CancelButton->SetVisibility(bShowCancel ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UConfirmationDialogWidget::OnConfirmClicked()
{
	PlayUISound(ClickSound);

	UE_LOG(LogDawnlight, Log, TEXT("[ConfirmationDialogWidget] 確認"));

	// ダイアログを閉じる
	CloseDialog();

	// デリゲートを発火
	OnDialogConfirmed.Broadcast();
}

void UConfirmationDialogWidget::OnCancelClicked()
{
	PlayUISound(BackSound);

	UE_LOG(LogDawnlight, Log, TEXT("[ConfirmationDialogWidget] キャンセル"));

	// ダイアログを閉じる
	CloseDialog();

	// デリゲートを発火
	OnDialogCancelled.Broadcast();
}

void UConfirmationDialogWidget::OnDialogButtonHovered()
{
	// ジューシーアニメーションがホバーサウンドを処理
}
