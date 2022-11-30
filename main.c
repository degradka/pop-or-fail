//--------//
//  Bugs  //
//--------//
// None! (yet)

//--------//
//  Todo  // 
//--------//
// -- Tweak score addition on line 293
// -- Add sounds using some 8bit sound generator
// -- Calculate and display mistakes (clicking on wrong circles) on game end screen
// -- Calculate and display accuracy on game end screen
// -- Display authors's nicknames somewhere in the game (likely on the title screen)

#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>

//---------//
// Defines //
//---------//
#define CIRCLE_DEFAULT        { defaultRadius, (Vector2) {0, 0}, BLACK }
#define RECTANGLE_DEFAULT     { (Vector2) {0, 0}, (Vector2) {130, 50}, 0.6f, 2, GRAY }
#define MAX_ROW               5
#define MAX_COL               5
#define MAX_COLOR             4
#define defaultRadius         30
#define defaultTimeOutTime    1.5f

//----------------------//
// Types and Structures //
//----------------------//
// Basic circle
typedef struct Circle {
    float radius;
    Vector2 position;
    Color color;
} Circle;

void circleDraw(Circle circle) {
    DrawCircleV(circle.position, circle.radius, circle.color);
}

// A rectangle with rounded corners (for UI)
typedef struct RoundedRectangle {
    Vector2 position;
    Vector2 size;

    float roundness;
    int segments;
    Color color;
} RoundedRectangle;

void roundedRectangleDraw(RoundedRectangle roundedRectangle) {
    DrawRectangleRounded((Rectangle){ roundedRectangle.position.x, roundedRectangle.position.y, roundedRectangle.size.x, roundedRectangle.size.y },
                        roundedRectangle.roundness, roundedRectangle.segments, roundedRectangle.color);
}

// Text (for UI)
typedef struct TextPro {
    Font font;
    const char *text;
    Vector2 position;
    Vector2 origin;
    float rotation;
    float fontSize;
    float spacing;
    Color color;
} TextPro;

void textProDraw(struct TextPro textPro) {
    DrawTextPro(textPro.font, textPro.text, textPro.position, textPro.origin, textPro.rotation, textPro.fontSize, textPro.spacing, textPro.color);
}

//------------------//
// Global Variables //
//------------------//
const char *gameTitle = "Pop or Fail";
const int screenWidth = 360;    // Original: 360
const int screenHeight = 460;   // Original: 460
const int defaultFontSize = 20; // Original: 20
const int gap = 10;             // Size of the gap between every circle; Original: 10
const float startTime = 20.0f;
static Circle circles[MAX_COL][MAX_ROW];

static Circle circleToEliminate = CIRCLE_DEFAULT;
static RoundedRectangle scoreRectangle = RECTANGLE_DEFAULT;
static RoundedRectangle timeRectangle = RECTANGLE_DEFAULT;

static TextPro scoreText;
static TextPro timeText;

static Color backgroundColor = RAYWHITE;

static bool isMouseInCircle;
static int hoveredCircle[2];
static float time = startTime;
static int score;
static int endScore;
static int highscore;
static int circlesToKill;
static bool gameEnded = false;
static bool gameStarted = false;
static bool timeOut = false;
static float timeOutTime = defaultTimeOutTime;

//------------------//
// Module Functions //
//------------------//
static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void UnloadGame(void);
static void UpdateDrawFrame(void);

int main(void) {
    InitWindow(screenWidth, screenHeight, gameTitle);
    InitGame();

    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }

    UnloadGame();
    CloseWindow();
    return 0;
}

static Color GetRandomColor(void) {
    return (Color[MAX_COLOR]){ RED, GREEN, BLUE, GOLD }[rand() % MAX_COLOR];
}

// Get the amount of colors that are present in the sheet
static int GetRandomPresentColors() {
    int count = 0;
    bool whichArePresent[4] = { false, false, false, false }; // Red, Green, Blue, Gold
    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {

            // Red
            if (!whichArePresent[0]
            && ColorToInt(circles[i][j].color) == ColorToInt(RED)) {
                whichArePresent[0] = true;
                count++;
            }
            // Green
            if (!whichArePresent[1]
            && ColorToInt(circles[i][j].color) == ColorToInt(GREEN)) {
                whichArePresent[1] = true;
                count++;
            }
            // Blue
            if (!whichArePresent[2]
            && ColorToInt(circles[i][j].color) == ColorToInt(BLUE)) {
                whichArePresent[2] = true;
                count++;
            }
            // Gold
            if (!whichArePresent[3]
            && ColorToInt(circles[i][j].color) == ColorToInt(GOLD)) {
                whichArePresent[3] = true;
                count++;
            }
        }
    }
    return count;
}

