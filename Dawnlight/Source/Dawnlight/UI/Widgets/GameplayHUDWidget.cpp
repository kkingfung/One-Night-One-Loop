// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#include "GameplayHUDWidget.h"
#include "Dawnlight.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "UI/ViewModels/GameplayHUDViewModel.h"

void UGameplayHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Soul Reaperテーマの色を設定
	ReaperGaugeNormalColor = FLinearColor(0.4f, 0.2f, 0.6f, 1.0f);  // 紫
	ReaperGaugeHighColor = FLinearColor(0.9f, 0.5f, 0.1f, 1.0f);    // オレンジ
	ReaperGaugeMaxColor = FLinearColor(1.0f, 0.85f, 0.0f, 1.0f);    // 金

	// 初期状態を設定
	if (ReaperReadyWarningPanel)
	{
		ReaperReadyWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (DawnWarningPanel)
	{
		DawnWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ReaperModeIndicator)
	{
		ReaperModeIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (BuffIndicator)
	{
		BuffIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}

	// フェーズパネル初期状態
	if (NightPhasePanel)
	{
		NightPhasePanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (DawnPhasePanel)
	{
		DawnPhasePanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDWidget] Soul Reaper HUD初期化完了"));
}

void UGameplayHUDWidget::NativeDestruct()
{
	// ViewModelからアンバインド
	UnbindFromViewModel();

	// タイマーをクリア
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaveAnnouncementTimerHandle);
	}

	Super::NativeDestruct();
}

void UGameplayHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ViewModelがある場合は、時間系のみ毎フレーム同期
	// （他のプロパティはイベント駆動で更新）
	if (ViewModel && ViewModel->IsInitialized())
	{
		// Night Phase中は残り時間を更新（毎フレーム必要）
		if (ViewModel->CurrentPhase == EGamePhase::Night)
		{
			UpdateRemainingTime(ViewModel->NightTimeRemaining);

			// 動物数も更新
			UpdateAnimalCount(ViewModel->AliveAnimalCount, ViewModel->TotalAnimalCount);
		}

		// Dawn Phase中はWave情報を更新
		if (ViewModel->CurrentPhase == EGamePhase::Dawn)
		{
			UpdateWaveInfo(
				ViewModel->CurrentWaveNumber,
				ViewModel->TotalWaveCount,
				ViewModel->RemainingEnemies
			);
		}
	}
}

void UGameplayHUDWidget::SetViewModel(UGameplayHUDViewModel* InViewModel)
{
	// 既存のViewModelからアンバインド
	UnbindFromViewModel();

	ViewModel = InViewModel;

	// 新しいViewModelにバインド
	if (ViewModel)
	{
		BindToViewModel();
		RefreshFromViewModel();
	}

	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDWidget] ViewModel設定: %s"),
		ViewModel ? *ViewModel->GetName() : TEXT("null"));
}

void UGameplayHUDWidget::BindToViewModel()
{
	if (!ViewModel)
	{
		return;
	}

	// プロパティ変更イベントにバインド
	ViewModel->OnViewModelPropertyChanged.AddDynamic(this, &UGameplayHUDWidget::HandlePropertyChanged);
	ViewModel->OnAllPropertiesChanged.AddDynamic(this, &UGameplayHUDWidget::HandleAllPropertiesChanged);

	// 特定イベントにバインド
	ViewModel->OnWaveStartedEvent.AddDynamic(this, &UGameplayHUDWidget::HandleWaveStarted);
	ViewModel->OnReaperModeChanged.AddDynamic(this, &UGameplayHUDWidget::HandleReaperModeChanged);

	UE_LOG(LogDawnlight, Verbose, TEXT("[GameplayHUDWidget] ViewModelにバインド完了"));
}

void UGameplayHUDWidget::UnbindFromViewModel()
{
	if (!ViewModel)
	{
		return;
	}

	// プロパティ変更イベントからアンバインド
	ViewModel->OnViewModelPropertyChanged.RemoveDynamic(this, &UGameplayHUDWidget::HandlePropertyChanged);
	ViewModel->OnAllPropertiesChanged.RemoveDynamic(this, &UGameplayHUDWidget::HandleAllPropertiesChanged);

	// 特定イベントからアンバインド
	ViewModel->OnWaveStartedEvent.RemoveDynamic(this, &UGameplayHUDWidget::HandleWaveStarted);
	ViewModel->OnReaperModeChanged.RemoveDynamic(this, &UGameplayHUDWidget::HandleReaperModeChanged);

	UE_LOG(LogDawnlight, Verbose, TEXT("[GameplayHUDWidget] ViewModelからアンバインド完了"));
}

