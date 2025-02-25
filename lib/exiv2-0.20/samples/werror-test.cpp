// ***************************************************************** -*- C++ -*-
// werror-test.cpp, $Rev: 2016 $
// Simple tests for the wide-string error class WError

#include <iostream>
#include <exiv2/error.hpp>

int main()
{
    try {
        throw Exiv2::Error(-1, "ARG1", "ARG2", "ARG3");
    }
    catch (const Exiv2::Error& e) {
        std::cout << "Caught Error '" << e.what() << "'\n";
    }

#ifdef EXV_UNICODE_PATH
    try {
        throw Exiv2::WError(-1, L"WARG1", L"WARG2", L"WARG3");
    }
    catch (const Exiv2::WError& e) {
        std::wstring wmsg = e.wwhat();
        std::string msg(wmsg.begin(), wmsg.end());
        std::cout << "Caught WError '" << msg << "'\n";
    }
#endif

    return 0;
}
