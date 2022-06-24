# This is an example PKGBUILD file. Use this as a start to creating your own,
# and remove these comments. For more information, see 'man PKGBUILD'.
# NOTE: Please fill out the license field for your package! If it is unknown,
# then please put 'unknown'.

# Maintainer: Stephen <stephenvdw.social@gmail.com>
pkgname=yt-bgm-player-git
pkgver=1.2.5.r61.6ce36db
pkgrel=1
# epoch=
pkgdesc="A lightweight youtube background music player, for playing YouTube or local music in the background."
arch=(x86_64)
url="https://github.com/WeebNetsu/yt-playlist-bgm-player.git"
license=('GPL-3.0')
# groups=()
depends=(mpv yt-dlp)
makedepends=(git nim)
# checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
# below is any files we do not want to have
# touched in the case of update or delete
# like config files
backup=()
options=()
# install=
# changelog=
source=("git+$url")
noextract=()
md5sums=("SKIP")
# validpgpkeys=()

pkgver(){
	cd "${_pkgname}"
	printf "1.2.5.r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

# prepare() {
# 	cd "$pkgname-$pkgver"
# 	patch -p1 -i "$srcdir/$pkgname-$pkgver.patch"
# }

build() {
	pwd
	# cd yt-playlist-bgm-player
	export NIMBLE_DIR=/tmp/${pkgname}/nimble
	nimble build
	# nim c -d:release -o:ytbgmplayer main.nim
	chmod +x ytbgmplayer
}

# check() {
# 	cd "$pkgname-$pkgver"
# 	make -k check
# }

package() {
	# cd yt-playlist-bgm-player
	mkdir -p ${pkgdir}/usr/bin
	cp -rf ytbgmplayer ${pkgdir}/usr/bin/ytbgmplayer

	# TODO: Add readme and license https://youtu.be/iUz28vbWgVw

	echo "NOTICE: You need yt-dlp to play music from YouTube."
}
