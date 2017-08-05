cmake .
make
rm ./err.log
./lamduct-0.3 --game-port 9008 --log-level 3 --client-instance-logfile err.log ./bin/punter
