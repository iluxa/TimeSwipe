/*
This Source Code Form is subject to the terms of the GNU General Public License v3.0.
If a copy of the GPL was not distributed with this
file, You can obtain one at https://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2019 Panda Team
*/

#include "nodeControl.h"
#include "DataVis.h"


nodeControl::nodeControl()
{
    m_pMesChans.reserve(4);
    Instance().m_PersistStorage.AddItem(this->shared_from_this());
}

void nodeControl::Serialize(CStorage &st)
{
    m_OffsetSearch.Serialize(st);
    if(st.IsDefaultSettingsOrder())
    {
    //    SetGain(typeADgain::gainX1);
      //  SetUBRvoltage(false);
    }

    //st.ser( *((int*)&m_CurGain) ).ser(m_UBRVoltage);

    if(st.IsDownloading())
    {
      //  SetGain(m_CurGain);
        //SetUBRvoltage(m_UBRVoltage);
    }
}

void nodeControl::Update()
{
    for(auto &el : m_pMesChans) el->Update();

    m_PersistStorage.Update();
    m_OffsetSearch.Update();
}
void nodeControl::StartRecord(bool how)
{
    //make a stamp:
    static unsigned long count_mark=0;
    count_mark++;

    //generate an event:
    nlohmann::json v=count_mark;
    Instance().Fire_on_event("Record", v);
}

int nodeControl::gain_out(int val)
{
    //update channels gain setting:
    float gval=val;
    for(auto &el : m_pMesChans) el->SetAmpGain(gval);


     //set old IEPE gain:
     if(typeBoard::IEPEBoard==m_BoardType)
     {
         int gset=val-1;
         m_pGain1pin->Set(gset>>1);
         m_pGain0pin->Set(gset&1);
     }


     //generate an event:
     nlohmann::json v=val;
     Instance().Fire_on_event("Gain", v);

     return val;
}
bool nodeControl::GetBridge()
{
    assert(m_pUBRswitch);
    return m_pUBRswitch->Get();

}
void nodeControl::SetBridge(bool how)
{
     assert(m_pUBRswitch);
     m_pUBRswitch->Set(how);


    //generate an event:
    nlohmann::json v=how;
    Instance().Fire_on_event("Bridge", v);
}

void nodeControl::SetSecondary(int nMode)
{
    nMode&=1; //fit the value

    assert(m_pUBRswitch);
    m_pUBRswitch->Set(nMode);

    //generate an event:
    nlohmann::json v=nMode;
    Instance().Fire_on_event("Mode", v);

}
int nodeControl::GetSecondary()
{
    return GetBridge();
}


void nodeControl::SetMode(int nMode)
{
    m_OpMode=static_cast<MesModes>(nMode);
    if(m_OpMode<MesModes::IEPE) { m_OpMode=MesModes::IEPE; }
    if(m_OpMode>MesModes::Normsignal){ m_OpMode=MesModes::Normsignal; }

    SetSecondary(m_OpMode);
}
int nodeControl::GetMode()
{
    return GetSecondary();
}


void nodeControl::SetOffset(int nOffs)
{
    switch(nOffs)
    {
        case 1: //negative
            m_OffsetSearch.Start(4000);
        break;

        case 2: //zero
            m_OffsetSearch.Start();
        break;

        case 3: //positive
            m_OffsetSearch.Start(100);
        break;

        default:
            nOffs=0;
            m_OffsetSearch.StopReset();
        return;
    }

    nlohmann::json v=nOffs;
    Instance().Fire_on_event("Offset", v);
}
