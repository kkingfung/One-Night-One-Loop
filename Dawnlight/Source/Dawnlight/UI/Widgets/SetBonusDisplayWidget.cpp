// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "SetBonusDisplayWidget.h"
#include "Dawnlight.h"
#include "Subsystems/UpgradeSubsystem.h"
#include "Subsystems/SoulCollectionSubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "Engine/Texture2D.h"

void USetBonusDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// デフォルト値を初期化
	InitializeDefaults();

	// サブシステムをキャッシュ
	CacheSubsystems();

	// イベントをバインド
	if (SoulCollectionSubsystem)
	{
		SoulCollectionSubsystem->OnSoulCollected.AddDynamic(this, &USetBonusDisplayWidget::OnSoulCollected);
	}
	if (UpgradeSubsystem)
	{
		UpgradeSubsystem->OnSetBonusActivated.AddDynamic(this, &USetBonusDisplayWidget::OnSetBonusActivated);
	}

	// UI要素を生成
	CreateBonusItemWidgets();

	// 初回更新
	RefreshDisplay();

	UE_LOG(LogDawnlight, Log, TEXT("[SetBonusDisplayWidget] セットボーナス表示ウィジェット初期化完了"));
}

void USetBonusDisplayWidget::NativeDestruct()
{
	// イベントをアンバインド
	if (SoulCollectionSubsystem)
	{
		SoulCollectionSubsystem->OnSoulCollected.RemoveDynamic(this, &USetBonusDisplayWidget::OnSoulCollected);
	}
	if (UpgradeSubsystem)
	{
		UpgradeSubsystem->OnSetBonusActivated.RemoveDynamic(this, &USetBonusDisplayWidget::OnSetBonusActivated);
	}

	Super::NativeDestruct();
}

void USetBonusDisplayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 自動更新
	if (AutoRefreshInterval > 0.0f)
	{
		AutoRefreshTimer += InDeltaTime;
		if (AutoRefreshTimer >= AutoRefreshInterval)
		{
			AutoRefreshTimer = 0.0f;
			RefreshDisplay();
		}
	}
}

void USetBonusDisplayWidget::CacheSubsystems()
{
	if (UWorld* World = GetWorld())
	{
		UpgradeSubsystem = World->GetSubsystem<UUpgradeSubsystem>();
		SoulCollectionSubsystem = World->GetSubsystem<USoulCollectionSubsystem>();
	}
}

void USetBonusDisplayWidget::InitializeDefaults()
{
	// デフォルトの段階閾値
	if (BonusTierThresholds.Num() == 0)
	{
		BonusTierThresholds.Add(3);   // 段階1: 3個
		BonusTierThresholds.Add(5);   // 段階2: 5個
		BonusTierThresholds.Add(8);   // 段階3: 8個
	}

	// デフォルトの表示ソウルタイプ
	if (DisplaySoulTypes.Num() == 0)
	{
		DisplaySoulTypes.Add(ESoulType::Tiger);
		DisplaySoulTypes.Add(ESoulType::Horse);
		DisplaySoulTypes.Add(ESoulType::Dog);
		DisplaySoulTypes.Add(ESoulType::Cat);
		DisplaySoulTypes.Add(ESoulType::Deer);
		DisplaySoulTypes.Add(ESoulType::Wolf);
	}

	// デフォルトの色
	if (SoulTypeColors.Num() == 0)
	{
		SoulTypeColors.Add(ESoulType::Tiger, FLinearColor(1.0f, 0.5f, 0.0f, 1.0f));   // オレンジ
		SoulTypeColors.Add(ESoulType::Horse, FLinearColor(0.6f, 0.4f, 0.2f, 1.0f));   // 茶
		SoulTypeColors.Add(ESoulType::Dog, FLinearColor(0.8f, 0.7f, 0.5f, 1.0f));     // ベージュ
		SoulTypeColors.Add(ESoulType::Cat, FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));     // 灰
		SoulTypeColors.Add(ESoulType::Deer, FLinearColor(0.4f, 0.8f, 0.4f, 1.0f));    // 緑
		SoulTypeColors.Add(ESoulType::Wolf, FLinearColor(0.5f, 0.5f, 0.6f, 1.0f));    // 青灰
		SoulTypeColors.Add(ESoulType::Golden, FLinearColor(1.0f, 0.9f, 0.3f, 1.0f));  // 金
		SoulTypeColors.Add(ESoulType::Corrupted, FLinearColor(0.5f, 0.0f, 0.5f, 1.0f)); // 紫
	}
}

