#ifndef D84564C6_351D_4302_B405_8A9FB254BC36
#define D84564C6_351D_4302_B405_8A9FB254BC36

#include "stdint.h"
#include "oledPicType.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t addr;
    uint8_t pageNum;

    // @brief i2c发送数据接口
    // @param addr 7位从机地址
    // @param data 要发送数据的指针
    // @param dataLen 数据长度
    // @param sendStop 是否发送停止位
    // @param cb 回调函数
    // @param arg 回调函数参数
    void (*i2cWriteAsync)(uint8_t addr, uint8_t *data, uint32_t dataLen, uint8_t sendStop, void (*cb)(void *), void *arg);

    // void (*gpioWritePin)(uint8_t pinId, uint8_t pinLevel);
} Ssd1312I2cCfg_t;

typedef struct
{
    uint8_t fsm;
    // uint8_t targetFsm;
    // uint8_t cmdIdx;
    uint8_t busBusy;
    uint8_t cmdBuf[8];
    Ssd1312I2cCfg_t cfg;
    const OledPic_t *pic;
    // uint8_t *picDat;
    struct
    {
        void *arg;
        void (*func)(void *);
    } cbk;
} Ssd1312I2cHandle_t;

#define SSD1312_I2C_AddrBase 0x3c

#define SSD1312_MaxPage 8
#define SSD1312_MaxCol 128

enum
{
    SSD1312_ErrCode_Success = 0,
    SSD1312_ErrCode_BusBusy,
    SSD1312_ErrCode_BusErr,
    SSD1312_ErrCode_PicOutOfRange,
    SSD1312_ErrCode_PicFormatErr,
};
void ssd1312_i2cInitAsync(Ssd1312I2cHandle_t *handle, Ssd1312I2cCfg_t *cfg, void (*cbk)(void *), void *arg);
uint8_t ssd1312_i2cDisplayCtrlOnOffAsync(Ssd1312I2cHandle_t *handle, uint8_t on, void (*cbk)(void *), void *arg);
uint8_t ssd1312_i2cDisplayCtrlReverseAsync(Ssd1312I2cHandle_t *handle, uint8_t reverse, void (*cbk)(void *), void *arg);
uint8_t ssd1312_i2cDisplayCtrlRotateAsync(Ssd1312I2cHandle_t *handle, uint8_t rotate, void (*cbk)(void *), void *arg);
uint8_t ssd1312_WritePicAsync(Ssd1312I2cHandle_t *handle, uint8_t startPage, uint8_t startCol, const OledPic_t *pic, void (*cbk)(void *), void *arg);
void ssd1312_Loop(Ssd1312I2cHandle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* D84564C6_351D_4302_B405_8A9FB254BC36 */
