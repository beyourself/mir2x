/*
 * =====================================================================================
 *
 *       Filename: onsmhc.cpp
 *        Created: 02/23/2016 00:09:59
 *  Last Modified: 05/31/2016 21:56:08
 *
 *    Description: 
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */
#include "log.hpp"
#include "game.hpp"
#include "xmlconf.hpp"
#include "message.hpp"
#include "processrun.hpp"

void Game::OperateHC(uint8_t nHC)
{
    switch(nHC){
        case SM_PING:           OnPing();      break;
        case SM_LOGINOK:        OnLoginOK();   break;
        case SM_LOGINFAIL:      OnLoginFail(); break;
        default: break;
    }

    m_NetIO.ReadHC([&](uint8_t nHC){ OperateHC(nHC); });
}

void Game::OnPing()
{
    extern Log *g_Log;
    g_Log->AddLog(LOGTYPE_INFO, "on ping");
}

void Game::OnLoginOK()
{
    auto fnDoLoginOK = [this](const uint8_t *pBuf, size_t nLen){
        SwitchProcess(m_CurrentProcess->ID(), PROCESSID_RUN);
        auto pRun = (ProcessRun *)m_CurrentProcess;

        extern Log *g_Log;
        if(pRun){
            pRun->Net_LoginOK(pBuf, nLen);
            g_Log->AddLog(LOGTYPE_INFO, "login succeed");
        }else{
            g_Log->AddLog(LOGTYPE_INFO, "failed to jump into main loop");
        }
    };

    Read(sizeof(SMLoginOK), fnDoLoginOK);
}

void Game::OnLoginFail()
{
    extern Log *g_Log;
    g_Log->AddLog(LOGTYPE_INFO, "login failed");
}

void Game::OnMotionState()
{
    auto fnMotionState = [this](const uint8_t *pBuf, size_t nLen){
        // 1. receive object motion state update while game is not in running state
        //    do we need to inform server?
        if(!(m_CurrentProcess && m_CurrentProcess->ID() == PROCESSID_RUN)){ return; }

        // 2. ok we are in running state
        auto pRun = (ProcessRun *)m_CurrentProcess;
        pRun->Net_MotionState(pBuf, nLen);
    };

    Read(sizeof(SMLoginOK), fnMotionState);
}