/*
 * rfcc1101 - SPI Protocol Driver for TI CC1101 RF communication module.
 *
 * Copyright (C) 2013 Wolfgang Klenk <wolfgang.klenk@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "RegConfigurationProfile0_26MHz.hpp"

const uint8_t RegConfigurationProfile0_26MHz::register_configuration[] = {
		0x01,  // IOCFG2              GDO2 Output Pin Configuration
		0x2E,  // IOCFG1              GDO1 Output Pin Configuration
		0x2E,  // IOCFG0              GDO0 Output Pin Configuration
		0x4E,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
		0xD3,  // SYNC1               Sync Word, High Byte
		0x91,  // SYNC0               Sync Word, Low Byte
		0xFF,  // PKTLEN              Packet Length
		0x04,  // PKTCTRL1            Packet Automation Control
		0x05,  // PKTCTRL0            Packet Automation Control
		0x00,  // ADDR                Device Address
		0x00,  // CHANNR              Channel Number
		0x08,  // FSCTRL1             Frequency Synthesizer Control
		0x00,  // FSCTRL0             Frequency Synthesizer Control
		0x23,  // FREQ2               Frequency Control Word, High Byte
		0x31,  // FREQ1               Frequency Control Word, Middle Byte
		0x3B,  // FREQ0               Frequency Control Word, Low Byte
		0x7B,  // MDMCFG4             Modem Configuration
		0x83,  // MDMCFG3             Modem Configuration
		0x03,  // MDMCFG2             Modem Configuration
		0x22,  // MDMCFG1             Modem Configuration
		0xF8,  // MDMCFG0             Modem Configuration
		0x42,  // DEVIATN             Modem Deviation Setting
		0x07,  // MCSM2               Main Radio Control State Machine Configuration
		0x30,  // MCSM1               Main Radio Control State Machine Configuration
		0x18,  // MCSM0               Main Radio Control State Machine Configuration
		0x1D,  // FOCCFG              Frequency Offset Compensation Configuration
		0x1C,  // BSCFG               Bit Synchronization Configuration
		0xC7,  // AGCCTRL2            AGC Control
		0x00,  // AGCCTRL1            AGC Control
		0xB2,  // AGCCTRL0            AGC Control
		0x87,  // WOREVT1             High Byte Event0 Timeout
		0x6B,  // WOREVT0             Low Byte Event0 Timeout
		0xFB,  // WORCTRL             Wake On Radio Control
		0xB6,  // FREND1              Front End RX Configuration
		0x10,  // FREND0              Front End TX Configuration
		0xEA,  // FSCAL3              Frequency Synthesizer Calibration
		0x2A,  // FSCAL2              Frequency Synthesizer Calibration
		0x00,  // FSCAL1              Frequency Synthesizer Calibration
		0x1F,  // FSCAL0              Frequency Synthesizer Calibration
		0x41,  // RCCTRL1             RC Oscillator Configuration
		0x00,  // RCCTRL0             RC Oscillator Configuration
		0x59,  // FSTEST              Frequency Synthesizer Calibration Control
		0x7F,  // PTEST               Production Test
		0x3F,  // AGCTEST             AGC Test
		0x81,  // TEST2               Various Test Settings
		0x35,  // TEST1               Various Test Settings
		0x09,  // TEST0               Various Test Settings
};

