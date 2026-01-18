// Copyright Epic Games, Inc. All Rights Reserved.

#include "PauseMenuWidget.h"
#include "WidgetUtilities.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Dawnlight.h"

UPauseMenuWidget::UPauseMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// BindWidgetで見つからなかった場合、手動で検索
	if (!ResumeButton)
	{
		ResumeButton = FindWidgetByBaseName<UButton>(this, TEXT("ResumeButton"));
	}
	if (!SettingsButton)
	{
		SettingsButton = FindWidgetByBaseName<UButton>(this, TEXT("SettingsButton"));
	}
	if (!RestartButton)
	{
		RestartButton = FindWidgetByBaseName<UButton>(this, TEXT("RestartButton"));
	}
	if (!MainMenuButton)
	{
		MainMenuButton = FindWidgetByBaseName<UButton>(this, TEXT("MainMenuButton"));
	}
	if (!PauseTitleText)
	{
		PauseTitleText = FindWidgetByBaseName<UTextBlock>(this, TEXT("PauseTitleText"));
	}
	if (!BackgroundOverlay)
	{
		BackgroundOverlay = FindWidgetByBaseName<UImage>(this, TEXT("BackgroundOverlay"));
	}

	BindButtonEvents();

	UE_LOG(LogDawnlight, Log, TEXT("[PauseMenuWidget] 初期化完了"));
}

void UPauseMenuWidget::NativeDestruct()
{
	UnbindButtonEvents();

	Super::NativeDestruct();
}

FReply UPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// Escキーで再開
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnResumeClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPauseMenuWidget::BindButtonEvents()
{
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
		ApplyButtonHoverAnimation(ResumeButton);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnSettingsClicked);
		ApplyButtonHoverAnimation(SettingsButton);
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartClicked);
		ApplyButtonHoverAnimation(RestartButton);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnMainMenuClicked);
		ApplyButtonHoverAnimation(MainMenuButton);
	}
}

void UPauseMenuWidget::UnbindButtonEvents()
{
	if (ResumeButton)
	{
		ResumeButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnSettingsClicked);
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnRestartClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnMainMenuClicked);
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	PlayUISound(ClickSound);
	OnResumeRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[PauseMenuWidget] 再開"));
}

void UPauseMenuWidget::OnSettingsClicked()
{
	PlayUISound(ClickSound);
	OnPauseSettingsRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[PauseMenuWidget] 設定を開く"));
}

void UPauseMenuWidget::OnRestartClicked()
{
	PlayUISound(ClickSound);
	OnRestartRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[PauseMenuWidget] リスタート"));
}

void UPauseMenuWidget::OnMainMenuClicked()
{
	PlayUISound(ClickSound);
	OnReturnToMainMenuRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[PauseMenuWidget] メインメニューに戻る"));
}

void UPauseMenuWidget::OnMenuButtonHovered()
{
	// ジューシーアニメーションがホバーサウンドを処理
}
