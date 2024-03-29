#include "display.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "remote.h"
#include "eeprom.h"

#include "rtc.h"

static uint8_t pos;                                 /* Current position in framebuffer */

static uint8_t scrBuf[ROWS];                        /* Screen buffer */
static uint8_t newBuf[ROWS];                        /* NExt screen buffer */

static volatile uint8_t cmdBuf;
static volatile int8_t encCnt;
static volatile uint8_t stateBtnEnc;                /* Buttons and encoder raw state */

uint16_t displayTime;

static volatile uint16_t rtcTimer;
static volatile uint16_t rcTimer;

static uint8_t rcType;
static uint8_t rcAddr;
static uint8_t rcCode[CMD_RC_END];                  /* Array with RC commands */
static uint8_t rcIndex = 0;                         /* Index of RC command being learned */

static int8_t brStby;                               /* Brightness in standby mode */
static int8_t brWork;                               /* Brightness in working mode */

static const uint8_t font_dig_3x5[] PROGMEM = {
    0x00, 0x00, 0x00, // space
    0x00, 0x04, 0x04, // minus
    0x1F, 0x11, 0x1F, // 0
    0x12, 0x1F, 0x10, // 1
    0x1D, 0x15, 0x17, // 2
    0x15, 0x15, 0x1F, // 3
    0x07, 0x04, 0x1F, // 4
    0x17, 0x15, 0x1D, // 5
    0x1F, 0x15, 0x1D, // 6
    0x01, 0x01, 0x1F, // 7
    0x1F, 0x15, 0x1F, // 8
    0x17, 0x15, 0x1F, // 9
    0x1E, 0x05, 0x1F, // A
    0x1F, 0x15, 0x1B, // B
    0x0E, 0x11, 0x11, // C
    0x1F, 0x11, 0x0E, // D
    0x1F, 0x15, 0x11, // E
    0x1F, 0x05, 0x01, // F
};

static void matrixShowDig(uint8_t dig)              /* Show decimal digit */
{
    uint8_t i;

    for (i = 0; i < 3; i++) {
        if (pos < ROWS) {
            newBuf[pos] &= 0xE0;
            newBuf[pos] |= pgm_read_byte(font_dig_3x5 + dig * 3 + i);
            pos++;
        }
    }
    if (pos < ROWS)
        newBuf[pos++] &= 0xE0;

    return;
}

static void matrixSetPos(int8_t value)
{
    pos = value;

    return;
}

static void matrixShowDecimal(int8_t value)         /* Show decimal number */
{
    uint8_t neg = 0;

    if (value < 0) {
        neg = 1;
        value = -value;
    }
    if (value / 10) {
        matrixShowDig(neg);
        matrixShowDig(SYM_NUMBERS + value / 10);
    } else {
        matrixShowDig(SYM_SPACE);
        matrixShowDig(neg);
    }
    matrixShowDig(SYM_NUMBERS + value % 10);

    return;
}

static void matrixShowHex(uint8_t value, uint8_t twoChars)
{
    if (twoChars)
        matrixShowDig(SYM_NUMBERS + (value / 16));
    matrixShowDig(SYM_NUMBERS + (value % 16));

    return;
}

static void matrixShowBar(int8_t value)             /* Show asimmetric bar 0..16 */
{
    uint8_t i;
    uint8_t buf;

    for (i = 0; i < 16; i++) {
        buf = newBuf[i] & ~0xC0;
        if (value > i)
            buf |= 0xC0;
        newBuf[i] = buf;
    }

    return;
}

static void matrixShowSymBar(int8_t value)
{
    uint8_t i;
    uint8_t buf;

    for (i = 0; i < 16; i++) {
        buf = newBuf[i] & ~0xC0;
        if (value) {
            if ((i < 8) == (value < i - 7))
                buf |= 0xC0;
        } else {
            if (i == 7 || i == 8)
                buf |= 0x80;
        }
        newBuf[i] = buf;
    }

    return;
}

static CmdID rcCmdIndex(uint8_t rcCmd)
{
    CmdID i;

    for (i = 0; i < CMD_RC_END; i++)
        if (rcCmd == rcCode[i])
            return i;

    return CMD_RC_END;
}

static void rcCodesInit(void)
{
    rcType = eeprom_read_byte((uint8_t *)EEPROM_RC_TYPE);
    rcAddr = eeprom_read_byte((uint8_t *)EEPROM_RC_ADDR);
    eeprom_read_block(rcCode, (uint8_t *)EEPROM_RC_CMD, CMD_RC_END);

    return;
}

