/*
 * Diamond Graphics Library Header
 * Built-in graphics functions for Diamond language
 */

#ifndef DMO_GRAPHS_H
#define DMO_GRAPHS_H

#include <stdio.h>
#include <stdbool.h>
#include "interpreter.h"

// Graphics structures
typedef struct {
    int x, y, width, height;
} Hitbox;

typedef struct {
    int r, g, b;
} Color;

typedef struct {
    char* id;
    int x, y, width, height;
    Color color;
    Hitbox hitbox;
    int type; // 0=square, 1=circle, 2=line, 3=text
} GraphicsElement;

typedef struct {
    bool keys[256]; // Keyboard state
    bool mouse_pressed;
    int mouse_x, mouse_y;
} InputState;

typedef struct {
    int window_width;
    int window_height;
    char* window_title;
    bool window_created;
    FILE* svg_output;
    char svg_filename[256];
    GraphicsElement* elements;
    int element_count;
    int element_capacity;
    InputState input;
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
Value dmo_gr_display(ASTNode** args, int arg_count, InterpreterContext* ctx);

// Input and collision detection
Value dmo_key_pressed(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value dmo_element_pressed(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value dmo_collide(ASTNode** args, int arg_count, InterpreterContext* ctx);

// Utility functions for graphics
void add_graphics_element(const char* id, int x, int y, int width, int height, Color color, int type);
GraphicsElement* find_element_by_id(const char* id);
bool check_collision(GraphicsElement* a, GraphicsElement* b);
Color parse_color(int r, int g, int b);

// Utility functions
void start_svg_output();
void end_svg_output();
void write_svg_header();
void write_svg_footer();

#endif // DMO_GRAPHS_H
