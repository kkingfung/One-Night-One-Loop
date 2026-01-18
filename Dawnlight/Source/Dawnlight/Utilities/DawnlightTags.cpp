// Copyright Epic Games, Inc. All Rights Reserved.

#include "DawnlightTags.h"

// 状態タグ
UE_DEFINE_GAMEPLAY_TAG(State_Player_Hidden, "State.Player.Hidden");
UE_DEFINE_GAMEPLAY_TAG(State_Player_Photographing, "State.Player.Photographing");
UE_DEFINE_GAMEPLAY_TAG(State_Player_Detected, "State.Player.Detected");
UE_DEFINE_GAMEPLAY_TAG(State_Player_Safe, "State.Player.Safe");

// フェーズタグ
UE_DEFINE_GAMEPLAY_TAG(Phase_Night_Introduction, "Phase.Night.Introduction");
UE_DEFINE_GAMEPLAY_TAG(Phase_Night_Relaxation, "Phase.Night.Relaxation");
UE_DEFINE_GAMEPLAY_TAG(Phase_Night_Climax, "Phase.Night.Climax");

// 固定イベントタグ
UE_DEFINE_GAMEPLAY_TAG(Event_Fixed_FirstLight, "Event.Fixed.FirstLight");
UE_DEFINE_GAMEPLAY_TAG(Event_Fixed_LeftBehind, "Event.Fixed.LeftBehind");
UE_DEFINE_GAMEPLAY_TAG(Event_Fixed_SystemAwareness, "Event.Fixed.SystemAwareness");

// ランダムイベントタグ
UE_DEFINE_GAMEPLAY_TAG(Event_Random_PatrolLight, "Event.Random.PatrolLight");
UE_DEFINE_GAMEPLAY_TAG(Event_Random_ReactingShadow, "Event.Random.ReactingShadow");
UE_DEFINE_GAMEPLAY_TAG(Event_Random_BrokenTerminal, "Event.Random.BrokenTerminal");
UE_DEFINE_GAMEPLAY_TAG(Event_Random_StrangeNoise, "Event.Random.StrangeNoise");

// 監視レベルタグ
UE_DEFINE_GAMEPLAY_TAG(Surveillance_Level_Low, "Surveillance.Level.Low");
UE_DEFINE_GAMEPLAY_TAG(Surveillance_Level_Medium, "Surveillance.Level.Medium");
UE_DEFINE_GAMEPLAY_TAG(Surveillance_Level_High, "Surveillance.Level.High");
UE_DEFINE_GAMEPLAY_TAG(Surveillance_Level_Critical, "Surveillance.Level.Critical");

// 入力タグ
UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
UE_DEFINE_GAMEPLAY_TAG(Input_Action_Photograph, "Input.Action.Photograph");
UE_DEFINE_GAMEPLAY_TAG(Input_Action_Hide, "Input.Action.Hide");
UE_DEFINE_GAMEPLAY_TAG(Input_Action_Interact, "Input.Action.Interact");

// アビリティタグ
UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Photograph, "Ability.Action.Photograph");
UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Hide, "Ability.Action.Hide");
UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Interact, "Ability.Action.Interact");
