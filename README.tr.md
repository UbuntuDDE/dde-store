<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">DTK ile oluşturulmuş DDE için bir uygulama mağazası</h3>


## Bu nedir?
 
Bu uygulama mağazası, Deepin Masaüstü Ortamı'nın (Deepin Desktop Environment, DDE) diğer dağıtımlar üzerinde kullanılması amacıyla tasarlanmıştır. Resmi Deepin mağazası Debian ve üzerinde Deepin depolarının bulunduğu bir web görünümü kapsamında inşa edilmiştir, bu da Deepin mağazasını diğer dağıtımlarla uyumsuz hale getirir. Bu mağaza, PackageKit ve AppStream ile oluşturulmuştur, bu nedenle herhangi bir dağıtım üzerinde [neredeyse](#deepin-dağıtımı) mükemmel çalışmalıdır!

## Deepin Dağıtımı

Bu sadece resmi Deepin dağıtımı için geçerlidir, genel olarak DDE için geçerli değildir. Deepin, bu uygulamaya gerekli tüm verileri sağlayan AppStream'i desteklemez. **Bu mağaza, Deepin dağıtımında desteklenmiyor.** Bu, yine de DDE'yi çalıştıran diğer dağıtımlar için tasarlandı, bu yüzden onu desteklemeye dair hiçbir planım yok. Deepin AppStream'i kullanmaya başlarsa, düşünebilirim. Deepin kullanıyor ve alternatif bir uygulama mağazası arıyorsanız, [Spark Store](https://www.spark-app.store) veya [Deepines Store](https://deepines.com) gibi diğer harika projelere göz atın!

## Kurulum
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

### Kaynak dosyadan kurulum
**Dependencies (Ubuntu)**
- cmake
- qtbase5-dev
- libdtkwidget-dev
- libdtkcore-dev
- libdtkgui-dev
- libpackagekitqt5-dev
- libappstreamqt-dev

**Ubuntu'da kurulum**

Ubuntu'da kurulum için, UbuntuDDE PPA gereklidir çünkü `libdtkgui-dev` ana depolar üzerinde kullanılamaz.
```
sudo add-apt-repository ppa:ubuntudde-dev/stable
sudo apt-get update
```

**Kurulum**
```
git clone https://github.com/dekzi/dde-store.git
cd dde-store
cmake .
sudo make install
```

## Katkılar
Hata düzeltmeleri, iyileştirmeler, çeviriler veya başka herhangi bir şey için çekme talepleri (pull requests) her zaman memnuniyetle karşılanır!

## Lisans
[GPLv3](LICENSE)
