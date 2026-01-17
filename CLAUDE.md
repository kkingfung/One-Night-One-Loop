# Dawnlight - UE5 Project Documentation

## Project Overview

**Dawnlight** (One Night, One Loop) is a 2.5D top-down survival/horror game built with Unreal Engine 5.7.

- **Project Name**: Dawnlight
- **Repository**: One-Night-One-Loop
- **Engine Version**: Unreal Engine 5.7
- **Primary Language**: C++ with Blueprints (Hybrid Architecture)
- **Target Demo**: 15-20 minute playable demo

### Game Concept
> 「監視される夜を生き延び、証拠を記録せよ」

Night 1のテーマは「監視されている」。プレイヤーは光と影を使い分けながら証拠を撮影し、夜明けまで生き延びる。

### Core Design Documents
- **[GDD.md](Dawnlight/GDD.md)** - Game Design Document（全体設計）
- **[PROJECT_STRUCTURE.md](Dawnlight/PROJECT_STRUCTURE.md)** - UE5プロジェクト構成
- **[CODING_STANDARDS.md](Dawnlight/CODING_STANDARDS.md)** - C++コーディング規約
- **[ART_STYLE_GUIDE.md](Dawnlight/ART_STYLE_GUIDE.md)** - ビジュアルスタイルガイド
- **[SOUND_DESIGN.md](Dawnlight/SOUND_DESIGN.md)** - サウンドデザインドキュメント
- **[LEVEL_DESIGN.md](Dawnlight/LEVEL_DESIGN.md)** - レベルデザインドキュメント
- **[NARRATIVE.md](Dawnlight/NARRATIVE.md)** - ナラティブ・ストーリードキュメント

---

## Game Systems Overview

### Player Actions (3 Core)
| アクション | 説明 |
|-----------|------|
| **移動** | 光を避けて移動。光に入ると検知ゲージ上昇 |
| **撮影** | 証拠を記録。撮影中は無防備 |
| **隠れる** | 物陰に退避。安全だが時間消費 |

### Key Subsystems
| Subsystem | 役割 |
|-----------|------|
| `EventDirectorSubsystem` | イベント選択・管理 |
| `SurveillanceSubsystem` | 監視レベル・検知管理 |
| `NightProgressSubsystem` | 夜の進行（時間管理） |

### Technology Stack
- **GAS (Gameplay Ability System)** - アビリティ/エフェクト管理
- **Enhanced Input** - 入力システム（コンテキスト対応）
- **Gameplay Tags** - 状態管理、イベントフラグ
- **Data Assets** - イベント定義、設定データ
- **UnrealGenAISupport** - MCP連携、AI統合

### MCP Integration
- **[MCP_SETUP.md](Dawnlight/MCP_SETUP.md)** - MCPセットアップガイド
- **Plugin**: UnrealGenAISupport (Git Submodule)
- **用途**: シーン操作、Blueprint生成、プレイログ分析

## Architecture Guidelines

### Module Structure

```
Dawnlight/
├── Source/
│   └── Dawnlight/
│       ├── Dawnlight.Build.cs          # Module build configuration
│       ├── Dawnlight.h                  # Module header
│       ├── Dawnlight.cpp                # Module implementation
│       ├── Core/                        # Core game systems
│       │   ├── GameModes/
│       │   ├── GameStates/
│       │   └── PlayerControllers/
│       ├── Characters/                  # Character classes
│       │   ├── Player/
│       │   └── NPCs/
│       ├── Components/                  # Actor components
│       ├── Subsystems/                  # Game subsystems
│       ├── Abilities/                   # Gameplay abilities (if using GAS)
│       ├── AI/                          # AI controllers and behaviors
│       ├── UI/                          # UMG widgets and HUD
│       ├── Data/                        # Data assets and tables
│       └── Utilities/                   # Helper classes and functions
├── Content/
│   ├── Blueprints/                     # Blueprint assets
│   ├── Characters/                     # Character assets
│   ├── Maps/                           # Level assets
│   ├── Materials/                      # Material assets
│   ├── Textures/                       # Texture assets
│   ├── Audio/                          # Audio assets
│   ├── VFX/                            # Visual effects
│   ├── UI/                             # UI assets
│   └── Data/                           # Data tables and curves
└── Config/                             # Configuration files
```

### Design Patterns

#### Subsystem Pattern
Use UE5 Subsystems for global services:

```cpp
// Access via GetWorld() or GetGameInstance()
UMyGameSubsystem* Subsystem = GetWorld()->GetSubsystem<UMyGameSubsystem>();
```

#### Component-Based Architecture
Favor composition over inheritance:

```cpp
// Add functionality through components
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<UHealthComponent> HealthComponent;
```

#### Data-Driven Design
Use Data Assets and Data Tables for configuration:

```cpp
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
TObjectPtr<UCharacterDataAsset> CharacterData;
```

