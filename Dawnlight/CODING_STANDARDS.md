# Dawnlight C++ Coding Standards

このドキュメントはDawnlightプロジェクトにおけるC++コーディング規約を定義します。
全ての開発者はこの規約に従ってコードを記述してください。

---

## 1. ファイル構成

### 1.1 ファイル命名規則

```
ヘッダファイル:    PascalCase.h
ソースファイル:   PascalCase.cpp
```

### 1.2 ファイル構造

**ヘッダファイル (.h)**
```cpp
// Copyright (c) 2025 Dawnlight. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

// 前方宣言
class UMyComponent;

/**
 * クラスの説明をここに記述
 */
UCLASS()
class DAWNLIGHT_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    // コンストラクタ
    AMyActor();

    // Public関数

protected:
    // Protected関数
    virtual void BeginPlay() override;

private:
    // Private関数

public:
    // Public変数・プロパティ

protected:
    // Protected変数・プロパティ

private:
    // Private変数・プロパティ
};
```

**ソースファイル (.cpp)**
```cpp
// Copyright (c) 2025 Dawnlight. All Rights Reserved.

#include "MyActor.h"
#include "Components/MyComponent.h"

AMyActor::AMyActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMyActor::BeginPlay()
{
    Super::BeginPlay();
}
```

### 1.3 インクルード順序

```cpp
// 1. 対応するヘッダ（cppファイルの場合）
#include "MyClass.h"

// 2. プロジェクト内ヘッダ
#include "Core/GameSubsystem.h"
#include "Characters/PlayerCharacter.h"

// 3. エンジンヘッダ
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

// 4. サードパーティヘッダ（必要な場合）
```

---

## 2. 命名規約

### 2.1 クラス接頭辞

| タイプ | 接頭辞 | 例 |
|--------|--------|-----|
| Actor | A | `ADawnlightCharacter` |
| UObject | U | `UInventoryComponent` |
| Interface (UE) | U/I | `UInteractable` / `IInteractable` |
| Struct | F | `FItemData` |
| Enum | E | `ECharacterState` |
| Template | T | `TArray`, `TMap` |
| Delegate | F...Delegate | `FOnHealthChanged` |
| Multicast Delegate | F...Signature | `FOnDeathSignature` |

### 2.2 変数命名

```cpp
// ブール値: b接頭辞
bool bIsAlive;
bool bCanJump;
bool bHasWeapon;

// メンバ変数: PascalCase
float MaxHealth;
int32 CurrentAmmo;
FString PlayerName;

// ポインタ: 型に含める
TObjectPtr<UActorComponent> MyComponent;  // UPROPERTY用
UActorComponent* TempComponent;            // ローカル用

// 配列: 複数形
TArray<FItemData> Items;
TArray<TObjectPtr<AActor>> SpawnedActors;

// マップ
TMap<FName, FItemData> ItemDatabase;

// 静的変数
static constexpr float DefaultHealth = 100.0f;
static const FName SocketName;
```

### 2.3 関数命名

```cpp
// 動詞で始める
void Initialize();
void UpdateHealth(float NewHealth);
void ProcessInput();

// ゲッター: Get接頭辞
float GetHealth() const;
FVector GetTargetLocation() const;

// セッター: Set接頭辞
void SetHealth(float NewHealth);
void SetTargetLocation(const FVector& Location);

// ブール判定: Is/Can/Has/Should接頭辞
bool IsAlive() const;
bool CanAttack() const;
bool HasItem(const FName& ItemId) const;
bool ShouldProcessDamage() const;

// イベントハンドラ: On接頭辞
void OnDamageTaken(float Damage);
void OnOverlapBegin(AActor* OtherActor);
void OnAnimationComplete();

// 内部ヘルパー: 動詞
void CalculateDamage();
void ValidateInput();
FVector ComputeTargetPosition();
```

### 2.4 定数・列挙型

```cpp
// 列挙型: E接頭辞、PascalCase値
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    Idle        UMETA(DisplayName = "待機"),
    Walking     UMETA(DisplayName = "歩行"),
    Running     UMETA(DisplayName = "走行"),
    Attacking   UMETA(DisplayName = "攻撃中"),
    Dead        UMETA(DisplayName = "死亡")
};

// 定数: constexprまたはconst
static constexpr int32 MaxInventorySlots = 50;
static const FName WeaponSocketName = TEXT("WeaponSocket");
```

