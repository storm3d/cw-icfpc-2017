cmake .
make
rm ./log.json
./lamduct-0.3 --game-port 9190 --log-level 1 --client-instance-logfile log.json ./bin/punter
