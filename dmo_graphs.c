/*
 * DMO Graphics Library Implementation
 * Built-in graphics functions for DMO language
 */

#define _POSIX_C_SOURCE 200809L
#include "dmo_graphs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static DMOGraphicsContext* graphics_ctx = NULL;

void init_dmo_graphics() {
    if (graphics_ctx) {
        return; // Already initialized
    }
    
    graphics_ctx = malloc(sizeof(DMOGraphicsContext));
    graphics_ctx->window_width = 800;
    graphics_ctx->window_height = 600;
    graphics_ctx->window_title = strdup("DMO Graphics Window");
    graphics_ctx->window_created = false;
    graphics_ctx->svg_output = NULL;
    strcpy(graphics_ctx->svg_filename, "output.svg");
    
    // Initialize elements array
    graphics_ctx->element_capacity = 100;
    graphics_ctx->element_count = 0;
    graphics_ctx->elements = malloc(sizeof(GraphicsElement) * graphics_ctx->element_capacity);
    
    // Initialize input state
    for (int i = 0; i < 256; i++) {
        graphics_ctx->input.keys[i] = false;
    }
    graphics_ctx->input.mouse_pressed = false;
    graphics_ctx->input.mouse_x = 0;
    graphics_ctx->input.mouse_y = 0;
    
    printf("Diamond Graphics Library initialized\n");
}

void cleanup_dmo_graphics() {
    if (!graphics_ctx) {
        return;
    }
    
    if (graphics_ctx->svg_output) {
        end_svg_output();
    }
    
    // Free elements array
    for (int i = 0; i < graphics_ctx->element_count; i++) {
        if (graphics_ctx->elements[i].id) {
            free(graphics_ctx->elements[i].id);
        }
    }
    free(graphics_ctx->elements);
    
    free(graphics_ctx->window_title);
    free(graphics_ctx);
    graphics_ctx = NULL;
    
    printf("Diamond Graphics Library cleaned up\n");
}

void start_svg_output() {
    if (!graphics_ctx || graphics_ctx->svg_output) {
        return;
    }
    
    graphics_ctx->svg_output = fopen(graphics_ctx->svg_filename, "w");
    if (graphics_ctx->svg_output) {
        write_svg_header();
        printf("SVG output started: %s\n", graphics_ctx->svg_filename);
    }
}

void end_svg_output() {
    if (!graphics_ctx || !graphics_ctx->svg_output) {
        return;
    }
    
    write_svg_footer();
    fclose(graphics_ctx->svg_output);
    graphics_ctx->svg_output = NULL;
    
    printf("SVG output saved: %s\n", graphics_ctx->svg_filename);
}

void write_svg_header() {
    if (!graphics_ctx->svg_output) {
        return;
    }
    
    fprintf(graphics_ctx->svg_output,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n"
        "  <title>%s</title>\n"
        "  <rect width=\"100%%\" height=\"100%%\" fill=\"white\"/>\n",
        graphics_ctx->window_width, graphics_ctx->window_height, graphics_ctx->window_title);
}

void write_svg_footer() {
    if (!graphics_ctx->svg_output) {
        return;
    }
    
    fprintf(graphics_ctx->svg_output, "</svg>\n");
}

Value call_dmo_graphics_function(const char* name, ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (!graphics_ctx) {
        fprintf(stderr, "Error: Graphics system not initialized\n");
        return create_void_value();
    }
    
    // Handle dmo.gr.create.window
    if (strstr(name, "dmo.gr.create.window") || strstr(name, "create.window")) {
        return dmo_gr_create_window(args, arg_count, ctx);
    }
    
    // Handle dmo.gr.create.line
    if (strstr(name, "dmo.gr.create.line") || strstr(name, "create.line")) {
        return dmo_gr_create_line(args, arg_count, ctx);
    }
    
    // Handle dmo.gr.create.sqr
    if (strstr(name, "dmo.gr.create.sqr") || strstr(name, "create.sqr")) {
        return dmo_gr_create_sqr(args, arg_count, ctx);
    }
    
    // Handle dmo.gr.create.crle
    if (strstr(name, "dmo.gr.create.crle") || strstr(name, "create.crle")) {
        return dmo_gr_create_crle(args, arg_count, ctx);
    }
    
    // Handle dmo.gr.display
    if (strstr(name, "dmo.gr.display") || strstr(name, "display")) {
        return dmo_gr_display(args, arg_count, ctx);
    }
    
    // Handle input detection functions
    if (strstr(name, "dmo_key")) {
        return dmo_key_pressed(args, arg_count, ctx);
    }
    
    if (strstr(name, "dmo[") || strstr(name, "dmo_element")) {
        return dmo_element_pressed(args, arg_count, ctx);
    }
    
    if (strstr(name, "collide")) {
        return dmo_collide(args, arg_count, ctx);
    }
    
    fprintf(stderr, "Error: Unknown graphics function '%s'\n", name);
    return create_void_value();
}

