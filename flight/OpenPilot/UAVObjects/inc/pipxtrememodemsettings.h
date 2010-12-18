/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup PipXtremeModemSettings PipXtremeModemSettings 
 * @brief Settings for the @ref PipXtremeModemSettings
 *
 * Autogenerated files and functions for PipXtremeModemSettings Object
 
 * @{ 
 *
 * @file       pipxtrememodemsettings.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the PipXtremeModemSettings object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: pipxtrememodemsettings.xml. 
 *             This is an automatically generated file.
 *             DO NOT modify manually.
 *
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef PIPXTREMEMODEMSETTINGS_H
#define PIPXTREMEMODEMSETTINGS_H

// Object constants
#define PIPXTREMEMODEMSETTINGS_OBJID 3822692478U
#define PIPXTREMEMODEMSETTINGS_NAME "PipXtremeModemSettings"
#define PIPXTREMEMODEMSETTINGS_METANAME "PipXtremeModemSettingsMeta"
#define PIPXTREMEMODEMSETTINGS_ISSINGLEINST 1
#define PIPXTREMEMODEMSETTINGS_ISSETTINGS 1
#define PIPXTREMEMODEMSETTINGS_NUMBYTES sizeof(PipXtremeModemSettingsData)

// Object access macros
/**
 * @function PipXtremeModemSettingsGet(dataOut)
 * @brief Populate a PipXtremeModemSettingsData object
 * @param[out] dataOut 
 */
#define PipXtremeModemSettingsGet(dataOut) UAVObjGetData(PipXtremeModemSettingsHandle(), dataOut)
#define PipXtremeModemSettingsSet(dataIn) UAVObjSetData(PipXtremeModemSettingsHandle(), dataIn)
#define PipXtremeModemSettingsInstGet(instId, dataOut) UAVObjGetInstanceData(PipXtremeModemSettingsHandle(), instId, dataOut)
#define PipXtremeModemSettingsInstSet(instId, dataIn) UAVObjSetInstanceData(PipXtremeModemSettingsHandle(), instId, dataIn)
#define PipXtremeModemSettingsConnectQueue(queue) UAVObjConnectQueue(PipXtremeModemSettingsHandle(), queue, EV_MASK_ALL_UPDATES)
#define PipXtremeModemSettingsConnectCallback(cb) UAVObjConnectCallback(PipXtremeModemSettingsHandle(), cb, EV_MASK_ALL_UPDATES)
#define PipXtremeModemSettingsCreateInstance() UAVObjCreateInstance(PipXtremeModemSettingsHandle())
#define PipXtremeModemSettingsRequestUpdate() UAVObjRequestUpdate(PipXtremeModemSettingsHandle())
#define PipXtremeModemSettingsRequestInstUpdate(instId) UAVObjRequestInstanceUpdate(PipXtremeModemSettingsHandle(), instId)
#define PipXtremeModemSettingsUpdated() UAVObjUpdated(PipXtremeModemSettingsHandle())
#define PipXtremeModemSettingsInstUpdated(instId) UAVObjUpdated(PipXtremeModemSettingsHandle(), instId)
#define PipXtremeModemSettingsGetMetadata(dataOut) UAVObjGetMetadata(PipXtremeModemSettingsHandle(), dataOut)
#define PipXtremeModemSettingsSetMetadata(dataIn) UAVObjSetMetadata(PipXtremeModemSettingsHandle(), dataIn)
#define PipXtremeModemSettingsReadOnly(dataIn) UAVObjReadOnly(PipXtremeModemSettingsHandle())

// Object data
typedef struct {
    uint8_t Mode;
    uint8_t Serial_Baudrate;
    uint8_t Frequency_Calibration;
    uint32_t Frequency_Min;
    uint32_t Frequency_Max;
    uint32_t Frequency;
    uint8_t Max_RF_Bandwidth;
    uint8_t Max_Tx_Power;
    uint8_t AES_Encryption;
    uint8_t AES_EncryptionKey[16];
    uint32_t Paired_Serial_Number;

} __attribute__((packed)) PipXtremeModemSettingsData;

// Field information
// Field Mode information
/* Enumeration options for field Mode */
typedef enum { PIPXTREMEMODEMSETTINGS_MODE_NORMAL=0, PIPXTREMEMODEMSETTINGS_MODE_TEST_CARRIER=1, PIPXTREMEMODEMSETTINGS_MODE_TEST_SPECTRUM=2 } PipXtremeModemSettingsModeOptions;
// Field Serial_Baudrate information
/* Enumeration options for field Serial_Baudrate */
typedef enum { PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_1200=0, PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_2400=1, PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_4800=2, PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_9600=3, PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_19200=4, PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_38400=5, PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_57600=6, PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_115200=7, PIPXTREMEMODEMSETTINGS_SERIAL_BAUDRATE_230400=8 } PipXtremeModemSettingsSerial_BaudrateOptions;
// Field Frequency_Calibration information
// Field Frequency_Min information
// Field Frequency_Max information
// Field Frequency information
// Field Max_RF_Bandwidth information
/* Enumeration options for field Max_RF_Bandwidth */
typedef enum { PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_500=0, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_1000=1, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_2000=2, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_4000=3, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_8000=4, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_9600=5, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_16000=6, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_19200=7, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_24000=8, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_32000=9, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_64000=10, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_128000=11, PIPXTREMEMODEMSETTINGS_MAX_RF_BANDWIDTH_192000=12 } PipXtremeModemSettingsMax_RF_BandwidthOptions;
// Field Max_Tx_Power information
/* Enumeration options for field Max_Tx_Power */
typedef enum { PIPXTREMEMODEMSETTINGS_MAX_TX_POWER_1=0, PIPXTREMEMODEMSETTINGS_MAX_TX_POWER_2=1, PIPXTREMEMODEMSETTINGS_MAX_TX_POWER_5=2, PIPXTREMEMODEMSETTINGS_MAX_TX_POWER_8=3, PIPXTREMEMODEMSETTINGS_MAX_TX_POWER_11=4, PIPXTREMEMODEMSETTINGS_MAX_TX_POWER_14=5, PIPXTREMEMODEMSETTINGS_MAX_TX_POWER_17=6, PIPXTREMEMODEMSETTINGS_MAX_TX_POWER_20=7 } PipXtremeModemSettingsMax_Tx_PowerOptions;
// Field AES_Encryption information
/* Enumeration options for field AES_Encryption */
typedef enum { PIPXTREMEMODEMSETTINGS_AES_ENCRYPTION_FALSE=0, PIPXTREMEMODEMSETTINGS_AES_ENCRYPTION_TRUE=1 } PipXtremeModemSettingsAES_EncryptionOptions;
// Field AES_EncryptionKey information
/* Number of elements for field AES_EncryptionKey */
#define PIPXTREMEMODEMSETTINGS_AES_ENCRYPTIONKEY_NUMELEM 16
// Field Paired_Serial_Number information


// Generic interface functions
int32_t PipXtremeModemSettingsInitialize();
UAVObjHandle PipXtremeModemSettingsHandle();

#endif // PIPXTREMEMODEMSETTINGS_H

/**
 * @}
 * @}
 */
