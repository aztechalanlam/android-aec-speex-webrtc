#include <jni.h>
#include <stdlib.h>
#include <speex/speex_echo.h>

#include <jni.h>
#include <stdlib.h>
#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h> // Include if you are also using the preprocessor module

// This structure matches what the panzy repository typically tracks internally


SpeexEchoState *st = NULL;
SpeexPreprocessState *den = NULL;

JNIEXPORT void JNICALL
Java_speex_1aec_SpeexAecJNI_open(JNIEnv *jenv, jclass jcls, jint sampleRate, jint bufsize, jint totalSize) {
st = speex_echo_state_init(bufsize, totalSize);
den = speex_preprocess_state_init(bufsize, sampleRate);
speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);
int value = 1;
//speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_AGC, &value);
//speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_VAD, &value);
speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_DENOISE, &value);
    return ;
}


JNIEXPORT void JNICALL Java_speex_1aec_SpeexAecJNI_close(JNIEnv *jenv, jclass jcls) {
if (st) {
speex_echo_state_destroy(st);
st = NULL;
}
if (den) {
speex_preprocess_state_destroy(den);
den = NULL;
}
}

JNIEXPORT void JNICALL
Java_speex_1aec_SpeexAecJNI_process(JNIEnv *env, jclass jcls, jshortArray rec_frame, jshortArray play_frame, jshortArray out_frame)      // Cancelled audio block output
{
SpeexEchoState *echo_state = st;
    if (!echo_state) return;
jshort *rec = (*env)->GetShortArrayElements(env, rec_frame, NULL);
jshort *play = (*env)->GetShortArrayElements(env, play_frame, NULL);
jshort *out = (*env)->GetShortArrayElements(env, out_frame, NULL);

// Speex Echo Cancellation executes inline on spx_int16_t arrays
speex_echo_cancellation(echo_state, (spx_int16_t*)rec, (spx_int16_t*)play, (spx_int16_t*)out);

// Release and commit changes back to the JVM managed heap
(*env)->ReleaseShortArrayElements(env, rec_frame, rec, JNI_ABORT); // Input only, discard changes
(*env)->ReleaseShortArrayElements(env, play_frame, play, JNI_ABORT); // Input only, discard changes
(*env)->ReleaseShortArrayElements(env, out_frame, out, 0); // Output array, commit changes
}



// IMPORTANT: Replace "com_example_myapp" with your actual Java/Kotlin package name.
// Replace "SpeexAEC" with your actual Kotlin/Java object/class name.
#define JNI_METHOD(return_type, method_name) \
    JNIEXPORT return_type JNICALL Java_speex_1aec_SpeexAecJNI_##method_name

/**
 * Initializes the Speex Echo Cancellation module.
 * Returns a native memory address pointer cast to a jlong.
 */
JNI_METHOD(jlong, init)(JNIEnv *env, jobject thiz, jint frame_size, jint filter_length) {
    SpeexEchoState *echo_state = speex_echo_state_init(frame_size, filter_length);
    return (jlong)(intptr_t)echo_state;
}

/**
 * Executes the Acoustic Echo Cancellation filter loop.
 */
JNI_METHOD(jint, cancellation)(JNIEnv *env, jobject thiz, 
                               jlong echo_state_ptr, 
                               jshortArray rec_frame,   // Microphone Frame (input)
                               jshortArray play_frame,  // Speaker Reference Frame (input)
                               jshortArray out_frame)   // Clean Audio output frame
{
    SpeexEchoState *echo_state = (SpeexEchoState *)(intptr_t)echo_state_ptr;
    if (!echo_state) return -1;

    // Get direct pointers to the Java short arrays
    jshort *rec = (*env)->GetShortArrayElements(env, rec_frame, NULL);
    jshort *play = (*env)->GetShortArrayElements(env, play_frame, NULL);
    jshort *out = (*env)->GetShortArrayElements(env, out_frame, NULL);

    // Speex Echo Cancellation executes inline on spx_int16_t arrays
    speex_echo_cancellation(echo_state, (spx_int16_t*)rec, (spx_int16_t*)play, (spx_int16_t*)out);

    // Release and commit changes back to the JVM managed heap
    (*env)->ReleaseShortArrayElements(env, rec_frame, rec, JNI_ABORT); // Input only, discard changes
    (*env)->ReleaseShortArrayElements(env, play_frame, play, JNI_ABORT); // Input only, discard changes
    (*env)->ReleaseShortArrayElements(env, out_frame, out, 0); // Output array, commit changes

    return 0;
}

/**
 * Destroys and cleans up native memory heap allocations.
 */
JNI_METHOD(void, destroy)(JNIEnv *env, jobject thiz, jlong echo_state_ptr) {
    SpeexEchoState *echo_state = (SpeexEchoState *)(intptr_t)echo_state_ptr;
    if (echo_state) {
        speex_echo_state_destroy(echo_state);
    }
}
