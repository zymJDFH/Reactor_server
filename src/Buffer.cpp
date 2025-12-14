#include "Buffer.h"
Buffer::Buffer(/* args */)
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char*data,size_t size){
    buf_.append(data,size);
}
void Buffer::appendwithhead(const char*data,size_t size){
    buf_.append((char*)&size,4);
    buf_.append(data,size);
    
}

size_t Buffer::size(){
    return buf_.size();
}
//返回buf_首地址
const char *Buffer::data(){
    return buf_.data();
}     
void Buffer::clear(){
    buf_.clear();
}
void Buffer::erase(size_t pos,int nn){
    buf_.erase(pos,nn);
}