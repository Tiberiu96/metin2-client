using System.Diagnostics;
using System.IO;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Patcher;

public partial class MainWindow : Window
{
    private static readonly HttpClient _http = new();
    private bool _patchComplete;

    private readonly string _clientPath;
    private readonly string _localeCfgPath;

    private string _newsUrl = "http://192.168.184.132/news";
    private string _newsHost = "metin2-ignition.local";
    private string _patchBaseUrl = "http://192.168.184.132";
    private string _patchHost = "patches.metin2-ignition.local";

    private readonly Dictionary<string, (string code, int codepage, string cfgFile)> _languages = new()
    {
        { "English",     ("en", 1252, "locale_en.cfg") },
        { "Deutsch",     ("de", 1252, "locale_de.cfg") },
        { "Romana",      ("ro", 1250, "locale_ro.cfg") },
        { "Turkce",      ("tr", 1254, "locale_tr.cfg") },
        { "Espanol",     ("es", 1252, "locale_es.cfg") },
        { "Francais",    ("fr", 1252, "locale_fr.cfg") },
        { "Italiano",    ("it", 1252, "locale_it.cfg") },
        { "Polski",      ("pl", 1250, "locale_pl.cfg") },
        { "Portugues",   ("pt", 1252, "locale_pt.cfg") },
        { "Nederlands",  ("nl", 1252, "locale_nl.cfg") },
        { "Magyar",      ("hu", 1250, "locale_hu.cfg") },
        { "Cesky",       ("cz", 1250, "locale_cz.cfg") },
        { "Dansk",       ("dk", 1252, "locale_dk.cfg") },
        { "Ellhnika",    ("gr", 1253, "locale_gr.cfg") },
        { "Russkij",     ("ru", 1251, "locale_ru.cfg") },
    };

