rm ./bin/punter
mv icfp-19902dd7-1ef6-43c8-8ddd-17d67806316f.tar.gz $(mktemp old/icfp-19902dd7-1ef6-43c8-8ddd-17d67806316f.tar.gz_XXXX)
cmake .
make
tar -czvf icfp-19902dd7-1ef6-43c8-8ddd-17d67806316f.tar.gz /bin/punter