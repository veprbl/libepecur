#!/bin/bash

function print_version_definition()
{
	echo -n "const char *GIT_COMMIT_ID = \""

	git diff --quiet
	UNSTAGED=$?

	if [ $UNSTAGED = 129 ]; then
		echo "not in git\";"
		exit 0
	fi

	git diff --cached --quiet
	STAGED=$?

	if [ $STAGED = 1 ] || [ $UNSTAGED = 1 ]; then
		echo "dirty working tree\";"
		exit 0
	fi

	git rev-parse HEAD | xargs echo -n
	echo "\";"
}

DEFINITION_STR=`print_version_definition`
FILE_CONTENTS=`cat $1 2> /dev/null`
if [ "$DEFINITION_STR" != "$FILE_CONTENTS" ]; then
        echo $DEFINITION_STR > $1
fi
