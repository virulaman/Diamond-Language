#include "request_module.h"
#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Struct to hold memory for curl response
struct MemoryStruct {
    char* memory;
    size_t size;
};

// Callback function to write curl response to memory
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Error: not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// --- Request Function Implementations ---

Value request_get(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: request.get() requires exactly one argument (URL)\n");
        return create_string_value("");
    }

    Value url_val = execute_node(args[0], ctx);
    if (url_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: request.get() argument must be a string\n");
        free_value(url_val);
        return create_string_value("");
    }

    CURL* curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url_val.string);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "dmo-lang/1.0");

    res = curl_easy_perform(curl_handle);
    Value result;

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        result = create_string_value("");
    } else {
        result = create_string_value(chunk.memory);
    }

    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
    curl_global_cleanup();
    free_value(url_val);

    return result;
}

Value request_post(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 2) {
        fprintf(stderr, "Error: request.post() requires exactly two arguments (URL, data)\n");
        return create_string_value("");
    }

    Value url_val = execute_node(args[0], ctx);
    Value data_val = execute_node(args[1], ctx);

    if (url_val.type != VALUE_STRING || data_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: request.post() arguments must be strings\n");
        free_value(url_val);
        free_value(data_val);
        return create_string_value("");
    }

    CURL* curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url_val.string);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data_val.string);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "dmo-lang/1.0");

    res = curl_easy_perform(curl_handle);
    Value result;

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        result = create_string_value("");
    } else {
        result = create_string_value(chunk.memory);
    }

    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
    curl_global_cleanup();
    free_value(url_val);
    free_value(data_val);

    return result;
}

// --- Module Initialization ---

void init_request_module(InterpreterContext* ctx) {
    register_native_function(ctx, "request.get", request_get);
    register_native_function(ctx, "request.post", request_post);
}