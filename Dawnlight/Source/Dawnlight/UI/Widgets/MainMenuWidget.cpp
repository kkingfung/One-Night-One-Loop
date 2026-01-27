// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "MainMenuWidget.h"
#include "WidgetUtilities.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Dawnlight.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// BindWidgetで見つからなかった場合、手動で検索
	if (!NewGameButton)
	{
		NewGameButton = FindWidgetByBaseName<UButton>(this, TEXT("NewGameButton"));
	}
	if (!ContinueButton)
	{
		ContinueButton = FindWidgetByBaseName<UButton>(this, TEXT("ContinueButton"));
	}
	if (!SettingsButton)
	{
		SettingsButton = FindWidgetByBaseName<UButton>(this, TEXT("SettingsButton"));
	}
	if (!CreditsButton)
	{
		CreditsButton = FindWidgetByBaseName<UButton>(this, TEXT("CreditsButton"));
	}
	if (!ExitButton)
	{
		ExitButton = FindWidgetByBaseName<UButton>(this, TEXT("ExitButton"));
	}
	if (!TitleText)
	{
		TitleText = FindWidgetByBaseName<UTextBlock>(this, TEXT("TitleText"));
	}
	if (!SubtitleText)
	{
		SubtitleText = FindWidgetByBaseName<UTextBlock>(this, TEXT("SubtitleText"));
	}
	if (!VersionText)
	{
		VersionText = FindWidgetByBaseName<UTextBlock>(this, TEXT("VersionText"));
	}
	if (!BackgroundImage)
	{
		BackgroundImage = FindWidgetByBaseName<UImage>(this, TEXT("BackgroundImage"));
	}

	BindButtonEvents();
	SetupJuicyAnimations();

	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuWidget] 初期化完了"));
}

void UMainMenuWidget::Show()
{
	// 親クラスのShow()を呼び出す前に、メニュー項目を初期状態に設定
	TArray<UWidget*> MenuItems = GetMenuItems();
	for (UWidget* Item : MenuItems)
	{
		if (Item)
		{
			Item->SetRenderOpacity(0.0f);
		}
	}

	// タイトルも初期状態に
	if (TitleText)
	{
		TitleText->SetRenderOpacity(0.0f);
	}
	if (SubtitleText)
	{
		SubtitleText->SetRenderOpacity(0.0f);
	}

	// 親クラスのShow()を呼び出し
	Super::Show();

	// タイトルをフェードイン
	if (TitleText)
	{
		PlayWidgetFadeIn(TitleText, 0.5f, 0.0f);
	}
	if (SubtitleText)
	{
		PlayWidgetFadeIn(SubtitleText, 0.5f, 0.2f);
	}

	// メニュー項目をスタガーアニメーション
	PlayMenuItemsStaggerAnimation(MenuItems, EUIAnimationType::SlideInLeft);
}

void UMainMenuWidget::NativeDestruct()
{
	UnbindButtonEvents();

	Super::NativeDestruct();
}

void UMainMenuWidget::SetContinueButtonEnabled(bool bEnabled)
{
	if (ContinueButton)
	{
		ContinueButton->SetIsEnabled(bEnabled);
		ContinueButton->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UMainMenuWidget::SetVersionText(const FString& Version)
{
	if (VersionText)
	{
		VersionText->SetText(FText::FromString(Version));
	}
}

void UMainMenuWidget::BindButtonEvents()
{
	// クリックイベントのみバインド（ホバーはSetupJuicyAnimationsで処理）
	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGameClicked);
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueClicked);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsClicked);
	}

	if (CreditsButton)
	{
		CreditsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreditsClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitClicked);
	}
}

void UMainMenuWidget::UnbindButtonEvents()
{
	if (NewGameButton)
	{
		NewGameButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnNewGameClicked);
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnContinueClicked);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnSettingsClicked);
	}

	if (CreditsButton)
	{
		CreditsButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnCreditsClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveDynamic(this, &UMainMenuWidget::OnExitClicked);
	}
}

void UMainMenuWidget::OnNewGameClicked()
{
	PlayUISound(ClickSound);
	OnStartGameRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuWidget] 新規ゲーム開始"));
}

void UMainMenuWidget::OnContinueClicked()
{
	PlayUISound(ClickSound);
	OnContinueGameRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuWidget] 続きから"));
}

void UMainMenuWidget::OnSettingsClicked()
{
	PlayUISound(ClickSound);
	OnSettingsRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuWidget] 設定を開く"));
}

void UMainMenuWidget::OnCreditsClicked()
{
	PlayUISound(ClickSound);
	OnCreditsRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuWidget] クレジットを表示"));
}

void UMainMenuWidget::OnExitClicked()
{
	PlayUISound(ClickSound);
	OnExitGameRequested.Broadcast();

	UE_LOG(LogDawnlight, Log, TEXT("[MainMenuWidget] ゲーム終了"));
}

void UMainMenuWidget::OnMenuButtonHovered()
{
	// ジューシーアニメーションがホバーサウンドを処理するため、ここでは何もしない
}

void UMainMenuWidget::SetupJuicyAnimations()
{
	// 各ボタンにホバーアニメーションを適用
	if (NewGameButton)
	{
		ApplyButtonHoverAnimation(NewGameButton);
	}
	if (ContinueButton)
	{
		ApplyButtonHoverAnimation(ContinueButton);
	}
	if (SettingsButton)
	{
		ApplyButtonHoverAnimation(SettingsButton);
	}
	if (CreditsButton)
	{
		ApplyButtonHoverAnimation(CreditsButton);
	}
	if (ExitButton)
	{
		ApplyButtonHoverAnimation(ExitButton);
	}
}

TArray<UWidget*> UMainMenuWidget::GetMenuItems() const
{
	TArray<UWidget*> Items;

	if (NewGameButton)
	{
		Items.Add(NewGameButton);
	}
	if (ContinueButton)
	{
		Items.Add(ContinueButton);
	}
	if (SettingsButton)
	{
		Items.Add(SettingsButton);
	}
	if (CreditsButton)
	{
		Items.Add(CreditsButton);
	}
	if (ExitButton)
	{
		Items.Add(ExitButton);
	}

	return Items;
}