## Coding Standards

### CRITICAL: Comments in Japanese
All code comments, documentation, and inline notes must be written in Japanese:

```cpp
/**
 * プレイヤーキャラクターの基底クラス
 *
 * このクラスは全てのプレイヤー操作キャラクターの基本機能を提供します。
 * 移動、入力処理、カメラ制御などの共通機能を含みます。
 */
UCLASS()
class DAWNLIGHT_API ADawnlightCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    /** デフォルトコンストラクタ */
    ADawnlightCharacter();

protected:
    /** ゲーム開始時に呼び出される */
    virtual void BeginPlay() override;

    /** 毎フレーム呼び出される */
    virtual void Tick(float DeltaTime) override;

private:
    /** 体力コンポーネント - ダメージ処理を管理 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHealthComponent> HealthComponent;

    /** 現在の移動速度を計算する */
    float CalculateMovementSpeed() const;
};
```

### Naming Conventions

#### Classes
| Type | Prefix | Example |
|------|--------|---------|
| Actor | A | `ADawnlightCharacter` |
| UObject | U | `UHealthComponent` |
| Interface | I | `IInteractable` |
| Struct | F | `FCharacterStats` |
| Enum | E | `ECharacterState` |
| Template | T | `TWeakObjectPtr` |
| Typedef (bool) | b | `bIsAlive` |

#### Variables and Functions
```cpp
// Member variables: PascalCase
UPROPERTY()
float MaxHealth;

// Local variables: PascalCase
float CurrentHealth = MaxHealth;

// Boolean: b prefix
bool bIsAlive;
bool bCanAttack;

// Functions: PascalCase with verb
void UpdateHealth();
float CalculateDamage();
bool CanPerformAction();

// Getters/Setters
float GetHealth() const;
void SetHealth(float NewHealth);

// Event handlers: On prefix
void OnDamageTaken(float Damage);
void OnCharacterDeath();
```

#### File Naming
```
Source Files:     PascalCase.cpp, PascalCase.h
Blueprint:        BP_CharacterName
Widget:           WBP_WidgetName
Material:         M_MaterialName
Material Instance: MI_MaterialInstanceName
Texture:          T_TextureName_D (Diffuse), T_TextureName_N (Normal)
Static Mesh:      SM_MeshName
Skeletal Mesh:    SK_MeshName
Animation:        A_AnimationName or AM_AnimMontage
Sound:            S_SoundName
Particle:         P_ParticleName or NS_NiagaraSystem
Data Asset:       DA_DataAssetName
Data Table:       DT_DataTableName
Level:            L_LevelName or MAP_LevelName
```

### UPROPERTY Specifiers

```cpp
// Editable in editor, visible in blueprints
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
float BaseHealth = 100.0f;

// Editable per-instance
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
float MovementSpeed = 600.0f;

// Runtime only, blueprint accessible
UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
bool bIsInCombat;

// Replicated property
UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Stats")
float Health;

// Component reference
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
TObjectPtr<USceneComponent> RootComp;

// Asset reference (soft reference for async loading)
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assets")
TSoftObjectPtr<UStaticMesh> MeshAsset;

// Class reference
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
TSubclassOf<AActor> SpawnClass;
```

### UFUNCTION Specifiers

```cpp
// Blueprint callable
UFUNCTION(BlueprintCallable, Category = "Combat")
void Attack();

// Blueprint pure (no side effects)
UFUNCTION(BlueprintPure, Category = "Stats")
float GetHealthPercent() const;

// Blueprint implementable event
UFUNCTION(BlueprintImplementableEvent, Category = "Events")
void OnLevelUp();

// Blueprint native event (C++ default, BP override)
UFUNCTION(BlueprintNativeEvent, Category = "Events")
void OnDeath();
virtual void OnDeath_Implementation();

// Server RPC
UFUNCTION(Server, Reliable, WithValidation)
void ServerPerformAction();

// Client RPC
UFUNCTION(Client, Reliable)
void ClientReceiveUpdate();

// Multicast RPC
UFUNCTION(NetMulticast, Unreliable)
void MulticastPlayEffect();
```

### Memory Management

```cpp
// Use TObjectPtr for UPROPERTY object pointers (UE5 standard)
UPROPERTY()
TObjectPtr<UActorComponent> MyComponent;

// Use raw pointers for local/temporary references
UActorComponent* TempComponent = FindComponentByClass<UActorComponent>();

// Use TWeakObjectPtr when you don't own the object
TWeakObjectPtr<AActor> WeakActorRef;

// Use TSharedPtr/TUniquePtr for non-UObject classes
TSharedPtr<FMyStruct> SharedData;
TUniquePtr<FMyStruct> UniqueData;

// Use UPROPERTY() to prevent garbage collection
UPROPERTY()
TArray<TObjectPtr<UObject>> ManagedObjects;
```

