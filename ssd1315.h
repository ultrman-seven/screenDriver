#ifndef B373E681_0D71_4176_A3F6_D827B95A1817
#define B373E681_0D71_4176_A3F6_D827B95A1817

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
} Ssd1315I2cCfg_t;

typedef struct
{
    uint8_t fsm;
    // uint8_t targetFsm;
    // uint8_t cmdIdx;
    uint8_t busBusy;
    uint8_t cmdBuf[8];
    Ssd1315I2cCfg_t cfg;
    const OledPic_t *pic;
    // uint8_t *picDat;
    struct
    {
        void *arg;
        void (*func)(void *);
    } cbk;
} Ssd1315I2cHandle_t;

#define SSD1315_I2C_AddrBase 0x3c

#define SSD1315_MaxPage 8
#define SSD1315_MaxCol 128

enum
{
    SSD1315_ErrCode_Success = 0,
    SSD1315_ErrCode_BusBusy,
    SSD1315_ErrCode_BusErr,
    SSD1315_ErrCode_PicOutOfRange,
    SSD1315_ErrCode_PicFormatErr,
};
void ssd1315_i2cInitAsync(Ssd1315I2cHandle_t *handle, Ssd1315I2cCfg_t *cfg, void (*cbk)(void *), void *arg);
uint8_t ssd1315_i2cDisplayCtrlOnOffAsync(Ssd1315I2cHandle_t *handle, uint8_t on, void (*cbk)(void *), void *arg);
uint8_t ssd1315_i2cDisplayCtrlReverseAsync(Ssd1315I2cHandle_t *handle, uint8_t reverse, void (*cbk)(void *), void *arg);
uint8_t ssd1315_i2cDisplayCtrlRotateAsync(Ssd1315I2cHandle_t *handle, uint8_t rotate, void (*cbk)(void *), void *arg);
uint8_t ssd1315_WritePicAsync(Ssd1315I2cHandle_t *handle, uint8_t startPage, uint8_t startCol, const OledPic_t *pic, void (*cbk)(void *), void *arg);
void ssd1315_Loop(Ssd1315I2cHandle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* B373E681_0D71_4176_A3F6_D827B95A1817 */
