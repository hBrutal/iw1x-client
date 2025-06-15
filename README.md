# IW1x Client

**IW1x** is a modification for Call of Duty Multiplayer v1.1, focused on expanding functionality, improving security, and enhancing quality-of-life features.

## Compile from source

- Clone the Git repo. Do NOT download it as ZIP, that won't work.
- Update the submodules and run `premake5 vs2022` or simply use the delivered `generate.bat`.
- Build via solution file in `build\iw1x-client.sln`.

### Premake arguments

| Argument                    | Description                                    |
|:----------------------------|:-----------------------------------------------|
| `--copy-to=PATH`            | Optional, copy the EXE to a custom folder after build, define the path here if wanted. |
| `--dev-build`               | Enable development builds of the client. |

## Credits

- [h1-mod](https://github.com/auroramod/h1-mod)
- [Codextended-client](https://github.com/xtnded/codextended-client)
- [CoDPlusPlus](https://github.com/kartjom/CoDPlusPlus)
- [RTCW-MP](https://github.com/id-Software/RTCW-MP/)
- [ioq3](https://github.com/ioquake/ioq3)

## Disclaimer

This software has been created purely for the purposes of academic research. It is not intended to be used to attack other systems. Project maintainers are not responsible or liable for misuse of the software. Use responsibly.
