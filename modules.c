/*
 * DMO Language Module System Implementation
 * Handles module loading and imports
 */

#define _POSIX_C_SOURCE 200809L
#include "modules.h"
#include "stdlib_funcs.h"
#include "dmo_graphs.h"
#include "math_module.h"
#include "request_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ModuleSystem* module_system = NULL;

void init_module_system() {
    if (module_system) {
        return; // Already initialized
    }
    
    module_system = malloc(sizeof(ModuleSystem));
    module_system->loaded_modules = NULL;
    module_system->search_paths = malloc(sizeof(char*) * 10);
    module_system->path_count = 0;
    
    // Add default search paths
    add_search_path(".");
    add_search_path("./modules");
    add_search_path("/usr/local/lib/dmo");
}

void cleanup_module_system() {
    if (!module_system) {
        return;
    }
    
    // Free loaded modules
    Module* module = module_system->loaded_modules;
    while (module) {
        Module* next = module->next;
        free(module->name);
        free(module->path);
        free(module);
        module = next;
    }
    
    // Free search paths
    for (int i = 0; i < module_system->path_count; i++) {
        free(module_system->search_paths[i]);
    }
    free(module_system->search_paths);
    
    free(module_system);
    module_system = NULL;
}

void add_search_path(const char* path) {
    if (!module_system || module_system->path_count >= 10) {
        return;
    }
    
    module_system->search_paths[module_system->path_count++] = strdup(path);
}

bool is_module_loaded(const char* module_name) {
    if (!module_system) {
        return false;
    }
    
    Module* module = module_system->loaded_modules;
    while (module) {
        if (strcmp(module->name, module_name) == 0) {
            return module->loaded;
        }
        module = module->next;
    }
    
    return false;
}

void mark_module_loaded(const char* module_name, const char* path) {
    if (!module_system) {
        return;
    }
    
    Module* module = malloc(sizeof(Module));
    module->name = strdup(module_name);
    module->path = strdup(path);
    module->loaded = true;
    module->next = module_system->loaded_modules;
    module_system->loaded_modules = module;
}

char* find_module_file(const char* module_name) {
    if (!module_system) {
        return NULL;
    }
    
    for (int i = 0; i < module_system->path_count; i++) {
        char* full_path = malloc(strlen(module_system->search_paths[i]) + strlen(module_name) + 10);
        sprintf(full_path, "%s/%s.dmo", module_system->search_paths[i], module_name);

        FILE* file = fopen(full_path, "r");
        if (file) {
            fclose(file);
            return full_path;
        }

        free(full_path);
    }
    
    return NULL;
}

bool load_module(const char* module_name, InterpreterContext* ctx) {
    if (is_module_loaded(module_name)) {
        return true; // Already loaded
    }
    
    printf("Loading module: %s\n", module_name);
    
    // Check for built-in modules
    if (strcmp(module_name, "stdlib") == 0) {
        load_stdlib_module(ctx);
        mark_module_loaded(module_name, "built-in");
        return true;
    }
    
    if (strcmp(module_name, "dmo_graphs") == 0 || strcmp(module_name, "dmo.graphs") == 0) {
        load_dmo_graphs_module(ctx);
        mark_module_loaded(module_name, "built-in");
        return true;
    }

    if (strcmp(module_name, "math") == 0) {
        init_math_module(ctx);
        mark_module_loaded(module_name, "built-in");
        return true;
    }

    if (strcmp(module_name, "request") == 0) {
        init_request_module(ctx);
        mark_module_loaded(module_name, "built-in");
        return true;
    }
    
    // Try to find and load external module file
    char* module_path = find_module_file(module_name);
    if (!module_path) {
        fprintf(stderr, "Error: Module '%s' not found in search paths\n", module_name);
        return false;
    }
    
    // For now, just mark as loaded - full implementation would parse and execute the module
    mark_module_loaded(module_name, module_path);
    free(module_path);

    printf("Module '%s' loaded successfully\n", module_name);
    return true;
}

void load_stdlib_module(InterpreterContext* ctx) {
    // Standard library functions are loaded by default
    // This function exists for consistency
    init_stdlib_functions(ctx);
}

void load_dmo_graphs_module(InterpreterContext* ctx) {
    // DMO graphics functions are loaded by default
    // This function exists for consistency
    init_dmo_graphics();
}
