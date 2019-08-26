#!/bin/bash

#get version
./process --version > VERSION

#documentation
##logo
cp -p doc/logo.png doc/html/
##version
VERSION=`cat VERSION`
cat Doxyfile.template | sed 's/##VERSION##/'$VERSION'/' > Doxyfile
##clean *_help.output
for f in *_help.output
do
  ft=`basename $f _help.output`.help.output
  cat $f | sed 's/..0.31.59m//;s/..1m//;s/..0.0.0m//;s/..0.32.59m//;' > $ft
  mv $ft $f
done

##git graph
#dot -Gdpi=300 -Tpng graph.dot > graph.png
convert graph.png -resize 23% doc/git_graph.png

##make (html and prepare latex)
doxygen

ls doc/html/index.html
#GUI notify
notify-send 'documentation for RockAMali '$VERSION' doxygen ends,\ni.e. '$0' from "'$PWD'".' --icon=/usr/share/hardinfo/pixmaps/report-large.png -t 4321

exit

##make latex
cd doc/latex
make

