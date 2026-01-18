// Copyright Epic Games, Inc. All Rights Reserved.

#include "UIAnimationComponent.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Dawnlight.h"

UUIAnimationComponent::UUIAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUIAnimationComponent::PlayFadeIn(UWidget* Widget, float Duration, float Delay)
{
	if (!Widget)
	{
		return;
	}

	// 初期状態を設定
	Widget->SetRenderOpacity(0.0f);
	Widget->SetVisibility(ESlateVisibility::Visible);

	// タイマーで遅延後にアニメーション開始
	if (UWorld* World = Widget->GetWorld())
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;

		TimerDelegate.BindLambda([Widget, Duration]()
		{
			if (!IsValid(Widget))
			{
				return;
			}

			// 簡易的なフェードイン（TickベースでなくTimerベースで実装）
			const int32 Steps = FMath::Max(1, FMath::RoundToInt(Duration * 30.0f)); // 30fps相当
			const float StepDuration = Duration / Steps;
			const float OpacityStep = 1.0f / Steps;

			for (int32 i = 0; i <= Steps; ++i)
			{
				FTimerHandle StepHandle;
				const float TargetOpacity = FMath::Clamp(OpacityStep * i, 0.0f, 1.0f);

				if (UWorld* StepWorld = Widget->GetWorld())
				{
					StepWorld->GetTimerManager().SetTimer(
						StepHandle,
						[Widget, TargetOpacity]()
						{
							if (IsValid(Widget))
							{
								Widget->SetRenderOpacity(TargetOpacity);
							}
						},
						StepDuration * i,
						false
					);
				}
			}
		});

		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay > 0.0f ? Delay : 0.001f, false);
	}
}

void UUIAnimationComponent::PlayFadeOut(UWidget* Widget, float Duration, float Delay)
{
	if (!Widget)
	{
		return;
	}

	if (UWorld* World = Widget->GetWorld())
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;

		TimerDelegate.BindLambda([Widget, Duration]()
		{
			if (!IsValid(Widget))
			{
				return;
			}

			const int32 Steps = FMath::Max(1, FMath::RoundToInt(Duration * 30.0f));
			const float StepDuration = Duration / Steps;
			const float OpacityStep = 1.0f / Steps;

			for (int32 i = 0; i <= Steps; ++i)
			{
				FTimerHandle StepHandle;
				const float TargetOpacity = FMath::Clamp(1.0f - (OpacityStep * i), 0.0f, 1.0f);

				if (UWorld* StepWorld = Widget->GetWorld())
				{
					StepWorld->GetTimerManager().SetTimer(
						StepHandle,
						[Widget, TargetOpacity, i, Steps]()
						{
							if (IsValid(Widget))
							{
								Widget->SetRenderOpacity(TargetOpacity);

								// 最後のステップで非表示に
								if (i == Steps)
								{
									Widget->SetVisibility(ESlateVisibility::Collapsed);
								}
							}
						},
						StepDuration * i,
						false
					);
				}
			}
		});

		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay > 0.0f ? Delay : 0.001f, false);
	}
}

void UUIAnimationComponent::PlaySlideIn(UWidget* Widget, EUIAnimationType Direction, float Distance, float Duration, float Delay)
{
	if (!Widget)
	{
		return;
	}

	// 開始位置を計算
	FVector2D StartOffset = FVector2D::ZeroVector;
	switch (Direction)
	{
	case EUIAnimationType::SlideInLeft:
		StartOffset = FVector2D(-Distance, 0.0f);
		break;
	case EUIAnimationType::SlideInRight:
		StartOffset = FVector2D(Distance, 0.0f);
		break;
	case EUIAnimationType::SlideInTop:
		StartOffset = FVector2D(0.0f, -Distance);
		break;
	case EUIAnimationType::SlideInBottom:
		StartOffset = FVector2D(0.0f, Distance);
		break;
	default:
		break;
	}

	// 初期状態
	Widget->SetRenderTranslation(StartOffset);
	Widget->SetRenderOpacity(0.0f);
	Widget->SetVisibility(ESlateVisibility::Visible);

	if (UWorld* World = Widget->GetWorld())
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;

		TimerDelegate.BindLambda([Widget, StartOffset, Duration]()
		{
			if (!IsValid(Widget))
			{
				return;
			}

			const int32 Steps = FMath::Max(1, FMath::RoundToInt(Duration * 30.0f));
			const float StepDuration = Duration / Steps;

			for (int32 i = 0; i <= Steps; ++i)
			{
				FTimerHandle StepHandle;
				const float Alpha = static_cast<float>(i) / Steps;
				// EaseOutCubic
				const float EasedAlpha = 1.0f - FMath::Pow(1.0f - Alpha, 3.0f);

				if (UWorld* StepWorld = Widget->GetWorld())
				{
					StepWorld->GetTimerManager().SetTimer(
						StepHandle,
						[Widget, StartOffset, EasedAlpha]()
						{
							if (IsValid(Widget))
							{
								Widget->SetRenderTranslation(FMath::Lerp(StartOffset, FVector2D::ZeroVector, EasedAlpha));
								Widget->SetRenderOpacity(EasedAlpha);
							}
						},
						StepDuration * i,
						false
					);
				}
			}
		});

		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay > 0.0f ? Delay : 0.001f, false);
	}
}

