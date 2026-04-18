# Session Handoff #2 — 2026-04-11

## Proiect & context general
metin2-client (C++/VS2022, branch `main`). Continuare feature Change Channel in-game — fix-uri progresive pana la stare partiala functionala.

## Problema initiala
Change Channel: clientul reconnecta corect (fix din handoff_1), dar la reconnect serverul kickuia jucatorul in loc sa-l duca pe channel 2.

## Root cause identificat (cascada)

1. **HEADER_GC_CHANGE_CHANNEL (211) neinregistrat in packet header map** → `CheckPacket` facea PostQuitMessage pe pachetul GC primit. Fix: adaugat in `CMainPacketHeaderMap` ([PythonNetworkStream.cpp:186](ClientVS22/source/UserInterface/PythonNetworkStream.cpp#L186)).

2. **RecvChangeChannelPacket nu salva noul login_key** → clientul reconnecta cu token vechi (single-use). Fix: `m_dwLoginKey = pk.login_key` in Recv, scos `m_bChangingChannel=true` din Send (mutat in Recv). ([PythonNetworkStreamPhaseGame.cpp:4496](ClientVS22/source/UserInterface/PythonNetworkStreamPhaseGame.cpp#L4496))

3. **TPacketGCChangeChannel fara login_key** → struct pe client era 2 bytes, server trimitea 6. Fix: extins cu `DWORD login_key`. ([Packet.h:2831](ClientVS22/source/UserInterface/Packet.h#L2831))

4. **LoginSuccess3/4 in DirectEnterMode nu trimitea SendSelectCharacterPacket** → client ingheta in SelectPhase. Fix: adaugat `SendSelectCharacterPacket(m_kDirectEnterMode.m_dwChrSlotIndex)` in ambele Recv. ([PythonNetworkStreamPhaseLogin.cpp:172,200](ClientVS22/source/UserInterface/PythonNetworkStreamPhaseLogin.cpp#L172))

5. **PLAYER_LOAD_ERROR: LoginData IsPlay=1** (stare curenta nerezolvata) → race condition: CLoginData veche de pe channel 1 (IsPlay=1) inca exista cand channel 2 incearca PLAYER_LOAD. Server trimite AUTH_LOGIN + LOGOUT dar vechea CLoginData nu e curatata sincron. Fix necesar pe SERVER, nu pe client.

## Modificari facute
- `ClientVS22/source/UserInterface/PythonNetworkStream.cpp:186` — inregistrat HEADER_GC_CHANGE_CHANNEL=211 in packet header map (STATIC, sizeof=6)
- `ClientVS22/source/UserInterface/Packet.h:2831-2836` — TPacketGCChangeChannel extins: `BYTE header; BYTE channel; DWORD login_key;`
- `ClientVS22/source/UserInterface/PythonNetworkStreamPhaseGame.cpp:4481-4504` — SendChangeChannelPacket: scos m_bChangingChannel=true; RecvChangeChannelPacket: adaugat m_dwLoginKey=pk.login_key + m_bChangingChannel=true
- `ClientVS22/source/UserInterface/PythonNetworkStreamPhaseLogin.cpp:172-175,200-203` — LoginSuccess3+4: DirectEnterMode → SendSelectCharacterPacket automat cu slot salvat
- `ClientVS22/CLAUDE.md` — comprimat (42→36 linii), backup la CLAUDE.md.original.md

## Decizii importante / alternative respinse
- Server: eliminat QUERY_CHANGE_CHANNEL custom si HEADER_GD_CHANGE_CHANNEL — nu erau necesare.
- Race condition PLAYER_LOAD_ERROR: fix corect e in db_cache la HEADER_GD_AUTH_LOGIN (cleanup CLoginData veche cu acelasi account), NU pe client.

## Stare curenta
Compileaza. PARTIAL functional: flow pana la SelectCharacterPacket merge, dar server intoarce PLAYER_LOAD_ERROR (IsPlay=1 pe CLoginData veche). Clientul e corect — bug ramas e exclusiv pe server (db_cache).

## Next steps pentru sesiunea urmatoare
1. Asteapta fix server: la HEADER_GD_AUTH_LOGIN in db_cache, curata CLoginData veche cu acelasi account_id/login (IsPlay=1) inainte de creare noua.
2. Dupa fix server, testeaza — verifica in db syslog absenta `PLAYER_LOAD_ERROR`.
3. Daca merge, sterge toate `DBG_CC:` din cod inainte de commit.
4. Commit toate modificarile client (8 fisiere modificate din handoff_1 + handoff_2).

## Loguri/debug lasate in cod (de curatat)
- `PythonNetworkStreamEvent.cpp:6,11` — `DBG_CC: OnRemoteDisconnect`, `DBG_CC: Pending reconnect flag set`
- `PythonNetworkStream.cpp` (Process) — `DBG_CC: Executing pending reconnect`
- `PythonNetworkStreamPhaseGame.cpp:4483,4501` — `DBG_CC: SendChangeChannelPacket`, `DBG_CC: RecvChangeChannelPacket OK`
- `PythonNetworkStreamPhaseHandShake.cpp:99` — `DBG_CC: SetHandShakePhase DirectEnterMode`
- `PythonNetworkStreamPhaseLogin.cpp:116,174,203,214` — `DBG_CC: SetLoginPhase`, `DBG_CC: LoginSuccess3/4 DirectEnter`, `DBG_CC: OnConnectFailure`
