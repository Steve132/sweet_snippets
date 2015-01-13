#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include<memory>
#include<opencv2/opencv.hpp>

class VideoWindow
{
private:
	int type;
	size_t elemsize;
	size_t rows;
	size_t cols;
	size_t num_frames;
	size_t current_num_frames;
	size_t framesize;
	size_t tail;

	std::unique<uint8_t[]> data;
	std::unique<cv::Mat[]> framerefs;
public:
	//gets a frame from the video volume.  frame 0 is the least recently pushed frame (the head of the queue).  Frame -1 is the most recently pushed frame)
	cv::Mat& get_frame(int id=0)
	{
		id %= current_num_frames;
		size_t findex=(tail+current_num_frames+id) % current_num_frames;
		return framerefs[findex];
	}
	//pushes a new frame into the video volume queue and returns it (filled with garbage data)
	//you can write to it after returns and it will write to the video volume
	cv::Mat& push()
	{
		if(current_num_frames < num_frames)
		{
			++current_num_frames;
		}
		++tail;
		tail%=current_num_frames;
		return get_frame(-1);
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
		cv::Mat m(type,1,1);
		elemsize=m.elemsize();
		framesize=rows*cols*elemsize;
		size_t datasize=num_frames*framesize;
		data.reset(new uint8_t[datasize]);
		framerefs.reset(new cv::Mat[num_frames]);
		for(size_t findex=0;findex < num_frames;findex++)
		{
			uint8_t* dataptr=&data[0]+findex*framesize;
			framerefs[findex]=cv::Mat(type,r,c,dataptr);
		}
	}
};

#endif
