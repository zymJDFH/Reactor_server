#include "Buffer.h"

Buffer::Buffer(uint16_t sep):sep_(sep)
{

}

Buffer::~Buffer()
{

}

// 把数据追加到buf_中。
void Buffer::append(const char *data,size_t size)             
{
    buf_.append(data,size);
}

 // 把数据追加到buf_中，附加报文分隔符。
 void Buffer::appendwithsep(const char *data,size_t size)  
 {
    if (sep_==0)             // 没有分隔符。
    {
        buf_.append(data,size);                    // 处理报文内容。
    }
    else if (sep_==1)     // 四字节的报头。
    {
        buf_.append((char*)&size,4);           // 处理报文长度（头部）。
        buf_.append(data,size);                    // 处理报文内容。
    }
    // 其它的代码请各位自己完善。
 }

// 从buf_的pos开始，删除nn个字节，pos从0开始。
void Buffer::erase(size_t pos,size_t nn)                             
{
    buf_.erase(pos,nn);
}

// 返回buf_的大小。
size_t Buffer::size()                                                            
{
    return buf_.size();
}

// 返回buf_的首地址。
const char *Buffer::data()                                                  
{
    return buf_.data();
}

// 清空buf_。
void Buffer::clear()                                                            
{
    buf_.clear();
}

// 从buf_中拆分出一个报文，存放在ss中，如果buf_中没有报文，返回false。
bool Buffer::pickmessage(std::string &ss)                           
{
    if (buf_.size()==0) return false;

    if (sep_==0)                  // 没有分隔符。
    {
        ss=buf_;
        buf_.clear();
    }
    else if (sep_==1)          // 四字节的报头。
    {
        if (buf_.size()<4) return false;

        int len;
        memcpy(&len,buf_.data(),4);             // 从buf_中获取报文头部。

        if (buf_.size()<len+4) return false;     // 如果buf_中的数据量小于报文头部，说明buf_中的报文内容不完整。

        ss=buf_.substr(4,len);                        // 从buf_中获取一个报文。
        buf_.erase(0,len+4);                          // 从buf_中删除刚才已获取的报文。
    }
    else if (sep_==2)          // HTTP 头部以 "\r\n\r\n" 作为分隔。
    {
        const std::string delimiter="\r\n\r\n";
        size_t pos=buf_.find(delimiter);
        if (pos==std::string::npos) return false;

        ss=buf_.substr(0,pos);
        buf_.erase(0,pos+delimiter.size());
    }

    return true;
}

/*
int main()
{
    std::string s1="aaaaaaaaaaaaab";
    Buffer buf(1);
    buf.appendwithsep(s1.data(),s1.size());
    std::string s2;
    buf.pickmessage(s2);
    printf("s2=%s\n",s2.c_str());
}
// g++ -g -o test Buffer.cpp
*/
