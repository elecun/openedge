

#ifndef _OPENEDGE_DEVICE_SERIAL_HPP_
#define _OPENEDGE_DEVICE_SERIAL_HPP_
// https://github.com/karthickai/serial/blob/master/include/Serial.h
namespace oe::device {

    #if defined(_USE_BOOST_)

    class serial {

    }

    #endif

    class serial {


class Serial
	{

	public:
		typedef boost::asio::serial_port_base::flow_control::type flowControlType;
		typedef boost::asio::serial_port_base::parity::type parityType;
		typedef boost::asio::serial_port_base::stop_bits::type stopBitsType;
		typedef boost::system::errc::errc_t errorCode;

	public:
		Serial();
		~Serial();

		void open(std::string,
			unsigned int = 115200,
			flowControlType = flowControlType::none,
			unsigned int = 8,
			parityType = parityType::none,
			stopBitsType = stopBitsType::one);

		bool isOpen() const;
		void close();
		std::future<std::vector<uint8_t>> receiveAsync(size_t const num_bytes);
		std::future<std::vector<uint8_t>> receiveAsync(size_t const num_bytes, unsigned int timeout);

		void transmit(std::vector<uint8_t> const& data);
		std::size_t transmitAsync(const std::vector<uint8_t>& v);




	private:

		boost::asio::io_context io_context;
		boost::asio::serial_port serial_port;
		boost::asio::io_context::work serial_work;

		void asyncRead();
		void asyncReadHandler(boost::system::error_code const& error, size_t bytes_transferred);
		int16_t readByte();
		std::vector<uint8_t> readBuffer(size_t len);
		std::vector<uint8_t> readBufferTimeout(size_t len);


		std::array<uint8_t, 256> buf;


		std::vector<uint8_t> usableReadBuffer;
		std::size_t readBufferSize = 256;
		std::unique_ptr<std::thread> asyncReadThread;
		mutable std::mutex readBufferMtx; 


		mutable std::mutex errMtx;
		int error_value{};
		void setError(const int error_value);
		int getError() const;

		unsigned int timeoutVal = 60000;



		mutable std::mutex writeMtx;    
		std::condition_variable writeCv; 
		bool writeLocked = false;        
		void asyncWriteHandler(const boost::system::error_code& error, std::size_t bytes_transferred);


	};

    }; /* class */
} /* namespace  */

#endif