    private readonly Dictionary<string, Dictionary<string, string>> _translations = new()
    {
        { "English", new() {
            { "News", "— NEWS —" }, { "Language", "LANGUAGE" }, { "ServerStatus", "SERVER STATUS" },
            { "Version", "VERSION" }, { "StartGame", "START GAME" }, { "Ready", "Ready to play" },
            { "LangSet", "Language set to English" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Loading news..." }, { "NoNews", "No announcements yet." }, { "NewsError", "Could not load news." },
            { "GameNotFound", "Game files not found!\nMake sure the launcher is in the game folder." },
            { "GameError", "Failed to start game" },
            { "Checking", "Checking for updates..." }, { "Downloading", "Downloading" },
            { "PatchComplete", "All files up to date" }, { "PatchError", "Update failed — check connection" },
            { "PatchProgress", "{0} / {1} files" },
        }},
        { "Deutsch", new() {
            { "News", "— NACHRICHTEN —" }, { "Language", "SPRACHE" }, { "ServerStatus", "SERVERSTATUS" },
            { "Version", "VERSION" }, { "StartGame", "SPIEL STARTEN" }, { "Ready", "Bereit zum Spielen" },
            { "LangSet", "Sprache auf Deutsch gesetzt" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Nachrichten laden..." }, { "NoNews", "Noch keine Ankündigungen." }, { "NewsError", "Nachrichten konnten nicht geladen werden." },
            { "GameNotFound", "Spieldateien nicht gefunden!\nStellen Sie sicher, dass der Launcher im Spielordner ist." },
            { "GameError", "Spiel konnte nicht gestartet werden" },
            { "Checking", "Suche nach Updates..." }, { "Downloading", "Herunterladen" },
            { "PatchComplete", "Alle Dateien aktuell" }, { "PatchError", "Update fehlgeschlagen" },
            { "PatchProgress", "{0} / {1} Dateien" },
        }},
        { "Romana", new() {
            { "News", "— NOUTATI —" }, { "Language", "LIMBA" }, { "ServerStatus", "STATUS SERVER" },
            { "Version", "VERSIUNE" }, { "StartGame", "JOACA" }, { "Ready", "Gata de joc" },
            { "LangSet", "Limba setata: Romana" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Se incarca noutatile..." }, { "NoNews", "Nicio anunt momentan." }, { "NewsError", "Nu s-au putut incarca noutatile." },
            { "GameNotFound", "Fisierele jocului nu au fost gasite!\nAsigurati-va ca launcher-ul este in folderul jocului." },
            { "GameError", "Jocul nu a putut fi pornit" },
            { "Checking", "Se verifica actualizarile..." }, { "Downloading", "Se descarca" },
            { "PatchComplete", "Toate fisierele sunt la zi" }, { "PatchError", "Actualizare esuata" },
            { "PatchProgress", "{0} / {1} fisiere" },
        }},
        { "Turkce", new() {
            { "News", "— HABERLER —" }, { "Language", "DIL" }, { "ServerStatus", "SUNUCU DURUMU" },
            { "Version", "SÜRÜM" }, { "StartGame", "OYUNA BASLA" }, { "Ready", "Oynamaya hazir" },
            { "LangSet", "Dil Türkçe olarak ayarlandi" }, { "Online", "Çevrimiçi" }, { "Offline", "Çevrimdisi" },
            { "LoadingNews", "Haberler yükleniyor..." }, { "NoNews", "Henüz duyuru yok." }, { "NewsError", "Haberler yüklenemedi." },
            { "GameNotFound", "Oyun dosyalari bulunamadi!\nBaslaticinin oyun klasöründe oldugundan emin olun." },
            { "GameError", "Oyun baslatilamadi" },
            { "Checking", "Güncellemeler kontrol ediliyor..." }, { "Downloading", "Indiriliyor" },
            { "PatchComplete", "Tüm dosyalar güncel" }, { "PatchError", "Güncelleme basarisiz" },
            { "PatchProgress", "{0} / {1} dosya" },
        }},
        { "Espanol", new() {
            { "News", "— NOTICIAS —" }, { "Language", "IDIOMA" }, { "ServerStatus", "ESTADO DEL SERVIDOR" },
            { "Version", "VERSIÓN" }, { "StartGame", "INICIAR JUEGO" }, { "Ready", "Listo para jugar" },
            { "LangSet", "Idioma configurado a Español" }, { "Online", "En línea" }, { "Offline", "Desconectado" },
            { "LoadingNews", "Cargando noticias..." }, { "NoNews", "Sin anuncios aún." }, { "NewsError", "No se pudieron cargar las noticias." },
            { "GameNotFound", "Archivos del juego no encontrados!\nAsegúrate de que el launcher está en la carpeta del juego." },
            { "GameError", "No se pudo iniciar el juego" },
            { "Checking", "Buscando actualizaciones..." }, { "Downloading", "Descargando" },
            { "PatchComplete", "Todos los archivos actualizados" }, { "PatchError", "Actualización fallida" },
            { "PatchProgress", "{0} / {1} archivos" },
        }},
        { "Francais", new() {
            { "News", "— ACTUALITÉS —" }, { "Language", "LANGUE" }, { "ServerStatus", "ÉTAT DU SERVEUR" },
            { "Version", "VERSION" }, { "StartGame", "LANCER LE JEU" }, { "Ready", "Prêt à jouer" },
            { "LangSet", "Langue définie sur Français" }, { "Online", "En ligne" }, { "Offline", "Hors ligne" },
            { "LoadingNews", "Chargement des actualités..." }, { "NoNews", "Aucune annonce pour le moment." }, { "NewsError", "Impossible de charger les actualités." },
            { "GameNotFound", "Fichiers du jeu introuvables!\nAssurez-vous que le launcher est dans le dossier du jeu." },
            { "GameError", "Impossible de lancer le jeu" },
            { "Checking", "Recherche de mises à jour..." }, { "Downloading", "Téléchargement" },
            { "PatchComplete", "Tous les fichiers sont à jour" }, { "PatchError", "Mise à jour échouée" },
            { "PatchProgress", "{0} / {1} fichiers" },
        }},
        { "Italiano", new() {
            { "News", "— NOTIZIE —" }, { "Language", "LINGUA" }, { "ServerStatus", "STATO DEL SERVER" },
            { "Version", "VERSIONE" }, { "StartGame", "AVVIA GIOCO" }, { "Ready", "Pronto per giocare" },
            { "LangSet", "Lingua impostata su Italiano" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Caricamento notizie..." }, { "NoNews", "Nessun annuncio al momento." }, { "NewsError", "Impossibile caricare le notizie." },
            { "GameNotFound", "File di gioco non trovati!\nAssicurati che il launcher sia nella cartella del gioco." },
            { "GameError", "Impossibile avviare il gioco" },
            { "Checking", "Ricerca aggiornamenti..." }, { "Downloading", "Download" },
            { "PatchComplete", "Tutti i file sono aggiornati" }, { "PatchError", "Aggiornamento fallito" },
            { "PatchProgress", "{0} / {1} file" },
        }},
        { "Polski", new() {
            { "News", "— AKTUALNOSCI —" }, { "Language", "JEZYK" }, { "ServerStatus", "STATUS SERWERA" },
            { "Version", "WERSJA" }, { "StartGame", "ROZPOCZNIJ GRE" }, { "Ready", "Gotowy do gry" },
            { "LangSet", "Jezyk ustawiony na Polski" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Ladowanie aktualnosci..." }, { "NoNews", "Brak ogloszen." }, { "NewsError", "Nie mozna zaladowac aktualnosci." },
            { "GameNotFound", "Nie znaleziono plików gry!\nUpewnij sie, ze launcher jest w folderze gry." },
            { "GameError", "Nie mozna uruchomic gry" },
            { "Checking", "Sprawdzanie aktualizacji..." }, { "Downloading", "Pobieranie" },
            { "PatchComplete", "Wszystkie pliki aktualne" }, { "PatchError", "Aktualizacja nieudana" },
            { "PatchProgress", "{0} / {1} plików" },
        }},
        { "Portugues", new() {
            { "News", "— NOTÍCIAS —" }, { "Language", "IDIOMA" }, { "ServerStatus", "ESTADO DO SERVIDOR" },
            { "Version", "VERSÃO" }, { "StartGame", "INICIAR JOGO" }, { "Ready", "Pronto para jogar" },
            { "LangSet", "Idioma definido para Português" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Carregando notícias..." }, { "NoNews", "Nenhum anúncio ainda." }, { "NewsError", "Não foi possível carregar as notícias." },
            { "GameNotFound", "Ficheiros do jogo não encontrados!\nCertifique-se que o launcher está na pasta do jogo." },
            { "GameError", "Não foi possível iniciar o jogo" },
            { "Checking", "Verificando atualizações..." }, { "Downloading", "Baixando" },
            { "PatchComplete", "Todos os ficheiros atualizados" }, { "PatchError", "Atualização falhou" },
            { "PatchProgress", "{0} / {1} ficheiros" },
        }},
        { "Nederlands", new() {
            { "News", "— NIEUWS —" }, { "Language", "TAAL" }, { "ServerStatus", "SERVERSTATUS" },
            { "Version", "VERSIE" }, { "StartGame", "START SPEL" }, { "Ready", "Klaar om te spelen" },
            { "LangSet", "Taal ingesteld op Nederlands" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Nieuws laden..." }, { "NoNews", "Nog geen aankondigingen." }, { "NewsError", "Kon nieuws niet laden." },
            { "GameNotFound", "Spelbestanden niet gevonden!\nZorg ervoor dat de launcher in de spelmap staat." },
            { "GameError", "Kon het spel niet starten" },
            { "Checking", "Controleren op updates..." }, { "Downloading", "Downloaden" },
            { "PatchComplete", "Alle bestanden zijn bijgewerkt" }, { "PatchError", "Update mislukt" },
            { "PatchProgress", "{0} / {1} bestanden" },
        }},
        { "Magyar", new() {
            { "News", "— HÍREK —" }, { "Language", "NYELV" }, { "ServerStatus", "SZERVER ÁLLAPOT" },
            { "Version", "VERZIÓ" }, { "StartGame", "JÁTÉK INDÍTÁSA" }, { "Ready", "Készen áll a játékra" },
            { "LangSet", "Nyelv beállítva: Magyar" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Hírek betöltése..." }, { "NoNews", "Még nincsenek közlemények." }, { "NewsError", "Nem sikerült betölteni a híreket." },
            { "GameNotFound", "A játékfájlok nem találhatók!\nGyőződjön meg róla, hogy az indító a játék mappájában van." },
            { "GameError", "Nem sikerült elindítani a játékot" },
            { "Checking", "Frissítések keresése..." }, { "Downloading", "Letöltés" },
            { "PatchComplete", "Minden fájl naprakész" }, { "PatchError", "Frissítés sikertelen" },
            { "PatchProgress", "{0} / {1} fájl" },
        }},
        { "Cesky", new() {
            { "News", "— NOVINKY —" }, { "Language", "JAZYK" }, { "ServerStatus", "STAV SERVERU" },
            { "Version", "VERZE" }, { "StartGame", "SPUSTIT HRU" }, { "Ready", "Pripraveno ke hre" },
            { "LangSet", "Jazyk nastaven na Cesky" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Nacítání novinek..." }, { "NoNews", "Zatím žádná oznámení." }, { "NewsError", "Nepodařilo se načíst novinky." },
            { "GameNotFound", "Herní soubory nenalezeny!\nUjistete se, že launcher je ve složce hry." },
            { "GameError", "Nepodařilo se spustit hru" },
            { "Checking", "Kontrola aktualizací..." }, { "Downloading", "Stahování" },
            { "PatchComplete", "Všechny soubory aktuální" }, { "PatchError", "Aktualizace selhala" },
            { "PatchProgress", "{0} / {1} souborů" },
        }},
        { "Dansk", new() {
            { "News", "— NYHEDER —" }, { "Language", "SPROG" }, { "ServerStatus", "SERVERSTATUS" },
            { "Version", "VERSION" }, { "StartGame", "START SPIL" }, { "Ready", "Klar til at spille" },
            { "LangSet", "Sprog sat til Dansk" }, { "Online", "Online" }, { "Offline", "Offline" },
            { "LoadingNews", "Indlæser nyheder..." }, { "NoNews", "Ingen meddelelser endnu." }, { "NewsError", "Kunne ikke indlæse nyheder." },
            { "GameNotFound", "Spilfiler ikke fundet!\nSørg for at launcheren er i spilmappen." },
            { "GameError", "Kunne ikke starte spillet" },
            { "Checking", "Søger efter opdateringer..." }, { "Downloading", "Downloader" },
            { "PatchComplete", "Alle filer er opdaterede" }, { "PatchError", "Opdatering mislykkedes" },
            { "PatchProgress", "{0} / {1} filer" },
        }},
        { "Ellhnika", new() {
            { "News", "— NEA —" }, { "Language", "GLOSSA" }, { "ServerStatus", "KATASTASI SERVER" },
            { "Version", "EKDOSI" }, { "StartGame", "ENARXI PAICHNIOU" }, { "Ready", "Etoimo gia paichnidi" },
            { "LangSet", "I glossa oristike se Ellinika" }, { "Online", "Se syndesi" }, { "Offline", "Ektos syndesis" },
            { "LoadingNews", "Fortosi neon..." }, { "NoNews", "Den yparxoun anakoinoseis." }, { "NewsError", "Adynati i fortosi neon." },
            { "GameNotFound", "Ta archeia tou paichniou den vrethikan!\nVevaiotheite oti o launcher einai ston fakelo tou paichniou." },
            { "GameError", "Adynati i enarxi tou paichniou" },
            { "Checking", "Elegchos gia enimerosis..." }, { "Downloading", "Metafortosi" },
            { "PatchComplete", "Ola ta archeia einai enimera" }, { "PatchError", "I enimerosi apetiche" },
            { "PatchProgress", "{0} / {1} archeia" },
        }},
        { "Russkij", new() {
            { "News", "— NOVOSTI —" }, { "Language", "JAZYK" }, { "ServerStatus", "STATUS SERVERA" },
            { "Version", "VERSIJA" }, { "StartGame", "NACHAT IGRU" }, { "Ready", "Gotov k igre" },
            { "LangSet", "Jazyk ustanovlen: Russkij" }, { "Online", "V seti" }, { "Offline", "Ne v seti" },
            { "LoadingNews", "Zagruzka novostej..." }, { "NoNews", "Poká net objavlenij." }, { "NewsError", "Ne udalos zagruzit novosti." },
            { "GameNotFound", "Fajly igry ne najdeny!\nUbedites, chto launcher nahoditsja v papke igry." },
            { "GameError", "Ne udalos zapustit igru" },
            { "Checking", "Proverka obnovlenij..." }, { "Downloading", "Zagruzka" },
            { "PatchComplete", "Vse fajly obnovleny" }, { "PatchError", "Obnovlenie ne udalos" },
            { "PatchProgress", "{0} / {1} fajlov" },
        }},
    };

    public MainWindow()
    {
        InitializeComponent();

        // Determine client path (same folder as patcher or parent)
        _clientPath = AppDomain.CurrentDomain.BaseDirectory;
        _localeCfgPath = Path.Combine(_clientPath, "locale.cfg");

        LoadConfig();
        LoadLanguages();
        LoadCurrentLocale();
        StartButton.IsEnabled = false;
        _ = LoadNewsAsync();
        _ = LoadVersionAsync();
        _ = RunPatchAsync();
    }

    private void LoadConfig()
    {
        string cfgPath = Path.Combine(_clientPath, "patcher.cfg");
        if (!File.Exists(cfgPath))
            return;

        foreach (string line in File.ReadAllLines(cfgPath))
        {
            string trimmed = line.Trim();
            if (string.IsNullOrEmpty(trimmed) || trimmed.StartsWith('#'))
                continue;

            int eq = trimmed.IndexOf('=');
            if (eq < 0) continue;

            string key = trimmed[..eq].Trim().ToLowerInvariant();
            string val = trimmed[(eq + 1)..].Trim();

            switch (key)
            {
                case "patch_url": _patchBaseUrl = val; break;
                case "patch_host": _patchHost = val; break;
                case "news_url": _newsUrl = val; break;
                case "news_host": _newsHost = val; break;
            }
        }
    }

    private async Task LoadNewsAsync()
    {
        try
        {
            var request = new HttpRequestMessage(HttpMethod.Get, _newsUrl);
            request.Headers.Host = _newsHost;
            var response = await _http.SendAsync(request);
            string html = await response.Content.ReadAsStringAsync();

            // Parse news items from HTML
            var items = new List<(string date, string title, string excerpt)>();
            var itemPattern = new Regex(
                @"<div class=""rounded p-5[^""]*""[^>]*>.*?" +
                @"<div[^>]*>\s*(.+?)\s*</div>\s*" +
                @"<h2[^>]*>(.+?)</h2>\s*" +
                @"<p[^>]*>(.+?)</p>",
                RegexOptions.Singleline);

            foreach (Match m in itemPattern.Matches(html))
            {
                string date = m.Groups[1].Value.Trim();
                string title = m.Groups[2].Value.Trim();
                string excerpt = m.Groups[3].Value.Trim();
                items.Add((date, title, excerpt));
            }

            if (items.Count > 0)
            {
                var sb = new System.Text.StringBuilder();
                foreach (var (date, title, excerpt) in items)
                {
                    sb.AppendLine($"[{date}]");
                    sb.AppendLine(title);
                    sb.AppendLine(excerpt);
                    sb.AppendLine();
                }
                NewsText.Text = sb.ToString().TrimEnd();
            }
            else
            {
                NewsText.Text = T("NoNews");
            }
        }
        catch
        {
            NewsText.Text = T("NewsError");
        }
    }

    private async Task LoadVersionAsync()
    {
        try
        {
            var request = new HttpRequestMessage(HttpMethod.Get, $"{_patchBaseUrl}/version.txt");
            request.Headers.Host = _patchHost;
            var response = await _http.SendAsync(request);
            string version = (await response.Content.ReadAsStringAsync()).Trim();
            VersionText.Text = $"v{version}";
        }
        catch
        {
            VersionText.Text = "v?.?.?";
        }
    }

    private async Task RunPatchAsync()
    {
        try
        {
            StatusText.Text = T("Checking");
            PatchProgress.Value = 0;

            // Download patch list
            var request = new HttpRequestMessage(HttpMethod.Get, $"{_patchBaseUrl}/patch_list.json");
            request.Headers.Host = _patchHost;
            var response = await _http.SendAsync(request);
            string json = await response.Content.ReadAsStringAsync();
            var patchFiles = JsonSerializer.Deserialize<List<PatchFileInfo>>(json);

            if (patchFiles == null || patchFiles.Count == 0)
            {
                StatusText.Text = T("PatchComplete");
                StartButton.IsEnabled = true;
                _patchComplete = true;
                return;
            }

            // Find files that need updating
            var toDownload = new List<PatchFileInfo>();
            foreach (var pf in patchFiles)
            {
                // Skip the patcher itself
                if (pf.file.Equals("Metin2Ignition.exe", StringComparison.OrdinalIgnoreCase) ||
                    pf.file.Equals("Metin2Ignition.pdb", StringComparison.OrdinalIgnoreCase))
                    continue;

                string localPath = Path.Combine(_clientPath, pf.file.Replace('/', Path.DirectorySeparatorChar));
                if (!File.Exists(localPath))
                {
                    toDownload.Add(pf);
                    continue;
                }

                string localHash = ComputeSha256(localPath);
                if (!string.Equals(localHash, pf.hash, StringComparison.OrdinalIgnoreCase))
                    toDownload.Add(pf);
            }

            if (toDownload.Count == 0)
            {
                StatusText.Text = T("PatchComplete");
                PatchProgress.Value = 100;
                StartButton.IsEnabled = true;
                _patchComplete = true;
                return;
            }

            // Download missing/changed files
            for (int i = 0; i < toDownload.Count; i++)
            {
                var pf = toDownload[i];
                string fileName = Path.GetFileName(pf.file);
                StatusText.Text = $"{T("Downloading")}: {fileName}";
                PatchProgress.Value = (double)i / toDownload.Count * 100;

                string localPath = Path.Combine(_clientPath, pf.file.Replace('/', Path.DirectorySeparatorChar));
                string? dir = Path.GetDirectoryName(localPath);
                if (dir != null && !Directory.Exists(dir))
                    Directory.CreateDirectory(dir);

                // URL encode the path segments for spaces/special chars
                string urlPath = string.Join("/", pf.file.Split('/').Select(Uri.EscapeDataString));
                var dlRequest = new HttpRequestMessage(HttpMethod.Get, $"{_patchBaseUrl}/{urlPath}");
                dlRequest.Headers.Host = _patchHost;
                var dlResponse = await _http.SendAsync(dlRequest);
                dlResponse.EnsureSuccessStatusCode();

                byte[] data = await dlResponse.Content.ReadAsByteArrayAsync();
                await File.WriteAllBytesAsync(localPath, data);
            }

            StatusText.Text = T("PatchComplete");
            PatchProgress.Value = 100;
            StartButton.IsEnabled = true;
            _patchComplete = true;
        }
        catch (Exception ex)
        {
            StatusText.Text = $"{T("PatchError")}: {ex.Message}";
            // Allow starting anyway if patch fails
            StartButton.IsEnabled = true;
        }
    }

    private static string ComputeSha256(string filePath)
    {
        using var stream = File.OpenRead(filePath);
        byte[] hash = SHA256.HashData(stream);
        return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
    }

    private void LoadLanguages()
    {
        foreach (var lang in _languages)
        {
            LanguageComboBox.Items.Add(lang.Key);
        }
    }

    private void LoadCurrentLocale()
    {
        // Try to read existing locale.cfg to pre-select the current language
        if (File.Exists(_localeCfgPath))
        {
            try
            {
                string content = File.ReadAllText(_localeCfgPath).Trim();
                // locale.cfg format: "10002 1252 en" (port codepage code)
                string[] parts = content.Split(' ', '\t');
                if (parts.Length >= 3)
                {
                    string code = parts[2].Trim();
                    foreach (var lang in _languages)
                    {
                        if (lang.Value.code == code)
                        {
                            LanguageComboBox.SelectedItem = lang.Key;
                            return;
                        }
                    }
                }
            }
            catch { }
        }

        // Default to English
        LanguageComboBox.SelectedItem = "English";
    }

    private void SaveLocale(string langName)
    {
        if (!_languages.TryGetValue(langName, out var langData))
            return;

        // Write locale.cfg: port codepage code
        string content = $"10002\t{langData.codepage}\t{langData.code}";
        File.WriteAllText(_localeCfgPath, content);

        // Also copy the locale-specific cfg if it exists
        string specificCfg = Path.Combine(_clientPath, langData.cfgFile);
        string targetCfg = Path.Combine(_clientPath, "locale.cfg");

        if (File.Exists(specificCfg) && specificCfg != targetCfg)
        {
            File.Copy(specificCfg, targetCfg, overwrite: true);
        }
    }

    private string T(string key)
    {
        string lang = LanguageComboBox?.SelectedItem as string ?? "English";
        if (_translations.TryGetValue(lang, out var dict) && dict.TryGetValue(key, out var val))
            return val;
        if (_translations["English"].TryGetValue(key, out var fallback))
            return fallback;
        return key;
    }

    private void ApplyTranslation()
    {
        NewsLabel.Text = T("News");
        LanguageLabel.Text = T("Language");
        ServerStatusLabel.Text = T("ServerStatus");
        VersionLabel.Text = T("Version");
        StartButton.Content = T("StartGame");
        StatusText.Text = T("Ready");
    }

    // --- Event Handlers ---

    private void TitleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        DragMove();
    }

    private void Minimize_Click(object sender, RoutedEventArgs e)
    {
        WindowState = WindowState.Minimized;
    }

    private void Close_Click(object sender, RoutedEventArgs e)
    {
        Close();
    }

    private void LanguageComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (LanguageComboBox.SelectedItem is string langName)
        {
            SaveLocale(langName);
            ApplyTranslation();
            StatusText.Text = T("LangSet");
        }
    }

    private void StartButton_Click(object sender, RoutedEventArgs e)
    {
        string exePath = Path.Combine(_clientPath, "zgamecore.exe");

        if (!File.Exists(exePath))
        {
            // If patch didn't complete, game files might still be downloading
            MessageBox.Show(T("GameNotFound"),
                "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            return;
        }

        try
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = exePath,
                WorkingDirectory = _clientPath,
                UseShellExecute = true
            });

            Close();
        }
        catch (Exception ex)
        {
            MessageBox.Show($"{T("GameError")}: {ex.Message}",
                "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }
}

public class PatchFileInfo
{
    public string file { get; set; } = "";
    public string hash { get; set; } = "";
    public long size { get; set; }
}
