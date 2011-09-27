java -cp ../lib/client.jar borealis.client.Subscription
xterm -e sh -l -c "java -cp . TcpDump" &
sleep 2
./subscribe

