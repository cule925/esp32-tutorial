# SPI-MASTER

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je povezati sljedeće GPIO pinove na SPI *slave* uređaj:

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |         GPIO 15 +---------------------- CS
        |                 |
        |         GPIO 14 +---------------------- SCLK
        |                 |
        |         GPIO 12 +---------------------- MISO     TO SPI SLAVE
        |                 |
        |         GPIO 13 +---------------------- MOSI
        |                 |
        |             GND +---------------------- GND
        |                 |
        +-----------------+
```

Aplikacija radi sljedeće:

- aplikacija inicijalizira SPI sklop HSPI (SPI2) kao *master* koji radi na taktu od 1 MHz 
- stvara se zadatak koji priprema SPI transakciju i inicira ju sa *slaveom*, u ovom slučaju svake sekunde
- događa se *full-duplex* razmjena poruka između *mastera* i *slavea* koje su u ovom slučaju veličine 40 bajtova
- *master* ima po dvije poruke koje šalje naizmjenice svakom sljedećom transakcijom

Primjer *slavea* može biti [ovaj primjer s drugim ESP32 mikroupravljačem](../spi_slave/). Razmjena poruka ako se koristi dani primjer je sljedeća:

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