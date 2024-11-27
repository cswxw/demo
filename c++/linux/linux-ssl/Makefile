#  run
# ./server 7838 1 127.0.0.1 cacert.pem privkey.pem
# ./client 127.0.0.1 7838
server:
	g++ server.cpp -o server -lssl -lcrypto
client:
	g++ client.cpp -o client -lssl -lcrypto
key:
	openssl genrsa -out privkey.pem 2048
	openssl req -new -x509 -key privkey.pem -out cacert.pem -days 1095