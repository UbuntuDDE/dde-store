<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">Ein App Store für DDE, erstellt mit DTK</h3>

## Was ist es?

Dieser App Store ist für die Verwendung mit der Deepin Desktop-Umgebung in anderen Distributionen gedacht. Der offizielle Deepin Store basiert auf Debian und einer Webansicht mit den Deepin-Repositories, was ihn inkompatibel mit anderen Distributionen macht. Dieser Store wurde mit PackageKit und AppStream erstellt und sollte daher perfekt auf [fast](#deepin-distro) jeder Distribution funktionieren!

## Deepin Distro

Dies gilt nur für die offizielle Deepin-Distro, nicht für DDE im Allgemeinen. Deepin unterstützt kein AppStream, was diese App mit allen erforderlichen Daten versorgt. **Dieser Store wird auf der Deepin-Distribution nicht unterstützt.** Es war sowieso für andere Distributionen gedacht, die DDE verwenden, daher habe ich nicht vor, es jemals zu unterstützen. Sollte Deepin anfangen AppStream zu verwendet, werde ich darüber nachdenken. Wenn Sie Deepin nutzen und nach einem alternativen App Store suchen, sehen Sie sich andere tolle Projekte wie [Spark Store](https://www.spark-app.store) oder [Deepines Store](https://deepines.com) an!

## Installation
### Ubuntu
```
sudo add-apt-repository ppa:dekzi/ppa
sudo apt-get update
sudo apt-get install dde-store
```

### Arch/Manjaro (AUR)
```
sudo pacman -S dde-store
```

### Bauen vom Quellcode
**Abhängigkeiten (Ubuntu)**
- cmake
- qtbase5-dev
- libdtkwidget-dev
- libdtkcore-dev
- libdtkgui-dev
- libpackagekitqt5-dev
- libappstreamqt-dev

**Bauen auf Ubuntu**

Um auf Ubuntu zu bauen, wird die UbuntuDDE-PPA benötigt, da `libdtkgui-dev` auf den Hauptrepos nicht verfügbar ist.
```
sudo add-apt-repository ppa:ubuntudde-dev/stable
sudo apt-get update
```

**Bauen**
```
git clone https://github.com/dekzi/dde-store.git
cd dde-store
cmake .
sudo make install
```

## Beiträge
Pull-Anfragen sind für Fehlerbehebungen, Verbesserungen, Übersetzungen oder andere Dinge immer gerne gesehen!

## License
[GPLv3](../../LICENSE)
