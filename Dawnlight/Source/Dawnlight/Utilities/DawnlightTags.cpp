// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightTags.h"

// ========================================================================
// ゲームフェーズタグ (Phase)
// ========================================================================

/** Night Phase - 魂狩猟フェーズ */
UE_DEFINE_GAMEPLAY_TAG(Phase_Night, "Phase.Night");

/** Dawn Transition - 夜明け移行 */
UE_DEFINE_GAMEPLAY_TAG(Phase_DawnTransition, "Phase.DawnTransition");

/** Dawn Phase - 戦闘フェーズ */
UE_DEFINE_GAMEPLAY_TAG(Phase_Dawn, "Phase.Dawn");

/** Loop End - ループ終了 */
UE_DEFINE_GAMEPLAY_TAG(Phase_LoopEnd, "Phase.LoopEnd");

// ========================================================================
// プレイヤー状態タグ (State.Player)
// ========================================================================

/** リーパーモード中 */
UE_DEFINE_GAMEPLAY_TAG(State_Player_ReaperMode, "State.Player.ReaperMode");

/** 攻撃中 */
UE_DEFINE_GAMEPLAY_TAG(State_Player_Attacking, "State.Player.Attacking");

/** 死亡 */
UE_DEFINE_GAMEPLAY_TAG(State_Player_Dead, "State.Player.Dead");

/** 無敵状態 */
UE_DEFINE_GAMEPLAY_TAG(State_Player_Invincible, "State.Player.Invincible");

// ========================================================================
// 魂タイプタグ (Soul)
// ========================================================================

/** Tiger Soul - パワー */
UE_DEFINE_GAMEPLAY_TAG(Soul_Type_Tiger, "Soul.Type.Tiger");

/** Horse Soul - スピード */
UE_DEFINE_GAMEPLAY_TAG(Soul_Type_Horse, "Soul.Type.Horse");

/** Dog Soul - ガード */
UE_DEFINE_GAMEPLAY_TAG(Soul_Type_Dog, "Soul.Type.Dog");

/** Chicken Soul - コモン */
UE_DEFINE_GAMEPLAY_TAG(Soul_Type_Chicken, "Soul.Type.Chicken");

/** Deer Soul - スピリット */
UE_DEFINE_GAMEPLAY_TAG(Soul_Type_Deer, "Soul.Type.Deer");

/** Kitty Soul - ラック */
UE_DEFINE_GAMEPLAY_TAG(Soul_Type_Kitty, "Soul.Type.Kitty");

/** Penguin Soul - レア */
UE_DEFINE_GAMEPLAY_TAG(Soul_Type_Penguin, "Soul.Type.Penguin");

// ========================================================================
// バフタグ (Buff)
// ========================================================================

/** ダメージバフ */
UE_DEFINE_GAMEPLAY_TAG(Buff_Damage, "Buff.Damage");

/** スピードバフ */
UE_DEFINE_GAMEPLAY_TAG(Buff_Speed, "Buff.Speed");

/** 防御バフ */
UE_DEFINE_GAMEPLAY_TAG(Buff_Defense, "Buff.Defense");

/** クールダウン短縮バフ */
UE_DEFINE_GAMEPLAY_TAG(Buff_Cooldown, "Buff.Cooldown");

/** ラックバフ */
UE_DEFINE_GAMEPLAY_TAG(Buff_Luck, "Buff.Luck");

/** 全ステータスバフ */
UE_DEFINE_GAMEPLAY_TAG(Buff_AllStats, "Buff.AllStats");

// ========================================================================
// 入力アクションタグ (Input)
// ========================================================================

/** 移動入力 */
UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");

/** 通常攻撃入力 */
UE_DEFINE_GAMEPLAY_TAG(Input_Action_LightAttack, "Input.Action.LightAttack");

/** 強攻撃入力 */
UE_DEFINE_GAMEPLAY_TAG(Input_Action_HeavyAttack, "Input.Action.HeavyAttack");

/** 特殊攻撃入力 */
UE_DEFINE_GAMEPLAY_TAG(Input_Action_SpecialAttack, "Input.Action.SpecialAttack");

/** リーパーモード入力 */
UE_DEFINE_GAMEPLAY_TAG(Input_Action_ReaperMode, "Input.Action.ReaperMode");

// ========================================================================
// 敵タグ (Enemy)
// ========================================================================

/** 近接敵 */
UE_DEFINE_GAMEPLAY_TAG(Enemy_Type_Melee, "Enemy.Type.Melee");

/** 遠距離敵 */
UE_DEFINE_GAMEPLAY_TAG(Enemy_Type_Ranged, "Enemy.Type.Ranged");

/** ミニボス */
UE_DEFINE_GAMEPLAY_TAG(Enemy_Type_MiniBoss, "Enemy.Type.MiniBoss");

/** ボス */
UE_DEFINE_GAMEPLAY_TAG(Enemy_Type_Boss, "Enemy.Type.Boss");

// ========================================================================
// 動物タグ (Animal)
// ========================================================================

/** Tiger */
UE_DEFINE_GAMEPLAY_TAG(Animal_Type_Tiger, "Animal.Type.Tiger");

/** Horse */
UE_DEFINE_GAMEPLAY_TAG(Animal_Type_Horse, "Animal.Type.Horse");

/** Dog */
UE_DEFINE_GAMEPLAY_TAG(Animal_Type_Dog, "Animal.Type.Dog");

/** Chicken */
UE_DEFINE_GAMEPLAY_TAG(Animal_Type_Chicken, "Animal.Type.Chicken");

/** Deer */
UE_DEFINE_GAMEPLAY_TAG(Animal_Type_Deer, "Animal.Type.Deer");

/** Kitty */
UE_DEFINE_GAMEPLAY_TAG(Animal_Type_Kitty, "Animal.Type.Kitty");

/** Penguin */
UE_DEFINE_GAMEPLAY_TAG(Animal_Type_Penguin, "Animal.Type.Penguin");
