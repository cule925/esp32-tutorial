# UART-TX-RX

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Postoje dvije opcije za ovaj projekt. Prva opcija je koristiti jedan ESP32 gdje je potrebno napraviti sljedeće:

- GPIO 25 i GPIO 26 potrebno je spojiti žicom

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |    (TX) GPIO 25 +---------------------+
        |                 |                     |
        |    (RX) GPIO 26 +---------------------+
        |                 |
        +-----------------+
```

Druga opcija je koristiti dva ESP32 gdje je potrebno napraviti sljedeće:

- GPIO 25 prvog ESP32 povezati na GPIO 26 drugog ESP32
- GPIO 25 drugog ESP32 povezati na GPIO 26 prvog ESP32
- GND prvog ESP32 povezati na GND drugog ESP32

```
        +-----------------+                                           +-----------------+
        | ESP32-DevKitM-1 |                                           | ESP32-DevKitM-1 |
        |                 |                                           |                 |
        |    (TX) GPIO 25 +-------------------------------------------+ (RX) GPIO 26    |
        |                 |                                           |                 |
        |    (RX) GPIO 26 +-------------------------------------------+ (TX) GPIO 25    |
        |                 |                                           |                 |
        |             GND +-------------------------------------------+ GND             |
        |                 |                                           |                 |
        +-----------------+                                           +-----------------+
```

Aplikacija radi sljedeće:

- aplikacija inicijalizira jedan UART sklop
- stvaraju se dva zadatka, jedan zadatak šalje naizmjenično dvije poruke na TX dok drugi zadatak čeka na poruke s RX
- poruka u lokalnom međuspremniku se kopira u TX kružni međuspremnik, UART ISR nakon toga prebacuje te podatke u TX FIFO međuspremnik te se ti podatci šalju na TX
- podatci koji dođu na RX se spremaju u RX FIFO međuspremnik, UART ISR nakon toga prebacuje te podatke u RX kružni međuspremnik te poruka na kraju kopira u lokalni međuspremnik