//---------//
// Bugs    //
//---------//
// -- Sometimes, after the new circle sheet appears, the circle which was the last circle player clicked on
// appears with background color
// -- Player can hold down LMB and scroll over the circles and the game will count it as valid input 
// (should be an easy fix)

#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>

//---------//
// Defines //
//---------//
#define defaultRadius        30
#define CIRCLE_DEFAULT       { defaultRadius, (Vector2) {0, 0}, BLACK }
#define RECTANGLE_DEFAULT    { (Vector2) {0, 0}, (Vector2) {130, 50}, 0.6f, 2, GRAY }
#define MAX_ROW              5
#define MAX_COL              5
#define MAX_COLOR            4

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

    void (*Draw)(struct TextPro);
} TextPro;

void textProDraw(struct TextPro textPro) {
    DrawTextPro(textPro.font, textPro.text, textPro.position, textPro.origin, textPro.rotation, textPro.fontSize, textPro.spacing, textPro.color);
}

//------------------//
// Global Variables //
//------------------//
const int screenWidth = 360;    // Original: 360
const int screenHeight = 460;   // Original: 460
//const int defaultRadius = 30;   // Original: 30
const int defaultFontSize = 20; // Original: 20
const int gap = 10;             // Size of the gap between every circle; Original: 10
const float startTime = 20;
static Circle circles[MAX_COL][MAX_ROW];

static Circle circleToEliminate = CIRCLE_DEFAULT;
static RoundedRectangle scoreRectangle = RECTANGLE_DEFAULT;
static RoundedRectangle timeRectangle = RECTANGLE_DEFAULT;

static TextPro scoreText;
static TextPro timeText;

static Color backgroundColor = RAYWHITE;

static bool isMouseInCircle;
static int hoveredCircle[2];
static float time = 20.0f;
static int score;
static int circlesToKill;
//static bool colorPresent[MAX_COLOR] = {true, true, true, true};
static bool gameEnded;

//------------------//
// Module Functions //
//------------------//
static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void UnloadGame(void);
static void UpdateDrawFrame(void);

int main(void) {
    InitWindow(screenWidth, screenHeight, "Pop OR Fail");
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

// Get a random color only if it's present on the circle sheet
static Color GetRandomColorFromCircles(void) {
    Color colors[MAX_COLOR] = { RED, GREEN, BLUE, GOLD };
    bool skipColorElse[MAX_COLOR];  // Red, Green, Blue, Gold
    bool colorPresent[MAX_COLOR]; // Red, green, blue, gold

    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            // Check if a color is present in the circle sheet

            // Red
            if (ColorToInt(circles[i][j].color) == ColorToInt(colors[0])) {
                colorPresent[0] = skipColorElse[0] = true;
            } else {
                if (!skipColorElse[0]) colorPresent[0] = false;
            }
            // Green
            if (ColorToInt(circles[i][j].color) == ColorToInt(colors[1])) {
                colorPresent[1] = skipColorElse[1] = true;
            } else {
                if (!skipColorElse[1]) colorPresent[1] = false;
            }
            // Blue
            if (ColorToInt(circles[i][j].color) == ColorToInt(colors[2])) {
                colorPresent[2] = skipColorElse[2] = true;
            } else {
                if (!skipColorElse[2]) colorPresent[2] = false;
            }
            // Gold
            if (ColorToInt(circles[i][j].color) == ColorToInt(colors[3])) {
                colorPresent[3] = skipColorElse[3] = true;
            } else {
                if (!skipColorElse[3]) colorPresent[3] = false;
            }
        }
    }

    for (int i = 0; i < MAX_COLOR; i++) {
        skipColorElse[i] = false;
    }

    int replaceColorIndex;
    // Replace any color that's not present in the sheet

    for (int i = 0; i < MAX_COLOR; i++) {
        if (colorPresent[0] == false) {
            while (true) {
                if (ColorToInt(colors[replaceColorIndex = rand() % MAX_COLOR]) != ColorToInt(RED)) {
                    colors[0] = colors[replaceColorIndex];
                    break;
                }
            }
        }
        if (colorPresent[1] == false) {
            while (true) {
                if (ColorToInt(colors[replaceColorIndex = rand() % MAX_COLOR]) != ColorToInt(GREEN)) {
                    colors[1] = colors[replaceColorIndex];
                    break;
                }
            }
        }
        if (colorPresent[2] == false) {
            while (true) {
                if (ColorToInt(colors[replaceColorIndex = rand() % MAX_COLOR]) != ColorToInt(BLUE)) {
                    colors[2] = colors[replaceColorIndex];
                    break;
                }
            }
        }
        if (colorPresent[3] == false) {
            while (true) {
                if (ColorToInt(colors[replaceColorIndex = rand() % MAX_COLOR]) != ColorToInt(GOLD)) {
                    colors[3] = colors[replaceColorIndex];
                    break;
                }
            }
        }
    }

    return colors[rand() % MAX_COLOR];
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

