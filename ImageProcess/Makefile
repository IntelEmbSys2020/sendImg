Demo: client.cpp server.cpp
	g++ -g -o ./build/client client.cpp ImageProcess_copy.cpp -I ../include -Wl,-rpath=$(MVCAM_COMMON_RUNENV)/64 -L$(MVCAM_COMMON_RUNENV)/64 -lMvCameraControl
	g++ -g -o ./build/server server.cpp  -I ../include -Wl,-rpath=$(MVCAM_COMMON_RUNENV)/64 -L$(MVCAM_COMMON_RUNENV)/64 -lMvCameraControl
	

clean:
	rm client -rf
