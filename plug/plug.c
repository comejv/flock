#include "raylib.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#ifndef MODULE_NAME
#define MODULE_NAME "module.so"
#endif /* ifndef MODULE_NAME */

typedef void *module_main_func(void *state);

int main(int argc, char *argv[])
{
    void *state = NULL;
    while (1)
    {
        // Compile
        while (system("make"))
        {
            fprintf(stderr, "Failed to compile module. (%s)\n", dlerror());
            fprintf(stderr, "Retry ? ([y]/n).\n");
            if (getchar() == 'n')
            {
                exit(EXIT_FAILURE);
            }
        }

        // Load the library
        void *module = dlopen(MODULE_NAME, RTLD_NOW);

        while (module == NULL)
        {
            fprintf(stderr, "Failed to load module. (%s)\n", dlerror());
            fprintf(stderr, "Retry ? ([y]/n).\n");
            if (getchar() == 'n')
            {
                exit(EXIT_FAILURE);
            }
        }
        module_main_func *module_main = dlsym(module, "module_main");

        // Run the library
        if (state == NULL)
        {
            const int screenWidth = 1600;
            const int screenHeight = 800;

            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
            InitWindow(screenWidth, screenHeight, "Flocking simulator");
            SetTargetFPS(60);
        }

        state = module_main(state);

        dlclose(module);

        if (state == NULL)
        {
            break;
        }
    }

    CloseWindow();   // Close window and OpenGL context
    return EXIT_SUCCESS;
}
