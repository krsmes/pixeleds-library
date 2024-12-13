#!/bin/bash

# Use environment PLATFORM_NAME if set, otherwise use first argument, fallback to p2
: "${PLATFORM_NAME:=${1:-p2}}"
[ $# -gt 0 ] && shift 2>/dev/null  # Shift only if there were arguments

# Convert platform name to ID if not already set
if [ -z "$PARTICLE_PLATFORM_ID" ]; then
    if [[ "$PLATFORM_NAME" =~ ^[0-9]+$ ]]; then
        : "${PARTICLE_PLATFORM_ID:=$PLATFORM_NAME}"
    else
        case "${PLATFORM_NAME}" in
            "photon")
                : "${PARTICLE_PLATFORM_ID:=6}"
                ;;
            "electron")
                : "${PARTICLE_PLATFORM_ID:=10}"
                ;;
            "photon2"|"p2")
                : "${PARTICLE_PLATFORM_ID:=32}"
                ;;
            *)
                echo "Error: Unknown platform '${PLATFORM_NAME}'. Valid platforms: photon (6), electron (10), photon2/p2 (32), or any numeric ID" >&2
                exit 1
                ;;
        esac
    fi
fi


# Set buildpack version based on platform ID
: "${PARTICLE_BUILDPACK_VERSION:=$([ "$PARTICLE_PLATFORM_ID" -le 10 ] && echo "3.3.1" || echo "5.9.0")}"
: "${PARTICLE_BUILDPACK_IMAGE=particle/buildpack-particle-firmware:$PARTICLE_BUILDPACK_VERSION}"

echo "Building for platform: $PLATFORM_NAME ($PARTICLE_PLATFORM_ID) using buildpack: $PARTICLE_BUILDPACK_IMAGE"


_start_machine() {
	# ensure the docker machine is running
	docker-machine start default
	eval "$(docker-machine env default)"
}

HAS_EXTRA_FILES=0
_init_build() {
    rm -rf target/firmware.bin
	# flatten all the source files in the build directory
	rm -rf build
	mkdir build
	cp src/* build
	if [ $# -gt 0 ]; then
        for f in "$@"; do cp $f build; done
        HAS_EXTRA_FILES=1
    fi
	echo "========================= SOURCE FILES"
	ls -l build
	echo "========================="
}

_build() {
	# clean out and setup the target for build artifacts
	rm -rf target
	mkdir target

	# call the particle buildpack
	#   on osx this assumes that `pwd` results in a directory under /Users which is shared by default with docker-machine
	echo "========================= BUILDING"
	if [ $HAS_EXTRA_FILES -eq 1 ]; then
        docker run -v `pwd`/build:/input -v `pwd`/target:/output \
			-e PLATFORM_ID=$PARTICLE_PLATFORM_ID $PARTICLE_BUILDPACK_IMAGE 2>&1 | grep -v -E "^(arm-none-eabi-gcc|arm-none-eabi-g\+\+|mkdir|Invoking)"
    else
        docker run -v `pwd`/build:/input \
			-e PLATFORM_ID=$PARTICLE_PLATFORM_ID $PARTICLE_BUILDPACK_IMAGE 2>&1 | grep -v -E "^(arm-none-eabi-gcc|arm-none-eabi-g\+\+|mkdir|Invoking)"
    fi
}

_post_build() {
	# check results and flash if success
	echo "========================= RESULTS"
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