static void dispUpdate(void)
{
    static uint8_t row;                             /* Current row being scanned */

    row <<= 1;
    if (!row)
        row = 0x01;

    if (row & 0x80)
        PORT(REG_DATA) |= REG_DATA_LINE;
    else
        PORT(REG_DATA) &= ~REG_DATA_LINE;

    // Strob 250ns on F_CPU 8MHz
    PORT(REG_CLK) |= REG_CLK_LINE;
    PORT(REG_CLK) &= ~REG_CLK_LINE;

    uint8_t portD, portC, portB;
    portD = portC = portB = 0;

    if (scrBuf[0] & row) portB |= ROW_01_LINE;
    if (scrBuf[1] & row) portD |= ROW_02_LINE;
    if (scrBuf[2] & row) portB |= ROW_03_LINE;
    if (scrBuf[3] & row) portD |= ROW_04_LINE;
    if (scrBuf[4] & row) portB |= ROW_05_LINE;
    if (scrBuf[5] & row) portD |= ROW_06_LINE;
    if (scrBuf[6] & row) portB |= ROW_07_LINE;
    if (scrBuf[7] & row) portD |= ROW_08_LINE;
    if (scrBuf[8] & row) portB |= ROW_09_LINE;
    if (scrBuf[9] & row) portB |= ROW_10_LINE;
    if (scrBuf[10] & row) portB |= ROW_11_LINE;
    if (scrBuf[11] & row) portC |= ROW_12_LINE;
    if (scrBuf[12] & row) portC |= ROW_13_LINE;
    if (scrBuf[13] & row) portB |= ROW_14_LINE;
    if (scrBuf[14] & row) portC |= ROW_15_LINE;
    if (scrBuf[15] & row) portC |= ROW_16_LINE;

    PORT(ROW_08_06_04_02) |= portD;
    PORT(ROW_16_15_13_12) |= portC;
    PORT(ROW_14_11_10_09_07_05_03_01) |= portB;

    /* Update buttons state */
    if (PIN(BUTTON) & BUTTON_LINE)
        stateBtnEnc &= ~row;
    else
        stateBtnEnc |= row;

    return;
}

ISR (TIMER2_COMP_vect)
{
    // Switch off current line

    PORT(ROW_08_06_04_02) &= ~ROW_08_06_04_02_LINE;
    PORT(ROW_16_15_13_12) &= ~ROW_16_15_13_12_LINE;
    PORT(ROW_14_11_10_09_07_05_03_01) &= ~ROW_14_11_10_09_07_05_03_01_LINE;

    return;
}

ISR (TIMER2_OVF_vect)
{
    dispUpdate();

    uint8_t btnNow;
    static uint8_t btnPrev = BTN_STATE_0;
    static int16_t btnCnt = 0;                      /* Buttons press duration value */

    uint8_t encNow;
    static uint8_t encPrev = ENC_0;

    btnNow = stateBtnEnc & BTN_ALL;
    encNow = stateBtnEnc & ENC_AB;

    /* If encoder event has happened, inc/dec encoder counter */
    switch (encNow) {
    case ENC_AB:
        if (encPrev == ENC_B)
            encCnt++;
        if (encPrev == ENC_A)
            encCnt--;
        break;
        /*  case ENC_A:
        if (encPrev == ENC_AB)
            encCnt++;
        if (encPrev == ENC_0)
            encCnt--;
        break;
        case ENC_B:
        if (encPrev == ENC_0)
            encCnt++;
        if (encPrev == ENC_AB)
            encCnt++;
        break;
        case ENC_0:
        if (encPrev == ENC_A)
            encCnt++;
        if (encPrev == ENC_B)
            encCnt++;
        break;
        */
    }
    encPrev = encNow;               /* Save current encoder state */

    /* If button event has happened, place it to command buffer */
    if (btnNow) {
        if (btnNow == btnPrev) {
            btnCnt++;
            if (btnCnt == LONG_PRESS) {
                switch (btnPrev) {
                case BTN_1:
                    cmdBuf = CMD_BTN_1_LONG;
                    break;
                case BTN_2:
                    cmdBuf = CMD_BTN_2_LONG;
                    break;
                case BTN_3:
                    cmdBuf = CMD_BTN_3_LONG;
                    break;
                case BTN_1 | BTN_2:
                    cmdBuf = CMD_BTN_1_2_LONG;
                    break;
                }
            }
        } else {
            btnPrev = btnNow;
        }
    } else {
        if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS)) {
            switch (btnPrev) {
            case BTN_1:
                cmdBuf = CMD_BTN_1;
                break;
            case BTN_2:
                cmdBuf = CMD_BTN_2;
                break;
            case BTN_3:
                cmdBuf = CMD_BTN_3;
                break;
            }
        }
        btnCnt = 0;
    }

    /* Timer of current display mode */
    if (displayTime)
        displayTime--;

    /* RTC poll timer */
    if (rtcTimer)
        rtcTimer--;

    /* Time from last IR command */
    if (rcTimer < RC_PRESS_LIMIT)
        rcTimer++;

    return;
}

