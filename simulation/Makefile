build: build/.touch
	@$(MAKE) -C build

build/.touch: CMakeLists.txt
	mkdir -p build
	cd build && cmake ..
	touch $@

clean:
	rm -rf build
	git -C simavr reset .
	git -C simavr restore .
	git -C simavr clean -xdf

.PHONY: build clean
