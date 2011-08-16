#ifndef _Included_csl_DictSearch_DictSearch_cxx
#define _Included_csl_DictSearch_DictSearch_cxx _Included_csl_DictSearch_DictSearch_cxx

#include "./csl_DictSearch_DictSearch.h"
#include "./JNIUtils.h"
#include<csl/DictSearch/DictSearch.h>


/*
 * Class:     csl_DictSearch_DictSearch
 * Method:    native_init
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_csl_DictSearch_DictSearch_native_1init
(JNIEnv *env , jobject callerObj, jstring j_configFile ) {
    std::string configFile;
    csl::jstringToString( env, j_configFile, configFile );
    csl::DictSearch* ds = new csl::DictSearch();
    ds->readConfiguration( configFile );
    return (jlong)ds;
}


/*
 * Class:     csl_DictSearch_DictSearch
 * Method:    native_query
 * Signature: (Ljava/lang/String;Lcsl/DictSearch/DictSearch/ResultReceiver;)J
 */
JNIEXPORT void JNICALL Java_csl_DictSearch_DictSearch_native_1query
(JNIEnv * env, jobject callerObj, jstring query , jobject resultReceiver ) {
    
}



#endif
