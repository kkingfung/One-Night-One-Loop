# Dawnlight - UE5 Project Structure

## ディレクトリ構成

```
Dawnlight/
├── Config/
│   ├── DefaultEngine.ini
│   ├── DefaultGame.ini
│   ├── DefaultInput.ini
│   └── DefaultGameplayTags.ini      # Gameplay Tags定義
│
├── Content/
│   ├── _Core/                        # コアゲームアセット
│   │   ├── BP_GameMode.uasset
│   │   ├── BP_GameState.uasset
│   │   └── BP_PlayerController.uasset
│   │
│   ├── Characters/
│   │   └── Player/
│   │       ├── BP_PlayerCharacter.uasset
│   │       ├── ABP_Player.uasset     # Animation Blueprint
│   │       └── Input/
│   │           ├── IA_Move.uasset    # Input Action
│   │           ├── IA_Interact.uasset
│   │           ├── IA_Hide.uasset
│   │           ├── IA_Camera.uasset
│   │           └── IMC_Default.uasset # Input Mapping Context
│   │
│   ├── Systems/
│   │   ├── Surveillance/             # 監視システム
│   │   │   ├── BP_LightDetector.uasset
│   │   │   ├── BP_SurveillanceCamera.uasset
│   │   │   └── DA_SurveillanceSettings.uasset
│   │   │
│   │   ├── Events/                   # イベントシステム
│   │   │   ├── DA_Event_FirstLight.uasset
│   │   │   ├── DA_Event_LeftBehind.uasset
│   │   │   ├── DA_Event_SystemAwareness.uasset
│   │   │   └── RandomEvents/
│   │   │       ├── DA_Event_PatrolLight.uasset
│   │   │       ├── DA_Event_ReactingShadow.uasset
│   │   │       ├── DA_Event_BrokenTerminal.uasset
│   │   │       └── DA_Event_StrangeNoise.uasset
│   │   │
│   │   └── Photography/              # 撮影システム
│   │       ├── BP_CameraDevice.uasset
│   │       └── WBP_CameraViewfinder.uasset
│   │
│   ├── UI/
│   │   ├── HUD/
│   │   │   ├── WBP_MainHUD.uasset
│   │   │   ├── WBP_DetectionGauge.uasset
│   │   │   └── WBP_TimeRemaining.uasset
│   │   │
│   │   └── Screens/
│   │       ├── WBP_TitleScreen.uasset
│   │       ├── WBP_NightComplete.uasset
│   │       └── WBP_PauseMenu.uasset
│   │
│   ├── Maps/
│   │   ├── L_MainMenu.umap
│   │   ├── L_Night1/
│   │   │   ├── L_Night1_Persistent.umap
│   │   │   ├── L_Night1_Entrance.umap
│   │   │   ├── L_Night1_MainHall.umap
│   │   │   └── L_Night1_DeepArea.umap
│   │   └── L_DevTest.umap            # 開発テスト用
│   │
│   ├── Audio/
│   │   ├── SFX/
│   │   │   ├── S_Footstep_*.uasset
│   │   │   ├── S_CameraShutter.uasset
│   │   │   └── S_AlertSound.uasset
│   │   │
│   │   └── Ambience/
│   │       ├── S_Amb_Night1.uasset
│   │       └── S_Amb_Tension.uasset
│   │
│   ├── VFX/
│   │   ├── NS_LightFlicker.uasset
│   │   └── NS_DetectionPulse.uasset
│   │
│   ├── Materials/
│   │   ├── M_Master_Environment.uasset
│   │   ├── M_Master_Character.uasset
│   │   └── PostProcess/
│   │       ├── M_PP_NightVision.uasset
│   │       └── M_PP_Detection.uasset
│   │
│   └── Data/
│       ├── DT_GameplayTags.uasset
│       ├── DT_EventPool.uasset
│       └── Curves/
│           ├── C_TensionCurve.uasset
│           └── C_DetectionDecay.uasset
│
├── Source/
│   └── Dawnlight/
│       ├── Dawnlight.Build.cs
│       ├── Dawnlight.h
│       ├── Dawnlight.cpp
│       │
│       ├── Core/                     # コアゲームクラス
│       │   ├── DawnlightGameMode.h/cpp
│       │   ├── DawnlightGameState.h/cpp
│       │   ├── DawnlightPlayerController.h/cpp
│       │   └── DawnlightPlayerState.h/cpp
│       │
│       ├── Characters/
│       │   └── DawnlightCharacter.h/cpp
│       │
│       ├── Components/               # アクターコンポーネント
│       │   ├── SurveillanceDetectorComponent.h/cpp
│       │   ├── PhotographyComponent.h/cpp
│       │   └── HideableComponent.h/cpp
│       │
│       ├── Subsystems/               # ゲームサブシステム
│       │   ├── EventDirectorSubsystem.h/cpp
│       │   ├── SurveillanceSubsystem.h/cpp
│       │   └── NightProgressSubsystem.h/cpp
│       │
│       ├── Abilities/                # GAS関連
│       │   ├── DawnlightAbilitySystemComponent.h/cpp
│       │   ├── DawnlightAttributeSet.h/cpp
│       │   ├── Abilities/
│       │   │   ├── GA_Photograph.h/cpp
│       │   │   └── GA_Hide.h/cpp
│       │   └── Effects/
│       │       ├── GE_Detection.h/cpp
│       │       └── GE_TensionIncrease.h/cpp
│       │
│       ├── Events/                   # イベントシステム
│       │   ├── EventData.h           # データ構造体
│       │   ├── EventBase.h/cpp
│       │   ├── FixedEvent.h/cpp
│       │   └── RandomEvent.h/cpp
│       │
│       ├── AI/                       # AIシステム
│       │   └── EventDirector/
│       │       ├── EventSelector.h/cpp
│       │       └── TensionCalculator.h/cpp
│       │
│       ├── Data/                     # データアセット
│       │   ├── EventDataAsset.h/cpp
│       │   ├── SurveillanceSettings.h/cpp
│       │   └── NightSettings.h/cpp
│       │
│       └── Utilities/                # ユーティリティ
│           ├── DawnlightStatics.h/cpp
│           └── DawnlightTags.h       # Gameplay Tags定義
│
└── Plugins/                          # プロジェクトプラグイン
    └── (必要に応じて追加)
```

