using System.Diagnostics;
using System.IO;
using System.Net.Http;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Patcher;

public partial class MainWindow : Window
{
    private const string NewsUrl = "http://192.168.184.132/news";
    private const string NewsHost = "metin2-ignition.local";
    private static readonly HttpClient _http = new();

    private readonly string _clientPath;
    private readonly string _localeCfgPath;

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

    public MainWindow()
    {
        InitializeComponent();

        // Determine client path (same folder as patcher or parent)
        _clientPath = AppDomain.CurrentDomain.BaseDirectory;
        _localeCfgPath = Path.Combine(_clientPath, "locale.cfg");

        LoadLanguages();
        LoadCurrentLocale();
        _ = LoadNewsAsync();
    }

    private async Task LoadNewsAsync()
    {
        try
        {
            var request = new HttpRequestMessage(HttpMethod.Get, NewsUrl);
            request.Headers.Host = NewsHost;
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
                NewsText.Text = "No announcements yet.";
            }
        }
        catch
        {
            NewsText.Text = "Could not load news.";
        }
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
            StatusText.Text = $"Language set to {langName}";
        }
    }

    private void StartButton_Click(object sender, RoutedEventArgs e)
    {
        string exePath = Path.Combine(_clientPath, "gamecore.exe");

        if (!File.Exists(exePath))
        {
            MessageBox.Show("Game files not found!\nMake sure the launcher is in the game folder.",
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
            MessageBox.Show($"Failed to start game: {ex.Message}",
                "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }
}
