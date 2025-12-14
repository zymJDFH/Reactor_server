#pragma once
#include <string>
class Buffer
{
private:
    std::string buf_;
public:
    Buffer();
    ~Buffer();
    void append(const char*data,size_t size);
    void appendwithhead(const char*data,size_t size);
    size_t size();
    const char *data();     //返回buf_首地址
    void clear();
    void erase(size_t pos,int nn);//删除nn个字节
};

