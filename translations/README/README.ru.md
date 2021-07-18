<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">Магазин приложений для DDE, написанный с использованием DTK</h3>


## Что это?

Это приложение предназначено для использования С Deepin Desktop Environment, поставляемым в других дистрибутивах. Официальный магазин Deepin написан для Debian и репозиториев Deepin, что делает его несовместимым с другими дистрибутивами. Этот магазин написан с использованием PackageKit и AppStream, благодаря чему он должен работать [почти](#deepin-distro) с каждым дистрибутивом!

## Дистрибутив Deepin

Это относится только к официальному дистрибутиву Deepin, а не к DDE. Deepin не поддерживает AppStream, предоставляющий этому приложению все необходимые данные. **Этот магазин не поддерживает дистрибутив Deepin.** В любом случае этот магазин был создан для других дистрибутивов, использующих DDE, поэтому у меня нет никаких планов его поддерживать. Если Deepin начнет использовать AppStream, то я могу задуматься об этом. Если вы используете Deepin и ищете альтернативный магазин, обратите внимание на другие превосходные проекты, вроде [Spark Store](https://www.spark-app.store) или [Deepines Store](https://deepines.com)!

## Установка
### UbuntuDDE
```
sudo apt-get install dde-store
```

### Arch
```
sudo pacman -S deepin-store
```

### Сборка из исходного кода
**Зависимости (Ubuntu)**
- cmake
- qtbase5-dev
- qttools5-dev
- libdtkwidget-dev
- libdtkcore-dev
- libdtkgui-dev
- libpackagekitqt5-dev
- libappstreamqt-dev
- libsnapd-qt-dev

**Сборка**
```
git clone https://github.com/dekzi/dde-store.git
cd dde-store
cmake .
sudo make install
```

## Внести вклад
Пул-реквесты с исправлениями ошибок, улучшениями, переводами и прочим всегда приветствуются!

## Лицензия
[GPLv3](../../LICENSE)