void USetBonusDisplayWidget::RefreshDisplay()
{
	// 全てのソウルタイプの進捗を更新
	for (ESoulType SoulType : DisplaySoulTypes)
	{
		FSetBonusProgressInfo Progress = CalculateProgress(SoulType);
		ProgressCache.Add(SoulType, Progress);
		UpdateBonusItemWidget(SoulType, Progress);
	}
}

FSetBonusProgressInfo USetBonusDisplayWidget::GetProgressForSoulType(ESoulType SoulType) const
{
	if (const FSetBonusProgressInfo* CachedProgress = ProgressCache.Find(SoulType))
	{
		return *CachedProgress;
	}
	return CalculateProgress(SoulType);
}

TArray<FSetBonusProgressInfo> USetBonusDisplayWidget::GetAllProgress() const
{
	TArray<FSetBonusProgressInfo> Result;
	for (const auto& Pair : ProgressCache)
	{
		Result.Add(Pair.Value);
	}
	return Result;
}

FSetBonusProgressInfo USetBonusDisplayWidget::CalculateProgress(ESoulType SoulType) const
{
	FSetBonusProgressInfo Progress;
	Progress.SoulType = SoulType;
	Progress.MaxTier = BonusTierThresholds.Num();

	// ソウル収集数を取得
	if (SoulCollectionSubsystem)
	{
		// GameplayTagを使ってソウル数を取得
		// 注: SoulCollectionSubsystemの実装に依存
		Progress.CurrentCount = SoulCollectionSubsystem->GetTotalSoulCount();  // 仮の実装

		// ESoulTypeに対応するカウントを取得する必要がある
		// 実際の実装ではSoulCollectionSubsystemにタイプ別カウントメソッドが必要
	}

	// UpgradeSubsystemからセットボーナス段階を取得
	if (UpgradeSubsystem)
	{
		// 現在のアクティブ段階を確認
		// この実装はUpgradeSubsystemの実装に依存
	}

	// 段階を計算
	Progress.CurrentTier = 0;
	for (int32 i = 0; i < BonusTierThresholds.Num(); ++i)
	{
		if (Progress.CurrentCount >= BonusTierThresholds[i])
		{
			Progress.CurrentTier = i + 1;
		}
	}

	// 次の段階までの進捗を計算
	if (Progress.CurrentTier < Progress.MaxTier)
	{
		int32 NextThreshold = BonusTierThresholds[Progress.CurrentTier];
		int32 CurrentThreshold = Progress.CurrentTier > 0 ? BonusTierThresholds[Progress.CurrentTier - 1] : 0;

		Progress.NextTierCount = NextThreshold;
		if (NextThreshold > CurrentThreshold)
		{
			Progress.Progress = static_cast<float>(Progress.CurrentCount - CurrentThreshold) /
				static_cast<float>(NextThreshold - CurrentThreshold);
		}
	}
	else
	{
		Progress.Progress = 1.0f;
		Progress.NextTierCount = BonusTierThresholds.Last();
	}

	Progress.bIsActive = Progress.CurrentTier > 0;

	return Progress;
}

void USetBonusDisplayWidget::SetCompactMode(bool bCompact)
{
	bIsCompactMode = bCompact;
	if (bCompact)
	{
		bIsDetailedMode = false;
	}
	RefreshDisplay();
}

void USetBonusDisplayWidget::SetDetailedMode(bool bDetailed)
{
	bIsDetailedMode = bDetailed;
	if (bDetailed)
	{
		bIsCompactMode = false;
	}
	RefreshDisplay();
}

