#pragma once

#include "driver.hpp"
#include "font.hpp"

namespace ssd1306
{
	template <unsigned Width, unsigned Height,
		typename T1, typename T2, typename T3, typename T4, typename T5>
	class Display
		: public Driver<T1, T2, T3, T4, T5>
	{
		uint8_t buffer[Width * Height / 8];

		public:
			Display(T1 & pin_d0, T2 & pin_d1, T3 & pin_res, T4 & pin_dc, T5 & pin_cs)
				: Driver<T1, T2, T3, T4, T5>(pin_d0, pin_d1, pin_res, pin_dc, pin_cs)
			{
				clear();
			}

			void fill(uint8_t byte)
			{
				for(auto & seg : buffer)
					seg = byte;
			}

			inline void clear() { fill(0x00); }

			void putPixel(uint8_t x, uint8_t y, uint8_t color)
			{
				if(x < 0 || x > Width || y < 0 || y > Height)
					return;

				auto & seg = buffer[y/8 * Width + x];
				seg = (seg & ~(1 << (y % 8))) | (!!color << (y % 8));
			}

			void drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
			{
				for(int px = x, left = width + x; px < left; px++)
					for(int py = y, bottom = height + y; py < bottom; py++)
						if(px == x || px == left-1 || py == y || py == bottom-1)
							putPixel(px, py, color);
			}

			void fillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
			{
				for(int px = x, left = width + x; px < left; px++)
					for(int py = y, bottom = height + y; py < bottom; py++)
						if(px != x || px != left-1 || py != y || py != bottom-1)
							putPixel(px, py, color);
			}

			void drawChar(uint8_t x, uint8_t y, char c)
			{
				if(x < 0 || x > Width || y < 0 || y > Height)
					return;

				for(auto i = 0u; i < 5; i++)
				{
					uint8_t ch = pgm_read_byte(&font[static_cast<uint8_t>(c-32) * 5 + i]);
					if(x+i < Width) 
						buffer[(y / 8) * Width + x + i] |= ch << (y % 8);
					else
						break;
					
					if(y+7 < Height) 
						buffer[((y + 7) / 8) * Width + x + i] |= ch >> (7 - (y % 8));
				}
			}

			void drawString(uint8_t x, uint8_t y, const char * ptr)
			{
				for(auto i = 0; ptr[i] != 0; i++)
					drawChar(x+(i*6), y, ptr[i]);
			}

			void drawBitmap(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t * bitmap)
			{
				uint8_t segWidth = height + 7 / 8;
				uint8_t byte = 0;

				for(uint8_t py = 0; py < height; py++)
				{
					for(uint8_t px = 0; px < width; px++)
					{
						if(py & 7)
							byte <<= 1;
						else 
							byte = pgm_read_byte(&bitmap[py/8 * width + px]);

						if(byte & 0x80)
							putPixel(x+px, y+py, 1);
					}
				}
			}

			void drawNumber(uint8_t x, uint8_t y, uint16_t number, bool zero_fill = false)
			{
				for(int px = x, z = 0, div = 10000; div >= 1; div/10)
				{
					auto r = number % div;
					if(r == 0 && (zero_fill || z))
					{
						drawChar(px, y, r+48);
						px += 6;
					}
					else if(r != 0)
					{
						z = 1;
						drawChar(px, y, r+48);
						px += 6;
					}
				}
			}

			void display()
			{
				for(auto page = 0; page < 8; page++)
				{
					this->setPageStartAddress(page);
					this->setColumnStartAddress(0);
					for(auto seg = 0; seg < 128; seg++)
						this->writeData(buffer[page * Width + seg]);
				}
			}
	};

	template<unsigned Width, unsigned Height, typename T1, typename T2, typename T3, typename T4, typename T5>
	decltype(auto) initDisplay(T1 & pin_d0, T2 & pin_d1, T3 & pin_res, T4 & pin_dc, T5 & pin_cs)
	{
		return Display<Width, Height, T1, T2, T3, T4, T5>(pin_d0, pin_d1, pin_res, pin_dc, pin_cs);
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	inline decltype(auto) initDisplay128x64(T1 & pin_d0, T2 & pin_d1, T3 & pin_res, T4 & pin_dc, T5 & pin_cs)
	{
		return initDisplay<128, 64, T1, T2, T3, T4, T5>(pin_d0, pin_d1, pin_res, pin_dc, pin_cs);
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	inline decltype(auto) initDisplay128x32(T1 & pin_d0, T2 & pin_d1, T3 & pin_res, T4 & pin_dc, T5 & pin_cs)
	{
		return initDisplay<128, 32, T1, T2, T3, T4, T5>(pin_d0, pin_d1, pin_res, pin_dc, pin_cs);
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	inline decltype(auto) initDisplay96x64(T1 & pin_d0, T2 & pin_d1, T3 & pin_res, T4 & pin_dc, T5 & pin_cs)
	{
		return initDisplay<96, 64, T1, T2, T3, T4, T5>(pin_d0, pin_d1, pin_res, pin_dc, pin_cs);
	}
};