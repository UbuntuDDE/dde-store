<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">Alzuz n yisnasen i DDE yebnan s DTK</h3>


## Acu-t waya?

Asnas-a n ulzuz ittwafeṣel i useqdec akked tnarit Deepin deg isedggiren-nniḍen. Asnas unṣib n Deepin store ittwabna i Debian akked web view
s ukemmus Deepin, aya yerra-t d aramṣaḍan akked ikemmusen-nniḍen. Alzuz-a ittwabna s PackageKit akked AppStream, ihi issefk ad yeddu akken iwata
deg [tuget](#deepin-distro) isedggiren meṛṛa!

## Tussa Deepin

Aya ad iddu kan i tussa tunṣibt n Deepin, mačči i DDE. Deepin ur isefrak ara AppStream, iseddun asnas-a akked isefka yesra.
 **This store is not supported on the Deepin distribution.** This was intended for other distros running DDE anyways, so I have no plans on ever supporting it. If Deepin starts using AppStream, I may consider it. If you're running Deepin and looking for an alternative app store, check out some other awesome projects like [Spark Store](https://www.spark-app.store) or [Deepines Store](https://deepines.com)!

## Asebded
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

## Attekki
Pull requests are always welcome for bug fixes, improvements, translations, or anything else!

## Turagt
[GPLv3]
