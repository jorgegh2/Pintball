#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleFonts.h"

#include <string>

// Constructor
ModuleFonts::ModuleFonts(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

// Destructor
ModuleFonts::~ModuleFonts()
{}

// Load new texture from file path
int ModuleFonts::Load(const char* texture_path, const char* characters, uint rows)
{
	int id = -1;

	if (texture_path == nullptr || characters == nullptr || rows == 0)
	{
		LOG("Could not load font");
		return id;
	}

	SDL_Texture* tex = App->textures->Load(texture_path);

	if (tex == nullptr || strlen(characters) >= MAX_FONT_CHARS)
	{
		LOG("Could not load font at %s with characters '%s'", texture_path, characters);
		return id;
	}

	id = 0;
	for (; id < MAX_FONTS; ++id)
		if (fonts[id].graphic == nullptr)
			break;

	if (id == MAX_FONTS)
	{
		LOG("Cannot load font %s. Array is full (max %d).", texture_path, MAX_FONTS);
		return id;
	}
	uint font1W;
	uint font1H;

	fonts[id].graphic = tex; // graphic: pointer to the texture
	fonts[id].rows = rows; // rows: rows of characters in the texture
	fonts[id].len = strlen(characters); // len: length of the table
	App->textures->GetSize(tex, font1W, font1H);
	fonts[id].row_chars = SDL_strlen(characters);
	fonts[id].char_w = 15;
	fonts[id].char_h = 18;
	strcpy_s(fonts[id].table, characters);

	LOG("Successfully loaded BMP font from %s", texture_path);

	return id;
}

//for (int j = 0; j < rows; j++)
//{
//	for (int i = 0; i < fonts[1].row_chars; i++)
//	{

//		fonts[1].table[i] = characters + sizeof(characters)*i;
//	}
//}

void ModuleFonts::UnLoad(int font_id)
{
	if (font_id >= 0 && font_id < MAX_FONTS && fonts[font_id].graphic != nullptr)
	{
		App->textures->Unload(fonts[font_id].graphic);
		fonts[font_id].graphic = nullptr;
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
}



void ModuleFonts::BlitText(int x, int y, int font_id, const char* text) const
{
	if (text == nullptr || font_id < 0 || font_id >= MAX_FONTS || fonts[font_id].graphic == nullptr)
	{
		LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}

	const Font* font = &fonts[font_id];
	SDL_Rect rect;
	uint len = strlen(text);
	rect.w = font->char_w;
	rect.h = font->char_h;

	for (uint i = 0; i < len; ++i)
	{
		for (uint j = 0; j < font->len; ++j)
		{
			if (font->table[j] == text[i])
			{
				//We blit the letter
				rect.x = j * font->char_w;
				rect.y = 0;
				App->renderer->Blit(font->graphic, x, y, &rect);
				//And we go to the next one
				break;
			}
		}
		x += font->char_w;
	}
}

uint ModuleFonts::Get_CharWidth(int fontId) const
{
	if (fontId < 0 || fontId > MAX_FONTS)
	{
		LOG("Accessing invalid font! SDL_Error: %s\n", SDL_GetError());//This is not a SDL error, but hopefully it will crash if we access an invalid font
	}
	return fonts[fontId].char_w;
}