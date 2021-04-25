#!/bin/bash

NAME="$1"
TEMPLATE="_template_$2"

err() {
    echo $*
    exit 1
}

[ -z "$NAME" ] && err "Project name must be specified"
[ -d "$NAME" ] && err "Project $NAME already existing"
[ -z "$TEMPLATE" ] && err "Template must be specified"
[ ! -d "$TEMPLATE" ] && err "Template $TEMPLATE unknown"

cp -vr $TEMPLATE $NAME
jq ".uuid=\"$(uuid -v 4)\"" <$NAME/pcb/template.hprj >$NAME/pcb/$NAME.hprj
rm $NAME/pcb/template.hprj
