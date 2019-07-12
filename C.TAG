ÄÄ´ RESOURCE.CPP  ÃÄÄÄÄÄÄ´ 10-01-97 02:50pm ÃÄÄÄÄÄÄÄþDT=591492681
LoadResource	function	void *LoadResource(char *name, int id, DWORD *ressize)
LoadResourceHeader	function	void *LoadResourceHeader(char *name, int id, DWORD *ressize)
SaveResource	function	BOOL SaveResource(char *name, void *ptr, int id)
SaveResourceHeader	function	BOOL SaveResourceHeader(char *name, void *header, int hdrsize, int id)
                                                            þ
ÄÄ´ MAPPANE.CPP   ÃÄÄÄÄÄÄ´ 09-26-97 05:53pm ÃÄÄÄÄÄÄÄþDT=591040186
GRIDZOFF	#define	#define GRIDZOFF	334
SCROLLEDGESIDES	#define	#define SCROLLEDGESIDES		32
SCROLLEDGETOP	#define	#define SCROLLEDGETOP		32
SCROLLEDGEBOTTOM	#define	#define SCROLLEDGEBOTTOM	32
CHARACTER_HEIGHT	#define	#define CHARACTER_HEIGHT	32
MAPGRIDWIDTH	#define	#define MAPGRIDWIDTH		(MAPPANEWIDTH - (SCROLLEDGESIDES * 2))
MAPGRIDHEIGHT	#define	#define MAPGRIDHEIGHT		(MAPPANEHEIGHT - SCROLLEDGETOP - SCROLLEDGEBOTTOM)
ScreenGrid	function	inline int ScreenGrid(int n, int s)
TMapPane.Initialize	function	BOOL TMapPane::Initialize()
TMapPane.Close	function	void TMapPane::Close()
LEFT	#define	#define LEFT		0
RIGHT	#define	#define RIGHT		1
UP	#define	#define UP			2
DOWN	#define	#define DOWN		3
UP_LEFT	#define	#define UP_LEFT		4
UP_RIGHT	#define	#define UP_RIGHT	5
DOWN_LEFT	#define	#define DOWN_LEFT	6
DOWN_RIGHT	#define	#define DOWN_RIGHT	7
NUMKEYS	#define	#define NUMKEYS		8
=	struct	struct { int x, y; } KeyDirs[NUMKEYS] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }, { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 } }
UpdateControl	function	void UpdateControl(int dir, BOOL down)
TMapPane.KeyPress	function	void TMapPane::KeyPress(int key, BOOL down)
TMapPane.FindClickPos	function	void TMapPane::FindClickPos(int x, int y, RS3DPoint start, RS3DPoint target)
TMapPane.MouseClick	function	void TMapPane::MouseClick(int button, int x, int y)
MOUSESCROLLSPEED	#define	#define MOUSESCROLLSPEED	8
TMapPane.MouseMove	function	void TMapPane::MouseMove(int button, int x, int y)
TMapPane.MoveObjScreen	function	void TMapPane::MoveObjScreen(int index, int x, int y)
TMapPane.SetMapPos	function	void TMapPane::SetMapPos(RS3DPoint newpos)
TMapPane.NewObject	function	int TMapPane::NewObject(PSObjectDef def)
TMapPane.AddObject	function	int TMapPane::AddObject(PTObjectInstance oi)
TMapPane.AddObjectSector	function	int TMapPane::AddObjectSector(PTObjectInstance oi, PTSector sect, int sectindex)
TMapPane.RemoveObject	function	PTObjectInstance TMapPane::RemoveObject(int index)
TMapPane.AddShadow	function	int TMapPane::AddShadow(PTObjectInstance oi)
TMapPane.FindObject	function	int TMapPane::FindObject(char *name, int occurance)
TMapPane.FindClosestLights	function	void TMapPane::FindClosestLights(S3DPoint center)
TMapPane.FindClosestObject	function	int TMapPane::FindClosestObject(char *name, S3DPoint frompos, BOOL partial)
TMapPane.FindClosestObject	function	int TMapPane::FindClosestObject(char *name, PTObjectInstance from, BOOL partial)
TMapPane.FindObjectsInRange	function	int TMapPane::FindObjectsInRange(S3DPoint pos, int *array, int width, int height, int objclass, int maxnum)
TMapPane.MakeIndex	function	int TMapPane::MakeIndex(PTObjectInstance oi, int itemnum)
TMapPane.OnObject	function	int TMapPane::OnObject(int screenx, int screeny)
TMapPane.GetInstance	function	PTObjectInstance TMapPane::GetInstance(int index)
TMapPane.WalkmapHandler	function	void TMapPane::WalkmapHandler(PTObjectInstance oi, int mode)
TMapPane.RedrawWalkmapRect	function	void TMapPane::RedrawWalkmapRect(PTObjectInstance oi, int x, int y, int w, int l, PTSector dsect)
TMapPane.GetWalkHeight	function	int TMapPane::GetWalkHeight(RS3DPoint pos)
TMapPane.TransferAllWalkmaps	function	void TMapPane::TransferAllWalkmaps()
TMapPane.SetOwners	function	void TMapPane::SetOwners(PTSector sect)
TMapPane.CanMove	function	BOOL TMapPane::CanMove(RS3DPoint pos, RS3DPoint to, SRect rect, PS3DPoint obst)
TMapPane.LineOfSight	function	BOOL TMapPane::LineOfSight(RS3DPoint pos, RS3DPoint to, PS3DPoint obst)
TMapPane.CheckRect	function	BOOL TMapPane::CheckRect(SRect rect, int height, int facing)
TMapPane.DrawDLight	function	void TMapPane::DrawDLight()
TMapPane.SetAmbientLight	function	void TMapPane::SetAmbientLight(int hnormal, int vnormal, int intensityval, int ambientval)
TMapPane.Update3DScenePos	function	void TMapPane::Update3DScenePos()
TMapPane.DrawBackground	function	void TMapPane::DrawBackground()
TMapPane.UpdateEdges	function	void TMapPane::UpdateEdges(int updatex, int updatey)
TMapPane.ForceDrawEdges	function	void TMapPane::ForceDrawEdges(int updatex, int updatey, DWORD UpdateRect)
TMapPane.QueueEdges	function	void TMapPane::QueueEdges(int updatex, int updatey)
TMapPane.UpdateQueue	function	void TMapPane::UpdateQueue(BOOL tick)
TMapPane.RedrawAll	function	void TMapPane::RedrawAll()
TMapPane.AddBgUpdateRect	function	void TMapPane::AddBgUpdateRect(SRect &r, int bgdraw)
TMapPane.AddObjectUpdateRect	function	void TMapPane::AddObjectUpdateRect(int index)
TMapPane.ReloadImagery	function	void TMapPane::ReloadImagery()
TMapPane.DrawBackgroundRect	function	void TMapPane::DrawBackgroundRect(RSRect r, int bgdraw)
TMapPane.DrawUnlitObjects	function	void TMapPane::DrawUnlitObjects(SRect &r)
TMapPane.PulseObjects	function	void TMapPane::PulseObjects()
TMapPane.AnimateObjects	function	void TMapPane::AnimateObjects(BOOL draw)
TMapPane.DrawStaticLights	function	void TMapPane::DrawStaticLights(SRect &r)
TMapPane.DrawLitObjects	function	void TMapPane::DrawLitObjects(SRect &r)
TMapPane.DrawSelectedObjects	function	void TMapPane::DrawSelectedObjects(SRect &r)
TMapPane.AnimateSelectedObjects	function	void TMapPane::AnimateSelectedObjects()
TMapPane.SaveAllSectors	function	void TMapPane::SaveAllSectors()
TMapPane.FreeAllSectors	function	void TMapPane::FreeAllSectors()
TMapPane.ReloadSectors	function	void TMapPane::ReloadSectors()
TMapPane.UpdateSectors	function	void TMapPane::UpdateSectors()
LEASHX	#define	#define LEASHX		120
MINDISTX	#define	#define MINDISTX	20
LEASHY	#define	#define LEASHY		80
MINDISTY	#define	#define MINDISTY	10
LEASHZ	#define	#define LEASHZ		60
MINDISTZ	#define	#define MINDISTZ	10
leash_update	function	void leash_update(int c, int p, int &n, int d, int l)
TMapPane.Animate	function	void TMapPane::Animate(BOOL draw)
TMapPane.AddToInventory	function	void TMapPane::AddToInventory(PTObjectInstance oi, int item, int slot)
TMapPane.FindFreeInvSlot	function	int TMapPane::FindFreeInvSlot(PTObjectInstance oi)
TMapPane.FindObjInventory	function	PTObjectInstance TMapPane::FindObjInventory(PTObjectInstance oi, char *name)
TMapPane.InventoryItem	function	PTObjectInstance TMapPane::InventoryItem(PTObjectInstance oi, int slot)
TMapPane.RemoveFromInventory	function	PTObjectInstance TMapPane::RemoveFromInventory(PTObjectInstance oi, int slot)
TMapPane.GetOwner	function	PTObjectInstance TMapPane::GetOwner(PTObjectInstance oi)
TMapPane.CheckPos	function	int TMapPane::CheckPos(PTObjectInstance inst, RS3DPoint newpos)
TMapPane.TransferObject	function	int TMapPane::TransferObject(int itemnum, int sx, int sy, int newsx, int newsy)
TMapPane.CalculateWalkmap	function	void TMapPane::CalculateWalkmap()
TMapPane.AdjustWalkmap	function	void TMapPane::AdjustWalkmap(int deltaz, BOOL absolute, BOOL nonzero)
TMapPane.SetMode	function	void TMapPane::SetMode(void (*tfunc)(S3DPoint), void (*afunc)(), int newmode)
                                                            þ