void UGameplayHUDWidget::RefreshFromViewModel()
{
	if (!ViewModel)
	{
		return;
	}

	// 全プロパティをUIに反映
	UpdatePhaseDisplay(ViewModel->CurrentPhase);
	UpdatePhasePanels(ViewModel->CurrentPhase);
	UpdateRemainingTime(ViewModel->NightTimeRemaining);
	UpdateSoulCount(ViewModel->TotalSoulCount);
	UpdateReaperGauge(ViewModel->ReaperGaugePercent);
	UpdateAnimalCount(ViewModel->AliveAnimalCount, ViewModel->TotalAnimalCount);
	UpdateWaveInfo(ViewModel->CurrentWaveNumber, ViewModel->TotalWaveCount, ViewModel->RemainingEnemies);
	UpdatePlayerHealth(ViewModel->PlayerCurrentHP, ViewModel->PlayerMaxHP);

	// 警告状態
	if (ViewModel->bIsReaperModeReady)
	{
		ShowReaperReadyWarning();
	}
	else
	{
		HideReaperReadyWarning();
	}

	if (ViewModel->bShouldShowDawnWarning)
	{
		ShowDawnWarning();
	}

	// リーパーモード
	ShowReaperModeIndicator(ViewModel->bIsReaperModeActive);

	// バフ
	if (ViewModel->DamageBuffPercent > 0.0f)
	{
		ShowDamageBuffIndicator(ViewModel->DamageBuffPercent);
	}

	UE_LOG(LogDawnlight, Verbose, TEXT("[GameplayHUDWidget] ViewModelから全UI更新完了"));
}

void UGameplayHUDWidget::HandlePropertyChanged(FName PropertyName)
{
	if (!ViewModel)
	{
		return;
	}

	// プロパティ名に応じてUIを更新
	if (PropertyName == UGameplayHUDViewModel::PROP_CurrentPhase ||
		PropertyName == UGameplayHUDViewModel::PROP_PhaseName)
	{
		UpdatePhaseDisplay(ViewModel->CurrentPhase);
		UpdatePhasePanels(ViewModel->CurrentPhase);
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_NightTimeRemaining ||
			 PropertyName == UGameplayHUDViewModel::PROP_FormattedTimeRemaining)
	{
		UpdateRemainingTime(ViewModel->NightTimeRemaining);
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_TotalSoulCount)
	{
		UpdateSoulCount(ViewModel->TotalSoulCount);
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_ReaperGaugePercent)
	{
		UpdateReaperGauge(ViewModel->ReaperGaugePercent);
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_IsReaperModeReady)
	{
		if (ViewModel->bIsReaperModeReady)
		{
			ShowReaperReadyWarning();
		}
		else
		{
			HideReaperReadyWarning();
		}
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_IsReaperModeActive)
	{
		ShowReaperModeIndicator(ViewModel->bIsReaperModeActive);
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_CurrentWaveNumber ||
			 PropertyName == UGameplayHUDViewModel::PROP_TotalWaveCount ||
			 PropertyName == UGameplayHUDViewModel::PROP_RemainingEnemies)
	{
		UpdateWaveInfo(ViewModel->CurrentWaveNumber, ViewModel->TotalWaveCount, ViewModel->RemainingEnemies);
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_AliveAnimalCount ||
			 PropertyName == UGameplayHUDViewModel::PROP_TotalAnimalCount)
	{
		UpdateAnimalCount(ViewModel->AliveAnimalCount, ViewModel->TotalAnimalCount);
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_PlayerCurrentHP ||
			 PropertyName == UGameplayHUDViewModel::PROP_PlayerMaxHP ||
			 PropertyName == UGameplayHUDViewModel::PROP_PlayerHPPercent)
	{
		UpdatePlayerHealth(ViewModel->PlayerCurrentHP, ViewModel->PlayerMaxHP);
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_DamageBuffPercent)
	{
		if (ViewModel->DamageBuffPercent > 0.0f)
		{
			ShowDamageBuffIndicator(ViewModel->DamageBuffPercent);
		}
	}
	else if (PropertyName == UGameplayHUDViewModel::PROP_ShouldShowDawnWarning)
	{
		if (ViewModel->bShouldShowDawnWarning)
		{
			ShowDawnWarning();
		}
	}
}

void UGameplayHUDWidget::HandleAllPropertiesChanged()
{
	RefreshFromViewModel();
}

void UGameplayHUDWidget::HandleWaveStarted(int32 WaveNumber)
{
	ShowWaveStartWarning(WaveNumber);
}

void UGameplayHUDWidget::HandleReaperModeChanged(bool bIsActive)
{
	ShowReaperModeIndicator(bIsActive);
}

