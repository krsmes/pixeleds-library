#!/bin/bash

# "particle library upload" is giving a network error, this works...
tar -zcvf library.tar.gz --exclude=".git" --exclude=".idea" --exclude="library.tar.gz" --exclude="build" --exclude="target" --exclude="*.bin" .
curl -F "archive=@library.tar.gz" "https://api.particle.io/v1/libraries?access_token=$1"