void UUIAnimationComponent::PlayScaleAnimation(UWidget* Widget, float StartScale, float EndScale, float Duration, float Delay)
{
	if (!Widget)
	{
		return;
	}

	Widget->SetRenderScale(FVector2D(StartScale, StartScale));

	if (UWorld* World = Widget->GetWorld())
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;

		TimerDelegate.BindLambda([Widget, StartScale, EndScale, Duration]()
		{
			if (!IsValid(Widget))
			{
				return;
			}

			const int32 Steps = FMath::Max(1, FMath::RoundToInt(Duration * 30.0f));
			const float StepDuration = Duration / Steps;

			for (int32 i = 0; i <= Steps; ++i)
			{
				FTimerHandle StepHandle;
				const float Alpha = static_cast<float>(i) / Steps;
				// EaseOutBack for bounce effect
				const float EasedAlpha = 1.0f + 2.70158f * FMath::Pow(Alpha - 1.0f, 3.0f) + 1.70158f * FMath::Pow(Alpha - 1.0f, 2.0f);

				if (UWorld* StepWorld = Widget->GetWorld())
				{
					StepWorld->GetTimerManager().SetTimer(
						StepHandle,
						[Widget, StartScale, EndScale, EasedAlpha]()
						{
							if (IsValid(Widget))
							{
								const float CurrentScale = FMath::Lerp(StartScale, EndScale, FMath::Clamp(EasedAlpha, 0.0f, 1.0f));
								Widget->SetRenderScale(FVector2D(CurrentScale, CurrentScale));
							}
						},
						StepDuration * i,
						false
					);
				}
			}
		});

		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay > 0.0f ? Delay : 0.001f, false);
	}
}

void UUIAnimationComponent::PlayButtonHover(UWidget* Widget, bool bIsHovered, float Scale, float Duration)
{
	if (!Widget)
	{
		return;
	}

	const float TargetScale = bIsHovered ? Scale : 1.0f;
	const FVector2D CurrentScale = Widget->GetRenderTransform().Scale;
	const float StartScale = CurrentScale.X;

	if (UWorld* World = Widget->GetWorld())
	{
		const int32 Steps = FMath::Max(1, FMath::RoundToInt(Duration * 60.0f)); // 60fps for smooth hover
		const float StepDuration = Duration / Steps;

		for (int32 i = 0; i <= Steps; ++i)
		{
			FTimerHandle StepHandle;
			const float Alpha = static_cast<float>(i) / Steps;
			// EaseOutQuad
			const float EasedAlpha = 1.0f - (1.0f - Alpha) * (1.0f - Alpha);

			World->GetTimerManager().SetTimer(
				StepHandle,
				[Widget, StartScale, TargetScale, EasedAlpha]()
				{
					if (IsValid(Widget))
					{
						const float CurrentScaleValue = FMath::Lerp(StartScale, TargetScale, EasedAlpha);
						Widget->SetRenderScale(FVector2D(CurrentScaleValue, CurrentScaleValue));
					}
				},
				StepDuration * i,
				false
			);
		}
	}
}

void UUIAnimationComponent::PlayButtonPress(UWidget* Widget, float Scale, float Duration)
{
	if (!Widget)
	{
		return;
	}

	// 押下 -> 元に戻る の2段階アニメーション
	PlayScaleAnimation(Widget, 1.0f, Scale, Duration * 0.5f, 0.0f);

	if (UWorld* World = Widget->GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(
			TimerHandle,
			[Widget, Duration]()
			{
				if (IsValid(Widget))
				{
					PlayScaleAnimation(Widget, Widget->GetRenderTransform().Scale.X, 1.0f, Duration * 0.5f, 0.0f);
				}
			},
			Duration * 0.5f,
			false
		);
	}
}