// Check if any circle is present on the sheet
static bool AreCirclesPresent() {
    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            if (ColorToInt(circles[i][j].color) != ColorToInt(backgroundColor)) return true;
        }
    }
    return false;
}

static void CheckCircleCollision(Vector2 mousePos) {
    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            if (CheckCollisionPointCircle(mousePos, circles[i][j].position, defaultRadius) && ColorToInt(circles[i][j].color) != ColorToInt(backgroundColor)) { ///ФЫВАФЫВЫВФЫ
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
    if (!gameEnded) time = time - 1 * delta;
    UpdateText();
    CheckCircleCollision(mousePos);

    // Check if player is clicking the correct circle
    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            if (gameEnded == false
            && isMouseInCircle == true
            && ColorToInt(circles[hoveredCircle[0]][hoveredCircle[1]].color) == ColorToInt(circleToEliminate.color)
            && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                circlesToKill -= 1;
                score++;
                circles[hoveredCircle[0]][hoveredCircle[1]].color = backgroundColor;
                CheckIfPlayerDoneKillingThosePoorCircles();
                if (!AreCirclesPresent()) {
                    gameEnded = true;
                    return;
                }
                //circleToEliminate.color = GetRandomColorFromCircles();
                return;
            }
        }
    }
    if (time == 0.0f) {
        gameEnded = true;
    }
    if (gameEnded == true) {
        if (time == 0.0f) {

        }
    }
}

static void DrawGame(void) {
    BeginDrawing();

        ClearBackground(backgroundColor);

        DrawCircles();
        circleDraw(circleToEliminate);
        roundedRectangleDraw(scoreRectangle);
        roundedRectangleDraw(timeRectangle);

        textProDraw(scoreText);
        textProDraw(timeText);
        /*
        DrawText(colorPresent[0] ? "yes" : "no", 0*90, 0, 30, RED);
        DrawText(colorPresent[1] ? "yes" : "no", 1*90, 0, 30, GREEN);
        DrawText(colorPresent[2] ? "yes" : "no", 2*90, 0, 30, BLUE);
        DrawText(colorPresent[3] ? "yes" : "no", 3*90, 0, 30, GOLD);
        */
        if (isMouseInCircle == true) {
            DrawText("mouseInCircle", 0, screenHeight-30, 30, PURPLE);
        } else {
            DrawText("mouseNotInCircle", 0, screenHeight-30, 30, PURPLE);
        }
        DrawText(TextFormat("CTK: %d", circlesToKill), 0, screenHeight/2, 20, BLACK);
        //DrawText(TextFormat("%x", ColorToInt(circleToEliminate.color)), screenWidth/2+20, screenHeight-30, 30, RED);
    EndDrawing();
}

static void UpdateDrawFrame(void) {
    UpdateGame();
    DrawGame();
}