void USetBonusDisplayWidget::PlayBonusAchievedAnimation(ESoulType SoulType, int32 Tier)
{
	TWeakObjectPtr<UWidget>* WidgetPtr = ItemWidgetCache.Find(SoulType);
	if (WidgetPtr && WidgetPtr->IsValid())
	{
		// パルスアニメーション
		PlayAttentionPulse(WidgetPtr->Get(), false);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[SetBonusDisplayWidget] セットボーナス達成: %s 段階 %d"),
		*GetSoulTypeName(SoulType).ToString(), Tier);
}

void USetBonusDisplayWidget::PlaySoulCollectedAnimation(ESoulType SoulType)
{
	TWeakObjectPtr<UWidget>* WidgetPtr = ItemWidgetCache.Find(SoulType);
	if (WidgetPtr && WidgetPtr->IsValid())
	{
		// 軽いフラッシュアニメーション
		PlayWidgetFadeIn(WidgetPtr->Get(), 0.1f);
	}
}

void USetBonusDisplayWidget::CreateBonusItemWidgets()
{
	if (!BonusItemContainer)
	{
		return;
	}

	// 既存のウィジェットをクリア
	BonusItemContainer->ClearChildren();
	ItemWidgetCache.Empty();

	// 各ソウルタイプのウィジェットを作成
	for (ESoulType SoulType : DisplaySoulTypes)
	{
		UWidget* ItemWidget = CreateSingleBonusItemWidget(SoulType);
		if (ItemWidget)
		{
			BonusItemContainer->AddChild(ItemWidget);
			ItemWidgetCache.Add(SoulType, ItemWidget);
		}
	}
}

UWidget* USetBonusDisplayWidget::CreateSingleBonusItemWidget(ESoulType SoulType)
{
	// 水平ボックスを作成（アイコン + プログレスバー + テキスト）
	UHorizontalBox* ItemBox = NewObject<UHorizontalBox>(this);
	if (!ItemBox)
	{
		return nullptr;
	}

	// ソウルタイプの色を取得
	FLinearColor TypeColor = FLinearColor::White;
	if (const FLinearColor* FoundColor = SoulTypeColors.Find(SoulType))
	{
		TypeColor = *FoundColor;
	}

	// アイコン（またはカラーボックス）
	UImage* IconImage = NewObject<UImage>(this);
	if (IconImage)
	{
		if (const TSoftObjectPtr<UTexture2D>* IconPtr = SoulTypeIcons.Find(SoulType))
		{
			if (IconPtr->IsValid() || !IconPtr->IsNull())
			{
				UTexture2D* IconTexture = IconPtr->LoadSynchronous();
				if (IconTexture)
				{
					IconImage->SetBrushFromTexture(IconTexture);
				}
			}
		}
		IconImage->SetColorAndOpacity(TypeColor);
		IconImage->SetDesiredSizeOverride(FVector2D(24.0f, 24.0f));

		UHorizontalBoxSlot* IconSlot = ItemBox->AddChildToHorizontalBox(IconImage);
		if (IconSlot)
		{
			IconSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
			IconSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
	}

	// ソウルタイプ名
	UTextBlock* NameText = NewObject<UTextBlock>(this);
	if (NameText)
	{
		NameText->SetText(GetSoulTypeName(SoulType));
		NameText->SetColorAndOpacity(FLinearColor::White);

		UHorizontalBoxSlot* NameSlot = ItemBox->AddChildToHorizontalBox(NameText);
		if (NameSlot)
		{
			NameSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
			NameSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
	}

	// プログレスバー
	UProgressBar* ProgressBarWidget = NewObject<UProgressBar>(this);
	if (ProgressBarWidget)
	{
		ProgressBarWidget->SetPercent(0.0f);
		ProgressBarWidget->SetFillColorAndOpacity(TypeColor);

		UHorizontalBoxSlot* ProgressSlot = ItemBox->AddChildToHorizontalBox(ProgressBarWidget);
		if (ProgressSlot)
		{
			ProgressSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			ProgressSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
			ProgressSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
	}

	// カウントテキスト
	UTextBlock* CountText = NewObject<UTextBlock>(this);
	if (CountText)
	{
		CountText->SetText(FText::FromString(TEXT("0/3")));
		CountText->SetColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f));

		UHorizontalBoxSlot* CountSlot = ItemBox->AddChildToHorizontalBox(CountText);
		if (CountSlot)
		{
			CountSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
	}

	return ItemBox;
}

void USetBonusDisplayWidget::UpdateBonusItemWidget(ESoulType SoulType, const FSetBonusProgressInfo& Progress)
{
	TWeakObjectPtr<UWidget>* WidgetPtr = ItemWidgetCache.Find(SoulType);
	if (!WidgetPtr || !WidgetPtr->IsValid())
	{
		return;
	}

	UHorizontalBox* ItemBox = Cast<UHorizontalBox>(WidgetPtr->Get());
	if (!ItemBox)
	{
		return;
	}

	// プログレスバーを更新
	for (int32 i = 0; i < ItemBox->GetChildrenCount(); ++i)
	{
		UWidget* Child = ItemBox->GetChildAt(i);

		// プログレスバーを探して更新
		if (UProgressBar* ProgressBar = Cast<UProgressBar>(Child))
		{
			ProgressBar->SetPercent(FMath::Clamp(Progress.Progress, 0.0f, 1.0f));

			// アクティブ状態で色を変更
			if (Progress.bIsActive)
			{
				if (const FLinearColor* TypeColor = SoulTypeColors.Find(SoulType))
				{
					ProgressBar->SetFillColorAndOpacity(*TypeColor);
				}
			}
			else
			{
				ProgressBar->SetFillColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f, 1.0f));
			}
		}

		// カウントテキストを探して更新
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Child))
		{
			FString CurrentText = TextBlock->GetText().ToString();
			if (CurrentText.Contains(TEXT("/")))  // カウントテキストを識別
			{
				FString CountString = FString::Printf(TEXT("%d/%d"),
					Progress.CurrentCount,
					Progress.NextTierCount);
				TextBlock->SetText(FText::FromString(CountString));

				// アクティブ状態で色を変更
				if (Progress.bIsActive)
				{
					if (const FLinearColor* TypeColor = SoulTypeColors.Find(SoulType))
					{
						TextBlock->SetColorAndOpacity(*TypeColor);
					}
				}
				else
				{
					TextBlock->SetColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f));
				}
			}
		}
	}
}

