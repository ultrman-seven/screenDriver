#include "ssd1312.h"

#define SSD1312_CtrlByte_Cmd 0x00
#define SSD1312_CtrlByte_GraphicDisplayDRAM 0x40
#define SSD1312_CtrlByte_ContinueEn 0x00
#define SSD1312_CtrlByte_ContinueDis 0x80

#define SSD1312_Cmd_PageMode_SetStartPage 0xb0
#define SSD1312_Cmd_PageMode_SetColLow 0x00
#define SSD1312_Cmd_PageMode_SetColHigh 0x10

#define SSD1312_Cmd_SetMemAddressing 0x20

#define SSD1312_MemAddressingMode_Horizontal 0x00
#define SSD1312_MemAddressingMode_Vertical 0x01
#define SSD1312_MemAddressingMode_Page 0x02

#define SSD1312_Cmd_SetCol 0x21
#define SSD1312_Cmd_SetPage 0x22

#define SSD1312_Cmd_DisplayOnOff 0xae
#define SSD1312_Cmd_DisplayReverse 0xa6

enum
{
    Ssd1312_I2cFsm_PwrOn = 0,
    Ssd1312_I2cFsm_Boot,
    Ssd1312_I2cFsm_Init,
    Ssd1312_I2cFsm_SetMulCom,
    Ssd1312_I2cFsm_Standby,
    Ssd1312_I2cFsm_Idle,
    // Ssd1312_I2cFsm_BusWrite,
    Ssd1312_I2cFsm_CmdWritePrePic,
    Ssd1312_I2cFsm_PicWrite,
    Ssd1312_I2cFsm_CmdWriteWaitEnd,
    Ssd1312_I2cFsm_,
};

static void ssd1312_i2cTxEndCbk(void *handle)
{
    ((Ssd1312I2cHandle_t *)handle)->busBusy = 0;
}

#define __ssd1312_i2cWrite(_hd, _dat, _len, _stop)                                            \
    do                                                                                        \
    {                                                                                         \
        _hd->busBusy = 1;                                                                     \
        _hd->cfg.i2cWriteAsync((_hd)->cfg.addr, _dat, _len, _stop, ssd1312_i2cTxEndCbk, _hd); \
    } while (0)

#define _ssd1312_i2cWriteWithStop(_hd, _dat, _len) __ssd1312_i2cWrite(_hd, _dat, _len, 1)
#define _ssd1312_i2cWriteWithoutStop(_hd, _dat, _len) __ssd1312_i2cWrite(_hd, _dat, _len, 0)

void ssd1312_i2cInitAsync(Ssd1312I2cHandle_t *handle, Ssd1312I2cCfg_t *cfg, void (*cbk)(void *), void *arg)
{
    handle->cfg.i2cWriteAsync = cfg->i2cWriteAsync;
    handle->cfg.addr = cfg->addr;
    handle->cfg.pageNum = cfg->pageNum;
    handle->fsm = Ssd1312_I2cFsm_Boot;
    handle->cbk.func = cbk;
    handle->cbk.arg = arg;
}

uint8_t ssd1312_i2cDisplayCtrlOnOffAsync(Ssd1312I2cHandle_t *handle, uint8_t on, void (*cbk)(void *), void *arg)
{
    if (handle->fsm != Ssd1312_I2cFsm_Idle)
        return SSD1312_ErrCode_BusBusy;

    handle->fsm = Ssd1312_I2cFsm_CmdWriteWaitEnd;
    handle->cmdBuf[0] = SSD1312_CtrlByte_Cmd | SSD1312_CtrlByte_ContinueEn;
    handle->cmdBuf[1] = SSD1312_Cmd_DisplayOnOff + (on ? 1 : 0);
    _ssd1312_i2cWriteWithStop(handle, handle->cmdBuf, 2);

    handle->cbk.func = cbk;
    handle->cbk.arg = arg;
    return SSD1312_ErrCode_Success;
}

uint8_t ssd1312_i2cDisplayCtrlReverseAsync(Ssd1312I2cHandle_t *handle, uint8_t reverse, void (*cbk)(void *), void *arg)
{
    if (handle->fsm != Ssd1312_I2cFsm_Idle)
        return SSD1312_ErrCode_BusBusy;

    handle->fsm = Ssd1312_I2cFsm_CmdWriteWaitEnd;
    handle->cmdBuf[0] = SSD1312_CtrlByte_Cmd | SSD1312_CtrlByte_ContinueEn;
    handle->cmdBuf[1] = SSD1312_Cmd_DisplayReverse + (reverse ? 1 : 0);
    _ssd1312_i2cWriteWithStop(handle, handle->cmdBuf, 2);

    handle->cbk.func = cbk;
    handle->cbk.arg = arg;
    return SSD1312_ErrCode_Success;
}

uint8_t ssd1312_i2cDisplayCtrlRotateAsync(Ssd1312I2cHandle_t *handle, uint8_t rotate, void (*cbk)(void *), void *arg)
{
    if (handle->fsm != Ssd1312_I2cFsm_Idle)
        return SSD1312_ErrCode_BusBusy;

    handle->fsm = Ssd1312_I2cFsm_CmdWriteWaitEnd;
    handle->cmdBuf[0] = SSD1312_CtrlByte_Cmd | SSD1312_CtrlByte_ContinueEn;
    if (rotate)
    {
        handle->cmdBuf[1] = 0xc0;
        handle->cmdBuf[2] = 0xa0;
    }
    else
    {
        handle->cmdBuf[1] = 0xc8;
        handle->cmdBuf[2] = 0xa1;
    }
    _ssd1312_i2cWriteWithStop(handle, handle->cmdBuf, 3);

    handle->cbk.func = cbk;
    handle->cbk.arg = arg;
    return SSD1312_ErrCode_Success;
}

