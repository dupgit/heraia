#!/bin/sh
#
# Checks packages dependencies. Based on a script by Tamir ?? 
# (http://ubuntuforums.org/showthread.php?t=51003)
#

echo "Please wait while seeking package dependencies..."

strace -f -o /tmp/log ./configure >/dev/null 2>&1

dpkg -S $(grep open /tmp/log | perl -pe 's!.* open\(\"([^\"]*).*!$1!' | grep "^/" |\
grep -v "^\(/tmp\|/dev\|/proc\)" | sort | uniq) >/tmp/files 2>/dev/null

cat /tmp/files | cut -d":" -f1 | sed -e s/,\ /'\n'/g | sort | uniq >/tmp/packages

echo "This package depends on :"

for p in $(cat /tmp/packages); 
 do
   version=$(dpkg -s $p | grep ^Version | cut -f2 -d":");
   echo -n "$p (>=$version), ";
 done;

echo -e "\b\b ";

# Cleaning a bit...
rm -f /tmp/log /tmp/files /tmp/packages
