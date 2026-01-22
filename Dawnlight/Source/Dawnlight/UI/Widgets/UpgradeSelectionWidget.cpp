// Copyright Epic Games, Inc. All Rights Reserved.

#include "UpgradeSelectionWidget.h"
#include "Dawnlight.h"
#include "UpgradeCardWidget.h"
#include "Subsystems/UpgradeSubsystem.h"
#include "Data/UpgradeDataAsset.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"

void UUpgradeSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// UpgradeSubsystemをキャッシュ
	CacheUpgradeSubsystem();

	// ボタンイベントをバインド
	if (RerollButton)
	{
		RerollButton->OnClicked.AddDynamic(this, &UUpgradeSelectionWidget::OnRerollButtonClicked);
	}
	if (SkipButton)
	{
		SkipButton->OnClicked.AddDynamic(this, &UUpgradeSelectionWidget::OnSkipButtonClicked);
	}

	// 初期非表示
	SetVisibility(ESlateVisibility::Collapsed);

	// リロール回数を初期化
	RemainingRerolls = MaxRerolls;

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSelectionWidget] アップグレード選択ウィジェット初期化完了"));
}

void UUpgradeSelectionWidget::NativeDestruct()
{
	// タイマーをクリア
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CloseTimerHandle);
	}

	// ボタンイベントをアンバインド
	if (RerollButton)
	{
		RerollButton->OnClicked.RemoveDynamic(this, &UUpgradeSelectionWidget::OnRerollButtonClicked);
	}
	if (SkipButton)
	{
		SkipButton->OnClicked.RemoveDynamic(this, &UUpgradeSelectionWidget::OnSkipButtonClicked);
	}

	Super::NativeDestruct();
}

void UUpgradeSelectionWidget::CacheUpgradeSubsystem()
{
	if (UWorld* World = GetWorld())
	{
		UpgradeSubsystem = World->GetSubsystem<UUpgradeSubsystem>();
	}
}

void UUpgradeSelectionWidget::ShowWithChoices(const TArray<UUpgradeDataAsset*>& Choices, int32 WaveNumber)
{
	CurrentChoices.Empty();
	for (UUpgradeDataAsset* Choice : Choices)
	{
		CurrentChoices.Add(Choice);
	}
	CurrentWaveNumber = WaveNumber;
	bIsWaitingForSelection = true;
	bHasSelected = false;

	// Wave情報を更新
	if (WaveInfoText)
	{
		FString WaveString = FString::Printf(TEXT("WAVE %d クリア！"), WaveNumber);
		WaveInfoText->SetText(FText::FromString(WaveString));
	}

	// タイトルを設定
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("アップグレードを選択")));
	}

	// 説明テキストを設定
	if (InstructionText)
	{
		InstructionText->SetText(FText::FromString(TEXT("カードを選んで能力を強化しよう")));
	}

	// カードを生成・更新
	CreateCardWidgets(Choices.Num());
	UpdateCards(Choices);

	// リロールボタンの状態を更新
	UpdateRerollButton();

	// ウィジェットを表示
	SetVisibility(ESlateVisibility::Visible);
	Show();

	// カードの登場アニメーション
	PlayCardsRevealAnimation();

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSelectionWidget] アップグレード選択画面表示 Wave: %d, 選択肢: %d"),
		WaveNumber, Choices.Num());
}

void UUpgradeSelectionWidget::ClearAndHide()
{
	bIsWaitingForSelection = false;

	// カードをクリア
	for (UUpgradeCardWidget* Card : CardWidgets)
	{
		if (Card)
		{
			Card->RemoveFromParent();
		}
	}
	CardWidgets.Empty();
	CurrentChoices.Empty();

	// 非表示
	Hide();
}

void UUpgradeSelectionWidget::RequestReroll()
{
	if (!CanReroll() || !UpgradeSubsystem)
	{
		PlayErrorShake(RerollButton);
		return;
	}

	// リロール回数を減らす
	RemainingRerolls--;

	// 新しい選択肢を生成
	TArray<UUpgradeDataAsset*> NewChoices = UpgradeSubsystem->RerollUpgradeChoices(CurrentWaveNumber, 3);

	// 選択肢を更新
	CurrentChoices.Empty();
	for (UUpgradeDataAsset* Choice : NewChoices)
	{
		CurrentChoices.Add(Choice);
	}

	// カードを更新
	UpdateCards(NewChoices);

	// リロールボタンを更新
	UpdateRerollButton();

	// 登場アニメーション
	PlayCardsRevealAnimation();

	// サウンドを再生
	PlayUISound(ClickSound);

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSelectionWidget] リロール実行 残り: %d"), RemainingRerolls);
}

void UUpgradeSelectionWidget::RequestSkip()
{
	if (!bIsWaitingForSelection || bHasSelected)
	{
		return;
	}

	bHasSelected = true;
	bIsWaitingForSelection = false;

	// スキップサウンドを再生
	PlayUISound(BackSound);

	// nullptr を選択として通知（スキップ）
	OnSelectionComplete.Broadcast(nullptr);

	// 閉じる
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CloseTimerHandle,
			this,
			&UUpgradeSelectionWidget::ExecuteClose,
			CloseDelay * 0.5f,
			false
		);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSelectionWidget] アップグレードスキップ"));
}

bool UUpgradeSelectionWidget::CanReroll() const
{
	return RemainingRerolls > 0 && bIsWaitingForSelection && !bHasSelected;
}

