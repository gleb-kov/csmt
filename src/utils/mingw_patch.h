#ifndef CSMT_MINGW_PATCH_H
#define CSMT_MINGW_PATCH_H

#include <string>
#include <sstream>

#ifdef __MINGW32__
namespace std {

    template<typename T>
    string to_string(T &&value) {
        ostringstream s;
        s << value;
        return s.str();
    }

}
#endif

#endif // CSMT_MINGW_PATCH_H
