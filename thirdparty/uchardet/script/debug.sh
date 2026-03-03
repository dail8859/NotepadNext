mkdir --parents debug \
&& cd debug

cmake \
	-DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_INSTALL_PREFIX=`pwd`/root \
	.. \
&& make \
&& make install
