<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">An app store for DDE built with DTK</h3>


## What is this?

This app store is designed for use with the Deepin Desktop Environment on other distributions. The official Deepin store is built around Debian and a web view with the Deepin repos on it, making it incompatible with other distros. This store is built with PackageKit and AppStream, so it should work perfect on [almost](#deepin-distro) any distro!

## Deepin Distro

This only applies to the official Deepin distro, not DDE in general. Deepin does not support AppStream, which provides this app with all the needed data. **This store is not supported on the Deepin distribution.** This was intended for other distros running DDE anyways, so I have no plans on ever supporting it. If Deepin starts using AppStream, I may consider it. If you're running Deepin and looking for an alternative app store, check out some other awesome projects like [Spark Store](https://www.spark-app.store) or [Deepines Store](https://deepines.com)!

## Installation
### UbuntuDDE
```
sudo apt-get install dde-store
```

### Arch
```
sudo pacman -S deepin-store
```

### Building from source
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

## Contributions
Pull requests are always welcome for bug fixes, improvements, translations, or anything else!

## License
[GPLv3](LICENSE)
