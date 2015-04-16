#!/bin/sh

cat ~veprbl/libepecur/contrib/list_apr10 | \
	parallel \
		--colsep '\t' \
		--jobs 8 \
"
source ~veprbl/.zprofile
echo Processing {1}
rm -rf ~veprbl/plots/{1}
mkdir ~veprbl/plots/{1}
cd ~veprbl/plots/{1}
EPECUR_ROOTFILE=~veprbl/pass1_full/{1}.root \
EPECUR_ROOTFILE2=~veprbl/pass2_full/{1}.root \
ionice -c 3 nice -n 10 \
~veprbl/libepecur/build/make_plots > ~veprbl/plots/{1}/stdout.log 2>&1
"
