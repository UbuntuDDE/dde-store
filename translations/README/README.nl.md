<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">Een app store voor DDE gebouwd met DTK</h3>

## Wat is dit?

 Deze App Store is ontworpen voor gebruik met de Deepin Desktop Environment op andere distributies. De officiële Deepin-winkel is gebouwd rond Debian en een webweergave met de Deepin-repo's erop, waardoor deze niet compatibel is met andere distributies. Deze App Store is gebouwd met PackageKit en AppStream, dus het zou perfect moeten werken op [bijna](#deepin-distro) alle distributies!

## Deepin Distro

Dit is alleen van toepassing op de officiële Deepin-distro, niet op DDE in het algemeen. Deepin ondersteunt AppStream niet, die deze app van alle benodigde gegevens voorziet. **Deze App Store wordt niet ondersteund op de Deepin-distributie.** Dit was sowieso bedoeld voor andere distributies met DDE, dus ik heb geen plannen om het ooit te ondersteunen. Als Deepin AppStream gaat gebruiken, kan ik het overwegen. Als je Deepin gebruikt en op zoek bent naar een alternatieve app store, bekijk dan enkele andere geweldige projecten zoals [Spark Store](https://www.spark-app.store) of [Deepines Store](https://deepines.com)!

## Installatie
### UbuntuDDE
```
sudo apt-get install dde-store
```

### Arch
```
sudo pacman -S deepin-store
```

### Vanaf de bron installeren
**Dependencies (Ubuntu)**
- cmake
- qtbase5-dev
- qttools5-dev
- libdtkwidget-dev
- libdtkcore-dev
- libdtkgui-dev
- libpackagekitqt5-dev
- libappstreamqt-dev
- libsnapd-qt-dev

**Building**
```
git clone https://github.com/dekzi/dde-store.git
cd dde-store
cmake .
sudo make install
```

## Bijdrage aan ontwikkeling
Upload verzoeken zijn altijd welkom voor bugfixes, verbeteringen, vertalingen of iets anders!

## Licentie
[GPLv3](../../LICENSE)