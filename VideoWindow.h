#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include<memory>
#include<opencv2/opencv.hpp>

class VideoWindow
{
private:
	int type; // i.e. CV_U8C3
	size_t elemsize;
	size_t rows;
	size_t cols;
	size_t num_frames;
	size_t current_num_frames;
	size_t framesize;
	size_t tail;

	std::unique_ptr<uint8_t[]> data;
	std::unique_ptr<cv::Mat[]> framerefs;
public:
	//gets a frame from the video volume.  frame 0 is the least recently pushed frame (the head of the queue).  Frame -1 is the most recently pushed frame)
	/*
	[5 6 7 8 9] // Window
	internal:
	[1 2 3 4 5 6 7 8 9] // read 9 frames...
	[8 9 5 6 7]
	   t = 2, the start of the window
	   current_num_frames = 5
	   get_frame(0) = 5
	   get_frame(1) = 6
	   get_frame(2) = 7
	   ...
	*/	
	cv::Mat& get_frame(int id=0)
	{
		id %= (int)current_num_frames;
		size_t findex=(tail+current_num_frames+id) % current_num_frames;
		return framerefs[findex];
	}

	cv::Mat& operator[](int id)
	{
		return get_frame(id);
	}
	template<class T> 
	T& at(size_t r,size_t c,size_t f)
	{
		return get_frame(f).at<T>(r,c);
	}
	template<class T>
	const T& at(size_t r,size_t c,size_t f) const
	{
		return get_frame(f).at<T>(r,c);
	}

	//pushes a new frame into the video volume queue and returns it (filled with garbage data)
	//you can write to it after returns and it will write to the video volume
	// i.e. VideoCapture instance.read(VideoWindow instance.push())
	cv::Mat& push()
	{
		// If window isn't full yet:
		if(current_num_frames < num_frames)
		{
			++current_num_frames;
		}
		tail %=current_num_frames;
		cv::Mat& m=framerefs[tail++];
		return m;
	}
	
	VideoWindow(size_t r,size_t c,size_t nf,int t):
		type(t),
		elemsize(0),
		rows(r),
		cols(c),
		num_frames(nf),
		current_num_frames(0),
		tail(0)
	{
		cv::Mat m(1, 1, type);
		elemsize=m.elemSize();
		framesize=rows*cols*elemsize;
		size_t datasize=num_frames*framesize;
		data.reset(new uint8_t[datasize]);
		framerefs.reset(new cv::Mat[num_frames]);
		for(size_t findex=0;findex < num_frames;findex++)
		{
			uint8_t* dataptr=&data[0]+findex*framesize;
			framerefs[findex]=cv::Mat(r,c,type,reinterpret_cast<void*>(dataptr));
		}
	}
};

#endif
