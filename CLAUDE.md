# Soul Reaper - UE5 Project Documentation

## Project Overview

**Soul Reaper** (One Night, One Loop) is a top-down action roguelite built with Unreal Engine 5.7.

- **Project Name**: Soul Reaper (Code name: Dawnlight)
- **Repository**: One-Night-One-Loop
- **Engine Version**: Unreal Engine 5.7
- **Primary Language**: C++ with Blueprints (Hybrid Architecture)
- **Camera**: Top-Down (固定俯瞰視点)
- **Target Demo**: 10-15 minute single loop

### Game Concept
> 「夜に魂を集め、夜明けに力を解き放て」

プレイヤーは死神（Stylized Death）として、夜の墓地で動物の魂を収集する。夜明けと共に収集した魂がバフに変換され、襲い来るミニオンの群れと戦う。

### Core Fun
**「集めれば集めるほど強くなる。そしてその力を解き放つ瞬間。」**

### Core Design Documents
- **[GDD.md](Dawnlight/GDD.md)** - Game Design Document（全体設計）

---

## Game Systems Overview

### Two-Phase Gameplay
| Phase | 内容 | 時間 |
|-------|------|------|
| **Night Phase** | 動物を狩って魂を収集（戦闘チュートリアル） | 2-3分 |
| **Dawn Phase** | ウェーブ形式の敵と戦闘 | クリアまで |

### Player Actions
| 入力 | アクション | 説明 |
|------|-----------|------|
| **左クリック** | 通常攻撃 | Attack_A, B（鎌スイング） |
| **右クリック** | 強攻撃 | Attack_C, D（強力な一撃） |
| **Q** | 特殊攻撃 | Staff_Attack（長射程） |
| **WASD** | 移動 | 8方向移動 |
| **Space** | リーパーモード | ゲージ満タン時のみ |

### Key Subsystems（予定）
| Subsystem | 役割 |
|-----------|------|
| `SoulReaperGameMode` | フェーズ管理（Night/Dawn切り替え） |
| `SoulCollectionSubsystem` | 魂収集、インベントリ管理 |
| `WaveSpawnerSubsystem` | 敵ウェーブスポーン |
| `ReaperModeComponent` | リーパーモード管理 |

### Technology Stack
- **GAS (Gameplay Ability System)** - アビリティ、バフ/デバフ管理
- **Enhanced Input** - 入力処理
- **Gameplay Tags** - 状態管理（Night/Dawn/Reaper等）
- **Data Assets** - 魂定義、敵定義、アップグレード定義
- **AI (Behavior Trees)** - 敵AI、動物AI
- **Niagara** - VFX
- **UMG** - UI

### MCP Integration
- **Plugin**: UnrealGenAISupport (Git Submodule)
- **用途**: シーン操作、Blueprint生成

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
// ゲームフェーズタグ
Phase.Night                  // 夜フェーズ（魂収集）
Phase.Dawn                   // 夜明けフェーズ（戦闘）
Phase.Transition             // フェーズ移行中

// プレイヤー状態タグ
State.Player.Normal          // 通常状態
State.Player.ReaperMode      // リーパーモード発動中
State.Player.Attacking       // 攻撃中
State.Player.Dead            // 死亡状態
State.Player.Invincible      // 無敵状態（リスポーン後等）

// 魂タイプタグ
Soul.Type.Power              // パワー魂（Tiger）
Soul.Type.Speed              // スピード魂（Horse）
Soul.Type.Guard              // ガード魂（Dog）
Soul.Type.Luck               // ラック魂（Chicken）
Soul.Type.Crit               // クリティカル魂（Deer）
Soul.Type.Regen              // リジェネ魂（Kitty）
Soul.Type.Wild               // ワイルド魂（Penguin）- 何でも代用可

// 敵タイプタグ
Enemy.Type.Melee             // 近接敵
Enemy.Type.Ranged            // 遠距離敵
Enemy.Type.Boss              // ボス敵

// ウェーブタグ
Wave.State.Spawning          // ウェーブスポーン中
Wave.State.InProgress        // ウェーブ進行中
Wave.State.Complete          // ウェーブ完了
Wave.State.Final             // 最終ウェーブ
```

---

## Project-Specific Patterns

### Soul Data Asset Structure
```cpp
/**
 * 魂タイプデータアセット
 */
UCLASS(BlueprintType)
class USoulDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 魂タイプタグ */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "魂")
    FGameplayTag SoulTypeTag;

    /** 魂名（日本語） */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "魂")
    FText SoulName;

    /** 対応する動物クラス */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "魂")
    TSubclassOf<AActor> AnimalClass;

    /** バフ効果（GameplayEffect） */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "効果")
    TSubclassOf<UGameplayEffect> BuffEffect;

    /** 基本バフ倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "効果", meta = (ClampMin = "0.0"))
    float BaseMultiplier = 0.05f;

    /** 魂アイコン */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UTexture2D> SoulIcon;

    /** 収集時VFX */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
    TObjectPtr<UNiagaraSystem> CollectionVFX;
};
```

### Soul Collection Pattern
```cpp
/**
 * 魂収集処理
 */
