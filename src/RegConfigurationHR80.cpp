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

#include "RegConfigurationHR80.hpp"

const uint8_t RegConfigurationHR80::register_configuration[] = {
		0x04,   // IOCFG2: GDO2 - Asserts when the RX FIFO has overflowed. De-asserts when the FIFO has been flushed.
		0x2E,   // IOCFG1: GDO1
		0x2E,   // IOCFG0: GDO0
		0x07,   // FIFOTHR: Default value

		// Preamble of each message is as follows.
		// Note that after the 4x 01010101 there is also a start bit [0] and
		// a stop bit [1] for each byte.
		//
		// 01010101 01010101 01010101 01010101
		// [0]11111111[1] [0]00000000[1] [0]11001100[1] [0]10101010[1] [0]11001010[1]
		//
		// We instruct the CC1101 to sync on the following 16 bits,
		// starting at bit position 8x4 + 4 = 36
		// 11111[1][0]0 0000000[1]

		0xFC,   // SYNC1: Syncword 11111[1][0]0
		0x01,   // SYNC0: Syncword 0000000[1]
		0xFF,   // PKTLEN:
		0x00,   // PKTCTRL1: Do not append status (RSSI and LQI)
		0x00,   // PKTCTRL0: Fixed packet length (see PKTLEN)
		0x00,   // ADDR      Device Address
		0x00,   // CHANNR    Channel Number
		0x06,   // FSCTRL1   Frequency synthesizer control.
		0x00,   // FSCTRL0   Frequency synthesizer control.
		0x20,   // FREQ2     Frequency control word, high byte.
		0x29,   // FREQ1     Frequency control word, middle byte.
		0x23,   // FREQ0     Frequency control word, low byte.
		0x6A,   // MDMCFG4: Data rate = 38,38 kBaud
		0x75,   // MDMCFG3: Data rate = 38,38 kBaud
		0x16,   // MDMCFG2: GFSK, 16/16 + carrier-sense above threshold
		0x02,   // MDMCFG1   Modem configuration.
		0xE5,   // MDMCFG0   Modem configuration.
		0x47,   // DEVIATN   Modem deviation setting (when FSK modulation is enabled).
		0x07,   // MCSM2
		0x30,   // MCSM1
		0x18,   // MCSM0     Main Radio Control State Machine configuration.
		0x16,   // FOCCFG    Frequency Offset Compensation Configuration.
		0x6C,   // BSCFG     Bit Synchronization Configuration
		0x43,   // AGCCTRL2  AGC control.
		0x40,   // AGCCTRL1  AGC control.
		0x91,   // AGCCTRL0  AGC control.
		0x87,   // WOREVT1   High Byte Event0 Timeout
		0x6B,   // WOREVT0   Low Byte Event0 Timeout
		0xF8,   // WORCTRL   Wake On Radio Control
		0x56,   // FREND1    Front End RX Configuration
		0x10,   // FREND0    Front End TX Configuration
		0xE9,   // FSCAL3    Frequency synthesizer calibration.
		0x2A,   // FSCAL2    Frequency synthesizer calibration.
		0x00,   // FSCAL1    Frequency synthesizer calibration.
		0x1F,   // FSCAL0    Frequency synthesizer calibration.
		0x41,   // RCCTRL1   RC Oscillator Configuration
		0x00,   // RCCTRL0   RC Oscillator Configuration
		0x59,   // FSTEST    Frequency Synthesizer Calibration Control
		0x7F,   // PTEST     Production Test
		0x3F,   // AGCTEST   AGC Test
		0x81,   // TEST2     Various test settings.
		0x35,   // TEST1     Various test settings.
		0x09,   // TEST0     Various test settings.
};

