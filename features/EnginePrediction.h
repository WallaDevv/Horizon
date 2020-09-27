#pragma once
#include "../hooks/Hooks.h"
class CEnginePrediction : public Singleton<CEnginePrediction>
{
public:
    void Start(CUserCmd* cmd, IBasePlayer* local);
    void Finish(IBasePlayer* local);
    int     m_nButtons;
    bool m_bEnginePaused;
private:
    CMoveData data;
    struct {
        float curtime, frametime;
        int tickcount, tickbase;
    }old_vars;
};