void USoulCollectionSubsystem::CollectSoul(AActor* Animal, AActor* Collector)
{
    if (!IsValid(Animal) || !IsValid(Collector))
    {
        return;
    }

    // 動物から魂タイプを取得
    USoulDataAsset* SoulData = GetSoulDataFromAnimal(Animal);
    if (!SoulData)
    {
        UE_LOG(LogSoulReaper, Warning, TEXT("魂データが見つかりません: %s"), *Animal->GetName());
        return;
    }

    // インベントリに追加
    FSoulInventoryItem& Item = SoulInventory.FindOrAdd(SoulData->SoulTypeTag);
    Item.Count++;

    // リーパーゲージを増加
    if (UReaperModeComponent* ReaperComp = Collector->FindComponentByClass<UReaperModeComponent>())
    {
        ReaperComp->AddGauge(SoulData->BaseMultiplier * 10.0f);
    }

    // 収集VFXを再生
    if (SoulData->CollectionVFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            SoulData->CollectionVFX,
            Animal->GetActorLocation()
        );
    }

    // イベント通知
    OnSoulCollected.Broadcast(SoulData->SoulTypeTag, Item.Count);

    UE_LOG(LogSoulReaper, Log, TEXT("魂を収集: %s (合計: %d)"),
        *SoulData->SoulName.ToString(), Item.Count);
}
```

### Reaper Mode Activation Pattern
```cpp
/**
 * リーパーモード発動処理
 */
void UReaperModeComponent::ActivateReaperMode()
{
    if (!CanActivate())
    {
        return;
    }

    bIsReaperModeActive = true;
    CurrentGauge = 0.0f;

    // Roarアニメーションを再生
    if (UAnimInstance* AnimInstance = GetOwnerAnimInstance())
    {
        AnimInstance->Montage_Play(RoarMontage);
    }

    // リーパーモードタグを付与
    if (UAbilitySystemComponent* ASC = GetOwnerASC())
    {
        ASC->AddLooseGameplayTag(Tag_State_ReaperMode);

        // ダメージ2倍のGameplayEffectを適用
        FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
        ASC->ApplyGameplayEffectToSelf(ReaperModeEffect.GetDefaultObject(), 1.0f, Context);
    }

    // VFX開始
    if (ReaperModeVFX)
    {
        ActiveVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            ReaperModeVFX,
            GetOwner()->GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true
        );
    }

    // 持続時間タイマー開始
    GetWorld()->GetTimerManager().SetTimer(
        ReaperModeTimerHandle,
        this,
        &UReaperModeComponent::DeactivateReaperMode,
        ReaperModeDuration,
        false
    );

    OnReaperModeActivated.Broadcast();
    UE_LOG(LogSoulReaper, Log, TEXT("リーパーモード発動！"));
}
```

### Wave Spawner Pattern
```cpp
/**
 * ウェーブスポーン処理
 */
void UWaveSpawnerSubsystem::StartWave(int32 WaveIndex)
{
    if (!WaveDataTable)
    {
        UE_LOG(LogSoulReaper, Error, TEXT("ウェーブデータテーブルが設定されていません"));
        return;
    }

    CurrentWaveIndex = WaveIndex;

    // ウェーブデータを取得
    FWaveData* WaveData = WaveDataTable->FindRow<FWaveData>(
        FName(*FString::Printf(TEXT("Wave_%d"), WaveIndex)),
        TEXT("WaveSpawner")
    );

    if (!WaveData)
    {
        UE_LOG(LogSoulReaper, Warning, TEXT("ウェーブデータが見つかりません: %d"), WaveIndex);
        return;
    }

    // スポーン状態を設定
    UGameplayStatics::GetGameInstance(this)->GetSubsystem<UGameplayTagSubsystem>()
        ->AddTag(Tag_Wave_Spawning);

    // 各敵タイプをスポーン
    RemainingEnemies = 0;
    for (const FEnemySpawnInfo& SpawnInfo : WaveData->EnemySpawns)
    {
        for (int32 i = 0; i < SpawnInfo.Count; i++)
        {
            // スポーンポイントを取得
            FVector SpawnLocation = GetRandomSpawnPoint();

            // 敵をスポーン
            AActor* Enemy = GetWorld()->SpawnActor<AActor>(
                SpawnInfo.EnemyClass,
                SpawnLocation,
                FRotator::ZeroRotator
            );

            if (Enemy)
            {
                // 死亡時コールバックを登録
                if (UHealthComponent* Health = Enemy->FindComponentByClass<UHealthComponent>())
                {
                    Health->OnDeath.AddDynamic(this, &UWaveSpawnerSubsystem::OnEnemyDeath);
                }
                RemainingEnemies++;
            }
        }
    }

    OnWaveStarted.Broadcast(WaveIndex, RemainingEnemies);
    UE_LOG(LogSoulReaper, Log, TEXT("ウェーブ %d 開始: 敵 %d 体"), WaveIndex, RemainingEnemies);
}
```

### Phase Transition Pattern
```cpp
/**
 * フェーズ移行処理（Night → Dawn）
 */
void ASoulReaperGameMode::TransitionToDawnPhase()
{
    // 移行中タグを設定
    SetPhaseTag(Tag_Phase_Transition);

    // 魂をバフに変換
    USoulCollectionSubsystem* SoulSystem = GetWorld()->GetSubsystem<USoulCollectionSubsystem>();
    if (SoulSystem)
    {
        SoulSystem->ConvertSoulsToBuffs(GetPlayerPawn());
    }

    // UI演出（フェーズ名表示等）
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (ASoulReaperHUD* HUD = Cast<ASoulReaperHUD>(PC->GetHUD()))
        {
            HUD->ShowPhaseTransition(TEXT("DAWN"), DawnPhaseColor);
        }
    }

    // 移行演出後にDawnフェーズ開始
    GetWorld()->GetTimerManager().SetTimer(
        PhaseTransitionHandle,
        [this]()
        {
            SetPhaseTag(Tag_Phase_Dawn);
            StartDawnPhase();
        },
        PhaseTransitionDuration,
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
