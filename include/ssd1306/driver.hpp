#pragma once

#include <util/delay.h>

namespace ssd1306
{
	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	class Driver
	{
		protected:
			enum : uint8_t { Command = 0, Data = 1 }; // Signals
			enum : uint8_t { Horizontal = 0, Vertical = 1, Page = 2 }; // Addressing modes
			
			enum class Voltage : uint8_t 
			{ 
				Low = 0,       // Factor 0.65 * Vcc. For Vcc = 5V it gives 3.25V
				Medium = 0x20, // Factor 0.77 * Vcc. For Vcc = 5V it gives 3.85V
				High = 0x30,   // Factor 0.83 * Vcc. For Vcc = 5V it gives 4.15V
				Highest = 0x40 // Undocumented, but works, and gives maximum brightness. Factor unknown.
				// Some says that factor of highest is 0.9, but it is not covered by any measurements.
			}; // Voltage levels
			
			T1 & d0;
			T2 & d1;
			T3 & res;
			T4 & dc;
			T5 & cs;

		public:
			Driver(T1 & pin_d0, T2 & pin_d1, T3 & pin_res, T4 & pin_dc, T5 & pin_cs)
				: d0(pin_d0), d1(pin_d1), res(pin_res), dc(pin_dc), cs(pin_cs)			
			{
				dc = 0;
				cs = 0;
				res = 1;
				_delay_ms(10);
				res = 0;
				_delay_ms(60);
				res = 1;

				this->setDisplayOff();
				this->setColumnStartAddress(0);
				this->setDisplayStartLine(0);
				this->setAddressingMode(Page);
				this->setContrast(210);
				this->setSegmentRemap(false);
				this->setComScanDirection(false);
				this->setMuxRatio(63);
				this->setDisplayOffset(0);
				this->setDivideAndFrequency(0, 8);
				this->setPreChargePeriod(1, 15);
				this->setComPinsConfiguration(true, false);
				this->setVcommhLevel(Voltage::Low);
				this->enableChargePump(true);
				this->continueRamDisplay();
				this->setNormalDisplay();
				this->setDisplayOn();
			}

		protected:
			template <bool signal>
			void write(uint8_t byte)
			{
				cs = 1;
				dc = signal;
				cs = 0;
				d0 = 0;

				for(auto i=0; i<8; i++)
				{
					d1 = byte & 0x80;

					d0 = 1;
					d0 = 0;

					byte <<= 1;
				}

				cs = 1;
			}

			void writeCommand(uint8_t cmd) { write<Command>(cmd); }
			void writeData(uint8_t data) { write<Data>(data); }

			#define wc(cmd) writeCommand(cmd)

			// Fundamental commands
			void setContrast(uint8_t contrast) { wc(0x81); wc(contrast); }
			void continueRamDisplay() { wc(0xA4); }
			void setEntireDisplayOn() { wc(0xA5); }
			void setNormalDisplay() { wc(0xA6); }
			void setInverseDisplay() { wc(0xA7); }
			void setDisplayOff() { wc(0xAE); }
			void setDisplayOn() { wc(0xAF); }

			// Addressing commands
			void setColumnStartAddress(uint8_t start) { wc(start & 0x0F); wc((start >> 4) | 0x10); }
			void setAddressingMode(uint8_t mode) { wc(0x20); wc(mode); }
			void setColumnAddress(uint8_t start, uint8_t end) { wc(0x21); wc(start); wc(end); }
			void setPageAddress(uint8_t start, uint8_t end) { wc(0x22); wc(start); wc(end); }
			void setPageStartAddress(uint8_t start) { wc(0xB0 | start); }

			// Hardware Configuration
			void setDisplayStartLine(uint8_t start) { wc(0x40 | start); }
			void setSegmentRemap(bool inverse) { wc(0xA0 | inverse); }
			void setMuxRatio(uint8_t ratio) { wc(0xA8); wc(ratio); }
			void setComScanDirection(bool inverse) { wc(0xC0 | (inverse << 3)); }
			void setDisplayOffset(uint8_t offset) { wc(0xD3); wc(offset); }
			void setComPinsConfiguration(bool sequental, bool remap) { wc(0xDA); wc((sequental << 4) | (remap << 5) | 2); }

			// Timing commands
			void setDivideAndFrequency(uint8_t divide, uint8_t freq) { wc(0xD5); wc(divide | (freq << 4)); }
			void setPreChargePeriod(uint8_t phase1, uint8_t phase2) { wc(0xD9); wc(phase1 | (phase2 << 4)); }
			void setVcommhLevel(Voltage level) { wc(0xDB); wc(static_cast<uint8_t>(level)); }
			void nop() { wc(0xE0); }

			// Charge pump
			void enableChargePump(bool enable) { wc(0x8D); wc(0x10 | (enable << 2)); }

			#undef wc
	};
};