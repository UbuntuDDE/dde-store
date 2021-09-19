<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">Uma loja de aplicativos para DDE contruída com DTK</h3>

## O que é isso?

Esta loja foi projetada para uso com o Deepin Desktop Environment em outras distribuições. A loja oficial da Deepin é construída em torno do Debian e uma página web com os repositórios do Deepin nela, tornando-a incompatível com outras distros. Esta loja é construída com PackageKit e AppStream, então deve funcionar perfeitamente em [quase] (#deepin-distro) qualquer distro!

## Deepin Distro

Isso se aplica apenas à distribuição oficial da Deepin, não ao DDE em geral. Deepin não oferece suporte a AppStream, que fornece a este aplicativo todos os dados necessários. **Esta loja não é compatível com a distribuição Deepin.** Esta loja foi projetada para outras distros executando DDE, então não tenho planos de oferecer suporte. Se o Deepin começar a usar AppStream, posso considerar isso. Se você estiver executando o Deepin e procurando uma loja de aplicativos alternativa, verifique alguns outros projetos incríveis como [Spark Store] (https://www.spark-app.store) ou [Deepines Store] (https://deepines.com)!

## Instalação
### UbuntuDDE
```
sudo apt-get install dde-store
```

### Arch
```
sudo pacman -S deepin-store
```

### Empacotando a partir da fonte
**Dependências (Ubuntu)**
- cmake
- qtbase5-dev
- qttools5-dev
- libdtkwidget-dev
- libdtkcore-dev
- libdtkgui-dev
- libpackagekitqt5-dev
- libappstreamqt-devLicença
- libsnapd-qt-dev

**Empacotando**
```
git clone https://github.com/dekzi/dde-store.git
cd dde-store
cmake .
sudo make install
```

## Contribuições
Pull requests são sempre bem-vindas para correções de bugs, melhorias, traduções ou qualquer outra coisa!

## Licença
[GPLv3](LICENSE)