static void SetupCircles(void) {
    // Initialize the first circles' row
    for (int j = 0; j < MAX_COL; j++) {
        circles[0][j].radius = defaultRadius;
        circles[0][2].position = (Vector2){ screenWidth/2, screenHeight/2-defaultRadius*2-gap*3 };
        circles[0][1].position = (Vector2){ circles[0][2].position.x-(defaultRadius*2)-gap, circles[0][2].position.y };
        circles[0][0].position = (Vector2){ circles[0][1].position.x-(defaultRadius*2)-gap, circles[0][2].position.y };
        circles[0][3].position = (Vector2){ circles[0][2].position.x+(defaultRadius*2)+gap, circles[0][2].position.y };
        circles[0][4].position = (Vector2){ circles[0][3].position.x+(defaultRadius*2)+gap, circles[0][2].position.y };
        circles[0][j].color = GetRandomColor();
    }

    for (int i = 1; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            // Initialize circles' ...

            // Radius
            circles[i][j].radius = defaultRadius;

            // Position
            circles[i][2].position = (Vector2){ circles[0][2].position.x, (circles[i-1][2].position.y+defaultRadius*2)+gap };
            circles[i][1].position = (Vector2){ circles[0][1].position.x, circles[i][2].position.y };
            circles[i][0].position = (Vector2){ circles[0][0].position.x, circles[i][2].position.y };
            circles[i][3].position = (Vector2){ circles[0][3].position.x, circles[i][2].position.y };
            circles[i][4].position = (Vector2){ circles[0][4].position.x, circles[i][2].position.y };

            // Color
            circles[i][j].color = GetRandomColor();
        }
    }

    // If the sheet doesn't have all 4 colors, re-setup the circles
    if (GetRandomPresentColors() != 4) {
        SetupCircles();
    }
}

static void DrawCircles(void) {
    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            circleDraw(circles[i][j]);
        }
    }
}

static void SetupCircleToEliminate(void) {
    circleToEliminate.position = (Vector2){ circles[0][2].position.x, (circles[0][2].position.y-defaultRadius*2)-gap*2.5f };
    circleToEliminate.color = GetRandomColor();
}

static void SetupRectangles(void) {
    // Put the score rectangle at the left side
    scoreRectangle.size.x = defaultRadius*4+gap;
    scoreRectangle.size.y = circleToEliminate.radius*2-10;
    scoreRectangle.position = (Vector2){ circleToEliminate.position.x-defaultRadius-scoreRectangle.size.x-gap, circleToEliminate.position.y-scoreRectangle.size.y/2 };

    // Put the time rectangle at the right side
    timeRectangle.size.x = defaultRadius*4+gap;
    timeRectangle.size.y = circleToEliminate.radius*2-10;
    timeRectangle.position = (Vector2){ circleToEliminate.position.x+defaultRadius+gap, circleToEliminate.position.y-timeRectangle.size.y/2 };
}

static void SetupText(void) {
    // Put the score text in the middle of the score rectangle
    scoreText.font = GetFontDefault();
    const char *scoreTextValue = TextFormat("Score: %d", score);
    scoreText.text = scoreTextValue;
    scoreText.position = (Vector2){ scoreRectangle.position.x+(scoreRectangle.size.x-MeasureText(scoreTextValue, defaultFontSize))/2, circleToEliminate.position.y };
    scoreText.origin = (Vector2){ 0, defaultFontSize/2 };
    scoreText.rotation = 0;
    scoreText.fontSize = defaultFontSize;
    scoreText.spacing = 2;
    scoreText.color = backgroundColor;

    // Put the time text in the middle of the score rectangle
    timeText.font = GetFontDefault();
    const char *timeTextValue = TextFormat("Time: %.2f", time);
    timeText.text = timeTextValue;
    timeText.position = (Vector2){ timeRectangle.position.x+(timeRectangle.size.x-MeasureText(timeTextValue, defaultFontSize))/2, circleToEliminate.position.y };
    timeText.origin = (Vector2){ 0, defaultFontSize/2 };
    timeText.rotation = 0;
    timeText.fontSize = defaultFontSize;
    timeText.spacing = 2;
    timeText.color = backgroundColor;
}

static void UpdateText(void) {
    const char *scoreTextValue = TextFormat("Score: %d", score);
    scoreText.text = scoreTextValue;
    scoreText.position = (Vector2){ scoreRectangle.position.x+(scoreRectangle.size.x-MeasureText(scoreTextValue, defaultFontSize))/2, circleToEliminate.position.y };

    const char *timeTextValue = TextFormat("Time: %.2f", time);
    timeText.text = timeTextValue;
    timeText.position = (Vector2){ timeRectangle.position.x+(timeRectangle.size.x-MeasureText(timeTextValue, defaultFontSize))/2, circleToEliminate.position.y };
}

static void CheckCircleCollision(Vector2 mousePos) {
    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            if (CheckCollisionPointCircle(mousePos, circles[i][j].position, defaultRadius) && ColorToInt(circles[i][j].color) != ColorToInt(backgroundColor)) { 
                hoveredCircle[0] = i;
                hoveredCircle[1] = j;
                isMouseInCircle = true;
                return; // Assume player can't have their mouse on >=2 circles at the same time
            } else {
                isMouseInCircle = false;
            }
        }
    }
}

static void SetupCirclesToKill(void) {
    circlesToKill = 0;
    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            if (ColorToInt(circles[i][j].color) == ColorToInt(circleToEliminate.color)) {
                circlesToKill += 1;
            }
        }
    }
}

