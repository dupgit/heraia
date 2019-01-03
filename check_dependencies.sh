#!/bin/bash
#
# Checks packages dependencies. Based on a script by Tamir ?? 
# (http://ubuntuforums.org/showthread.php?t=51003)
#

echo "Please wait while seeking package dependencies..."

strace -f -o /tmp/log ./configure >/dev/null 2>&1

dpkg -S $(grep open /tmp/log | perl -pe 's!.* open\(\"([^\"]*).*!$1!' | grep "^/" |\
grep -v "^\(/tmp\|/dev\|/proc\)" | sort | uniq) >/tmp/files 2>/dev/null

cut -d":" -f1 /tmp/files | sed -e s/,\ /'\n'/g | sort | uniq >/tmp/packages

rm -f /tmp/packages-build-dep /tmp/packages-dep

for p in $(cat /tmp/packages); 
 do
   dpkg -s "${p}" >/tmp/packages-desc;
   version=$(grep ^Version /tmp/packages-desc | cut -f2 -d":");

	if [[ $(grep ^Section /tmp/packages-desc | grep -e "devel") ]]; then
		echo -n "${p} (>=${version}), " >>/tmp/packages-build-dep;
	else
		echo -n "${p} (>=${version}), " >>/tmp/packages-dep;
	fi;
 done;

echo -e "\b\b " >>/tmp/packages-build-dep
echo -e "\b\b " >>/tmp/packages-dep

echo "This package depends on :"

cat /tmp/packages-build-dep
cat /tmp/packages-dep


# Cleaning a bit...
#rm -f /tmp/log /tmp/files /tmp/packages /tmp/packages-build-dep /tmp/packages-dep
