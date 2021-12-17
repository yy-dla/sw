#ifndef __UTILS_H__
#define __UTILS_H__

#define __init_array(len, source, arr) \
    for(int i = 0; i < len; i++)    \
        arr[i] = source[i];

#define __clear_array(len, arr)\
    for(int i = 0; i < len; i++)\
        arr[i] = 0;

#define __addr_offset(len, addr)\
    addr += len;

#endif // !__UTILS_H__