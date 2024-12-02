#!/bin/bash

: "${PARTICLE_BUILDPACK_VERSION=3.3.0}"
: "${PARTICLE_BUILDPACK_IMAGE=particle/buildpack-particle-firmware:$PARTICLE_BUILDPACK_VERSION}"
: "${PARTICLE_PLATFORM_ID=6}"
# 6  = Photon
# 8  = P1
# 10 = Electron

_start_machine() {
	# ensure the docker machine is running
	docker-machine start default
	eval "$(docker-machine env default)"
}

_download_deps() {
  mkdir -p lib
  if [ ! -f lib/neopixel.h ]; then
    echo "Downloading NeoPixel library..."
    curl -L https://raw.githubusercontent.com/technobly/Particle-NeoPixel/refs/heads/master/src/neopixel.h -o lib/neopixel.h
    curl -L https://raw.githubusercontent.com/technobly/Particle-NeoPixel/refs/heads/master/src/neopixel.cpp -o lib/neopixel.cpp
  fi
}

_init_build() {
  _download_deps
	# flatten all the source files in the build directory
	rm -rf build
	mkdir build
	cp lib/* build
	cp src/* build
	for f in "$@"; do cp $f build; done
	echo "========================="
	ls -l build
}

_build() {
	# clean out and setup the target for build artifacts
	rm -rf target
	mkdir target

	# call the particle buildpack
	#   on osx this assumes that `pwd` results in a directory under /Users which is shared by default with docker-machine
	echo "========================="
	docker run -v `pwd`/build:/input \
	           -v `pwd`/target:/output \
	           -e PLATFORM_ID=$PARTICLE_PLATFORM_ID \
	           $PARTICLE_BUILDPACK_IMAGE
}

_post_build() {
	# check results and flash if success
	echo "========================="
	if [ -s target/firmware.bin ]; then
		echo "SUCCESS"
		if [ "$PARTICLE_DEVICE" != "" ]; then
			particle flash $PARTICLE_DEVICE target/firmware.bin
		else
			echo "use: 'particle flash NAME target/firmware.bin' to upload"
		fi
	else
		echo "FAILED"
	fi
}

#_start_machine
_init_build $@
_build
_post_build
