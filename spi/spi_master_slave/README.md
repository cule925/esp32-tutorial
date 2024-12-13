# SPI-MASTER-SLAVE

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je napraviti sljedeće:

- GPIO 15 povezati na GPIO 5
- GPIO 14 povezati na GPIO 18
- GPIO 12 povezati na GPIO 19
- GPIO 13 povezati na GPIO 23

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |         GPIO 15 +---------------------+
        |                 |                     |
        |         GPIO 14 +----------------+    |
        |                 |                |    |
        |         GPIO 12 +-----------+    |    |
        |                 |           |    |    |
        |         GPIO 13 +------+    |    |    |
        |                 |      |    |    |    |
        |                 |      |    |    |    |
        |                 |      |    |    |    |
        |         GPIO 5  +------|----|----|----+
        |                 |      |    |    |
        |         GPIO 18 +------|----|----+
        |                 |      |    |
        |         GPIO 19 +------|----+
        |                 |      |
        |         GPIO 23 +------+
        |                 |
        +-----------------+
```

Aplikacija radi sljedeće:

- aplikacija inicijalizira dva SPI sklopa: HSPI (SPI2) i VSPI (SPI3) koji su međusobno povezani u *master-slave* konfiguraciju koja radi na taktu od 1 MHz 
- na jednom se SPI sklopu predstavlja kao I2C *master* dok se na drugom SPI sklopu predstavlja kao *slave*
- stvaraju se dva zadatka, jedan zadatak uzima ulogu *slavea*, priprema SPI transakciju i čeka iniciranje transakcije od *mastera* dok drugi zadatak uzima ulogu *mastera*, priprema SPI transakciju i inicira ju sa *slaveom*, u ovom slučaju svake sekunde
- događa se *full-duplex* razmjena poruka između *mastera* i *slavea*
- i *master* i *slave* imaju po dvije poruke koje šalju te se one šalju naizmjenice svakom sljedećom transakcijom

Razmjena poruka u ovom slučaju je sljedeća:

```
        +---------------+                             +---------------+
        |               |         TRANSACTION         |               |
        |               |   "Hello this is master."   |               |
        |               | --------------------------> |               |
        |               |   "Hello this is slave."    |               |
        |     ESP32     | <-------------------------- |     ESP32     |
        | HSPI (master) |                             | VSPI (slave)  |
        |               |         TRANSACTION         |               |
        |               |     "Any slave there?"      |               |
        |               | --------------------------> |               |
        |               |     "Any master there?"     |               |
        |               | <-------------------------- |               |
        +---------------+                             +---------------+
```