#!/bin/bash
#
# Author : Olivier Delhomme
# Licence : GNU GPL v2
#

##
# Deletes all files passed in arguments
#
function delete_files {

    for file in "${@}"; do
        if test -f "${file}"; then
            echo "${file}"
            rm -f "${file}"
        fi
    done
}


##
# Deletes a file everywhere in the current path
#
function delete_files_everywhere {

    for file in "${@}"; do
        echo "find . -name ${file} -exec rm -f {} \;"
        find . -name "${file}" -exec rm -f {} \;
    done
}


if [ -f Makefile ];
 then
    make distclean
fi;

delete_files "aclocal.m4" "autom4te.cache" "config.guess" "config.sub" "depcomp" 
delete_files "INSTALL" "install-sh" "ltmain.sh" "missing" "configure"
delete_files "config.status" "config.log" "libtool" 
delete_files_everywhere "Makefile" "Makefile.in" '*~' '*#"' '*.gcd[ao]' '*.gcov'
delete_files_everywhere '*.gcno' 'gmon.out'
