# Controller Prototype

Arduino firmware for a controller with **25 buttons and two analog inputs**. It scans a 5 × 5 button matrix, debounces button changes, reads two potentiometers or joystick axes, and sends their state to a computer over USB serial.

The hardware reference uses a classic **Arduino Nano v3.x with an ATmega328P, running at 5 V / 16 MHz**. The firmware sends a binary serial protocol; using it as a game controller requires host software that understands that protocol. It does not appear as a native USB HID gamepad.

## Start with a breadboard

**Everyone should build and test the circuit on a breadboard before proceeding to final assembly.** Check for wiring mistakes and potential hardware or firmware bugs before soldering, designing a PCB, or installing the controls in an enclosure.

Everything in the [hardware directory](hardware/) was generated or assembled by AI, including the reference schematic, documentation, parts list, and validation script. The bundled KiCad symbols come from the KiCad Community libraries and retain their upstream license and attribution. The hardware choices are assumptions derived from the firmware, so treat the design as a starting point to verify.

The schematic passes KiCad's electrical rules check with **zero errors and zero warnings**, and its exported netlist passes the included connectivity check. These checks do not replace testing a physical circuit.

If you find an issue, please open a GitHub issue. **I'm more than happy to fix issues that arise**, and reports from people building and testing the project are very welcome.

## What it does

- Scans 25 buttons through five column pins and five row pins.
- Debounces presses and releases using a three-count threshold.
- Reads A0 and A1 with the default 10-bit ADC resolution, giving values from 0 to 1023.
- Sends the full channel state at a nominal interval of 10 ms over serial at 115200 baud.
- Uses internal pull-ups for the button matrix. Analog input pull-ups are disabled at startup with `PORTC = 0x00`.

## Hardware and wiring

The reference circuit contains one Nano, 25 normally open pushbuttons, 25 isolation diodes, two 10 kΩ linear potentiometers, and two 100 nF filter capacitors. Power it through the Nano's USB connector.

| Function | Arduino pins | Channel order |
| --- | --- | --- |
| Matrix columns | D2, D3, D4, D5, D6 | Columns 0–4 |
| Matrix rows | D7, D8, D9, D10, D11 | Rows 0–4 |
| Analog controls | A0, A1 | Analog channels 0 and 1 |

Digital channels are numbered 0–24 using `channel = row * 5 + column`. Each button has a 1N4148 diode with its **striped cathode facing the column**. Potentiometer end terminals connect to +5 V and GND; their wipers connect to A0 and A1. Keep analog signals within 0–5 V and do not leave the inputs floating.

Use the [hardware guide](hardware/README.md) for the complete wiring map, component assumptions, and validation commands. The [schematic PDF](hardware/controller_prototype.pdf) is a convenient reference, and the [bill of materials](hardware/bom.csv) lists the parts. The KiCad project contains a schematic; there is no PCB layout.

The firmware directly writes ATmega328P port registers. Other Nano families and boards with different microcontrollers need firmware changes and a separate wiring review.

## Build and upload

1. Download or clone this repository. Keep the sketch in a folder named `controller_prototype` so Arduino can open it with the matching sketch name.
2. Open [controller_prototype.ino](controller_prototype.ino) in the Arduino IDE. Keep the accompanying `.h` and `.cpp` files in the same folder.
3. Install **Arduino AVR Boards** through the Boards Manager if needed.
4. Select **Arduino Nano**, the processor/bootloader option appropriate for your module, and its serial port.
5. Click **Verify**, then **Upload**.

The sketch uses the Arduino core and the protocol sources included here; no additional Arduino libraries are required.

## Test before final assembly

Start with a small section of the matrix and both analog controls on a breadboard, then expand to all 25 buttons. Before committing to the final build:

- Check the supply connections, module pin numbers, and diode orientation.
- Confirm that every button reports the expected channel and returns to the released state.
- Try simultaneous button presses and check for unexpected channels or missed presses.
- Move each analog control through its full travel and check for smooth readings, reasonable endpoints, and noise.
- Check debounce behavior and repeat the tests after disconnecting and reconnecting USB power.

Use host software that can decode the binary frames to inspect channel values. Arduino's Serial Monitor will show binary data rather than readable button or axis values.

## Serial output

The serial connection runs at **115200 baud, 8 data bits, no parity, and one stop bit**. Each update contains all digital and analog channels.

[yajproto.cpp](yajproto.cpp) defines the YAJ protocol version 1 message: a length byte, protocol version, command ID, channel counts, packed digital states, 16-bit little-endian analog values, and a 16-bit checksum. A set digital bit means the button is pressed. [pseudo_cobs.h](pseudo_cobs.h) encodes the message and appends a zero-byte frame delimiter.

A receiver should split the stream at the zero delimiters, decode the framing, and validate the message length, version, and checksum before using the channel data. This repository includes the firmware encoder; it does not include a host application or decoder.

## Project files

| File or directory | Purpose |
| --- | --- |
| [controller_prototype.ino](controller_prototype.ino) | Startup configuration, matrix scanning, debounce, analog reads, and serial output |
| [sketch_settings.h](sketch_settings.h) | Compile-time settings, including the optional profiling code |
| [yajproto.h](yajproto.h), [yajproto.cpp](yajproto.cpp) | Channel state storage and binary message construction |
| [pseudo_cobs.h](pseudo_cobs.h) | Serial frame encoding |
| [hardware/](hardware/) | AI-generated hardware reference, KiCad project, PDF, parts list, and validation tools |

## Credits and inspiration

This design was inspired, among other sources, by Bald Engineer's [Arduino Keyboard Matrix Tutorial](https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html). Thanks for sharing the ideas and explanations behind keyboard matrix scanning.

The hardware reference also uses KiCad Community symbols; their attribution and license are included in [hardware/SYMBOLS-LICENSE.txt](hardware/SYMBOLS-LICENSE.txt).

## Reporting issues and contributing

Bug reports, corrections, and pull requests are welcome. For a useful report, include your board model, wiring or schematic changes, firmware version or commit, steps to reproduce the problem, and what you expected to happen. Decoded channel values or a short serial capture can help diagnose input and protocol issues.

