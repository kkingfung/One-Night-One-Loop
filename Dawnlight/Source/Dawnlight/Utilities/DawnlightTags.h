// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * Soul Reaper Gameplay Tags 定義
 *
 * ゲーム全体で使用されるタグを一元管理
 */

// ========================================================================
// ゲームフェーズタグ (Phase)
// ========================================================================

/** Night Phase - 魂狩猟フェーズ */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Phase_Night);

/** Dawn Transition - 夜明け移行 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Phase_DawnTransition);

/** Dawn Phase - 戦闘フェーズ */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Phase_Dawn);

/** Loop End - ループ終了 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Phase_LoopEnd);

// ========================================================================
// プレイヤー状態タグ (State.Player)
// ========================================================================

/** リーパーモード中 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Player_ReaperMode);

/** 攻撃中 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Player_Attacking);

/** 死亡 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Player_Dead);

/** 無敵状態 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Player_Invincible);

// ========================================================================
// 魂タイプタグ (Soul)
// ========================================================================

/** Tiger Soul - パワー */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Soul_Type_Tiger);

/** Horse Soul - スピード */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Soul_Type_Horse);

/** Dog Soul - ガード */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Soul_Type_Dog);

/** Chicken Soul - コモン */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Soul_Type_Chicken);

/** Deer Soul - スピリット */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Soul_Type_Deer);

/** Kitty Soul - ラック */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Soul_Type_Kitty);

/** Penguin Soul - レア */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Soul_Type_Penguin);

// ========================================================================
// バフタグ (Buff)
// ========================================================================

/** ダメージバフ */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buff_Damage);

/** スピードバフ */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buff_Speed);

/** 防御バフ */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buff_Defense);

/** クールダウン短縮バフ */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buff_Cooldown);

/** ラックバフ */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buff_Luck);

/** 全ステータスバフ */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buff_AllStats);

// ========================================================================
// 入力アクションタグ (Input)
// ========================================================================

/** 移動入力 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);

/** 通常攻撃入力 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LightAttack);

/** 強攻撃入力 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_HeavyAttack);

/** 特殊攻撃入力 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_SpecialAttack);

/** リーパーモード入力 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_ReaperMode);

// ========================================================================
// 敵タグ (Enemy)
// ========================================================================

/** 近接敵 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_Melee);

/** 遠距離敵 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_Ranged);

/** ミニボス */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_MiniBoss);

/** ボス */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Type_Boss);

// ========================================================================
// 動物タグ (Animal)
// ========================================================================

/** Tiger */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animal_Type_Tiger);

/** Horse */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animal_Type_Horse);

/** Dog */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animal_Type_Dog);

/** Chicken */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animal_Type_Chicken);

/** Deer */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animal_Type_Deer);

/** Kitty */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animal_Type_Kitty);

/** Penguin */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animal_Type_Penguin);

// ========================================================================
// SoulReaperTags 名前空間（便利なアクセス用）
// ========================================================================
namespace SoulReaperTags
{
	// フェーズタグ
	static const FGameplayTag& Phase_Night = ::Phase_Night;
	static const FGameplayTag& Phase_DawnTransition = ::Phase_DawnTransition;
	static const FGameplayTag& Phase_Dawn = ::Phase_Dawn;
	static const FGameplayTag& Phase_LoopEnd = ::Phase_LoopEnd;

	// プレイヤー状態タグ
	static const FGameplayTag& State_Player_ReaperMode = ::State_Player_ReaperMode;
	static const FGameplayTag& State_Player_Attacking = ::State_Player_Attacking;
	static const FGameplayTag& State_Player_Dead = ::State_Player_Dead;
	static const FGameplayTag& State_Player_Invincible = ::State_Player_Invincible;

	// 魂タイプタグ
	static const FGameplayTag& Soul_Type_Tiger = ::Soul_Type_Tiger;
	static const FGameplayTag& Soul_Type_Horse = ::Soul_Type_Horse;
	static const FGameplayTag& Soul_Type_Dog = ::Soul_Type_Dog;
	static const FGameplayTag& Soul_Type_Chicken = ::Soul_Type_Chicken;
	static const FGameplayTag& Soul_Type_Deer = ::Soul_Type_Deer;
	static const FGameplayTag& Soul_Type_Kitty = ::Soul_Type_Kitty;
	static const FGameplayTag& Soul_Type_Penguin = ::Soul_Type_Penguin;

	// バフタグ
	static const FGameplayTag& Buff_Damage = ::Buff_Damage;
	static const FGameplayTag& Buff_Speed = ::Buff_Speed;
	static const FGameplayTag& Buff_Defense = ::Buff_Defense;
	static const FGameplayTag& Buff_Cooldown = ::Buff_Cooldown;
	static const FGameplayTag& Buff_Luck = ::Buff_Luck;
	static const FGameplayTag& Buff_AllStats = ::Buff_AllStats;

	// 入力タグ
	static const FGameplayTag& Input_Action_Move = ::Input_Action_Move;
	static const FGameplayTag& Input_Action_LightAttack = ::Input_Action_LightAttack;
	static const FGameplayTag& Input_Action_HeavyAttack = ::Input_Action_HeavyAttack;
	static const FGameplayTag& Input_Action_SpecialAttack = ::Input_Action_SpecialAttack;
	static const FGameplayTag& Input_Action_ReaperMode = ::Input_Action_ReaperMode;

	// 敵タグ
	static const FGameplayTag& Enemy_Type_Melee = ::Enemy_Type_Melee;
	static const FGameplayTag& Enemy_Type_Ranged = ::Enemy_Type_Ranged;
	static const FGameplayTag& Enemy_Type_MiniBoss = ::Enemy_Type_MiniBoss;
	static const FGameplayTag& Enemy_Type_Boss = ::Enemy_Type_Boss;

	// 動物タグ
	static const FGameplayTag& Animal_Type_Tiger = ::Animal_Type_Tiger;
	static const FGameplayTag& Animal_Type_Horse = ::Animal_Type_Horse;
	static const FGameplayTag& Animal_Type_Dog = ::Animal_Type_Dog;
	static const FGameplayTag& Animal_Type_Chicken = ::Animal_Type_Chicken;
	static const FGameplayTag& Animal_Type_Deer = ::Animal_Type_Deer;
	static const FGameplayTag& Animal_Type_Kitty = ::Animal_Type_Kitty;
	static const FGameplayTag& Animal_Type_Penguin = ::Animal_Type_Penguin;
}

// 旧タグの後方互換性（コンパイル通す用、後で削除可能）
namespace DawnlightTags
{
	static const FGameplayTag& State_Player_Hidden = ::State_Player_Invincible;
	static const FGameplayTag& State_Player_Photographing = ::State_Player_Attacking;
	static const FGameplayTag& State_Player_Detected = ::State_Player_Dead;
	static const FGameplayTag& State_Player_Safe = ::State_Player_Invincible;
}
