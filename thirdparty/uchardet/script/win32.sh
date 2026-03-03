mkdir --parents win32 \
&& cd win32 \
&& cmake .. \
	-G "MSYS Makefiles" \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX="" \
&& make