#!/bin/bash

BASEDIR="$(dirname $0)"
INSTALLDIR=/opt
TTYDEV=/dev/ttyAMA0
TAPDEV=homie0

make -C $BASEDIR/../../../../firmware/RIOT/dist/tools/ethos ethos
cp $BASEDIR/../../../../firmware/RIOT/dist/tools/ethos/ethos $INSTALLDIR/ethos

cat >/etc/systemd/system/homie.service <<EOF
[Unit]
Description=Homie Bus
Before=network-online.target

[Service]
Type=simple
ExecStart=$INSTALLDIR/ethos $TAPDEV $TTYDEV
ExecStartPost=ip link set $TAPDEV up
Restart=on-failure

[Install]
WantedBy=network.target
EOF

echo "systemctl start homie; systemctl enable homie"
