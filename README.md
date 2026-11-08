This is the Meme-O-Clock PCB.

I built it with the help of a Youtube tutorial, ChatGPT, and KiCAD.

This is my first ever time designing a PCB.

I chose an Arduino Nano ESP 32 as my microcontroller due to the familiar ecosystem and WiFi capabilities.

The arduino will instruct a speaker and display to work as an alarm clock, with custom sounds and objects displayed to wake you up in a personal way.

Light sensor automatically adjusts display brightness based on the room.

Motion sensor will turn on display when motion detected.

MAX98357A amplifier for 5W, 8Ohm Speaker.

The whole project should cost under $30.

![Final PCB 3D Design](<Screenshot 2026-10-11 144437.png>)

![Final Board Layout](<Screenshot 2026-10-11 144421.png>)

![Final Board Schematic](<Screenshot 2026-10-11 214417.png>)

## Bill of Materials (BOM)

All prices are in USD and exclude shipping and tax. Prices are estimates or listed prices and may change.

### 1. PCB Components

| Component | Qty. | Part / Model | Unit Price | Supplier |
|---|---:|---|---:|---|
| Arduino Nano ESP32 | 1 | Arduino Nano ESP32 | $19.30 | [Arduino](https://store-usa.arduino.cc/products/nano-esp32) |
| USB-C receptacle | 1 | GCT USB4105-GF-A | $0.80 | [DigiKey](https://www.digikey.com/en/products/detail/gct/USB4105-GF-A/11198441) |
| JST EH 4-pin connector | 1 | S4B-EH | $0.116 | [GAM-GEC](https://gam-gec.com/product/s4b-eh/) |
| JST EH 2-pin connector | 1 | S2B-EH | $0.14 | [GAM-GEC](https://gam-gec.com/product/s2b-eh/) |
| JST EH 3-pin connector | 1 | S3B-EH | $0.17 | [GAM-GEC](https://gam-gec.com/product/s3b-eh/) |
| 10 kΩ resistor, 0603 | 4 | Stackpole RMCF0603FG10K0 | $0.10 | [DigiKey](https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RMCF0603FG10K0/1761236) |
| 5.1 kΩ resistor, 0603 | 2 | Stackpole RNCF0603DTE5K10 | $0.10 | [DigiKey](https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RNCF0603DTE5K10/6264054) |
| 634 kΩ resistor, 0603 | 1 | Vishay CRCW0603634KFKEA | $0.10 | [DigiKey](https://www.digikey.com/en/products/detail/vishay-dale/CRCW0603634KFKEA/1180130) |
| 6 × 6 mm tactile pushbutton, THT | 4 | Schurter 1301.9317 | $0.373 | [Newark](https://www.newark.com/) |
| I²S audio amplifier | 1 | MAX98357AETE+T | $4.08 | [DigiKey](https://www.digikey.com/en/products/detail/analog-devices-inc-maxim-integrated/MAX98357AETE-T/4936122) |
| Ambient light sensor | 1 | APDS-9306 | TBD | [Broadcom Datasheet](https://docs.broadcom.com/docs/AV02-4755EN) |
| Ceramic capacitor | 5 | 100 nF, 0603 | $0.05 | [TME](https://www.tme.com/us/en-us/details/c0603c104k4rac7081/mlcc-smd-capacitors/kemet/) |
| Ceramic capacitor | 1 | 1 µF, 0603 | $0.13 | [Mouser](https://www.mouser.com/en/c/passive-components/capacitors/ceramic-capacitors/mlccs-multilayer-ceramic-capacitors/multilayer-ceramic-capacitors-mlcc-smd-smt/?capacitance=1+uF&case+code+-+in=0603&dielectric=X7R&termination=Standard&voltage+rating+dc=10+VDC) |
| Ceramic capacitor | 1 | 10 µF, 0603 | $0.06 | [Mylsco](https://spartce.com/products/c1608x5r1a106m080ac-1) |

### 2. External Components

| Component | Qty. Needed | Product | Pack Price | Effective Unit Price | Supplier |
|---|---:|---|---:|---:|---|
| Speaker, 28 mm, 8 Ω, 2 W | 1 | 6-piece pack | TBD | TBD | [Amazon](https://www.amazon.com/dp/B09YGDQV3Z) |
| OLED display, 0.96 in, 128 × 64, SSD1306, I²C | 1 | ELEGOO 3-piece pack | $9.99 | $3.33 | [Amazon](https://www.amazon.com/dp/B0D2RMQQHR) |