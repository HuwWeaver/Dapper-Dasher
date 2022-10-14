#include "raylib.h"

struct AnimData
{
    Rectangle rect;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rect.height;
};

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    //Update running time
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        //Update animation frame
        data.rect.x = data.frame * data.rect.width;
        data.frame++;
        if(data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }

    return data;
}

int main ()
{
    //Window Dimensions
    int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 358;

    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher");

    //Acceleration due to gravity (pixels/s)/s
    const int gravity{1000};

    //Nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    const int sizeOfNebulae{6};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rect.x = 0.0;
        nebulae[i].rect.y = 0.0;
        nebulae[i].rect.width = nebula.width/8;
        nebulae[i].rect.height = nebula.height/8;
        nebulae[i].pos.x = windowDimensions[0] + (300 * i);
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 0.0;
    }

    //Nebula x velocity (pixels/s)
    const int nebVel{-200};

    //Finish line
    float finishLine{nebulae[sizeOfNebulae - 1].pos.x};
    
    //Scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    AnimData scarfyData{
        {0, 0, scarfy.width/6.0f, scarfy.height/1.0f}, 
        {windowDimensions[0]/2 - (scarfy.width/6.0f)/2.0f, windowDimensions[1] - scarfy.height/1.0f}, 
        0, 
        1.0/12.0, 
        0.0
    };

    //Jump velocity (pixels/s)
    const int jumpVel{-600};
    bool isInAir{false};
    int scarfyVel{0};

    //Nebula collision
    bool collision{};
    
    //Background Textures
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        //get delta time
        const float dt{GetFrameTime()};
        
        BeginDrawing();
        ClearBackground(WHITE);

        //Scroll background images
        bgX -= 20  * dt;
        if(bgX <= -background.width*2)
        {
            bgX = 0.0;
        }

        mgX -= 40 * dt;
        if(mgX <= -midground.width*2)
        {
            mgX = 0.0;
        }

        fgX -= 80 * dt;
        if (fgX <= -foreground.width*2)
        {
            fgX = 0.0;
        }

        //Draw background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        //Perform ground check
        if (isOnGround(scarfyData, windowDimensions[1]))
        {
            //Rectangle on the floor
            scarfyVel = 0;
            isInAir = false;
        }
        else
        {
            //Rectangle in the air - Apply Gravity
            scarfyVel += gravity * dt;
            isInAir = true;
        }
        
        //Check for Jumping
        if(IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            scarfyVel += jumpVel;
        }

        for (int i=0; i < sizeOfNebulae; i++)
        {
            //Update position of each nebula
            nebulae[i].pos.x += nebVel * dt;
        }

        //Update finish line posistion
        finishLine += nebVel * dt;

        //Update scarfy Position
        scarfyData.pos.y += scarfyVel * dt;

        //Update scarfy animation if not jumping
        if(!isInAir)
        {
            scarfyData = updateAnimData(scarfyData, dt, 5);
        }

        //Update nebulae animation
        for(int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dt, 7);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRect{nebula.pos.x + pad, nebula.pos.y + pad, nebula.rect.width - 2*pad, nebula.rect.height - 2*pad};
            Rectangle scarfyRect{scarfyData.pos.x, scarfyData.pos.y, scarfyData.rect.width, scarfyData.rect.height};

            if(CheckCollisionRecs(nebRect, scarfyRect))
            {
                collision = true;
            }
        
        }

        if(collision)
        {
            //Lose the game
            DrawText("Game Over!", windowDimensions[0]/4, windowDimensions[1]/2, 50, WHITE);
        }
        else
        {   
            if(scarfyData.pos.x >= finishLine)
            {
                //Win the game
                DrawText("You Win!", windowDimensions[0]/4, windowDimensions[1]/2, 50, WHITE);
            }
            else
            {
                //Draw Textures
                for(int i=0; i < sizeOfNebulae; i++)
                {
                    //Draw Nebulae
                    DrawTextureRec(nebula, nebulae[i].rect, nebulae[i].pos, WHITE);
                }
            }
            //Draw Scarfy
            DrawTextureRec(scarfy, scarfyData.rect, scarfyData.pos, WHITE);
        }

        EndDrawing();
    }

    //Unload all textures
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    CloseWindow();
}