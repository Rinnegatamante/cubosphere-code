/**
Copyright (C) 2010 Chriddo
Copyright (C) 2018 Valeri Ochinski <v19930312@gmail.com>

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/

///DOING: Needs to be changed to SDL_TTF

#include <iostream>
#include <vector>
#include "textures.hpp"
#include "fonts.hpp"
#include "game.hpp"
#include "cuboutils.hpp"
#include "globals.hpp"
#include <sstream>


#ifdef WIN32

#include <windows.h>
#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <SDL.h>
#include <SDL_ttf.h>

#else

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#endif


#define CHAR_SPACE_DIST 0.7

#define FONT_SIZE 40
#define V_DIST_FACTOR 1.2

void TSizedFont::DestructFont()
	{
	if (font) {
			size=0;
			TTF_CloseFont(font);
			}
	}


//Currently not in use!!
int TSizedFont::Load(SDL_RWops *rwops,int fontsize)
	{
	DestructFont();
	SDL_RWseek(rwops,0,SEEK_SET);
	if(!(font = TTF_OpenFontRW(rwops,1, fontsize))) {
			coutlog("Error loading font from RWops",1);
			font=NULL;
			return 0;
			}
	size=fontsize;
	return 1;
	}

int TSizedFont::Load(std::string fontname,int fontsize)
	{
	DestructFont();
	TCuboFile * finfo=GetFileName(fontname,FILE_FONT,".ttf");
	if (!finfo) {
			coutlog("Font "+fontname+".ttf not found!",1);
			return 0;
			}

	if(!(font = TTF_OpenFontRW(finfo->GetAsRWops(1),1, fontsize))) {
			std::ostringstream os;
			os<<"Error loading font: "<<TTF_GetError();
			coutlog(os.str(),1);
			font=NULL;
			delete finfo;
			return 0;
			}
	size=fontsize;
	finfo->DisownData();
	delete finfo;
	return 1;
	}

TSizedFont* TLoadedFont::GetSized(int dessize)
	{
	//First attempt: The font should be the exactly right size   ///Later: add a delta value
	for (unsigned int i=0; i<sized.size(); i++)
			{
			if (sized[i])
					{
					if (sized[i]->GetSize()==dessize) return sized[i];
					}
			}
//Oh, none found: Add it
	TSizedFont *n=new TSizedFont();

	if (!(n->Load(fname,dessize))) return NULL;
	sized.push_back(n);

	return n;
	}

int TLoadedFont::Load(std::string fontname)
	{
	if (fontname==fname) return 1;
	Clear();
	fname=fontname;

	if (!Prepare()) return 0;
	return 1;
	}


int TLoadedFont::Prepare()
	{
	for (int i=0; i<NUM_FONT_SIZES; i++)
		if (!GetSized(g_FontSizes[i])) return 0;

	return 1;
	}


void TLoadedFont::Clear()
	{
	for (unsigned int i=0; i<sized.size(); i++)
			{
			if (sized[i]) delete sized[i];
			sized[i]=NULL;
			}
	fname="";
	sized.resize(0);
	}


TSizedFont *TLoadedFont::GetBestFont(int pixelsize)
	{
	int best=-1;
	int mindev=100000;
	for (unsigned i=0; i<sized.size(); i++)
			{

			int dev=sized[i]->GetSize()-pixelsize;
			if (dev<0) dev=-dev;
			if (dev<mindev) {
					best=i;
					mindev=dev;
					}
			}
	if (best==-1) return NULL;
	return sized[best];
	}


void TFontCache::Clear()
	{
	if (!initialized) return;
//cout << "Clearing Font Cache " << endl;
	glFinish();
	SDL_FreeSurface(surf);
	glDeleteTextures(1, &texture);
	initialized=0;
	}

#ifdef VCPP
double round(double x) {return (double)((int)(x+0.5));}
#endif

int nextpoweroftwo(int x)
	{
	double logbase2 = log((double)x) / log((double)2);
	return (int)round(pow(2,ceil(logbase2)));
	}

void TFontCache::Setup(TLoadedFont *font,std::string text,int size)
	{
	Clear();
	mysize=size;
	myfontname=font->GetName();
	mytext=text;

	SDL_Surface *initial;
	int w,h;


	/* Use SDL_TTF to render our text */
	SDL_Color color;
	color.r=255;
	color.g=255;
	color.b=255;


	TSizedFont *fnt=font->GetBestFont(size);
	fontsize=fnt->GetSize();
	if (!fnt) return;
	initial = TTF_RenderUTF8_Blended(fnt->GetFont(), text.c_str(), color);

	w = nextpoweroftwo(initial->w);
	h = nextpoweroftwo(initial->h);

	surf = SDL_CreateRGBSurface(0, w, h, 32,
			0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_BlitSurface(initial, 0, surf, 0);

	/* Tell GL about our new texture */
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA,
			GL_UNSIGNED_BYTE, surf->pixels );

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


	iw=initial->w;
	ih=initial->h;


	sw=surf->w;
	sh=surf->h;

	SDL_FreeSurface(initial);
	//	SDL_FreeSurface(surf); //I hope this is ok
	initialized=1;
	}