---

## モジュール依存関係

```
Dawnlight (Primary Game Module)
├── Core                    # 依存なし
├── Characters              # → Core
├── Components              # → Core
├── Subsystems              # → Core, Components
├── Abilities               # → Core, Components (GAS)
├── Events                  # → Core, Data
├── AI                      # → Events, Subsystems
├── Data                    # → Core
└── Utilities               # 依存なし
```

---

## Gameplay Tags 構成

```
// DawnlightTags.h で定義

// 状態タグ
State.Player.Hidden
State.Player.Photographing
State.Player.Detected
State.Player.Safe

// フェーズタグ
Phase.Night.Introduction
Phase.Night.Relaxation
Phase.Night.Climax

// イベントタグ
Event.Fixed.FirstLight
Event.Fixed.LeftBehind
Event.Fixed.SystemAwareness
Event.Random.PatrolLight
Event.Random.ReactingShadow
Event.Random.BrokenTerminal
Event.Random.StrangeNoise

// 監視タグ
Surveillance.Level.Low
Surveillance.Level.Medium
Surveillance.Level.High
Surveillance.Level.Critical

// 入力タグ
Input.Action.Move
Input.Action.Photograph
Input.Action.Hide
Input.Action.Interact
```

---

## Enhanced Input 構成

### Input Actions

| IA名 | タイプ | トリガー | 用途 |
|------|--------|---------|------|
| IA_Move | Axis2D | Continuous | WASD移動 |
| IA_Photograph | Bool | Pressed | 撮影開始 |
| IA_Hide | Bool | Pressed | 隠れる |
| IA_Interact | Bool | Pressed | インタラクト |
| IA_Camera | Axis2D | Continuous | カメラ操作（マウス/右スティック） |
| IA_Pause | Bool | Pressed | ポーズメニュー |

### Input Mapping Contexts

| IMC名 | 用途 | 優先度 |
|-------|------|--------|
| IMC_Default | 通常プレイ | 0 |
| IMC_Hidden | 隠れ中（移動制限） | 1 |
| IMC_Photographing | 撮影中（移動制限） | 1 |
| IMC_UI | UIナビゲーション | 2 |

---

## GAS 構成

