<p align="center"><img src="https://user-images.githubusercontent.com/56656996/99621835-b2d8dc80-29dd-11eb-8183-987e80f8b3a7.png"></p>
<h1 align="center">DDE Store</h1>
<h3 align="center">DTK で構築された DDE 用アプリストア</h3>

## DDE Store とは

このアプリストアは、他のディストリビューションの Deepin デスクトップ環境で使用するように設計されています。公式の Deepin ストアは、Debian と Deepin リポジトリを含む Web ビューを中心に構築されているため、他のディストリビューションとの互換性がありません。このストアは PackageKit と AppStream で構築されているため、[ほぼ](#deepin-ディストリビューション)任意のディストリビューションで完璧に動作するはずです！

## Deepin ディストリビューション

以下は、DDE 全般ではなく、公式 Deepin ディストリビューションにのみ適用されます。 Deepin は、このアプリに必要なすべてのデータを提供する AppStream をサポートしていません。 **このストアは Deepin ディストリビューションではサポートされていません。** とにかくこのプロジェクトは、DDE を実行している他のディストリビューションを対象としていたため、Deepin をサポートする予定はありません。Deepin が AppStream を使い始めた場合は、検討するかもしれません。もし Deepin をお使いで、代替のアプリストアをお探しのようでしたら、他の [Spark Store](https://www.spark-app.store) や [Deepines Store](https://deepines.com) のような素晴らしいプロジェクトをチェックしてみてください！

## インストール方法
### UbuntuDDE
```
sudo apt-get install dde-store
```

### Arch
```
sudo pacman -S deepin-store
```

### ソースからビルド
**依存関係 (Ubuntu)**
- cmake
- qtbase5-dev
- qttools5-dev
- libdtkwidget-dev
- libdtkcore-dev
- libdtkgui-dev
- libpackagekitqt5-dev
- libappstreamqt-dev
- libsnapd-qt-dev

**ビルド**
```
git clone https://github.com/dekzi/dde-store.git
cd dde-store
cmake .
sudo make install
```

## 貢献
バグ修正、改善、翻訳など、プルリクエストはいつでも歓迎です！

## ライセンス
[GPLv3](../../LICENSE)
