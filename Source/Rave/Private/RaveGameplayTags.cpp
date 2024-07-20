


#include "RaveGameplayTags.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"

namespace RaveGameplayTags
{
	/** Ability Tags */
	UE_DEFINE_GAMEPLAY_TAG(Ability, "Ability");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Move, "Ability.Move");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Look, "Ability.Look");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact, "Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint, "Ability.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump, "Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Climb, "Ability.Climb");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Evade, "Ability.Evade");
	UE_DEFINE_GAMEPLAY_TAG(Ability_PrimaryAttack, "Ability.PrimaryAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_PrimaryAttackCharge, "Ability.PrimaryAttackCharge");
	UE_DEFINE_GAMEPLAY_TAG(Ability_SecondaryAttack, "Ability.SecondaryAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Identity, "Ability.Identity");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_1, "Ability.Skill_1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_2, "Ability.Skill_2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_3, "Ability.Skill_3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_4, "Ability.Skill_4");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Spell_1, "Ability.Spell_1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Spell_2, "Ability.Spell_2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Spell_3, "Ability.Spell_3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Spell_4, "Ability.Spell_4");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Spell_5, "Ability.Spell_5");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Module_1, "Ability.Module_1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Module_2, "Ability.Module_2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Module_3, "Ability.Module_3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Module_4, "Ability.Module_4");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Module_5, "Ability.Module_5");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Module_6, "Ability.Module_6");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Property_Cancellable, "Ability.Property.Cancellable");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Property_BlockMove, "Ability.Property.BlockMove");

	/** Input Tags */
	UE_DEFINE_GAMEPLAY_TAG(Input, "Input");
	UE_DEFINE_GAMEPLAY_TAG(Input_Move, "Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Look, "Input.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact, "Input.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_Sprint, "Input.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Input_Jump, "Input.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_Evade, "Input.Evade");
	UE_DEFINE_GAMEPLAY_TAG(Input_PrimaryAttack, "Input.PrimaryAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_SecondaryAttack, "Input.SecondaryAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Barrier, "Input.Barrier");
	UE_DEFINE_GAMEPLAY_TAG(Input_Identity, "Input.Identity");
	UE_DEFINE_GAMEPLAY_TAG(Input_Skill_1, "Input.Skill_1");
	UE_DEFINE_GAMEPLAY_TAG(Input_Skill_2, "Input.Skill_2");
	UE_DEFINE_GAMEPLAY_TAG(Input_Skill_3, "Input.Skill_3");
	UE_DEFINE_GAMEPLAY_TAG(Input_Skill_4, "Input.Skill_4");
	UE_DEFINE_GAMEPLAY_TAG(Input_Spell_1, "Input.Spell_1");
	UE_DEFINE_GAMEPLAY_TAG(Input_Spell_2, "Input.Spell_2");
	UE_DEFINE_GAMEPLAY_TAG(Input_Spell_3, "Input.Spell_3");
	UE_DEFINE_GAMEPLAY_TAG(Input_Spell_4, "Input.Spell_4");
	UE_DEFINE_GAMEPLAY_TAG(Input_Spell_5, "Input.Spell_5");
	UE_DEFINE_GAMEPLAY_TAG(Input_Module_1, "Input.Module_1");
	UE_DEFINE_GAMEPLAY_TAG(Input_Module_2, "Input.Module_2");
	UE_DEFINE_GAMEPLAY_TAG(Input_Module_3, "Input.Module_3");
	UE_DEFINE_GAMEPLAY_TAG(Input_Module_4, "Input.Module_4");
	UE_DEFINE_GAMEPLAY_TAG(Input_Module_5, "Input.Module_5");
	UE_DEFINE_GAMEPLAY_TAG(Input_Module_6, "Input.Module_6");
	UE_DEFINE_GAMEPLAY_TAG(Input_Escape, "Input.Escape");
	UE_DEFINE_GAMEPLAY_TAG(Input_CharacterSelect, "Input.CharacterSelect");

	/** GameplayEvent Tags */
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent, "GameplayEvent");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Collision_Begin, "GameplayEvent.Collision.Begin");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Collision_End, "GameplayEvent.Collision.End");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Combo_Begin, "GameplayEvent.Combo.Begin");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Combo_End, "GameplayEvent.Combo.End");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Block_Begin, "GameplayEvent.Block.Begin");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Block_End, "GameplayEvent.Block.End");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Sprint, "GameplayEvent.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Launch, "GameplayEvent.Launch");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Spawn, "GameplayEvent.Spawn");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Invoke, "GameplayEvent.Invoke");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Damage, "GameplayEvent.Damage");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death, "GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Land, "GameplayEvent.Land");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Knockdown, "GameplayEvent.Knockdown");

	/** GameplayEffect Tags */
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect, "GameplayEffect");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Damage, "GameplayEffect.Damage");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Healing, "GameplayEffect.Healing");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Cooldown, "GameplayEffect.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Force, "GameplayEffect.Force");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Stun, "GameplayEffect.Stun");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Property_RemoveByDamage, "GameplayEffect.Property.RemoveByDamage");

	/** Character Tags */
	UE_DEFINE_GAMEPLAY_TAG(Character, "Character");
	UE_DEFINE_GAMEPLAY_TAG(Character_BlockMove, "Character.BlockMove");
	UE_DEFINE_GAMEPLAY_TAG(Character_ZeroAcceleration, "Character.ZeroAcceleration");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Blocking, "Character.State.Blocking");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Evade, "Character.State.Evade");
	UE_DEFINE_GAMEPLAY_TAG(Character_Debuff_Stun, "Character.Debuff.Stun");

	/** Gameplay Data Tags */
	UE_DEFINE_GAMEPLAY_TAG(Data, "Data");
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Data.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Data_CooldownReduction, "Data.Healing");
	UE_DEFINE_GAMEPLAY_TAG(Data_Force, "Data.Force");
	UE_DEFINE_GAMEPLAY_TAG(Data_Cooldown, "Data.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(Data_Duration, "Data.Duration");
}
