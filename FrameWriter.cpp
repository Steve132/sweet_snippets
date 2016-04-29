class KinectFrameWriter
{
public:
	enum DataType
	{
		DEPTH,
		COLOR
	};
	std::future<std::vector<uint8_t>> compressed_stream;
	std::future<void> writebacktofile;
	DataType dtype;
	std::vector<uint8_t> uncompressed_stream;
	uint64_t timestamp;
	std::mutex& iomutex;
	KinectFrameWriter(DataType dtype,const void* data,size_t length, uint64_t timestamp, std::mutex& m):
		dtype(dtype),
		uncompressed_stream(reinterpret_cast<const uint8_t*>(data),reinterpret_cast<const uint8_t*>(data)+length),
		timestamp(timestamp),
		iomutex(m)
	{
		compressed_stream=std::async(std::launch::async,KinectFrameWriter::compress,this);
		writebacktofile=std::async(std::launch::async,KinectFrameWriter::writeback,this);
	}
};

class KinectFrameWriterPool
{
public:
	std::mutex poolmutex;
	std::forward_list<std::shared_ptr<KinectFrameWriter> > pool;
	std::atomic<bool> keeprunning;
	
	void run()
	{
		while(keeprunning)
		{
			std::lock_guard lg(poolmutex);
			pool.remove_if([](std::shared_ptr<KinectFrameWriter> ptr){ ptr->writebacktofile.valid(); });
			std::this_thread::yield();
		}
	}
};