FText USetBonusDisplayWidget::GetSoulTypeName(ESoulType SoulType) const
{
	switch (SoulType)
	{
	case ESoulType::Tiger:
		return FText::FromString(TEXT("虎"));
	case ESoulType::Horse:
		return FText::FromString(TEXT("馬"));
	case ESoulType::Dog:
		return FText::FromString(TEXT("犬"));
	case ESoulType::Cat:
		return FText::FromString(TEXT("猫"));
	case ESoulType::Deer:
		return FText::FromString(TEXT("鹿"));
	case ESoulType::Wolf:
		return FText::FromString(TEXT("狼"));
	case ESoulType::Golden:
		return FText::FromString(TEXT("金"));
	case ESoulType::Corrupted:
		return FText::FromString(TEXT("穢"));
	default:
		return FText::FromString(TEXT("?"));
	}
}

FText USetBonusDisplayWidget::GetTierName(int32 Tier) const
{
	switch (Tier)
	{
	case 1:
		return FText::FromString(TEXT("I"));
	case 2:
		return FText::FromString(TEXT("II"));
	case 3:
		return FText::FromString(TEXT("III"));
	default:
		return FText::FromString(FString::Printf(TEXT("%d"), Tier));
	}
}

void USetBonusDisplayWidget::OnSoulCollected(const FSoulCollectedEventData& EventData)
{
	// 収集されたソウルのタイプを取得（仮実装）
	// 実際にはEventDataからソウルタイプを取得する必要がある
	RefreshDisplay();

	UE_LOG(LogDawnlight, Verbose, TEXT("[SetBonusDisplayWidget] ソウル収集を検知、表示を更新"));
}

void USetBonusDisplayWidget::OnSetBonusActivated(ESoulType SoulType, int32 Tier)
{
	PlayBonusAchievedAnimation(SoulType, Tier);
	RefreshDisplay();
}