---

## 3. フォーマット規約

### 3.1 インデント・スペース

```cpp
// タブは使用しない、スペース4つを使用
void MyFunction()
{
    if (bCondition)
    {
        // インデントはスペース4つ
        DoSomething();
    }
}

// 演算子の前後にスペース
int32 Result = A + B * C;
bIsValid = (Health > 0.0f) && (bIsAlive);

// カンマの後にスペース
FVector Location(100.0f, 200.0f, 0.0f);
MyFunction(Param1, Param2, Param3);

// キーワードと括弧の間にスペース
if (bCondition)
for (const auto& Item : Items)
while (bRunning)
```

### 3.2 ブレース配置

```cpp
// Allman style（新しい行にブレース）
void MyFunction()
{
    if (bCondition)
    {
        DoSomething();
    }
    else
    {
        DoSomethingElse();
    }
}

// 単一行でもブレースを使用
if (bCondition)
{
    return;
}

// ラムダは例外的にインラインも可
Items.RemoveAll([](const FItem& Item) { return Item.bShouldRemove; });

// 複数行ラムダ
Items.Sort([](const FItem& A, const FItem& B)
{
    return A.Priority > B.Priority;
});
```

### 3.3 行の長さ

- 1行は120文字以内を推奨
- 長い関数呼び出しは適切に改行

```cpp
// 長いパラメータリスト
void MyFunction(
    const FString& LongParameterName,
    float AnotherParameter,
    bool bYetAnotherParameter)
{
    // 処理
}

// 長い条件式
if (bFirstCondition &&
    bSecondCondition &&
    SomeValue > Threshold)
{
    // 処理
}
```

---

## 4. UE5特有の規約

### 4.1 UPROPERTY

```cpp
// カテゴリは日本語で統一
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ステータス")
float MaxHealth = 100.0f;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント")
TObjectPtr<USceneComponent> SceneRoot;

// meta指定子
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
float DamageMultiplier = 1.0f;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "参照", meta = (AllowedClasses = "/Script/Engine.StaticMesh"))
FSoftObjectPath MeshPath;

// レプリケーション
UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "ステータス")
float Health;
```

### 4.2 UFUNCTION

```cpp
// Blueprint公開関数
UFUNCTION(BlueprintCallable, Category = "アクション")
void PerformAttack();

UFUNCTION(BlueprintPure, Category = "ステータス")
float GetHealthPercent() const;

// イベント
UFUNCTION(BlueprintImplementableEvent, Category = "イベント")
void OnLevelUp(int32 NewLevel);

UFUNCTION(BlueprintNativeEvent, Category = "イベント")
void OnDeath();
virtual void OnDeath_Implementation();

// RPC
UFUNCTION(Server, Reliable, WithValidation, Category = "ネットワーク")
void ServerRequestAction(int32 ActionId);
bool ServerRequestAction_Validate(int32 ActionId);
void ServerRequestAction_Implementation(int32 ActionId);
```

### 4.3 デリゲート

```cpp
// ダイナミックマルチキャストデリゲート（Blueprint対応）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);

UPROPERTY(BlueprintAssignable, Category = "イベント")
FOnHealthChangedSignature OnHealthChanged;

// ネイティブデリゲート（C++のみ）
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDamageReceived, float, AActor*);
FOnDamageReceived OnDamageReceived;

// 使用例
OnHealthChanged.Broadcast(NewHealth);
OnDamageReceived.Broadcast(Damage, DamageCauser);
```

---

## 5. メモリ管理

### 5.1 スマートポインタ

```cpp
// UObject: UPROPERTYで管理
UPROPERTY()
TObjectPtr<UActorComponent> OwnedComponent;

// 非所有参照: TWeakObjectPtr
TWeakObjectPtr<AActor> TargetActor;
if (TargetActor.IsValid())
{
    TargetActor->DoSomething();
}

// 非UObject: TSharedPtr/TUniquePtr
TSharedPtr<FMyData> SharedData = MakeShared<FMyData>();
TUniquePtr<FMyData> UniqueData = MakeUnique<FMyData>();
```