Value dmo_gr_create_window(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    // Parse arguments: title="title", size=349
    char* title = "DMO Graphics Window";
    int size = 500;
    
    for (int i = 0; i < arg_count; i++) {
        // For simplicity, assume first arg is title, second is size
        Value arg = execute_node(args[i], ctx);
        
        if (i == 0 && arg.type == VALUE_STRING) {
            title = arg.string;
        } else if ((i == 1 || (i == 0 && arg.type == VALUE_NUMBER)) && arg.type == VALUE_NUMBER) {
            size = (int)arg.number;
        }
        
        if (i > 0 || arg.type != VALUE_STRING) {
            free_value(arg);
        }
    }
    
    // Update graphics context
    free(graphics_ctx->window_title);
    graphics_ctx->window_title = strdup(title);
    graphics_ctx->window_width = size;
    graphics_ctx->window_height = size;
    graphics_ctx->window_created = true;
    
    // Start SVG output
    start_svg_output();
    
    printf("Created window: '%s' (size: %dx%d)\n", title, size, size);
    
    return create_void_value();
}

Value dmo_gr_create_line(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count < 1) {
        fprintf(stderr, "Error: create.line requires at least one argument\n");
        return create_void_value();
    }
    
    Value length_val = execute_node(args[0], ctx);
    if (length_val.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: line length must be a number\n");
        free_value(length_val);
        return create_void_value();
    }
    
    int length = (int)length_val.number;
    free_value(length_val);
    
    // Ensure SVG output is started
    if (!graphics_ctx->svg_output) {
        start_svg_output();
    }
    
    // Draw line in SVG (simple horizontal line)
    if (graphics_ctx->svg_output) {
        fprintf(graphics_ctx->svg_output,
            "  <line x1=\"50\" y1=\"100\" x2=\"%d\" y2=\"100\" "
            "stroke=\"black\" stroke-width=\"2\"/>\n",
            50 + length);
    }
    
    printf("Created line with length: %d\n", length);
    
    return create_void_value();
}

Value dmo_gr_create_sqr(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count < 4) {
        fprintf(stderr, "Error: create.sqr requires 4 arguments (x, y, width, height)\n");
        return create_void_value();
    }
    
    int coords[4];
    for (int i = 0; i < 4; i++) {
        Value val = execute_node(args[i], ctx);
        if (val.type != VALUE_NUMBER) {
            fprintf(stderr, "Error: square coordinates must be numbers\n");
            free_value(val);
            return create_void_value();
        }
        coords[i] = (int)val.number;
        free_value(val);
    }
    
    // Ensure SVG output is started
    if (!graphics_ctx->svg_output) {
        start_svg_output();
    }
    
    // Draw rectangle in SVG
    if (graphics_ctx->svg_output) {
        fprintf(graphics_ctx->svg_output,
            "  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
            "fill=\"none\" stroke=\"black\" stroke-width=\"2\"/>\n",
            coords[0], coords[1], coords[2], coords[3]);
    }
    
    printf("Created square at (%d, %d) with size %dx%d\n", 
           coords[0], coords[1], coords[2], coords[3]);
    
    return create_void_value();
}

