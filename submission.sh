reg_id=19902dd7-1ef6-43c8-8ddd-17d67806316f
rm ./bin/punter
mv icfp-$reg_id.tar.gz $(mktemp old/icfp-$reg_id.tar.gz_XXXX)
cmake .
make
tar -czvf icfp-$reg_id.tar.gz ./bin/punter