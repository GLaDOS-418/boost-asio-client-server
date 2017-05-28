//============================================================================
// Name        : ArnobServer_Sync.cpp
// Author      : Arnob
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include<queue>
#include<future>
#include<string>
#include "Server.h"
#include "Client.h"

using namespace std;

class Buffer {
	std::queue<std::shared_ptr<Session> > qConnections;
public:
	template<typename callbackT>
	void push(std::shared_ptr<Session> conn, callbackT callback) {
		std::cout << "connection pushed...\n";
		qConnections.push(conn);
		callback();
	}

	std::shared_ptr<Session> pop() {
		auto tempConn = qConnections.front();
		qConnections.pop();
		return tempConn;
	}
};

/////////////////////////////////////////////////////////////////

void func(std::shared_ptr<Session> connection) {
	std::cout << "server func started...\n";
	int counter = 0;
	while (true) {
		try {
			std::cout<<"[server]: fetching ip...."<<std::endl;
			ip::tcp::endpoint remote_ep =
					connection->getSocket().remote_endpoint();
			ip::address remote_ad = remote_ep.address();

			std::string temp = "[from server]: " + std::to_string(counter)
					+ "-->" + remote_ad.to_string(); //createMessage();
					//std::cout<<"message: "<<temp<<std::endl;
			counter++;
			connection->sendMessage(temp);
			std::cout << "message sent...\n";
			std::string ack = connection->receiveMessage();
			std::cout << ack << std::endl;
		} catch (std::exception& e) {
			std::cout << "[error occured in server side]: " << e.what()	<< std::endl;
			break;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));

	}
}

void clientFunc(std::shared_ptr<Session> connection) {

	std::cout << "client func started...\n";
	int counter = 0;
	while (true) {
		std::string ack = connection->receiveMessage();
		std::cout << ack << std::endl;

		try {
			ip::tcp::endpoint remote_ep =
					connection->getSocket().remote_endpoint();
			ip::address remote_ad = remote_ep.address();

			std::string temp = "[from client]: " + std::to_string(counter)
					+ "-->" + remote_ad.to_string();
			counter++;
			connection->sendMessage(temp);

		} catch (std::exception& e) {
			std::cout << "[error occured in client side]: " << e.what()
					<< std::endl;
			break;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

////////////////////////////////////////////////////////

void listener(Server& s) {

	Buffer b;

	std::cout << "listener..\n";
	std::cout<<"Listener Starts..."<<std::endl;
	while (true) {
		std::cout << "waiting for connection...." << std::endl;
		auto conn = s.createConnection();
		//push this connection to connection buffer
		if (conn)
			std::cout << "connection created...\n";
		b.push(conn, [&b]() {

			std::cout<<"callback...\n";

			auto con = b.pop();

			if(con)
			std::cout<<"connection extracted...\n";
			//std::thread t(func, con);
				std::thread(func,con).detach();
			//t.detach();
				std::cout<<"Waiting ...."<<std::endl;
			});
	}

}

int main(int argc, char* argv[]) {

	if (argc < 2)
		return 1;

	int port = std::stoi(argv[2]);

	if (argv[1][0] == 's') {
		try {
			Server s(port);
			std::cout << "server created...\n";
			std::thread lst(listener, std::ref(s));
			lst.join();
			s.runService();
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	} else if (argv[1][0] == 'c') {

		int port1 = std::stoi(argv[2]);
		Client c1("127.0.0.1", port1);
		auto conn1 = c1.getConnection();

		std::shared_ptr<Session> conn2;
		if (argc == 4) {
			int port2 = std::stoi(argv[3]);
			Client c2("127.0.0.1", port2);
			conn2 = c2.getConnection();
		}

		/*
		 std::thread cnt(connector);
		 cnt.join();
		 c.runService();
		 */

		/*if (conn) {
		 std::cout << "client connection created...\n";
		 }*/

		std::thread tc(clientFunc, conn1);
		if (argc == 4) {
			std::thread tc2(clientFunc, conn2);
			tc2.join();
		}
		tc.join();

	}
	return 0;
}
