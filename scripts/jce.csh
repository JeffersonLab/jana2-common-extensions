#!/bin/csh -f
# Use the Bash wrapper so both entry points share configuration semantics.
set script_dir = "`dirname "$0"`"
exec bash "${script_dir}/jce.sh" $argv:q
