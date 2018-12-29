#include "stick_figure_detect.h"
#include <iostream>
#include <fstream>
using namespace std;
StickCompress::StickCompress(){
    reset();
}
StickCompress::StickCompress(std::string file_name){
    reset();
}

void StickCompress::reset(){
    offset = {0,0};
    stick_point.clear();  
    box_ = {9999, 9999, 0, 0}; //letf, top, right, bottom
}
void StickCompress::push_stick(int x, int y){
    stick_point.push_back({x, y});
    box_[0] > x ? box_[0] = x:box_[0];
    box_[2] < x ? box_[2] = x:box_[0];
    box_[1] > y ? box_[1] = y:box_[0];
    box_[3] < y ? box_[3] = y:box_[0];
}
void StickCompress::scale_to_size(int  width ,int height){

}
std::string StickCompress::to_buf(){
    std::string buf;
    return buf;
}
void StickCompress::save(std::string file_name){
    std::ofstream ofs(file_name);
    auto buf = to_buf();
    ofs.write(buf.data(), buf.size());
}

class StickFigure::StickFigureImpl{
public:

    cv::Mat src;
    cv::Mat hist[3]; 
    cv::Rect stick_box;

    StickFigureImpl(){}
    ~StickFigureImpl(){}

    bool is_stick_figure(cv::Mat src_in, int threshold){
        src = src_in;
        hist[0] = cv::Mat::zeros(1, 256, CV_32SC1);
        hist[1] = cv::Mat::zeros(1, 256, CV_32SC1);
        hist[2] = cv::Mat::zeros(1, 256, CV_32SC1);
        if(src.type() == CV_8UC1){
            int black_count = 0;
    
            for(int r = 0; r < src.rows;r++){
                for(int c = 0; c < src.cols;c++){
                    if(src.at<uchar>(r,c) < 128){
                        black_count ++;
                    }
               }
            }
            return black_count < src.rows* src.cols /4;
        }
        else if( src.type() == CV_8UC3 || src.type() == CV_8UC4){
            
            int black_cout = 0;
            int white_cout = 0;
            
            for(int r = 0; r < src.rows;r++){
                for(int c = 0; c < src.cols;c++){
                    uchar* ptr =  src.ptr(r,c); 
                    hist[0].at<int>(ptr[0])++;
                    hist[1].at<int>(ptr[1])++;
                    hist[2].at<int>(ptr[2])++;
                    if(ptr[0] < threshold && ptr[1] < threshold  && ptr[2] < threshold){
                        black_cout ++;
                    }
                    else if(ptr[0] > 255 -threshold && ptr[0] > 255 -threshold  && ptr[1] > 255 -threshold){
                        white_cout ++;
                    }
                }
            }
            if((black_cout + white_cout)/(float)(src.rows*src.cols) > 0.9 && black_cout /(float)(src.rows*src.cols) < 0.25){
                return true;
            }
        }
    
        return false;

    }

};

StickFigure::StickFigure():impl_(std::make_unique<StickFigureImpl>()){

}
bool StickFigure::is_stick_figure(cv::Mat src, int threshold){
    return impl_->is_stick_figure(src, threshold);
}

cv::Mat StickFigure::to_binary_stick(int threshold){
    auto src = impl_->src;
    cv::Mat res = cv::Mat::zeros(src.size(),CV_8UC1);
        for(int r = 0; r < src.rows;r++){
            for(int c = 0; c < src.cols;c++){
                uchar* ptr =  src.ptr(r,c); 
                if(ptr[0] < threshold && ptr[1] < threshold && ptr[2] < threshold
                   && abs(ptr[0] - ptr[1]) + abs(ptr[1] - ptr[2]) < threshold){
                    res.at<uchar>(r,c) = 255;
               }
            }
        }
 
    return res;
}
