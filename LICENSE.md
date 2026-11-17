# Project licenses

Controller Prototype uses the following licenses. These grants apply to the
project authors' copyrightable contributions; third-party material retains its
own terms as described below.

| Material | License |
| --- | --- |
| Firmware (`*.ino`, `*.cpp`, `*.h`), Python scripts, and repository configuration (`.gitignore`, `hardware/.gitignore`, `controller_prototype.iml`) | [Mozilla Public License 2.0](LICENSES/MPL-2.0.txt) (`MPL-2.0`) |
| Hardware design: `hardware/controller_prototype.kicad_sch`, `hardware/controller_prototype.kicad_pro`, `hardware/sym-lib-table`, `hardware/bom.csv`, and the schematic PDF | [CC0 1.0 Universal](LICENSES/CC0-1.0.txt) (`CC0-1.0`), except the embedded third-party symbols described below |
| Documentation (`README.md`, `hardware/README.md`) and the electrical rules report (`hardware/controller_prototype_erc.rpt`) | [Creative Commons Attribution-ShareAlike 4.0 International](LICENSES/CC-BY-SA-4.0.txt) (`CC-BY-SA-4.0`) |
| KiCad Community symbols in `hardware/Controller.kicad_sym` and embedded in the schematic's `lib_symbols` cache | CC BY-SA 4.0 with the KiCad electronic-design exception; see [the upstream license and attribution](hardware/SYMBOLS-LICENSE.txt) |

These terms apply to the respective materials; they are not a choice of licenses
for the entire repository. License texts and third-party notices retain their
own terms. This file records the licensing scope.

## Software

The software is licensed under MPL-2.0. When distributing covered source or
executables, preserve the applicable notices and comply with the license's
source availability requirements. Separate files may use other licenses as
permitted by MPL-2.0.

## Hardware

To the extent the project authors hold copyright and related rights in the
hardware design listed above, they dedicate those contributions to the public
domain under CC0 1.0 Universal. CC0 includes a fallback license where the waiver
is ineffective. You may copy, modify, distribute, and use those contributions,
including commercially, without attribution or a requirement to share changes.
This dedication does not assert that the circuit itself is copyrightable.

The hardware design is provided without warranty; see section 4 of CC0 for the
full limitations and disclaimers. CC0 does not waive or license patent or
trademark rights.

The bundled KiCad symbols retain their upstream license and attribution. The
KiCad electronic-design exception permits their use in designs without requiring
those designs to use the symbols' license. It does not relicense the symbol
library itself or the embedded symbols under CC0.

## Documentation

The documentation and electrical rules report listed above are licensed under
CC BY-SA 4.0. Retain supplied attribution, link to the license, indicate changes,
and license shared adaptations under the same license or a compatible license
as its terms permit. Existing credits and third-party notices must be preserved
where applicable.

The full license texts govern in all cases.
