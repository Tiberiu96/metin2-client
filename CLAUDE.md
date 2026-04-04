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

## IMPORTANT — assets/fisiere client
- Toate asseturile (`.py`, locale, `itemdesc.txt`, `item_proto` etc.) sunt citite din **pack-uri** din `ClientIgnition/pack/`, NU din rootul clientului.
- Versiunile despachetate se gasesc in `ClientIgnition/Eternexus/{nume_pack}/` (ex: `locale_en/locale/en/itemdesc.txt`).
- **Orice modificare .py necesita repackuire** — regenereaza `root.epk` + `root.eix` din `C:\Users\skema\Desktop\ClientIgnition\`.

## Debug — cand nu e clar, pune loguri
- **C++:** `TraceError("DBG_X: ...")` → apare in `syserr.txt` din `ClientIgnition/`
- **Python:** `import dbg; dbg.TraceError("DBG_X: ...")` → acelasi `syserr.txt`
- Dupa debug, sterge logurile inainte de commit.
- Daca problema implica server: adauga si log pe server (`syslog` / `fprintf`) **inainte** de a incerca fix-uri oarbe.

## Multi-Language (EN/DE/HU/FR/CZ/DK/ES/GR/IT/NL/PL/PT/RO/RU/TR)
**Client:** redenumeste `locale.cfg` ↔ `locale_{xy}.cfg`
**Server:** `item_names_xy.txt`→`item_names.txt`, `mob_names_xy.txt`→`mob_names.txt`, `translate_xy.lua` + `locale_string_xy.txt` activ + questcompile
**Coduri server:** english / germany / hungary / france / czech / denmark / spain / greek / italy / netherlands / poland / portugal / romania / russia / turkey