### Attribute Set

```cpp
// DawnlightAttributeSet.h

UPROPERTY(BlueprintReadOnly, Category = "監視")
FGameplayAttributeData DetectionLevel;  // 0.0 - 100.0

UPROPERTY(BlueprintReadOnly, Category = "監視")
FGameplayAttributeData TensionLevel;    // 0.0 - 100.0

UPROPERTY(BlueprintReadOnly, Category = "進行")
FGameplayAttributeData NightProgress;   // 0.0 - 1.0
```

### Gameplay Abilities

| GA名 | 説明 | タグ |
|------|------|------|
| GA_Photograph | 撮影アビリティ | Ability.Action.Photograph |
| GA_Hide | 隠れるアビリティ | Ability.Action.Hide |
| GA_Interact | インタラクトアビリティ | Ability.Action.Interact |

### Gameplay Effects

| GE名 | 説明 | 持続 |
|------|------|------|
| GE_Detection_Increase | 光に入った時の検知増加 | Instant |
| GE_Detection_Decay | 時間経過での検知減少 | Duration |
| GE_Tension_PhaseShift | フェーズ移行時の緊張度変更 | Instant |

---

## Subsystem 設計

### EventDirectorSubsystem

```cpp
/**
 * イベント選択・管理を行うサブシステム
 * 固定イベントの進行管理とランダムイベントの選択を担当
 */
UCLASS()
class UEventDirectorSubsystem : public UGameInstanceSubsystem
{
    // 現在のフェーズ
    EPhaseType CurrentPhase;

    // 固定イベント進行状況
    TArray<bool> FixedEventCompleted;

    // ランダムイベント選択
    UEventDataAsset* SelectRandomEvent();

    // フェーズ進行
    void AdvancePhase();
};
```

### SurveillanceSubsystem

```cpp
/**
 * 監視レベルと検知状態を管理するサブシステム
 */
UCLASS()
class USurveillanceSubsystem : public UWorldSubsystem
{
    // 登録された光源
    TArray<TWeakObjectPtr<AActor>> RegisteredLightSources;

    // 検知判定
    bool IsInLight(const FVector& Location);

    // 監視レベル更新
    void UpdateSurveillanceLevel(float DeltaTime);
};
```

### NightProgressSubsystem

```cpp
/**
 * 夜の進行（時間管理）を行うサブシステム
 */
UCLASS()
class UNightProgressSubsystem : public UWorldSubsystem
{
    // 夜の残り時間
    float RemainingTime;

    // 夜明け判定
    bool IsDawnApproaching();

    // 強制終了トリガー
    void TriggerForcedReturn();
};
```

---

## 開発優先順位

### Priority 1: 基盤（Week 1-2）
1. プロジェクトセットアップ（Build.cs, Module）
2. Gameplay Tags定義
3. 基本キャラクター移動
4. Enhanced Input設定

### Priority 2: コアシステム（Week 3-4）
1. SurveillanceSubsystem（光検知）
2. 基本Attribute Set
3. 撮影アビリティ（GA_Photograph）
4. 隠れるアビリティ（GA_Hide）

### Priority 3: イベント（Week 5-6）
1. EventDirectorSubsystem
2. 固定イベント3つ実装
3. ランダムイベント2枠実装
4. フェーズ進行システム

### Priority 4: 演出・仕上げ（Week 7-8）
1. NightProgressSubsystem
2. 夜明け演出
3. UI/HUD
4. サウンド・VFX

---

## 命名規則（リマインダー）

| タイプ | 接頭辞 | 例 |
|--------|--------|-----|
| Blueprint Actor | BP_ | BP_PlayerCharacter |
| Widget Blueprint | WBP_ | WBP_MainHUD |
| Data Asset | DA_ | DA_Event_FirstLight |
| Input Action | IA_ | IA_Move |
| Input Mapping Context | IMC_ | IMC_Default |
| Level | L_ | L_Night1_Entrance |
| Material | M_ | M_Master_Environment |
| Sound | S_ | S_CameraShutter |
| Niagara System | NS_ | NS_LightFlicker |
| Animation Blueprint | ABP_ | ABP_Player |
| Data Table | DT_ | DT_EventPool |
| Curve | C_ | C_TensionCurve |

---

**Last Updated**: 2025-01-15
