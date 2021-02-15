#!/bin/bash

NAME="$1"

err() {
    echo $*
    exit 1
}

[ -z "$NAME" ] && err "Project name must be specified"
[ -d "$NAME" ] && err "Project $NAME already existing"

cp -vr _template $NAME
jq ".uuid=\"$(uuid -v 4)\"" <$NAME/pcb/template.hprj >$NAME/pcb/$NAME.hprj
rm $NAME/pcb/template.hprj