void UUpgradeSelectionWidget::CreateCardWidgets(int32 Count)
{
	if (!CardContainer || !CardWidgetClass)
	{
		UE_LOG(LogDawnlight, Warning, TEXT("[UpgradeSelectionWidget] CardContainerまたはCardWidgetClassが設定されていません"));
		return;
	}

	// 既存のカードが足りていれば再利用
	if (CardWidgets.Num() >= Count)
	{
		// 余分なカードを非表示
		for (int32 i = Count; i < CardWidgets.Num(); ++i)
		{
			if (CardWidgets[i])
			{
				CardWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		return;
	}

	// 足りない分を作成
	for (int32 i = CardWidgets.Num(); i < Count; ++i)
	{
		UUpgradeCardWidget* NewCard = CreateWidget<UUpgradeCardWidget>(this, CardWidgetClass);
		if (NewCard)
		{
			// コンテナに追加
			UHorizontalBoxSlot* CardSlot = CardContainer->AddChildToHorizontalBox(NewCard);
			if (CardSlot)
			{
				// 均等に配置
				CardSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
				CardSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
				CardSlot->SetPadding(FMargin(20.0f, 0.0f, 20.0f, 0.0f));
			}

			// 選択イベントをバインド
			NewCard->OnCardSelected.AddDynamic(this, &UUpgradeSelectionWidget::OnCardSelected);

			CardWidgets.Add(NewCard);
		}
	}
}

void UUpgradeSelectionWidget::UpdateCards(const TArray<UUpgradeDataAsset*>& Choices)
{
	for (int32 i = 0; i < CardWidgets.Num(); ++i)
	{
		if (!CardWidgets[i])
		{
			continue;
		}

		if (i < Choices.Num())
		{
			CardWidgets[i]->SetUpgradeData(Choices[i]);
			CardWidgets[i]->SetSelectable(true);
			CardWidgets[i]->SetSelected(false);
			CardWidgets[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CardWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UUpgradeSelectionWidget::PlayCardsRevealAnimation()
{
	for (int32 i = 0; i < CardWidgets.Num(); ++i)
	{
		if (CardWidgets[i] && CardWidgets[i]->GetVisibility() == ESlateVisibility::Visible)
		{
			float Delay = CardRevealDelay * i;
			CardWidgets[i]->PlayRevealAnimation(Delay);
		}
	}
}

void UUpgradeSelectionWidget::PlaySelectionAnimation(UUpgradeCardWidget* SelectedCard)
{
	for (UUpgradeCardWidget* Card : CardWidgets)
	{
		if (!Card)
		{
			continue;
		}

		if (Card == SelectedCard)
		{
			Card->PlaySelectAnimation();
		}
		else
		{
			Card->PlayDismissAnimation();
		}
	}
}

void UUpgradeSelectionWidget::UpdateRerollButton()
{
	if (RerollButton)
	{
		RerollButton->SetIsEnabled(CanReroll());
	}

	if (RerollCountText)
	{
		FString RerollString = FString::Printf(TEXT("リロール (%d)"), RemainingRerolls);
		RerollCountText->SetText(FText::FromString(RerollString));

		// 残り0の場合は色を暗くする
		FLinearColor TextColor = RemainingRerolls > 0
			? FLinearColor::White
			: FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
		RerollCountText->SetColorAndOpacity(TextColor);
	}
}

void UUpgradeSelectionWidget::OnCardSelected(UUpgradeDataAsset* SelectedUpgrade)
{
	if (!bIsWaitingForSelection || bHasSelected || !SelectedUpgrade)
	{
		return;
	}

	bHasSelected = true;
	bIsWaitingForSelection = false;

	// 他のカードを選択不可に
	for (UUpgradeCardWidget* Card : CardWidgets)
	{
		if (Card)
		{
			Card->SetSelectable(false);
		}
	}

	// 選択されたカードを見つけて演出
	for (UUpgradeCardWidget* Card : CardWidgets)
	{
		if (Card && Card->GetUpgradeData() == SelectedUpgrade)
		{
			PlaySelectionAnimation(Card);
			break;
		}
	}

	// リロール・スキップボタンを無効化
	if (RerollButton)
	{
		RerollButton->SetIsEnabled(false);
	}
	if (SkipButton)
	{
		SkipButton->SetIsEnabled(false);
	}

	// UpgradeSubsystemにアップグレード取得を通知
	if (UpgradeSubsystem)
	{
		UpgradeSubsystem->AcquireUpgrade(SelectedUpgrade, CurrentWaveNumber);
	}

	// 選択完了デリゲートを発火
	OnSelectionComplete.Broadcast(SelectedUpgrade);

	// 少し待ってから閉じる
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CloseTimerHandle,
			this,
			&UUpgradeSelectionWidget::ExecuteClose,
			CloseDelay,
			false
		);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeSelectionWidget] アップグレード選択完了: %s"),
		*SelectedUpgrade->DisplayName.ToString());
}

void UUpgradeSelectionWidget::OnRerollButtonClicked()
{
	RequestReroll();
}

void UUpgradeSelectionWidget::OnSkipButtonClicked()
{
	RequestSkip();
}

void UUpgradeSelectionWidget::ExecuteClose()
{
	// リロール回数をリセット（次のWave用）
	RemainingRerolls = MaxRerolls;

	ClearAndHide();
}
