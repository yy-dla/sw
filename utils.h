#ifndef __UTILS_H__
#define __UTILS_H__

#define __init_array(len, source, arr) \
    for(int i = 0; i < len; i++)    \
        arr[i] = source[i];

#define __clear_array(len, arr)\
    for(int i = 0; i < len; i++)\
        arr[i] = 0;

#endif // !__UTILS_H__