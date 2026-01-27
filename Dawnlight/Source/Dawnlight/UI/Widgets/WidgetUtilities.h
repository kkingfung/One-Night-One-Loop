// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

/**
 * ウィジェット検索ユーティリティ
 *
 * MCP等のツールで作成されたウィジェットは名前に_0サフィックスが付く場合がある。
 * この関数は両方の名前パターンで検索を試みる。
 */
template<typename T>
T* FindWidgetByBaseName(UUserWidget* Owner, const FString& BaseName)
{
	if (!Owner || !Owner->WidgetTree)
	{
		return nullptr;
	}

	// まず正確な名前で検索
	if (T* Widget = Owner->WidgetTree->FindWidget<T>(FName(*BaseName)))
	{
		return Widget;
	}

	// _0サフィックス付きで検索
	FString NameWithSuffix = BaseName + TEXT("_0");
	if (T* Widget = Owner->WidgetTree->FindWidget<T>(FName(*NameWithSuffix)))
	{
		return Widget;
	}

	// _1, _2 など他のサフィックスも試す（念のため）
	for (int32 i = 1; i <= 5; ++i)
	{
		FString AltSuffix = FString::Printf(TEXT("%s_%d"), *BaseName, i);
		if (T* Widget = Owner->WidgetTree->FindWidget<T>(FName(*AltSuffix)))
		{
			return Widget;
		}
	}

	return nullptr;
}
