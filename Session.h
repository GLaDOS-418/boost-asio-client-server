#ifndef SESSION_H_
#define SESSION_H_

#include <iostream>
#include <string>
#include<memory>
#include<array>
#include <boost/asio.hpp>
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

        std::cout<<"waiting to receive message"<< __PRETTY_FUNCTION__;
        try{
			size_t len = m_socket.read_some(boost::asio::buffer(m_acceptMessage), error);
			m_acceptMessageWrapper = std::string(m_acceptMessage.begin(),m_acceptMessage.begin() + len);


			std::cout<<"message received"<< __PRETTY_FUNCTION__;


        }catch(std::exception& e){
			std::cout<<"error in reading: "<< e.what()<< __PRETTY_FUNCTION__;
			m_isConnected = false;
        }

        return m_acceptMessageWrapper;
    }

    void sendMessage(const std::string& message) {
    	try{
    		std::cout<<"sending message"<<__PRETTY_FUNCTION__;
    		boost::asio::write(m_socket, boost::asio::buffer(message));
    		std::cout<<"message sent"<< __PRETTY_FUNCTION__;
    	}catch(std::exception& e){
    		std::cout<<"error in writing"<<e.what()<< __PRETTY_FUNCTION__;
    		m_isConnected = false;
    	}
    }

    ip::tcp::socket& getSocket() {
        return m_socket;
    }

};


#endif /* SESSION_H_ */
