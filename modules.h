/*
 * DMO Language Module System Header
 * Handles module loading and imports
 */

#ifndef MODULES_H
#define MODULES_H

#include "interpreter.h"

// Module structure
typedef struct Module {
    char* name;
    char* path;
    bool loaded;
    struct Module* next;
} Module;

// Module system context
typedef struct {
    Module* loaded_modules;
    char** search_paths;
    int path_count;
} ModuleSystem;

// Function prototypes
void init_module_system();
void cleanup_module_system();
bool load_module(const char* module_name, InterpreterContext* ctx);
char* find_module_file(const char* module_name);
void add_search_path(const char* path);
bool is_module_loaded(const char* module_name);
void mark_module_loaded(const char* module_name, const char* path);

// Built-in module loaders
void load_stdlib_module(InterpreterContext* ctx);
void load_dmo_graphs_module(InterpreterContext* ctx);

#endif // MODULES_H
