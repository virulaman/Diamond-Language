/*
 * DMO Graphics Library Header
 * Built-in graphics functions for DMO language
 */

#ifndef DMO_GRAPHS_H
#define DMO_GRAPHS_H

#include <stdio.h>
#include <stdbool.h>
#include "interpreter.h"

// Graphics context structure
typedef struct {
    int window_width;
    int window_height;
    char* window_title;
    bool window_created;
    FILE* svg_output;
    char svg_filename[256];
} DMOGraphicsContext;

// Function prototypes
void init_dmo_graphics();
void cleanup_dmo_graphics();
Value call_dmo_graphics_function(const char* name, ASTNode** args, int arg_count, InterpreterContext* ctx);

// Graphics function implementations
Value dmo_gr_create_window(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value dmo_gr_create_line(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value dmo_gr_create_sqr(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value dmo_gr_create_crle(ASTNode** args, int arg_count, InterpreterContext* ctx);

// Utility functions
void start_svg_output();
void end_svg_output();
void write_svg_header();
void write_svg_footer();

#endif // DMO_GRAPHS_H
