-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: freetype
Binary: libfreetype6, libfreetype-dev, libfreetype6-dev, freetype2-demos, freetype2-doc, libfreetype6-udeb
Architecture: any all
Version: 2.10.1-2ubuntu0.2
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Anthony Fok <foka@debian.org>, Keith Packard <keithp@keithp.com>
Homepage: https://www.freetype.org
Standards-Version: 4.4.1
Vcs-Browser: https://salsa.debian.org/debian/freetype
Vcs-Git: https://salsa.debian.org/debian/freetype.git
Build-Depends: debhelper-compat (= 12), bzip2, gettext, libx11-dev <!stage1>, x11proto-core-dev <!stage1>, libz-dev, pkg-config, libpng-dev, autoconf, libtool, libjs-jquery
Package-List:
 freetype2-demos deb utils optional arch=any profile=!stage1
 freetype2-doc deb doc optional arch=all
 libfreetype-dev deb libdevel optional arch=any
 libfreetype6 deb libs optional arch=any
 libfreetype6-dev deb oldlibs optional arch=any
 libfreetype6-udeb udeb debian-installer optional arch=any
Checksums-Sha1:
 26464f20441409af345aea2ffc0218c23ec6aa95 305328 freetype_2.10.1.orig-ft2demos.tar.gz
 e4f107d7fa28ad93b53f238f4bda7c9c42d2b443 195 freetype_2.10.1.orig-ft2demos.tar.gz.asc
 20fe80d5bf4b6dba3c43449bc58f990572ea3e06 2123885 freetype_2.10.1.orig-ft2docs.tar.gz
 e290f03d39c7b8ef48be3c32f8075d2e222c9f14 195 freetype_2.10.1.orig-ft2docs.tar.gz.asc
 3296b64ad1e7540289f22e4b6383e26e928b0a20 3478158 freetype_2.10.1.orig.tar.gz
 44b71af13fc400addd1d1a97cb587716cb150bac 195 freetype_2.10.1.orig.tar.gz.asc
 948a4882724deb3daf3d6c53dd0061a1c5693876 116652 freetype_2.10.1-2ubuntu0.2.debian.tar.xz
Checksums-Sha256:
 5e9e94a2db9d1a945293a1644a502f6664a2173a454d4a55b19695e2e2f4a0bc 305328 freetype_2.10.1.orig-ft2demos.tar.gz
 ccee51c4b4101b89a66ba5f2bdd54d127e93e120086982db57fa33761f310e9e 195 freetype_2.10.1.orig-ft2demos.tar.gz.asc
 a4e4a8e69c7bf833eba7c158254a572fd43131d5e9b8791bd2ecbb03546ce155 2123885 freetype_2.10.1.orig-ft2docs.tar.gz
 aaedd84036d9e615fbb5acf71081dd05c9d7333686593432e445ee89655a79c9 195 freetype_2.10.1.orig-ft2docs.tar.gz.asc
 3a60d391fd579440561bf0e7f31af2222bc610ad6ce4d9d7bd2165bca8669110 3478158 freetype_2.10.1.orig.tar.gz
 3952cc2651536ef5157601143d1efc453a7fe5ca64eaf765d034c417aabd4210 195 freetype_2.10.1.orig.tar.gz.asc
 acea22b20f822dc79a24ee1704f23c6b9345ca4bf3fe4527fb54fefa90626451 116652 freetype_2.10.1-2ubuntu0.2.debian.tar.xz
Files:
 c376adf4782bac9b9ac8e427884752d2 305328 freetype_2.10.1.orig-ft2demos.tar.gz
 ae119c24f39209dde9ed18d106b22ff8 195 freetype_2.10.1.orig-ft2demos.tar.gz.asc
 c1ea33faf45dd11ef620356627d9cdd2 2123885 freetype_2.10.1.orig-ft2docs.tar.gz
 871c334c76782a65548e2ef9933fbfc5 195 freetype_2.10.1.orig-ft2docs.tar.gz.asc
 c50a3c9e5e62bdc938a6e1598a782947 3478158 freetype_2.10.1.orig.tar.gz
 7504395b82bc9ff6767810ce9a3c275d 195 freetype_2.10.1.orig.tar.gz.asc
 b86e7d0b0bfcc5912f92d88d4645412b 116652 freetype_2.10.1-2ubuntu0.2.debian.tar.xz
Original-Maintainer: Hugh McMaster <hugh.mcmaster@outlook.com>

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEUMSg3c8x5FLOsZtRZWnYVadEvpMFAmLW39wACgkQZWnYVadE
vpPX3w//Y9LYQ4e+vmG5H9HVbWW4ccoy3CHU1NIkDZJYlOSUb0nKmh0IKcSjZew9
TzofoN+LHdwxXObYjYrRQcou4u0opgrExtn02sNJKZERVWn1Ic+I8PDNAp9twH9c
stgIzJxR7BDNmbeK6b9TDmjaMsZAYBSWw/9NjEOkNdVBbnQjcLI0NLLl+MBvImCT
e6gVTaLH2jE/oWnypfy2bFXBpFwDiFTZivAudeYCInp1OrZDhuIGPnStO3nFgNhV
9dQ58PCjxDG5xg6CArNDs5ALWXTVSRXOYQr1OBn3uZeBoQ0pbxjTIdk817m+CHQ/
13gSXBkAzttMhbVDuEZdaA2CXMCK/CSwq1Oeu6fP8aNwtZWyo0gaXySV1ipbWZlC
Wx7YQlDclRyauLGSu2Re2S3hjmVykkGgkoP1btfR0RKNZ+HTAKSyrle1tLtQZYlx
95gREGhhwHMawpFTA7udfkqwfesiQeKmFgG7TMhj7MjGxh/RSTOioEKYbwf1qDhC
JKew2uc71DgD0ZY1Yz4/jOXhT3s0S+RJASdWIisEBMXl6wmvd0kyTpq3HvqUSaQQ
VYEHsghTa5m5dDCoBFuKzro7XQ5OLL90HjVrHyHPj2XNRXXQTGuCr9ajNBmz31I/
2Z8eujDBIqB83j9D6kX/YAM1WG78AuEwm8t4+e2CHnf+q3CfOG4=
=70uE
-----END PGP SIGNATURE-----
