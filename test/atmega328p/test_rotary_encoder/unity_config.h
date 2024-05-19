#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

    void unity_output_start();
    void unity_output_char(char);
    void unity_output_flush();
    void unity_output_complete();

#define UNITY_OUTPUT_START() unity_output_start()
#define UNITY_OUTPUT_CHAR(c) unity_output_char(c)
#define UNITY_OUTPUT_FLUSH() unity_output_flush()
#define UNITY_OUTPUT_COMPLETE() unity_output_complete()

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* UNITY_CONFIG_H */