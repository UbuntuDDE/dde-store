<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">Una tienda de aplicaciones para DDE hecha con DTK</h3>

## Qué es esto?

Esta tienda de aplicaciones está diseñada para el uso con el Entorno de Escritorio de Deepin (Deepin Desktop Envoirment, o DDE) en otras distribuciones. La tienda oficial de Deepin está hecha a base de Debian y una vista web con los repositorios de Deepin, haciéndola incompatible con otras distribuciones. ¡Esta tienda está hecha con PackageKit y AppStream así que debería trabajar perfecto con [casi](#distribuciones-de-deepin) cualquier distribución!

## Distribuciones de Deepin

This only applies to the official Deepin distro, not DDE in general. Deepin does not support AppStream, which provides this app with all the needed data. **This store is not supported on the Deepin distribution.** This was intended for other distros running DDE anyways, so I have no plans on ever supporting it. If Deepin starts using AppStream, I may consider it. If you're running Deepin and looking for an alternative app store, check out some other awesome projects like [Spark Store](https://www.spark-app.store) or [Deepines Store](https://deepines.com)!

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

### Building from source
**Dependencies (Ubuntu)**
- cmake
- qtbase5-dev
- libdtkwidget-dev
- libdtkcore-dev
- libdtkgui-dev
- libpackagekitqt5-dev
- libappstreamqt-dev

**Building on Ubuntu**

To build on Ubuntu, the UbuntuDDE PPA is needed because `libdtkgui-dev` is unavailable on the main repos.
```
sudo add-apt-repository ppa:ubuntudde-dev/stable
sudo apt-get update
```

**Building**
```
git clone https://github.com/dekzi/dde-store.git
cd dde-store
cmake .
sudo make install
```

## Contributions
Pull requests are always welcome for bug fixes, improvements, translations, or anything else!

## License
[GPLv3](LICENSE)
