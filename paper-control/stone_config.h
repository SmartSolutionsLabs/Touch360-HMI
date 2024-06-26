/*
 * @file        stone_config.h
 *
 * @version     V1.1.1
 *
 * @date        2023-5-14
 *
 * @produce   STONE Technologies
 *
 * Customized only this file for this project to use Serial2.
 * All files of this API were not edited.
 *
 */
#ifndef _STONE_CONFIG_H__
#define _STONE_CONFIG_H__

#include "limits.h"

#define MCU_ESP 1

#include "Arduino.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "HardwareSerial.h"

// Reference to Serial
#define SERIAL_OBJECT Serial2

#define STONE_Delay(num)       delayMicroseconds(num*1000)
#define stone_uart_get_flag(USER_UART)   SERIAL_OBJECT.available()
#define stone_uart_read(USER_UART)   (STONE_RX_BUF[STONE_RX_CNT]=SERIAL_OBJECT.read())

#define transport_unfinished SERIAL_OBJECT.availableForWrite()
#define stone_Transmit(BUF, ...); SERIAL_OBJECT.println(("%s",BUF));
#define print_with_flash           1//Whether to enable the storage of fixed strings to be printed in Flash (enable saves memory and reduces running speed; disable improves running speed and occupies memory)
#define stone_print   SERIAL_OBJECT.print
#define stone_printf(string)     ((print_with_flash) ? SERIAL_OBJECT.print(F(string)) : SERIAL_OBJECT.print(string))
#define stone_println   SERIAL_OBJECT.println
#define stone_printfln(string)     ((print_with_flash) ? SERIAL_OBJECT.println(F(string)) : SERIAL_OBJECT.println(string))
#define stone_write     SERIAL_OBJECT.write
#define stone_writef(string)     ((print_with_flash) ? SERIAL_OBJECT.write(F(string)) : SERIAL_OBJECT.write(string))


//Print instructions to parse data for Arduino observation******************************************
#define Instruction_parsing        1    //The default open

//Print data, used for Arduino debugging, will occupy memory after enabled********************************
#define print_recive_ALL           0    //Off by default

#define print_recive_sys           0
#define print_recive_button        0
#define print_recive_switch        0
#define print_recive_check         0
#define print_recive_radio         0
#define print_recive_slider        0
#define print_recive_progress      0
#define print_recive_label         0
#define print_recive_edit          0
#define print_recive_selector      0
#define print_recive_image_value   0

#endif
