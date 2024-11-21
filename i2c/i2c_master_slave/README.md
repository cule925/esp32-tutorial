# I2C-MASTER-SLAVE

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je napraviti sljedeće:

- GPIO 18 i GPIO 21 potrebno je spojiti pa je na taj spoj potrebno nadodati otpor od 2 kOhma koji vodi prema 3.3 V izvoru napajanja
- GPIO 19 i GPIO 22 potrebno je spojiti pa je na taj spoj potrebno nadodati otpor od 2 kOhma koji vodi prema 3.3 V izvoru napajanja

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
        |         GPIO 18 +---------------------+                |
        |                 |                     |                |
        |         GPIO 21 +---------------------+                |
        |                 |                                      |
        |         GPIO 19 +--------------------------------------+
        |                 |                                      |
        |         GPIO 22 +--------------------------------------+
        |                 |
        +-----------------+
```

Aplikacija radi sljedeće:

- aplikacija inicijalizira oba I2C sklopa
- na jednom se I2C sklopu predstavlja kao I2C *master* dok se na drugom I2C sklopu predstavlja kao *slave*
- SDA i SCL prvog sklopa proslijeđeni su na GPIO pinove 22 i 21, a SCL drugog sklopa na GPIO pinove 19 i 18, pinovi 22 i 19 su kratkospojeni kao i pinovi 21 i 18 što efektivno povezuje i *master* i *slave* na jednu I2C sabirnicu
- stvaraju se dva zadatka, jedan zadatak uzima ulogu *mastera* i šalje naizmjenično dvije poruke *slaveu* dok drugi zadatak uzima ulogu *slavea* i asinkrono čeka na poruke *mastera* (nakon dolaska određene količine podataka zove se ISR i korisničko registrirani *callback*)
