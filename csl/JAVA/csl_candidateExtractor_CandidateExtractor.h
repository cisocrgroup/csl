/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class csl_candidateExtractor_CandidateExtractor */

#ifndef _Included_csl_candidateExtractor_CandidateExtractor
#define _Included_csl_candidateExtractor_CandidateExtractor
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     csl_candidateExtractor_CandidateExtractor
 * Method:    initNativeCandExtractor
 * Signature: (ILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_csl_candidateExtractor_CandidateExtractor_initNativeCandExtractor
  (JNIEnv *, jobject, jint, jstring, jstring);

/*
 * Class:     csl_candidateExtractor_CandidateExtractor
 * Method:    destroyNativeCandExtractor
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_csl_candidateExtractor_CandidateExtractor_destroyNativeCandExtractor
  (JNIEnv *, jobject);

/*
 * Class:     csl_candidateExtractor_CandidateExtractor
 * Method:    nativeQuery
 * Signature: (Ljava/lang/String;ILcsl/candidateExtractor/CandidateReceiver;)V
 */
JNIEXPORT void JNICALL Java_csl_candidateExtractor_CandidateExtractor_nativeQuery
  (JNIEnv *, jobject, jstring, jint, jobject);

/*
 * Class:     csl_candidateExtractor_CandidateExtractor
 * Method:    nativeLookup
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_csl_candidateExtractor_CandidateExtractor_nativeLookup
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif