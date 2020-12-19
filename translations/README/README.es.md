<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">Una tienda de aplicaciones para DDE hecha con DTK</h3>

## Qué es esto?

Esta tienda de aplicaciones está diseñada para el uso con el Entorno de Escritorio de Deepin (Deepin Desktop Envoirment, o DDE) en otras distribuciones. La tienda oficial de Deepin está hecha a base de Debian y una vista web con los repositorios de Deepin, haciéndola incompatible con otras distribuciones. ¡Esta tienda está hecha con PackageKit y AppStream así que debería trabajar perfecto con [casi](#distribución-deepin) cualquier distribución!

## Distribución Deepin

Esto solo aplica a la distribución oficial de Deepin, no DDE en general. Deepin no es compatible con AppStream, lo que provee a DDE Store de todos los datos necesarios.. **Esta tienda no es compatible con la distribución Deepin** De todas formas, DDE Store fue diseñada para otras distribuciones con DDE, así que no tengo planes de hacerla compatible con Deepin. Si Deepin empieza a usar AppStream, podría considerarlo. ¡Si estás usando Deepin y estás buscando una tienda de aplicaciones, prueba otros proyectos muy buenos como [Spark Store](https://www.spark-app.store) o [Deepines Store](https://deepines.com)!

## Instalación
### Ubuntu
```
sudo add-apt-repository ppa:dekzi/ppa
sudo apt-get update
sudo apt-get install dde-store
```

### Arch/Manjaro
```
sudo pacman -S deepin-store
```

### Construyéndolo desde la fuente
**Dependencias (Ubuntu)**
- cmake
- qtbase5-dev
- qttools5-dev
- libdtkwidget-dev
- libdtkcore-dev
- libdtkgui-dev
- libpackagekitqt5-dev
- libappstreamqt-dev

**Construyéndolo en Ubuntu**

Para construirlo en Ubuntu, se necesita el PPA de UbuntuDDE porque `libdtkgui-dev` no está disponible en el repositorio principal.
```
sudo add-apt-repository ppa:ubuntudde-dev/stable
sudo apt-get update
```

**Construyéndolo**
```
git clone https://github.com/dekzi/dde-store.git
cd dde-store
cmake .
sudo make install
```

## Contribuciones
¡Pull requests son siempre bienvenidas para arreglo de bugs, mejoras, traducciones, o cualquier otra cosa!

## Licencia
[GPLv3](../../LICENSE)
