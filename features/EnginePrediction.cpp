#include "EnginePrediction.h"
#include "C:\Users\gerar\source\repos\Horizon\hooks\Hooks.h"
#include "AntiAims.h"
#include "C:\Users\gerar\source\repos\Horizon\checksum_md5.h"
void CEnginePrediction::Start(CUserCmd* cmd, IBasePlayer* local) {
    float flPrev = interfaces.global_vars->curtime;
    static ConVar* weapon_recoil_scale = interfaces.cvars->FindVar(("weapon_recoil_scale"));
    static ConVar* view_recoil_tracking = interfaces.cvars->FindVar(("view_recoil_tracking"));
    csgo->local->GetPunchAngle()* (weapon_recoil_scale ? weapon_recoil_scale->GetFloat() : 2.f);
    old_vars.curtime = interfaces.global_vars->curtime;
    old_vars.frametime = interfaces.global_vars->frametime;
    old_vars.tickcount = interfaces.global_vars->tickcount;
    interfaces.global_vars->curtime = TICKS_TO_TIME(local->GetTickBase()) - TICK_INTERVAL;
    interfaces.global_vars->frametime = m_bEnginePaused ? 0 : TICK_INTERVAL;
    interfaces.global_vars->tickcount = TIME_TO_TICKS(interfaces.global_vars->curtime);
    interfaces.global_vars->curtime = TICKS_TO_TIME(local->GetTickBase()) - TICK_INTERVAL;
    interfaces.global_vars->frametime = m_bEnginePaused ? 0 : TICK_INTERVAL;
    interfaces.game_movement->StartTrackPredictionErrors(local);
    interfaces.move_helper->SetHost(local);
    interfaces.global_vars->curtime = flPrev;
    m_nButtons = cmd->buttons;
    int buttonsChanged = cmd->buttons ^ *reinterpret_cast<int*>(uintptr_t(local) + 0x31E8);
    *reinterpret_cast<int*>(uintptr_t(local) + 0x31DC) = (uintptr_t(local) + 0x31E8);
    *reinterpret_cast<int*>(uintptr_t(local) + 0x31E8) = cmd->buttons;
    interfaces.game_movement->StartTrackPredictionErrors(local);
    memset(&data, 0, sizeof(data));
    interfaces.prediction->SetupMove(local, cmd, interfaces.move_helper, &data);
    interfaces.game_movement->ProcessMovement(local, &data);
    interfaces.prediction->FinishMove(local, cmd, &data);
    interfaces.global_vars->curtime = flPrev;
}
void CEnginePrediction::Finish(IBasePlayer* local) {
    interfaces.game_movement->FinishTrackPredictionErrors(local);
    interfaces.move_helper->SetHost(0);
    interfaces.global_vars->curtime = old_vars.curtime;
    interfaces.global_vars->frametime = old_vars.frametime;
}