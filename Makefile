.PHONY: web desktop clean

web:
	mkdir -p build_web
	./build_web.sh

desktop:
	mkdir -p build_desktop
	cd build_desktop && cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build_desktop && make -j $(shell nproc)
	mkdir -p out_desktop/
	cp build_desktop/NRG_DN03 out_desktop/

clean:
	rm -rf build_web
	rm -rf out_web
	rm -rf build_desktop
	rm -rf out_desktop
