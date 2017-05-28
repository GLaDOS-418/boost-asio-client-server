#ifndef CLIENT_H_
#define CLIENT_H_

#include<string>
#include<iostream>
#include<boost/asio.hpp>

#include "Session.h"

using namespace boost::asio;

class Client {
	boost::asio::io_service m_ioService;
	std::shared_ptr<Session> m_connection;

	std::shared_ptr<Session> createConnection(const std::string& ip,
			const int port) {

		std::shared_ptr<Session> newConnection = Session::create(
				m_ioService);
		try {
			ip::tcp::endpoint endp(ip::address::from_string(ip), port);
			boost::asio::connect(newConnection->getSocket(), &endp);

		} catch (std::exception& e) {
			Logger::getLogger()->DebugLog("client connection not created: "+std::string( e.what()),__PRETTY_FUNCTION__);
		}

			return newConnection;
		}

	public:
		Client(const std::string& ip, const int port) {
			Logger::getLogger()->InfoLog("Client created", __PRETTY_FUNCTION__); //log
			m_connection = createConnection(ip, port);
		}

		virtual ~Client() {
		}

		void runService() {
			m_ioService.run();
		}

		std::shared_ptr < Session > getConnection()
	const {
		return m_connection;
	}
}
;

#endif /* CLIENT_H_ */
