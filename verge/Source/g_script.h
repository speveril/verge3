#ifndef _G_SCRIPT
#define _G_SCRIPT

// See below.
struct VergeCallback;

extern bool die;
extern VergeCallback renderfunc, timerfunc, maploadfunc;

extern int event_tx;
extern int event_ty;
extern int event_zone;
extern int event_entity;
extern int event_param;
extern int event_sprite;
extern int event_entity_hit;

extern int __grue_actor_index;

extern int invc;

extern std::string _trigger_onStep, _trigger_afterStep;
extern std::string _trigger_beforeEntityScript, _trigger_afterEntityScript;
extern std::string _trigger_onEntityCollide;
extern std::string _trigger_afterPlayerMove;

#define VCFILES				51
#define VC_READ				1
#define VC_WRITE			2
#define VC_WRITE_APPEND		3 // Overkill (2006-07-05): Append mode added.

struct FileRecord
{
	bool active;
	FILE *fptr;
	VFILE *vfptr;
	int mode;
};

extern FileRecord vcfiles[VCFILES];

void InitScriptEngine();
void HookTimer();
void HookRetrace();
void HookMapLoad();
image *ImageForHandle(int handle);
void FreeImageHandle(int handle);
void SetHandleImage(int handle, image *img);
int HandleForImage(image *img);

class ScriptEngine;
extern ScriptEngine *se;

class MapScriptCompiler {
public:
	virtual bool CompileMap(const char *fname) = 0;
};

struct argument_t
{
	byte type_id;
	int int_value;
	std::string string_value;
};

// Callbacks used by Verge's hooks/builtins.
struct VergeCallback
{
	/// For resolving names at runtime
    // (VC/Lua) The name of the function to resolve.
    std::string funcname;
    
	/// For direct references to a callback.
    // (VC) function index for library or user function
    // (Lua) an index to the lua table where this callback is held.
    int functionIndex;
    // (VC only) Either opLIBFUNC or opUSERFUNC.
    int opType;
    // (VC only) the core image the function is on.
    int cimage;

	VergeCallback()
		: funcname(""), functionIndex(-1), opType(0), cimage(0)
	{
	}
};

class ScriptEngine {
public:
	ScriptEngine();

	int vcreturn;
	std::string vcretstr;

	void Error(const char *s, ...);

	virtual bool ExecuteFunctionString(const std::string& script) = 0;
	virtual bool FunctionExists(const std::string& func) = 0;
	virtual void ExecAutoexec() = 0;
	virtual void LoadMapScript(VFILE *f, const std::string& filename) = 0;
	virtual void DisplayError(const std::string& msg) = 0;
	virtual int ResolveOperand() = 0;
	virtual std::string ResolveString() = 0;
	virtual VergeCallback ResolveCallback() = 0; // Get callback from argument list (Lua allocates a reference).
	virtual void ReleaseCallback(VergeCallback& cb) = 0; // Free callback (or else, memory bloat in Lua.).
	virtual void ExecuteCallback(VergeCallback& cb, bool calling_from_library) = 0; // Invoke callback.
	virtual bool CheckForVarargs() = 0;
	virtual void ReadVararg(std::vector<argument_t>& vararg) = 0;
	void vcerr(char *str, ...) {
	  	va_list argptr;
		char msg[256];

		va_start(argptr, str);
		vsprintf(msg, str, argptr);
		va_end(argptr);
		Error("%s",msg);
	}

	std::string ReadHvar_str(int category, int loc, int ofs);
	virtual std::string ReadHvar_str_derived(int category, int loc, int ofs);
	void WriteHvar_str(int category, int loc, int arg, const std::string& value);
	virtual void WriteHvar_str_derived(int category, int loc, int arg, const std::string& value);

	int ReadHvar(int category, int loc, int ofs);
	virtual int ReadHvar_derived(int category, int loc, int ofs);
	void WriteHvar(int category, int loc, int ofs, int value);
	virtual void WriteHvar_derived(int category, int loc, int ofs, int value);