### 5.2 GC考慮事項

```cpp
// UObjectはUPROPERTYで参照を維持
UPROPERTY()
TArray<TObjectPtr<UObject>> ManagedObjects;

// AddToRootは最終手段（通常使用しない）
// RemoveFromRootを忘れるとリークする

// IsValidでnullチェックとpendingkillチェック
if (IsValid(MyActor))
{
    MyActor->DoSomething();
}
```

---

## 6. エラー処理

### 6.1 アサーション

```cpp
// プログラミングエラー（全ビルドで致命的）
check(Pointer != nullptr);
checkf(Index < ArraySize, TEXT("Index %d out of bounds"), Index);

// 回復可能なエラー（ログ出力して継続）
if (!ensure(Pointer != nullptr))
{
    return;
}

// 副作用のある式（結果が必要）
verify(ImportantFunctionThatMustSucceed());

// 一度だけのensure
if (!ensureAlways(ShouldBeTrueOnce))
{
    HandleError();
}
```

### 6.2 ログ出力

```cpp
// ログカテゴリ定義
// .h
DECLARE_LOG_CATEGORY_EXTERN(LogDawnlight, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDawnlightAI, Log, All);

// .cpp
DEFINE_LOG_CATEGORY(LogDawnlight);
DEFINE_LOG_CATEGORY(LogDawnlightAI);

// 使用（日本語メッセージ）
UE_LOG(LogDawnlight, Log, TEXT("ゲームを初期化しています"));
UE_LOG(LogDawnlight, Warning, TEXT("アセットが見つかりません: %s"), *AssetPath);
UE_LOG(LogDawnlight, Error, TEXT("致命的なエラー: %s"), *ErrorMessage);
UE_LOG(LogDawnlight, Verbose, TEXT("デバッグ情報: 位置=%s"), *Location.ToString());
```

---

## 7. パフォーマンスガイドライン

### 7.1 一般原則

```cpp
// constを積極的に使用
float GetHealth() const { return Health; }
void ProcessData(const FMyStruct& Data);

// 参照渡しで不要なコピーを避ける
void ProcessItems(const TArray<FItemData>& Items);

// 戻り値最適化を活用
FVector CalculatePosition()
{
    return FVector(X, Y, Z);  // NRVOで最適化
}

// moveセマンティクス
TArray<FItemData> Items = MoveTemp(OtherItems);
```

### 7.2 避けるべきパターン

```cpp
// 悪い: Tickで毎フレーム検索
void Tick(float DeltaTime)
{
    // 毎フレームのFindComponent呼び出しは避ける
    auto* Comp = FindComponentByClass<UMyComponent>();
}

// 良い: BeginPlayでキャッシュ
void BeginPlay()
{
    CachedComponent = FindComponentByClass<UMyComponent>();
}

// 悪い: ループ内で文字列結合
FString Result;
for (const auto& Item : Items)
{
    Result += Item.Name;  // 毎回再アロケーション
}

// 良い: StringBuilder使用または事前予約
TStringBuilder<256> Builder;
for (const auto& Item : Items)
{
    Builder.Append(Item.Name);
}
```

---

## 8. コメント規約

### 8.1 ドキュメントコメント

```cpp
/**
 * キャラクターにダメージを与える
 *
 * このメソッドはダメージ計算、耐性適用、死亡判定を行います。
 * ダメージが適用されるとOnDamageTakenデリゲートがブロードキャストされます。
 *
 * @param BaseDamage 基本ダメージ量
 * @param DamageType ダメージタイプ（物理、魔法など）
 * @param Instigator ダメージを与えたアクター
 * @return 実際に適用されたダメージ量
 */
float ApplyDamage(float BaseDamage, EDamageType DamageType, AActor* Instigator);
```

### 8.2 インラインコメント

```cpp
void UpdateCombatState()
{
    // 現在のターゲットが有効かチェック
    if (!IsValid(CurrentTarget))
    {
        // ターゲットを失った場合、索敵状態に移行
        SetCombatState(ECombatState::Searching);
        return;
    }

    // 距離に基づいて攻撃可能か判定
    const float DistanceToTarget = FVector::Distance(GetActorLocation(), CurrentTarget->GetActorLocation());

    // 攻撃範囲内であれば攻撃状態に移行
    if (DistanceToTarget <= AttackRange)
    {
        SetCombatState(ECombatState::Attacking);
    }
}
```

