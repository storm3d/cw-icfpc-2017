cmake .
make
rm ./err.log
./lamduct-0.3 --game-port 9007 --log-level 1 --client-instance-logfile err.log ./bin/punter
