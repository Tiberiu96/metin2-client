# metin2-client — C++ / Visual Studio 2022 (Windows)

## Structura repo
- `ClientVS22/source/` → C++ (EterLib, GameLib, UserInterface)
- `ClientVS22/client.sln` | `extern/` (167MB, exclus git) | `binary/` (exclus git)
- `Patcher/` → WPF .NET 8 launcher | `extensions/` → extensii zip

## Compilare
VS2022 → copiaza `gamecore.exe` in `C:\Users\skema\Desktop\ClientIgnition\`

## Patcher (Launcher)
- Stack: WPF, .NET 8, C#
- Publish: `dotnet publish -c Release -r win-x64 --self-contained true -p:PublishSingleFile=true`
- Output: `Patcher/bin/Release/net8.0-windows/win-x64/publish/` → copiaza in `ClientIgnition/`
- News: `http://192.168.184.132/news` (metin2-ignition.local) | locale.cfg → sare peste SELECT LOCALE

## IMPORTANT — fisiere .py/script
Citite din **pack-uri** (`root.epk`/`root.eix`), NU direct din folder.
**Orice modificare .py necesita repackuire** — regenereaza `root.epk` + `root.eix` din `C:\Users\skema\Desktop\ClientIgnition\`.

## Multi-Language (EN/DE/HU/FR/CZ/DK/ES/GR/IT/NL/PL/PT/RO/RU/TR)
**Client:** redenumeste `locale.cfg` ↔ `locale_{xy}.cfg`
**Server:** `item_names_xy.txt`→`item_names.txt`, `mob_names_xy.txt`→`mob_names.txt`, `translate_xy.lua` + `locale_string_xy.txt` activ + questcompile
**Coduri server:** english / germany / hungary / france / czech / denmark / spain / greek / italy / netherlands / poland / portugal / romania / russia / turkey