### Modern C++ in UE5

```cpp
// Use auto judiciously
auto* Component = GetComponentByClass<UMyComponent>();

// Range-based for loops
for (const auto& Element : MyArray)
{
    // Process element
}

// Lambda expressions
MyArray.RemoveAll([](const FMyStruct& Item)
{
    return Item.bShouldRemove;
});

// Use nullptr instead of NULL
if (MyPointer != nullptr)

// Use constexpr where applicable
static constexpr float DefaultSpeed = 600.0f;

// Use override and final
virtual void BeginPlay() override;
virtual void Tick(float DeltaTime) override final;
```

### Error Handling

```cpp
// Use check() for programming errors (fatal in all builds)
check(MyPointer != nullptr);

// Use ensure() for recoverable errors (logs but continues)
if (!ensure(MyPointer != nullptr))
{
    return;
}

// Use verify() when the expression has side effects
verify(ImportantFunction());

// Use checkf() for informative crashes
checkf(Index < ArraySize, TEXT("Index %d out of bounds (Size: %d)"), Index, ArraySize);

// IsValid() for UObject pointers
if (IsValid(MyActor))
{
    MyActor->DoSomething();
}
```

### Logging

```cpp
// Define custom log category in header
DECLARE_LOG_CATEGORY_EXTERN(LogDawnlight, Log, All);

// Define in cpp
DEFINE_LOG_CATEGORY(LogDawnlight);

// Usage
UE_LOG(LogDawnlight, Log, TEXT("プレイヤーがスポーンしました"));
UE_LOG(LogDawnlight, Warning, TEXT("アイテムが見つかりません: %s"), *ItemName);
UE_LOG(LogDawnlight, Error, TEXT("致命的なエラーが発生しました"));

// Conditional logging (development only)
UE_CLOG(bDebugMode, LogDawnlight, Verbose, TEXT("デバッグ情報: %f"), Value);
```

## Blueprint Standards

### Naming Conventions
- Use clear, descriptive names for variables and functions
- Prefix local variables with `Local_`
- Prefix input parameters with `In_`
- Prefix output parameters with `Out_`

### Organization
- Group related nodes with comments (in Japanese)
- Use Reroute nodes to keep wires clean
- Collapse complex logic into functions or macros
- Keep Event Graph focused on event handling
- Use separate functions for logic

### Best Practices
- Avoid Tick in Blueprints when possible
- Use Timers or Events instead of continuous checking
- Cache component references in BeginPlay
- Use interfaces for cross-blueprint communication

## Asset Guidelines

### Import Settings

#### Textures
- Use power-of-2 dimensions (512, 1024, 2048, 4096)
- Compress appropriately (BC7 for quality, BC1/BC3 for performance)
- Generate mipmaps for 3D textures
- Use appropriate texture groups

#### Static Meshes
- Keep polycount reasonable for target platform
- Use LODs for complex meshes
- Ensure proper UV layout for lightmaps
- Set up collision appropriately

#### Skeletal Meshes
- Limit bone count based on target platform
- Use LODs for character meshes
- Ensure consistent skeleton hierarchy

### Content Organization
- One asset type per folder
- Use descriptive folder names
- Maintain consistent folder structure across projects
- Use asset redirectors when moving assets

## Version Control

### Commit Messages
Write commit messages in Japanese:
```
[機能] プレイヤーの移動システムを実装
[修正] ジャンプ時のコリジョン問題を修正
[改善] キャラクターのアニメーション遷移を改善
[リファクタ] HealthComponentのコードを整理
```

### Branching Strategy
- `main` - Stable, release-ready code
- `develop` - Integration branch for features
- `feature/*` - Feature development
- `bugfix/*` - Bug fixes
- `hotfix/*` - Emergency production fixes

## Performance Guidelines

### General
- Profile before optimizing
- Use Unreal Insights for profiling
- Target frame budget allocation

### C++ Specific
- Mark functions `const` when they don't modify state
- Use `FORCEINLINE` sparingly and only after profiling
- Avoid allocations in hot paths
- Use object pooling for frequently spawned actors

### Blueprint Specific
- Minimize nodes in Tick
- Use C++ for performance-critical logic
- Cache expensive operations

## Testing

### PIE Testing
- Test in various PIE configurations
- Test with different player counts for multiplayer
- Verify network replication

### Automation
- Write unit tests for critical systems
- Use Functional Tests for gameplay testing
- Automate build and packaging

## Dependencies and Plugins

### Required Plugins
- **GameplayAbilities** - GAS (Gameplay Ability System)
- **EnhancedInput** - Enhanced Input System
- **GenerativeAISupport** - MCP/AI統合 (Git Submodule)
- **ModelingToolsEditorMode** (Editor only)

