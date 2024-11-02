# ESP32

[ESP32](https://www.espressif.com/en/products/socs/esp32) je serija sustava na čipu (*eng. System on Chip - SoC*) koju je razvio Espressif Systems. Serija sustava na čipu dolazi s integriranim Wi-Fi i Bluetooth radijima što je pogodno za sustave Interneta stvari (*eng. Internet of Things - IoT*).

## Pojmovi

### ESP32 sustav na čipu

Sustav na čipu (*eng. System on Chip*) je integrirani sklop koji sadrži većinu bitnih komponenti računala, primjerice: procesorske jezgre (ARM), sabirnice (AMBA), memoriju (SRAM, DRAM, ROM) ili sučelja za pristup memoriji, ulazno-izlazna sučelja (GPIO, USART, SPI, I2C, USB ...), sučelja za pristup pohranama, dodatne procesorske jezgre za specifične namjene, mrežna sučelja i mrežni stog (Ethernet, Wi-Fi, Bluetooth ...). Jedan od ciljeva SoC-ova je mala potrošnja energije i smanjenje topline koju generira. Komponente su razmještene relativno blizu (na jednom jedinom čipu) pa su gubitci energije (kao toplina) i greške komunikacije između njih minimalne.

Espressif nudi nekoliko [serija ESP32 SoC-ova](https://www.espressif.com/en/products/socs), a neke [bitnije za napomenuti su](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/hw-reference/chip-series-comparison.html):

- ESP32 serija
- ESP32-C3 serija
- ESP32-S2 serija

### ESP32 modul

ESP32 modul (*eng. ESP32 module*) je gotov sklop koji sadrži ESP32 SoC i osnovne komponente za ispravan rad kao primjerice kristal za takt, potrebne otpornike i kondenzatore te antenu. Popis modula može se naći [ovdje](https://www.espressif.com/en/products/modules).

### ESP32 razvojni komplet

ESP32 razvojni komplet (*eng. ESP32 DevKit*) je razvojna ploča za testiranje i razvoj. Sadrži ESP32 modul i komponente za napajanje (primjerice regulatore napona), komponente za programiranje (primjerice USB-UART mostove), razna tipkala (primjerice reset), svjetleće diode i pristup GPIO pinovima. Olakšavaju rad i razvoj s ESP32 modulima.

Popis razvojnih kompleta se može naći [ovdje](https://www.espressif.com/en/products/devkits).

## ESP-IDF

[ESP-IDF](https://idf.espressif.com/) je razvojna okolina za ESP32 razvojne komplete koju je razvio Espressif. Nudi skup alata za razvoj softvera (*eng. Software Development Kit*) za ESP32. Programi se pišu u C ili C++ programskim jezicima. Trenutno se nudi kao plugin za VSCode i Eclipse, ali moguća je i ručna instalacija.

### Instalacija ESP-IDF plugina za VSCode na Arch Linuxu

Prije instalacije ESP-IDF plugina, potrebno je instalirati sam VSCode razvojno okruženje te Python i PIP (Python upravitelj paketa). Prvo je potrebno ažurirati sustav naredbom:

```
sudo pacman -Syu
```

Nakon ažuriranja sustava, paketi se mogu instalirati naredbom:

```
sudo pacman -S code python python-pip
```

Zatim je potrebno pokrenuti VSCode:

```
code
```

Nakon pokretanja VSCodea, potrebno je otići na karticu *Extensions* (kombinacija tipki *Ctrl+Shift+X*) i upisati "ESP-IDF" te kliknuti *Install*. Nakon instalacije ekstenzije potrebno je kliknuti na padajući izbornik *View* i odabrati *Command Palette* (odnosno tipka *F1*) i upisati "ESP-IDF: Configure ESP-IDF Extension" te odabrati opciju *EXPRESS*. Prije instalacije, potrebno je postaviti sljedeće postavke:

- Select download server: **GitHub**
- Select ESP-IDF version: **v5.3.1 (release version)**
	- ova opcija je po želji
- Enter ESP-IDF container directory: **{korisnički home direktorij}**
	- ovo je vjerojatno već odabrano, primjerice za korisnika *alfred* odabrani direktorij je */home/alfred/esp*
	- sadrži radnu okolinu za ESP32 seriju; razne biblioteke, alate specifične za radnu okolinu i primjere koda
	- moguće je imati instalirano više verzija radne okoline
- Enter ESP-IDF Tools directory (IDF_TOOLS_PATH):
	- ovo je također vjerojatno već odabrano, primjerice za korisnika *alfred* odabrani direktorij je */home/alfred/.espressif*
	- sadrži unakrsne prevoditelje (*eng. cross compilers*) za ESP32 seriju te virtualno Python okruženje koje omogućuje instalaciju specifičnih verzija Python paketa koji su izolirani od sustavskih Python paketa
- Select Python version:
	- vjerojatno već odabrano */usr/bin/python*, ako nije potrebno je odabrati tu putanju

Za kraj potrebno je kliknuti *Install* za instalaciju.

### Instalacija ESP-IDF lanca alata preko terminala na Arch Linuxu

Ako se želi [samostalno instalirati ESP-IDF za Arch Linux](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/get-started/linux-macos-setup.html), prvo je potrebno instalirati potrebne pakete:

```
sudo pacman -S --needed gcc git make flex bison gperf python cmake ninja ccache dfu-util libusb
```

Prije preuzimanja ESP-IDF GitHub repozitorija, potrebno je stvoriti direktorij *esp* u korisnikovom *home* direktoriju i premjestiti se u njega:

```
mkdir ~/esp
cd ~/esp
```

Nakon toga, preuzimanje verzije **v5.3.1** s GitHuba može se napraviti naredbom:

```
git clone -b v5.3.1 --recursive https://github.com/espressif/esp-idf.git
```

Sada je potrebno premjestiti se u taj direktorij i izvršiti skriptu *install.sh* koja će instalirati potrebne pakete, odgovarajući prevoditelj, debugger i slično za sve ESP32 mikroupravljače:

```
cd ~/esp/esp-idf
./install.sh all
```

Umjesto stavke *all*, moglo se postaviti recimo *esp32* ili *esp32c3* odnosno instalacija paketa za konkretni mikroupravljač. Stavka *all* će instalirati pakete za sve ESP32 mikroupravljače. Naredba će stvoriti direktorij *.espressif* u korisničkom direktoriju koja će sadržavati navedene pakete i unakrsne prevoditelje. Alati koji su instalirani nisu dostupni van direktorija *esp-idf* i *.espressif* pa je potrebno postaviti varijable okruženja izvršavanjem skripte *export.sh*:

```
. ~/esp/esp-idf/export.sh
```

Prethodna naredba je ekvivalent naredbi ```source ~/esp/esp-idf/export.sh``` i ona će postaviti varijable okruženja trenutnoj ljusci terminala (*bash*). Problem je što naredbe dostupne samo ovoj instanci ljuske. Kako bi naredbe bile dostupne svakoj ljusci izvršenjem naredbe ```get_idf``` u terminalu, potrebno je u *.bashrc* dodati liniju:

```
...
alias get_idf='. $HOME/esp/esp-idf/export.sh'
```

## ESP32 razvojni komplet

Razvojni komplet koji će se koristiti u ovom projektu je [**ESP32-DevKitM-1**](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/index.html). Sadrži [ESP32-MINI-1 modul](https://www.espressif.com/sites/default/files/documentation/esp32-mini-1_datasheet_en.pdf), USB-UART most za programiranje (Micro USB), regulator napona, svjetleću diodu i dva tipkala (reset i boot) te razvedene GPIO pinove. Modul dolazi s antenom i [ESP32-U4WDH sustavom na čipu](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf). Više informacija o razvojnoj pločici se može naći [ovdje](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/user_guide.html).

Neke specifikacije ESP32-U4WDH SoC-a:

- 32-bitni Xtensa dvojezgreni (noviji) ili Xtensa jednojezgreni (stariji) procesor, maksimalna frekvencija 240 MHz
- 520 KiB SRAM (16 KiB za cache memoriju)
- 448 KiB ROM
- 4 MiB Flash memorije
- 16 KiB SRAM u RTC
- 34 GPIO-a, 4 SPI-a, 3 UART-a, 2 I2C-a, 2 I2S-a, 12-bitni ADC, PWM ...
- RF modul za Wi-Fi i Bluetooth
- radni napon od 3 V do 3.6 V

### Povezivanje putem USB-a

Pločica koristi CP210x seriju USB-UART mostova te Linux jezgra već ima u sebi ugrađen upravljački program koji omogućuje upravljanje tip čipom. Dostupnost upravljačkog programa može se provjeriti naredbom:

```
ls /lib/modules/6.11.4-arch2-1/kernel/drivers/usb/serial/ | grep cp210x
```

Nakon što se priključi razvojna pločica, upravljački program će stvoriti datoteku uređaja koja predstavlja spojenu pločicu (najvjerojatnije */dev/ttyUSB0*). Kako bi se mogao pisati program na pločici, potrebno je omogućiti pisanje i čitanje na datoteku uređaja koja predstavlja taj uređaj za sve ostale korisnike. Primjerice, ako je spojena pločica predstavljena s */dev/ttyUSB0*:

```
sudo chmod o+rw /dev/ttyUSB0
```

Kako saznati koja datoteka uređaja predstavlja pločicu može se napraviti naredbom:

```
dmesg
```

Naredba *dmesg* ispisuje zadnje zapise Linux jezgre. Kad se uređaj spoj na USB računalo, USB upravljački program će ga predstaviti datotekom uređaja te će to zapisati u jezgrine zapise.

Međutim, svaki put kad se isključi i uključi pločica na računalo preko USB-a potrebno je promijeniti ovlasti uz pomoć prethodno navedene *chmod* naredbe. Kako bi eliminirali izvršavanje te naredbe pri svakom ponovnom priključenju pločice, potrebno je trenutnog korisnika dodati u grupu *uucp*. Svi serijski uređaji pripadaju toj grupi i korisnici koji su u toj grupi automatski imaju pravo pisanja i čitanja na te serijske uređaje preko njihovih datoteka uređaja. Dakle, korisnika se dodaje u grupu naredbom:

```
sudo usermod -aG uucp $USER
```

Potrebno je odjaviti se s računala i ponovno se prijaviti.

### Stvaranje novog projekta

#### U VSCode razvojnom okruženju

Novi projekt se može stvoriti pritiskom na tipku *F1* i upisivanjem "ESP-IDF: New Project". Potrebno je postaviti sljedeće postavke:

- Project Name: **{po želji}**
	- primjerice *my-simple-esp32-project*
- Enter Project directory: **{korisnički home direktorij}/{po želji}**
	- ovo je vjerojatno već odabrano, primjerice za korisnika *alfred* i ime po želji ime direktorija *esp-project-directory*, odabrani direktorij je */home/alfred/esp-project-directory*
- Choose ESP-IDF Board: **ESP32 chip (via ESP-PROG)**
	- za trenutnu pločicu, odabire alat za programiranje i debagiranje
	- opcija se može kasnije promijeniti u projektu pritiskom na tipku *F1* i odabirom "ESP-IDF: Set Espressif Device Target"
- Choose serial port: **/dev/ttyUSB0**
	- datoteka uređaja koja predstavlja pločicu
	- opcija se može kasnije promijeniti u projektu pritiskom na tipku *F1* i odabirom "ESP-IDF: Select Port to Use:"
- Add your ESP-IDF Component directory: **{ništa}**

Zatim je potrebno kliknuti na "Choose Template", odabrati "ESP-IDF" u padajućem izborniku, odabrati "sample_project" te kliknuti na "Create project using template sample_project". Zatim će se u kutu pojaviti prozor koji će biti upit za otvaranje novostvorenog projekta. U tom slučaju potrebno je kliknuti "Yes" (ili se može jednostavno otvoriti klikanjem na "File" i lokacija projekta). Nakon toga, ako se pojavi prozor "Do you trust the authors of the files in this folder?" potrebno potrebno je i odabrati "Yes, I trust the authors".

#### U terminalu

Ako već postoji *alias* *get_idf* u *.bashrc* datoteci koji izvršava skriptu *export.sh*, prilikom otvaranje nove instance terminala potrebno je izvršiti naredbu:

```
get_idf
```

Nakon toga je potrebno stvoriti direktorij *esp-project-directory* u korisničkom *home* direktoriju gdje će se spremati ESP32 projekti i premjestiti se u njega:

```
mkdir ~/esp-project-directory
cd ~/esp-project-directory
```

Zatim je potrebno kopirati *sample_project* u trenutni direktorij i nazvati ga *my-simple-esp32-project* te premjestit se u njega:

```
cp -r $IDF_PATH/examples/get-started/sample_project my-simple-esp32-project
cd my-simple-esp32-project
```

### Početna konfiguracija

#### U VSCode razvojnom okruženju

ESP32-U4WDH SoC koji se koristi u modulu na razvojnoj pločici je u početku bio zamišljen kao SoC koji sadrži jednu procesorsku jezgru, no međutim 2021 godine donesena je odluka da ESP32-U4WDH ipak bude sadržavao dvije procesorske jezgre. Ovo znači da neki stariji moduli sadrže jednojezgrene Xtensa procesorske jezgre. Ako je modul na pločici stariji (jednojezgreni), potrebno je pritisnuti tipku *F1* i upisati "ESP-IDF: SDK Configuration Editor (Menuconfig)", onda u tražilicu pretražiti "Run FreeRTOS only on first core" i označiti opciju kvačicom. Ovo će pokretati operacijski sustav za rad u stvarnom vremenu *FreeRTOS* na jednoj jezgri. Također, uobičajeno je postavljeno iskoristivo 2 MiB Flash memorije iako postoji ugrađeno 4 MiB. Ovo se može promijeniti upisujući u tražilicu "Flash size" i mijenjanje joj vrijednosti na 4 MiB. Na kraju ovih promjena potrebno je kliknuti "Save".

Ostale bitne operacije koje se mogu izvest nad projektom su:

- izgradnja projekta: tipka *F1* i odabir "ESP-IDF: Build your project"
- prijenos izvršnog koda na pločicu (USB-UART): tipka *F1* i odabir "ESP-IDF: Flash (UART) your Project"
- otvaranje serijskog monitora na pločici: tipka *F1* i odabir "ESP-IDF: Monitor Device"
- izgradnja, prijenos i otvaranje serijskog monitora na pločici: tipka *F1* i odabir "ESP-IDF: Build, Flash and Start a Monitor on your Device"
	- prekid serijske komunikacije: tipka *Ctrl* i tipka *]* (engleska tipkovnica)
- brisanje Flash memorije: tipka *F1* i odabir "Erase Flash Memory From Device"

#### U terminalu

Za početak, potrebno je postaviti cilj izgradnje na *esp32*:

```
idf.py set-target esp32
```

U repozitoriju projekta je prvo ako je potrebno namjestiti FreeRTOS u višejezgrenom načinu rada ili jednojezgrenom načinu rada zbog prethodno objašnjenog razloga te postaviti dostupnu Flash memoriju na 4 MiB. To se može napraviti izvršavajući naredbu:

```
idf.py menuconfig
```

Potrebno je odabrati sljedeće opcije:

- Component config -> FreeRTOS -> Kernel -> Run FreeRTOS only on first core (tipka *Space* za označavanje, tipka *S* za spremanje)
- Serial flasher config -> Flash size -> 4 MB (tipka *Space* za odabir, tipka *S* za spremanje)

Ostale bitne operacije koje se mogu izvest nad projektom su:

- izgradnja projekta:

```
idf.py build
```

- prijenos izvršnog koda na pločicu (USB-UART) i izgradnja projekta ako već nije izgrađen (pretpostavka je da pločica predstavljena s */dev/ttyUSB0* datotekom):

```
idf.py -p /dev/ttyUSB0 flash
```

- otvaranje serijskog monitora na pločici:

```
idf.py -p /dev/ttyUSB0 monitor
```

- prekid serijske komunikacije: tipka *Ctrl* i tipka *]* (engleska tipkovnica)

- brisanje Flash memorije:

```
idf.py -p /dev/ttyUSB0 erase-flash
```
