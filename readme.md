# FM7segm (AVR ATmega8 version)

This project is a simple FM radio tuner with 4-digits LED indication.
Up to 50 FM stations can be stored in EEPROM memory of ATmega8 MCU.
Also, device can show current time and temperature.

There is a [video](https://youtu.be/3I_dFwAWISo) on Youtube about this.

## Main features:

|Feature description                                       |Supported|
|----------------------------------------------------------|---------|
|FM radio support                                          |yes      |
|Up to 60 FM stations can be saved in EEPROM               |yes      |
|Common cathode indicators with direct connection do digits|yes      |
|Common anode indicators with direct connection do digits  |yes      |
|Common cathode indicators with transistor on digits       |yes      |
|Common anode indicators with transistor on digits         |yes      |
|Indicator brightness control (12 levels)                  |yes      |
|RDA580x internal volume control support                   |yes      |
|PWM volume control on other tuners                        |yes      |
|RTC DS1307 support (DS323x also are working)              |yes      |
|Temperature sensors DS18x20                               |yes      |

## Supported FM tuners:

|Tuner     |Description                                   |Supported|
|----------|----------------------------------------------|---------|
|TEA5767   |I²C FM tuner                                  |yes      |
|RDA5807   |I²C FM tuner with RDS support                 |yes      |
|TUX032    |I²C FM tuner found in some Sony car radio     |yes      |
|LM7001    |SPI-control frequency synthesizer             |yes      |
|RDA5802   |I²C FM tuner                                  |yes      |
|RDA5807_DF|Option for RDA5807 with direct frequency input|yes      |

## Schematic:

* First pinout:

	![First pinout](https://raw.githubusercontent.com/WiseLord/fm7segm/master/files/fm7segm_pin1_sch.png)

* Second pinout:

	![Second pinout](https://raw.githubusercontent.com/WiseLord/fm7segm/master/files/fm7segm_pin2_sch.png)

Both pinouts can be used with any FM tuner supported and any indicator type and connection combination.
All this can be configured via Makefile and pins.h files.

## Building the code

AVR GNU GCC toolchain is required to build the project.

The project build system is based on [GNU Make](https://www.gnu.org/software/make/).
Linux users must have it already installed or they can easily do it.

The process of building is very simple and doesn't depend on any other software installed.
Build commands should be executed from the *src* directory containing Makefile.

### Default build:

`make`

### Build for the specific wiring:

`make -j4 PINOUT=PIN3`

Refer to build_all.sh script for possible options

## Building EEPROM Editor application

To setup various options of the project it's possible to pre-edit the eeprom.bin file before
uploading it to the board. It can be easily built either from QtCreator or from the console:

```
qmake .
make
```

## Links

* [Article about the project](http://radiokot.ru/circuit/digital/home/202) (in Russian)
* [Forum thread](http://radiokot.ru/forum/viewtopic.php?t=109632) (in Russian)
