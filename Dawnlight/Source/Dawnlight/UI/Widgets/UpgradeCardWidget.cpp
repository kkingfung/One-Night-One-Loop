// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "UpgradeCardWidget.h"
#include "Dawnlight.h"
#include "Data/UpgradeDataAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Engine/Texture2D.h"

void UUpgradeCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// デフォルト色を初期化
	InitializeDefaultColors();

	// ボタンイベントをバインド
	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UUpgradeCardWidget::OnSelectButtonClicked);
		SelectButton->OnHovered.AddDynamic(this, &UUpgradeCardWidget::OnSelectButtonHovered);
		SelectButton->OnUnhovered.AddDynamic(this, &UUpgradeCardWidget::OnSelectButtonUnhovered);
	}

	// グロー効果を初期非表示
	if (GlowEffect)
	{
		GlowEffect->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[UpgradeCardWidget] カードウィジェット初期化完了"));
}

void UUpgradeCardWidget::NativeDestruct()
{
	// イベントをアンバインド
	if (SelectButton)
	{
		SelectButton->OnClicked.RemoveDynamic(this, &UUpgradeCardWidget::OnSelectButtonClicked);
		SelectButton->OnHovered.RemoveDynamic(this, &UUpgradeCardWidget::OnSelectButtonHovered);
		SelectButton->OnUnhovered.RemoveDynamic(this, &UUpgradeCardWidget::OnSelectButtonUnhovered);
	}

	Super::NativeDestruct();
}

void UUpgradeCardWidget::SetUpgradeData(UUpgradeDataAsset* InUpgradeData)
{
	UpgradeData = InUpgradeData;
	UpdateUI();

	if (UpgradeData)
	{
		UE_LOG(LogDawnlight, Log, TEXT("[UpgradeCardWidget] アップグレードを設定: %s"),
			*UpgradeData->DisplayName.ToString());
	}
}

void UUpgradeCardWidget::SetSelectable(bool bSelectable)
{
	bIsSelectable = bSelectable;

	if (SelectButton)
	{
		SelectButton->SetIsEnabled(bSelectable);
	}
}

void UUpgradeCardWidget::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;

	// 選択状態のビジュアルを更新
	if (bSelected)
	{
		PlaySelectAnimation();
	}
}

void UUpgradeCardWidget::PlayRevealAnimation(float Delay)
{
	// フェードイン + スライドインアニメーション
	PlayWidgetSlideIn(this, EUIAnimationType::SlideInBottom, 50.0f, 0.3f, Delay);
}

void UUpgradeCardWidget::PlaySelectAnimation()
{
	// 選択時のパルスアニメーション
	PlayAttentionPulse(CardBorder, false);
}

void UUpgradeCardWidget::PlayDismissAnimation()
{
	// フェードアウトアニメーション
	PlayWidgetFadeOut(this, 0.2f);
}

void UUpgradeCardWidget::UpdateUI()
{
	if (!UpgradeData)
	{
		return;
	}

	// アップグレード名を設定
	if (UpgradeName)
	{
		UpgradeName->SetText(UpgradeData->DisplayName);
	}

	// 説明文を設定
	if (UpgradeDescription)
	{
		UpgradeDescription->SetText(UpgradeData->Description);
	}

	// アイコンを設定
	if (UpgradeIcon)
	{
		if (UpgradeData->Icon.IsValid() || !UpgradeData->Icon.IsNull())
		{
			UTexture2D* IconTexture = UpgradeData->Icon.LoadSynchronous();
			if (IconTexture)
			{
				UpgradeIcon->SetBrushFromTexture(IconTexture);
			}
			else if (DefaultIcon)
			{
				UpgradeIcon->SetBrushFromTexture(DefaultIcon);
			}
		}
		else if (DefaultIcon)
		{
			UpgradeIcon->SetBrushFromTexture(DefaultIcon);
		}
	}

	// レアリティテキストを設定
	if (RarityText)
	{
		RarityText->SetText(GetRarityDisplayName(UpgradeData->Rarity));
	}

	// カテゴリテキストを設定
	if (CategoryText)
	{
		CategoryText->SetText(GetCategoryDisplayName(UpgradeData->Category));
	}

	// レアリティスタイルを適用
	ApplyRarityStyle(UpgradeData->Rarity);

	// ステータス修正を更新
	UpdateStatModifiers();
}