static const uint8_t
    Ssd1312InitCmds[] = {
        SSD1312_CtrlByte_Cmd | SSD1312_CtrlByte_ContinueEn,
        0xAE, /*display off*/
        // 设置对比度
        0x81,
        0x4f, /*128*/

        SSD1312_Cmd_SetMemAddressing,
        // SSD1312_MemAddressingMode_Horizontal,
        SSD1312_MemAddressingMode_Vertical,

        0xA1, /*set segment remap*/
        0xA6, /*normal / reverse*/

        // 0xA8, /*multiplex ratio*/
        // 0x1F, /*duty = 1/32*/

        0xC8, /*Com scan direction*/

        0xD3, /*set display offset*/
        0x00,

        0xD5, /*set osc division*/
        0x80,

        0xD9, /*set pre-charge period*/
        0x1f,

        0xDA, /*set COM pins*/
        0x12,

        0xdb, /*set vcomh*/
        0x40,

        0x8d, /*set charge pump enable*/
        0x14};

static const uint8_t Ssd1312InitCmdLen = sizeof(Ssd1312InitCmds);

void ssd1312_Loop(Ssd1312I2cHandle_t *handle)
{
    switch (handle->fsm)
    {
    case Ssd1312_I2cFsm_Boot:
        // handle->cmdIdx = 0;
        // handle->fsm = Ssd1312_I2cFsm_Init;
        handle->busBusy = 0;
        //     break;
        // case Ssd1312_I2cFsm_Init:
        _ssd1312_i2cWriteWithStop(handle, Ssd1312InitCmds, Ssd1312InitCmdLen);
        handle->fsm = Ssd1312_I2cFsm_SetMulCom;
        break;
    case Ssd1312_I2cFsm_SetMulCom:
        if (handle->busBusy)
            break;
        handle->cmdBuf[0] = SSD1312_CtrlByte_Cmd | SSD1312_CtrlByte_ContinueEn;
        handle->cmdBuf[1] = 0xA8;                             /*multiplex ratio*/
        handle->cmdBuf[2] = ((handle->cfg.pageNum) << 3) - 1; /*duty = 1/32*/
        _ssd1312_i2cWriteWithoutStop(handle, handle->cmdBuf, 3);

        handle->fsm = Ssd1312_I2cFsm_CmdWriteWaitEnd;
        break;
    case Ssd1312_I2cFsm_CmdWritePrePic:
        if (handle->busBusy)
            break;
        if (handle->pic)
        {
            handle->cmdBuf[0] = SSD1312_CtrlByte_GraphicDisplayDRAM | SSD1312_CtrlByte_ContinueEn;
            _ssd1312_i2cWriteWithoutStop(handle, handle->cmdBuf, 1);
            handle->fsm = Ssd1312_I2cFsm_PicWrite;
        }
        else
            handle->fsm = Ssd1312_I2cFsm_Idle;
        break;
    case Ssd1312_I2cFsm_PicWrite:
        if (handle->busBusy)
            break;
        {
            uint32_t dl;
            dl = (handle->pic->pixelWidth * handle->pic->pixelHeight) >> 3; // / 8;
            _ssd1312_i2cWriteWithStop(handle, handle->pic->dataPtr, dl);
            handle->fsm = Ssd1312_I2cFsm_CmdWriteWaitEnd;
        }
        break;
    case Ssd1312_I2cFsm_CmdWriteWaitEnd:
        if (handle->busBusy)
            break;
        if (handle->cbk.func)
            handle->cbk.func(handle->cbk.arg);
        handle->fsm = Ssd1312_I2cFsm_Idle;
        break;
    default:
        break;
    }
}

uint8_t ssd1312_WritePicAsync(Ssd1312I2cHandle_t *handle, uint8_t startPage, uint8_t startCol, const OledPic_t *pic, void (*cbk)(void *), void *arg)
{
    uint8_t picPageEnd;
    uint8_t picColEnd;
    if (handle->fsm != Ssd1312_I2cFsm_Idle)
        return SSD1312_ErrCode_BusBusy;
    if (startPage >= (handle->cfg.pageNum))
        return SSD1312_ErrCode_PicOutOfRange;
    if (startCol >= SSD1312_MaxCol)
        return SSD1312_ErrCode_PicOutOfRange;
    // if ((pic->pixelHeight % 8))
    if ((pic->pixelHeight) & 0x07)
        return SSD1312_ErrCode_PicFormatErr;
    // picPageEnd = (pic->pixelHeight) / 8;
    picPageEnd = ((pic->pixelHeight) >> 3) + startPage - 1;
    if ((picPageEnd) >= (handle->cfg.pageNum))
        return SSD1312_ErrCode_PicOutOfRange;
    picColEnd = startCol + pic->pixelWidth - 1;
    if ((picColEnd) >= SSD1312_MaxCol)
        return SSD1312_ErrCode_PicOutOfRange;

    handle->fsm = Ssd1312_I2cFsm_CmdWritePrePic;
    handle->cmdBuf[0] = SSD1312_CtrlByte_Cmd | SSD1312_CtrlByte_ContinueEn;
    handle->cmdBuf[1] = SSD1312_Cmd_SetPage;
    handle->cmdBuf[2] = startPage;
    handle->cmdBuf[3] = picPageEnd;
    handle->cmdBuf[4] = SSD1312_Cmd_SetCol;
    handle->cmdBuf[5] = startCol;
    handle->cmdBuf[6] = picColEnd;

    _ssd1312_i2cWriteWithStop(handle, handle->cmdBuf, 7);

    handle->cbk.func = cbk;
    handle->cbk.arg = arg;
    handle->pic = pic;
    return SSD1312_ErrCode_Success;
}
