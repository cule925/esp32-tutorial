# SPI-SLAVE

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je povezati sljedeće GPIO pinove na SPI *master* uređaj:

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |         GPIO 5  +---------------------- CS
        |                 |
        |         GPIO 18 +---------------------- SCLK
        |                 |
        |         GPIO 19 +---------------------- MISO     TO SPI MASTER
        |                 |
        |         GPIO 23 +---------------------- MOSI
        |                 |
        |             GND +---------------------- GND
        |                 |
        +-----------------+
```

Aplikacija radi sljedeće:

- aplikacija inicijalizira SPI sklop VSPI (SPI3) kao *slave*
- stvara se zadatak koji priprema SPI transakciju i čeka iniciranje od *mastera*
- događa se *full-duplex* razmjena poruka između *slavea* i *mastera* koje su u ovom slučaju veličine 40 bajtova
- *slave* ima po dvije poruke koje šalje naizmjenice svakom sljedećom transakcijom

Primjer *mastera* može biti [ovaj primjer s drugim ESP32 mikroupravljačem](../spi_master/). Razmjena poruka ako se koristi dani primjer je sljedeća:

```
        +---------------+                             +---------------+
        |               |         TRANSACTION         |               |
        |               |   "Hello this is master."   |               |
        |               | --------------------------> |               |
        |               |   "Hello this is slave."    |               |
        |  First ESP32  | <-------------------------- | Second ESP32  |
        | HSPI (master) |                             | VSPI (slave)  |
        |               |         TRANSACTION         |               |
        |               |     "Any slave there?"      |               |
        |               | --------------------------> |               |
        |               |     "Any master there?"     |               |
        |               | <-------------------------- |               |
        +---------------+                             +---------------+
```

**Oprez, naponi signala kojom mikroupravljači i mikroračunala komuniciraju moraju biti od 0 V do 3.3 V.**