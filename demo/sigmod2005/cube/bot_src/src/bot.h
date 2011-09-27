//
// C++ Interface: bot
//
// Description: 
//
// Main bot header
//
//
// Author:  Rick <rickhelmus@gmail.com>, (C) 2004
//
//
//


/*

     TODO list
     
     -bots that "cover" each other in team matches
     -If a bot gets ammo it should ignore that kind of ammo for the next 10 seconds.
     -bots should ignore players that are too far away
     -bots should dodge/jump when shot at
     -If the bot's only weapon is the fist it should make looking for ammo its priority
     -bots should rocket jump to get quads and the 150 armor if normal jumping doesn't
      work (only if health is x so they don't suicide).
     -bots should have a weapon preferance depending on the distance they are from the
      player or other bot.
     -bots shouldn't be able to "see" behind them properly
     -more names for bots :)
     -bots should "keep their distance" kind of
     -bots should not go into water unless they need to.
     -Make all the "UNDONE's" done :)
     -Finish waypoint navigation
     -Tweak skills(aiming, reaction time)
     -Make A* cost function
     -Make bot personalities
     -More advanced enemy scoring
     -Bots should less bump into walls
     -Bots should remember enemies which are recently out of sight
     -Bots should be able to 'hunt' enemies
*/

//#define RELEASE_BUILD // Set when you want to make a release build

#ifndef BOT_H
#define BOT_H

#include "bot_util.h"
#include "bot_waypoint.h"

#include <fstream>

#ifdef __linux__
#include <sys/syslog.h>
#else // Windows doesn't have syslog
#define openlog     (void)0
#define syslog      (void)0
#define closelog    (void)0
#endif

#ifdef RELEASE_BUILD
inline void condebug(const char *s, int a = 0, int b = 0, int c = 0) {};
inline void debugnav(const char *s, int a = 0, int b = 0, int c = 0) {};
inline void debugbeam(vec &s, vec &e) { };
#else
inline void condebug(const char *s, int a = 0, int b = 0, int c = 0) { conoutf(s, a, b, c); };
inline void debugnav(const char *s, int a = 0, int b = 0, int c = 0) { conoutf(s, a, b, c); };
inline void debugbeam(vec &s, vec &e) { particle_trail(1, 500, s, e); };
#endif

#define JUMP_HEIGHT      4.0f // NOT accurate
#define MAX_WEAPONS      5

enum EDirection
{
     FORWARD=0,
     FORWARD_LEFT,
     FORWARD_RIGHT,
     BACKWARD,
     BACKWARD_LEFT,
     BACKWARD_RIGHT,
     LEFT,
     RIGHT,
     UP,
     DOWN
};

struct bot_skill_s
{
     float flMinReactionDelay; // Minimal reaction time
     float flMaxReactionDelay; // Maximal reaction time
     float flMinAimXSpeed; // Minimal X aim speed in degrees(base value)
     float flMaxAimXSpeed; // Maximal X aim speed in degrees(base value)
     float flMinAimYSpeed; // Minimal Y aim speed in degrees(base value)
     float flMaxAimYSpeed; // Maximal Y aim speed in degrees(base value)
     float flMinAimXOffset; // Minimal X aim offset in degrees(base value)
     float flMaxAimXOffset; // Maximal X aim offset in degrees(base value)
     float flMinAimYOffset; // Minimal Y aim offset in degrees(base value)
     float flMaxAimYOffset; // Maximal Y aim offset in degrees(base value)
     float flMinAttackDelay; // Minimal delay for when a bot can attack again
     float flMaxAttackDelay; // Maximal delay for when a bot can attack again
     float flMinEnemySearchDelay; // Minimal delay for when a bot can search for an
                                  // enemy again
     float flMaxEnemySearchDelay; // Maximal delay for when a bot can search for an
                                  // enemy again
     short sShootAtFeetWithRLPercent; // Percent that a bot shoot with a rocket
                                      // launcher at the enemy feet.
     int iMaxHearVolume; // Max volume that bot can hear
     bool bCanPredict; // Can this bot predict his enemy position?
     bool bCircleStrafe; // Can this bot circle strafe?
     bool bCanSearchItemsInCombat;
};