void TFontCaches::Clear()
	{
	for (unsigned int i=0; i<caches.size(); i++)
			{
			if (caches[i]) {
					delete caches[i];
					caches[i]=NULL;
					}
			}
	caches.resize(0);
	}

TFontCache *TFontCaches::GetCache(TLoadedFont *font,std::string text,int size)
	{

	for (unsigned int i=0; i<caches.size(); i++)
			{
			if (caches[i]->IsTheSame(font->GetName(),text,size)) {  caches[i]->SetTime(g_Game()->GetTime()); return caches[i]; }
			}
	//Test if we can add a surf
	int index=caches.size();
	if (index<MAX_FONT_CACHE) {
			TFontCache *n=new TFontCache();
			caches.push_back(n);
			}
	else
			{
			//Oh, no slot left... search the one which is the oldest one
			double MinTime=g_Game()->GetTime();
			index=0;
			for (unsigned int i=0; i<caches.size(); i++)
					{
					if (caches[i]->GetTime()<MinTime)
							{
							index=i;
							MinTime=caches[i]->GetTime();
							}
					}
			}

	caches[index]->Setup(font,text,size);
	caches[index]->SetTime(g_Game()->GetTime());
	return caches[index];


	}

void TFont::RenderText(std::string text)
	{
	TFontCache* c=cache.GetCache(&font,text,(int)(scaley*g_Game()->GetScreenSize().v));

	//glEnable(GL_TEXTURE_2D);
	g_Game()->GetTextures()->EnableTexturing();
	glBindTexture(GL_TEXTURE_2D, c->GetTexture());
	//glColor3f(1.0f, 1.0f, 1.0f);
//	glBegin(GL_QUADS);
	glBegin(GL_TRIANGLE_STRIP);

	float tw=c->iw/g_Game()->GetScreenSize().u;
	float th=c->ih/g_Game()->GetScreenSize().v;

	float tw2=c->sw/g_Game()->GetScreenSize().u;
	float th2=c->sh/g_Game()->GetScreenSize().v;

	float x=xpos;
	float y=ypos;


	float Xscale=scalex*g_Game()->GetScreenSize().u/ (float)c->GetFontSize();
	float Yscale=scaley*g_Game()->GetScreenSize().v/(float)c->GetFontSize();


	tw*=Xscale;
	tw2*=Xscale;
	th*=Yscale;
	th2*=Yscale;

	if (halign=="right") x-=tw;
	else if (halign=="center") x-=tw/2.0;

	glTexCoord2f(0.0, th/th2);
	glVertex3f(x,y,-10);
	glTexCoord2f(tw/tw2, th/th2);
	glVertex3f(x+tw,y,-10);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, y+th,-10);
	glTexCoord2f(tw/tw2, 0.0f);
	glVertex3f(x+tw, y+th,-10);



	glEnd();

	}


void TFont::Init()
	{
	if(TTF_Init())
			{
			std::ostringstream os;
			os <<"FONT ERROR: " << TTF_GetError();
			coutlog(os.str(),1);
			}
	atexit(TTF_Quit);
	cname="";

	}

void TFont::Load(std::string textname)
	{
	if (textname!=cname)
			{
			font.Load(textname);
			cname=textname;
			}
	}


void TFont::Begin()
	{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5,0.5,-0.5,0.5,0,128);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	g_Game()->GetTextures()->DeactivateStage(0); //important for GLSL!!!
//glFinish();
	}

void TFont::End()
	{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	}

