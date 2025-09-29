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
    
    printf("DMO Graphics Library initialized\n");
}

void cleanup_dmo_graphics() {
    if (!graphics_ctx) {
        return;
    }
    
    if (graphics_ctx->svg_output) {
        end_svg_output();
    }
    
    free(graphics_ctx->window_title);
    free(graphics_ctx);
    graphics_ctx = NULL;
    
    printf("DMO Graphics Library cleaned up\n");
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
