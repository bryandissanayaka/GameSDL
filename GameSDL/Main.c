#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "constants.h"

bool borderless = true;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int lastFrameTime = 0;

struct Ball {
	float x;
	float y;
	float diameter;
	float velocityX;
	float velocityY;
} ball;

void StartBall() {
	ball.x = (WINDOW_WIDTH - ball.diameter) * 0.5f;
	ball.y = (WINDOW_HEIGHT - ball.diameter) * 0.5f;
	int pseudoRand = lastFrameTime % 4;
	if (pseudoRand == 0) {
		ball.velocityY = 90.0f;
		ball.velocityX = 150.0f;
	}
	else if (pseudoRand == 1){
		ball.velocityY = -90.0f;
		ball.velocityX = 150.0f;
	}
	else if (pseudoRand == 2) {
		ball.velocityY = -90.0f;
		ball.velocityX = -150.0f;
	}
	else if (pseudoRand == 3) {
		ball.velocityY = 90.0f;
		ball.velocityX = -150.0f;
	}
}

void BounceBall() {
	ball.velocityX *= -1.05f;
	int pseudoRand = lastFrameTime % 10;
	ball.velocityY += pseudoRand * 0.2f;
	if (ball.velocityX > MAX_BALL_SPEED) {
		ball.velocityX = MAX_BALL_SPEED;
	}else if (ball.velocityX < -MAX_BALL_SPEED) {
		ball.velocityX = -MAX_BALL_SPEED;
	}
	if (ball.velocityY > MAX_BALL_SPEED) {
		ball.velocityY = MAX_BALL_SPEED;
	}
	else if (ball.velocityY < -MAX_BALL_SPEED) {
		ball.velocityY = -MAX_BALL_SPEED;
	}
}


struct Player {
	float x;
	float y;
	int movingDirection;
} playerLeft, playerRight;


bool InitializeWindow(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}
	if (borderless) {
		window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);
	}
	else {
		window = SDL_CreateWindow("SDL C Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	}
	if (!window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}
	return true;
}

void DestroyWindow(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return;
}

void Start() {
	ball.diameter = 15;
	StartBall();

	playerLeft.x = 20;
	playerLeft.y = WINDOW_HEIGHT / 2;
	playerLeft.movingDirection = 0;

	playerRight.x = WINDOW_WIDTH - PLAYER_WIDTH - playerLeft.x;
	playerRight.y = WINDOW_HEIGHT / 2;
	playerRight.movingDirection = 0;
}

int ProcessInput(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		return 0;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			fprintf(stdout, "Exited game with Escape Key.\n");
			return 0;
		}
		else if(event.key.keysym.sym == SDLK_SPACE) {
			StartBall();
		}
		default:
			break;
	}
	return 1;
}

void Update(void) {
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), lastFrameTime + TARGET_FRAME_TIME)); //lock until reaching the target frame time
	int timeToWait = TARGET_FRAME_TIME - (SDL_GetTicks() - lastFrameTime);
	if (timeToWait > 0 && timeToWait <= TARGET_FRAME_TIME) {
		SDL_Delay(timeToWait);
	}

	float deltaTime = (SDL_GetTicks() - lastFrameTime) / 1000.0f;
	lastFrameTime = SDL_GetTicks();
	Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	
	//playerLeft movement
	if (currentKeyStates[SDL_SCANCODE_W]) {
		playerLeft.movingDirection = -1;
	}
	else if (currentKeyStates[SDL_SCANCODE_S]) {
		playerLeft.movingDirection = 1;
	}
	else {
		playerLeft.movingDirection = 0;
	}
	
	playerLeft.y += playerLeft.movingDirection * PLAYER_SPEED * deltaTime;
	if (playerLeft.y < 0) {
		playerLeft.y = 0;
	}
	else if (playerLeft.y + PLAYER_HEIGHT > WINDOW_HEIGHT) {
		playerLeft.y = WINDOW_HEIGHT - PLAYER_HEIGHT;
	}

	//playerRightMovement
	if (currentKeyStates[SDL_SCANCODE_UP]) {
		playerRight.movingDirection = -1;
	}
	else if (currentKeyStates[SDL_SCANCODE_DOWN]) {
		playerRight.movingDirection = 1;
	}
	else {
		playerRight.movingDirection = 0;
	}
	playerRight.y += playerRight.movingDirection * PLAYER_SPEED * deltaTime;
	
	
	if (playerRight.y < 0) {
		playerRight.y = 0;
	}
	else if (playerRight.y + PLAYER_HEIGHT > WINDOW_HEIGHT) {
		playerRight.y = WINDOW_HEIGHT - PLAYER_HEIGHT;
	}

	ball.x += ball.velocityX * deltaTime;
	ball.y += ball.velocityY * deltaTime;

	if (ball.y < 0 || (ball.y + ball.diameter) > WINDOW_HEIGHT) {
		ball.velocityY *= -1;
	}

	if (ball.x < playerLeft.x + PLAYER_WIDTH) {
		if (ball.y >= playerLeft.y && ball.y + ball.diameter <= playerLeft.y + PLAYER_HEIGHT) {
			BounceBall();
		}
		else {
			StartBall();
		}	
	}

	if (ball.x + ball.diameter > playerRight.x) {
		if (ball.y >= playerRight.y && ((ball.y + ball.diameter) <= (playerRight.y + PLAYER_HEIGHT))) {
			BounceBall();
		}
		else {
			StartBall();
		}
	}
}

void Render(void) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


	SDL_Rect ballRect = { (int)ball.x, (int)ball.y, (int)ball.diameter, (int)ball.diameter };
	SDL_Rect playerLeftRect = { (int)playerLeft.x, (int)playerLeft.y, PLAYER_WIDTH, PLAYER_HEIGHT };
	SDL_Rect playerRightRect = { (int)playerRight.x, (int)playerRight.y, PLAYER_WIDTH, PLAYER_HEIGHT };

	SDL_RenderFillRect(renderer, &ballRect);
	SDL_RenderFillRect(renderer, &playerLeftRect);
	SDL_RenderFillRect(renderer, &playerRightRect);
	
	SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]) {
	bool gameIsRunning = InitializeWindow();

	Start();

	while (gameIsRunning) {
		gameIsRunning = ProcessInput();
		Update();
		Render();
	}

	DestroyWindow();

	return 0;
}