	// Varargs support!
	std::vector<argument_t> argument_pass_list;

	void ArgumentPassAddInt(int value);
	void ArgumentPassAddString(std::string value);
	void ArgumentPassClear();

	//script services
	//VI.a. General Utility Functions
	//TODO(CallFunction);
	static void Exit(const std::string& message);
	static void HookButton(int b, VergeCallback cb);
	static void HookKey(int k, VergeCallback cb);
	void HookTimer(VergeCallback s);
	void HookRetrace(VergeCallback s);
	void HookMapLoad(VergeCallback s);
	void ResizeLayer(int l, int w, int h, int offx, int offy);
	static void Log(const std::string& s);
	static void MessageBox(const std::string& msg);
	static int Random(int min, int max);
	static void SetAppName(const std::string& s);
	static void SetButtonJB(int b, int jb);
	static void SetButtonKey(int b, int k);
	static void SetRandSeed(int seed);
	static void SetResolution(int v3_xres, int v3_yres);
	static void Unpress(int n);
	static void UpdateControls();
	//VI.d. Map Functions
	static int GetObs(int x, int y);
	static int GetObsPixel(int x, int y);
	static int GetTile(int x, int y, int i);
	static int GetZone(int x, int y);
	static void Map(const std::string& map);
	static void Render();
	static void RenderMap(int x, int y, int d);
	static void SetObs(int x, int y, int c);
	static void SetTile(int x, int y, int i, int z);
	static void SetZone(int x, int y, int z);
	//VI.e. Entity Functions
	static void ChangeCHR(int e, const std::string& c);
	static void EntityMove(int e, const std::string& s);
	static void EntitySetWanderDelay(int e, int d);
	static void EntitySetWanderRect(int e, int x1, int y1, int x2, int y2);
	static void EntitySetWanderZone(int e);
	static int EntitySpawn(int x, int y, const std::string& s);
	static void EntityStalk(int stalker, int stalkee);
	static void EntityStop(int e);
	static void HookEntityRender(int i, const std::string& s);
	static void PlayerMove(const std::string& s);
	static void PlayerEntityMoveCleanup();
	static void SetEntitiesPaused(int i);
	static void SetPlayer(int e);
	static int GetPlayer();
	//VI.f. Graphics Functions
	static void AdditiveBlit(int x, int y, int src, int dst);
	static void AlphaBlit(int x, int y, int src, int alpha, int dst);
	static void Blit(int x, int y, int src, int dst);
	static void BlitEntityFrame(int x, int y, int e, int f, int dst);
	static void BlitLucent(int x, int y, int lucent, int src, int dst);
	static void BlitObs(int x, int y, int t, int dst);
	static void BlitTile(int x, int y, int t, int dst);
	static void BlitWrap(int x, int y, int src, int dst);
	static void Circle(int x1, int y1, int xr, int yr, int c, int dst);
	static void CircleFill(int x1, int y1, int xr, int yr, int c, int dst);
	static void ColorFilter(int filter, int dst);
	static void CopyImageToClipboard(int s);
	static int DuplicateImage(int s);
	static void FlipBlit(int x, int y, bool fx, bool fy, int src, int dst);
	static void FreeImage(int handle);
	static int GetB(int c);
	static int GetG(int c);
	static int GetImageFromClipboard();
	static int GetPixel(int x, int y, int src);
	static int GetR(int c);
	static void GrabRegion(int sx1, int sy1, int sx2, int sy2, int dx, int dy, int src, int dst);
	static int ImageHeight(int src);
	static int ImageShell(int x, int y, int w, int h, int src);
	static int ImageValid(int handle);
	static int ImageWidth(int src);
	static void Line(int x1, int y1, int x2, int y2, int c, int dst);
	static int LoadImage(const std::string& fn);
	static int LoadImage0(const std::string& fn);
	static int LoadImage8(const std::string& fn);
	static int MakeColor(int r, int g, int b);
	static int MixColor(int c1, int c2, int p);
	static void Mosaic(int xgran, int ygran, int dst);
	static int NewImage(int xsize, int ysize);
	static void Rect(int x1, int y1, int x2, int y2, int c, int dst);
	static void RectFill(int x1, int y1, int x2, int y2, int c, int dst);
	static int rgb(int r, int g, int b) { return MakeColor(r,g,b); }
	static void RotScale(int x, int y, int angle, int scale, int src, int dst);
	static void ScaleBlit(int x, int y, int dw, int dh, int src, int dst);
	static void SetClip(int x1, int y1, int x2, int y2, int img);
	static void SetCustomColorFilter(int c1, int c2);
	static void SetLucent(int p);
	static void SetPixel(int x, int y, int c, int dst);
	static void ShowPage();
	static void Silhouette(int x, int y, int c, int src, int dst);
	static void SubtractiveBlit(int x, int y, int src, int dst);
	static void SuperSecretThingy(int xskew, int yofs, int y, int src, int dst);
	static void TAdditiveBlit(int x, int y, int src ,int dst);
	static void TBlit(int x, int y, int src, int dst);
	static void TBlitLucent(int x, int y, int lucent, int src, int dst);
	static void TBlitTile(int x, int y, int t, int dst);
	static void TGrabRegion(int sx1, int sy1, int sx2, int sy2, int dx, int dy, int src, int dst);
	static void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int c, int dst);
	static void TScaleBlit(int x, int y, int dw, int dh, int src, int dst);
	static void TSubtractiveBlit(int x, int y, int src, int dst);
	static void TWrapBlit(int x, int y, int src, int dst);
	static void WrapBlit(int x, int y, int src, int dst);
	//VI.g. Sprite Functions
	static int GetSprite();
	static void ResetSprites();
	//VI.h. Sound/Music Functions
	static void FreeSong(int handle);
	static void FreeSound(int slot);
	static int GetSongPos(int handle);
	static int GetSongVolume(int handle);
	static int LoadSong(const std::string& fn);
	static int LoadSound(const std::string& fn);
	static void PlayMusic(const std::string& fn);
	static void PlaySong(int handle);
	static int PlaySound(int slot, int volume);
	static void SetMusicVolume(int v);
	static void SetSongPaused(int h, int p);
	static void SetSongPos(int h, int p);
	static void SetSongVolume(int h, int v);
	static void StopMusic();
	static void StopSong(int handle);
	static void StopSound(int chan);
	//VI.i. Font Functions
	static void EnableVariableWidth(int fh);
	static int FontHeight(int f);
	static void FreeFont(int f);
	static int LoadFont(const std::string& filename, int width, int height);
	static int LoadFontEx(const std::string& filename);
	static void PrintCenter(int x, int y, int d, int fh, const std::string& text);
	static void PrintRight(int x, int y, int d, int fh, const std::string& text);
	static void PrintString(int x, int y, int d, int fh, const std::string& text);
	static int TextWidth(int fh, const std::string& text);
	//VI.j. Math Functions
	static int acos(int val);
	static int facos(int val);
	static int asin(int val);
	static int fasin(int val);
	static int atan(int val);
	static int fatan(int val);
	static int atan2(int y, int x);
	static int fatan2(int y, int x);
	static int cos(int val);
	static int fcos(int val);
	static int pow(int a, int b);
	static int sin(int val);
	static int fsin(int val);
	static int sqrt(int val);
	static int tan(int val);
	static int ftan(int val);
	//VI.k. File Functions
	static void FileClose(int handle);
	static int FileCurrentPos(int handle);
	static bool FileEOF(int handle);
	static int FileOpen(const std::string& fname, int filemode);
	static int FileReadByte(int handle);
	static std::string FileReadln(int handle);
	static int FileReadQuad(int handle);
	static std::string FileReadString(int handle);
	static std::string FileReadToken(int handle);
	static int FileReadWord(int handle);
	static void FileSeekLine(int handle, int line);
	static void FileSeekPos(int handle, int offset, int mode);
	static void FileWrite(int handle, const std::string& s);
	static void FileWriteByte(int handle, int var);
	static void FileWriteln(int handle, const std::string& s);
	static void FileWriteQuad(int handle, int var);
	static void FileWriteString(int handle, const std::string& s);
	static void FileWriteWord(int handle, int var);
	static std::string ListFilePattern(const std::string& pattern);
	static void FileWriteCHR(int handle, int ent);
	static void FileWriteMAP(int handle);
	static void FileWriteVSP(int handle);
	//VI.l. Window Managment Functions
	static void WindowClose(int win);
	static int WindowCreate(int x, int y, int w, int h, const std::string& s);
	static int WindowGetHeight(int win);
	static int WindowGetImage(int win);
	static int WindowGetWidth(int win);
	static int WindowGetXRes(int win);
	static int WindowGetYRes(int win);
	static void WindowHide(int win);
	static void WindowPositionCommand(int win, int command, int arg1, int arg2);
	static void WindowSetPosition(int win, int x, int y);
	static void WindowSetResolution(int win, int x, int y);
	static void WindowSetSize(int win, int x, int y);
	static void WindowSetTitle(int win, const std::string& s);
	static void WindowShow(int win);
	//VI.m. Movie Playback Functions
	static void AbortMovie();
	static void MovieClose(int m);
	static int MovieGetCurrFrame(int m);
	static int MovieGetFramerate(int m);
	static int MovieGetImage(int m);
	static int MovieLoad(const std::string& s, bool mute);
	static void MovieNextFrame(int m);
	static void MoviePlay(int m, bool loop);
	static void MovieRender(int m);
	static void MovieSetFrame(int m, int f);
	static int PlayMovie(const std::string& s);
	//VI.n. Netcode Functions
	static void SetConnectionPort(int port);
	static int Connect(const std::string& ip);
	static int GetConnection();
	static int GetUrlImage(const std::string& url);
	static std::string GetUrlText(const std::string& url);
	static void SocketClose(int sh);
	static bool SocketConnected(int sh);
	static std::string SocketGetFile(int sh, const std::string& override);
	static int SocketGetInt(int sh);
	static std::string SocketGetString(int sh);
	static bool SocketHasData(int sh);
	static void SocketSendFile(int sh, const std::string& fn);
	static void SocketSendInt(int sh, int i);
	static void SocketSendString(int sh, const std::string& str);
	static std::string SocketGetRaw(int sh, int len);
	static void SocketSendRaw(int sh, const std::string& str);
	static int SocketByteCount(int sh); // Overkill (2008-04-20): Peek at how many bytes are in buffer. Requested by ustor.
	//XX: unsorted functions and variables, mostly newly added and undocumented
	static std::string Get_EntityChr(int arg);
	static void Set_EntityChr(int arg, const std::string& chr);
	static int Get_EntityFrameW(int ofs);
	static int Get_EntityFrameH(int ofs);
	static std::string Get_EntityDescription(int arg);
	static void Set_EntityDescription(int arg, const std::string& val);
	static void Set_EntityActivateScript(int arg, const std::string& val);
	static bool SoundIsPlaying(int chn);
	static void RectVGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void RectHGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void RectRGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void Rect4Grad(int x1, int y1, int x2, int y2, int c1, int c2, int c3, int c4, int d);
	static int HSV(int h, int s, int v);
	static int GetH(int col);
	static int GetS(int col);
	static int GetV(int col);
	static void HueReplace(int hue_find, int hue_tolerance, int hue_replace, int image);
	static void ColorReplace(int find, int replace, int image);
	static std::string GetKeyBuffer();
	static void FlushKeyBuffer();
	static void SetKeyDelay(int d);
	static std::string GetEngineString(const std::string& key);
};



#endif