ÄÄ´ DISPLAY.CPP   ÃÄÄÄÄÄÄ´ 09-10-97 07:44pm ÃÄÄÄÄÄÄÄþDT=589995420
TRY_DD	#define	#define TRY_DD(exp) { { HRESULT rval = exp; if (rval != DD_OK) { TraceErrorDD(rval, __FILE__, __LINE__); return FALSE; } } }
TRY_D3D	#define	#define TRY_D3D(exp) { { HRESULT rval = exp; if (rval != D3D_OK) { TraceErrorD3D(rval, __FILE__, __LINE__); return FALSE; } } }
TRY_D3DRM	#define	#define TRY_D3DRM(exp) { { HRESULT rval = exp; if (rval != D3DRM_OK) { TraceErrorD3DRM(rval, __FILE__, __LINE__); return FALSE; 
TDisplay.TDisplay	function	TDisplay::TDisplay()
TDisplay.Initialize	function	BOOL TDisplay::Initialize(int dwidth, int dheight, int dbitsperpixel)
TDisplay.Close	function	BOOL TDisplay::Close()
TDisplay.~TDisplay	function	TDisplay::~TDisplay()
TDisplay.FlipPage	function	BOOL TDisplay::FlipPage(BOOL Wait)
TDisplay.Rect	function	BOOL TDisplay::Rect(int x, int y, int w, int h, SColor &color)
TDisplay.Box	function	BOOL TDisplay::Box(int x, int y, int w, int h, SColor &color)
TDisplay.Line	function	BOOL TDisplay::Line(int x1, int y1, int x2, int y2, SColor &color)
updaterect	typedef	{
MAXRESTORERECTS	#define	#define MAXRESTORERECTS 50
RestoreBuf	struct	{
TDisplay.InitBackgroundSystem	function	BOOL TDisplay::InitBackgroundSystem()
TDisplay.CloseBackgroundSystem	function	BOOL TDisplay::CloseBackgroundSystem()
TDisplay.ClearBackgroundAreas	function	BOOL TDisplay::ClearBackgroundAreas()
TDisplay.CreateBackgroundArea	function	int TDisplay::CreateBackgroundArea(int x, int y, int width, int height, BOOL createzbuf, int vsflags)
TDisplay.UseBackgroundArea	function	int TDisplay::UseBackgroundArea(int x, int y, int width, int height, PTSurface surface)
TDisplay.RestoreBackgroundAreas	function	BOOL TDisplay::RestoreBackgroundAreas()
TDisplay.DrawRestoreRect	function	BOOL TDisplay::DrawRestoreRect(int index, int x, int y, int width, int height, DWORD drawmode)
TDisplay.ScrollBackground	function	BOOL TDisplay::ScrollBackground(int index, int originx, int originy)
MERGELIMITX	#define	#define MERGELIMITX    10
MERGELIMITY	#define	#define MERGELIMITY    10
MARKMERGED	#define	#define MARKMERGED(r)  r.x = -10000
RECTMERGED	#define	#define RECTMERGED(r) (r.x == -10000)
TDisplay.AddUpdateRect	function	void TDisplay::AddUpdateRect(int x, int y, int width, int height, int flags)
AddUpdateCallback	function	void AddUpdateCallback(PSDrawBlock db, PSDrawParam dp)
TDisplay.GetDrawCallBack	function	DRAWCALLBACK TDisplay::GetDrawCallBack()
TDisplay.AddSubRect	function	inline void TDisplay::AddSubRect(int index, int x1, int y1, int x2, int y2, int flags)
TDisplay.AddBackgroundUpdateRect	function	BOOL TDisplay::AddBackgroundUpdateRect(int index, int x, int y, int width, int height,
                                                            þ
ÄÄ´ SCREEN.CPP    ÃÄÄÄÄÄÄ´ 09-10-97 11:12am ÃÄÄÄÄÄÄÄþDT=589978008
TScreen.TScreen	function	TScreen::TScreen()
TScreen.~TScreen	function	TScreen::~TScreen()
TScreen.BeginScreen	function	BOOL TScreen::BeginScreen()
TScreen.EndScreen	function	void TScreen::EndScreen()
TScreen.FindPane	function	int TScreen::FindPane(PTPane pane)
TScreen.AddPane	function	int TScreen::AddPane(PTPane pane, int panenum)
TScreen.RemovePane	function	PTPane TScreen::RemovePane(int panenum)
TScreen.SetExclusivePane	function	BOOL TScreen::SetExclusivePane(int panenum, BOOL completeexclusion)
TScreen.ReleaseExclusivePane	function	void TScreen::ReleaseExclusivePane(int panenum)
TScreen.RedrawAllPanes	function	void TScreen::RedrawAllPanes()
TScreen.DrawBackground	function	void TScreen::DrawBackground()
TScreen.Animate	function	void TScreen::Animate(BOOL draw)
TScreen.MouseClick	function	void TScreen::MouseClick(int button, int x, int y)
TScreen.MouseMove	function	void TScreen::MouseMove(int button, int x, int y)
TScreen.KeyPress	function	void TScreen::KeyPress(int key, BOOL down)
TScreen.CharPress	function	void TScreen::CharPress(int key, BOOL down)
TScreen.Joystick	function	void TScreen::Joystick(int button, int x, int y)
TScreen.Command	function	void TScreen::Command(int command)
TScreen.ShowScreen	function	PTScreen TScreen::ShowScreen(PTScreen screen, int ticks)
TScreen.TimerTick	function	BOOL TScreen::TimerTick(BOOL draw)
TScreen.TimerLoop	function	BOOL TScreen::TimerLoop(long ticks)
TPane.Initialize	function	BOOL TPane::Initialize()
TPane.SetClipRect	function	void TPane::SetClipRect()
TPane.UpdateBackgroundScrollPos	function	void TPane::UpdateBackgroundScrollPos()
TPane.DrawRestoreRect	function	void TPane::DrawRestoreRect(int x, int y, int width, int height, DWORD drawmode)
                                                            þ
ÄÄ´ PLAYSC~1.CPP  ÃÄÄÄÄÄÄ´ 10-03-97 12:27pm ÃÄÄÄÄÄÄÄþDT=591618928
NUMOVERHANGS	#define	#define NUMOVERHANGS	17
NUMMAINHANGS	#define	#define NUMMAINHANGS	13
STARTMULTIHANGS	#define	#define STARTMULTIHANGS	NUMMAINHANGS
ENDMULTIHANGS	#define	#define ENDMULTIHANGS	NUMOVERHANGS
=	struct	{ { 32, 16, 46, 9 }, { 32, 25, 9, 37 }, { 278, 16, 84, 30 },
TPlayScreen.TPlayScreen	function	TPlayScreen::TPlayScreen()
TPlayScreen.Initialize	function	BOOL TPlayScreen::Initialize()
TPlayScreen.Close	function	void TPlayScreen::Close()
TPlayScreen.DrawBackground	function	void TPlayScreen::DrawBackground()
TPlayScreen.Animate	function	void TPlayScreen::Animate(BOOL draw)
TPlayScreen.KeyPress	function	void TPlayScreen::KeyPress(int key, BOOL down)
TPlayScreen.DrawOverhangs	function	void TPlayScreen::DrawOverhangs()
TPlayScreen.LoadEnvironment	function	BOOL TPlayScreen::LoadEnvironment(char *name, BOOL loadpos)
TPlayScreen.SaveEnvironment	function	BOOL TPlayScreen::SaveEnvironment(char *name)
                                                            þ
ÄÄ´ OBJECT.CPP    ÃÄÄÄÄÄÄ´ 09-24-97 09:15am ÃÄÄÄÄÄÄÄþDT=590891511
WorldToScreen	function	void WorldToScreen(RS3DPoint pos, int &x, int &y)
WorldToScreen	function	void WorldToScreen(RS3DPoint pos, RS3DPoint spos)
ScreenToWorld	function	void ScreenToWorld(int x, int y, RS3DPoint pos)
ConvertToVector	function	void ConvertToVector(int angle, int speed, RS3DPoint vect)
ConvertToFacing	function	int ConvertToFacing(RS3DPoint pos, RS3DPoint target)
TObjectBuilder.TObjectBuilder	function	TObjectBuilder::TObjectBuilder(char *name)
TObjectBuilder.GetBuilder	function	PTObjectBuilder TObjectBuilder::GetBuilder(int objtype)
TObjectBuilder.GetBuilder	function	PTObjectBuilder TObjectBuilder::GetBuilder(char *name)
TObjectInstance.ClearObject	function	void TObjectInstance::ClearObject()
TObjectInstance.TObjectInstance	function	TObjectInstance::TObjectInstance(PTObjectImagery img)
TObjectInstance.TObjectInstance	function	TObjectInstance::TObjectInstance(PSObjectDef def, PTObjectImagery img)
TObjectInstance.~TObjectInstance	function	TObjectInstance::~TObjectInstance()
TObjectInstance.GetImagery	function	PTObjectImagery TObjectInstance::GetImagery()
TObjectInstance.CreateAnimator	function	BOOL TObjectInstance::CreateAnimator()
TObjectInstance.FreeAnimator	function	void TObjectInstance::FreeAnimator()
TObjectInstance.NeedsAnimator	function	BOOL TObjectInstance::NeedsAnimator()
TObjectInstance.Damage	function	void TObjectInstance::Damage(int damage)
TObjectInstance.RepaintObject	function	void TObjectInstance::RepaintObject()
TObjectInstance.SetPos	function	int TObjectInstance::SetPos(RS3DPoint newpos)
TObjectInstance.InRange	function	BOOL TObjectInstance::InRange(S3DPoint rangepos)
TObjectInstance.SetState	function	BOOL TObjectInstance::SetState(int newstate)
TObjectInstance.PlaceInInventory	function	int TObjectInstance::PlaceInInventory(int item)
TObjectInstance.SendToInventory	function	void TObjectInstance::SendToInventory(int nextcontent, int slot)
TObjectInstance.AddToInventory	function	void TObjectInstance::AddToInventory(PTObjectInstance oi)
TObjectInstance.RemoveFromInventory	function	void TObjectInstance::RemoveFromInventory(PTObjectInstance oi)
TObjectInstance.Use	function	BOOL TObjectInstance::Use(int with)
TObjectInstance.Face	function	void TObjectInstance::Face(int newfacing)
TObjectInstance.Pulse	function	void TObjectInstance::Pulse()
TObjectInstance.Move	function	DWORD TObjectInstance::Move()
TObjectInstance.Animate	function	void TObjectInstance::Animate(BOOL draw)
TObjectInstance.AI	function	void TObjectInstance::AI()
TObjectInstance.OnScreen	function	void TObjectInstance::OnScreen()
TObjectInstance.OffScreen	function	void TObjectInstance::OffScreen()
TObjectInstance.FreeLight	function	void TObjectInstance::FreeLight()
TObjectInstance.RedrawBackground	function	void TObjectInstance::RedrawBackground(int bgdraw)
TObjectInstance.InitScript	function	void TObjectInstance::InitScript(PTScript newscr)
TObjectInstance.ContinueScript	function	void TObjectInstance::ContinueScript()
TObjectInstance.RedrawLight	function	void TObjectInstance::RedrawLight()
TObjectInstance.GetLightRect	function	void TObjectInstance::GetLightRect(RSRect r)
TObjectInstance.SetLightIntensity	function	void TObjectInstance::SetLightIntensity(BYTE newintensity)
TObjectInstance.SetLightMultiplier	function	void TObjectInstance::SetLightMultiplier(int mult)
TObjectInstance.SetLightPos	function	void TObjectInstance::SetLightPos(RS3DPoint newpos)
TObjectInstance.SetLightColor	function	void TObjectInstance::SetLightColor(SColor color)
TObjectInstance.DrawLight	function	void TObjectInstance::DrawLight(PTSurface surface, BOOL resetid)
TObjectInstance.SetCommandDone	function	void TObjectInstance::SetCommandDone(BOOL newcmd)
TObjectInstance.Load	function	void TObjectInstance::Load(RTInputStream is)
TObjectInstance.Save	function	void TObjectInstance::Save(RTOutputStream os)
TObjectClass.TObjectClass	function	TObjectClass::TObjectClass(char *classname, int classid, WORD flags)
TObjectClass.AddType	function	int TObjectClass::AddType(RSObjectInfo inf)
TObjectClass.NewObject	function	PTObjectInstance TObjectClass::NewObject(PSObjectDef objectdef)
TObjectClass.FindClass	function	int TObjectClass::FindClass(char *name)
TObjectClass.FindObjType	function	int TObjectClass::FindObjType(char *objtypename, BOOL partial)
TObjectClass.FindStatistic	function	int TObjectClass::FindStatistic(char *statname)
TObjectClass.FindStatisticVal	function	int TObjectClass::FindStatisticVal(int statid, int searchvalue)
TObjectClass.FindRandStatisticVal	function	int TObjectClass::FindRandStatisticVal(int statid, int searchvalue, int *heightflux)
TObjectClass.LoadClasses	function	BOOL TObjectClass::LoadClasses(BOOL lock)
TObjectClass.SaveClasses	function	BOOL TObjectClass::SaveClasses(BOOL lock)
TObjectClass.WriteClass	function	BOOL TObjectClass::WriteClass(FILE *fp)
TObjectClass.ParseClass	function	BOOL TObjectClass::ParseClass(TToken &t)
                                                            þ
ÄÄ´ GRAPHICS.CPP  ÃÄÄÄÄÄÄ´ 10-02-97 11:37am ÃÄÄÄÄÄÄÄþDT=591551649
Draw	function	BOOL Draw(PSDrawBlock db, PSDrawParam dp, DRAWCALLBACK callback)
Clip	function	BOOL Clip(PSDrawBlock db, PSDrawParam dp, PSDrawParam dparray, int &numrects)
StretchClip	function	BOOL StretchClip(PSDrawBlock db, PSDrawParam dp, PSDrawParam dparray,
Put	function	BOOL Put(PSDrawBlock db, PSDrawParam dp)
Put8	function	BOOL Put8(PSDrawBlock db, PSDrawParam dp)
Put88	function	BOOL Put88(PSDrawBlock db, PSDrawParam dp)
Put816	function	BOOL Put816(PSDrawBlock db, PSDrawParam dp)
Put32	function	BOOL Put32(PSDrawBlock db, PSDrawParam dp)
ShutterPut	function	BOOL ShutterPut(PSDrawBlock db, PSDrawParam dp)
ShutterPut8	function	BOOL ShutterPut8(PSDrawBlock db, PSDrawParam dp)
ShutterPut816	function	BOOL ShutterPut816(PSDrawBlock db, PSDrawParam dp)
ShutterPut32	function	BOOL ShutterPut32(PSDrawBlock db, PSDrawParam dp)
PutChunk8	function	BOOL PutChunk8(PSDrawBlock db, PSDrawParam dp)
TransPut	function	BOOL TransPut(PSDrawBlock db, PSDrawParam dp)
TransPut8	function	BOOL TransPut8(PSDrawBlock db, PSDrawParam dp)
TransZStaticPut8	function	BOOL TransZStaticPut8(PSDrawBlock db, PSDrawParam dp)
TransPut816	function	BOOL TransPut816(PSDrawBlock db, PSDrawParam dp)
TransPut88	function	BOOL TransPut88(PSDrawBlock db, PSDrawParam dp)
TransPutColor	function	BOOL TransPutColor(PSDrawBlock db, PSDrawParam dp)
TransPutSVChange	function	BOOL TransPutSVChange(PSDrawBlock db, PSDrawParam dp)
PutHueChange	function	BOOL PutHueChange(PSDrawBlock db, PSDrawParam dp)
TransPutKey	function	BOOL TransPutKey(PSDrawBlock db, PSDrawParam dp)
TransPut32	function	BOOL TransPut32(PSDrawBlock db, PSDrawParam dp)
Stretch	function	BOOL Stretch(PSDrawBlock db, PSDrawParam dp)
TransStretch	function	BOOL TransStretch(PSDrawBlock db, PSDrawParam dp)
Mask	function	BOOL Mask(PSDrawBlock db, PSDrawParam dp)
MaskStretch	function	BOOL MaskStretch(PSDrawBlock db, PSDrawParam dp)
Translucent	function	BOOL Translucent(PSDrawBlock db, PSDrawParam dp)
TransTranslucent	function	BOOL TransTranslucent(PSDrawBlock db, PSDrawParam dp)
TranslucentStretch	function	BOOL TranslucentStretch(PSDrawBlock db, PSDrawParam dp)
AlphaLighten	function	BOOL AlphaLighten(PSDrawBlock db, PSDrawParam dp)
Alpha	function	BOOL Alpha(PSDrawBlock db, PSDrawParam dp)
Alpha8	function	BOOL Alpha8(PSDrawBlock db, PSDrawParam dp)
AlphaZ	function	BOOL AlphaZ(PSDrawBlock db, PSDrawParam dp)
AlphaZ8	function	BOOL AlphaZ8(PSDrawBlock db, PSDrawParam dp)
AlphaDim	function	BOOL AlphaDim(PSDrawBlock db, PSDrawParam dp)
AlphaDimZ	function	BOOL AlphaDimZ(PSDrawBlock db, PSDrawParam dp)
Alpha32	function	BOOL Alpha32(PSDrawBlock db, PSDrawParam dp)
Alias	function	BOOL Alias(PSDrawBlock db, PSDrawParam dp)
AliasColor	function	BOOL AliasColor(PSDrawBlock db, PSDrawParam dp)
Alias32	function	BOOL Alias32(PSDrawBlock db, PSDrawParam dp)
DrawSelected8	function	BOOL DrawSelected8(PSDrawBlock db, PSDrawParam dp)
DrawSelected	function	BOOL DrawSelected(PSDrawBlock db, PSDrawParam dp)
ZPut	function	BOOL ZPut(PSDrawBlock db, PSDrawParam dp)
ZPut8	function	BOOL ZPut8(PSDrawBlock db, PSDrawParam dp)
ZPut816	function	BOOL ZPut816(PSDrawBlock db, PSDrawParam dp)
ZPut32	function	BOOL ZPut32(PSDrawBlock db, PSDrawParam dp)
ShutterZPut	function	BOOL ShutterZPut(PSDrawBlock db, PSDrawParam dp)
ShutterZPut8	function	BOOL ShutterZPut8(PSDrawBlock db, PSDrawParam dp)
ShutterZPut816	function	BOOL ShutterZPut816(PSDrawBlock db, PSDrawParam dp)
ShutterZPut32	function	BOOL ShutterZPut32(PSDrawBlock db, PSDrawParam dp)
ZNormalPut	function	BOOL ZNormalPut(PSDrawBlock db, PSDrawParam dp)
ZNormalPut8	function	BOOL ZNormalPut8(PSDrawBlock db, PSDrawParam dp)
ZNormalPut816	function	BOOL ZNormalPut816(PSDrawBlock db, PSDrawParam dp)
ZNormalPut32	function	BOOL ZNormalPut32(PSDrawBlock db, PSDrawParam dp)
ShutterZNormalPut	function	BOOL ShutterZNormalPut(PSDrawBlock db, PSDrawParam dp)
ShutterZNormalPut8	function	BOOL ShutterZNormalPut8(PSDrawBlock db, PSDrawParam dp)
ShutterZNormalPut816	function	BOOL ShutterZNormalPut816(PSDrawBlock db, PSDrawParam dp)
ShutterZNormalPut32	function	BOOL ShutterZNormalPut32(PSDrawBlock db, PSDrawParam dp)
ParamZFind	function	BOOL ParamZFind(PSDrawBlock db, PSDrawParam dp)
ZStretch	function	BOOL ZStretch(PSDrawBlock db, PSDrawParam dp)
ZMaskStretch	function	BOOL ZMaskStretch(PSDrawBlock db, PSDrawParam dp)
Clear	function	BOOL Clear(PSDrawBlock db, PSDrawParam dp)
Convert15to16	function	BOOL Convert15to16(PTBitmapData bitmap)
ConvertPal15to16	function	BOOL ConvertPal15to16(PTBitmapData bitmap)
Convert16to15	function	BOOL Convert16to15(PTBitmapData bitmap)
ConvertPal16to15	function	BOOL ConvertPal16to15(PTBitmapData bitmap)
ParamLine	function	BOOL ParamLine(int x1, int y1, int x2, int y2, SColor &color, PSDrawBlock db, PSDrawParam dp)
ParamWriteText	function	int ParamWriteText(PSDrawBlock db, PSDrawParam dp, int startline, int numlines, char *text,
ParamNumLinesInText	function	void ParamNumLinesInText(char *text, PTFont font, int wrapwidth)
ParamFrame	function	void ParamFrame(PSDrawBlock db, PSDrawParam dp, BOOL down, DRAWCALLBACK callback)
ZFindChunk	function	WORD ZFindChunk(PSDrawBlock db, PSDrawParam dp)
                                                            þ
ÄÄ´ MULTIS~1.CPP  ÃÄÄÄÄÄÄ´ 08-15-97 04:28pm ÃÄÄÄÄÄÄÄþDT=588219286
TMultiSurface.TMultiSurface	function	TMultiSurface::TMultiSurface()
TMultiSurface.~TMultiSurface	function	TMultiSurface::~TMultiSurface()
TMultiSurface.Initialize	function	void TMultiSurface::Initialize(PTSurface surface1,
TMultiSurface.Reset	function	void TMultiSurface::Reset()
TMultiSurface.SetOrigin	function	void TMultiSurface::SetOrigin(int x, int y)
TMultiSurface.SetClipRect	function	void TMultiSurface::SetClipRect(int x, int y, int w, int h)
TMultiSurface.SetClipMode	function	void TMultiSurface::SetClipMode(int mode)
TMultiSurface.Close	function	void TMultiSurface::Close()
                                                            þ
ÄÄ´ EDITOR.CPP    ÃÄÄÄÄÄÄ´ 10-03-97 12:26pm ÃÄÄÄÄÄÄÄþDT=591618888
MemCopy	function	void MemCopy(void *dest, void *source, int size)
sprintbit	function	char *sprintbit(int bits, char *enumbits[], char *buf)
StartEditor	function	void StartEditor()
ShutDownEditor	function	void ShutDownEditor()
TERRAIN_NOTHING	#define	#define TERRAIN_NOTHING		0
TERRAIN_GRASS	#define	#define TERRAIN_GRASS		1
TERRAIN_DIRT	#define	#define TERRAIN_DIRT		2
TERRAIN_MOUNTAIN	#define	#define TERRAIN_MOUNTAIN	3
TERRAIN_LAKE	#define	#define TERRAIN_LAKE		4
TERRAIN_RIVER	#define	#define TERRAIN_RIVER		5
TERRAIN_OCEAN	#define	#define TERRAIN_OCEAN		6
TERRAIN_WASTELAND	#define	#define TERRAIN_WASTELAND	7
NUMTERRAINTYPES	#define	#define NUMTERRAINTYPES		8
TERRAINTYPESHIFT	#define	#define TERRAINTYPESHIFT	4
OVERLAY_NOTHING	#define	#define OVERLAY_NOTHING		0
OVERLAY_PATH	#define	#define OVERLAY_PATH		1
OVERLAY_ROAD	#define	#define OVERLAY_ROAD		2
OVERLAY_FORESTSTART	#define	#define OVERLAY_FORESTSTART	16
OVERLAY_FORESTEND	#define	#define OVERLAY_FORESTEND	32
GetTerrainType	function	BYTE GetTerrainType(BYTE *map, int x, int y)
TERRAIN	#define	#define TERRAIN(dx, dy)		GetTerrainType(map, x + dx, y + dy)
BaseTerrain	function	int BaseTerrain(BYTE *map, int x, int y)
GetTileCode	function	int GetTileCode(BYTE *map, int x, int y)
ELEVATIONCHANGE	#define	#define ELEVATIONCHANGE		46
TerrainHeight	function	int TerrainHeight(BYTE *map, int x, int y)
=	struct	struct { char ch; int code; } TerrainCodes[] = {
TerrainToCode	function	int TerrainToCode(char ch)
FindSuperTile	function	int FindSuperTile(BYTE *map, int x, int y, BYTE *filled, int *flux)
overlay	function	int overlay(BYTE *overmap, int x, int y)
OVERLAY	#define	#define OVERLAY(dx, dy) (overlay(overmap, x+(dx), y+(dy)))
GetOverlayTile	function	int GetOverlayTile(BYTE *overmap, int x, int y, int code)
GenerateObj	function	PTObjectInstance GenerateObj(PTSector sect, BYTE *map, BYTE *overmap, int type, int x, int y, int px, int py, int flux = 0)
QUAD	#define	#define QUAD(c, q)	(((c) >> ((q) * 8)) & 0xFF)
GenerateMap	function	void GenerateMap(int startx, int starty, int sizex, int sizey)
TTextPane.Initialize	function	BOOL TTextPane::Initialize()
TTextPane.Close	function	void TTextPane::Close()
TTextPane.DrawBackground	function	void TTextPane::DrawBackground()
isvalid	function	inline isvalid(int key)
endofline	function	int endofline(char *text)
TTextPane.InsertText	function	BOOL TTextPane::InsertText(char *newtext)
TTextPane.RemoveText	function	BOOL TTextPane::RemoveText(int numchars, BOOL forward)
TTextPane.UpdateWindow	function	void TTextPane::UpdateWindow()
TTextPane.SetCursor	function	BOOL TTextPane::SetCursor(int nx, int ny, BOOL ignorebounds)
TTextPane.KeyPress	function	void TTextPane::KeyPress(int key, BOOL down)
TTextPane.CharPress	function	void TTextPane::CharPress(int key, BOOL down)
TConsolePane.Initialize	function	BOOL TConsolePane::Initialize()
TConsolePane.SetupMouseBitmap	function	void TConsolePane::SetupMouseBitmap(int xspan, int yspan)
TConsolePane.ChainMouse	function	void TConsolePane::ChainMouse(void (*cfunc)(int, int), int x0, int x1, int xcur,
TConsolePane.SetBounds	function	void TConsolePane::SetBounds()
TConsolePane.AddAxis	function	BOOL TConsolePane::AddAxis(PTObjectInstance inst)
TConsolePane.ClearAxis	function	void TConsolePane::ClearAxis()
TConsolePane.KeyPress	function	void TConsolePane::KeyPress(int key, BOOL down)
TConsolePane.MouseMove	function	void TConsolePane::MouseMove(int button, int x, int y)
TConsolePane.MouseClick	function	void TConsolePane::MouseClick(int button, int x, int y)
TConsolePane.DrawBackground	function	void TConsolePane::DrawBackground()
TConsolePane.Input	function	BOOL TConsolePane::Input(char *string)
TConsolePane.Output	function	BOOL TConsolePane::Output(char *string)
TScriptPane.Initialize	function	BOOL TScriptPane::Initialize()
TScriptPane.LoadScript	function	void TScriptPane::LoadScript(PTObjectInstance oi)
TScriptPane.SaveScript	function	void TScriptPane::SaveScript()
TScriptPane.KeyPress	function	void TScriptPane::KeyPress(int key, BOOL down)
TScrollEditorPane.Initialize	function	BOOL TScrollEditorPane::Initialize()
TScrollEditorPane.SetScroll	function	void TScrollEditorPane::SetScroll(PTObjectInstance s)
TScrollEditorPane.SaveText	function	void TScrollEditorPane::SaveText()
TScrollEditorPane.KeyPress	function	void TScrollEditorPane::KeyPress(int key, BOOL down)
COMMAND	function	COMMAND(CmdSelect)
COMMAND	function	COMMAND(CmdDeselect)
COMMAND	function	COMMAND(CmdAdd)
COMMAND	function	COMMAND(CmdGenerate)
COMMAND	function	COMMAND(CmdCalcWalkmap)
COMMAND	function	COMMAND(CmdSaveWalkmap)
COMMAND	function	COMMAND(CmdWalkmap)
COMMAND	function	COMMAND(CmdTileWalkmap)
COMMAND	function	COMMAND(CmdState)
COMMAND	function	COMMAND(CmdReveal)
COMMAND	function	COMMAND(CmdLevel)
COMMAND	function	COMMAND(CmdCGSGen)
COMMAND	function	COMMAND(CmdUnusedImagery)
COMMAND	function	COMMAND(CmdTemplate)
COMMAND	function	COMMAND(CmdSmoothScroll)
COMMAND	function	COMMAND(CmdDLight)
COMMAND	function	COMMAND(CmdText)
COMMAND	function	COMMAND(CmdUndo)
COMMAND	function	COMMAND(CmdMove)
COMMAND	function	COMMAND(CmdDelete)
COMMAND	function	COMMAND(CmdStat)
COMMAND	function	COMMAND(CmdScript)
COMMAND	function	COMMAND(CmdGroup)
COMMAND	function	COMMAND(CmdCenterOn)
COMMAND	function	COMMAND(CmdMapPos)
ShowObjList	function	BOOL ShowObjList(PTObjectClass cl, char *match)
ShowObjTypeInfo	function	void ShowObjTypeInfo(PTObjectClass cl, int objtype)
COMMAND	function	COMMAND(CmdShow)
Ambient	function	void Ambient(int ambient, int dummy = -1)
COMMAND	function	COMMAND(CmdAmbient)
AmbientColorRed	function	void AmbientColorRed(int red, int dummy = -1)
AmbientColorGreen	function	void AmbientColorGreen(int green, int dummy = -1)
AmbientColorBlue	function	void AmbientColorBlue(int blue, int dummy = -1)
COMMAND	function	COMMAND(CmdAmbColor)
Mono	function	void Mono(int percent, int dummy = -1)
COMMAND	function	COMMAND(CmdMono)
COMMAND	function	COMMAND(CmdDirLight)
COMMAND	function	COMMAND(CmdBaseLight)
COMMAND	function	COMMAND(CmdReplace)
COMMAND	function	COMMAND(CmdLight)
ZOffset	function	void ZOffset(int zoffset, int dummy = -1)
COMMAND	function	COMMAND(CmdZOffset)
COMMAND	function	COMMAND(CmdRegistration)
COMMAND	function	COMMAND(CmdAnimRegistration)
COMMAND	function	COMMAND(CmdAnimZ)
COMMAND	function	COMMAND(CmdName)
COMMAND	function	COMMAND(CmdFlip)
COMMAND	function	COMMAND(CmdLock)
COMMAND	function	COMMAND(CmdUnlock)
COMMAND	function	COMMAND(CmdImmobile)
COMMAND	function	COMMAND(CmdUnimmobile)
COMMAND	function	COMMAND(CmdLoad)
COMMAND	function	COMMAND(CmdSave)
COMMAND	function	COMMAND(CmdSaveEnv)
COMMAND	function	COMMAND(CmdLoadEnv)
COMMAND	function	COMMAND(CmdDXStats)
COMMAND	function	COMMAND(CmdMemory)
BtnInvert	function	void BtnInvert()
BtnPrev	function	void BtnPrev()
BtnNext	function	void BtnNext()
BtnDeselect	function	void BtnDeselect()
BtnCenterOn	function	void BtnCenterOn()
BtnUndo	function	void BtnUndo()
BtnWalkmap	function	void BtnWalkmap()
BtnScroll	function	void BtnScroll()
TEditStatusPane.Initialize	function	BOOL TEditStatusPane::Initialize()
TEditStatusPane.Close	function	void TEditStatusPane::Close()
TEditStatusPane.DrawBackground	function	void TEditStatusPane::DrawBackground()
TEditStatusPane.InvertAxisButtons	function	void TEditStatusPane::InvertAxisButtons()
TEditStatusPane.Select	function	BOOL TEditStatusPane::Select(int index, BOOL add)
TEditStatusPane.Deselect	function	BOOL TEditStatusPane::Deselect()
TEditStatusPane.Deselect	function	BOOL TEditStatusPane::Deselect(int index)
TEditStatusPane.Reselect	function	BOOL TEditStatusPane::Reselect(int oldindex, int newindex)
TEditStatusPane.SetCurObj	function	BOOL TEditStatusPane::SetCurObj(int newcurobj)
TEditStatusPane.Next	function	BOOL TEditStatusPane::Next()
TEditStatusPane.Prev	function	BOOL TEditStatusPane::Prev()
TEditStatusPane.Validate	function	void TEditStatusPane::Validate()
TEditStatusPane.Delete	function	void TEditStatusPane::Delete()
StartObjMoving	function	void StartObjMoving(PTObjectInstance oi)
TEditStatusPane.StartMoving	function	void TEditStatusPane::StartMoving()
StopObjMoving	function	void StopObjMoving(PTObjectInstance oi)
TEditStatusPane.StopMoving	function	void TEditStatusPane::StopMoving()
TEditStatusPane.Undo	function	void TEditStatusPane::Undo()
BtnSave	function	void BtnSave()
BtnReveal	function	void BtnReveal()
BtnStat	function	void BtnStat()
BtnZOff	function	void BtnZOff()
Registration	function	void Registration(S3DPoint pos)
RegistrationAbort	function	void RegistrationAbort()
BtnReg	function	void BtnReg()
BtnDel	function	void BtnDel()
BtnAmb	function	void BtnAmb()
BtnAmbCol	function	void BtnAmbCol()
BtnMono	function	void BtnMono()
Clone	function	void Clone(S3DPoint pos)
CloneAbort	function	void CloneAbort()
BtnClone	function	void BtnClone()
BtnFlip	function	void BtnFlip()
BtnCalc	function	void BtnCalc()
BtnSavWM	function	void BtnSavWM()
BtnBound	function	void BtnBound()
AnimRegistration	function	void AnimRegistration(int x, int y)
AnimZ	function	void AnimZ(int z, int dummy = -1)
BtnAnReg	function	void BtnAnReg()
BtnAnimZ	function	void BtnAnimZ()
BtnTemplate	function	void BtnTemplate()
TOOLBUTTONWIDTH	#define	#define TOOLBUTTONWIDTH		36
TOOLBUTTONHEIGHT	#define	#define TOOLBUTTONHEIGHT	20
=	struct	{
TEditToolsPane.Initialize	function	BOOL TEditToolsPane::Initialize()
TEditToolsPane.DrawBackground	function	void TEditToolsPane::DrawBackground()
BtnMaximize	function	void BtnMaximize()
BtnScrollUp	function	void BtnScrollUp()
BtnScrollDown	function	void BtnScrollDown()
BtnTileClass	function	void BtnTileClass()
BtnCharacterClass	function	void BtnCharacterClass()
BtnWeaponClass	function	void BtnWeaponClass()
BtnArmorClass	function	void BtnArmorClass()
BtnTalismanClass	function	void BtnTalismanClass()
BtnFoodClass	function	void BtnFoodClass()
BtnLightSourceClass	function	void BtnLightSourceClass()
BtnToolClass	function	void BtnToolClass()
BtnContainerClass	function	void BtnContainerClass()
BtnNothing	function	void BtnNothing()
ObjDownCallback	function	void ObjDownCallback(S3DPoint pos)
ObjDownAbortCallback	function	void ObjDownAbortCallback()
TEditClassPane.Initialize	function	BOOL TEditClassPane::Initialize()
TEditClassPane.Close	function	void TEditClassPane::Close()
OBJWIDTH	#define	#define OBJWIDTH	(143 - 32)
OBJHEIGHT	#define	#define OBJHEIGHT	(THUMBNAILHEIGHT)
TEditClassPane.DrawBackground	function	void TEditClassPane::DrawBackground()
TEditClassPane.MouseClick	function	void TEditClassPane::MouseClick(int button, int x, int y)
TEditClassPane.MouseMove	function	void TEditClassPane::MouseMove(int button, int x, int y)
TEditClassPane.Animate	function	void TEditClassPane::Animate(BOOL draw)
TEditClassPane.GetThumbnail	function	PTBitmap TEditClassPane::GetThumbnail(char *fname)
TEditClassPane.PutObject	function	int TEditClassPane::PutObject(S3DPoint pos)
TEditClassPane.Scroll	function	void TEditClassPane::Scroll(int lines)
TEditClassPane.SelectSame	function	void TEditClassPane::SelectSame(int index)
                                                            þ
ÄÄ´ EXLMAIN.CPP   ÃÄÄÄÄÄÄ´ 10-02-97 04:31pm ÃÄÄÄÄÄÄÄþDT=591561714
RUNPATHLEN	#define	#define RUNPATHLEN 100 // RunPath Defines
InitSystem	function	void InitSystem()
CloseSystem	function	void CloseSystem()
FatalError	function	void FatalError(char *error, char *extra)
Error	function	void Error(char *error, char *extra)
xmalloc	function	void *xmalloc(int size)
xrealloc	function	void *xrealloc(void *p, int size)
xfree	function	void xfree(void *p)
malloc	#define	#define malloc(m) xmalloc(m)
free	#define	#define free(p) xfree(p)
realloc	#define	#define realloc(p, m) xrealloc(p, m)
new	function	void* __cdecl operator new(size_t size)
delete	function	void __cdecl operator delete(void *pointer)
popen	function	FILE *popen(char *name, char *flags)
random	function	int random(int min, int max)
MemUsed	function	DWORD MemUsed()
FreeMem	function	DWORD FreeMem()
TotalMem	function	DWORD TotalMem()
FreePhys	function	DWORD FreePhys()
TotalPhys	function	DWORD TotalPhys()
FreePage	function	DWORD FreePage()
TotalPage	function	DWORD TotalPage()
GetProgramPaths	function	void GetProgramPaths(char *lpCmdLine, char *RunPath, char *SavePath)
joyGetNumDevs	function	HasJoyStick = joyGetNumDevs() > 0;
while	function	while (NextScreen)
                                                            þ
ÄÄ´ ANIMIM~1.CPP  ÃÄÄÄÄÄÄ´ 10-02-97 11:36am ÃÄÄÄÄÄÄÄþDT=591551642
TAnimImagery.AlwaysOnTop	function	BOOL TAnimImagery::AlwaysOnTop(PTObjectInstance oi)
TAnimImagery.DrawLit	function	void TAnimImagery::DrawLit(PTObjectInstance oi, PTSurface surface)
TAnimImagery.DrawUnlit	function	void TAnimImagery::DrawUnlit(PTObjectInstance oi, PTSurface surface)
TAnimImagery.GetZ	function	BOOL TAnimImagery::GetZ(PTObjectInstance oi, PTSurface surface)
TAnimImagery.DrawSelected	function	void TAnimImagery::DrawSelected(PTObjectInstance oi, PTSurface surface)
TAnimImagery.DrawInvItem	function	void TAnimImagery::DrawInvItem(PTObjectInstance oi, int x, int y)
TAnimImagery.NeedsAnimator	function	BOOL TAnimImagery::NeedsAnimator(PTObjectInstance oi)
TAnimImagery.NewObjectAnimator	function	PTObjectAnimator TAnimImagery::NewObjectAnimator(PTObjectInstance oi)
TAnimAnimator.TAnimAnimator	function	TAnimAnimator::TAnimAnimator(PTObjectInstance oi) : TObjectAnimator(oi)
TAnimAnimator.Animate	function	void TAnimAnimator::Animate(BOOL draw)
TAnimAnimator.~TAnimAnimator	function	TAnimAnimator::~TAnimAnimator()
                                                            þ
ÄÄ´ SURFACE.CPP   ÃÄÄÄÄÄÄ´ 10-02-97 11:38am ÃÄÄÄÄÄÄÄþDT=591551690
TSurface.TSurface	function	TSurface::TSurface()
TSurface.~TSurface	function	TSurface::~TSurface()
TSurface.Reset	function	void TSurface::Reset()
TSurface.ParamPut	function	BOOL TSurface::ParamPut(PSDrawParam dp, PTBitmap bitmap)
TSurface.ParamBlit	function	BOOL TSurface::ParamBlit(PSDrawParam dp, PTSurface surface, int flags)
TSurface.Put	function	BOOL TSurface::Put(int x, int y, PTBitmap bitmap, DWORD drawmode, PSColor color)
TSurface.PutHue	function	BOOL TSurface::PutHue(int x, int y, PTBitmap bitmap, DWORD drawmode, int hue)
TSurface.PutSV	function	BOOL TSurface::PutSV(int x, int y, PTBitmap bitmap, DWORD drawmode, int saturation, int brightness)
TSurface.PutDim	function	BOOL TSurface::PutDim(int x, int y, PTBitmap bitmap, DWORD drawmode, int dim)
TSurface.ZPut	function	BOOL TSurface::ZPut(int x, int y, int z, PTBitmap bitmap, DWORD drawmode)
TSurface.ZFind	function	DWORD TSurface::ZFind(int x, int y, int z, PTBitmap bitmap, DWORD drawmode)
TSurface.Clear	function	BOOL TSurface::Clear(DWORD color, WORD zpos, DWORD drawmode)
TSurface.Clear	function	BOOL TSurface::Clear(int dx, int dy, int dwidth, int dheight,
TSurface.WriteTextShadow	function	void TSurface::WriteTextShadow(char *text, int x, int y, int numlines, PTFont font,
TSurface.WriteText	function	void TSurface::WriteText(char *text, int x, int y, int numlines, PTFont font,
TSurface.Frame	function	void TSurface::Frame(int x, int y, int w, int h, BOOL down)
                                                            þ
ÄÄ´ EQUIP.CPP     ÃÄÄÄÄÄÄ´ 09-11-97 06:26pm ÃÄÄÄÄÄÄÄþDT=590058311
=	struct	{ { { 46, 22 }, { 46, 64 }, { 4, 81 }, { 88, 81 }, { -1, -1 }, { -1, -1 }, { 88, 3 }, { 4, 3 }, { -1, -1 } },
BtnEquipDown	function	void BtnEquipDown()
BtnEquipUp	function	void BtnEquipUp()
TEquipPane.Initialize	function	BOOL TEquipPane::Initialize()
TEquipPane.MouseClick	function	void TEquipPane::MouseClick(int button, int x, int y)
TEquipPane.DrawBackground	function	void TEquipPane::DrawBackground()
TEquipPane.Scroll	function	void TEquipPane::Scroll(int amount)
TEquipPane.DrawAnim	function	void TEquipPane::DrawAnim(PTObjectInstance inst, PTBitmap bm)
TEquipPane.OnSlot	function	int TEquipPane::OnSlot(int x, int y)
                                                            þ
ÄÄ´ 3DIMAGE.CPP   ÃÄÄÄÄÄÄ´ 09-26-97 05:53pm ÃÄÄÄÄÄÄÄþDT=591040181
T3DImagery.T3DImagery	function	T3DImagery::T3DImagery(int imageryid) : TObjectImagery(imageryid)
T3DImagery.~T3DImagery	function	T3DImagery::~T3DImagery()
T3DImagery.InitializeMesh	function	BOOL T3DImagery::InitializeMesh(PS3DImageryBody mesh)
T3DImagery.ClearMesh	function	void T3DImagery::ClearMesh()
T3DImagery.RefreshZBuffer	function	void T3DImagery::RefreshZBuffer(PTObjectInstance oi)
T3DImagery.Render	function	BOOL T3DImagery::Render(PTObjectInstance oi, D3DMATRIX &pos, int state, int frame, D3DRECT &extent, PS3DRect worldextents, PS3DRect objextents)
T3DImagery.LoadTexture	function	BOOL T3DImagery::LoadTexture(int texnum, LPDDSURFACEDESC srcsd,
T3DImagery.LoadTexture	function	BOOL T3DImagery::LoadTexture(int texnum, LPDDSURFACEDESC srcsd,
T3DImagery.ReleaseTexture	function	void T3DImagery::ReleaseTexture(int texnum)
T3DImagery.SurfacesLost	function	BOOL T3DImagery::SurfacesLost()
T3DImagery.RestoreSurfaces	function	void T3DImagery::RestoreSurfaces()
T3DImagery.GetZ	function	BOOL T3DImagery::GetZ(PTObjectInstance oi, PTSurface surface)
T3DImagery.NewObjectAnimator	function	PTObjectAnimator T3DImagery::NewObjectAnimator(PTObjectInstance oi)
T3DAnimator.T3DAnimator	function	T3DAnimator::T3DAnimator(PTObjectInstance oi) : TObjectAnimator(oi)
T3DAnimator.~T3DAnimator	function	T3DAnimator::~T3DAnimator()
T3DAnimator.Animate	function	void T3DAnimator::Animate(BOOL draw)
T3DAnimator.SurfacesLost	function	BOOL T3DAnimator::SurfacesLost()
T3DAnimator.RestoreSurfaces	function	void T3DAnimator::RestoreSurfaces()
T3DAnimator.Render	function	BOOL T3DAnimator::Render()
T3DAnimator.GetExtents	function	void T3DAnimator::GetExtents(RSRect r)
Angle	function	int Angle(int x, int y)
Distance	function	int Distance(int x, int y)
Rotate	function	inline void Rotate(int angle, int &opposite, int &adjacent)
T3DAnimator.GetObjBoundParams	function	void T3DAnimator::GetObjBoundParams(int obj, RSBoundParams bp)
T3DAnimator.CheckIntersect	function	BOOL T3DAnimator::CheckIntersect(int obj, PTObjectAnimator targanim, int targobj)
T3DHelperImagery.T3DHelperImagery	function	T3DHelperImagery::T3DHelperImagery(int imageryid) : T3DImagery(imageryid)
T3DHelperImagery.~T3DHelperImagery	function	T3DHelperImagery::~T3DHelperImagery()
COPY_VERTEX	#define	#define COPY_VERTEX(v0, v1)	{ (v0)->x = (v1)->x;				\
COPY_VERTEX	function	(v0)->y = (v1)->y;				\
y	function	(v0)->y = (v1)->y;				\
\	function	(v0)->z = (v1)->z;	(v0)++; }
z	function	(v0)->z = (v1)->z;	(v0)++; }
z	function	(v0)->z = (v1)->z;	(v0)++; }
T3DHelperImagery.Render	function	BOOL T3DHelperImagery::Render(PTObjectInstance oi, D3DMATRIX &pos, int state, int frame, D3DRECT &extent, PS3DRect worldextents, PS3DRect objextents)
T3DHelperImagery.SetWorldBoundBox	function	void T3DHelperImagery::SetWorldBoundBox(int state, int width, int length, int height)
                                                            þ
ÄÄ´ SECTOR.CPP    ÃÄÄÄÄÄÄ´ 09-17-97 02:18pm ÃÄÄÄÄÄÄÄþDT=590443088
TSector.TSector	function	TSector::TSector(int newlevel, int newsectorx, int newsectory)
TSector.~TSector	function	TSector::~TSector()
TSector.Load	function	BOOL TSector::Load(BOOL lock)
TSector.Save	function	void TSector::Save()
SECTORLREDGE	#define	#define SECTORLREDGE	256				// Left/right edge
SECTORUEDGE	#define	#define SECTORUEDGE		512				// Upper edge (nothing can be below the sector
TSector.GetMaxScreenRect	function	void TSector::GetMaxScreenRect(RSRect r)
TSector.GetMaxMapRect	function	void TSector::GetMaxMapRect(RS3DRect r)
TSector.AddObject	function	int TSector::AddObject(PTObjectInstance oi, int c)
TSector.SetObject	function	int TSector::SetObject(PTObjectInstance oi, int item)
TSector.RemoveObject	function	PTObjectInstance TSector::RemoveObject(int item)
TSector.FindObject	function	int TSector::FindObject(PTObjectInstance inst)
TSector.FindObject	function	int TSector::FindObject(char *name, int &found, int occurance)
TSector.FindClosestObject	function	int TSector::FindClosestObject(char *name, RS3DPoint pos, BOOL partial)
WalkmapToZPos	function	inline int WalkmapToZPos(BYTE height)
ZPosToWalkmap	function	inline int ZPosToWalkmap(int z)
TSector.WalkmapHandler	function	void TSector::WalkmapHandler(int mode, BYTE *walk, int zpos, int x, int y, int width, int length, int stride)
TSector.ReturnWalkmap	function	int TSector::ReturnWalkmap(RS3DPoint pos)
TSector.SetWalkmap	function	void TSector::SetWalkmap(RS3DPoint pos)
TSector.CheckRect	function	BOOL TSector::CheckRect(SRect rect, int height, int facing)
                                                            þ
ÄÄ´ CHARAC~1.CPP  ÃÄÄÄÄÄÄ´ 10-02-97 11:36am ÃÄÄÄÄÄÄÄþDT=591551644
REGISTER_BUILDER	function	REGISTER_BUILDER(TCharacter)
MAXLOOPS	#define	#define MAXLOOPS		3
TCharacter.ClearChar	function	void TCharacter::ClearChar()
TCharacter.Pulse	function	void TCharacter::Pulse()
TCharacter.UpdateAction	function	void TCharacter::UpdateAction(int bits)
TCharacter.ResolveAction	function	int TCharacter::ResolveAction(int bits)
TCharacter.Animate	function	void TCharacter::Animate(BOOL draw)
MAXZMOVE	#define	#define MAXZMOVE	2
TCharacter.Move	function	DWORD TCharacter::Move()
TCharacter.SetDoing	function	void TCharacter::SetDoing(PTActionBlock ab)
TCharacter.SetDesired	function	void TCharacter::SetDesired(PTActionBlock ab)
TCharacter.TryCommand	function	int TCharacter::TryCommand(PTActionBlock ab, int bits)
TCharacter.ForceCommand	function	int TCharacter::ForceCommand(PTActionBlock ab, int bits)
TCharacter.Advance	function	void TCharacter::Advance(int angle, int speed, int maxturn)
MAXCHANGE	#define	#define MAXCHANGE	200
STOPHEIGHT	#define	#define STOPHEIGHT	50
TCharacter.UpdateAngle	function	int TCharacter::UpdateAngle(int angle)
TCharacter.TryFall	function	int TCharacter::TryFall(PSActionBlock ab, int bits)
TCharacter.TryStand	function	int TCharacter::TryStand(PSActionBlock ab, int bits)
TCharacter.TryCombatStance	function	int TCharacter::TryCombatStance(PSActionBlock ab, int bits)
TCharacter.TryWalk	function	int TCharacter::TryWalk(PSActionBlock ab, int bits)
TCharacter.TryPickup	function	int TCharacter::TryPickup(PSActionBlock ab, int bits)
TCharacter.TrySay	function	int TCharacter::TrySay(PSActionBlock ab, int bits)
TCharacter.TryAttack	function	int TCharacter::TryAttack(PSActionBlock ab, int bits)
TCharacter.TryDodge	function	int TCharacter::TryDodge(PSActionBlock ab, int bits)
TCharacter.TryBlock	function	int TCharacter::TryBlock(PSActionBlock ab, int bits)
TCharacter.TryStopBlock	function	int TCharacter::TryStopBlock(PSActionBlock ab, int bits)
TCharacter.TryImpact	function	int TCharacter::TryImpact(PSActionBlock ab, int bits)
TCharacter.TryCollapse	function	int TCharacter::TryCollapse(PSActionBlock ab, int bits)
TCharacter.TryInvoke	function	int TCharacter::TryInvoke(PSActionBlock ab, int bits)
TCharacter.TryWait	function	int TCharacter::TryWait(PSActionBlock ab, int bits)
TCharacter.TryWalkTo	function	int TCharacter::TryWalkTo(PSActionBlock ab, int bits)
TCharacter.ResolveWalk	function	int TCharacter::ResolveWalk(PTActionBlock ab, int bits)
TCharacter.ResolveSwing	function	int TCharacter::ResolveSwing(PTActionBlock ab, int bits)
TCharacter.ResolveThrust	function	int TCharacter::ResolveThrust(PTActionBlock ab, int bits)
TCharacter.ResolveBlock	function	int TCharacter::ResolveBlock(PTActionBlock ab, int bits)
TCharacter.ResolveImpact	function	int TCharacter::ResolveImpact(PTActionBlock ab, int bits)
TCharacter.ResolveCombatWalk	function	int TCharacter::ResolveCombatWalk(PTActionBlock ab, int bits)
TCharacter.IsFighting	function	BOOL TCharacter::IsFighting()
TCharacter.Go	function	void TCharacter::Go(int angle)
TCharacter.Go	function	void TCharacter::Go(S3DPoint vect)
TCharacter.Goto	function	void TCharacter::Goto(int x, int y)
TCharacter.Stop	function	void TCharacter::Stop()
TCharacter.FollowChar	function	void TCharacter::FollowChar(int index)
TCharacter.Pickup	function	void TCharacter::Pickup(int index)
TCharacter.Say	function	void TCharacter::Say(char *string)
TCharacter.Swing	function	void TCharacter::Swing(int index)
TCharacter.Thrust	function	void TCharacter::Thrust(int index)
TCharacter.Block	function	void TCharacter::Block(int index)
TCharacter.Invoke	function	void TCharacter::Invoke(int *spell)
TCharacter.BeginCombat	function	void TCharacter::BeginCombat(int index)
TCharacter.EndCombat	function	void TCharacter::EndCombat()
TCharacter.Wait	function	void TCharacter::Wait(int waitlen)
TCharacter.PlayAnim	function	void TCharacter::PlayAnim(char *string)
                                                            þ
ÄÄ´ SCROLL.CPP    ÃÄÄÄÄÄÄ´ 10-02-97 11:38am ÃÄÄÄÄÄÄÄþDT=591551683
REGISTER_BUILDER	function	REGISTER_BUILDER(TScroll)
TScroll.SetText	function	void TScroll::SetText(char *newtext)
TScroll.Load	function	void TScroll::Load(RTInputStream is)
TScroll.Save	function	void TScroll::Save(RTOutputStream os)
TScroll.Use	function	BOOL TScroll::Use(int with)
NUMLINES	#define	#define NUMLINES		11		// depends on font and scroll height
SCROLLWIDTH	#define	#define SCROLLWIDTH		430		// depends on font and scroll width
ScrollBtnScrollUp	function	void ScrollBtnScrollUp()
ScrollBtnScrollDown	function	void ScrollBtnScrollDown()
ScrollBtnExit	function	void ScrollBtnExit()
TScrollPane.Initialize	function	BOOL TScrollPane::Initialize()
TScrollPane.Close	function	void TScrollPane::Close()
TScrollPane.DrawBackground	function	void TScrollPane::DrawBackground()
TScrollPane.KeyPress	function	void TScrollPane::KeyPress(int key, BOOL down)
TScrollPane.SetScroll	function	void TScrollPane::SetScroll(PTScroll s)
TScrollPane.Scroll	function	void TScrollPane::Scroll(int numscrolllines)
                                                            þ
ÄÄ´ PARSE.CPP     ÃÄÄÄÄÄÄ´ 09-11-97 09:03pm ÃÄÄÄÄÄÄÄþDT=590063715
TIdent.TIdent	function	TIdent::TIdent()
TIdent.Find	function	Ident *TIdent::Find(char *text, int type)
TIdent.FindIndex	function	int TIdent::FindIndex(char *text, int type)
TIdent.Add	function	void TIdent::Add(char *ident, int type, int value)
NUMKEYWORDS	#define	#define NUMKEYWORDS		4
TToken.Get	function	void TToken::Get()
if	function	if (isalpha(*f))
Parse	function	BOOL __cdecl Parse(TToken &t, char *format, ...)
ParseString	function	BOOL __cdecl ParseString(char *string, char *format, ...)
                                                            þ
ÄÄ´ BITMAP.CPP    ÃÄÄÄÄÄÄ´ 09-22-97 12:49pm ÃÄÄÄÄÄÄÄþDT=590767665
TBitmap.NewBitmap	function	PTBitmap TBitmap::NewBitmap(int width, int height, int bmflags,
TBitmap.Load	function	PTBitmap TBitmap::Load(int resource)
TBitmap.WriteText	function	void TBitmap::WriteText(char *text, int x, int y, int lines, PTFont font, PSColor color, DWORD drawmode)
TBitmap.Decompress	function	BOOL TBitmap::Decompress(PTBitmap to)
TBitmap.Decompress	function	BOOL TBitmap::Decompress(void *buffer, int width, int height)
TBitmap.Clear	function	BOOL TBitmap::Clear(SColor &color, DWORD bmdrawmode, WORD zpos)
TBitmap.Put	function	BOOL TBitmap::Put(int x, int y, PTBitmap bitmap, int srcx, int srcy, int srcw, int srch,
TBitmap.Put	function	BOOL TBitmap::Put(int x, int y, PTSurface surface, int srcx, int srcy, int srcw,
BI_RGB	#define	#define BI_RGB 0
BI_RLE8	#define	#define BI_RLE8 1
BI_RLE4	#define	#define BI_RLE4 2
BI_BITFIELDS	#define	#define BI_BITFIELDS 3
BitmapHeader	struct	{
BitmapInfo	struct	{
TBitmap.SaveBMP	function	BOOL TBitmap::SaveBMP(char *filename)
TBitmap.OnPixel	function	BOOL TBitmap::OnPixel(int x, int y)
TBitmap.Line	function	BOOL TBitmap::Line(int x1, int y1, int x2, int y2, SColor &color)
TBitmap.Box	function	BOOL TBitmap::Box(int x1, int y1, int x2, int y2, SColor &color)
TranslateColor	function	WORD TranslateColor(SColor &color)
                                                            þ
ÄÄ´ BUTTON.CPP    ÃÄÄÄÄÄÄ´ 10-02-97 11:36am ÃÄÄÄÄÄÄÄþDT=591551644
TButton.TButton	function	TButton::TButton(char *bname, int bx, int by, int bw, int bh, WORD keypr,
TButton.Draw	function	void TButton::Draw()
REPEATWAIT	#define	#define REPEATWAIT		7
TButton.Animate	function	void TButton::Animate(BOOL draw)
TButton.OnButton	function	BOOL TButton::OnButton(int bx, int by)
TButton.IsKey	function	inline BOOL TButton::IsKey(int keypr)
TButtonPane.Initialize	function	BOOL TButtonPane::Initialize()
TButtonPane.Close	function	void TButtonPane::Close()
TButtonPane.DrawBackground	function	void TButtonPane::DrawBackground()
TButtonPane.Animate	function	void TButtonPane::Animate(BOOL draw)
TButtonPane.RedrawButtons	function	void TButtonPane::RedrawButtons()
TButtonPane.KeyPress	function	void TButtonPane::KeyPress(int key, BOOL down)
TButtonPane.MouseClick	function	void TButtonPane::MouseClick(int button, int x, int y)
TButtonPane.MouseMove	function	void TButtonPane::MouseMove(int button, int x, int y)
TButtonPane.NewButton	function	BOOL TButtonPane::NewButton(char *bname, int bx, int by, int bw, int bh, WORD key,
TButtonPane.ClearGroup	function	void TButtonPane::ClearGroup(int group)
TButtonPane.CheckGroup	function	void TButtonPane::CheckGroup(int group)
                                                            þ
ÄÄ´ COMMAND.CPP   ÃÄÄÄÄÄÄ´ 09-26-97 05:53pm ÃÄÄÄÄÄÄÄþDT=591040185
CommandInterpreter	function	int CommandInterpreter(PTObjectInstance context, TToken &t, int abrevlen)
Output	function	void Output(char *buf)
COMMAND	function	COMMAND(CmdHelp)
COMMAND	function	COMMAND(CmdSay)
COMMAND	function	COMMAND(CmdGoto)
COMMAND	function	COMMAND(CmdFace)
COMMAND	function	COMMAND(CmdUse)
COMMAND	function	COMMAND(CmdAttack)
COMMAND	function	COMMAND(CmdBlock)
COMMAND	function	COMMAND(CmdInvoke)
COMMAND	function	COMMAND(CmdWait)
COMMAND	function	COMMAND(CmdStop)
ApplyOperation	function	int ApplyOperation(int &lval, int op, int rval)
FindElement	function	int FindElement(char *elem, char *list[])
ParseExpression	function	BOOL ParseExpression(TToken &t, int *value)
COMMAND	function	COMMAND(CmdBegin)
COMMAND	function	COMMAND(CmdEnd)
COMMAND	function	COMMAND(CmdIf)
COMMAND	function	COMMAND(CmdElse)
COMMAND	function	COMMAND(CmdWhile)
COMMAND	function	COMMAND(CmdSet)
                                                            þ
ÄÄ´ MULTIC~1.CPP  ÃÄÄÄÄÄÄ´ 10-01-97 01:37pm ÃÄÄÄÄÄÄÄþDT=591490218
BtnEquip	function	void BtnEquip()
BtnMap	function	void BtnMap()
BtnSpell	function	void BtnSpell()
BtnStats	function	void BtnStats()
TMultiCtrlPane.Initialize	function	BOOL TMultiCtrlPane::Initialize()
TMultiCtrlPane.Close	function	void TMultiCtrlPane::Close()
TMultiCtrlPane.ActivatePane	function	void TMultiCtrlPane::ActivatePane(int pane)
TMultiCtrlPane.Hide	function	void TMultiCtrlPane::Hide()
TMultiCtrlPane.Show	function	void TMultiCtrlPane::Show()
TMultiCtrlPane.RedrawCurPane	function	void TMultiCtrlPane::RedrawCurPane()
                                                            þ
ÄÄ´ IMAGERY.CPP   ÃÄÄÄÄÄÄ´ 09-25-97 05:52pm ÃÄÄÄÄÄÄÄþDT=590974611
QE_NONE	#define	#define QE_NONE		 0
QE_QUEUED	#define	#define QE_QUEUED	 1
QE_LOADING	#define	#define QE_LOADING	 2
QE_LOADED	#define	#define QE_LOADED	 3
QE_FAILED	#define	#define QE_FAILED	 4
TObjectAnimator.TObjectAnimator	function	TObjectAnimator::TObjectAnimator(PTObjectInstance oi)
TObjectAnimator.~TObjectAnimator	function	TObjectAnimator::~TObjectAnimator()
TObjectAnimator.Animate	function	void TObjectAnimator::Animate(BOOL draw)
TImageryBuilder.TImageryBuilder	function	TImageryBuilder::TImageryBuilder(int newid)
TObjectImagery.TObjectImagery	function	TObjectImagery::TObjectImagery(int id)
TObjectImagery.~TObjectImagery	function	TObjectImagery::~TObjectImagery()
TObjectImagery.RegisterImagery	function	int TObjectImagery::RegisterImagery(char *filename)
TObjectImagery.FreeAllImagery	function	void TObjectImagery::FreeAllImagery()
TObjectImagery.ReloadImagery	function	void TObjectImagery::ReloadImagery()
TObjectImagery.NeedsAnimator	function	BOOL TObjectImagery::NeedsAnimator(PTObjectInstance oi)
TObjectImagery.SetImageryPath	function	void TObjectImagery::SetImageryPath(char *path)
TObjectImagery.GetImageryPath	function	char *TObjectImagery::GetImageryPath()
TObjectImagery.SaveAllHeaders	function	void TObjectImagery::SaveAllHeaders()
TObjectImagery.LoadImagery	function	PTObjectImagery TObjectImagery::LoadImagery(int imgid)
TObjectImagery.FreeImagery	function	void TObjectImagery::FreeImagery(PTObjectImagery imagery)
TObjectImagery.FindState	function	int TObjectImagery::FindState(char *name)
StateMatch	function	BOOL StateMatch(char *string, char *statename)
TObjectImagery.FindTransitionState	function	int TObjectImagery::FindTransitionState(char *from, char *to)
TObjectImagery.GetUseCount	function	int TObjectImagery::GetUseCount()
TObjectImagery.GetScreenRect	function	void TObjectImagery::GetScreenRect(PTObjectInstance oi, SRect &r)
TObjectImagery.SaveHeader	function	void TObjectImagery::SaveHeader(int imgid)
TObjectImagery.SetWorldBoundBox	function	void TObjectImagery::SetWorldBoundBox(int state, int width, int length, int height)
TObjectImagery.BeginLoaderThread	function	void TObjectImagery::BeginLoaderThread()
TObjectImagery.EndLoaderThread	function	void TObjectImagery::EndLoaderThread()
TObjectImagery.LoaderThread	function	void _cdecl TObjectImagery::LoaderThread(void *)
TObjectImagery.LoadBody	function	PSImageryBody TObjectImagery::LoadBody(BOOL wait)
TObjectImagery.FreeBody	function	void TObjectImagery::FreeBody()
                                                            þ
ÄÄ´ DDSURF~1.CPP  ÃÄÄÄÄÄÄ´ 09-10-97 08:09pm ÃÄÄÄÄÄÄÄþDT=589996327
TVideoSurface.TVideoSurface	function	TVideoSurface::TVideoSurface()
TVideoSurface.Initialize	function	void TVideoSurface::Initialize(int vidwidth, int vidheight, int flags)
TVideoSurface.Initialize	function	void TVideoSurface::Initialize(PTBitmap bitmap, int intensity, BOOL usevideomem)
TVideoSurface.Initialize	function	void TVideoSurface::Initialize(LPDIRECTDRAWSURFACE ddsurface)
TVideoSurface.Close	function	void TVideoSurface::Close()
TVideoSurface.~TVideoSurface	function	TVideoSurface::~TVideoSurface()
TVideoSurface.Restore	function	BOOL TVideoSurface::Restore()
TVideoSurface.Lock	function	void *TVideoSurface::Lock()
TVideoSurface.Unlock	function	BOOL TVideoSurface::Unlock()
TVideoSurface.Rect	function	BOOL TVideoSurface::Rect(int x, int y, int w, int h, SColor &color)
TVideoSurface.Rect	function	BOOL TVideoSurface::Rect(SRect r, SColor &color)
TVideoSurface.Box	function	BOOL TVideoSurface::Box(int x, int y, int w, int h, SColor &color)
TVideoSurface.Line	function	BOOL TVideoSurface::Line(int x1, int y1, int x2, int y2, SColor &color)
TVideoSurface.BltDraw	function	BOOL TVideoSurface::BltDraw(PSDrawBlock db, PSDrawParam dp, PTSurface srcsurface)
TVideoSurface.BlitPrimary	function	BOOL TVideoSurface::BlitPrimary(PSDrawParam dp, PTSurface srcsurface, int flags)
                                                            þ
ÄÄ´ BATTLE.CPP    ÃÄÄÄÄÄÄ´ 04-02-97 05:37pm ÃÄÄÄÄÄÄÄþDT=578981051
TBattle.Pulse	function	void TBattle::Pulse()
TBattle.Clear	function	void TBattle::Clear()
TBattle.Attack	function	void TBattle::Attack(int index, int x, int y, int width, int height,
TBattle.Ambush	function	void TBattle::Ambush(int index, int x, int y, int width, int height, DWORD endflag,
TBattle.Defend	function	void TBattle::Defend(int index, int x, int y, int width, int height,
TBattle.Guard	function	void TBattle::Guard(int x, int y, int width, int height, DWORD endflag,
TBattle.Patrol	function	void TBattle::Patrol(int x, int y, int width, int height, DWORD endflag,
TBattle.Flee	function	void TBattle::Flee(int x, int y, int speed, DWORD endflag, DWORD condition)
                                                            þ
ÄÄ´ HOLEANIM.CPP  ÃÄÄÄÄÄÄ´ 06-13-97 05:16pm ÃÄÄÄÄÄÄÄþDT=583895568
THoleAnimator.THoleAnimator	function	THoleAnimator::THoleAnimator(PTObjectInstance oi) : TObjectAnimator(oi)
THoleAnimator.Animate	function	void THoleAnimator::Animate(BOOL draw)
THoleAnimator.~THoleAnimator	function	THoleAnimator::~THoleAnimator()
                                                            þ
ÄÄ´ 3DSCENE.CPP   ÃÄÄÄÄÄÄ´ 09-26-97 05:53pm ÃÄÄÄÄÄÄÄþDT=591040181
T3DLight	class	{
MAX3DANIMATORS	#define	#define MAX3DANIMATORS 128
T3DAnimatorArray	typedef	typedef TPointerArray<T3DAnimator, MAX3DANIMATORS> T3DAnimatorArray;
MAX3DLIGHTS	#define	#define MAX3DLIGHTS 64
T3DLightArray	typedef	typedef TPointerArray<T3DLight, MAX3DLIGHTS> T3DLightArray;
MAXTEXFORMATS	#define	#define MAXTEXFORMATS 64
T3DLight.T3DLight	function	T3DLight::T3DLight(D3DLIGHT &newlight)
T3DLight.~T3DLight	function	T3DLight::~T3DLight()
T3DLight.Initialize	function	BOOL T3DLight::Initialize()
T3DLight.Close	function	BOOL T3DLight::Close()
T3DLight.SetIntensity	function	void T3DLight::SetIntensity(D3DVALUE constant, D3DVALUE linear, D3DVALUE quadratic)
T3DLight.SetColor	function	void T3DLight::SetColor(D3DVALUE red, D3DVALUE green, D3DVALUE blue)
T3DLight.SetPosition	function	void T3DLight::SetPosition(D3DVALUE x, D3DVALUE y, D3DVALUE z)
T3DScene.T3DScene	function	T3DScene::T3DScene()
T3DScene.~T3DScene	function	T3DScene::~T3DScene()
T3DScene.Initialize	function	BOOL T3DScene::Initialize(int x, int y, int width, int height)
T3DScene.Close	function	BOOL T3DScene::Close()
T3DScene.SetCameraPos	function	void T3DScene::SetCameraPos(RS3DPoint pos, int zdist)
T3DScene.DrawScene	function	BOOL T3DScene::DrawScene()
T3DScene.AdjustAmbientLight	function	void T3DScene::AdjustAmbientLight(int ambient)  // What the heck
T3DScene.SetAmbientLight	function	void T3DScene::SetAmbientLight(SColor color)
T3DScene.AddLight	function	int T3DScene::AddLight(RS3DPoint pos, SColor color, int intensity)
T3DScene.DeleteLight	function	BOOL T3DScene::DeleteLight(int lightid)
T3DScene.SetLightIntensity	function	BOOL T3DScene::SetLightIntensity(int lightid, int intensity)
T3DScene.SetLightColor	function	BOOL T3DScene::SetLightColor(int lightid, SColor color)
T3DScene.SetLightPosition	function	BOOL T3DScene::SetLightPosition(int lightid, RS3DPoint pos)
T3DScene.GetNumLights	function	int T3DScene::GetNumLights()
T3DScene.AddAnimator	function	int T3DScene::AddAnimator(PT3DAnimator animator)
T3DScene.RemoveAnimator	function	BOOL T3DScene::RemoveAnimator(DWORD animid)
T3DScene.GetNumAnimators	function	int T3DScene::GetNumAnimators()
EnumTextureFormatsCallback	function	HRESULT CALLBACK EnumTextureFormatsCallback(LPDDSURFACEDESC lpDDSD, LPVOID lpContext)
T3DScene.GetTextureFormats	function	void T3DScene::GetTextureFormats()
countbits	function	inline int countbits(DWORD d)
T3DScene.GetClosestTextureFormat	function	void T3DScene::GetClosestTextureFormat(LPDDSURFACEDESC srcsd, LPDDSURFACEDESC dstsd)
getmaskshift	function	inline int getmaskshift(DWORD mask)
getmaskbits	function	inline int getmaskbits(DWORD mask)
T3DScene.ConvertTexture	function	void T3DScene::ConvertTexture(
                                                            þ
ÄÄ´ SCRIPT.CPP    ÃÄÄÄÄÄÄ´ 09-17-97 02:18pm ÃÄÄÄÄÄÄÄþDT=590443087
ScriptError	function	void ScriptError(char *buf, int linenum)
SkipBlock	function	void SkipBlock(TToken &t)
TScript.TScript	function	TScript::TScript(PTScriptProto prototype)
TScript.~TScript	function	TScript::~TScript()
TScript.Load	function	BOOL TScript::Load(char *filename)
TScript.Save	function	BOOL TScript::Save(char *filename)
TScript.Start	function	void TScript::Start(char *ptr, int newpriority)
Triggered	function	BOOL Triggered(TToken &t, PTObjectInstance context)
MAXITERATIONS	#define	#define MAXITERATIONS	6000			// for catching endless loops
TScript.Continue	function	void TScript::Continue(PTObjectInstance context)
TScript.Break	function	void TScript::Break()
TScript.Resume	function	void TScript::Resume()
TScript.End	function	void TScript::End()
TScriptProto.FitsCriteria	function	BOOL TScriptProto::FitsCriteria(PTObjectInstance inst)
TScriptProto.SetBuffer	function	void TScriptProto::SetBuffer(char *buffer)
TScriptProto.ParseCriteria	function	BOOL TScriptProto::ParseCriteria(TToken &t)
TScriptProto.ParseScript	function	int TScriptProto::ParseScript(char *buffer)
TScriptProto.WriteScript	function	BOOL TScriptProto::WriteScript(FILE *fp)
TGameState.Load	function	BOOL TGameState::Load(char *filename)
TGameState.Save	function	BOOL TGameState::Save(char *filename)
TScriptManager.Initialize	function	BOOL TScriptManager::Initialize()
TScriptManager.Close	function	void TScriptManager::Close()
TScriptManager.Load	function	BOOL TScriptManager::Load(char *filename)
TScriptManager.Save	function	BOOL TScriptManager::Save(char *filename)
TScriptManager.ParseScripts	function	void TScriptManager::ParseScripts(char *buffer)
TScriptManager.ObjectScript	function	PTScript TScriptManager::ObjectScript(PTObjectInstance inst)
                                                            þ
ÄÄ´ TEMPLATE.CPP  ÃÄÄÄÄÄÄ´ 09-17-97 02:18pm ÃÄÄÄÄÄÄÄþDT=590443088
TTerrainTemplate.TTerrainTemplate	function	TTerrainTemplate::TTerrainTemplate()
TTerrainTemplate.~TTerrainTemplate	function	TTerrainTemplate::~TTerrainTemplate()
TTerrainTemplate.Load	function	BOOL TTerrainTemplate::Load()
TTerrainTemplate.Save	function	BOOL TTerrainTemplate::Save()
TTerrainTemplate.NewTemplate	function	int TTerrainTemplate::NewTemplate(char *tilename)
TTerrainTemplate.AddObject	function	int TTerrainTemplate::AddObject(int index, PTObjectInstance inst, S3DPoint offset)
MAXFOUND	#define	#define MAXFOUND	32
TTerrainTemplate.ApplyTemplate	function	void TTerrainTemplate::ApplyTemplate(PTObjectInstance inst, PTSector sect, int density)
                                                            þ
ÄÄ´ MULTI.CPP     ÃÄÄÄÄÄÄ´ 05-30-97 06:11pm ÃÄÄÄÄÄÄÄþDT=582914420
TMulti.Object	function	void *TMulti::Object(char *name)
                                                            þ
ÄÄ´ DLS.CPP       ÃÄÄÄÄÄÄ´ 09-16-97 11:32am ÃÄÄÄÄÄÄÄþDT=590371863
RADIUS	#define	#define RADIUS 254
DIAMETER	#define	#define DIAMETER (RADIUS << 1)
UNLIT	struct	{
NORMAL	struct	{
ZBUFFER	struct	{
*	function	RTBitmap UnlitBuffer  = *(PTBitmap)&ub;
*	function	RTBitmap NormalBuffer = *(PTBitmap)&nb;
*	function	RTBitmap ZBuffer      = *(PTBitmap)&zb;
DrawLight	function	void DrawLight(RS3DPoint pos, RSColor color, int intensity, PTSurface surface)
DrawLightNoNormals	function	void DrawLightNoNormals(RS3DPoint pos, RSColor color, int intensity,
DrawStaticLight	function	void DrawStaticLight(RS3DPoint pos, RSColor color, int intensity, PTSurface surface, int id)
DrawStaticMonoLight	function	void DrawStaticMonoLight(RS3DPoint pos, int intensity, PTSurface surface)
DrawStaticDirLight	function	void DrawStaticDirLight(RS3DPoint pos, RSColor color, int intensity, PTSurface surface, int id)
DrawStaticDirMonoLight	function	void DrawStaticDirMonoLight(RS3DPoint pos, int intensity, PTSurface surface)
DrawStaticLightNoNormals	function	void DrawStaticLightNoNormals(RS3DPoint pos, RSColor color, int intensity, PTSurface surface, int id)
DrawStaticMonoLightNoNormals	function	void DrawStaticMonoLightNoNormals(RS3DPoint pos, int intensity, PTSurface surface)
DrawAmbientLight	function	void DrawAmbientLight(PTSurface surface, RSRect r)
SetAmbientLight	function	void SetAmbientLight(int hnormal, int vnormal, int intensityval, int ambientval)
Transfer32to16	function	void Transfer32to16(PTSurface dest, PTSurface source, RSRect rect)
                                                            þ
ÄÄ´ INVENT~1.CPP  ÃÄÄÄÄÄÄ´ 10-03-97 12:26pm ÃÄÄÄÄÄÄÄþDT=591618889
TInventory.Initialize	function	BOOL TInventory::Initialize()
TInventory.Close	function	void TInventory::Close()
TInventory.DrawBackground	function	void TInventory::DrawBackground()
TInventory.Animate	function	void TInventory::Animate()
TInventory.MouseClick	function	void TInventory::MouseClick(int button, int x, int y)
TInventory.MouseMove	function	void TInventory::MouseMove(int button, int x, int y)
TInventory.DrawAnim	function	void TInventory::DrawAnim(PTObjectInstance inst, PTBitmap bm)
TInventory.OnSlot	function	int TInventory::OnSlot(int x, int y)
TInventory.SwapSlots	function	void TInventory::SwapSlots(int oldslot, int newslot)
                                                            þ
ÄÄ´ MULTII~1.CPP  ÃÄÄÄÄÄÄ´ 08-07-97 06:00pm ÃÄÄÄÄÄÄÄþDT=587698202
TMultiImagery.AlwaysOnTop	function	BOOL TMultiImagery::AlwaysOnTop(PTObjectInstance oi)
TMultiImagery.DrawLit	function	void TMultiImagery::DrawLit(PTObjectInstance oi, PTSurface surface)
TMultiImagery.DrawUnlit	function	void TMultiImagery::DrawUnlit(PTObjectInstance oi, PTSurface surface)
TMultiImagery.NewObjectAnimator	function	PTObjectAnimator TMultiImagery::NewObjectAnimator(PTObjectInstance oi)
TMultiImagery.GetScreenRect	function	void TMultiImagery::GetScreenRect(PTObjectInstance oi, SRect &r)
                                                            þ
ÄÄ´ STATUS~1.CPP  ÃÄÄÄÄÄÄ´ 09-26-97 05:55pm ÃÄÄÄÄÄÄÄþDT=591040224
TStatusBar.Initialize	function	BOOL TStatusBar::Initialize()
TStatusBar.Close	function	void TStatusBar::Close()
TStatusBar.DrawBackground	function	void TStatusBar::DrawBackground()
THealthBar.Initialize	function	BOOL THealthBar::Initialize()
THealthBar.DrawBackground	function	void THealthBar::DrawBackground()
TStaminaBar.Initialize	function	BOOL TStaminaBar::Initialize()
TStaminaBar.DrawBackground	function	void TStaminaBar::DrawBackground()
                                                            þ
ÄÄ´ MONEY.CPP     ÃÄÄÄÄÄÄ´ 08-07-97 11:48am ÃÄÄÄÄÄÄÄþDT=587685395
REGISTER_BUILDER	function	REGISTER_BUILDER(TMoney)
=	struct	{ { 20, 28 }, { 13, 22 }, { 22, 18 }, { 22, 31 }, { 13, 35 }, { 0, 27 }, { 9, 25 },
TMoney.Initialize	function	BOOL TMoney::Initialize()
TMoney.Close	function	void TMoney::Close()
TMoney.~TMoney	function	TMoney::~TMoney()
TMoney.AddToInventory	function	void TMoney::AddToInventory(PTObjectInstance oi)
TMoney.RemoveFromInventory	function	void TMoney::RemoveFromInventory(PTObjectInstance oi)
TMoney.SetAmount	function	void TMoney::SetAmount(int amt)
TMoney.Load	function	void TMoney::Load(RTInputStream is)
TMoney.Save	function	void TMoney::Save(RTOutputStream os)
TMoney.AllocInvItem	function	void TMoney::AllocInvItem(PTBitmap inv, int type, int count)
TMoney.FreeInvItem	function	void TMoney::FreeInvItem(int type, int count)
TMoney.AllocGroundItem	function	void TMoney::AllocGroundItem(PTBitmap ground, int type, int count)
TMoney.FreeGroundItem	function	void TMoney::FreeGroundItem(int type, int count)
TMoney.DrawInvItem	function	void TMoney::DrawInvItem(int x, int y)
TMoney.InventoryImage	function	PTBitmap TMoney::InventoryImage()
TMoney.GetScreenRect	function	void TMoney::GetScreenRect(SRect &r)
TMoney.DrawUnlit	function	void TMoney::DrawUnlit(PTSurface surface)
TMoney.GetStillImage	function	PTBitmap TMoney::GetStillImage(int ostate)
                                                            þ
ÄÄ´ AMMO.CPP      ÃÄÄÄÄÄÄ´ 08-07-97 11:48am ÃÄÄÄÄÄÄÄþDT=587685396
REGISTER_BUILDER	function	REGISTER_BUILDER(TAmmo)
IMGAMOUNT	#define	#define IMGAMOUNT(x)	((x) < MAXAMMOIMAGE ? (x) : ((x) - (MAXAMMOIMAGE/2)) % (MAXAMMOIMAGE/2) + (MAXAMMOIMAGE/2))
=	struct	{ { 0, 4, 4 }, { 0, 5, 7 }, { 0, 1, 5 },
TAmmo.Initialize	function	BOOL TAmmo::Initialize()
TAmmo.Close	function	void TAmmo::Close()
TAmmo.~TAmmo	function	TAmmo::~TAmmo()
TAmmo.AddToInventory	function	void TAmmo::AddToInventory(PTObjectInstance oi)
TAmmo.RemoveFromInventory	function	void TAmmo::RemoveFromInventory(PTObjectInstance oi)
TAmmo.SetAmount	function	void TAmmo::SetAmount(int amt)
TAmmo.Load	function	void TAmmo::Load(RTInputStream is)
TAmmo.Save	function	void TAmmo::Save(RTOutputStream os)
TAmmo.AllocInvItem	function	void TAmmo::AllocInvItem(PTObjectImagery img, int state, int type, int count)
TAmmo.FreeInvItem	function	void TAmmo::FreeInvItem(int type, int count)
TAmmo.AllocGroundItem	function	void TAmmo::AllocGroundItem(PTObjectImagery img, int state, int type, int count)
TAmmo.FreeGroundItem	function	void TAmmo::FreeGroundItem(int type, int count)
TAmmo.DrawInvItem	function	void TAmmo::DrawInvItem(int x, int y)
TAmmo.InventoryImage	function	PTBitmap TAmmo::InventoryImage()
TAmmo.GetScreenRect	function	void TAmmo::GetScreenRect(SRect &r)
TAmmo.DrawUnlit	function	void TAmmo::DrawUnlit(PTSurface surface)
TAmmo.GetStillImage	function	PTBitmap TAmmo::GetStillImage(int ostate)
                                                            þ
ÄÄ´ EXIT.CPP      ÃÄÄÄÄÄÄ´ 09-03-97 07:09am ÃÄÄÄÄÄÄÄþDT=589510967
REGISTER_BUILDER	function	REGISTER_BUILDER(TExit)
CLOSED	#define	#define CLOSED		0
OPEN	#define	#define OPEN		1
CLOSING	#define	#define CLOSING		2
OPENING	#define	#define OPENING		3
TExit.Use	function	BOOL TExit::Use(int with)
TExit.Pulse	function	void TExit::Pulse()
TExit.Pulse	function	void TExit::Pulse()
TExit.Use	function	BOOL TExit::Use(int with)
                                                            þ
ÄÄ´ TILE.CPP      ÃÄÄÄÄÄÄ´ 02-14-97 07:16pm ÃÄÄÄÄÄÄÄþDT=575576578
REGISTER_BUILDER	function	REGISTER_BUILDER(TTile)
                                                            þ
ÄÄ´ MAINWND.CPP   ÃÄÄÄÄÄÄ´ 10-03-97 01:40pm ÃÄÄÄÄÄÄÄþDT=591621403
ID_ICON	#define	#define ID_ICON 101
TMainWindow.Initialize	function	BOOL TMainWindow::Initialize(HANDLE hInstance, HANDLE hPrevInstance,
TMainWindow.Close	function	void TMainWindow::Close()
TMainWindow.~TMainWindow	function	TMainWindow::~TMainWindow()
MainWndProc	function	LRESULT CALLBACK MainWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
                                                            þ
ÄÄ´ 3DSURF~1.CPP  ÃÄÄÄÄÄÄ´ 01-16-97 01:29pm ÃÄÄÄÄÄÄÄþDT=573598646
T3dSurface.T3dSurface	function	T3dSurface::T3dSurface()
T3dSurface.~T3dSurface	function	T3dSurface::~T3dSurface()
T3dSurface.LockBuffer	function	LPDIRECTDRAWSURFACE T3dSurface::LockBuffer()
T3dSurface.UnlockBuffer	function	BOOL T3dSurface::UnlockBuffer()
                                                            þ
ÄÄ´ ARMOR.CPP     ÃÄÄÄÄÄÄ´ 08-06-97 05:02pm ÃÄÄÄÄÄÄÄþDT=587630668
REGISTER_BUILDER	function	REGISTER_BUILDER(TArmor)
                                                            þ
ÄÄ´ BMSURF~1.CPP  ÃÄÄÄÄÄÄ´ 02-07-97 03:22pm ÃÄÄÄÄÄÄÄþDT=575109834
TBitmapSurface.TBitmapSurface	function	TBitmapSurface::TBitmapSurface()
TBitmapSurface.~TBitmapSurface	function	TBitmapSurface::~TBitmapSurface()
TBitmapSurface.Initialize	function	void TBitmapSurface::Initialize(int bmwidth, int bmheight, int bmflags)
TBitmapSurface.Initialize	function	void TBitmapSurface::Initialize(PTBitmap newbitmap)
TBitmapSurface.Rect	function	BOOL TBitmapSurface::Rect(int x, int y, int w, int h, SColor &color)
TBitmapSurface.Rect	function	BOOL TBitmapSurface::Rect(SRect r, SColor &color)
TBitmapSurface.Box	function	BOOL TBitmapSurface::Box(int x, int y, int w, int h, SColor &color)
TBitmapSurface.Line	function	BOOL TBitmapSurface::Line(int x1, int y1, int x2, int y2, SColor &color)
TBitmapSurface.Copy	function	BOOL TBitmapSurface::Copy(PTBitmap bitmap)
TBitmapSurface.BlitPrimary	function	BOOL TBitmapSurface::BlitPrimary(PSDrawParam dp, PTSurface surface, int flags)
                                                            þ
ÄÄ´ LOGOSC~1.CPP  ÃÄÄÄÄÄÄ´ 03-07-97 02:01pm ÃÄÄÄÄÄÄÄþDT=577204275
TLogoScreen.TLogoScreen	function	TLogoScreen::TLogoScreen()
TLogoScreen.Initialize	function	BOOL TLogoScreen::Initialize()
TLogoScreen.InitScreenRestore	function	void TLogoScreen::InitScreenRestore()
TLogoScreen.Close	function	void TLogoScreen::Close()
TLogoScreen.DrawBackground	function	void TLogoScreen::DrawBackground()
TLogoScreen.Animate	function	void TLogoScreen::Animate(BOOL draw)
TLogoScreen.MouseClick	function	void TLogoScreen::MouseClick(int button, int x, int y)
TLogoScreen.KeyPress	function	void TLogoScreen::KeyPress(int key, BOOL down)
TLogoScreen.Joystick	function	void TLogoScreen::Joystick(int button, int x, int y)
TLogoScreen.Command	function	void TLogoScreen::Command(int command)
TLogoScreen.ShowScreen	function	PTScreen TLogoScreen::ShowScreen(PTScreen screen, int ticks)
TLogoScreen.TimerTick	function	BOOL TLogoScreen::TimerTick(BOOL draw)
TLogoScreen.TimerLoop	function	BOOL TLogoScreen::TimerLoop(long ticks)
                                                            þ
ÄÄ´ ANIMAT~1.CPP  ÃÄÄÄÄÄÄ´ 05-05-97 05:42pm ÃÄÄÄÄÄÄÄþDT=581274946
TAnimation.TAnimation	function	TAnimation::TAnimation()
TAnimation.~TAnimation	function	TAnimation::~TAnimation()
TAnimation.Load	function	PTAnimation TAnimation::Load(int resource)
TAnimation.Decompress	function	BOOL TAnimation::Decompress(PSDrawBlock db, int frame, BOOL Transparent)
TAnimation.Put	function	void TAnimation::Put(PTSurface surface, void *decbuf, int frame, PSDrawParam dp)
TAnimation.Stretch	function	void TAnimation::Stretch(PTSurface surface, PTSurface decbuf, int x, int y, int size,
TAnimation.GetFrame	function	PTBitmap TAnimation::GetFrame(int frame)
TAnimation.Move	function	void TAnimation::Move(int &x, int &y, int frame, int orientation)
TAnimation.SizeMove	function	void TAnimation::SizeMove(int &x, int &y, int size, int frame, int orientation)
                                                            þ
ÄÄ´ COLORT~1.CPP  ÃÄÄÄÄÄÄ´ 08-29-97 10:40pm ÃÄÄÄÄÄÄÄþDT=589149458
Round	function	int Round(double d)
SetAmbientColor	function	void SetAmbientColor(RSColor color)
GetAmbientColor	function	void GetAmbientColor(RSColor color)
SetMonoPercent	function	void SetMonoPercent(int pcnt)
GetMonoPercent	function	int GetMonoPercent()
TOLERANCE	#define	#define TOLERANCE		3
NewLightIndex	function	int NewLightIndex(RSColor color, int mult)
FreeLightIndex	function	void FreeLightIndex(int id)
ClearLights	function	void ClearLights()
SetLightColor	function	void SetLightColor(int id, RSColor color, int mult)
GetLightColor	function	void GetLightColor(int id, RSColor color, int &mult)
MakeColorTables	function	void MakeColorTables()
                                                            þ
ÄÄ´ SPELL.CPP     ÃÄÄÄÄÄÄ´ 10-02-97 11:38am ÃÄÄÄÄÄÄÄþDT=591551690
BtnSpellPane	function	void BtnSpellPane()
BtnSpellBook	function	void BtnSpellBook()
BtnSpellAdd	function	void BtnSpellAdd()
BtnSpellBack	function	void BtnSpellBack()
BtnSpellDown	function	void BtnSpellDown()
BtnSpellUp	function	void BtnSpellUp()
BtnSpellMin	function	void BtnSpellMin()
REPEATRATE	#define	#define REPEATRATE		8
TSpellPane.Initialize	function	BOOL TSpellPane::Initialize()
TAL_STARTX	#define	#define TAL_STARTX		19
TAL_STARTY	#define	#define TAL_STARTY		10
TAL_WIDTH	#define	#define TAL_WIDTH		24
TAL_HEIGHT	#define	#define TAL_HEIGHT		24
TAL_WRAPWIDTH	#define	#define TAL_WRAPWIDTH	100
TAL_WRAPHEIGHT	#define	#define TAL_WRAPHEIGHT	76
TSpellPane.DrawBackground	function	void TSpellPane::DrawBackground()
TSpellPane.Scroll	function	void TSpellPane::Scroll(int numlines)
TSpellPane.OnTalisman	function	int TSpellPane::OnTalisman(int x, int y)
TSpellPane.Invoke	function	void TSpellPane::Invoke(int *spell, int numtal)
BtnSpellOne	function	void BtnSpellOne()
BtnSpellTwo	function	void BtnSpellTwo()
BtnSpellThree	function	void BtnSpellThree()
BtnSpellFour	function	void BtnSpellFour()
TQuickSpellPane.Initialize	function	BOOL TQuickSpellPane::Initialize()
TQuickSpellPane.Close	function	void TQuickSpellPane::Close()
TQuickSpellPane.DrawBackground	function	void TQuickSpellPane::DrawBackground()
                                                            þ
ÄÄ´ FONT.CPP      ÃÄÄÄÄÄÄ´ 10-02-97 11:37am ÃÄÄÄÄÄÄÄþDT=591551648
TFont.GetChar	function	PTBitmap TFont::GetChar(unsigned char ch)
TFont.FindNumLinesInText	function	int TFont::FindNumLinesInText(char *text, int wrapwidth, int justify)
                                                            þ
ÄÄ´ CONTAI~1.CPP  ÃÄÄÄÄÄÄ´ 09-24-97 09:15am ÃÄÄÄÄÄÄÄþDT=590891508
REGISTER_BUILDER	function	REGISTER_BUILDER(TContainer)
TContainer.Use	function	BOOL TContainer::Use(int with)
                                                            þ
ÄÄ´ ZCOMP.CPP     ÃÄÄÄÄÄÄ´ 04-22-97 05:57pm ÃÄÄÄÄÄÄÄþDT=580292395
ReadFile	function	unsigned int ReadFile(char *buff, unsigned int *size, void *Param)
WriteFile	function	void WriteFile(char *buff, unsigned int *size, void *Param)
ReadBuff	function	unsigned int ReadBuff(char *buff, unsigned int *size,  void *Param)
WriteBuff	function	void WriteBuff(char *buff, unsigned int *size, void *Param)
cfopen	function	FILE *cfopen(const char *filename, const char *mode, int type, int dsize)
cfread	function	size_t cfread(void *ptr, long size, size_t n, FILE *stream)
cfwrite	function	size_t cfwrite(const void *ptr, long size, size_t n, FILE *stream)
cfclose	function	int cfclose(FILE *stream)
getCompSize	function	long getCompSize()
                                                            þ
ÄÄ´ TIMER.CPP     ÃÄÄÄÄÄÄ´ 04-08-97 06:25pm ÃÄÄÄÄÄÄÄþDT=579375922
period	function	DWORD period = (DWORD)(1000 / FRAMERATE);
TTimer.TimerCallback	function	void WINAPI TTimer::TimerCallback(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
TTimer.TTimer	function	TTimer::TTimer()
TTimer.~TTimer	function	TTimer::~TTimer()
TTimer.Close	function	BOOL TTimer::Close()
TTimer.Initialize	function	BOOL TTimer::Initialize()
                                                            þ
ÄÄ´ SAVEGAME.CPP  ÃÄÄÄÄÄÄ´ 09-26-97 11:35am ÃÄÄÄÄÄÄÄþDT=591027320
NOTHING	#define	#define NOTHING		0
CONTENTS	#define	#define CONTENTS	1
NEXT	#define	#define NEXT		2
TSaveGame.ExtractSaveObj	function	PSObjectList TSaveGame::ExtractSaveObj(PTObjectInstance inst)
TSaveGame.Save	function	BOOL TSaveGame::Save()
TSaveGame.RestoreObjList	function	int TSaveGame::RestoreObjList(PSObjectList objlist)
TSaveGame.Restore	function	BOOL TSaveGame::Restore()
TSaveGame.PurgeObjList	function	void TSaveGame::PurgeObjList(PSObjectList objlist)
TSaveGame.Purge	function	void TSaveGame::Purge()
WriteObjList	function	BOOL WriteObjList(RTOutputStream os, PSObjectList objlist)
TSaveGame.WriteGame	function	BOOL TSaveGame::WriteGame()
ReadObjList	function	PSObjectList ReadObjList(RTInputStream is)
TSaveGame.ReadGame	function	BOOL TSaveGame::ReadGame()
                                                            þ
ÄÄ´ SPRITE.CPP    ÃÄÄÄÄÄÄ´ 01-16-97 12:25pm ÃÄÄÄÄÄÄÄþDT=573596455
TSprite.Clear	function	void TSprite::Clear()
TSprite.Move	function	void TSprite::Move(int deltax, int deltay)
TSprite.Goto	function	void TSprite::Goto(int absx, int absy)
                                                            þ
ÄÄ´ DECOMP~1.CPP  ÃÄÄÄÄÄÄ´ 05-27-97 12:14pm ÃÄÄÄÄÄÄÄþDT=582705600
ZBufferDecompress32	function	BOOL ZBufferDecompress32(PSDrawBlock db, PSDrawParam dp)
ClipZBufferDecompress32	function	BOOL ClipZBufferDecompress32(PSDrawBlock db, PSDrawParam dp)
                                                            þ
ÄÄ´ ANIMSP~1.CPP  ÃÄÄÄÄÄÄ´ 01-09-97 04:23pm ÃÄÄÄÄÄÄÄþDT=573145842
TAnimSprite.AllocDecBuffer	function	void TAnimSprite::AllocDecBuffer(long decbufsize)
TAnimSprite.FreeDecBuffer	function	void TAnimSprite::FreeDecBuffer()
TAnimSprite.Draw	function	void TAnimSprite::Draw(PTSurface surface)
TAnimSprite.Erase	function	void TAnimSprite::Erase(PTSurface surface)
TAnimSprite.NextFrame	function	void TAnimSprite::NextFrame()
TAnimSprite.Move	function	void TAnimSprite::Move(int deltax, int deltay)
TAnimSprite.Goto	function	void TAnimSprite::Goto(int absx, int absy)
TAnimSprite.SizeMove	function	void TAnimSprite::SizeMove(int deltax, int deltay)
TAnimSprite.SizeGoto	function	void TAnimSprite::SizeGoto(int absx, int absy)
TAnimSprite.Done	function	BOOL TAnimSprite::Done()
                                                            þ
ÄÄ´ SPRITE~1.CPP  ÃÄÄÄÄÄÄ´ 01-09-97 04:23pm ÃÄÄÄÄÄÄÄþDT=573145842
NumSprites	function	int NumSprites()
FreeSprites	function	int FreeSprites()
TSpriteList.AllocPlayer	function	PTAnimSprite TSpriteList::AllocPlayer(PTAnimation newani)
TSpriteList.FreePlayer	function	void TSpriteList::FreePlayer(PTAnimSprite player)
TSpriteList.AnimationDone	function	BOOL TSpriteList::AnimationDone(PTAnimSprite queuenum)
TSpriteList.Clear	function	void TSpriteList::Clear()
TSpriteList.Animate	function	void TSpriteList::Animate(PTSurface surface)
                                                            þ
ÄÄ´ TEXTBAR.CPP   ÃÄÄÄÄÄÄ´ 09-02-97 01:07pm ÃÄÄÄÄÄÄÄþDT=589457660
TTextBar.Initialize	function	BOOL TTextBar::Initialize()
TTextBar.Close	function	void TTextBar::Close()
TTextBar.DrawBackground	function	void TTextBar::DrawBackground()
TTextBar.Print	function	void TTextBar::Print(char *txt)
                                                            þ
ÄÄ´ STATPANE.CPP  ÃÄÄÄÄÄÄ´ 10-02-97 11:38am ÃÄÄÄÄÄÄÄþDT=591551690
BtnStatStats	function	void BtnStatStats()
BtnStatSkills	function	void BtnStatSkills()
BtnStatMin	function	void BtnStatMin()
BtnStatMax	function	void BtnStatMax()
BtnStatUp	function	void BtnStatUp()
BtnStatDown	function	void BtnStatDown()
NUMLINES	#define	#define NUMLINES	6
SCROLLRATE	#define	#define SCROLLRATE	8
TEXT_POS_Y	#define	#define TEXT_POS_Y	22
TStatPane.Initialize	function	BOOL TStatPane::Initialize()
TStatPane.Close	function	void TStatPane::Close()
GEM	#define	#define GEM			(char)('~' + 13)
GEM_LINE	#define	#define GEM_LINE	(char)(GEM + 1)
LINE	#define	#define LINE		(char)(GEM_LINE + 1)
LINE_BRANCH	#define	#define LINE_BRANCH	(char)(LINE + 1)
LINE_END	#define	#define LINE_END	(char)(LINE_BRANCH + 1)
TStatPane.DrawBackground	function	void TStatPane::DrawBackground()
TStatPane.MouseClick	function	void TStatPane::MouseClick(int button, int x, int y)
TStatPane.MouseMove	function	void TStatPane::MouseMove(int button, int x, int y)
TStatPane.Scroll	function	void TStatPane::Scroll(int numlines)
TStatPane.SetAllExpanded	function	void TStatPane::SetAllExpanded(BOOL value)
TStatPane.OnSlot	function	int TStatPane::OnSlot(int x, int y)
                                                            þ
ÄÄ´ SOUND.CPP     ÃÄÄÄÄÄÄ´ 10-02-97 11:49am ÃÄÄÄÄÄÄÄþDT=591552061
CDPlayTrack	function	void CDPlayTrack(int track)
CDPlayRandomTrack	function	void CDPlayRandomTrack()
CDStop	function	void CDStop()
CDPlaying	function	BOOL CDPlaying()
CDSetVolume	function	void CDSetVolume(WORD volume)
TSound.Load	function	PTSound TSound::Load(int resource)
TSound.Load	function	PTSound TSound::Load(PTWaveData wave)
TSound.Play	function	void TSound::Play(int volume)
TSound.Stop	function	void TSound::Stop()
TSound.GetStatus	function	DWORD TSound::GetStatus()
TSoundPlayer.Initialize	function	BOOL TSoundPlayer::Initialize()
TSoundPlayer.Close	function	void TSoundPlayer::Close()
TSoundPlayer.Pause	function	void TSoundPlayer::Pause()
TSoundPlayer.Unpause	function	void TSoundPlayer::Unpause()
TSoundPlayer.SetVolume	function	void TSoundPlayer::SetVolume(int volume)
TSoundPlayer.FindSound	function	PSSoundRef TSoundPlayer::FindSound(char *soundname, int nr)
TSoundPlayer.UpdateDying	function	void TSoundPlayer::UpdateDying()
TSoundPlayer.Mount	function	void TSoundPlayer::Mount(char *soundname, int nr)
TSoundPlayer.Unmount	function	void TSoundPlayer::Unmount(char *soundname, int nr)
TSoundPlayer.Play	function	void TSoundPlayer::Play(char *soundname, int nr, int volume)
TSoundPlayer.Stop	function	void TSoundPlayer::Stop(char *soundname)
TSoundPlayer.ReadSoundList	function	void TSoundPlayer::ReadSoundList()
                                                            þ
ÄÄ´ STREAM.CPP    ÃÄÄÄÄÄÄ´ 01-09-97 04:23pm ÃÄÄÄÄÄÄÄþDT=573145842
TOutputStream.TOutputStream	function	TOutputStream::TOutputStream(int startsize, int grwsize)
TOutputStream.~TOutputStream	function	TOutputStream::~TOutputStream()
TOutputStream.MakeFreeSpace	function	void TOutputStream::MakeFreeSpace(int freespace)
                                                            þ
ÄÄ´ PLAYER.CPP    ÃÄÄÄÄÄÄ´ 10-02-97 11:37am ÃÄÄÄÄÄÄÄþDT=591551651
REGISTER_BUILDER	function	REGISTER_BUILDER(TPlayer)
TPlayer.AdjustSkill	function	int TPlayer::AdjustSkill(int skillnum)
TPlayer.ClearPlayer	function	void TPlayer::ClearPlayer()
TPlayer.Move	function	DWORD TPlayer::Move()
TPlayer.GetOnYerHog	function	void TPlayer::GetOnYerHog()
                                                            þ
ÄÄ´ CURSOR.CPP    ÃÄÄÄÄÄÄ´ 05-06-97 11:54am ÃÄÄÄÄÄÄÄþDT=581328592
SetMouseBitmap	function	void SetMouseBitmap(PTBitmap cursor)
SetMouseAnim	function	void SetMouseAnim(PTAnimation anim)
SetDragBitmap	function	void SetDragBitmap(PTBitmap drag, int x, int y)
ClearDragBitmap	function	void ClearDragBitmap()
DrawMouseCursor	function	void DrawMouseCursor()
                                                            þ
ÄÄ´ AUTOMAP.CPP   ÃÄÄÄÄÄÄ´ 10-03-97 06:12pm ÃÄÄÄÄÄÄÄþDT=591630739
FILTER_SIZE	#define	#define FILTER_SIZE 5
FILTER_PERCENT	#define	#define FILTER_PERCENT 90
FILTER_CUTOFF	#define	#define FILTER_CUTOFF FILTER_SIZE * FILTER_SIZE * FILTER_PERCENT / 100
TAutoMap.Initialize	function	BOOL TAutoMap::Initialize()
TAutoMap.Close	function	void TAutoMap::Close()
TAutoMap.DrawBackground	function	void TAutoMap::DrawBackground()
TAutoMap.DrawPlayerMarker	function	void TAutoMap::DrawPlayerMarker(int PlayerX, int PlayerY)
TAutoMap.RestorePlayerMarker	function	void TAutoMap::RestorePlayerMarker()
TAutoMap.DrawAutoMap	function	void TAutoMap::DrawAutoMap()
TAutoMap.MergePixel	function	inline WORD TAutoMap::MergePixel(WORD *Background, WORD *Overlay, int Mul, int Div)
TAutoMap.MergePixelToBlack	function	inline WORD TAutoMap::MergePixelToBlack(WORD *Overlay, int Mul, int Div)
TAutoMap.LoadMapGraphics	function	BOOL TAutoMap::LoadMapGraphics(int MapNum)
TAutoMap.FreeMapGraphics	function	void TAutoMap::FreeMapGraphics()
TAutoMap.RecordTravels	function	void TAutoMap::RecordTravels()
TAutoMap.CheckMaps	function	void TAutoMap::CheckMaps()
TAutoMap.GetMapNumber	function	int TAutoMap::GetMapNumber(char *MapName)
TAutoMap.GetMaskColorActive	function	BOOL TAutoMap::GetMaskColorActive(int MapNum, int Color)
TAutoMap.GetMaskColorActive	function	BOOL TAutoMap::GetMaskColorActive(char *MapName, int Color)
TAutoMap.SetMaskColorActive	function	void TAutoMap::SetMaskColorActive(int MapNum, int Color, BYTE Active)
TAutoMap.SetMaskColorActive	function	void TAutoMap::SetMaskColorActive(char *MapName, int Color, BYTE Active)
TAutoMap.MouseClick	function	void TAutoMap::MouseClick(int button, int x, int y)
TAutoMap.MouseMove	function	void TAutoMap::MouseMove(int button, int x, int y)
TAutoMap.WriteAutoMapData	function	size_t TAutoMap::WriteAutoMapData(FILE *fp)
TAutoMap.ReadAutoMapData	function	size_t TAutoMap::ReadAutoMapData(FILE *fp)
TAutoMap.GetPlayerOnAutoMap	function	void TAutoMap::GetPlayerOnAutoMap(int &PlayerX, int &PlayerY)
                                                            þ
ÄÄ´ DIRECT~1.CPP  ÃÄÄÄÄÄÄ´ 08-15-97 05:51pm ÃÄÄÄÄÄÄÄþDT=588222060
INITGUID	#define	#define INITGUID
InitDirectDraw	function	BOOL InitDirectDraw()
InitD3DDevice	function	BOOL InitD3DDevice()
SetExclusiveMode	function	BOOL SetExclusiveMode()
CloseDirectDraw	function	BOOL CloseDirectDraw()
EnterVideoMode	function	BOOL EnterVideoMode(int width, int height, int bitdepth)
Setup3D	function	BOOL Setup3D(int width, int height)
CloseScene	function	void CloseScene()
GetTotalVRAM	function	int GetTotalVRAM()
GetAvailableVRAM	function	int GetAvailableVRAM()
GetColorMode	function	int GetColorMode()
IsUsingHardware	function	BOOL IsUsingHardware()
CapsData	struct	{
NUMCAPS1	#define	#define NUMCAPS1 32
NUMCAPS2	#define	#define NUMCAPS2 2
NUMCKEYCAPS	#define	#define NUMCKEYCAPS 19
ALPHACAPS	#define	#define ALPHACAPS 10
PALCAPS	#define	#define PALCAPS 9
                                                            þ
ÄÄ´ WEAPON.CPP    ÃÄÄÄÄÄÄ´ 01-22-97 10:50am ÃÄÄÄÄÄÄÄþDT=573986380
REGISTER_BUILDER	function	REGISTER_BUILDER(TWeapon)
                                                            þ
ÄÄ´ CHUNKC~1.CPP  ÃÄÄÄÄÄÄ´ 09-25-97 05:52pm ÃÄÄÄÄÄÄÄþDT=590974606
TChunkCache.TChunkCache	function	TChunkCache::TChunkCache()
TChunkCache.AllocCache	function	void TChunkCache::AllocCache(int megabytes)
TChunkCache.MemUsed	function	DWORD TChunkCache::MemUsed()
TChunkCache.AddChunk	function	void *TChunkCache::AddChunk(void *chunk, DWORD type)
TChunkCache.AddChunkZ	function	void *TChunkCache::AddChunkZ(void *chunk, DWORD type)
TChunkCache.AddChunk16	function	void *TChunkCache::AddChunk16(void *chunk, DWORD type)
TChunkCache.RemoveChunk	function	BOOL TChunkCache::RemoveChunk(int number)
TChunkCache.RemoveChunk16	function	BOOL TChunkCache::RemoveChunk16(int number)
TChunkCache.ChunkDecompress	function	int TChunkCache::ChunkDecompress(void *source, void *dest, DWORD clear)
TChunkCache.ChunkDecompressZ	function	int TChunkCache::ChunkDecompressZ(void *source, void *dest, DWORD clear)
TChunkCache.ChunkDecompress16	function	int TChunkCache::ChunkDecompress16(void *source, void *dest, DWORD clear)
TChunkCache.~TChunkCache	function	TChunkCache::~TChunkCache()
                                                            þ
ÄÄ´ SHADOW.CPP    ÃÄÄÄÄÄÄ´ 05-27-97 12:32pm ÃÄÄÄÄÄÄÄþDT=582706200
REGISTER_BUILDER	function	REGISTER_BUILDER(TShadow)
                                                            þ
ÄÄ´ HOLEINST.CPP  ÃÄÄÄÄÄÄ´ 06-10-97 05:23am ÃÄÄÄÄÄÄÄþDT=583674612
REGISTER_BUILDER	function	REGISTER_BUILDER(THoleInst)
THoleInst.ClearHole	function	void THoleInst::ClearHole()
THoleInst.THoleInst	function	THoleInst::THoleInst() : TObjectInstance()
THoleInst.THoleInst	function	THoleInst::THoleInst(S3DPoint p, int face)
THoleInst.THoleInst	function	THoleInst::THoleInst(PTObjectImagery img) : TObjectInstance(img)
THoleInst.THoleInst	function	THoleInst::THoleInst(PSObjectDef def, PTObjectImagery img) : TObjectInstance(img)
THoleInst.~THoleInst	function	THoleInst::~THoleInst()
THoleInst.RepaintObject	function	void THoleInst::RepaintObject()
THoleInst.AddToInventory	function	int THoleInst::AddToInventory(int item)
THoleInst.SendToInventory	function	void THoleInst::SendToInventory(int nextcontent, int slot)
THoleInst.RedrawBackground	function	void THoleInst::RedrawBackground(int bgdraw)
THoleInst.Move	function	DWORD THoleInst::Move()
THoleInst.Pulse	function	void THoleInst::Pulse()
THoleInst.Animate	function	void THoleInst::Animate(BOOL draw)
THoleInst.AI	function	void THoleInst::AI()
THoleInst.OnScreen	function	void THoleInst::OnScreen()
THoleInst.OffScreen	function	void THoleInst::OffScreen()
THoleInst.RedrawLight	function	void THoleInst::RedrawLight()
THoleInst.GetLightRect	function	void THoleInst::GetLightRect(RSRect r)
THoleInst.SetLightIntensity	function	void THoleInst::SetLightIntensity(BYTE newintensity)
THoleInst.SetLightMultiplier	function	void THoleInst::SetLightMultiplier(int mult)
THoleInst.SetLightPos	function	void THoleInst::SetLightPos(RS3DPoint newpos)
THoleInst.SetLightColor	function	void THoleInst::SetLightColor(SColor color)
THoleInst.SetCommandDone	function	void THoleInst::SetCommandDone(BOOL newcmd)
THoleInst.Load	function	void THoleInst::Load(RTInputStream is)
THoleInst.Save	function	void THoleInst::Save(RTOutputStream os)
                                                            þ
ÄÄ´ LIGHTN~1.CPP  ÃÄÄÄÄÄÄ´ 05-17-97 05:58pm ÃÄÄÄÄÄÄÄþDT=582061895
SLightning	struct	{
for	function	for (int calc = 0; calc < 256; calc++)
for	function	for (int loop = 0; loop < 640; loop += 80)
                                                            þ
ÄÄ´ LIGHTN~2.CPP  ÃÄÄÄÄÄÄ´ 06-10-97 05:09am ÃÄÄÄÄÄÄÄþDT=583674168
REGISTER_BUILDER	function	REGISTER_BUILDER(TLightningInst)
TLightningInst.ClearLightning	function	void TLightningInst::ClearLightning()
TLightningInst.TLightningInst	function	TLightningInst::TLightningInst() : TObjectInstance()
TLightningInst.TLightningInst	function	TLightningInst::TLightningInst(S3DPoint p, int face)
TLightningInst.TLightningInst	function	TLightningInst::TLightningInst(PTObjectImagery img) : TObjectInstance(img)
TLightningInst.TLightningInst	function	TLightningInst::TLightningInst(PSObjectDef def, PTObjectImagery img) : TObjectInstance(img)
TLightningInst.~TLightningInst	function	TLightningInst::~TLightningInst()
TLightningInst.RepaintObject	function	void TLightningInst::RepaintObject()
TLightningInst.AddToInventory	function	int TLightningInst::AddToInventory(int item)
TLightningInst.SendToInventory	function	void TLightningInst::SendToInventory(int nextcontent, int slot)
TLightningInst.RedrawBackground	function	void TLightningInst::RedrawBackground(int bgdraw)
TLightningInst.Move	function	DWORD TLightningInst::Move()
TLightningInst.Pulse	function	void TLightningInst::Pulse()
TLightningInst.Animate	function	void TLightningInst::Animate(BOOL draw)
TLightningInst.AI	function	void TLightningInst::AI()
TLightningInst.OnScreen	function	void TLightningInst::OnScreen()
TLightningInst.OffScreen	function	void TLightningInst::OffScreen()
TLightningInst.RedrawLight	function	void TLightningInst::RedrawLight()
TLightningInst.GetLightRect	function	void TLightningInst::GetLightRect(RSRect r)
TLightningInst.SetLightIntensity	function	void TLightningInst::SetLightIntensity(BYTE newintensity)
TLightningInst.SetLightMultiplier	function	void TLightningInst::SetLightMultiplier(int mult)
TLightningInst.SetLightPos	function	void TLightningInst::SetLightPos(RS3DPoint newpos)
TLightningInst.SetLightColor	function	void TLightningInst::SetLightColor(SColor color)
TLightningInst.SetCommandDone	function	void TLightningInst::SetCommandDone(BOOL newcmd)
TLightningInst.Load	function	void TLightningInst::Load(RTInputStream is)
TLightningInst.Save	function	void TLightningInst::Save(RTOutputStream os)
TLightningInst.AdjustLightning	function	void TLightningInst::AdjustLightning()
                                                            þ
ÄÄ´ LIGHTN~3.CPP  ÃÄÄÄÄÄÄ´ 06-13-97 05:21pm ÃÄÄÄÄÄÄÄþDT=583895725
TLightningAnimator.TLightningAnimator	function	TLightningAnimator::TLightningAnimator(PTObjectInstance oi) : TObjectAnimator(oi)
TLightningAnimator.Animate	function	void TLightningAnimator::Animate(BOOL draw)
TLightningAnimator.~TLightningAnimator	function	TLightningAnimator::~TLightningAnimator()
                                                            þ
ÄÄ´ DEATH.CPP     ÃÄÄÄÄÄÄ´ 10-03-97 12:26pm ÃÄÄÄÄÄÄÄþDT=591618887
DeathBtnRestart	function	void DeathBtnRestart()
DeathBtnLoad	function	void DeathBtnLoad()
DeathBtnExit	function	void DeathBtnExit()
TDeathPane.Initialize	function	BOOL TDeathPane::Initialize()
TDeathPane.Close	function	void TDeathPane::Close()
TDeathPane.DrawBackground	function	void TDeathPane::DrawBackground()
                                                            þ
ÄÄ´ TALISMAN.CPP  ÃÄÄÄÄÄÄ´ 01-30-97 01:14pm ÃÄÄÄÄÄÄÄþDT=574515648
REGISTER_BUILDER	function	REGISTER_BUILDER(TTalisman)
                                                            þ
ÄÄ´ DECOMP~2.CPP  ÃÄÄÄÄÄÄ´ 06-23-97 11:21am ÃÄÄÄÄÄÄÄþDT=584538803
AlphaDecompress8	function	BOOL AlphaDecompress8(PSDrawBlock db, PSDrawParam dp, DWORD *palette)
AlphaStaticZDecompress8	function	BOOL AlphaStaticZDecompress8(PSDrawBlock db, PSDrawParam dp, DWORD *palette)
ZBufferDecompress8	function	BOOL ZBufferDecompress8(PSDrawBlock db, PSDrawParam dp, DWORD *palette)
ClipZBufferDecompress8	function	BOOL ClipZBufferDecompress8(PSDrawBlock db, PSDrawParam dp, DWORD *palette)
                                                            þ
ÄÄ´ DECOMP~3.CPP  ÃÄÄÄÄÄÄ´ 05-27-97 12:13pm ÃÄÄÄÄÄÄÄþDT=582705590
ZBufferDecompress832	function	BOOL ZBufferDecompress832(PSDrawBlock db, PSDrawParam dp, DWORD *palette)
ClipZBufferDecompress832	function	BOOL ClipZBufferDecompress832(PSDrawBlock db, PSDrawParam dp, DWORD *palette)
                                                            þ
ÄÄ´ DECOMP~4.CPP  ÃÄÄÄÄÄÄ´ 06-18-97 06:10pm ÃÄÄÄÄÄÄÄþDT=584225112
Decompress	function	BOOL Decompress(PSDrawBlock db, PSDrawParam dp, BOOL Transparent)
ClipDecompress	function	BOOL ClipDecompress(PSDrawBlock db, PSDrawParam dp, BOOL Transparent)
AlphaDecompress	function	BOOL AlphaDecompress(PSDrawBlock db, PSDrawParam dp)
AlphaStaticZDecompress	function	BOOL AlphaStaticZDecompress(PSDrawBlock db, PSDrawParam dp)
ZBufferDecompress	function	BOOL ZBufferDecompress(PSDrawBlock db, PSDrawParam dp)
ClipZBufferDecompress	function	BOOL ClipZBufferDecompress(PSDrawBlock db, PSDrawParam dp)
ZNormalDecompress	function	BOOL ZNormalDecompress(PSDrawBlock db, PSDrawParam dp)
ZFindDecompress	function	WORD ZFindDecompress(PSDrawBlock db, PSDrawParam dp)
                                                            þ
ÄÄ´ FOOD.CPP      ÃÄÄÄÄÄÄ´ 05-06-97 12:56pm ÃÄÄÄÄÄÄÄþDT=581330697
REGISTER_BUILDER	function	REGISTER_BUILDER(TFood)
TFood.Use	function	BOOL TFood::Use(int with)
                                                            þ
ÄÄ´ HELPER.CPP    ÃÄÄÄÄÄÄ´ 05-13-97 04:52pm ÃÄÄÄÄÄÄÄþDT=581797504
REGISTER_BUILDER	function	REGISTER_BUILDER(THelper)
                                                            þ
ÄÄ´ MULTIA~1.CPP  ÃÄÄÄÄÄÄ´ 07-24-97 05:44pm ÃÄÄÄÄÄÄÄþDT=586714497
                                                            þ
ÄÄ´ RANGED~1.CPP  ÃÄÄÄÄÄÄ´ 08-29-97 09:21pm ÃÄÄÄÄÄÄÄþDT=589146791
REGISTER_BUILDER	function	REGISTER_BUILDER(TRangedWeapon)
                                                            þ
ÄÄ´ INPUT.CPP     ÃÄÄÄÄÄÄ´ 03-24-97 02:41pm ÃÄÄÄÄÄÄÄþDT=578319649
TRY_DI	#define	#define TRY_DI(exp) { { HRESULT rval = exp; if ((rval != DI_OK) && (rval != DIERR_ACQUIRED)) { TraceErrorDI(rval, __FILE__, __LI
TRY_JOY	#define	#define TRY_JOY(exp) { { HRESULT rval = exp; if (rval != JOYERR_NOERROR) { TraceErrorJoy(rval, __FILE__, __LINE__); return FALSE
TInput.TInput	function	TInput::TInput()
TInput.InitKeyboard	function	BOOL TInput::InitKeyboard()
TInput.InitMouse	function	BOOL TInput::InitMouse()
TInput.InitJoystick	function	BOOL TInput::InitJoystick()
TInput.MouseInput	function	BOOL TInput::MouseInput(PTInput input)
TInput.KeyboardInput	function	BOOL TInput::KeyboardInput(PTInput input)
TInput.ReleaseKeyboard	function	BOOL TInput::ReleaseKeyboard()
TInput.ReleaseMouse	function	BOOL TInput::ReleaseMouse()
TInput.ReleaseJoystick	function	BOOL TInput::ReleaseJoystick()
TInput.~TInput	function	TInput::~TInput()
TraceErrorDI	function	void TraceErrorDI(HRESULT Err, char *file, int line)
TraceErrorJoy	function	void TraceErrorJoy(HRESULT Err, char *file, int line)
ConvertToVK	function	BYTE ConvertToVK(BYTE code)
                                                            þ
ÄÄ´ D3DMATH.CPP   ÃÄÄÄÄÄÄ´ 04-21-97 12:32pm ÃÄÄÄÄÄÄÄþDT=580215830
MATRIX	typedef	typedef D3DVALUE MATRIX[4][4];
D3DVECTORNormalise	function	D3DVECTORNormalise(LPD3DVECTOR v)
D3DVECTORCrossProduct	function	D3DVECTORCrossProduct(LPD3DVECTOR lpd, LPD3DVECTOR lpa, LPD3DVECTOR lpb)
D3DMATRIXClear	function	LPD3DMATRIX D3DMATRIXClear(LPD3DMATRIX lpMat)
MultiplyD3DMATRIX	function	LPD3DMATRIX MultiplyD3DMATRIX(LPD3DMATRIX lpDst, LPD3DMATRIX lpSrc1,
D3DMATRIXTransform	function	void D3DMATRIXTransform(LPD3DMATRIX lpm, LPD3DVECTOR lpV, LPD3DVECTOR lpD)
D3DMATRIXInvert	function	LPD3DMATRIX D3DMATRIXInvert(LPD3DMATRIX d, LPD3DMATRIX a)
D3DMATRIXSetRotation	function	LPD3DMATRIX D3DMATRIXSetRotation(LPD3DMATRIX lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU)
D3DMATRIXTranslate	function	LPD3DMATRIX D3DMATRIXTranslate(LPD3DMATRIX lpDst, LPD3DVECTOR lpV)
D3DMATRIXMove	function	LPD3DMATRIX D3DMATRIXMove(LPD3DMATRIX lpDst, LPD3DVECTOR lpV)
D3DMATRIXScale	function	LPD3DMATRIX D3DMATRIXScale(LPD3DMATRIX lpDst, LPD3DVECTOR lpV)
D3DMATRIXRotateX	function	LPD3DMATRIX D3DMATRIXRotateX(LPD3DMATRIX lpDst, D3DVALUE a)
D3DMATRIXRotateY	function	LPD3DMATRIX D3DMATRIXRotateY(LPD3DMATRIX lpDst, D3DVALUE a)
D3DMATRIXRotateZ	function	LPD3DMATRIX D3DMATRIXRotateZ(LPD3DMATRIX lpDst, D3DVALUE a)
spline	function	spline(LPD3DVECTOR p, float t, LPD3DVECTOR p1, LPD3DVECTOR p2,
                                                            þ
ÄÄ´ TOOL.CPP      ÃÄÄÄÄÄÄ´ 05-01-97 10:48am ÃÄÄÄÄÄÄÄþDT=580998678
REGISTER_BUILDER	function	REGISTER_BUILDER(TTool)
                                                            þ
ÄÄ´ LIGHTS~1.CPP  ÃÄÄÄÄÄÄ´ 05-01-97 11:04am ÃÄÄÄÄÄÄÄþDT=580999299
REGISTER_BUILDER	function	REGISTER_BUILDER(TLightSource)
                                                            þ
ÄÄ´ BLOOD.CPP     ÃÄÄÄÄÄÄ´ 09-24-97 09:15am ÃÄÄÄÄÄÄÄþDT=590891508
REGISTER_BUILDER	function	REGISTER_BUILDER(TBlood)
                                                            þ
ÄÄ´ EFFECT.CPP    ÃÄÄÄÄÄÄ´ 09-29-97 09:39am ÃÄÄÄÄÄÄÄþDT=591219958
REGISTER_BUILDER	function	REGISTER_BUILDER(TEffect)
                                                            þ
ÄÄ´ BOX.CPP       ÃÄÄÄÄÄÄ´ 09-30-97 11:15am ÃÄÄÄÄÄÄÄþDT=591288823
new	#define	#define new DEBUG_NEW
CBox.CBox	function	CBox::CBox(char *BoxName, int Access)
CBox.~CBox	function	CBox::~CBox()
CBox.UpdateBoxNow	function	BOOL CBox::UpdateBoxNow()
CBox.GetFileDirEntry	function	long CBox::GetFileDirEntry(char *FileName)
CBox.ReadFile	function	void *CBox::ReadFile(char *FileName, void *Buffer, long *FileSize)
CBox.AddFile	function	BOOL CBox::AddFile(char *FileName, void *Buffer, long FileSize, BOOL Compress)
CBox.StartFileOutput	function	int CBox::StartFileOutput()
CBox.EndFileOutput	function	BOOL CBox::EndFileOutput(char *FileName, long FileSize, long CheckSum)
CBox.GetFileHandle	function	int CBox::GetFileHandle(char *FileName)
CBox.GetFileSize	function	long CBox::GetFileSize(char *FileName)
CBox.CalcCheckSum	function	long CBox::CalcCheckSum(BYTE *Data, long Size)
CBox.GetCRC	function	DWORD CBox::GetCRC(BYTE *Data, long Size)
CBox.InitCRCTable	function	void CBox::InitCRCTable()
                                                            þ
ÄÄ´ LZSS.CPP      ÃÄÄÄÄÄÄ´ 09-30-97 10:48am ÃÄÄÄÄÄÄÄþDT=591287813
CLZSS.CLZSS	function	CLZSS::CLZSS()
CLZSS.~CLZSS	function	CLZSS::~CLZSS()
CLZSS.InitTree	function	void CLZSS::InitTree()
CLZSS.InsertNode	function	void CLZSS::InsertNode(int r)
CLZSS.DeleteNode	function	void CLZSS::DeleteNode(int p)
CLZSS.PutByte	function	int CLZSS::PutByte(BYTE b)
CLZSS.LZSS	function	long CLZSS::LZSS(int DestFile, BYTE *Buffer, long DataSize, long &CheckSum)
CLZSS.GetByte	function	int CLZSS::GetByte()
CLZSS.UnLZSS	function	long CLZSS::UnLZSS(int SourceFile, BYTE *Buffer, long CompressedSize)
