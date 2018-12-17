# AutoBleem - former cbleemsync
BleemSync implementation in C++ that works on Playstation Classic 

Rewritten in C++ with additional validation.
**This is getting stable now ... but ...  use it ONLY if you know what you are doing.** 

## Features ported from BleemSync

1. Overmounts portions of the PSC's filesystem to safely allow modifications
2. Modifies the stock UI to show added games
3. Uses Game name as folders instead of numbered folders
4. Supports multi-disc games(not tested)
5. **Includes offline metadata and cover art databases - no download needed**
6. **Runs on boot of the PlayStation Classic** as it was designed to do so from the beginning
7. Intelligent algorithm is trying to "fix" missing files like .cue, unpack .ecm (currently switched off)
