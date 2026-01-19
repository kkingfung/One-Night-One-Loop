// Copyright Epic Games, Inc. All Rights Reserved.

#include "UIAnimationLibrary.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"

// ========================================================================
// イージング関数
// ========================================================================

float UUIAnimationLibrary::CalculateEase(float Alpha, EUIEaseType EaseType)
{
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

	switch (EaseType)
	{
	case EUIEaseType::Linear:
		return Alpha;

	case EUIEaseType::EaseIn:
		return Alpha * Alpha;

	case EUIEaseType::EaseOut:
		return 1.0f - FMath::Pow(1.0f - Alpha, 2.0f);

	case EUIEaseType::EaseInOut:
		return Alpha < 0.5f
			? 2.0f * Alpha * Alpha
			: 1.0f - FMath::Pow(-2.0f * Alpha + 2.0f, 2.0f) * 0.5f;

	case EUIEaseType::Bounce:
		return EaseBounce(Alpha);

	case EUIEaseType::Elastic:
		return EaseElastic(Alpha);

	case EUIEaseType::Back:
		return EaseBack(Alpha);

	default:
		return Alpha;
	}
}

float UUIAnimationLibrary::EaseBounce(float Alpha)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (Alpha < 1.0f / d1)
	{
		return n1 * Alpha * Alpha;
	}
	else if (Alpha < 2.0f / d1)
	{
		Alpha -= 1.5f / d1;
		return n1 * Alpha * Alpha + 0.75f;
	}
	else if (Alpha < 2.5f / d1)
	{
		Alpha -= 2.25f / d1;
		return n1 * Alpha * Alpha + 0.9375f;
	}
	else
	{
		Alpha -= 2.625f / d1;
		return n1 * Alpha * Alpha + 0.984375f;
	}
}

float UUIAnimationLibrary::EaseElastic(float Alpha)
{
	if (Alpha == 0.0f || Alpha == 1.0f)
	{
		return Alpha;
	}

	const float c4 = (2.0f * PI) / 3.0f;
	return FMath::Pow(2.0f, -10.0f * Alpha) * FMath::Sin((Alpha * 10.0f - 0.75f) * c4) + 1.0f;
}

float UUIAnimationLibrary::EaseBack(float Alpha)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * FMath::Pow(Alpha - 1.0f, 3.0f) + c1 * FMath::Pow(Alpha - 1.0f, 2.0f);
}

// ========================================================================
// スケールアニメーション
// ========================================================================

void UUIAnimationLibrary::SetWidgetScale(UWidget* Widget, FVector2D Scale)
{
	if (!Widget)
	{
		return;
	}

	Widget->SetRenderScale(Scale);
}

void UUIAnimationLibrary::AnimatePopIn(UWidget* Widget, float Duration)
{
	if (!Widget)
	{
		return;
	}

	// 初期状態を設定（実際のアニメーションはTimerやTweenライブラリで実行）
	// ここでは即時に最終状態を設定
	Widget->SetRenderScale(FVector2D(1.0f, 1.0f));
	Widget->SetRenderOpacity(1.0f);

	// TODO: TimerManagerを使ったアニメーション実装
	// または、WidgetAnimationをBPで作成して使用
}

void UUIAnimationLibrary::AnimatePopOut(UWidget* Widget, float Duration)
{
	if (!Widget)
	{
		return;
	}

	Widget->SetRenderScale(FVector2D(0.0f, 0.0f));
	Widget->SetRenderOpacity(0.0f);
}

void UUIAnimationLibrary::AnimateHoverScale(UWidget* Widget, bool bIsHovered, float Scale, float Duration)
{
	if (!Widget)
	{
		return;
	}

	FVector2D TargetScale = bIsHovered ? FVector2D(Scale, Scale) : FVector2D(1.0f, 1.0f);
	Widget->SetRenderScale(TargetScale);
}

