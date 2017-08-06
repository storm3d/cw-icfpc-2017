cmake .
make
rm ./err.log
./lamduct-0.3 --game-port 9048 --log-level 2 --client-instance-logfile err.log ./bin/punter
