build dev:
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j
build:
	@cmake -S . -B build && cmake --build build -j
run dev:
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j && ./build/app 
run:
	@cmake -S . -B build && cmake --build build -j && ./build/app 
