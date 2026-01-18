// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DawnlightAttributeSet.generated.h"

// マクロ定義：属性アクセサ
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Dawnlight 属性セット
 *
 * GAS用の属性定義
 * - 検知レベル
 * - 緊張度
 * - 夜の進行度
 */
UCLASS()
class DAWNLIGHT_API UDawnlightAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDawnlightAttributeSet();

	// ========================================================================
	// UAttributeSet インターフェース
	// ========================================================================

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// ========================================================================
	// 監視属性
	// ========================================================================

	/** 検知レベル（0.0 - 100.0） */
	UPROPERTY(BlueprintReadOnly, Category = "監視", ReplicatedUsing = OnRep_DetectionLevel)
	FGameplayAttributeData DetectionLevel;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, DetectionLevel)

	/** 監視レベルによる危険度（0.0 - 100.0） */
	UPROPERTY(BlueprintReadOnly, Category = "監視", ReplicatedUsing = OnRep_SurveillanceDanger)
	FGameplayAttributeData SurveillanceDanger;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, SurveillanceDanger)

	// ========================================================================
	// 緊張度属性
	// ========================================================================

	/** 緊張度（0.0 - 100.0） */
	UPROPERTY(BlueprintReadOnly, Category = "緊張度", ReplicatedUsing = OnRep_TensionLevel)
	FGameplayAttributeData TensionLevel;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, TensionLevel)

	// ========================================================================
	// 進行度属性
	// ========================================================================

	/** 夜の進行度（0.0 - 1.0） */
	UPROPERTY(BlueprintReadOnly, Category = "進行", ReplicatedUsing = OnRep_NightProgress)
	FGameplayAttributeData NightProgress;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, NightProgress)

	// ========================================================================
	// 撮影属性
	// ========================================================================

	/** 撮影カウント */
	UPROPERTY(BlueprintReadOnly, Category = "撮影", ReplicatedUsing = OnRep_PhotoCount)
	FGameplayAttributeData PhotoCount;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, PhotoCount)

	/** 撮影可能な最大数 */
	UPROPERTY(BlueprintReadOnly, Category = "撮影", ReplicatedUsing = OnRep_MaxPhotoCount)
	FGameplayAttributeData MaxPhotoCount;
	ATTRIBUTE_ACCESSORS(UDawnlightAttributeSet, MaxPhotoCount)

protected:
	// ========================================================================
	// レプリケーション
	// ========================================================================

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_DetectionLevel(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SurveillanceDanger(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_TensionLevel(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_NightProgress(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_PhotoCount(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxPhotoCount(const FGameplayAttributeData& OldValue);

private:
	/** 属性値をクランプ */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