### 8.3 TODOコメント

```cpp
// TODO: マルチプレイヤー対応を追加する
// FIXME: 稀にnullptrクラッシュが発生する問題
// HACK: 暫定対応、後でリファクタリングが必要
// NOTE: このパラメータはデザイナーからの要望で追加
```

---

## 9. ファイルテンプレート

### 9.1 Actorクラス

```cpp
// DawnlightCharacter.h
// Copyright (c) 2025 Dawnlight. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DawnlightCharacter.generated.h"

class UHealthComponent;
class UInventoryComponent;

/**
 * Dawnlightの基本キャラクタークラス
 *
 * プレイヤーおよびNPCの基底クラスとして機能します。
 * 体力管理、インベントリ、基本的な戦闘機能を提供します。
 */
UCLASS(Abstract)
class DAWNLIGHT_API ADawnlightCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADawnlightCharacter();

    /** 体力コンポーネントを取得 */
    UFUNCTION(BlueprintPure, Category = "コンポーネント")
    UHealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
    virtual void BeginPlay() override;

    /** ダメージを受けた時の処理 */
    UFUNCTION()
    void HandleDamageTaken(float Damage, AActor* DamageCauser);

private:
    /** 体力管理コンポーネント */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHealthComponent> HealthComponent;

    /** インベントリコンポーネント */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "コンポーネント", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInventoryComponent> InventoryComponent;
};
```

### 9.2 Componentクラス

```cpp
// HealthComponent.h
// Copyright (c) 2025 Dawnlight. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageTakenSignature, float, Damage, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

/**
 * 体力管理コンポーネント
 *
 * アクターの体力、ダメージ処理、死亡判定を管理します。
 */
UCLASS(ClassGroup = (Dawnlight), meta = (BlueprintSpawnableComponent))
class DAWNLIGHT_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    /** ダメージを適用する */
    UFUNCTION(BlueprintCallable, Category = "体力")
    float ApplyDamage(float Damage, AActor* DamageCauser = nullptr);

    /** 体力を回復する */
    UFUNCTION(BlueprintCallable, Category = "体力")
    float Heal(float Amount);

    /** 現在の体力を取得 */
    UFUNCTION(BlueprintPure, Category = "体力")
    float GetHealth() const { return CurrentHealth; }

    /** 最大体力を取得 */
    UFUNCTION(BlueprintPure, Category = "体力")
    float GetMaxHealth() const { return MaxHealth; }

    /** 生存しているか */
    UFUNCTION(BlueprintPure, Category = "体力")
    bool IsAlive() const { return CurrentHealth > 0.0f; }

    /** 体力変更イベント */
    UPROPERTY(BlueprintAssignable, Category = "イベント")
    FOnHealthChangedSignature OnHealthChanged;

    /** ダメージ受信イベント */
    UPROPERTY(BlueprintAssignable, Category = "イベント")
    FOnDamageTakenSignature OnDamageTaken;

    /** 死亡イベント */
    UPROPERTY(BlueprintAssignable, Category = "イベント")
    FOnDeathSignature OnDeath;

protected:
    virtual void BeginPlay() override;

private:
    /** 最大体力 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "設定", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
    float MaxHealth = 100.0f;

    /** 現在の体力 */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "状態", meta = (AllowPrivateAccess = "true"))
    float CurrentHealth;

    /** 死亡済みフラグ */
    bool bIsDead = false;
};
```

---

## 10. レビューチェックリスト

コードレビュー時に確認する項目:

- [ ] 命名規約に従っているか
- [ ] コメントは日本語で記述されているか
- [ ] UPROPERTYのカテゴリは適切か
- [ ] メモリ管理は適切か（リーク、ダングリングポインタ）
- [ ] エラー処理は適切か
- [ ] パフォーマンスに問題はないか
- [ ] コードの可読性は十分か
- [ ] 不要なコードや TODO は残っていないか

---

**最終更新**: 2025-01-15
**対象エンジン**: Unreal Engine 5.7
