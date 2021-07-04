// python -m glad --out-path=build --api="gl=2.1" --extensions="" --generator="c"
// g++ example/c++/sdl.cpp build/src/glad.c -Ibuild/include -lSDL2 -ldl

// OpenGL reference: https://learnopengl.com/book/book_pdf.pdf
// SDL: Simple DirectMedia Layer https://en.wikipedia.org/wiki/Simple_DirectMedia_Layer
// GLAD: Multi-language graphics library https://github.com/Dav1dde/glad
#include "../shader.h"
#include <iostream>
// Use vcpkg to install glad and sdl2 - see instructions here: https://vcpkg.io/en/getting-started.html?platform=windows
// make sure to edit solution to link the installed files e.g. "C:\vcpkg\installed\x64-windows\include"
#include <glad/glad.h>
// https://stackoverflow.com/questions/48723523/lnk2019-unresolved-external-symbol-c-sdl2-library
#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>

int main(int argc, char** argv) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL2 video subsystem couldn't be initialized. Error: "
            << SDL_GetError()
            << std::endl;
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("OpenGL GLAD SDL Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN |
        SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        std::cerr << "SDL2 Renderer couldn't be created. Error: "
            << SDL_GetError()
            << std::endl;
        exit(1);
    }

    // Create a OpenGL context on SDL2
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    // Load GL extensions using glad
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize the OpenGL context." << std::endl;
        exit(1);
    }

    // Loaded OpenGL successfully.
    std::cout << "OpenGL version loaded: " << GLVersion.major << "."
        << GLVersion.minor << std::endl;

    // Create an event handler
    SDL_Event event;
    // Loop condition
    bool running = true;

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("hello_v_shader.vs", "hello_f_shader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // top 
         -0.25f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // mid left
         0.25f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // mid right
         0.0f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // bottom mid 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // main render loop
    while (running) {
        SDL_PollEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;
                break;
            }
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // get and set offset variable - // 6.8 Exercise 2, pg 53
        int vertexOffset = glGetUniformLocation(ourShader.ID, "offset");
        //std::cout << "vertex offset retrieved: " << vertexOffset << std::endl;
        float offset = 0.2f;
        
        // render the triangle
        ourShader.use();
        // set offset in currently active shader program
        glUniform4f(vertexOffset, offset, offset*2, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6); // location 0 with 6 vertices

        SDL_GL_SwapWindow(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
   // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Destroy everything to not leak memory.
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}