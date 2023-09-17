#!/usr/bin/env bash
get-key() {
    while read -r n; do
        if ((n < 0)); then
            n=$((n + 128 | 128)) # (n + 128) | 128
        fi
        printf '%x' "$n"
    done < <(curl -q 'https://raw.githubusercontent.com/dbeaver/dbeaver/devel/plugins/org.jkiss.dbeaver.registry/src/org/jkiss/dbeaver/registry/BaseProjectImpl.java' 2> /dev/null |
        grep -F 'LOCAL_KEY_CACHE =' |
        cut -d= -f2- |
        cut '-d{' -f2- |
        sed -r -e 's/\};$//' -e 's/,//g' |
        perl -lpe 's/^\s+|\s+$//g' |
        tr ' ' '\n')
}
key=$(get-key)
sed -re "s/^KEY=.*/KEY=${key}/" -i ./dbeaver-creds
sed -re "s/^set KEY=.*/set KEY=${key}/" -i ./dbeaver-creds.bat
sed -re "s/^  KEY =.*/  KEY = '${key}'/" -i ./DBeaverCreds/DBeaverCreds.psm1