void TFont::TextOut(std::string s)
	{

	s=RemapString(s);
	std::vector<std::string> lines;
	Tokenize(s,lines,"\n");

	float hoffs=scaley*(lines.size()-1)*(V_DIST_FACTOR);
	if (valign=="top") ypos-=scaley;
	else if (valign=="center") ypos-=scaley/2.0-hoffs/2.0;
	else if (valign=="bottom") ypos+=hoffs;

	for (unsigned int l=0; l<lines.size(); l++)
			{
			s=lines[l];

			/*        	SDL_Rect position;
			        position.x = (Sint16)xpos;
					position.y = (Sint16)ypos;
			*/
			RenderText(s);

			ypos-=scaley*V_DIST_FACTOR;

			}
	}

std::string TFont::RemapString(std::string s)
	{

	///SENSELESS
	return s;


	size_t pos=0;

	while (1)
			{
			size_t minpos=std::string::npos;
			unsigned int minind=0;
			for (unsigned int r=0; r<remaps.size(); r++)
					{
					size_t p=s.find(remaps[r].oldc,pos);
					if (p!=std::string::npos)
							{
							minpos=p;
							minind=r;
							}
					}
			if (minpos==std::string::npos)
					{
					//Finished
					return s;
					}
//Substitute now and getting a new pos
			s=s.substr(0,minpos)+remaps[minind].newc+s.substr(minpos+remaps[minind].oldc.length());
			pos=minpos+remaps[minind].oldc.length();
			}
	}

void TFont::SetSize(float s)
	{
	float ratio=g_Game()->GetScreenSize().u/g_Game()->GetScreenSize().v;
	if (ratio<=640.0 / 480.0)
		scalex=s;
	else
		scalex=s*(640.0 / 480.0)/ratio;
	scaley=s;

	}


void TFont::ClearCache()
	{
	cache.Clear();
	}


void TFont::StopFontEngine()
	{
	ClearCache();
	ClearRemaps();
	font.Clear();
	}

void TFont::ClearRemaps()
	{
	remaps.resize(0);
	}

void TFont::AddRemap(std::string oldc,std::string newc)
	{
	TFontRemap nr;
	nr.oldc=oldc;
	nr.newc=newc;
	remaps.push_back(nr);
	}



////////////////////LUA-ACCESS//////////////////7


int FONT_Begin(lua_State *state)
	{
	g_Game()->GetFont()->Begin();
	return 0;
	}

int FONT_ClearRemaps(lua_State *state)
	{
	g_Game()->GetFont()->ClearRemaps();
	return 0;
	}

int FONT_AddRemap(lua_State *state)
	{
	std::string to=LUA_GET_STRING;
	std::string from=LUA_GET_STRING;
	g_Game()->GetFont()->AddRemap(from,to);
	return 0;
	}

int FONT_End(lua_State *state)
	{
	g_Game()->GetFont()->End();
	return 0;
	}

int FONT_SetSize(lua_State *state)
	{
	float size=LUA_GET_DOUBLE;
	g_Game()->GetFont()->SetSize(size);
	return 0;
	}

int FONT_Goto(lua_State *state)
	{
	float y=LUA_GET_DOUBLE;
	float x=LUA_GET_DOUBLE;
	g_Game()->GetFont()->Goto(x,y);
	return 0;
	}

int FONT_Load(lua_State *state)
	{
	std::string n=LUA_GET_STRING;

	g_Game()->GetFont()->Load(n);
	return 0;
	}

int FONT_SetAlign(lua_State *state)
	{
	std::string v=LUA_GET_STRING;
	std::string h=LUA_GET_STRING;
	g_Game()->GetFont()->SetAlign(h,v);
	return 0;
	}

int FONT_TextOut(lua_State *state)
	{
	std::string n=LUA_GET_STRING;
	g_Game()->GetFont()->TextOut(n);
	return 0;
	}





void LUA_FONT_RegisterLib()
	{
	g_CuboLib()->AddFunc("FONT_Begin",FONT_Begin);
	g_CuboLib()->AddFunc("FONT_End",FONT_End);
	g_CuboLib()->AddFunc("FONT_Load",FONT_Load);
	g_CuboLib()->AddFunc("FONT_SetSize",FONT_SetSize);
	g_CuboLib()->AddFunc("FONT_Goto",FONT_Goto);
	g_CuboLib()->AddFunc("FONT_TextOut",FONT_TextOut);
	g_CuboLib()->AddFunc("FONT_SetAlign",FONT_SetAlign);
	g_CuboLib()->AddFunc("FONT_AddRemap",FONT_AddRemap);
	g_CuboLib()->AddFunc("FONT_ClearRemaps",FONT_ClearRemaps);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
