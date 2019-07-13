/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
Copyright (c) 2019 Panda Team
*/

#include <iostream>

using namespace std;

#include "console.h"
#include "bcmspi.h"
#include "BSC_SLV_SPI.h"
#include "frm_stream.h"

void Wait(unsigned long time_mS);
int main(int argc, char *argv[])
{
    int nSPI=0;
    bool bMasterMode=true;
    if(argc>1)
    {
        nSPI=atoi(argv[1]);
        if(0!=nSPI && 1!=nSPI)
        {
            std::cout<<"Wrong SPI number: must be 0 or 1!"<<std::endl;
            return 0;
        }
    }
    if(argc>2)
    {
        if(*argv[2]!='s')
        {
            std::cout<<"Unrecognized key: must be s!"<<std::endl;
            return 0;
        }
        bMasterMode=false;
        if(1!=nSPI)
        {
            std::cout<<"Only SPI1 can work in a slave mode!"<<std::endl;
            return 0;
        }
    }

    std::cout<<"+++SPI terminal+++"<<std::endl;
    if(bMasterMode){

    CBcmSPI     spi(nSPI ? CBcmLIB::iSPI::SPI1  :  CBcmLIB::iSPI::SPI0);
    if(!spi.is_initialzed())
    {
        std::cout<<"Failed to initialize BCM SPI-"<<nSPI<<"Master. Try sudo"<<std::endl;
        return 0;
    }

    CNixConsole cio;
    CFIFO  msg;
    CFIFO answer;

     std::cout<<"SPI-"<<nSPI<<" Master"<<std::endl<<"type the commands:"<<std::endl<<"->"<<std::endl;
    while(true)
    {
        if(cio.receive(msg))
        {
            spi.send(msg);
            if(spi.receive(answer))
            {
                cio.send(answer);
            }
            else
            {
                switch(spi.m_ComCntr.get_state())
                {
                    case CSyncSerComFSM::errLine:
                        std::cout<<"!Line_err!";
                    break;

                    case CSyncSerComFSM::errTimeout:
                        std::cout<<"!Timeout_err!";
                    break;
                }
            }
            cout<<std::endl<<"->"<<std::endl;
        }
    } }
    else {

        //slave mode:
        CBSCslaveSPI spi;

        if(!spi.is_initialzed())
        {
            std::cout<<"Failed to initialize BCM SPI-"<<nSPI<<"Slave. Try sudo"<<std::endl;
            return 0;
        }

        CNixConsole cio;
        CFIFO  msg;

       std::cout<<"SPI-"<<nSPI<<" Slave"<<std::endl<<"listening messages:"<<std::endl<<"-> ";
       while(true)
       {
           if(spi.receive(msg))
           {
			 /*  CFIFO answer;
               CFrmStream out(&answer);
               out<<"received:"<<msg.in_avail()<<"\n";
               spi.send(answer);*/
			   
               cio.send(msg);
               std::cout<<std::endl<<"-> ";
           }
           //Wait(1); //????
       }
    }

    return 0;
}
