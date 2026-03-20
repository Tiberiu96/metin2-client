# metin2-client

Sursa client Metin2 privat, compilabila pe **Windows cu Visual Studio 2022**.

## Structura repo

```
ClientVS22/
  source/       → sursa C++ (EterLib, GameLib, UserInterface, etc.)
  client.sln    → solutia Visual Studio 2022
  DumpProto/    → utilitar proto dump
  vs_files/     → fisiere proiect VS
  extern/       → librarii externe (167MB, exclus din Git)
  binary/       → output compilare (exclus din Git)
Patcher/        → WPF launcher (.NET 8, C#) — selectie limba, news, start game
extensions/     → extensii zip (4 Inventory, 6th skill, Firewall)
```

## Compilare

Compilarea se face **doar pe Windows**, din Visual Studio 2022.
Output-ul (`.exe`) se copiaza in folderul Client/ (runtime, pe Google Drive).

## Client runtime

Fisierele mari (pack/, dll-uri, exe) sunt stocate pe **Google Drive**, nu in acest repo.
Dupa compilare, copiaza `gamecore.exe` in folderul `C:\Users\skema\Desktop\ClientIgnition\`.

## Patcher (Launcher)

- **Stack:** WPF, .NET 8, C#
- **Publish:** `dotnet publish -c Release -r win-x64 --self-contained true -p:PublishSingleFile=true`
- **Output:** `Patcher/bin/Release/net8.0-windows/win-x64/publish/` → copiaza in `ClientIgnition/`
- **Functionalitati:** selectie limba (seteaza locale.cfg), afisare news de pe website, start game
- **News source:** `http://192.168.184.132/news` (VM local, Host: metin2-ignition.local)
- Clientul (Locale.cpp) citeste `locale.cfg` si sare peste dialogul SELECT LOCALE daca fisierul exista

## Excluderi Git (.gitignore)

- `ClientVS22/extern/` — librarii externe (167MB)
- `ClientVS22/binary/` — output compilare
- `ClientVS22/.vs/` — setari locale VS
- `**/Release/`, `**/Debug/` — output build
- `Patcher/bin/`, `Patcher/obj/` — output build patcher

## Proiecte separate

Acest repo este **doar clientul**. Serverul si website-ul sunt proiecte complet separate:
- **metin2-server** → `C:\Users\skema\Desktop\metin2-server\` (FreeBSD C++)
- **metin2-client** → `C:\Users\skema\Desktop\metin2-client\` (Windows VS2022) ← ESTI AICI
- **metin2-ignition-website** → alt calculator, izolat complet

## Multi-Language Setup

**Limbi disponibile:** EN/DE/HU/FR/CZ/DK/ES/GR/IT/NL/PL/PT/RO/RU/TR (default: EN)

### Client
- Redenumește `locale.cfg` → `locale_en.cfg`
- Redenumește `locale_de.cfg` → `locale.cfg`

### Server (înlocuiește `xy` cu codul limbii)
- `share/conf/item_names_xy.txt` → `item_names.txt`
- `share/conf/mob_names_xy.txt` → `mob_names.txt`
- `share/locale/english/translate_xy.lua` → `translate_xy.lua` activ
- `share/locale/english/locale_string_xy.txt` → activ
- Rulează `questcompile` după modificări

### Setare limbă server
1. Redenumește `share/locale/english/` → `share/locale/germany/` (sau alt locale)
2. `common.locale` tabelă → `LOCALE` mValue = `germany`
3. Actualizează `~/.cshrc` questcompile path

### Coduri locale
`english` `germany` `hungary` `france` `czech` `denmark` `spain` `greek` `italy` `netherlands` `poland` `portugal` `romania` `russia` `turkey`
