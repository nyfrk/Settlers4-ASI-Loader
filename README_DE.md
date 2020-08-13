# Die Siedler 4: ASI-Loader [![GitHub release (latest by date)](https://img.shields.io/github/v/release/nyfrk/Settlers4-ASI-Loader)](https://github.com/nyfrk/Settlers4-ASI-Loader/releases/latest)

Dieses Tool ermöglicht es ASI-Plugins in Die Siedler 4 zu laden.

Für dieses README gibt es eine [englische Version](README.md). Bitte beachte, dass die deutsche Übersetzung ggf. veraltet sein kann.

![dialog](dialog.png)



## Features

* Ermöglicht die Nutzung von ASI Plugins um das Spiel zu erweitern.
* Funktioniert sowohl mit der Gold Edition als auch mit der History Edition von Die Siedler 4.
* Kompatibel mit Windows XP oder neuer.



## Installation

1. Lade ein [Release](https://github.com/nyfrk/Settlers4-ASI-Loader/releases) herunter und starte die `S4_ASI.exe`.
2. Selektiere dein Siedler 4 Installationsverzeichnis durch öffnen der S4_Main.exe. Wenn du die History Edition installiert hast, kannst du diesen Schritt überspringen.
3. Klicke auf Installieren
4. Klicke auf "Plugins Verzeichnis öffnen". In dieses Verzeichnis kannst du deine Plugins verschieben. Der Loader wird nur Plugins mit der Endung .asi beim Starten des Spiels laden. Sie werden in alphabetischer Reihenfolge geladen.



## Probleme und Fragen

Das Projekt verwendet den Github Issue Tracker. Bitte öffne [hier](https://github.com/nyfrk/Settlers4-ASI-Loader/issues) ein Ticket für dein Anliegen.



## Plugins

Hier ist eine Liste von bekannten Plugins, die Du für Die Siedler 4 installieren kannst. Du kannst ein Plugin zu dieser Liste hinzufügen. Die einzige Voraussetzung ist jedoch, dass es mit der neuesten Gold Edition UND der History Edition des Spiels kompatibel ist. 

* [Unbegrenzte Auswahllimit Mod](https://github.com/nyfrk/Settlers4-UnlimitedSelectionMod) von [nyfrk](https://github.com/nyfrk): Entferne das Auswahllimit Deiner Einheiten und füge zusätzliche Auswahlfilteroptionen hinzu.
* [Settler Limit Remover Mod](https://github.com/nyfrk/S4_SettlerLimitRemover) von [nyfrk](https://github.com/nyfrk): Produce as much settlers as you want.
* [Texture Changer](https://github.com/nyfrk/Settlers4-TextureChanger) von [nyfrk](https://github.com/nyfrk): Play always with the New World textures.
* [Ferry Limit Customizer](https://github.com/nyfrk/FerryLimitCustomizer) von [nyfrk](https://github.com/nyfrk): Increase the capacity of your ferries.



## Beitragen

Das offizielle Repository dieses Projekts ist unter https://github.com/nyfrk/Settlers4-ASI-Loader verfügbar. Du kannst auf die folgenden Arten einen Beitrag leisten:

* Beantworte Fragen
* Fehler melden oder bei der Verifizierung dieser helfen
* Code sichten und die vorgeschlagenen Korrekturen testen
* Pull Requests einreichen
* Plugins erstellen oder einreichen (um sie der Liste hinzuzufügen)

##### Kompilieren

Lade Visual Studio 2017 oder 2019 mit der C++-Toolchain herunter. Das Projekt ist so konfiguriert, dass es mit der Windows XP-kompatiblen **v141_xp**-Toolchain gebaut wird. Du solltest jedoch die Toolchain nach Belieben ändern können. Es sind keine zusätzlichen Bibliotheken erforderlich, sodass das Projekt ohne weiteres gebaut werden kann.

##### Entwickler

Der Loader ist eine Proxy-Dll der binkw32.dll. Sie eignet sich gut als Proxy-Ziel, da die Gold Edition und die History Edition die gleiche binkw32.dll verwenden und weil sie nur einige wenige stdcall-Funktionen epxortiert. Beachte, dass ASI-Plugins im Grunde nur reguläre DLLs mit der Dateiendung .asi sind. Diese speziellen DLLs wenden jedoch normalerweise ihre Mods innerhalb ihrer DllMain-Funktion an. Nimm beim Erstellen eines Plugins keinen bestimmten Ladezustand an. Wenn Du auf ein Modul (DLL) angewiesen bist, verwenden LoadLibrary, um es selbst zu laden.

##### Zukünftige Arbeit

* Verbesserte Crash-Dumps, wie es [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) tut. (Die üblichen Fatal Error Meldungen sind nutzlos) 



## Lizenz

Das Projekt ist unter der [MIT](https://github.com/nyfrk/Settlers4-UnlimitedSelectionMod/blob/master/LICENSE.md)-Lizenz lizenziert.
