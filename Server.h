#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <string>
#include <memory>
#include <boost/asio.hpp>

#include "Session.h"

using namespace boost::asio;

class Server {

	io_service *m_ioService;
	ip::tcp::endpoint m_endpoint;
	ip::tcp::acceptor m_acceptor;
public:
	Server(int port) :
			m_ioService(new io_service()), m_endpoint(
					ip::tcp::endpoint(ip::tcp::v4(), port)), m_acceptor(
					ip::tcp::acceptor(*m_ioService, m_endpoint)) {
	}

	virtual ~Server() {
	}

	std::shared_ptr<Session> createConnection() {
		std::shared_ptr<Session> newConnection(Session::create(*m_ioService));
		try {
			m_acceptor.accept(newConnection->getSocket());
		} catch (std::exception& e) {
			Logger::getLogger()->DebugLog("server connection not created"+std::string(e.what()), __PRETTY_FUNCTION__);
		}

		return newConnection;
	}

	void runService() {
		m_ioService->run();
	}

};

#endif /* SERVER_H_ */
