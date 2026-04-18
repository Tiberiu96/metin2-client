---
name: ghistory
description: Genereaza un handoff compact al sesiunii curente (fisiere modificate, de ce, context) pentru a fi folosit ca primer in urmatoarea conversatie Claude. Output < 100 linii, salvat incremental in {project_root}/.claude/history/handoff_N.md.
---

# /ghistory — Session Handoff Compactor

Cand userul invoca `/ghistory`, genereaza un rezumat dens al sesiunii curente si salveaza-l INCREMENTAL in `C:\Users\skema\Desktop\metin2-client\.claude\history\handoff_{N}.md`. Scopul: userul incepe o noua conversatie cu Claude si poate pasta continutul (sau cere "citeste handoff_{N}.md") ca sa restaureze contextul fara sa fie nevoie de re-explicatii.

## Reguli hard

1. **Maxim 100 linii total** (inclusiv titluri si linii goale). Daca depasesti, compacteaza mai agresiv.
2. **Nu include cod integral** — doar nume fisier + numere linie + ce s-a schimbat.
3. **Scrie in Romana**, acelasi stil ca restul conversatiei cu acest user.
4. **Nu inventa** — daca un fapt nu e in conversatie, omite-l.
5. **Salvare incrementala**: gaseste urmatorul index liber in `C:\Users\skema\Desktop\metin2-client\.claude\history\` si scrie `handoff_{N}.md`. NU suprascrie niciodata un fisier existent.
6. Dupa save, afiseaza in chat path-ul fisierului + continutul complet.

## Procedura de save incremental

1. Verifica daca exista directorul `C:\Users\skema\Desktop\metin2-client\.claude\history\`. Daca nu, creeaza-l (`Bash: mkdir -p`).
2. Listeaza fisierele cu pattern `handoff_*.md` folosind `Glob` in `C:\Users\skema\Desktop\metin2-client\.claude\history\`.
3. Extrage numerele existente, gaseste `maxN`, noul index = `maxN + 1`. Daca nu exista niciunul, incepe de la 1.
4. Scrie `Write` la `C:\Users\skema\Desktop\metin2-client\.claude\history\handoff_{N}.md`.

## Structura output (template)

```markdown
# Session Handoff #{N} — {data ISO scurta}

## Proiect & context general
{1-3 linii: ce proiect, pe ce branch, ce problema se rezolva la nivel inalt}

## Problema initiala
{2-4 linii: ce a raportat userul, simptome observate, ce log/eroare a aparut}

## Root cause identificat
{2-5 linii: cauza reala descoperita, cu file:linie unde e bug-ul. Fara cod.}

## Modificari facute
- `path/to/file.ext:linie` — ce s-a schimbat si DE CE (1 linie)
- `path/to/file2.ext:linie` — ...
{maxim 10-15 bullet-uri, fiecare 1 linie}

## Decizii importante / alternative respinse
- {decizie + motiv scurt, daca e cazul}

## Stare curenta
{1-3 linii: compileaza? testat? ce mai ramane de facut?}

## Next steps pentru sesiunea urmatoare
1. {actiune concreta 1}
2. {actiune concreta 2}

## Loguri/debug lasate in cod (de curatat)
- `file:linie` — {ex: TraceError("DBG_XX: ...")}
{daca nu e niciunul, scrie "niciunul"}
```

## Procedura de executie completa

1. Revizuieste **conversatia curenta** — nu face apeluri noi de tool decat daca e strict necesar (ex: `git diff --stat` pentru confirmare fisiere modificate, DAR numai daca nu ai deja info clara din istoric).
2. Extrage: problema → cauza → fix → stare. Fara narrativ.
3. Calculeaza indexul `N` (vezi "Procedura de save incremental").
4. Scrie fisierul cu `Write` la `C:\Users\skema\Desktop\metin2-client\.claude\history\handoff_{N}.md`.
5. Afiseaza in chat: `Handoff #{N} salvat in C:\Users\skema\Desktop\metin2-client\.claude\history\handoff_{N}.md ({linii} linii)` + continutul integral intr-un code block markdown.
6. La final spune userului: `In noua sesiune ruleaza: citeste C:\Users\skema\Desktop\metin2-client\.claude\history\handoff_{N}.md`.

## Ce NU include

- Narrativ lung despre ce a incercat Claude si nu a mers
- Cod sursa integral (doar referinte file:linie)
- Informatii deja documentate in CLAUDE.md sau memorie persistenta
- Timestamp-uri exacte, mesaje pas-cu-pas
- Multumiri, politete, meta-comentarii despre proces

## Exemplu de bullet bun vs rau

BUN: `ClientVS22/source/UserInterface/PythonNetworkStreamEvent.cpp:11` — OnRemoteDisconnect nu mai apeleaza Connect direct; seteaza flag m_bPendingChannelReconnect (Clear() base class distrugea noul socket).

RAU: Am modificat PythonNetworkStreamEvent.cpp ca sa repare problema de reconnect pentru ca nu mergea schimbarea de channel si a trebuit sa facem asta pentru ca socketul era distrus dupa ce Connect era apelat in OnRemoteDisconnect...
