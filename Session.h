#ifndef SESSION_H_
#define SESSION_H_

#include <iostream>
#include <string>
#include<memory>
#include<array>
#include <boost/asio.hpp>
#include "Logger.h"
using namespace boost::asio;


///////////////////      Connection Class      ////////////////////////

class Session: public std::enable_shared_from_this<Session> {

    ip::tcp::socket m_socket;
    std::array<char, 2056> m_acceptMessage;
    std::string m_acceptMessageWrapper;
    std::string m_buffer;
    bool m_isConnected;
public:
    Session(io_service& ioService) :
            m_socket(ioService), m_isConnected(true) {
    }

    virtual ~Session() {
    }

    static std::shared_ptr<Session> create(io_service& ioService) {
        return std::shared_ptr < Session > (new Session(ioService));
    }

    std::string& receiveMessage() {
        boost::system::error_code error;

        Logger::getLogger()->InfoLog("waiting to receive message", __PRETTY_FUNCTION__);
        try{
			size_t len = m_socket.read_some(boost::asio::buffer(m_acceptMessage), error);
			m_acceptMessageWrapper = std::string(m_acceptMessage.begin(),m_acceptMessage.begin() + len);


			Logger::getLogger()->InfoLog("message received", __PRETTY_FUNCTION__);


        }catch(std::exception& e){
			Logger::getLogger()->DebugLog("error in reading: "+ std::string(e.what()), __PRETTY_FUNCTION__);
			m_isConnected = false;
        }

        return m_acceptMessageWrapper;
    }

    void sendMessage(const std::string& message) {
    	try{
    		Logger::getLogger()->InfoLog("sending message",__PRETTY_FUNCTION__);
    		boost::asio::write(m_socket, boost::asio::buffer(message));
    		Logger::getLogger()->InfoLog("message sent", __PRETTY_FUNCTION__);
    	}catch(std::exception& e){
    		Logger::getLogger()->DebugLog("error in writing"+ std::string(e.what()), __PRETTY_FUNCTION__);
    		m_isConnected = false;
    	}
    }

    ip::tcp::socket& getSocket() {
        return m_socket;
    }

};


#endif /* SESSION_H_ */