void UUpgradeCardWidget::ApplyRarityStyle(EUpgradeRarity Rarity)
{
	// 枠色を設定
	if (CardBorder)
	{
		if (const FLinearColor* BorderColor = RarityBorderColors.Find(Rarity))
		{
			CardBorder->SetBrushColor(*BorderColor);
		}
	}

	// 背景色を設定
	if (CardBackground)
	{
		if (const FLinearColor* BgColor = RarityBackgroundColors.Find(Rarity))
		{
			CardBackground->SetColorAndOpacity(*BgColor);
		}
	}

	// テキスト色を設定
	if (RarityText)
	{
		if (const FLinearColor* TextColor = RarityTextColors.Find(Rarity))
		{
			RarityText->SetColorAndOpacity(*TextColor);
		}
	}

	// レジェンダリーの場合、グロー効果を表示
	if (GlowEffect)
	{
		const bool bShowGlow = (Rarity == EUpgradeRarity::Legendary || Rarity == EUpgradeRarity::Epic);
		GlowEffect->SetVisibility(bShowGlow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		if (bShowGlow)
		{
			PlayAttentionPulse(GlowEffect, true);
		}
	}
}

void UUpgradeCardWidget::UpdateStatModifiers()
{
	if (!StatModifiersBox || !UpgradeData)
	{
		return;
	}

	// 既存の子ウィジェットをクリア
	StatModifiersBox->ClearChildren();

	// 各ステータス修正を表示
	for (const FStatModifier& Modifier : UpgradeData->StatModifiers)
	{
		// テキストブロックを作成
		UTextBlock* StatText = NewObject<UTextBlock>(this);
		if (!StatText)
		{
			continue;
		}

		// 修正値のテキストを構築
		FString ModifierText;
		FText StatName = GetStatTypeName(Modifier.StatType);

		if (Modifier.AdditiveValue != 0.0f)
		{
			ModifierText = FString::Printf(TEXT("%s: %s%.0f"),
				*StatName.ToString(),
				Modifier.AdditiveValue > 0 ? TEXT("+") : TEXT(""),
				Modifier.AdditiveValue);
		}
		else if (Modifier.MultiplicativeValue != 0.0f)
		{
			ModifierText = FString::Printf(TEXT("%s: %s%.0f%%"),
				*StatName.ToString(),
				Modifier.MultiplicativeValue > 0 ? TEXT("+") : TEXT(""),
				Modifier.MultiplicativeValue * 100.0f);
		}

		if (!ModifierText.IsEmpty())
		{
			StatText->SetText(FText::FromString(ModifierText));

			// プラスは緑、マイナスは赤
			const bool bPositive = (Modifier.AdditiveValue > 0.0f || Modifier.MultiplicativeValue > 0.0f);
			StatText->SetColorAndOpacity(bPositive
				? FLinearColor(0.3f, 0.9f, 0.3f, 1.0f)  // 緑
				: FLinearColor(0.9f, 0.3f, 0.3f, 1.0f)); // 赤

			StatModifiersBox->AddChild(StatText);
		}
	}
}

void UUpgradeCardWidget::InitializeDefaultColors()
{
	// 枠色（レアリティごと）
	if (RarityBorderColors.Num() == 0)
	{
		RarityBorderColors.Add(EUpgradeRarity::Common, FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));       // 灰色
		RarityBorderColors.Add(EUpgradeRarity::Uncommon, FLinearColor(0.3f, 0.7f, 0.3f, 1.0f));     // 緑
		RarityBorderColors.Add(EUpgradeRarity::Rare, FLinearColor(0.3f, 0.5f, 0.9f, 1.0f));         // 青
		RarityBorderColors.Add(EUpgradeRarity::Epic, FLinearColor(0.7f, 0.3f, 0.9f, 1.0f));         // 紫
		RarityBorderColors.Add(EUpgradeRarity::Legendary, FLinearColor(1.0f, 0.8f, 0.0f, 1.0f));    // 金
	}

	// 背景色（レアリティごと）
	if (RarityBackgroundColors.Num() == 0)
	{
		RarityBackgroundColors.Add(EUpgradeRarity::Common, FLinearColor(0.15f, 0.15f, 0.15f, 0.9f));
		RarityBackgroundColors.Add(EUpgradeRarity::Uncommon, FLinearColor(0.1f, 0.2f, 0.1f, 0.9f));
		RarityBackgroundColors.Add(EUpgradeRarity::Rare, FLinearColor(0.1f, 0.15f, 0.25f, 0.9f));
		RarityBackgroundColors.Add(EUpgradeRarity::Epic, FLinearColor(0.2f, 0.1f, 0.25f, 0.9f));
		RarityBackgroundColors.Add(EUpgradeRarity::Legendary, FLinearColor(0.25f, 0.2f, 0.05f, 0.9f));
	}

	// テキスト色（レアリティごと）
	if (RarityTextColors.Num() == 0)
	{
		RarityTextColors.Add(EUpgradeRarity::Common, FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));
		RarityTextColors.Add(EUpgradeRarity::Uncommon, FLinearColor(0.4f, 0.9f, 0.4f, 1.0f));
		RarityTextColors.Add(EUpgradeRarity::Rare, FLinearColor(0.4f, 0.6f, 1.0f, 1.0f));
		RarityTextColors.Add(EUpgradeRarity::Epic, FLinearColor(0.8f, 0.4f, 1.0f, 1.0f));
		RarityTextColors.Add(EUpgradeRarity::Legendary, FLinearColor(1.0f, 0.9f, 0.3f, 1.0f));
	}
}

