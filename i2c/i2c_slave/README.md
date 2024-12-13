# I2C-SLAVE

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je napraviti sljedeće:

- između GPIO 22 i GND potrebno je u seriju spojiti svjetleću diodu i otpornik (1 kOhm)
- između GPIO 21 i GND potrebno je u seriju spojiti svjetleću diodu i otpornik (1 kOhm)
- GPIO 19 potrebno je spojiti SDA nekog I2C *mastera*
- GPIO 18 potrebno je spojiti SCL nekog I2C *mastera*
- GND potrebno je spojiti GND nekog I2C *mastera*

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |         GPIO 19 +------------------------------------------------------------------------- SDA
        |                 |
        |         GPIO 18 +------------------------------------------------------------------------- SCL     TO I2C MASTER
        |                 |
        |         GPIO 22 +--------------------------------------+                +----------------- GND
        |                 |                                      |                |
        |         GPIO 21 +---------------------+                |                |
        |                 |                     |                |                |
        |             GND +----+               _|_              _|_               |
        |                 |    |               \ / LED 2        \ / LED 1         |
        +-----------------+    |                |                |                |
                               |                |                |                |
                               |               +++              +++               |
                               |               | | R - 1kOhm    | | R - 1kOhm     |
                               |               | |              | |               |
                               |               +++              +++               |
                               |                |                |                |
                               +----------------+----------------+----------------+
```

Aplikacija radi sljedeće:

- aplikacija inicijalizira jedan I2C sklop i spaja se na njega kao *slave*, u ovom slučaju s adresom 0x22
- aplikacija čeka da neki *master* upiše dva bajta gdje prvi bajt označava svjetleću diodu (prvu ili drugu), a drugi bajt što raditi s njom (paliti ili gasiti), ovo se događa beskonačno dugo

Primjer *mastera* može biti [ovaj primjer s drugim ESP32 mikroupravljačem](../i2c_master/) ili [ovaj primjer s Raspberry Pi mikroračunalom](https://github.com/cule925/raspberrypi-tutorial/tree/master/gpio/direct-gpio-control/i2c_master), bitno je da *master* šalje ovom *slaveu* točno dva bajta, jedan za odabir svjetleće diode (0 ili 1), a drugi za postavljanje stanja odabrane diode (0 ili 1).

**Oprez, naponi signala kojom mikroupravljači i mikroračunala komuniciraju moraju biti od 0 V do 3.3 V.**
