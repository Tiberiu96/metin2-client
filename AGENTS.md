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
- News: `http://192.168.184.132/news` (metin2-ignition.local) | locale.cfg → sare SELECT LOCALE

## Assets/fisiere client

- Assets (`.py`, locale, `itemdesc.txt`, `item_proto` etc.) citite din **pack-uri** `ClientIgnition/pack/`, NU din root.
- Despachetate in `ClientIgnition/Eternexus/{nume_pack}/` (ex: `locale_en/locale/en/itemdesc.txt`).
- **Modificare .py → repackuire** — regenereaza `root.epk` + `root.eix` din `C:\Users\skema\Desktop\ClientIgnition\`.

## Verificare encoding inainte de commit

Fisiere text (`.py`, `.txt`, `.h`, `.cpp`) — verifica coruptie encoding:

- **Coruptie:** `?` / `ï¿½` / `\x80-\x9F` in loc de diacritice/coreeana — **doar cod functional, nu comentarii**
- **Verificare:** `git diff` — daca diacritice (ro/hu/cz/pl/pt/it/gr/ru) sau coreeana apar ca `?`, NU commit
- **Cauza:** Edit tool scrie UTF-8 peste ISO-8859/EUC-KR
- **Fix:** restaureaza din backup + aplica cu PowerShell (`GetEncoding(1252)` / `GetEncoding('euc-kr')`)
- **Sensibile:** `locale_interface.txt` (toate limbile), `.txt`/`.cpp` cu comentarii coreene

## Debug

- **C++:** `TraceError("DBG_X: ...")` → `syserr.txt` in `ClientIgnition/`
- **Python:** `import dbg; dbg.TraceError("DBG_X: ...")` → acelasi `syserr.txt`
- Sterge logurile inainte de commit.
- Problema implica server → adauga log pe server (`syslog`/`fprintf`) **inainte** de fix-uri oarbe.

## Multi-Language (EN/DE/HU/FR/CZ/DK/ES/GR/IT/NL/PL/PT/RO/RU/TR)
**Client:** redenumeste `locale.cfg` ↔ `locale_{xy}.cfg`
**Server:** `item_names_xy.txt`→`item_names.txt`, `mob_names_xy.txt`→`mob_names.txt`, `translate_xy.lua` + `locale_string_xy.txt` + questcompile
**Coduri:** english / germany / hungary / france / czech / denmark / spain / greek / italy / netherlands / poland / portugal / romania / russia / turkey

## Reguli Codex locale

- Codex NU repackuieste resursele din Eternexus. Dupa modificari in `ClientIgnition/Eternexus/`, utilizatorul face manual repack pentru pack-urile afectate.
- La finalul fiecarei sarcini, Codex trebuie sa specifice folderele modificate, separat pentru repo si pentru asset-urile client din `ClientIgnition/Eternexus/`.