static void showIcon(uint8_t icon)
{
    uint8_t ic = icon;
    uint8_t pgmData;

    if (ic >= MODE_SND_GAIN0 && ic < MODE_SND_END)
        ic = eeprom_read_byte((uint8_t *)(EEPROM_INPUT_ICONS + (ic - MODE_SND_GAIN0)));
    if (ic < ICON_END)
        icon = ic;

    const uint8_t *icPtr = &icons[5 * icon];

    for (ic = 0; ic < 5; ic++) {
        pgmData = pgm_read_byte(icPtr + ic);
        pgmData &= 0x01F;
        newBuf[ic] &= 0xE0;
        newBuf[ic] |= pgmData;
    }

    return;
}

void matrixInit(void)
{
    DDR(ROW_08_06_04_02) |= ROW_08_06_04_02_LINE;
    DDR(ROW_16_15_13_12) |= ROW_16_15_13_12_LINE;
    DDR(ROW_14_11_10_09_07_05_03_01) |= ROW_14_11_10_09_07_05_03_01_LINE;

    DDR(REG_DATA) |= REG_DATA_LINE;
    DDR(REG_CLK) |= REG_CLK_LINE;

    TIMSK |= (1 << OCIE2) | (1 <<
                             TOIE2);           /* Enable timer 2 overflow and compare match interrups */
    TCCR2 |= (0 << CS22) | (1 << CS21) | (0 <<
                                          CS20); /* Set timer prescaller to 8 (8000000/8/256 = 3906 polls/sec)*/

    cmdBuf = CMD_END;
    encCnt = 0;

    rcCodesInit();

    brStby = eeprom_read_byte((uint8_t *)EEPROM_BR_STBY);
    brWork = eeprom_read_byte((uint8_t *)EEPROM_BR_WORK);

    return;
}

void matrixFill(uint8_t data)
{
    uint8_t i;

    for (i = 0; i < ROWS; i++)
        newBuf[i] = data;

    return;
}

void matrixSetBr(uint8_t value)
{
    if (value > DISP_MAX_BR)
        value = DISP_MAX_BR;

    OCR2 = 32 + value * 8;
}

void showSndParam(sndMode mode, uint8_t icon)
{
    sndParam *param = &sndPar[mode];
    int16_t value = param->value;
    int8_t min = pgm_read_byte(&param->grid->min);
    int8_t max = pgm_read_byte(&param->grid->max);

    matrixFill(0x00);

    if (icon == ICON_NATIVE)
        showIcon(mode);

    matrixSetPos(5);
    matrixShowDecimal(param->value * ((pgm_read_byte(&param->grid->step) + 4) >> 3));

    if (min + max) {
        max -= min;
        value -= min;
        value *= 17;
        value /= max;
        matrixShowBar(value);
    } else {
        value *= 9;
        value /= max;
        matrixShowSymBar(value);
    }

    return;
}

void showMute(void)
{
    showSndParam(MODE_SND_VOLUME, ICON_OTHER);
    showIcon(ICON_MUTE);

    return;
}

void showLoudness(void)
{
    showSndParam(MODE_SND_VOLUME, ICON_OTHER);
    showIcon(ICON_LOUDNESS);

    return;
}

void showStby(void)
{
    rcIndex = 0;
    rtc.etm = RTC_NOEDIT;
    showTime();

    return;
}

