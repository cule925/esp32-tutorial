# MQTT-CLIENT

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

Aplikacija koristi MQTT protokol verzije 3.1.1. Ovoj aplikaciji je potreban MQTT posrednik i on se može instalirati na računalu koje pokreće Linux operacijski sustav. MQTT posrednik koji je potrebno instalirati je [mosquitto](https://mosquitto.org/). Za Arch Linux, to se radi naredbom:
```
sudo pacman -S mosquitto
```

Prije prevođenja, potrebno je urediti datoteku *config.h* tako da se definicijama umjesto *"[PLACEHOLDER]"* upišu prikladne definicije: SSID Wi-Fija, zaporka Wi-Fija, ime ESP32 na mreži, URL posrednika (koristi se IP adresa računala koje ima instaliran *mosquitto*), korisničko i zaporka (potrebni su za prijavu klijenta na posrednik). Pokretanje ESP32 se preporučuje tek nakon što se pokrene MQTT posrednik.

Na Linuxu je je potrebno napraviti direktorij u korisnikovom *home* direktoriju i premjestiti se u njega naredbama:
```
mkdir ~/mosquitto
cd ~/mosquitto
```

Prvo je potrebno napraviti datoteku koja će sadržavati korisničko ime i sažetu zaporku koje su iste kao i definicijama u zaglavlju *config.h*. To se može napraviti naredbom:
```
mosquitto_passwd -c mosquitto_password_file [Korisničko ime za prijavu na posrednik]
```

Zatim je potrebno napraviti datoteku konfiguracije posrednika i u nju upisati (primjerice uz ```nano mosquitto.conf```):

```
listener 1883 0.0.0.0
password_file mosquitto_password_file
```

Zapis *listener 1883 0.0.0.0* će dopustiti prosljeđivanje paketa izvan *localhosta* Linux računala na MQTT posrednik na Linux računalu, a zapis *password_file mosquitto_password_file* će reći posredniku koja datoteka sadrži korisničko ime i zaporku.

Konačno, pokretanje posrednika radi se naredbom:
```
mosquitto -c mosquitto.conf
```

Pretplata na neku temu se može napraviti naredbom:
```
mosquitto_sub -t [Tema] -u [Korisničko ime MQTT posrednika] -P [Zaporka MQTT posrednika]
```

Objava podatka na temu se može napraviti naredbom:
```
mosquitto_pub -t [Tema] -m [Podatak] -u [Korisničko ime MQTT posrednika] -P [Zaporka MQTT posrednika]
```

**Aplikacija koristi MQTT verziju 3.1.1 protokola**, verzija se mijenja uz pomoć *menuconfig*.

Aplikacija radi sljedeće:

- postavlja GPIO pin 22 na kojem je spojen svjetleća dioda kao izlazni što znači da mu se može postaviti stanje i u isto vrijeme čitati s njega
- kada se uspostavi MQTT konekcija na temu "/machine/servo" objavljuje podatak "90", isto tako prijavljuje se na slušanje podataka s teme "/machine/led"
- kada se objavi podatak "ON" na temu "/machine/led", ESP32 pali LED, a kad se objavi podatak "OFF" na temu "/machine/led", ESP32 gasi LED
- ispisuje logove na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)