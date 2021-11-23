#ifndef __DLA_H__
#define __DLA_H__

#include "xparameters.h"
#include "xil_types.h"

#define DLA_EN_POS 0
#define DLA_EN_Msk (0x1 << DLA_EN_POS)

#define DLA_RAM_RW_POS 1
#define DLA_RAM_RW_Msk (0x1 << DLA_RAM_RW_POS)

#define DLA_T_CONV_PARAM_RAM_SEL_POS 2
#define DLA_T_CONV_PARAM_RAM_SEL_Msk (0x1 << DLA_T_CONV_PARAM_RAM_SEL_POS)

#define DLA_DW_CONV_PARAM_RAM_SEL_POS 3
#define DLA_DW_CONV_PARAM_RAM_SEL_Msk (0x1 << DLA_DW_CONV_PARAM_RAM_SEL_POS)

#define DLA_PW_CONV_PARAM_RAM_SEL_POS 4
#define DLA_PW_CONV_PARAM_RAM_SEL_Msk (0x1 << DLA_PW_CONV_PARAM_RAM_SEL_POS)

#define DLA_FMAP_RAM_SEL_POS 5
#define DLA_FMAP_RAM_SEL_Msk (0x1 << DLA_FMAP_RAM_SEL_POS)

#define DLA_RESULT_RAM_SEL_POS 6
#define DLA_RESULT_RAM_SEL_Msk (0x1 << DLA_RESULT_RAM_SEL_POS)

#define DLA_T_CONV_CAL_POS 7
#define DLA_T_CONV_CAL_Msk (0x1 << DLA_T_CONV_CAL_POS)

#define DLA_DW_CONV_CAL_POS 8
#define DLA_DW_CONV_CAL_Msk (0x1 << DLA_DW_CONV_CAL_POS)

#define DLA_PW_CONV_CAL_POS 9
#define DLA_PW_CONV_CAL_Msk (0x1 << DLA_PW_CONV_CAL_POS)

#define DLA_DW_PW_CONV_CAL_POS 10
#define DLA_DW_PW_CONV_CAL_Msk (0x1 << DLA_DW_PW_CONV_CAL_POS)

#define DLA_PING_PONG_SEL_POS 11
#define DLA_PING_PONG_SEL_Msk (0x1 << DLA_PING_PONG_SEL_POS)

#define DLA_IS_BUSY_POS 1
#define DLA_IS_BUSY_Msk (0x3 << DLA_IS_BUSY_POS)


#define MOBILENET_S00_AXI_SLV_STATE_REG_OFFSET      0
#define MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET     4
#define MOBILENET_S00_AXI_SLV_F_W_OFFSET            8
#define MOBILENET_S00_AXI_SLV_F_H_OFFSET            12
#define MOBILENET_S00_AXI_SLV_STRIDE_OFFSET         16
#define MOBILENET_S00_AXI_SLV_K_W_OFFSET            20
#define MOBILENET_S00_AXI_SLV_K_H_OFFSET            24
#define MOBILENET_S00_AXI_SLV_CHANNEL_OFFSET        28
#define MOBILENET_S00_AXI_SLV_N_OFFSET              32

#define MOBILENET_BASEADDR                          XPAR_MOBILENET_0_S00_AXI_BASEADDR

#define RAM_READ 0
#define RAM_WRITE 1

#define T_CONV_RAM 0
#define DW_CONV_RAM 1
#define PW_CONV_RAM 2
#define FMAP_RAM 3
#define RESULT_RAM 4

#define T_CONV_CAL 0
#define DW_CONV_CAL 1
#define PW_CONV_CAL 2
#define DW_PW_CONV_CAL 3

#define PINGPONG_0 0
#define PINGPONG_1 1

class dla
{
private:
    bool isEnable;
public:
    dla();
    ~dla();

    void enable();
    
    void disable();

    void reset();

    bool isBusy();

    void setRAM_RW(u8 RW);

    void RAMSelect(u8 sel);
    void RAMUnselect();

    void startCal(u8 calType);
    void unsetCal();

    void setPingPong(u8 Sel);
    void unsetPingPong();

    /**
    ** For ARM-9 registers reading and writing.
    */
    void writeReg(int offset, int data);

    unsigned int readReg(int offset);

};

#endif