void UGameplayHUDWidget::UpdateReaperGauge(float NormalizedValue)
{
	if (!ReaperGauge)
	{
		return;
	}

	// ゲージ値を設定
	ReaperGauge->SetPercent(FMath::Clamp(NormalizedValue, 0.0f, 1.0f));

	// 色を更新
	UpdateReaperGaugeColor(NormalizedValue);

	// MAXに達したら警告表示
	if (NormalizedValue >= MaxThreshold)
	{
		ShowReaperReadyWarning();
	}
	else
	{
		HideReaperReadyWarning();
	}
}

void UGameplayHUDWidget::UpdateReaperGaugeColor(float NormalizedValue)
{
	if (!ReaperGauge)
	{
		return;
	}

	FLinearColor GaugeColor;

	if (NormalizedValue >= MaxThreshold)
	{
		GaugeColor = ReaperGaugeMaxColor;
	}
	else if (NormalizedValue >= ChargeThreshold)
	{
		// チャージ中とMAXの間を補間
		float Alpha = (NormalizedValue - ChargeThreshold) / (MaxThreshold - ChargeThreshold);
		GaugeColor = FMath::Lerp(ReaperGaugeHighColor, ReaperGaugeMaxColor, Alpha);
	}
	else
	{
		// 通常とチャージ中の間を補間
		float Alpha = NormalizedValue / ChargeThreshold;
		GaugeColor = FMath::Lerp(ReaperGaugeNormalColor, ReaperGaugeHighColor, Alpha);
	}

	ReaperGauge->SetFillColorAndOpacity(GaugeColor);
}

void UGameplayHUDWidget::UpdateRemainingTime(float RemainingSeconds)
{
	if (!RemainingTimeText)
	{
		return;
	}

	RemainingTimeText->SetText(FormatTime(RemainingSeconds));

	// 残り時間が少なくなったら夜明け警告
	if (RemainingSeconds <= 30.0f && RemainingSeconds > 0.0f)
	{
		ShowDawnWarning();
	}
}

FText UGameplayHUDWidget::FormatTime(float Seconds) const
{
	int32 Minutes = FMath::FloorToInt(Seconds / 60.0f);
	int32 Secs = FMath::FloorToInt(FMath::Fmod(Seconds, 60.0f));

	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Secs));
}

void UGameplayHUDWidget::UpdateSoulCount(int32 TotalSouls)
{
	if (!SoulCountText)
	{
		return;
	}

	FString CountString = FString::Printf(TEXT("x %d"), TotalSouls);
	SoulCountText->SetText(FText::FromString(CountString));

	// 魂アイコンの色を変更（多いほど輝く）
	if (SoulIcon)
	{
		float Intensity = FMath::Clamp(TotalSouls / 50.0f, 0.0f, 1.0f);
		FLinearColor IconColor = FMath::Lerp(
			FLinearColor(0.6f, 0.4f, 0.8f, 1.0f),  // 薄紫
			FLinearColor(1.0f, 0.85f, 0.0f, 1.0f), // 金
			Intensity
		);
		SoulIcon->SetColorAndOpacity(IconColor);
	}
}

void UGameplayHUDWidget::UpdatePhaseDisplay(EGamePhase Phase)
{
	if (!PhaseText)
	{
		return;
	}

	PhaseText->SetText(GetPhaseName(Phase));
}

FText UGameplayHUDWidget::GetPhaseName(EGamePhase Phase) const
{
	switch (Phase)
	{
	case EGamePhase::Night:
		return FText::FromString(TEXT("NIGHT PHASE"));
	case EGamePhase::DawnTransition:
		return FText::FromString(TEXT("DAWN APPROACHES..."));
	case EGamePhase::Dawn:
		return FText::FromString(TEXT("DAWN PHASE"));
	case EGamePhase::LoopEnd:
		return FText::FromString(TEXT("LOOP COMPLETE"));
	default:
		return FText::FromString(TEXT("---"));
	}
}

void UGameplayHUDWidget::UpdateWaveInfo(int32 CurrentWave, int32 TotalWaves, int32 InRemainingEnemies)
{
	if (WaveInfoText)
	{
		FString WaveString = FString::Printf(TEXT("WAVE %d / %d"), CurrentWave, TotalWaves);
		WaveInfoText->SetText(FText::FromString(WaveString));
	}

	if (EnemyCountText)
	{
		FString EnemyString = FString::Printf(TEXT("Enemies: %d"), InRemainingEnemies);
		EnemyCountText->SetText(FText::FromString(EnemyString));
	}
}

void UGameplayHUDWidget::ShowReaperReadyWarning()
{
	if (ReaperReadyWarningPanel && ReaperReadyWarningPanel->GetVisibility() != ESlateVisibility::Visible)
	{
		ReaperReadyWarningPanel->SetVisibility(ESlateVisibility::Visible);
		PlayAttentionPulse(ReaperReadyWarningPanel, true);

		UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDWidget] リーパーモード発動可能！"));
	}
}

