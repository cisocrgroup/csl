#include<string>
#include<csl/CSLLocale/CSLLocale.h>

namespace csl {
    // get std::string from java string
    inline void jstringToString( JNIEnv*& env, jstring& jstr, std::string& str  ) {
	
	char const* cstr = env->GetStringUTFChars( jstr, 0 );
	str = cstr;
	
	env->ReleaseStringUTFChars( jstr, cstr );
    }

    inline void jstringToWstring( JNIEnv*& env, jstring& jstr, std::wstring& wstr  ) {
	std::string str;
	jstringToString( env, jstr, str);
	CSLLocale::string2wstring( str, wstr );
    }
}
