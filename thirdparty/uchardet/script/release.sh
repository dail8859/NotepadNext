mkdir --parents release \
&& cd release

cmake \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=/usr \
	.. \
&& make