Value dmo_gr_create_crle(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count < 1) {
        fprintf(stderr, "Error: create.crle requires at least one argument\n");
        return create_void_value();
    }
    
    Value radius_val = execute_node(args[0], ctx);
    if (radius_val.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: circle radius must be a number\n");
        free_value(radius_val);
        return create_void_value();
    }
    
    int radius = (int)radius_val.number;
    free_value(radius_val);
    
    int center_x = 150, center_y = 150;
    
    // Check for second argument (might be center position or curve parameter)
    if (arg_count >= 2) {
        Value second_val = execute_node(args[1], ctx);
        if (second_val.type == VALUE_NUMBER) {
            center_y = (int)second_val.number;
        }
        free_value(second_val);
    }
    
    // Ensure SVG output is started
    if (!graphics_ctx->svg_output) {
        start_svg_output();
    }
    
    // Draw circle in SVG
    if (graphics_ctx->svg_output) {
        if (arg_count == 1) {
            // Simple circle
            fprintf(graphics_ctx->svg_output,
                "  <circle cx=\"%d\" cy=\"%d\" r=\"%d\" "
                "fill=\"none\" stroke=\"black\" stroke-width=\"2\"/>\n",
                center_x, center_y, radius);
            printf("Created circle at (%d, %d) with radius: %d\n", center_x, center_y, radius);
        } else {
            // Curve (simplified as an ellipse)
            fprintf(graphics_ctx->svg_output,
                "  <ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" "
                "fill=\"none\" stroke=\"black\" stroke-width=\"2\"/>\n",
                center_x, center_y, radius, center_y);
            printf("Created curve with radius: %d, parameter: %d\n", radius, center_y);
        }
    }
    
    return create_void_value();
}

// Utility functions for graphics
void add_graphics_element(const char* id, int x, int y, int width, int height, Color color, int type) {
    if (!graphics_ctx) return;
    
    if (graphics_ctx->element_count >= graphics_ctx->element_capacity) {
        graphics_ctx->element_capacity *= 2;
        graphics_ctx->elements = realloc(graphics_ctx->elements, 
            sizeof(GraphicsElement) * graphics_ctx->element_capacity);
    }
    
    GraphicsElement* element = &graphics_ctx->elements[graphics_ctx->element_count++];
    element->id = id ? strdup(id) : NULL;
    element->x = x;
    element->y = y;
    element->width = width;
    element->height = height;
    element->color = color;
    element->hitbox.x = x;
    element->hitbox.y = y;
    element->hitbox.width = width;
    element->hitbox.height = height;
    element->type = type;
}

GraphicsElement* find_element_by_id(const char* id) {
    if (!graphics_ctx || !id) return NULL;
    
    for (int i = 0; i < graphics_ctx->element_count; i++) {
        if (graphics_ctx->elements[i].id && strcmp(graphics_ctx->elements[i].id, id) == 0) {
            return &graphics_ctx->elements[i];
        }
    }
    return NULL;
}

bool check_collision(GraphicsElement* a, GraphicsElement* b) {
    if (!a || !b) return false;
    
    return (a->hitbox.x < b->hitbox.x + b->hitbox.width &&
            a->hitbox.x + a->hitbox.width > b->hitbox.x &&
            a->hitbox.y < b->hitbox.y + b->hitbox.height &&
            a->hitbox.y + a->hitbox.height > b->hitbox.y);
}

Color parse_color(int r, int g, int b) {
    Color color;
    color.r = r < 0 ? 0 : (r > 255 ? 255 : r);
    color.g = g < 0 ? 0 : (g > 255 ? 255 : g);
    color.b = b < 0 ? 0 : (b > 255 ? 255 : b);
    return color;
}