FText UUpgradeCardWidget::GetRarityDisplayName(EUpgradeRarity Rarity) const
{
	switch (Rarity)
	{
	case EUpgradeRarity::Common:
		return FText::FromString(TEXT("コモン"));
	case EUpgradeRarity::Uncommon:
		return FText::FromString(TEXT("アンコモン"));
	case EUpgradeRarity::Rare:
		return FText::FromString(TEXT("レア"));
	case EUpgradeRarity::Epic:
		return FText::FromString(TEXT("エピック"));
	case EUpgradeRarity::Legendary:
		return FText::FromString(TEXT("レジェンダリー"));
	default:
		return FText::FromString(TEXT("???"));
	}
}

FText UUpgradeCardWidget::GetCategoryDisplayName(EUpgradeCategory Category) const
{
	switch (Category)
	{
	case EUpgradeCategory::Weapon:
		return FText::FromString(TEXT("武器"));
	case EUpgradeCategory::Skill:
		return FText::FromString(TEXT("スキル"));
	case EUpgradeCategory::Passive:
		return FText::FromString(TEXT("パッシブ"));
	case EUpgradeCategory::SoulAffinity:
		return FText::FromString(TEXT("魂親和"));
	case EUpgradeCategory::Visual:
		return FText::FromString(TEXT("ビジュアル"));
	default:
		return FText::FromString(TEXT("???"));
	}
}

FText UUpgradeCardWidget::GetStatTypeName(EStatModifierType StatType) const
{
	switch (StatType)
	{
	case EStatModifierType::AttackDamage:
		return FText::FromString(TEXT("攻撃力"));
	case EStatModifierType::AttackSpeed:
		return FText::FromString(TEXT("攻撃速度"));
	case EStatModifierType::AttackRange:
		return FText::FromString(TEXT("攻撃範囲"));
	case EStatModifierType::CriticalChance:
		return FText::FromString(TEXT("クリティカル率"));
	case EStatModifierType::CriticalDamage:
		return FText::FromString(TEXT("クリティカルダメージ"));
	case EStatModifierType::MaxHealth:
		return FText::FromString(TEXT("最大HP"));
	case EStatModifierType::HealthRegen:
		return FText::FromString(TEXT("HP回復"));
	case EStatModifierType::DamageReduction:
		return FText::FromString(TEXT("ダメージ軽減"));
	case EStatModifierType::DodgeChance:
		return FText::FromString(TEXT("回避率"));
	case EStatModifierType::MoveSpeed:
		return FText::FromString(TEXT("移動速度"));
	case EStatModifierType::DashCooldown:
		return FText::FromString(TEXT("ダッシュCD"));
	case EStatModifierType::DashDistance:
		return FText::FromString(TEXT("ダッシュ距離"));
	case EStatModifierType::SoulGainMultiplier:
		return FText::FromString(TEXT("ソウル獲得量"));
	case EStatModifierType::SoulDuration:
		return FText::FromString(TEXT("ソウル持続"));
	case EStatModifierType::ReaperGaugeRate:
		return FText::FromString(TEXT("リーパーゲージ率"));
	case EStatModifierType::SkillCooldown:
		return FText::FromString(TEXT("スキルCD"));
	case EStatModifierType::SkillDamage:
		return FText::FromString(TEXT("スキルダメージ"));
	case EStatModifierType::SkillRange:
		return FText::FromString(TEXT("スキル範囲"));
	default:
		return FText::FromString(TEXT("???"));
	}
}

void UUpgradeCardWidget::OnSelectButtonClicked()
{
	if (!bIsSelectable || !UpgradeData)
	{
		return;
	}

	// クリックサウンドを再生
	PlayUISound(ClickSound);

	// 選択状態に設定
	SetSelected(true);

	// デリゲートを発火
	OnCardSelected.Broadcast(UpgradeData);

	UE_LOG(LogDawnlight, Log, TEXT("[UpgradeCardWidget] カード選択: %s"),
		*UpgradeData->DisplayName.ToString());
}

void UUpgradeCardWidget::OnSelectButtonHovered()
{
	if (!bIsSelectable)
	{
		return;
	}

	// ホバーサウンドを再生
	PlayUISound(HoverSound);

	// スケールアップアニメーション
	if (CardBorder)
	{
		FWidgetTransform Transform;
		Transform.Scale = FVector2D(HoverScale, HoverScale);
		CardBorder->SetRenderTransform(Transform);
	}
}

void UUpgradeCardWidget::OnSelectButtonUnhovered()
{
	// スケールを元に戻す
	if (CardBorder && !bIsSelected)
	{
		FWidgetTransform Transform;
		Transform.Scale = FVector2D(1.0f, 1.0f);
		CardBorder->SetRenderTransform(Transform);
	}
}
