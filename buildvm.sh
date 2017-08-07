cmake .
make
rm ./log.json
./lamduct-0.3 --game-port 9725 --log-level 2 --client-instance-logfile log.json ./bin/punter
