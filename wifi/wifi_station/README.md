# WIFI-STATION

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Nije potrebno spajati nikakve dodatne komponente na pločicu.

Aplikacija radi sljedeće:

- inicijalizira NVS particiju za spremanje SSID-a i zaporke Wi-Fi mreže
- inicijalizira *handlere* i sinkronizacijski mehanizam *group handler*
- inicijalizira TCP/IP stog, stvara mrežno sučelje koje funkcionira kao STATION način rada i postavlja ESP32 mikroupravljaču ime koje će se koristiti na mreži (*eng. hostname*)
- inicijalizira Wi-Fi stog i Wi-Fi upravljački program
- povezuje se na definiranu mrežu, ako ne uspije to napraviti pokušat će to u ovom slučaju još 5 puta
- ispisuje logove na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)

**Za isprobavanje na vlastitoj mreži potrebno je promijeniti vrijednosti makroa ESP_WIFI_SSID i ESP_WIFI_PASS. Isto tako, ako se želi promijeniti ime ESP32 mikroupravljača na mreži potrebno je promijeniti vrijednost makroa ESP_WIFI_HOSTNAME. Naposljetku, ako se želi promijeniti vrijednost ponovnih pokušaja prijave na mrežu, to se radi uređivanjem vrijednosti makroa ESP_WIFI_RETRY_NUM.**