# I2C-MASTER

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je napraviti sljedeće:

- između GPIO 22 i 3.3 V potrebno je u seriju spojiti otpor 2 kOhm
- između GPIO 21 i 3.3 V potrebno je u seriju spojiti otpor 2 kOhm

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |           3.3 V +---------------------+----------------+
        |                 |                     |                |     
        |                 |                    +++              +++
        |                 |                    | | R - 1kOhm    | | R - 1kOhm
        |                 |                    | |              | |
        |                 |                    +++              +++
        |                 |                     |                |
        |                 |                    +++              +++
        |                 |                    | | R - 1kOhm    | | R - 1kOhm
        |                 |                    | |              | |
        |                 |                    +++              +++
        |                 |                     |                |
        |         GPIO 22 +---------------------+----------------|----------------- SDA
        |                 |                                      |
        |         GPIO 21 +--------------------------------------+----------------- SCL     TO I2C SLAVE
        |                 |
        |             GND +-------------------------------------------------------- GND
        |                 |
        +-----------------+
```

Aplikacija radi sljedeće:

- aplikacija inicijalizira jedan I2C sklop i spaja se na njega kao *master*
- aplikacija šalje *slaveu*, u ovom slučaju s adresom 0x22, dva bajta nizom: "{0x0, 0x1}, {0x0, 0x0}, {0x1, 0x1}, {0x1, 0x0}" 10 puta

Primjer *slavea* može biti [ovaj primjer s drugim ESP32 mikroupravljačem](../i2c_slave/), bitno je da slave omogućuje primanje podatkovna dva bajta.

**Oprez, naponi signala kojom mikroupravljači i mikroračunala komuniciraju moraju biti od 0 V do 3.3 V.**