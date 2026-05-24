# AVR Slot Machine

Slot machine functional implementat pe Arduino Uno R3.

## Componente
- Arduino Uno R3
- Display TFT 1.8" ST7735S
- Modul DFPlayer Mini + difuzor 8 ohm
- 4x butoane arcade 24mm
- Card microSD cu fisiere MP3

## Compilare
Proiect PlatformIO. Deschide in VSCode si apasa Upload.

## Conexiuni
- TFT: CS=D10, RST=D9, DC=D8, MOSI=D11, SCK=D13
- DFPlayer: RX=D7, TX=D6 (1k rezistenta pe TX)
- Butoane: SPIN=D2, BET+=D3, RED=D4, BLACK=D5