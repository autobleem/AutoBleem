#!/bin/bash
# find all the _("some text") strings in the code.  handle case if more than one is on a line.
# strip off the _(" and ")
# delete the line |@lang|
# sort and get rid of duplicates
# double space so english.txt can be easily compared to the other lang.txt files to see what has been added or modified.

if [[ "$OSTYPE" != "darwin"* ]]; then
grep -r --include=*.cpp --include=*.h --exclude-dir=libs -h -o '_("[^"]*")' | sed 's/_("//g' | sed 's/")//g' | sed '/^|@lang|$/d' | sed '/^$/d' | sort -u | sed G > src/resources/lang/English.txt
fi
