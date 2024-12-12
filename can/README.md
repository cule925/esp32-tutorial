# CAN

[CAN (*eng. Controller Area Network*)](https://en.wikipedia.org/wiki/CAN_bus) je serijski komunikacijski protokol koji se koristi u automobilskoj industriji, robotici, željeznicama, avijaciji i slično. CAN protokol je razvila tvrtka Robert Bosch GmbH, a prve integrirane sklopove koji implementiraju CAN protokol razvili su Intel i kasnije Philips.

Uz klasični CAN, postoji i novi standard CAN FD koji nudi brži prijenos podataka.

## Način rada CAN sabirnice

**CAN koristi diferencijalne signale za komunikaciju**. Ti signali se odašilju dvjema sabirnicama:

- CAN H
	- 2.5 V kada je u recesivnom stanju
	- 3.5 V kada je u dominantnom stanju
- CAN L
	- 2.5 V kada je u recesivnom stanju
	- 1.5 V kada je u dominantnom stanju

Ove dvije sabirnice terminirane su na njihovim krajevima otpornicima od 120 Ohma kako bi se smanjila refleksija signala zbog parazitskih induktiviteta i kapaciteta. Brzina prijenosa podataka može biti do 1 Mb/s i **ona se unaprijed postavlja**. Sabirnice su spojene na čvorove spojem otvorenog kolektora. Maksimalne duljine sabirnica mogu biti 40 metara.

Na obje sabirnice se spajaju CAN čvorovi. CAN čvorovi su uređaji koji šalju signale i čitaju signale na CAN sabirnicama:

```
          CAN H +----------------+-------------+-------------+---- .  .  .  . ---------+---------+
                |                |             |             |                         |         |
               +++               |             |             |                         |        +++
               | | 120 Ohm       |             |             |                         |        | | 120 Ohm
               | |               |             |             |                         |        | |
               +++               |             |             |                         |        +++
                |                |             |             |                         |         |
          CAN L +-----------+----|--------+----|--------+----|---- .  .  .  . ----+----|---------+
                            |    |        |    |        |    |                    |    |
                           L|    |H      L|    |H      L|    |H                  L|    |H
                          +-+----+-+    +-+----+-+    +-+----+-+                +-+----+-+
                          | CAN    |    | CAN    |    | CAN    |                | CAN    |
                          | Node   |    | Node   |    | Node   |                | Node   |
                          |        |    |        |    |        |                |        |
                          +--------+    +--------+    +--------+                +--------+
                           _|_           _|_           _|_                       _|_
                           GND           GND           GND                       GND
```

Kada su obje CAN sabirnice u **dominantnom** stanju, CAN čvorovi registriraju logičku **0**. Kada su obje CAN sabirnice u **recesivnom** stanju, CAN čvorovi registriraju logičku **1**. Zadano stanje sabirnice kada se ne šalju nikakve poruke je **recesivno** odnosno **0**.

Fizičke žice CAN H i CAN L se obično međusobno uvrću kako bi smanjile vlastito elektromagnetsko zračenje. Promjena logičkog stanja žica generira promjene u elektromagnetskom poljima oko žica. Kako se promjene napona na žicama događaju u suprotnim smjerovima tijekom promjene stanja, generiraju se elektromagnetska polja suprotnih smjerova oko žica i ona se međusobno poništavaju.

Potrebno je naglasiti da udaljenost CAN čvora do njegovog čvorišta sa sabirnicama treba biti što manja (barem manja od 30 cm) radi smanjenja refleksija signala pogotovo ako se koriste velike brzine prijenosa.

```
                    3.5 V       *---------*    *----*    *----*         *----*
                                |         |    |    |    |    |         |    |
          CAN H                 |         |    |    |    |    |         |    |
                                |         |    |    |    |    |         |    |
                    2.5 V  *----*         *----*    *----*    *---------*    *----*

          -----------------------------------------------------------------------------

                    2.5 V  *----*         *----*    *----*    *---------*    *----*
                                |         |    |    |    |    |         |    |
          CAN H                 |         |    |    |    |    |         |    |
                                |         |    |    |    |    |         |    |
                    1.5 V       *---------*    *----*    *----*         *----*

          -----------------------------------------------------------------------------

                      2 V       *---------*    *----*    *----*         *----*
                                |         |    |    |    |    |         |    |
                                |         |    |    |    |    |         |    |
          Voltage               |         |    |    |    |    |         |    |
          Difference             |         |    |    |    |    |         |    |
                                |         |    |    |    |    |         |    |
                                |         |    |    |    |    |         |    |
                      0 V  *----*         *----*    *----*    *---------*    *----*

          -----------------------------------------------------------------------------

                         1      *---------*    *----*    *----*         *----*
          Logical               |         |    |    |    |    |         |    |
          Value                 |         |    |    |    |    |         |    |
                         0 *----*         *----*    *----*    *---------*    *----*

```

Diferencijalni signali pružaju zaštitu od šumova i smetnji drugih elektroničkih uređaja. Primjerice:

```
                    A                            NOISE ON CAN H
            Voltage |
                    |   CAN H                        *-*
                    |                               *   *--*
              3.5 V |       *------*             *-*        *--*
                    |       |      |             |             |
                    |       |      |             |             |
                    |       |      |             |             |
              2.5 V |-------*      *-------------*             *-------
                    |       |      |             |             |
                    |       |      |             |   *-*       |
                    |       |      |             |  *   *--*   |
              1.5 V |       *------*             *-*        *--*
                    |
                    |   CAN L                    NOISE ON CAN L
                    |
                    |
                    *------------------------------------------------------>
                                                                         Time
          Logical    __________________________________________________
          Value      |  1   |  0   |  1   |  1   |  0   |  0   |  1   |
          
          Voltage    __________________________________________________
          Difference  | 0 V  | 2 V  | 0 V  | 0 V  | 2 V  | 2 V  | 0 V  |
```

### CAN okvir

CAN okvir (*eng. CAN Frame*) je niz bitova kodiranih po CAN protokolu koji sadrži kontrolne bitove, bitove za sinkronizaciju i podatke.

Bosch je 1991. objavio dvije specifikacije CAN-a:

- CAN 2.0A - sadrži 11 bitni identifikator kod okvira
- CAN 2.0B - sadrži 29 bitni identifikator kod okvira

Postoje 4 vrste CAN okvira:

- podatkovni okvir - sadrži podatke
- okvir zahtjeva za podatkom
- okvir greške
- okvir odgađanja slanja

Format osnovnog okvira (11 bitni identifikator) je sljedeći:

```
          *---*----------*---*---*--*---*----*---*-------------*---*-------------*---*---*
          |SOF|IDENTIFIER|RTR|IDE|R0|DLC|DATA|CRC|CRC DELIMITER|ACK|ACK DELIMITER|EOF|IFS|
          *---*----------*---*---*--*---*----*---*-------------*---*-------------*---*---*
```

Dijelovi okvira su:

- početak okvira (*eng. SOF - Start Of Frame*)
	- 1 bit
	- znak za početak transakcije
	- uvijek 0
- identifikator poruke (*eng. Identifier*)
	- 11 bita
	- prioritet poruke
- zahtjev za porukom (*eng. RTR - Remote Transmission Request*)
	- 1 bit
	- označava je li okvir podatkovni (0) ili zahtjev za podatkom (1)
- bit za ekstenziju identifikatora (*eng. IDE - Identifier Extension Bit*)
	- 1 bit
	- ako je 0 onda se koristi osnovni okvir s 11 bitnim identifikatorom, ako je 1 onda se koristi prošireni okvir s 18 bitnim identifikatorom
	- u ovom slučaju je 0
- rezervirani bit (*eng. R0 - Reserved Bit 0*)
	- 1 bit
	- uvijek 0, ali se prihvaća i 1
- duljina podataka u bajtovima (*eng. Data Length Code*)
	4 bita
	- vrijednost koje ovo polje može imati je od 0 do 8
- podatci (*eng. Data*)
	- od 0 do 64 bita
- ciklička provjera redundancije (*eng. CRC - Cyclic Redundancy Check*)
	- 15 bita
	- služi za otkrivanje pogrešaka, ako je pogreška otkrivena, ostali čvorovi šalju okvir greške
- razdjelnik ciklične provjere redundancije (*eng. CRC Delimiter*)
	- 1 bit
	- uvijek 1
- potvrda (*eng. ACK - Acknowledge*)
	- 1 bit
	- čvor koji šalje postavlja ga u 1, čvorovi koji primaju postavljaju 0
- razdjelnik potvrde (*eng. ACK Delimiter*)
	- 1 bit
	- uvijek 1
- kraj okvira (*eng. EOF - End Of Frame*)
	- 7 bita
	- svi su uvijek 1
- razmak između okvira (*eng. IFS - Inter Frame spacing*)
	- 3 bita
	- svi su uvijek 1

Format proširenog okvira (29 bitni identifikator) je sljedeći

```
          *---*------------*---*---*------------*---*--*--*---*----*---*-------------*---*-------------*---*---*
          |SOF|IDENTIFIER A|SRR|IDE|IDENTIFIER B|RTR|R0|R1|DLC|DATA|CRC|CRC DELIMITER|ACK|ACK DELIMITER|EOF|IFS|
          *---*------------*---*---*------------*---*--*--*---*----*---*-------------*---*-------------*---*---*
```

Dijelovi okvira su:

- početak okvira (*eng. SOF - Start Of Frame*)
	- 1 bit
	- znak za početak transakcije
	- uvijek 0
- prvi dio identifikatora poruke (*eng. Identifier A*)
	- prvih 11 bitova
	- prioritet poruke
- zamjena za zahtjev za poruku (*eng. SRR - Substitute Remote Request*)
	- 1 bit
	- uvijek 1
- bit za ekstenziju identifikatora (*eng. IDE - Identifier Extension Bit*)
	- 1 bit
	- ako je 0 onda se koristi osnovni okvir s 11 bitnim identifikatorom, ako je 1 onda se koristi prošireni okvir s 18 bitnim identifikatorom
	- u ovom slučaju je 1
- drugi dio identifikatora poruke (*eng. Identifier B*)
	- drugih 18 bitova
	- prioritet poruke
- zahtjev za porukom (*eng. RTR - Remote Transmission Request*)
	- 1 bit
	- označava je li okvir podatkovni (0) ili zahtjev za podatkom (1)
- prvi rezervirani bit (*eng. R0 - Reserved Bit 0*)
	- 1 bit
	- uvijek 0, ali se prihvaća i 1
- drugi rezervirani bit (*eng. R1 - Reserved Bit 1*)
	- 1 bit
	- uvijek 0, ali se prihvaća i 1
- duljina podataka u bajtovima (*eng. Data Length Code*)
	- 4 bita
	- vrijednost koje ovo polje može imati je od 0 do 8
- podatci (*eng. Data*)
	- od 0 do 64 bita
- ciklička provjera redundancije (*eng. CRC - Cyclic Redundancy Check*)
	- 15 bita
	- služi za otkrivanje pogrešaka, ako je pogreška otkrivena, ostali čvorovi šalju okvir greške
- razdjelnik ciklične provjere redundancije (*eng. CRC Delimiter*)
	- 1 bit
	- uvijek 1
- potvrda (*eng. ACK - Acknowledge*)
	- 1 bit
	- čvor koji šalje postavlja ga u 1, čvorovi koji primaju postavljaju 0
- razdjelnik potvrde (*eng. ACK Delimiter*)
	- 1 bit
	- uvijek 1
- kraj okvira (*eng. EOF - End Of Frame*)
	- 7 bitova
	- svi su uvijek 1
- razmak između okvira (*eng. IFS - Inter Frame spacing*)
	- 3 bitova
	- svi su uvijek 1

#### Umetanje bitova

Kako bi se održala sinkronizacija tijekom dugog niza istih bitova, nakon niza od 5 istih bitova čvor koji šalje umeće bit (*eng. Bit Stuffing*) koji je suprotne vrijednosti od prethodnih 5 bitova tijekom slanja. Čvorovi koji prime takav niz bitova izvlače i ignoriraju te umetnute bitove jer znaju da se oni nalaze iza niza od 5 istih bitova. To se događa na bilo kojem dijelu okvira. Primjerice, ako je 11 bitna vrijednost identifikatora poruke 00001111001, početni dio osnovnog okvira bez umetanja bitova bi izgledao ovako: 000001111001... . Nakon umetanja bitova, stvarni okvir bi izgledao ovako: 00000**1**1111**0**001... . Umetanje bitova povećava veličinu okvira, pogotovo ako ima mnogo uzastopno istih bitova.

### Arbitraža sabirnice

CAN čvorovi u isto vrijeme šalju okvire i prisluškuju stanje sabirnice. U slučaju da dva ili više CAN čvora žele slati okvire na sabirnicu u isto vrijeme, prednost će se dati onome čija je sljedeći bit u **dominantnom** stanju odnosno 0. Čvor koji je u tom trenutku postavio bit u **recesivnom** stanju odnosno 1, a detektirao je **dominantno** stanje odnosno 0 na sabirnici (otvoreni kolektor), će odustati od daljnjeg slanja. Ovo dovodi do koncepta prioriteta poruke uz pomoć identifikatora. Poruke s nižom vrijednosti identifikatora će imati prednost nad porukama s višom vrijednosti identifikatora.

```
                                                                      GIVES UP
                                                                         |
                                                                         V
                        1 -----*     *-----*           *-----*           *-----------------------------
          CAN NODE 1           |     |     |           |     |           |
                        0      *-----*     *-----------*     *-----------*
                               <SOF:0><IDENTIFIER:10010010011___...




                        1 -----*     *-----*           *-----*                             *-----*
          CAN NODE 2           |     |     |           |     |                             |     |
                        0      *-----*     *-----------*     *-----------------------------*     *-----
                               <SOF:0><IDENTIFIER:10010000010___...

                                                                                  GIVES UP
                                                                                     |
                                                                                     V
                        1 -----*     *-----*           *-----*                       *-----------------
          CAN NODE 3           |     |     |           |     |                       |
                        0      *-----*     *-----------*     *-----------------------*
                               <SOF:0><IDENTIFIER:10010000100___...




          ACTUAL READ   1 -----*     *-----*           *-----*                             *-----*
          BUS STATE BY         |     |     |           |     |                             |     |
          ALL NODES     0      *-----*     *-----------*     *-----------------------------*     *-----


          IDENTIFIER COMPARISON
          CAN NODE 1: 10010010011
          CAN NODE 2: 10010000010 ----> lowest value, highest priority
          CAN NODE 3: 10010000100
```

### ESP32 i CAN

[ESP32 podupire oba načina rada klasičnog CAN-a](https://docs.espressif.com/projects/esp-idf/en/release-v3.3/api-reference/peripherals/can.html), s 11 bitnim identifikatorom poruke i s 29 bitnim identifikatorom poruke. Međutim, ESP32 ima ugrađen samo CAN upravljač, ali ne i CAN primopredajnik čiji bi se izlazi spajali sabirnice CAN H i CAN L. Zato je potrebno koristiti zaseban CAN primopredajnik:

```
                  *-----------------*       *-----------------*              *-----------------*
                  | ESP32           |       | ESP32           |              | ESP32           |
                  |                 |       |                 |              |                 |
                  | *-------------* |       | *-------------* |              | *-------------* |  
                  | | CAN         | |       | | CAN         | |              | | CAN         | |
                  | | Controller  | |       | | Controller  | |              | | Controller  | |
                  | *--|-------|--* |       | *--|-------|--* |              | *--|-------|--* |
                  |  TX|       |RX  |       |  TX|       |RX  |              |  TX|       |RX  |
                  *----|-------|----*       *----|-------|----*              *----|-------|----*
                       |       |                 |       |                        |       |
                    *--|-------|--*           *--|-------|--*                  *--|-------|--*
                    | CAN         |           | CAN         |                  | CAN         |
                    | Transceiver |           | Transceiver |                  | Transceiver |
                    *--|-------|--*           *--|-------|--*                  *--|-------|--*
                 CAN L |       | CAN H     CAN L |       | CAN H            CAN L |       | CAN H
                       |       |                 |       |                        |       |
                       |       |                 |       |                        |       |
           CAN H       |       |                 |       |                        |       |       CAN H
           +-----------|-------+-----------------|-------+-------- . . . ---------|-------+-----------+
           |           |                         |                                |                   |
          +++          |                         |                                |                  +++
          | | 120 Ohm  |                         |                                |          120 Ohm | |
          | |          |                         |                                |                  | |
          +++          |                         |                                |                  +++
           |           |                         |                                |                   |
           +-----------+-------------------------+---------------- . . . ---------+-------------------+
           CAN L                                                                                  CAN L
```
