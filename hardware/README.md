# Controller prototype schematic

Open [controller_prototype.kicad_pro](controller_prototype.kicad_pro) in **KiCad 7 or newer**, then open its schematic. [controller_prototype.pdf](controller_prototype.pdf) is a printable A3 preview. The project includes a local symbol library and an embedded symbol cache; no third-party library installation is needed. [bom.csv](bom.csv) lists the 55 physical components.

This is a complete **module-based reference circuit derived from the firmware**, rather than a reconstruction of an existing device. The code determines the signals and scan behavior, but cannot identify the original board, controls, power source, or mechanical parts. Those choices are explicit below. Firmware files are unchanged.

## Evidence and implementation choices

| Source in `../controller_prototype.ino` | Hardware consequence |
| --- | --- |
| `cols[] = {2, 3, 4, 5, 6}` | Five columns on Arduino D2–D6, in that order. |
| `rows[] = {7, 8, 9, 10, 11}` | Five rows on Arduino D7–D11, in that order. |
| `(rowIndex * colCount) + colIndex` | 25 digital channels, numbered 0–24, ordered by row. |
| Selected column is `OUTPUT LOW`; other columns return to `INPUT_PULLUP` | Normally open switches pull a row low through the selected column. |
| `PORTB = 0x0F`, `PORTC = 0x00`, `PORTD = 0xFC` and the ATmega328P comment | Use the classic ATmega328P Arduino pin mapping; digital matrix pins have pull-ups, while A0/A1 pull-ups are disabled at startup. |
| `analogPins[] = {A0, A1}` and `analogRead()` | Two analog inputs using the default supply-referenced ADC. |
| `Serial.begin(115200)` and `Serial.write()` | UART data, carried to the host by the chosen module's USB serial bridge. |

**Chosen implementation:** A1 is a classic **Arduino Nano v3.x, ATmega328P, 5 V / 16 MHz**, powered through its onboard USB connector. Its onboard USB serial bridge, regulator, clock, reset network, and MCU decoupling are included in the purchased module. This is not a bare MCU design. Nano Every, Nano 33, Nano ESP32 and ATmega32U4 boards are not substitutes for this firmware's direct register writes. The firmware sends a serial protocol; the circuit does not provide native USB HID.

SW1–SW25 are normally open momentary pushbuttons. Each has a 1N4148 isolation diode to support simultaneous presses without matrix ghosting. These diodes are an engineering addition; the firmware does not prove whether the original hardware had them. **The cathode stripe faces the column; the anode faces the switch/row.** Current flows from a pulled-up row, through a closed switch and its diode, into the selected low column. No external pull-up or debounce resistors are needed for the buttons: the MCU supplies the pull-ups, and the firmware debounces over three scans at a nominal 10 ms update period.

RV1 and RV2 are assumed to be **10 kΩ linear potentiometers** (two joystick axes could be wired identically). Potentiometer end terminals go to +5 V and GND; the wiper goes to its ADC input. C1/C2 are chosen 100 nF ceramic wiper filters to ground, placed near A1. The firmware alone cannot establish the analog control type, resistance, or filtering. A different sensor must provide a common-ground signal within 0–5 V; do not leave either input floating.

**Analog input configuration:** `PORTC = 0x00` disables the internal pull-ups on A0/A1 at startup. The potentiometer dividers set the input voltages without internal pull-up bias; no pull-up compensation is needed. Keep both wipers connected to their ADC inputs so the inputs have a defined DC voltage.

## Wiring and channel order

Arduino names and Nano physical pad numbers are different:

| Net | Arduino pin | Nano pad |
| --- | --- | --- |
| COL0_D2 | D2 | 5 |
| COL1_D3 | D3 | 6 |
| COL2_D4 | D4 | 7 |
| COL3_D5 | D5 | 8 |
| COL4_D6 | D6 | 9 |
| ROW0_D7 | D7 | 10 |
| ROW1_D8 | D8 | 11 |
| ROW2_D9 | D9 | 12 |
| ROW3_D10 | D10 | 13 |
| ROW4_D11 | D11 | 14 |
| ANALOG0_A0 | A0 | 19 |
| ANALOG1_A1 | A1 | 20 |
| +5V | 5V | 27 |
| GND | GND | 4 and 29 |

Each cell below is `switch / digital channel`. The channel numbers are zero based, matching the serial protocol:

| Row | COL0 / D2 | COL1 / D3 | COL2 / D4 | COL3 / D5 | COL4 / D6 |
| --- | --- | --- | --- | --- | --- |
| ROW0 / D7 | SW1 / 0 | SW2 / 1 | SW3 / 2 | SW4 / 3 | SW5 / 4 |
| ROW1 / D8 | SW6 / 5 | SW7 / 6 | SW8 / 7 | SW9 / 8 | SW10 / 9 |
| ROW2 / D9 | SW11 / 10 | SW12 / 11 | SW13 / 12 | SW14 / 13 | SW15 / 14 |
| ROW3 / D10 | SW16 / 15 | SW17 / 16 | SW18 / 17 | SW19 / 18 | SW20 / 19 |
| ROW4 / D11 | SW21 / 20 | SW22 / 21 | SW23 / 22 | SW24 / 23 | SW25 / 24 |

RV1 is analog channel 0; RV2 is analog channel 1. Dn belongs to SWn.

Power the module through **USB only** for this reference implementation. The external +5 V rail is supplied by A1 pad 27. VIN, 3V3, AREF, reset and unused I/O have explicit no-connect markers; D0/D1 need no external wiring because the USB serial bridge is on A1. The ground PWR_FLAG represents the USB/module ground source.

The Nano, diodes, and filter capacitors have standard KiCad footprint assignments. Switch and potentiometer footprints are intentionally unspecified because their packages and mechanical arrangement cannot be inferred from code. Select those parts and verify their terminal numbering before PCB layout. This addition provides the schematic and wiring definition; it contains no PCB layout.

## Validation

The updated schematic was loaded and rendered with KiCad 10.0.8. The electrical rules check returned **0 errors and 0 warnings**, with no violations excluded; [controller_prototype_erc.rpt](controller_prototype_erc.rpt) is the saved report. Unused wire tails on the two labeled ADC wires and the tops of the five matrix columns have been trimmed. The report also lists checks ignored by KiCad's defaults.

Its exported XML netlist was checked against the actual firmware pin arrays with [check_netlist.py](check_netlist.py), including all diode polarities, 25 channel labels, both ADC inputs, the supply rails and every unused module pin. Disabling the analog pull-ups requires no wiring or BOM changes.

To reproduce the connectivity check from the repository root:

```sh
kicad-cli sch export netlist --format kicadxml \
  -o /tmp/controller_prototype.xml hardware/controller_prototype.kicad_sch
python3 hardware/check_netlist.py /tmp/controller_prototype.xml
```

To regenerate the preview:

```sh
kicad-cli sch export pdf -o hardware/controller_prototype.pdf \
  hardware/controller_prototype.kicad_sch
```

To refresh the electrical rules report with KiCad 10:

```sh
kicad-cli sch erc --severity-all --exit-code-violations \
  -o hardware/controller_prototype_erc.rpt hardware/controller_prototype.kicad_sch
```

For the Flatpak installation, replace `kicad-cli` in these commands with `flatpak run --command=kicad-cli org.kicad.KiCad`.

KiCad Community symbols are redistributed in `Controller.kicad_sym` under CC BY-SA 4.0 with the KiCad electronic-design exception; see [SYMBOLS-LICENSE.txt](SYMBOLS-LICENSE.txt).
