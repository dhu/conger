//
// C++ Implementation: bot_waypoint.h
//
// Description: Containts all defintions for bot waypoint code
//
//
// Author: Rick Helmus <rickhelmus@gmail.com>, (C) 2004
//

// Enable this for flood support
//#define WP_FLOOD

#ifndef BOT_WAYPOINT_H
#define BOT_WAYPOINT_H

#define MAX_PATH_INDEX 4
#define MAX_WAYPOINTS 1024
#define WAYPOINT_VERSION 1
#define WAYPOINT_UNREACHABLE   USHRT_MAX
#define WAYPOINT_MAX_DISTANCE (USHRT_MAX-1)
#define REACHABLE_RANGE 15.0f
#define MAX_MAP_GRIDS    32
#define SECTOR_SIZE      8

#define W_FL_DELETED          (1<<0) // used if waypoint isn't used(deleted)
#define W_FL_TELEPORT         (1<<1) // used if waypoint is at a teleporter
#define W_FL_TELEPORTDEST     (1<<2) // used if waypoint is at a teleporter destination
#define W_FL_FLOOD            (1<<3) // used if this waypoint is made by flooding the map
#define W_FL_JUMP             (1<<4) // if set, bot jumps when reached this waypoint

#define MAX_FLOODWPCONNECTDIST     2
#define MAX_FLOODWPDIST            4.0f

struct waypoint_header_s
{
    char szFileType[10];
    int iWPFileVersion;
    int iWPFileFlags;  // not currently used
    int iWPCount;
    char szMapName[32];  // name of map for these waypoints
};

struct waypoint_s
{
     int iFlags;
     vec v_origin;
     TLinkedList<waypoint_s *> ConnectedWPs;
     
     // A* stuff
     float f, g;
     waypoint_s *pParent;
     
     // Construction
     waypoint_s(void) : iFlags(0), f(0.0f), g(0.0f), pParent(NULL) { };     
};

enum EFloodDir
{
	FDIR_LEFT=0,
	FDIR_RIGHT,
	FDIR_FORWARD,
	FDIR_BACK,
	FDIR_NONE
};

class CWaypointClass
{
     bool m_bDrawWaypoints;
     bool m_bDrawWPPaths;
     bool m_bAutoWaypoint;
     bool m_bAutoPlacePaths;
     bool m_bDrawWPText;
     vec m_vLastCreatedWP;
     float m_fPathDrawTime;
     
#ifdef WP_FLOOD
     // Waypoint flood variabeles
     bool m_bFlooding;
     TLinkedList<waypoint_s*> m_FloodWPs[MAX_MAP_GRIDS][MAX_MAP_GRIDS];
     TLinkedList<waypoint_s*> m_FloodWPsToCheck;
#endif

public:
     waypoint_s m_Waypoints[MAX_WAYPOINTS];
     int m_iWaypointCount; // number of waypoints currently in use

     CWaypointClass(void);
     ~CWaypointClass(void) { Clear(); };

     void Think(void);
     void Init(void);
     void Clear(void);
     bool LoadWaypoints(const char *szMapName);
     void SaveWaypoints(void);

     void SetWaypointsVisible(bool Visible) { m_bDrawWaypoints = Visible; };
     void SetPathsVisible(bool Visible) { m_bDrawWPPaths = Visible; };
     void SetAutoWaypoint(bool On) { m_bAutoWaypoint = On; };
     void SetWPInfo(bool on) { m_bDrawWPText = on; };
     void SetWPFlags(waypoint_s *wp, int iFlags) { wp->iFlags |= iFlags; };
     void UnsetWPFlags(waypoint_s *wp, int iFlags) { wp->iFlags &= ~iFlags; };
     bool WaypointsAreVisible(void) { return m_bDrawWaypoints; };
     waypoint_s *AddWaypoint(vec o, bool connectwp);
     void DeleteWaypoint(dynent *d);
     void AddPath(waypoint_s *pWP1, waypoint_s *pWP2);
     void DeletePath(waypoint_s *pWP);
     void DeletePath(waypoint_s *pWP1, waypoint_s *pWP2);
     void ManuallyCreatePath(dynent *d, int iCmd, bool TwoWay);
     void ManuallyDeletePath(dynent *d, int iCmd, bool TwoWay);
     void CreateWPsAtTeleporters(void);

     bool WPIsReachable(vec o, short sWaypointIndex);
     bool IsValidWPPos(vec pos);
     waypoint_s *GetNearestWaypoint(dynent *d, float Range);
     waypoint_s *GetNearestWaypoint(vec v_src, dynent *d, float range);
     waypoint_s *GetNearestWaypoint(vec v_src, float range);
     
#ifdef WP_FLOOD
     // Flood functions
     void StartFlood(void);
     void ConnectFloodWP(waypoint_s *pWP);
     void GetFloodWPIndexes(vec v_origin, short *i, short *j);
     waypoint_s *GetNearestFloodWP(vec v_origin, float flRange, waypoint_s *pIgnore=NULL);
     waypoint_s *GetNearestFloodWP(dynent *d, float flRange) { return GetNearestFloodWP(d->o, flRange, NULL); };
     void DrawNearFloodWPs();
#endif
};

#endif // BOT_WAYPOINT_H
