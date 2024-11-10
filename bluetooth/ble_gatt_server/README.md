# BLE-GATT-SERVER

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

Aplikacija radi sljedeće:

- postavlja GPIO pin 22 na kojem je spojen svjetleća dioda kao ulazno-izlazni što znači da mu se može postaviti stanje i u isto vrijeme čitati s njega
- inicijalizira Bluetooth sklop u BLE načinu rada i inicijalizirra Bluedroid Bluetooth stog
- postavlja GAP parametre za oglašavanje
- inicijalizira uređaj kao GATT poslužitelj i postavlja profil, servis i karakteristiku
- ponaša se kao Bluetooth periferija i oglašavat će svoje GATT servise dok se na njega ne spoji neki Bluetooth centralni uređaj, a u slučaju da se centralni uređaj odspoji ponovno započinje oglašavanje
- prima zahtjeve za pisanje od GATT klijenta, uređaj postavlja LED-icu u stanje zahtjevano zahtjevom
- prima zahtjeve za čitanje od GATT klijenta, uređaj kao odgovor na zahtjev šalje trenutno stanje razine LED-ice
- ispisuje logove na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)

Organizacija podataka za trenutni uređaj odnosno GATT poslužitelj je sljedeća:

```
        +-------------------------------+
        | PROFILE 1                     |
        |                               |
        |  +-------------------------+  |
        |  | SERVICE 1               |  |
        |  |                         |  |
        |  |  +-------------------+  |  |
        |  |  | CHARACTERISTIC 1  |  |  |
        |  |  |                   |  |  |
        |  |  +-------------------+  |  |
        |  |                         |  |
        |  +-------------------------+  |
        +-------------------------------+
```

GATT klijent može biti ESP32 koji pokreće aplikaciju GATT klijenta iz ovog repozitorija ([ovdje](../ble_gatt_client/)), mobilna aplikacija ili program na računalu. Naravno, pametni mobitel i osobno računalo moraju imati ugrađen BLE.