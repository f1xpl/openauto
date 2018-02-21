#!/bin/bash

set -o errexit

scripts_dir="$(dirname "${BASH_SOURCE[0]}")"

# make sure we're running as the owner of the checkout directory
RUN_AS="$(ls -ld "$scripts_dir" | awk 'NR==1 {print $3}')"
if [ "$USER" != "$RUN_AS" ]
then
    echo "This script must run as $RUN_AS, trying to change user..."
    USERNAME=$RUN_AS
    exec sudo -u $RUN_AS $0
else
    USERNAME=$USER
fi
cd /home/$USERNAME/
git clone -b master https://github.com/f1xpl/openauto.git
sed 's/#.*//' /home/$USERNAME/openauto/Requirements.txt | xargs sudo apt-get install -y
git clone -b master https://github.com/f1xpl/aasdk.git
mkdir aasdk_build
cd ./aasdk_build
cmake -DCMAKE_BUILD_TYPE=Release ../aasdk
make
cd /opt/vc/src/hello_pi/libs/ilclient
make
cd /home/$USERNAME/
mkdir openauto_build
cd ./openauto_build
cmake -DCMAKE_BUILD_TYPE=Release -DRPI3_BUILD=TRUE -DAASDK_INCLUDE_DIRS="/home/$USERNAME/aasdk/include" -DAASDK_LIBRARIES="/home/$USERNAME/aasdk/lib/libaasdk.so" -DAASDK_PROTO_INCLUDE_DIRS="/home/$USERNAME/aasdk_build" -DAASDK_PROTO_LIBRARIES="/home/$USERNAME/aasdk/lib/libaasdk_proto.a" ../openauto
make