### Git Submodules
```bash
# 初回クローン時
git clone --recurse-submodules <repository-url>

# 既存リポジトリでsubmodule取得
git submodule update --init --recursive

# submodule更新
git submodule update --remote
```

### Adding Plugins
1. Add plugin via Plugins browser or .uproject
2. Regenerate project files
3. Document plugin usage and version

---

## Project-Specific Gameplay Tags

```cpp
// 状態タグ
State.Player.Hidden          // プレイヤーが隠れている
State.Player.Photographing   // 撮影中
State.Player.Detected        // 検知された
State.Player.Safe            // 安全状態

// フェーズタグ
Phase.Night.Introduction     // 導入（低緊張）
Phase.Night.Relaxation       // 緩和（中緊張）
Phase.Night.Climax           // 締め（再緊張）

// 監視レベルタグ
Surveillance.Level.Low       // 監視レベル：低
Surveillance.Level.Medium    // 監視レベル：中
Surveillance.Level.High      // 監視レベル：高
Surveillance.Level.Critical  // 監視レベル：危険
```

---

## Project-Specific Patterns

### Event Data Asset Structure
```cpp
/**
 * イベントデータアセットの基底クラス
 */
UCLASS(BlueprintType)
class UEventDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    /** イベント識別子 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
    FGameplayTag EventTag;

    /** イベント名（日本語） */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
    FText EventName;

    /** イベント名（英語） */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
    FString EventNameEN;

    /** 緊張度レベル (0.0 - 1.0) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float TensionLevel = 0.5f;

    /** 発生するフェーズ */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "設定")
    FGameplayTag RequiredPhase;
};
```

### Surveillance Detection Pattern
```cpp
/**
 * 光源による検知判定
 */
bool USurveillanceSubsystem::IsInLight(const FVector& Location) const
{
    // 登録された全ての光源をチェック
    for (const auto& LightSource : RegisteredLightSources)
    {
        if (!LightSource.IsValid())
        {
            continue;
        }

        // 光の範囲内かチェック
        const float Distance = FVector::Distance(Location, LightSource->GetActorLocation());
        const float LightRadius = GetLightRadius(LightSource.Get());

        if (Distance <= LightRadius)
        {
            // 遮蔽物チェック
            if (!IsOccluded(Location, LightSource->GetActorLocation()))
            {
                return true;
            }
        }
    }

    return false;
}
```

### Photography Ability Pattern
```cpp
/**
 * 撮影アビリティの実装パターン
 */
void UGA_Photograph::ActivateAbility(...)
{
    // 撮影開始タグを付与
    AbilitySystemComponent->AddLooseGameplayTag(Tag_State_Photographing);

    // カメラビューファインダーUIを表示
    ShowViewfinderWidget();

    // シャッター音のリスク判定
    if (ShouldPlayShutterSound())
    {
        // 周囲の敵にサウンドイベントを発行
        BroadcastSoundEvent(ShutterSoundRadius);
    }

    // 撮影データを保存
    RecordPhotographData(GetViewfinderTarget());

    // タイマーで撮影完了を待機
    GetWorld()->GetTimerManager().SetTimer(
        PhotographTimerHandle,
        this,
        &UGA_Photograph::OnPhotographComplete,
        PhotographDuration,
        false
    );
}
```

## Frequently Used Patterns

### Spawning Actors
```cpp
FActorSpawnParameters SpawnParams;
SpawnParams.Owner = this;
SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

AActor* SpawnedActor = GetWorld()->SpawnActor<AMyActor>(
    ActorClass,
    SpawnLocation,
    SpawnRotation,
    SpawnParams
);
```

### Timer Usage
```cpp
// タイマーを設定
GetWorld()->GetTimerManager().SetTimer(
    TimerHandle,
    this,
    &AMyActor::OnTimerExpired,
    Delay,
    bLooping
);

// タイマーをクリア
GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
```

### Async Asset Loading
```cpp
// 非同期でアセットをロード
FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
StreamableManager.RequestAsyncLoad(
    AssetPath,
    FStreamableDelegate::CreateUObject(this, &AMyActor::OnAssetLoaded)
);
```

### Interface Implementation
```cpp
// Interface definition
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class IInteractable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor* Interactor);
};

// Check and call
if (Actor->Implements<UInteractable>())
{
    IInteractable::Execute_Interact(Actor, this);
}
```

## Troubleshooting

### Common Issues

#### Compilation Errors
- Regenerate project files after adding new source files
- Check for circular dependencies
- Ensure all GENERATED_BODY() macros are present

#### Blueprint Errors
- Check for broken references after renaming
- Use Fix Up Redirectors after moving assets
- Verify parent class changes

#### Runtime Crashes
- Check for null pointer access
- Verify garbage collection isn't collecting active objects
- Use IsValid() before accessing UObjects

---

**Last Updated**: 2025-01-15
**UE Version**: 5.7
