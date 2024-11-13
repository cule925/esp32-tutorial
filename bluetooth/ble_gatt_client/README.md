# BLE-GATT-CLIENT

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je na GPIO pin 22 postaviti svjetleću diodu (*eng. LED*) i otpornik (1 kOhm) u seriju ih na drugi kraju povezati s GND:

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |         GPIO 22 +---------------------+
        |                 |                     |
        |             GND +----+               _|_
        |                 |    |               \ / LED
        +-----------------+    |                |
                               |                |
                               |               +++
                               |               | | R - 1kOhm
                               |               | |
                               |               +++
                               |                |
                               +----------------+
```

**Aplikaciju je potrebno koristiti s već implementiranim GATT poslužiteljem čiji se projekt nalazi [ovdje](../ble_gatt_server/). Dakle potrebno je pokrenuti dva ESP32 mikroupravljača, jedan kao GATT klijent, a drugi kao GATT poslužitelj.**

Aplikacija radi sljedeće:

- postavlja GPIO pin 22 na kojem je spojen svjetleća dioda kao ulazno-izlazni što znači da mu se može postaviti stanje i u isto vrijeme čitati s njega
- stvara zadatak koji pali i gasi LED i ako je povezan GATT poslužiteljem postavlja mu karakteristiku kako pali i gasi lokalnu LED što efektivno pali i gasi LED na GATT poslužitelju
- stvara zadatak koji čita vrijednost karakteristike na GATT poslužitelju ako je spojen
- inicijalizira Bluetooth sklop u BLE načinu rada i inicijalizira Bluedroid Bluetooth stog
- postavlja GAP parametre za skeniranje
- inicijalizira uređaj kao GATT klijent i postavlja profil, servis i karakteristiku
- ponaša se kao Bluetooth centralni uređaj i skenirat će GATT servise sve dok se ne spoji s nekom Bluetooth periferijom, a u slučaju da se dogodi prekid veze ponovno započinje skeniranje
- ispisuje logove na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)