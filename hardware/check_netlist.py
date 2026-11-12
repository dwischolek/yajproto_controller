#!/usr/bin/env python3
"""Check a fresh KiCad XML netlist against the firmware and Nano pinout.

Usage: kicad-cli sch export netlist --format kicadxml -o /tmp/controller.xml \
           hardware/controller_prototype.kicad_sch
       python3 hardware/check_netlist.py /tmp/controller.xml
"""
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


def require(condition, message):
    if not condition:
        raise ValueError(message)


def main():
    if len(sys.argv) != 2:
        raise ValueError('Pass the XML netlist exported by KiCad as the only argument.')
    firmware = (Path(__file__).resolve().parents[1] / 'controller_prototype.ino').read_text()
    root = ET.parse(sys.argv[1]).getroot()
    components = {item.attrib['ref']: item for item in root.findall('components/comp')}
    expected_refs = {'A1', 'RV1', 'RV2', 'C1', 'C2'}
    expected_refs |= {f'{prefix}{i}' for prefix in ('SW', 'D') for i in range(1, 26)}
    require(set(components) == expected_refs, 'Expected exactly 55 physical components.')
    nets = {}
    for net in root.findall('nets/net'):
        name = net.attrib['name'].removeprefix('/')
        require(name not in nets, f'Duplicate net name: {name}')
        nets[name] = {(node.attrib['ref'], node.attrib['pin']) for node in net.findall('node')}

    def check_net(name, expected):
        actual = nets.get(name)
        require(actual == expected, f'{name}: expected {sorted(expected)}, found {actual}')

    def firmware_pins(name):
        match = re.search(r'\b' + name + r'\[\]\s*=\s*\{([^}]+)\}', firmware)
        require(match is not None, f'Missing firmware pin array: {name}')
        return [token.strip() for token in match[1].split(',')]

    cols, rows = firmware_pins('cols'), firmware_pins('rows')
    require(len(cols) == len(rows) == 5, 'Schematic requires a 5 by 5 matrix.')
    require(firmware_pins('analogPins') == ['A0', 'A1'], 'Analog input assignment changed.')
    # Classic Nano module pad numbers differ from Arduino digital pin numbers.
    nano_digital = {str(digital): str(digital + 3) for digital in range(2, 14)}
    for c, pin in enumerate(cols):
        require(pin in nano_digital, f'Unsupported Nano digital pin: {pin}')
        check_net(f'COL{c}_D{pin}', {('A1', nano_digital[pin])} |
                  {(f'D{r * 5 + c + 1}', '1') for r in range(5)})
    for r, pin in enumerate(rows):
        require(pin in nano_digital, f'Unsupported Nano digital pin: {pin}')
        check_net(f'ROW{r}_D{pin}', {('A1', nano_digital[pin])} |
                  {(f'SW{r * 5 + c + 1}', '1') for c in range(5)})
    for i in range(1, 26):
        check_net(f'Net-(D{i}-A)', {(f'D{i}', '2'), (f'SW{i}', '2')})
        require(components[f'SW{i}'].findtext('value') == f'CH{i - 1:02}',
                f'SW{i} has the wrong channel label.')
        require(components[f'D{i}'].findtext('value') == '1N4148', f'Wrong diode: D{i}')
    for i in range(2):
        check_net(f'ANALOG{i}_A{i}', {('A1', str(19 + i)),
                  (f'RV{i + 1}', '2'), (f'C{i + 1}', '1')})
    check_net('+5V', {('A1', '27'), ('RV1', '1'), ('RV2', '1')})
    check_net('GND', {('A1', '4'), ('A1', '29'), ('RV1', '3'), ('RV2', '3'),
                     ('C1', '2'), ('C2', '2')})
    unconnected = [nodes for name, nodes in nets.items() if name.startswith('unconnected-')]
    unused_pads = {'1', '2', '3', '15', '16', '17', '18', '21', '22', '23',
                   '24', '25', '26', '28', '30'}
    require(len(unconnected) == 15 and
            {frozenset(nodes) for nodes in unconnected} ==
            {frozenset({('A1', pin)}) for pin in unused_pads},
            'Unused Nano pads must each remain disconnected.')
    require(len(nets) == 54, f'Unexpected extra or merged nets: {len(nets)}')
    print('PASS: 55 components; all 25 diode polarities and channel labels; '
          '10 matrix nets; 2 analog inputs; power; 15 unused Nano pads.')


if __name__ == '__main__':
    try:
        main()
    except (ValueError, KeyError, ET.ParseError) as error:
        sys.exit(f'FAIL: {error}')