void showTime(void)
{
    uint8_t i;
    uint8_t blink;

    if (rtcTimer == 0) {
        rtcReadTime();
        rtcTimer = TIMEOUT_RTC;
    }

    blink = (rtcTimer & 0x0200) ? 1 : 0;

    matrixFill(0x00);

    if (rtc.etm != RTC_HOUR || blink) {
        matrixSetPos(0);
        matrixShowHex(rtcDecToBinDec(rtc.hour), 1);
    }

    if (rtc.etm != RTC_MIN || blink) {
        matrixSetPos(9);
        matrixShowHex(rtcDecToBinDec(rtc.min), 1);
    }

    if (rtc.etm != RTC_SEC || blink) {
        for (i = 0; i < rtc.sec / 10; i++)
            newBuf[i] |= 0xC0;
        for (i = 0; i < rtc.sec % 10; i++)
            newBuf[i + 7] |= 0xC0;
    }

    return;
}

void showLearn(void)
{
    IRData irBuf = getIrData();

    matrixFill(0x00);

    matrixSetPos(13);
    matrixShowHex(rcIndex, 0);

    // Binary data of RC command
    matrixSetPos(0);
    matrixShowHex(irBuf.command, 1);

    return;
}

void nextRcCmd(void)
{
    IRData irBuf = getIrData();

    eeprom_update_byte((uint8_t *)EEPROM_RC_TYPE, irBuf.type);
    eeprom_update_byte((uint8_t *)EEPROM_RC_ADDR, irBuf.address);
    eeprom_update_byte((uint8_t *)EEPROM_RC_CMD + rcIndex, irBuf.command);

    // Re-read new codes array from EEPROM
    rcCodesInit();

    if (++rcIndex >= CMD_RC_END)
        rcIndex = CMD_RC_STBY;

    switchTestMode(rcIndex);

    return;
}

void switchTestMode(uint8_t index)
{
    rcIndex = index;
    setIrData(eeprom_read_byte((uint8_t *)EEPROM_RC_TYPE),
              eeprom_read_byte((uint8_t *)EEPROM_RC_ADDR),
              eeprom_read_byte((uint8_t *)EEPROM_RC_CMD + rcIndex));

    return;
}

void updateScreen(uint8_t effect, uint8_t dispMode)
{
    uint8_t i;

    switch (effect) {
    case EFFECT_NONE:
        matrixSetBr(dispMode == MODE_STANDBY ? brStby : brWork);
        for (i = 0; i < ROWS; i++)
            scrBuf[i] = newBuf[i];
        break;
    case EFFECT_SPLASH:
        for (i = 0; i < ROWS / 2; i++) {
            scrBuf[i] = 0x00;
            scrBuf[ROWS - 1 - i] = 0x00;
            _delay_ms(20);
        }
        matrixSetBr(dispMode == MODE_STANDBY ? brStby : brWork);
        for (i = ROWS / 2; i < ROWS; i++) {
            _delay_ms(20);
            scrBuf[i] = newBuf[i];
            scrBuf[ROWS - 1 - i] = newBuf[ROWS - 1 - i];
        }
        break;
    }

    return;
}


int8_t getEncoder(void)
{
    int8_t ret = encCnt;
    encCnt = 0;
    return ret;
}

CmdID getCmdBuf(void)
{
    CmdID ret;

    ret = cmdBuf;
    cmdBuf = CMD_END;

    return ret;
}

CmdID getRcBuf(void)
{

    /* Place RC5 event to command buffer if enough RC5 timer ticks */
    IRData ir = takeIrData();

    CmdID rcCmdBuf = CMD_END;

    if (ir.ready && (ir.type == rcType && ir.address == rcAddr)) {
        if (!ir.repeat || (rcTimer > RC_LONG_PRESS)) {
            rcTimer = 0;
            rcCmdBuf = rcCmdIndex(ir.command);
        }
        if (ir.command == rcCode[CMD_RC_VOL_UP] || ir.command == rcCode[CMD_RC_VOL_DOWN]) {
            if (rcTimer > RC_VOL_REPEAT) {
                rcTimer = RC_VOL_DELAY;
                rcCmdBuf = rcCmdIndex(ir.command);
            }
        }
    }

    return rcCmdBuf;
}

void showBrWork()
{
    matrixFill(0x00);

    matrixSetPos(5);
    matrixShowDecimal(brWork);

    showIcon(ICON_BR);

    matrixShowBar(brWork);
}

void changeBrWork(int8_t diff)
{
    brWork += diff;
    if (brWork > DISP_MAX_BR)
        brWork = DISP_MAX_BR;
    if (brWork < DISP_MIN_BR)
        brWork = DISP_MIN_BR;
    matrixSetBr(brWork);

    eeprom_update_byte((uint8_t *)EEPROM_BR_WORK, brWork);

    return;
}
