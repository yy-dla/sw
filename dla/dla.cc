#include "dla.h"
#include "xil_io.h"

dla::dla(/* args */)
{

}

dla::~dla()
{

}

void dla::enable(){
    writeReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET, 
                readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_EN_Msk);
}

void dla::disable(){
    writeReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET, 
                readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) & ~(DLA_EN_Msk));
}

void dla::reset(){
    writeReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET, 0x0);
}

bool dla::isBusy(){
    return (bool)(
        (
            readReg(MOBILENET_S00_AXI_SLV_STATE_REG_OFFSET) & DLA_IS_BUSY_Msk) 
            != 0
        );
};

void dla::setRAM_RW(u8 RW){
    if(RW == 1){  // RW = 1: Write
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_RAM_RW_Msk
        );
    }
    else if(RW == 0){
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) & ~(DLA_RAM_RW_Msk)
        );
    }
}

void dla::RAMSelect(u8 sel){
    switch (sel)
    {
    case T_CONV_RAM:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_T_CONV_PARAM_RAM_SEL_Msk
        );
        break;
    case DW_CONV_RAM:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_DW_CONV_PARAM_RAM_SEL_Msk
        );
        break;
    case PW_CONV_RAM:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_PW_CONV_PARAM_RAM_SEL_Msk
        );
        break;
    case FMAP_RAM:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_FMAP_RAM_SEL_Msk
        );
        break;
    case RESULT_RAM:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_RESULT_RAM_SEL_Msk
        );
        break;
    default:
        throw "Invalid RAM selection.";
        return;
        break;
    }
}

void dla::RAMUnselect(){
    writeReg(
        MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
        readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) & ~(
            DLA_T_CONV_PARAM_RAM_SEL_Msk |
            DLA_DW_CONV_PARAM_RAM_SEL_Msk |
            DLA_PW_CONV_PARAM_RAM_SEL_Msk |
            DLA_FMAP_RAM_SEL_Msk |
            DLA_RESULT_RAM_SEL_Msk
        )
    );
}

void dla::startCal(u8 calType){
    switch (calType)
    {
    case T_CONV_CAL:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_T_CONV_CAL_Msk
        );
        break;
    case DW_CONV_CAL:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_DW_CONV_CAL_Msk
        );
        break;
    case PW_CONV_CAL:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_PW_CONV_CAL_Msk
        );
        break;
    case DW_PW_CONV_CAL:
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | DLA_DW_PW_CONV_CAL_Msk
        );
        break;
    
    default:
        throw "Invalid calcuation type.";
        break;
    }
}

void dla::unsetCal(){
     writeReg(
        MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
        readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) & ~(
            DLA_T_CONV_CAL_Msk |
            DLA_DW_CONV_CAL_Msk |
            DLA_PW_CONV_CAL_Msk |
            DLA_DW_PW_CONV_CAL_Msk
        )
    );
}

void dla::setPingPong(u8 sel){
    if(sel == PINGPONG_0)
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) & ~(DLA_PING_PONG_SEL_Msk)
        );
    else if(sel == PINGPONG_1)
        writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) | (DLA_PING_PONG_SEL_Msk)
        );
}
void dla::unsetPingPong(){
    writeReg(
            MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET,
            readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) & ~(DLA_PING_PONG_SEL_Msk)
        );
}

void dla::writeReg(int offset, int data){
    Xil_Out32((MOBILENET_BASEADDR + offset), (unsigned int)data);
}

unsigned int dla::readReg(int offset){
    return Xil_In32(MOBILENET_BASEADDR + offset);
}
