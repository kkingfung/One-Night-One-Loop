// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ViewModelBase.generated.h"

/**
 * プロパティ変更通知デリゲート（ViewModel用）
 *
 * ViewModelのプロパティが変更された時にViewに通知
 * 注: FOnPropertyChangedはEngine側で定義済みのため、別名を使用
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewModelPropertyChanged, FName, PropertyName);

/**
 * 全プロパティ変更通知デリゲート（ViewModel用）
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnViewModelAllPropertiesChanged);

/**
 * ViewModelの基底クラス
 *
 * MVVMパターンにおけるViewModelの基本機能を提供
 * - プロパティ変更通知
 * - ライフサイクル管理
 * - Viewとの疎結合
 *
 * 使用方法:
 * 1. このクラスを継承してViewModelを作成
 * 2. プロパティをUPROPERTYで定義
 * 3. プロパティ変更時にNotifyPropertyChanged()を呼び出し
 * 4. ViewはOnPropertyChangedにバインドして変更を監視
 */
UCLASS(Abstract, BlueprintType)
class DAWNLIGHT_API UViewModelBase : public UObject
{
	GENERATED_BODY()

public:
	UViewModelBase();
	virtual ~UViewModelBase();

	// ========================================================================
	// プロパティ変更通知
	// ========================================================================

	/**
	 * プロパティが変更された時のデリゲート
	 * Viewはこのデリゲートを購読してUIを更新
	 */
	UPROPERTY(BlueprintAssignable, Category = "ViewModel|イベント")
	FOnViewModelPropertyChanged OnViewModelPropertyChanged;

	/**
	 * 全プロパティの更新が必要な時のデリゲート
	 * 初期化時やリセット時に使用
	 */
	UPROPERTY(BlueprintAssignable, Category = "ViewModel|イベント")
	FOnViewModelAllPropertiesChanged OnAllPropertiesChanged;

	// ========================================================================
	// ライフサイクル
	// ========================================================================

	/**
	 * ViewModelを初期化
	 * サブシステムやサービスへの参照を取得
	 */
	UFUNCTION(BlueprintCallable, Category = "ViewModel")
	virtual void Initialize(UWorld* InWorld);

	/**
	 * ViewModelを破棄
	 * イベント購読の解除やリソースの解放
	 */
	UFUNCTION(BlueprintCallable, Category = "ViewModel")
	virtual void Deinitialize();

	/**
	 * 初期化済みかどうか
	 */
	UFUNCTION(BlueprintPure, Category = "ViewModel")
	bool IsInitialized() const { return bIsInitialized; }

	/**
	 * 関連付けられたWorldを取得
	 */
	UFUNCTION(BlueprintPure, Category = "ViewModel")
	UWorld* GetViewModelWorld() const { return WorldContext.Get(); }

protected:
	// ========================================================================
	// プロパティ変更ヘルパー
	// ========================================================================

	/**
	 * プロパティ変更を通知
	 *
	 * @param PropertyName 変更されたプロパティ名
	 */
	UFUNCTION(BlueprintCallable, Category = "ViewModel|内部")
	void NotifyPropertyChanged(FName PropertyName);

	/**
	 * 全プロパティの変更を通知
	 * 初期化時やリセット時に使用
	 */
	UFUNCTION(BlueprintCallable, Category = "ViewModel|内部")
	void NotifyAllPropertiesChanged();

	/**
	 * プロパティを設定し、変更があれば通知
	 *
	 * @param CurrentValue 現在の値への参照
	 * @param NewValue 新しい値
	 * @param PropertyName プロパティ名
	 * @return 値が変更されたらtrue
	 */
	template<typename T>
	bool SetProperty(T& CurrentValue, const T& NewValue, FName PropertyName)
	{
		if (CurrentValue != NewValue)
		{
			CurrentValue = NewValue;
			NotifyPropertyChanged(PropertyName);
			return true;
		}
		return false;
	}

	// ========================================================================
	// 状態
	// ========================================================================

	/** 初期化済みフラグ */
	bool bIsInitialized;

	/** World参照（WeakPtr） */
	TWeakObjectPtr<UWorld> WorldContext;
};