// Advanced graphics functions
Value dmo_gr_display(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count < 1) {
        fprintf(stderr, "Error: display requires at least text argument\n");
        return create_void_value();
    }
    
    Value text_val = execute_node(args[0], ctx);
    if (text_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: display text must be a string\n");
        free_value(text_val);
        return create_void_value();
    }
    
    // Default parameters
    int x = 10, y = 10, width = 100, height = 20;
    Color color = parse_color(0, 0, 0); // Black text
    char* id = NULL;
    
    // Parse named parameters (simplified - would need proper parser enhancement)
    // For now, use positional parameters: text, x, y, width, height, r, g, b, id
    if (arg_count >= 3) {
        Value x_val = execute_node(args[1], ctx);
        Value y_val = execute_node(args[2], ctx);
        if (x_val.type == VALUE_NUMBER) x = (int)x_val.number;
        if (y_val.type == VALUE_NUMBER) y = (int)y_val.number;
        free_value(x_val);
        free_value(y_val);
    }
    
    if (arg_count >= 5) {
        Value w_val = execute_node(args[3], ctx);
        Value h_val = execute_node(args[4], ctx);
        if (w_val.type == VALUE_NUMBER) width = (int)w_val.number;
        if (h_val.type == VALUE_NUMBER) height = (int)h_val.number;
        free_value(w_val);
        free_value(h_val);
    }
    
    if (arg_count >= 8) {
        Value r_val = execute_node(args[5], ctx);
        Value g_val = execute_node(args[6], ctx);
        Value b_val = execute_node(args[7], ctx);
        if (r_val.type == VALUE_NUMBER && g_val.type == VALUE_NUMBER && b_val.type == VALUE_NUMBER) {
            color = parse_color((int)r_val.number, (int)g_val.number, (int)b_val.number);
        }
        free_value(r_val);
        free_value(g_val);
        free_value(b_val);
    }
    
    // Ensure SVG output is started
    if (!graphics_ctx->svg_output) {
        start_svg_output();
    }
    
    // Draw text in SVG
    if (graphics_ctx->svg_output) {
        fprintf(graphics_ctx->svg_output,
            "  <text x=\"%d\" y=\"%d\" font-family=\"Arial\" font-size=\"%d\" "
            "fill=\"rgb(%d,%d,%d)\">%s</text>\n",
            x, y + height, height, color.r, color.g, color.b, text_val.string);
        printf("Display text '%s' at (%d, %d) with color rgb(%d,%d,%d)\n", 
               text_val.string, x, y, color.r, color.g, color.b);
    }
    
    // Add to elements list
    add_graphics_element(id, x, y, width, height, color, 3); // type 3 = text
    
    free_value(text_val);
    return create_void_value();
}

// Input detection functions
Value dmo_key_pressed(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: dmo_key requires exactly one argument\n");
        return create_number_value(0);
    }
    
    Value key_val = execute_node(args[0], ctx);
    if (key_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: dmo_key argument must be a string\n");
        free_value(key_val);
        return create_number_value(0);
    }
    
    // Simulate key press detection (in real implementation would check actual input)
    // For demo purposes, simulate some key presses
    bool pressed = false;
    if (strcmp(key_val.string, "a") == 0) {
        pressed = graphics_ctx->input.keys['a'];
    } else if (strcmp(key_val.string, "space") == 0) {
        pressed = graphics_ctx->input.keys[' '];
    }
    // Add more key mappings as needed
    
    free_value(key_val);
    return create_number_value(pressed ? 1 : 0);
}

Value dmo_element_pressed(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: dmo element check requires exactly one argument\n");
        return create_number_value(0);
    }
    
    Value id_val = execute_node(args[0], ctx);
    if (id_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: element ID must be a string\n");
        free_value(id_val);
        return create_number_value(0);
    }
    
    GraphicsElement* element = find_element_by_id(id_val.string);
    bool pressed = false;
    
    if (element && graphics_ctx->input.mouse_pressed) {
        // Check if mouse is within element bounds
        pressed = (graphics_ctx->input.mouse_x >= element->x &&
                  graphics_ctx->input.mouse_x <= element->x + element->width &&
                  graphics_ctx->input.mouse_y >= element->y &&
                  graphics_ctx->input.mouse_y <= element->y + element->height);
    }
    
    free_value(id_val);
    return create_number_value(pressed ? 1 : 0);
}

Value dmo_collide(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 2) {
        fprintf(stderr, "Error: collide requires exactly two element IDs\n");
        return create_number_value(0);
    }
    
    Value id1_val = execute_node(args[0], ctx);
    Value id2_val = execute_node(args[1], ctx);
    
    if (id1_val.type != VALUE_STRING || id2_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: collide arguments must be strings\n");
        free_value(id1_val);
        free_value(id2_val);
        return create_number_value(0);
    }
    
    GraphicsElement* elem1 = find_element_by_id(id1_val.string);
    GraphicsElement* elem2 = find_element_by_id(id2_val.string);
    
    bool colliding = check_collision(elem1, elem2);
    
    free_value(id1_val);
    free_value(id2_val);
    return create_number_value(colliding ? 1 : 0);
}