static void CheckIfPlayerDoneKillingThosePoorCircles(void) {
    if (circlesToKill == 0) {
        time = time + 0.5f;
        SetupCircles();
        SetupCircleToEliminate();
        SetupCirclesToKill();
    }
}

static void InitGame(void) {
    SetMousePosition(0, 0);
    SetupCircles();
    SetupCircleToEliminate();
    SetupCirclesToKill();
    SetupRectangles();
    SetupText();
}

static void UnloadGame(void) {
    //
}

static void UpdateGame(void) {
    float delta = GetFrameTime();
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    Vector2 mousePos = (Vector2){ mouseX, mouseY };
    if (!gameEnded && gameStarted) {
        time = time - 1 * delta;
    } else if (!gameEnded && !gameStarted) {
        time = startTime;
    } else if (gameEnded && !gameStarted) {
        time = startTime;
        if (endScore > highscore) {
            highscore = endScore;
        }
        score = 0;
    }
    UpdateText();
    CheckCircleCollision(mousePos);

    // Check if player is clicking the correct circle
    if (!gameEnded
    && !timeOut
    && gameStarted
    && isMouseInCircle
    && ColorToInt(circles[hoveredCircle[0]][hoveredCircle[1]].color) == ColorToInt(circleToEliminate.color)
    && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        circlesToKill--;
        score++;
        circles[hoveredCircle[0]][hoveredCircle[1]].color = backgroundColor;
        CheckIfPlayerDoneKillingThosePoorCircles();
        return;
    }

    if (gameEnded == true) {
        return;
    }

    if (time <= 0.0f && !gameEnded && !timeOut) {
        timeOut = true;
        gameEnded = false;
        gameStarted = false;
        endScore = score;
        SetupCircles();
        SetupCircleToEliminate();
        SetupCirclesToKill();
    }

    if (timeOutTime >= 0.0f && !gameEnded && timeOut) {
        timeOutTime = timeOutTime - 1 * delta;
    }

    if (timeOutTime <= 0.0f && !gameEnded && timeOut && !gameStarted) {
        gameEnded = true;
        timeOut = false;
        timeOutTime = defaultTimeOutTime;
    }
}

static void DrawGame(void) {
    if (!gameStarted && !gameEnded && !timeOut) {
        BeginDrawing();

            ClearBackground(backgroundColor);

            DrawText(gameTitle, screenWidth/2-MeasureText(gameTitle, 40)/2, screenHeight/2-40, 40, BLACK);
            DrawText("Click to start game", screenWidth/2-MeasureText("Click to start game", 30)/2, screenHeight/2+30, 30, BLACK);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                gameStarted = true;
                gameEnded = false;
                timeOut =  false;
            }

        EndDrawing();
    } else if (gameStarted && !gameEnded && !timeOut) {
        BeginDrawing();

            ClearBackground(backgroundColor);

            DrawCircles();
            circleDraw(circleToEliminate);
            roundedRectangleDraw(scoreRectangle);
            roundedRectangleDraw(timeRectangle);

            textProDraw(scoreText);
            textProDraw(timeText);

            /*
            if (isMouseInCircle == true) {
                DrawText("mouseInCircle", 0, screenHeight-30, 30, PURPLE);
            } else {
                DrawText("mouseNotInCircle", 0, screenHeight-30, 30, PURPLE);
            }

            DrawText(TextFormat("CTK: %d", circlesToKill), 0, screenHeight/2, 20, BLACK);
            */

        EndDrawing();
    } else if (!gameStarted && gameEnded && !timeOut) {
        BeginDrawing();

            ClearBackground(backgroundColor);

            //DrawText("Time is up!", screenWidth/2-MeasureText("Time is up!", 40)/2, screenHeight/2-120, 40, BLACK);
            DrawText(TextFormat("Highscore: %d", highscore), screenWidth/2-MeasureText(TextFormat("Highscore: %d", highscore), 24)/2, screenHeight/2-60, 24, BLACK);
            DrawText(TextFormat("You've scored: %d points!", endScore), screenWidth/2-MeasureText(TextFormat("You've scored: %d points!", endScore), 24)/2, screenHeight/2-30, 24, BLACK);

            DrawText("Click to start game", screenWidth/2-MeasureText("Click to start game", 30)/2, screenHeight/2+60, 30, BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                gameStarted = true;
                gameEnded = false;
                SetupCircles();
                SetupCircleToEliminate();
                SetupCirclesToKill();
            }

        EndDrawing();
    } else if (!gameStarted && !gameEnded && timeOut) {
        BeginDrawing();

            ClearBackground(backgroundColor);

            // DrawText(TextFormat("timeOutTime: %f", timeOutTime), screenWidth/2-MeasureText(TextFormat("timeOutTime: %f"), timeOutTime), screenHeight/2-20, 20, PURPLE);
            DrawText("Time is out!", screenWidth/2-MeasureText(TextFormat("Time is Out!"), 30)/2, screenHeight/2-30, 30, BLACK);

        EndDrawing();
    }
}

static void UpdateDrawFrame(void) {
    UpdateGame();
    DrawGame();
}
