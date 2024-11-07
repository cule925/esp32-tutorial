# TASKS-QUEUE

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Nije potrebno spajati nikakve dodatne komponente na pločicu.

Aplikacija radi sljedeće:

- stvara jedan red
- stvara tri zadatka: jedan zadatak koji piše u red (blokirajuće), jedan zadatak koji čita iz reda i jedan zadatak koji iščitava koliko elemenata ima u redu
- ispisuje logove svih zadataka na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)

U inicijalnoj konfiguraciji, veličina reda je 5. Jedan zadatak piše svake 0.7 sekundi u red (i tako 10 puta) dok drugi zadatak čita iz reda svakih 1.5 sekundi (i tako 10 puta). Ovo znači da će se red brže puniti nego prazniti što znači da zadatak neće stići sve svoje elemente upisati u red i bit će blokiran prije nego li zadatak koji iščitava isprazni elemente iz reda. Slikovito svakih 1.5 sekundi red bi izgledao ovako:

```
              element                   direction
              that is                   of writing
              being read                the elements

                 ||                     <==========
                 \/
        
        QUEUE: |[A]| B |   |   |   |         1.5 seconds

        QUEUE: |[B]| C | D |   |   |         3 seconds

        QUEUE: |[C]| D | E | F |   |         4.5 seconds

        QUEUE: |[D]| E | F | G | H |         6 seconds

        QUEUE: |[E]| F | G | H | I |         7.5 seconds

        QUEUE: |[F]| G | H | I | J |         9 seconds

        QUEUE: |[G]| H | I | J |   |         10.5 seconds

        QUEUE: |[H]| I | J |   |   |         10.5 seconds

        QUEUE: |[I]| J |   |   |   |         10.5 seconds

        QUEUE: |[J]|   |   |   |   |         10.5 seconds

        ...
```

Nakon što se red napuni, zadatak koji piše ostaje blokiran sve dok zadatak koji čita ne pročita element iz reda. Tada se od blokirani zadatak oslobađa i piše svoj podatak u red nakon čega je opet red pun i zadatak koji piše će se ponovno blokirati pri ponovnoj želji za pisanje u red. Nakon nekog vremena ova gužva u redu će se smanjiti jer će zadatku koji piše ponestati elemenata za pisanje, a zadatak koji čita će lagano rješavati (čitati) elemente iz reda. Broj elemenata u redu ispisuje posebni zadatak.