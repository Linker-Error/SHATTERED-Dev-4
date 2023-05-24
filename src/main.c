/*******************************************************************************************
*
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "structs.h"
#include "DrawFunctions.h"
#include "InputChecks.h"
#include "Enemies.h"
#include "general.h"
#include <string.h>
#include "PixelFont.h"




const int WIDTH = 250, HEIGHT = 220;

Texture2D StoneTile;
Texture2D StoneWall;
Texture2D CharacterSpriteSheet;
Texture2D SwordSpriteSheet;
Texture2D IconSprites;
Texture2D EnemySpriteSheet1;

struct MapTextures DungeonTextures;


struct Enemy Enemies[16];


static Font LoadFont_PixelFont(void)
{
	Font font = { 0 };

	font.baseSize = 32;
	font.glyphCount = 95;
	font.glyphPadding = 4;

	// Custom font loading
	// NOTE: Compressed font image data (DEFLATE), it requires DecompressData() function
	int fontDataSize_PixelFont = 0;
	unsigned char* data = DecompressData(fontData_PixelFont, COMPRESSED_DATA_SIZE_FONT_PIXELFONT, &fontDataSize_PixelFont);
	Image imFont = { data, 512, 512, 1, 2 };

	// Load texture from image
	font.texture = LoadTextureFromImage(imFont);
	UnloadImage(imFont);  // Uncompressed data can be unloaded from memory

	// Assign glyph recs and info data directly
	// WARNING: This font data must not be unloaded
	font.recs = fontRecs_PixelFont;
	font.glyphs = fontGlyphs_PixelFont;

	return font;
}



void PauseMenu(struct Inventory* PlayerInventory, struct Sprite ItemIcons, Font *FontUsed)
{
	while (!IsKeyPressed(KEY_L))
	{
		BeginDrawing();

		ClearBackground(BLACK);
		DrawMenu(*PlayerInventory, ItemIcons, FontUsed);

		EndDrawing();
	}
}



int Game()
{
	int Tick = 0;
	int PlayerHealth = 25;
	int MaxHealth = 25;
	int ButtonPressed = 0;
	int CurrentPlayerAnimationFrame = 0;
	int AttackStartTick = 0;

	int PlayerAnimation[4] = {0, 2, 1, 2};
	int AttackAnimationWalking[2] = { 4, 3 };
	int AttackAnimationStanding[2] = { 5, 2 };

	bool moving = false;
	bool PlayerAttacking = false;
	bool AttackAnimation = false;
	bool PauseBuffer = false;


	StoneTile = LoadTexture("resources/StoneTile.png");
	StoneWall = LoadTexture("resources/StoneWall.png");
	CharacterSpriteSheet = LoadTexture("resources/CharacterSpriteSheet.png");
	SwordSpriteSheet = LoadTexture("resources/SwordSpriteSheet.png");
	IconSprites = LoadTexture("resources/IconSprites.png");
	EnemySpriteSheet1 = LoadTexture("resources/EnemySpriteSheet1.png");

	Vector2 PreviousPlayerPosition = { 200, 200 };
	Vector2 PlayerPosition = { 200, 200 };
	Vector2 PlayerMovement = { 0, 0 };

	Font PixelFont = LoadFont_PixelFont();







	struct Sprite CharacterSprite;
	struct Sprite EnemySprite;
	struct Sprite IconImages;
	

	struct Enemy Ghoul;

	struct Item ItemHeld;
	struct Item Sword;
	struct Item Bow;

	

	DungeonTextures.Floors[0] = &StoneTile;
	DungeonTextures.Walls[0] = &StoneWall;



	CharacterSprite.SpriteSheet = &CharacterSpriteSheet;
	CharacterSprite.SpriteLength = 10;
	CharacterSprite.SpriteHeight = 15;
	CharacterSprite.CurrentSprite.x = 0;
	CharacterSprite.CurrentSprite.y = 0;


	EnemySprite.SpriteHeight = 15;
	EnemySprite.SpriteLength = 10;
	EnemySprite.CurrentSprite.y = 0;
	EnemySprite.CurrentSprite.x = 0;
	EnemySprite.SpriteSheet = &EnemySpriteSheet1;


	IconImages.CurrentSprite = (Vector2){0, 0};
	IconImages.SpriteHeight = 10;
	IconImages.SpriteLength = 10;
	IconImages.SpriteSheet = &IconSprites;


	Bow.IconID.x = 1;
	Bow.IconID.y = 0;
	Bow.WeaponType = RangedBow;
	Bow.ItemType = Weapon;


	Sword.IsOnTop[0] = true;
	Sword.IsOnTop[3] = false;
	Sword.IsOnTop[1] = false;
	Sword.IsOnTop[2] = true;
	Sword.OffSet[0].x = 0;
	Sword.OffSet[1].x = 0;
	Sword.OffSet[2].x = 0;
	Sword.OffSet[3].x = 0;
	Sword.OffSet[0].y = 0;
	Sword.OffSet[1].y = 0;
	Sword.OffSet[2].y = 0;
	Sword.OffSet[3].y = 0;
	Sword.ItemSprite.SpriteHeight = 15;
	Sword.ItemSprite.SpriteLength = 10;
	Sword.ItemSprite.SpriteSheet = &SwordSpriteSheet;
	Sword.ItemSprite.CurrentSprite.x = 0;
	Sword.ItemSprite.CurrentSprite.y = 0;
	Sword.ItemType = Weapon;
	Sword.WeaponType = MeleeOneHand;
	Sword.IconID.x = 0;
	Sword.IconID.y = 0;

	


	ItemHeld = Sword;


	Ghoul.EnemySprite = EnemySprite;
	Ghoul.AttackingPlayer = false;
	Ghoul.IsDead = false;
	Ghoul.AttackRange = 20;
	Ghoul.CurrentInstruction = 0;
	Ghoul.EnemyPosition.x = 0;
	Ghoul.EnemyPosition.y = 0;
	Ghoul.LastPosition = Ghoul.EnemyPosition;
	Ghoul.EnemyHealth = 10;
	Ghoul.EnemyType = ghoul;
	Ghoul.EnemyTint = WHITE;
	Ghoul.LastAttacked = -100000;

	for (int i = 0; i < 256; ++i)
	{
		Ghoul.Routine[i] = 2;
	}

	Ghoul.Routine[100] = LookAround;



	Enemies[0] = Ghoul;
	Enemies[0].EnemyPosition.x = 200;
	Enemies[0].EnemyPosition.y = 40;
	Enemies[0].EnemySlowness = 2;
	Enemies[1] = Ghoul;
	Enemies[1].EnemyPosition.x = 280;
	Enemies[1].EnemyPosition.y = 40;
	Enemies[1].EnemySlowness = 3;


	

	struct Map Area =
	{
		{
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
		},
		{
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
		}
	};
	struct Inventory PlayerInventory =
	{
		{
			{Sword, Sword, Sword, Sword, Sword},
			{Bow, Bow, Bow, Bow, Bow},
			{Sword, Sword, Sword, Sword, Sword},
			{Bow, Bow, Bow, Bow, Bow},
			{Sword, Sword, Sword, Sword, Sword}
		}
	};

	while (!WindowShouldClose())
	{
		moving = false;
		PlayerAttacking = false;
		PauseBuffer = false;

		BeginDrawing();


		ClearBackground(BLACK);


		
		PlayerMovement = Movement_Checks();
		ButtonPressed = ButtonChecks();

		PreviousPlayerPosition = PlayerPosition;

		PlayerPosition.x += PlayerMovement.x;

		if ((Area.ceiling[RoundDown(PlayerPosition.y, 40) / 40][RoundDown(PlayerPosition.x - 100, 40) / 40] > 0) ||
			(Area.ceiling[RoundDown(PlayerPosition.y, 40) / 40][RoundUp(PlayerPosition.x - 100, 40) / 40] > 0))
		{
			PlayerPosition.x = PreviousPlayerPosition.x;
		}

		PlayerPosition.y += PlayerMovement.y;

		if ((Area.ceiling[RoundDown(PlayerPosition.y, 40) / 40][RoundDown(PlayerPosition.x - 100, 40) / 40] > 0) ||
			(Area.ceiling[RoundDown(PlayerPosition.y, 40) / 40][RoundUp(PlayerPosition.x - 100, 40) / 40] > 0))
		{
			PlayerPosition.y = PreviousPlayerPosition.y;
		}

		if ((PlayerPosition.x != PreviousPlayerPosition.x) || (PlayerPosition.y != PreviousPlayerPosition.y))
			moving = true;


		//TraceLog(5, TextFormat("%d %d", (int)PlayerMovement.x, (int)PlayerMovement.y));


		if (PlayerMovement.y == 4) { CharacterSprite.CurrentSprite.y = 0; }
		else if (PlayerMovement.y == -4) { CharacterSprite.CurrentSprite.y = 3; }
		else if (PlayerMovement.x == -4) { CharacterSprite.CurrentSprite.y = 1; }
		else if (PlayerMovement.x == 4) { CharacterSprite.CurrentSprite.y = 2; }
		
		if ((ButtonPressed == 1 && ItemHeld.WeaponType == MeleeOneHand) && Tick - AttackStartTick > 20)
		{
			PlayerAttacking = true;
			AttackAnimation = true;
			AttackStartTick = Tick;
		}
		else if (ButtonPressed == 2)
			PauseBuffer = true;

		if (Tick - AttackStartTick >= 15 && AttackAnimation == true)
			AttackAnimation = false;
			


		if ((Tick - AttackStartTick >= 8 && AttackAnimation == true) && moving == true)
		{
			CharacterSprite.CurrentSprite.x = AttackAnimationWalking[1];
		}
		else if ((Tick - AttackStartTick < 8 && AttackAnimation == true) && moving == true)
		{
			CharacterSprite.CurrentSprite.x = AttackAnimationWalking[0];
		}
		else if ((Tick - AttackStartTick >= 8 && AttackAnimation == true) && moving == false)
		{
			CharacterSprite.CurrentSprite.x = AttackAnimationStanding[1];
		}
		else if ((Tick - AttackStartTick < 8 && AttackAnimation == true) && moving == false)
		{
			CharacterSprite.CurrentSprite.x = AttackAnimationStanding[0];
		}
		else
		{
			if ((Tick % 15 == 0) && (moving == true) && (CharacterSprite.CurrentSprite.y == 0 || CharacterSprite.CurrentSprite.y == 3))
			{


				CurrentPlayerAnimationFrame = 0;
				if (CharacterSprite.CurrentSprite.x == 0)
					CharacterSprite.CurrentSprite.x = 1;
				else
					CharacterSprite.CurrentSprite.x = 0;
			}
			else if ((Tick % 10 == 0) && (moving == true) && (CharacterSprite.CurrentSprite.y == 1 || CharacterSprite.CurrentSprite.y == 2))
			{
				CharacterSprite.CurrentSprite.x = PlayerAnimation[CurrentPlayerAnimationFrame];
				CurrentPlayerAnimationFrame++;
				if (CurrentPlayerAnimationFrame == 4)
					CurrentPlayerAnimationFrame = 0;
			}
			else if (moving == false) 
			{ 
				CharacterSprite.CurrentSprite.x = 2; 
			}
		}

		ItemHeld.ItemSprite.CurrentSprite.y = CharacterSprite.CurrentSprite.y;
		ItemHeld.ItemSprite.CurrentSprite.x = CharacterSprite.CurrentSprite.x;




		
			





		for (int i = 0; i < 16; ++i)
		{
			if (Enemies[i].EnemyHealth > 0)
			{

				UpdateEnemy(&Enemies[i], Tick, &Area, PlayerPosition, &PlayerHealth, PlayerAttacking, CharacterSprite.CurrentSprite.y);

			}
			else
				Enemies[i].IsDead = true;
		}





		//TraceLog(5, TextFormat("%d, %d\n", (int)(CharacterSprite.CurrentSprite.x), (int)(CharacterSprite.CurrentSprite.y)));
		DrawGame(Area, DungeonTextures, PlayerPosition, CharacterSprite, Enemies, 2, ItemHeld, PlayerHealth, MaxHealth, PixelFont, AttackAnimation);
		//DrawTexturePro(CharacterSpriteSheet, (Rectangle){0, 0, 10, 15}, (Rectangle){200, 200, 40, 60}, (Vector2){0, 0}, 0, WHITE);

		//DrawSprite(Dummy.EnemySprite, Dummy.EnemyPosition);


		

		Tick++;


		

		
		EndDrawing();




		if (PauseBuffer == true)
			PauseMenu(&PlayerInventory, IconImages, &PixelFont);
	}

	UnloadTexture(StoneTile);
	UnloadTexture(StoneWall);
	UnloadTexture(CharacterSpriteSheet);
	UnloadTexture(SwordSpriteSheet);
	UnloadTexture(IconSprites);
	UnloadTexture(EnemySpriteSheet1);
}


int main()
{
	SetRandomSeed(GetTime());

	InitWindow(WIDTH * 4, HEIGHT * 4, "Shattered");
	SetTargetFPS(60);

	Game();

	CloseWindow();

	return 0;
}