void UUIAnimationComponent::PlayStaggeredAnimation(const TArray<UWidget*>& Widgets, EUIAnimationType AnimationType, float StaggerDelay, float Duration)
{
	for (int32 i = 0; i < Widgets.Num(); ++i)
	{
		UWidget* Widget = Widgets[i];
		if (!Widget)
		{
			continue;
		}

		const float Delay = StaggerDelay * i;

		switch (AnimationType)
		{
		case EUIAnimationType::FadeIn:
			PlayFadeIn(Widget, Duration, Delay);
			break;
		case EUIAnimationType::SlideInLeft:
		case EUIAnimationType::SlideInRight:
		case EUIAnimationType::SlideInTop:
		case EUIAnimationType::SlideInBottom:
			PlaySlideIn(Widget, AnimationType, 100.0f, Duration, Delay);
			break;
		case EUIAnimationType::ScaleIn:
			Widget->SetRenderOpacity(0.0f);
			Widget->SetVisibility(ESlateVisibility::Visible);
			if (UWorld* World = Widget->GetWorld())
			{
				FTimerHandle TimerHandle;
				World->GetTimerManager().SetTimer(
					TimerHandle,
					[Widget, Duration]()
					{
						if (IsValid(Widget))
						{
							Widget->SetRenderOpacity(1.0f);
							PlayScaleAnimation(Widget, 0.5f, 1.0f, Duration, 0.0f);
						}
					},
					Delay,
					false
				);
			}
			break;
		default:
			break;
		}
	}
}

void UUIAnimationComponent::PlayPulse(UWidget* Widget, float MinScale, float MaxScale, float Duration, bool bLoop)
{
	if (!Widget)
	{
		return;
	}

	if (UWorld* World = Widget->GetWorld())
	{
		const int32 Steps = FMath::Max(1, FMath::RoundToInt(Duration * 30.0f));
		const float StepDuration = Duration / Steps;

		for (int32 i = 0; i <= Steps; ++i)
		{
			FTimerHandle StepHandle;
			const float Alpha = static_cast<float>(i) / Steps;
			// Sin wave for pulse
			const float PulseAlpha = (FMath::Sin(Alpha * PI * 2.0f - PI * 0.5f) + 1.0f) * 0.5f;

			World->GetTimerManager().SetTimer(
				StepHandle,
				[Widget, MinScale, MaxScale, PulseAlpha, bLoop, Duration, i, Steps]()
				{
					if (IsValid(Widget))
					{
						const float CurrentScale = FMath::Lerp(MinScale, MaxScale, PulseAlpha);
						Widget->SetRenderScale(FVector2D(CurrentScale, CurrentScale));

						// ループの場合、最後のステップで再度呼び出し
						if (bLoop && i == Steps)
						{
							PlayPulse(Widget, MinScale, MaxScale, Duration, true);
						}
					}
				},
				StepDuration * i,
				false
			);
		}
	}
}

void UUIAnimationComponent::PlayShake(UWidget* Widget, float Intensity, float Duration)
{
	if (!Widget)
	{
		return;
	}

	const FVector2D OriginalTranslation = Widget->GetRenderTransform().Translation;

	if (UWorld* World = Widget->GetWorld())
	{
		const int32 Steps = FMath::Max(1, FMath::RoundToInt(Duration * 60.0f));
		const float StepDuration = Duration / Steps;

		for (int32 i = 0; i <= Steps; ++i)
		{
			FTimerHandle StepHandle;
			const float Alpha = static_cast<float>(i) / Steps;
			// 減衰するシェイク
			const float DecayedIntensity = Intensity * (1.0f - Alpha);

			World->GetTimerManager().SetTimer(
				StepHandle,
				[Widget, OriginalTranslation, DecayedIntensity, i, Steps]()
				{
					if (IsValid(Widget))
					{
						if (i == Steps)
						{
							// 最後は元の位置に戻す
							Widget->SetRenderTranslation(OriginalTranslation);
						}
						else
						{
							// ランダムなオフセットを適用
							const FVector2D ShakeOffset(
								FMath::RandRange(-DecayedIntensity, DecayedIntensity),
								FMath::RandRange(-DecayedIntensity, DecayedIntensity)
							);
							Widget->SetRenderTranslation(OriginalTranslation + ShakeOffset);
						}
					}
				},
				StepDuration * i,
				false
			);
		}
	}
}

void UUIAnimationComponent::PlayGlow(UWidget* Widget, FLinearColor GlowColor, float Duration)
{
	// グロー効果は通常、マテリアルやポストプロセスで実装
	// ここでは色のアニメーションとして実装
	UE_LOG(LogDawnlight, Log, TEXT("[UIAnimation] PlayGlow called - implement in Blueprint with Material"));
}

void UUIAnimationComponent::StopAllAnimations(UWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	// タイマーベースのアニメーションは個別のタイマーハンドルを管理する必要がある
	// 現在の実装では、ウィジェットをリセット
	Widget->SetRenderOpacity(1.0f);
	Widget->SetRenderScale(FVector2D(1.0f, 1.0f));
	Widget->SetRenderTranslation(FVector2D::ZeroVector);
}

bool UUIAnimationComponent::IsAnimating(UWidget* Widget)
{
	// 現在の簡易実装ではアニメーション状態を追跡していない
	// より高度な実装では、アクティブなタイマーを追跡する必要がある
	return false;
}