struct weaponinfo_s
{
     float flMinDesiredDistance;
     float flMaxDesiredDistance;
     float flMinFireDistance;
     float flMaxFireDistance;
     short sMinDesiredAmmo;
};
     
class CBot
{
     dynent *m_pMyEnt;
     int m_iLastBotUpdate;
        
     // Combat variabels
     int m_iShootDelay;
     int m_iChangeWeaponDelay;
     int m_iCombatNavTime;
     int m_iEnemySearchDelay;
     bool m_bCombatJump;
     bool m_bShootAtFeet;
     
     // Navigation variabeles
     int m_iCheckEnvDelay;
     int m_iLastJumpPad;
     vec m_vPrevOrigin;
     float m_iStuckCheckDelay;
     bool m_bStuck;
     int m_iStuckTime;
     int m_iStrafeTime;
     int m_iStrafeCheckDelay;
     int m_iStrafeDir, m_iMoveDir;
     bool m_bCalculatingAStarPath;
     TLinkedList<waypoint_s *> m_AStarNodeList;
     TPriorList<waypoint_s *, float> m_AStarOpenList;
     TLinkedList<waypoint_s *> m_AStarClosedList;
     vec m_vWaterGoal; // Place to go to when underwater
        
     // Waypoint variabeles
     float m_iWaypointTime;
     waypoint_s *m_pCurrentWaypoint;
     waypoint_s *m_pCurrentGoalWaypoint;
     vec m_vGoal;
     waypoint_s *m_pPrevWaypoints[5];
     int m_iRandomWaypointTime;
     float m_fPrevWaypointDistance;
     int m_iLookForWaypointTime;
     int m_iWaypointHeadLastTurnLessTime; // Last time the didn't had to turn much wile
                                          // heading to a WP
     int m_iWaypointHeadPauseTime; // Pause time to give the bot time to aim.
     bool m_bGoToDebugGoal;
        
     // Misc stuff
     entity *m_pTargetEnt;
     int m_iCheckTeleporterDelay;
     int m_iCheckJumppadsDelay;
     int m_iCheckEntsDelay;
     int m_iAimDelay;
     float m_fYawToTurn, m_fPitchToTurn;
     bot_skill_s *m_pBotSkill; // Pointer to current bot skill
     short m_sSkillNr; // Skill number, 0 == best 4 == worst
        
     void AimToVec(const vec &o);
     void AimToIdeal(void);
     float ChangeAngle(float speed, float ideal, float current);
     bool SelectGun(int Gun);
     void CheckItems(void);
     void CheckQuad(int Time);
     void PickUp(int n);
     void AddItem(int i, int &v, int spawnsec);
     void SendBotInitTuple(void); // Added by yna.
     void SendBotTuple(void); // Added by yna.
     //void SendBotInfo(void);
     float GetDistance(const vec &o);
     float GetDistance(const vec &v1, const vec &v2);
     float GetDistance(entity *e);
     float Get2DDistance(const vec &v) { return ::Get2DDistance(m_pMyEnt->o, v); };
     vec GetViewAngles(void) { vec temp = { m_pMyEnt->pitch, m_pMyEnt->yaw,
                               m_pMyEnt->roll }; return temp; };
     void ResetMoveSpeed(void) { m_pMyEnt->move = m_pMyEnt->strafe = 0; };
     void LogBotMotion(); // Added by yna.
     
