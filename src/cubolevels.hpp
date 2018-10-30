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

#ifndef CUBO_LEVELS_H_G
#define CUBO_LEVELS_H_G

#include "cuboboxes.hpp"
#include <vector>
#include <iostream>
#include <string>
#include "cuboutils.hpp"
#include "c3dobjects.hpp"
#include "definitions.hpp"
#include "textures.hpp"

#define DIST_RENDER_SIDE 0
#define DIST_RENDER_ITEM 1
#define DIST_RENDER_ACTOR 2
#define DIST_RENDER_BLOCK 3
#define DIST_RENDER_CUSTOM 4


typedef struct
	{
	int hit;
	tfloat dist;
	int block;
	int side;
	} TTraceResult;

class TDistRenderObj
	{
	protected:
		float dist;
		int id; //The id to the object
		int type; //Blockside, Item etc.
		std::string myhint;
		lua_State *callstate;
		double CullRadius;
		T3dVector CullCenter;

	public:
		TDistRenderObj(int tid,int ttype,float tdist,lua_State *ccallstate) : dist(tdist), id(tid), type(ttype), callstate(ccallstate), CullRadius(-1) {};
		const float GetDist() const {return dist;}
		const int GetID() const {return id;}
		const int GetType() const {return type;}
		const std::string GetHint() const {return myhint;}
		void SetHint(std::string h) {myhint=h;}
		void SetCulling(T3dVector cc,double cr) {CullCenter=cc; CullRadius=cr;}
		bool operator < (TDistRenderObj b ) const {return dist > b.dist;}
		void DistRender(TCamera *cam);
		void Render(TCamera *cam);
		void SpecialRender(TCamera *cam,std::string nam,int defrender);
	};

class TCuboLevel
	{
	protected:
		std::vector<TCuboBlock*> blocks;
		std::vector<TCuboItem*> items;
		std::vector<TDistRenderObj> distrenderlist; //Used for Blending stuff
		std::vector<TDistRenderObj> rlist;
		T3dVector bbmax,bbmin; //Sizes of the bounding box
//    TTextFileReader tr;
		TLuaAccess lua;
		TBlockDefServer bdefs;
		TTextureDefServer tdefs;
		TItemDefServer idefs;
		std::string errorstring;
		std::string filename;
		float timescale;
		float leveltime;
		std::string lastrenderedtype;
		std::string nextlevel;
		int loadnextlevel;
		int nextleveluserlevel;

		int errorline;
		//  bool ReadBlock();
		void BindBlocksToNext();
		int docollisions;
	public:
		TBlockDefServer * GetBlockDefs() {return  &bdefs;}
		TTextureDefServer *GetTexDefs() {return  &tdefs;}
		TItemDefServer *GetItemDefs() {return  &idefs;}
		std::string GetLastRendered() const {return lastrenderedtype;}
		void SetLastRendered(const std::string &s) {lastrenderedtype=s;}
		TCuboBlock* LastBlock();
		void clear();
		void clearAll(); ///TODO: add a function to clear all the data between episodes
		void CreateBBox();
		void Reload();
		//  bool LoadFromFile(string fname);
		void SetNewLevel(std::string nfname,int ulevel) {nextlevel=nfname; loadnextlevel=1; if (ulevel>=0) nextleveluserlevel=ulevel;}
		bool LoadFromLua(std::string fname);
		void Think();
		void CheckCollides();
		void PostThink();
		void Render(TCamera *cam);
		void SpecialRender(TCamera *cam,std::string nam,int defrender);
		void DistRender(TCamera *cam);
		void LastDistanceRenderCull(T3dVector center,double rad);
		void SpecialDistRender(std::string nam,int defrender);
		void FrameRenderStart();
		void FrameRenderEnd();
		void DrawHUD();
		T3dVector GetCenter();
		tfloat GetRadius();
		int LoadTexDef(std::string name);
		TBlockDef *GetBlockDef(int i) {return bdefs.GetDefPtr(i);}
		TTextureDef *GetTexDef(int i) {return tdefs.GetDefPtr(i);}
		TItemDef *GetItemDef(int i) {return idefs.GetDefPtr(i);}
		TCuboItem *GetItem(int i) {return items[i];}
		TCuboBlock *GetBlockAtIPos(int x,int y,int z);
		TCuboBlock *GetBlockAtPos(T3dVector p);
		TCuboBlock *GetBlock(int i);
		TCuboBlock *GetBlockFromType(std::string name,int i);
		TTraceResult TraceLine(T3dVector start,T3dVector dir, int onlyblocking=0);
		TCuboBlockSide *GetBlockSide(int id);
		std::string GetErrorString();
		void AddDistRenderItem(int id,int type,float dist,lua_State *callstate);
		void AddBlock(int x,int y,int z,std::string bdefname);
		void DeleteBlock(int i);
		void ChangeSide(int block, std::string sidestr,std::string sdefname);
		int AddItem(int block, std::string sidestr,std::string idefname);
		void LoadSky(std::string skyname);
		int NumItemsOfType(std::string itemname);
		int PointInBBox(T3dVector cp,float extrasize=40*CUBO_SCALE);
		std::string GetLevelFileName() {return filename;}
		void SendKey(int key,int down,int toggle);
		void SendJoyButton(int joy,int button,int dir,int down,int toggle);
		void JoyAxisChange(int joys,int axis,double val,double pval);
		double Elapsed();
		double Time() {return leveltime;}
		void SetTimeScale(double ts) {timescale=ts;}
		double GetTimeScale() {return timescale;}
		void ChangeBlock(int block, std::string sdefname);
		void WriteLevelData(FILE *f);
		void RemoveItemFromSide(TCuboBlockSide *s);
		int GetNumBlocks() {return blocks.size();}
		TCuboItem *GetItemOnSide(TCuboBlockSide *s);
		int GetSideOfType(std::string tname,int startside,int offs);
		void SortDistRenderList();
		void SetCollisionChecksActive(int i) {docollisions=i;}
		int GetCollisionChecksActive() {return docollisions;}
		std::string CheckDefExchange(std::string defname,std::string deftype);
	};

extern void LUA_LEVEL_RegisterLib();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