void UGameplayHUDWidget::HideReaperReadyWarning()
{
	if (ReaperReadyWarningPanel && ReaperReadyWarningPanel->GetVisibility() == ESlateVisibility::Visible)
	{
		StopWidgetAnimation(ReaperReadyWarningPanel);
		ReaperReadyWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGameplayHUDWidget::ShowDawnWarning()
{
	if (DawnWarningPanel && DawnWarningPanel->GetVisibility() != ESlateVisibility::Visible)
	{
		DawnWarningPanel->SetVisibility(ESlateVisibility::Visible);
		PlayAttentionPulse(DawnWarningPanel, true);

		UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDWidget] 夜明けが近い！"));
	}
}

void UGameplayHUDWidget::ShowWaveStartWarning(int32 WaveNumber)
{
	UE_LOG(LogDawnlight, Log, TEXT("[GameplayHUDWidget] Wave %d 開始！"), WaveNumber);

	// Waveアナウンスメントテキストを設定
	if (WaveAnnouncementText)
	{
		FString WaveString = FString::Printf(TEXT("WAVE %d"), WaveNumber);
		WaveAnnouncementText->SetText(FText::FromString(WaveString));
	}

	// アナウンスメントパネルを表示
	if (WaveAnnouncementPanel)
	{
		WaveAnnouncementPanel->SetVisibility(ESlateVisibility::Visible);

		// パルスアニメーションを再生
		PlayAttentionPulse(WaveAnnouncementPanel, false);
	}

	// 一定時間後に非表示にするタイマーを設定
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaveAnnouncementTimerHandle);
		World->GetTimerManager().SetTimer(
			WaveAnnouncementTimerHandle,
			this,
			&UGameplayHUDWidget::HideWaveAnnouncement,
			WaveAnnouncementDuration,
			false
		);
	}
}

void UGameplayHUDWidget::HideWaveAnnouncement()
{
	if (WaveAnnouncementPanel)
	{
		// フェードアウトしてから非表示にする
		StopWidgetAnimation(WaveAnnouncementPanel);
		WaveAnnouncementPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGameplayHUDWidget::ShowReaperModeIndicator(bool bShow)
{
	if (ReaperModeIndicator)
	{
		if (bShow)
		{
			ReaperModeIndicator->SetVisibility(ESlateVisibility::Visible);
			PlayAttentionPulse(ReaperModeIndicator, true);
		}
		else
		{
			StopWidgetAnimation(ReaperModeIndicator);
			ReaperModeIndicator->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UGameplayHUDWidget::ShowDamageBuffIndicator(float BuffPercent)
{
	if (BuffIndicator)
	{
		BuffIndicator->SetVisibility(ESlateVisibility::Visible);
	}

	if (BuffPercentText)
	{
		FString BuffString = FString::Printf(TEXT("+%.0f%% DMG"), BuffPercent);
		BuffPercentText->SetText(FText::FromString(BuffString));
	}
}

void UGameplayHUDWidget::UpdateAnimalCount(int32 AliveAnimals, int32 TotalAnimals)
{
	if (!AnimalCountText)
	{
		return;
	}

	FString AnimalString = FString::Printf(TEXT("Animals: %d / %d"), AliveAnimals, TotalAnimals);
	AnimalCountText->SetText(FText::FromString(AnimalString));
}

void UGameplayHUDWidget::UpdatePlayerHealth(float CurrentHP, float MaxHP)
{
	// HPバーを更新
	if (PlayerHealthBar)
	{
		const float Percent = MaxHP > 0.0f ? CurrentHP / MaxHP : 0.0f;
		PlayerHealthBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));

		// HPに応じて色を変更
		FLinearColor HealthColor;
		if (Percent > 0.6f)
		{
			HealthColor = FLinearColor(0.2f, 0.8f, 0.2f, 1.0f);  // 緑
		}
		else if (Percent > 0.3f)
		{
			HealthColor = FLinearColor(0.9f, 0.7f, 0.1f, 1.0f);  // 黄
		}
		else
		{
			HealthColor = FLinearColor(0.9f, 0.2f, 0.2f, 1.0f);  // 赤
		}
		PlayerHealthBar->SetFillColorAndOpacity(HealthColor);
	}

	// HPテキストを更新
	if (PlayerHealthText)
	{
		FString HealthString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, MaxHP);
		PlayerHealthText->SetText(FText::FromString(HealthString));
	}
}

void UGameplayHUDWidget::UpdatePhasePanels(EGamePhase Phase)
{
	// Night Phase用パネル
	if (NightPhasePanel)
	{
		const bool bShowNight = (Phase == EGamePhase::Night);
		NightPhasePanel->SetVisibility(bShowNight ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Dawn Phase用パネル
	if (DawnPhasePanel)
	{
		const bool bShowDawn = (Phase == EGamePhase::Dawn);
		DawnPhasePanel->SetVisibility(bShowDawn ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
