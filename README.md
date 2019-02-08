# SSD1306 AVR Library

SSD1306 driver library for AVR microcontrollers. SSD1306 is a single-chip CMOS OLED/PLED driver with controller for organic / polymer light emitting
diode dot-matrix graphic display system. It consists of 128 segments and 64commons. This IC is designed for Common Cathode type OLED panel. 

This project is more of an experiment of using c++ templates on a small microcontroller, such as avr, than a library, that focuses on optimization. The only goal was to provide easy to use interface.

### Supported communication interfaces

- [x] 4-wire SPI
- [ ] 3-wire SPI
- [ ] I2C

### Usage

Library usage example:
```C++
#include <ssd1306/display.hpp>
#include "ports.hpp" // Include custom IO implementation

int main()
{
	/* Set correct data direction values of the used ports */
	auto display = ssd1306::initDisplay128x64(port_c.p0, port_c.p1, port_c.p2, port_c.p3, port_c.p4);

	while(1)
	{
		display.clear();
		/* Draw something */
		display.display();
	}

	return 0;
}
```

Library needs custom implementation of I/O pin manipulation. Pin must be a class, and overload assignment operator.
Library allows template class pin implementations. Pins are passed to display instance by reference. 
Example pin implementation: 

```C++
#include <stdint.h>

using reg8 = volatile uint8_t 
using u8 = uint8_t;

struct Pin
{
	reg8 & ddrReg;
	reg8 & pinReg;
	reg8 & portReg;
	u8 pos;

	Pin(reg8 & _ddr, reg8 & _pin, reg8 & _port, u8 _pos)
		: ddrReg(_ddr)
		, pinReg(_pin)
		, portReg(_port)
		, pos(_pos)
	{
	}

	void setup(bool output) 
	{
		ddrReg = (ddrReg & ~(1 << pos)) | (output << pos); 
	}

	template <typename T>
	Pin & operator=(T value) 
	{ 
		portReg = (portReg & ~(1 << pos)) | (!!value << pos); 
	}

	operator bool() const 
	{
		return pinReg & (1 << pos);
	}
};
```