void UUIAnimationLibrary::AnimatePressScale(UWidget* Widget, bool bIsPressed, float Scale, float Duration)
{
	if (!Widget)
	{
		return;
	}

	FVector2D TargetScale = bIsPressed ? FVector2D(Scale, Scale) : FVector2D(1.0f, 1.0f);
	Widget->SetRenderScale(TargetScale);
}

// ========================================================================
// シェイク/バウンス効果
// ========================================================================

void UUIAnimationLibrary::AnimateShake(UWidget* Widget, float Intensity, float Duration)
{
	if (!Widget)
	{
		return;
	}

	// シェイク効果はTickベースで実装が必要
	// ここでは初期位置に戻す
	Widget->SetRenderTranslation(FVector2D::ZeroVector);
}

void UUIAnimationLibrary::AnimateBounce(UWidget* Widget, float Intensity, float Duration)
{
	if (!Widget)
	{
		return;
	}

	// バウンス効果はTickベースで実装が必要
	Widget->SetRenderTranslation(FVector2D::ZeroVector);
}

// ========================================================================
// カラー効果
// ========================================================================

FLinearColor UUIAnimationLibrary::GetSoulPurple()
{
	return FLinearColor(0.5f, 0.2f, 0.7f, 1.0f);
}

FLinearColor UUIAnimationLibrary::GetSoulGold()
{
	return FLinearColor(1.0f, 0.85f, 0.0f, 1.0f);
}

FLinearColor UUIAnimationLibrary::GetSoulDark()
{
	return FLinearColor(0.05f, 0.02f, 0.1f, 1.0f);
}

FLinearColor UUIAnimationLibrary::GetRarityColor(int32 RarityLevel)
{
	switch (RarityLevel)
	{
	case 0: // Common
		return FLinearColor(0.7f, 0.7f, 0.7f, 1.0f);  // グレー

	case 1: // Uncommon
		return FLinearColor(0.2f, 0.8f, 0.2f, 1.0f);  // 緑

	case 2: // Rare
		return FLinearColor(0.2f, 0.5f, 1.0f, 1.0f);  // 青

	case 3: // Epic
		return FLinearColor(0.6f, 0.2f, 0.9f, 1.0f);  // 紫

	case 4: // Legendary
		return FLinearColor(1.0f, 0.7f, 0.0f, 1.0f);  // 金

	default:
		return FLinearColor::White;
	}
}

FLinearColor UUIAnimationLibrary::PulseColor(FLinearColor BaseColor, FLinearColor TargetColor, float Time, float Speed)
{
	float Alpha = (FMath::Sin(Time * Speed) + 1.0f) * 0.5f;
	return FMath::Lerp(BaseColor, TargetColor, Alpha);
}

FLinearColor UUIAnimationLibrary::LerpGradient(const TArray<FLinearColor>& Colors, float Alpha)
{
	if (Colors.Num() == 0)
	{
		return FLinearColor::White;
	}

	if (Colors.Num() == 1)
	{
		return Colors[0];
	}

	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

	// インデックスと補間値を計算
	float ScaledAlpha = Alpha * (Colors.Num() - 1);
	int32 LowerIndex = FMath::FloorToInt(ScaledAlpha);
	int32 UpperIndex = FMath::Min(LowerIndex + 1, Colors.Num() - 1);
	float LocalAlpha = ScaledAlpha - LowerIndex;

	return FMath::Lerp(Colors[LowerIndex], Colors[UpperIndex], LocalAlpha);
}

// ========================================================================
// ユーティリティ
// ========================================================================

float UUIAnimationLibrary::GetRandomAngle(float MaxAngle)
{
	return FMath::FRandRange(-MaxAngle, MaxAngle);
}

float UUIAnimationLibrary::GetPulseValue(float Time, float Speed)
{
	return (FMath::Sin(Time * Speed * PI * 2.0f) + 1.0f) * 0.5f;
}

float UUIAnimationLibrary::PingPong(float Time, float Length)
{
	float Mod = FMath::Fmod(Time, Length * 2.0f);
	if (Mod <= Length)
	{
		return Mod / Length;
	}
	else
	{
		return 1.0f - (Mod - Length) / Length;
	}
}
