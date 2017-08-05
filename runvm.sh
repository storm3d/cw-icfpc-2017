cmake .
make
lamduct --game-port 9031 --log-level 3 --client-instance-logfile err.log ./bin/punter
