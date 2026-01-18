// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "UI/Data/UIStyleDataAsset.h"
#include "Dawnlight.h"

UDawnlightWidgetBase::UDawnlightWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsShowing(false)
	, bIsAnimating(false)
{
}

void UDawnlightWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] NativeConstruct"), *GetName());
}

void UDawnlightWidgetBase::NativeDestruct()
{
	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] NativeDestruct"), *GetName());

	Super::NativeDestruct();
}

FReply UDawnlightWidgetBase::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// Escキーでの戻る処理はサブクラスで実装
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UDawnlightWidgetBase::Show()
{
	if (bIsAnimating)
	{
		return;
	}

	bIsShowing = true;
	SetVisibility(ESlateVisibility::Visible);

	if (bUseAnimation)
	{
		bIsAnimating = true;
		BP_PlayShowAnimation();
	}
	else
	{
		OnShowAnimationFinished();
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] Show"), *GetName());
}

void UDawnlightWidgetBase::Hide()
{
	if (bIsAnimating)
	{
		return;
	}

	if (bUseAnimation)
	{
		bIsAnimating = true;
		BP_PlayHideAnimation();
	}
	else
	{
		OnHideAnimationFinished();
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] Hide"), *GetName());
}

void UDawnlightWidgetBase::ShowImmediate()
{
	bIsShowing = true;
	bIsAnimating = false;
	SetVisibility(ESlateVisibility::Visible);
	BP_OnShowCompleted();
	BP_SetInitialFocus();

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] ShowImmediate"), *GetName());
}

void UDawnlightWidgetBase::HideImmediate()
{
	bIsShowing = false;
	bIsAnimating = false;
	SetVisibility(ESlateVisibility::Collapsed);
	BP_OnHideCompleted();

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] HideImmediate"), *GetName());
}

void UDawnlightWidgetBase::OnShowAnimationFinished()
{
	bIsAnimating = false;
	BP_OnShowCompleted();
	BP_SetInitialFocus();

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] Show animation finished"), *GetName());
}

void UDawnlightWidgetBase::OnHideAnimationFinished()
{
	bIsAnimating = false;
	bIsShowing = false;
	SetVisibility(ESlateVisibility::Collapsed);
	BP_OnHideCompleted();

	UE_LOG(LogDawnlight, Verbose, TEXT("[%s] Hide animation finished"), *GetName());
}

void UDawnlightWidgetBase::PlayUISound(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

// ========================================================================
// ジューシー機能
// ========================================================================

void UDawnlightWidgetBase::ApplyButtonHoverAnimation(UButton* Button)
{
	if (!Button)
	{
		return;
	}

	// ボタンをリストに追加
	AnimatedButtons.AddUnique(Button);

	// ホバーイベントをバインド
	Button->OnHovered.AddDynamic(this, &UDawnlightWidgetBase::OnButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &UDawnlightWidgetBase::OnButtonUnhovered);
}

void UDawnlightWidgetBase::PlayMenuItemsStaggerAnimation(const TArray<UWidget*>& MenuItems, EUIAnimationType AnimationType)
{
	float StaggerDelay = 0.08f;
	float Duration = DefaultAnimationDuration;

	if (StyleDataAsset)
	{
		StaggerDelay = StyleDataAsset->AnimationSettings.StaggerDelay;
		Duration = StyleDataAsset->AnimationSettings.FadeInDuration;
	}

	UUIAnimationComponent::PlayStaggeredAnimation(MenuItems, AnimationType, StaggerDelay, Duration);
}

void UDawnlightWidgetBase::PlayWidgetFadeIn(UWidget* Widget, float Duration, float Delay)
{
	UUIAnimationComponent::PlayFadeIn(Widget, Duration, Delay);
}

void UDawnlightWidgetBase::PlayWidgetFadeOut(UWidget* Widget, float Duration, float Delay)
{
	UUIAnimationComponent::PlayFadeOut(Widget, Duration, Delay);
}

void UDawnlightWidgetBase::PlayWidgetSlideIn(UWidget* Widget, EUIAnimationType Direction, float Distance, float Duration, float Delay)
{
	UUIAnimationComponent::PlaySlideIn(Widget, Direction, Distance, Duration, Delay);
}

void UDawnlightWidgetBase::PlayErrorShake(UWidget* Widget)
{
	UUIAnimationComponent::PlayShake(Widget, 10.0f, 0.3f);

	// エラーサウンドを再生
	if (StyleDataAsset && StyleDataAsset->ErrorSound)
	{
		PlayUISound(StyleDataAsset->ErrorSound);
	}
}

void UDawnlightWidgetBase::PlayAttentionPulse(UWidget* Widget, bool bLoop)
{
	UUIAnimationComponent::PlayPulse(Widget, 0.95f, 1.05f, 0.5f, bLoop);
}

void UDawnlightWidgetBase::StopWidgetAnimation(UWidget* Widget)
{
	UUIAnimationComponent::StopAllAnimations(Widget);
}

void UDawnlightWidgetBase::OnButtonHovered()
{
	// 現在ホバーされているボタンを探す
	for (UButton* Button : AnimatedButtons)
	{
		if (Button && Button->IsHovered())
		{
			float Scale = 1.05f;
			float Duration = 0.15f;

			if (StyleDataAsset)
			{
				Scale = StyleDataAsset->AnimationSettings.HoveredScale;
				Duration = StyleDataAsset->AnimationSettings.HoverTransitionDuration;
			}

			UUIAnimationComponent::PlayButtonHover(Button, true, Scale, Duration);

			// ホバーサウンド
			PlayUISound(HoverSound);
			break;
		}
	}
}

void UDawnlightWidgetBase::OnButtonUnhovered()
{
	// アンホバーされたボタンを探す
	for (UButton* Button : AnimatedButtons)
	{
		if (Button && !Button->IsHovered())
		{
			float Duration = 0.15f;

			if (StyleDataAsset)
			{
				Duration = StyleDataAsset->AnimationSettings.HoverTransitionDuration;
			}

			UUIAnimationComponent::PlayButtonHover(Button, false, 1.0f, Duration);
		}
	}
}
