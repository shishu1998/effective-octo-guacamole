#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "GameState.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;
ShaderProgram program;
Matrix projectionMatrix;
const Uint8 *keys = SDL_GetKeyboardState(nullptr);
float lastFrameTicks = 0.0f;
float elapsed = 0.0f;
float accumulator = 0.0f;
GameState state;

void init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glViewport(0, 0, 960, 540);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	program.SetProjectionMatrix(projectionMatrix);
	glUseProgram(program.programID);
	glClearColor(0.553, 0.765, 0.855, 0.0);

	state.loadResources();
}

void updateGameState(GameState& state, float elapsed) {
	state.updateGameState(elapsed);
}

int main(int argc, char *argv[])
{
	init();
	SDL_Event event;
	bool done = false;
	state.playBackgroundMusic();
	while (!done) {
		glClear(GL_COLOR_BUFFER_BIT);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			//Selecting "back to menu" on defeat screen
			else if (state.mode == Defeat && event.type == SDL_MOUSEBUTTONDOWN) {
				float mouseX = (((float)event.button.x / 960.0f) * 3.55f) - 1.775f;
				float mouseY = (((float)(540.0f - event.button.y) / 540.0f) * 2.0f) - 1.0f;
				if (mouseX >= -0.55f && mouseX <= 0.45f && mouseY >= -0.3f && mouseY <= -0.15f) {
					state.goToNextLevel();
				}
			}
		}
		state.processKeys(keys);
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		elapsed += accumulator;
		if (elapsed < FIXED_TIMESTEP) {
			accumulator = elapsed;
			//continue;
		}
		while (elapsed >= FIXED_TIMESTEP) {
			state.updateGameState(FIXED_TIMESTEP);
			elapsed -= FIXED_TIMESTEP;
		}
		accumulator = elapsed;
		state.Render(program);
		SDL_GL_SwapWindow(displayWindow);
	}
	SDL_Quit();
	return 0;
}