     // AI Functions
     bool FindEnemy(void);
     void ShootEnemy(void);
     void DoCombatNav(void);
     void HomeThroughMap(void);
     bool CheckStuck(void);
     bool CheckJump(void);
     bool CheckStrafe(void);
     void CheckFOV(void);
     bool IsVisible(const vec &o) { return ::IsVisible(m_pMyEnt->o, o, m_pMyEnt); };
     bool IsVisible(dynent *d) { return ::IsVisible(m_pMyEnt->o, d->o, m_pMyEnt); }; 
     bool IsVisible(entity *e);
     bool IsVisible(vec o, EDirection Dir, float flDist, bool CheckPlayers);
     bool IsVisible(EDirection Dir, float flDist, bool CheckPlayers)
                    { return IsVisible(m_pMyEnt->o, Dir, flDist, CheckPlayers); };
     bool IsInFOV(const vec &o);
     bool IsInFOV(dynent *d) { return IsInFOV(d->o); };       
     int GetShootDelay(void);
     bool ChoosePreferredWeapon(void);
     entity *SearchForEnts(bool bUseWPs, float flRange=9999.0f,
                           float flMaxHeight=JUMP_HEIGHT);
     bool HeadToEnt(void);
     vec GetEnemyPos(dynent *d);
     bool AStar(void);
     float AStarCost(waypoint_s *pWP1, waypoint_s *pWP2);
     bool IsReachable(vec to, float flMaxHeight);
     bool WaterNav(void);
     void HearSound(int n, vec *o);
        
     // Waypoint functions
     bool HeadToWaypoint(void);
     bool FindWaypoint(void);
     void ResetWaypointVars(void);
     void SetCurrentWaypoint(short sIndex);
     void SetCurrentWaypoint(waypoint_s *pWP);
     void SetCurrentGoalWaypoint(waypoint_s *pWP) { m_pCurrentGoalWaypoint = pWP; };
     bool CurrentWPIsValid(void);
     bool ReachedGoalWP(void);
     bool HeadToGoal(void);
        
     friend class CBotManager;
     friend class CWaypointClass;

public:
     bool m_bSendC2SInit;
     
     void Spawn(void);
     void Think(void);
     static dynent *CreateBot(const char *team, const char *skill, const char *name);
     void BotPain(int damage, dynent *d);
     void GoToDebugGoal(vec o);        
};

class CBotManager
{
     char m_szBotNames[150][16]; // Max 150 bot names with a length of 16 characters
     short m_sBotNameCount;
     char m_szBotTeams[20][5]; // Max 100 bot teams with a length of 5 characters
     short m_sBotTeamCount;        
     bool m_bInit;
     bool m_bBotsShoot;
     bool m_bIdleBots;
     bot_skill_s m_BotSkills[5]; // 5 different bot skills, 0 = best 5 = worst
     int m_iFrameTime;
     int m_iPrevTime;
     short m_sBotSkill; // Bad - Worse - Medium - Good - Best
     
     void LoadBotNamesFile(void);
     char *GetBotName(void);
     void LoadBotTeamsFile(void);
     char *GetBotTeam(void);
     void InitSkillData(void);
               
     friend class CBot;
     
     // Logging, added by yna.
     bool m_enable_motion_logging;
     std::ofstream m_motion_log_file;
public:
     dynent *m_pBotToView;

     // Construction
     CBotManager(void) { m_bInit = true; };
     
     // Destruction
     ~CBotManager(void) { RemoveAllBots(); };
     
     void Init(void);
     void Think(void);
     void RenderBots(void);
     void RemoveAllBots(void);
     int  GetBotIndex(dynent *m);
     int  GetPendingBotIndex(dynent *pb);
     void LetBotsUpdateStats(void);
     void LetBotsHear(int n, vec *loc);
     bool BotsShoot(void) { return m_bBotsShoot; };
     bool IdleBots(void) { return m_bIdleBots; };
     void SetBotsShoot(bool bShoot) { m_bBotsShoot = bShoot; };
     void SetIdleBots(bool bIdle) { m_bIdleBots = bIdle; };
     void EnableBotView(dynent *bot) { m_pBotToView = bot; };
     void DisableBotView(void);
     void ChangeBotSkill(short Skill, dynent *bot = NULL);
     void ViewBot(void);
        
     void MakeBotFileName(const char *szFileName, const char *szDir1, const char *szDir2,
                          char *szOutput);        
};
     
extern dvector bots;
extern dvector pending_bots;
extern CBotManager BotManager;
extern const vec g_vecZero;
extern CWaypointClass WaypointClass;

// Logging support
extern bool enable_motion_logging;
extern std::string motion_log_filename;

#endif
