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
extensions/     → extensii zip (4 Inventory, 6th skill, Firewall)
```

## Compilare

Compilarea se face **doar pe Windows**, din Visual Studio 2022.
Output-ul (`.exe`) se copiaza in folderul Client/ (runtime, pe Google Drive).

## Client runtime

Fisierele mari (pack/, dll-uri, exe) sunt stocate pe **Google Drive**, nu in acest repo.
Dupa compilare, copiaza `Metin2Distribute.exe` in folderul Client/ local.

## Excluderi Git (.gitignore)

- `ClientVS22/extern/` — librarii externe (167MB)
- `ClientVS22/binary/` — output compilare
- `ClientVS22/.vs/` — setari locale VS
- `**/Release/`, `**/Debug/` — output build

## Proiecte separate

Acest repo este **doar clientul**. Serverul si website-ul sunt proiecte complet separate:
- **metin2-server** → `C:\Users\skema\Desktop\metin2-server\` (FreeBSD C++)
- **metin2-client** → `C:\Users\skema\Desktop\metin2-client\` (Windows VS2022) ← ESTI AICI
- **metin2-ignition-website** → alt calculator, izolat complet
