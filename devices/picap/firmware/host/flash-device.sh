#!/bin/bash

BASEDIR="$(dirname $0)"

systemctl stop homie
make -j4 -C $BASEDIR/../app flash
systemctl start homie
