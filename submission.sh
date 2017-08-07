reg_id=19902dd7-1ef6-43c8-8ddd-17d67806316f
mv icfp-$reg_id.tar.gz $(mktemp old/icfp-$reg_id.tar.gz_XXXX)

cmake .
make

rm /tmp/src
mkdir /tmp/src
cp -r {include,model,punter,punter2,solver,tests,CMakeLists.txt} /tmp/src

tar -czvf icfp-$reg_id.tar.gz install PACKAGES README \
-C bin punter \
-C /tmp src/include src/model src/punter src/punter2 src/solver src/tests src/CMakeLists.txt