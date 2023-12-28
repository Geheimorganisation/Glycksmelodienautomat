# Glücksmelodienautomat

Hier erfahren Sie Ihre persönliche Glücksmelodie.

https://www.youtube.com/watch?v=8lp-MERd6BQ

Der ursprüngliche Nachbau war 2016 auf dem 33. Chaos Communication Congress zu sehen:

https://www.flickr.com/photos/schwarzbrot/31279464133/

Hier beschrieben ist der Automat aus 2023, der auf dem 37c3 zu sehen sein wird.
Grundlegend handelt es sich um den Automaten aus 2015.
Der Raspberry Pi wurde allerdings gegen einen Teensy LC und einen YX 5300 MP3 Player ausgetauscht.

## Hardware

### Mikrocontroller: Teensy LC

Herstellerseite: https://www.pjrc.com/teensy/teensyLC.html

Das Board wird nicht mehr hergestellt.
Bei Bedarf sollte es allerdings kein Problem sein, ein anderes Arduino kompatibles Board zu verwenden.

### MP3 Player: YX5300 v1.0

Zusätzlich ist eine Micro SD Karte notwendig.
Die Micro SD Karte muss mit dem FAT Dateisystem formatiert sein.

Die Dateien müssen nach dem Schema 000xxx.mp3, 001xxx.mp3, … benannt werden.
Tracks werden anhand ihrem Index im Dateisystem aufgerufen.
Dazu müssen die Dateien im FAT Dateisystem geordnet vorliegen.
Um das sicherzustellen eignet sich z.B. dieses Programm:
[FatDriveSorter](https://github.com/lwouis/fat-drive-sorter).

Verwendete Bibliothek und Datenblatt: https://github.com/MajicDesigns/MD_YX5300

Verbindung mit dem Mikrocontroller:

| Pin name | Teensy |
| -------- | ------ |
| RX       | 10     |
| TX       | 9      |
| VCC      | 3V3    |
| GND      | GND    |

### Keypad

Das Keypad wurde ursprünglich bei Conrad mit der Artikelnummer 709840 verkauft, ist heute aber nicht mehr erhältlich.

Schaltplan: https://asset.conrad.com/media10/add/160267/c1/-/en/000709840DS01/datasheet-709840-tlacidlova-klavesnica-mriezka-klavesnice-3-x-4-1-ks.pdf

Zusätzliche Infos zur Nutzung von Matrix Keypads: https://learn.adafruit.com/matrix-keypad

Verwendete Bibliothek: https://github.com/Chris--A/Keypad

Das Pinout ist, von der Rückseite betrachtet und von links nach rechts, folgendermaßen:

```
ROW4 | ROW3 | ROW2 | ROW1 | COL3 | COL2 | COL1
```

Verbindung mit dem Mikrocontroller:

| Pin name | Teensy |
| -------- | ------ |
| ROW4     | 0      |
| ROW3     | 1      |
| ROW2     | 2      |
| ROW1     | 3      |
| COL3     | 4      |
| COL2     | 5      |
| COL1     | 6      |

### Bewegungssensor: HC SR501

Datenblatt: https://cdn-reichelt.de/documents/datenblatt/A300/SEN-HC-SR501-ANLEITUNG-23.09.2020.pdf

Verbindung mit dem Mikrocontroller:

| Pin name | Teensy |
| -------- | ------ |
| VCC      | 3V3    |
| DATA     | 7      |
| GND      | GND    |

### Münzeinwurf

Der Münzeinwurf enthält eine Lichtschrank, um den Einwurf zu erkennen.
Die Lichtschranke besteht aus einer LED, einer Photodiode und einer kleinen Lochrasterplatine mit unbekanntem Schaltkreis.

Das Pinout der Lochrasterplatine ist, von links nach rechts, folgendermaßen:

```
3V3 | DATA | GND
```

Standardmäßig ist DATA auf HIGH.
Wird die Lichtschranke durch einen Münzwurf durchbrochen, wechselt DATA auf LOW.

Verbindung mit dem Mikrocontroller:

| Pin name | Teensy |
| -------- | ------ |
| 3V3      | 3V3    |
| DATA     | 8      |
| GND      | GND    |

### Display

Beim Display handelt es sich um ein schwarz-weißes 1,3" SH1106 128x64 Pixel I2C OLED Display.

| Pin name | Teensy |
| -------- | ------ |
| 3V3      | 3V3    |
| GND      | GND    |
| SDA      | 18     |
| SDL      | 19     |

Der QR-Code wird folgendermaßen vorproduziert:

1. Einen Redeem-Token im Hub-Backoffice anlegen.
2. Die URL auf z.B. [is.gd](https://is.gd/) kürzen.
3. Einen QR-Code mit [qrcode](https://www.npmjs.com/package/qrcode) erstellen: `npx qrcode -q 1 -o ~/qr-redeem.png https://is.gd/xxxxxx`. Durch die Einstellungen sollte eine 54x54 Pixel große Grafik erstellt werden.
4. Den QR-Code mit [image2cpp](https://javl.github.io/image2cpp/) in ein Byte-Array umwandeln. Einstellungen:
   - Canvas Size: 128x64
   - Background Color: Black
   - Center image: horizontally, vertically
   - Swap bits in byte: swap
   - Alles weitere auf Standard—Einstellungen belassen.
5. Das Byte-Array in der main.cpp ersetzen.

## Software

Die Software ist mit [PlatformIO](https://platformio.org/) im Arduino Environment umgesetzt.

Zur Entwicklung, Kompillierung und zum Flashen bitte die Anleitungen von PlatformIO beachten!
Im Projekt ist eine VSCode-Konfiguration für PlatformIO enthalten.

## Audio Samples

Die Audio-Samples liegen geschützt im Regen unter Projekte → Glücksmelodie 37c3 2023.
Der Ordner `01/` kann 1:1 auf die Micro SD kopiert werden.

## Lizenz

Software + Hardware: Siehe [LICENSE](LICENSE)
