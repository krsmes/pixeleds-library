#!/bin/bash

# "particle library upload" is giving a network error, this works...
tar -zcvf library.tar.gz --exclude=".git" --exclude=".idea" --exclude="library.tar.gz" .
curl -F "archive=@library.tar.gz" "https://api.particle.io/v1/libraries?access_token=$1"
