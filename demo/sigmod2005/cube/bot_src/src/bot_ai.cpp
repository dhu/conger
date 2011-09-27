//
// C++ Implementation: bot_ai
//
// Description: The AI part comes here(navigation, shooting etc)
//
//
// Author:  <rickhelmus@gmail.com>, (C) 2004
//


// Code of CBot - Start

#include "cube.h"

struct itemstat { int add, max, sound; };

extern itemstat itemstats[];
extern vector<server_entity> sents;
extern vector<dynent*> monsters;

extern float sheight(sqr *s, sqr *t, float z);

weaponinfo_s WeaponInfoTable[MAX_WEAPONS] =
{
     // FIST
     { 0.0f, 3.5f, 0.0f, 5.0f, 1 },
     // SHOTGUN
     { 5.0f, 12.0f, 0.0f, 50.0f, 4 },
     // CHAINGUN
     { 10.0f, 20.0f, 0.0f, 100.0f, 10 },
     // ROCKETLAUNCHER
     { 15.0f, 25.0f, 10.0f, 70.0f, 3 },
     // RIFLE
     { 12.0f, 25.0f, 0.0f, 100.0f, 4 }
};

bool CBot::FindEnemy(void)
{
     // UNDONE: Enemies are now only scored on their distance
          
     if (m_pMyEnt->enemy) // Bot already has an enemy 
     {
          // Check if the enemy is still in game
          bool found = false;
          if (m_pMyEnt->enemy->monsterstate)
          {
               loopv(monsters)
               {
                    if (monsters[i] == m_pMyEnt->enemy)
                    {
                         found = true;
                         break;
                    }
               }
          }
          else if (m_pMyEnt->enemy->bIsBot)
          {
               loopv(bots)
               {
                    if (bots[i] == m_pMyEnt->enemy)
                    {
                         found = true;
                         break;
                    }
               }
          }
          else
          {
               if (m_pMyEnt->enemy == player1)
                    found = true;
               else
               {
                    loopv(players)
                    {
                         if (players[i] == m_pMyEnt->enemy)
                         {
                              found = true;
                              break;
                         }
                    }
               }
          }
          
          if (!found) m_pMyEnt->enemy = NULL;
               
          // Check if the enemy is still ingame, still alive, not joined my team and is visible
          if (found && (m_pMyEnt->enemy->state == CS_ALIVE) &&
              !isteam(m_pMyEnt->team, m_pMyEnt->enemy->team) &&
              (IsVisible(m_pMyEnt->enemy)))
               return true;
     }
     
     if (m_iEnemySearchDelay > lastmillis) return (m_pMyEnt->enemy!=NULL);
     
     m_pMyEnt->enemy = NULL;
          
     // Add enemy searchy delay
     float MinDelay = m_pBotSkill->flMinEnemySearchDelay;
     float MaxDelay = m_pBotSkill->flMaxEnemySearchDelay;
     m_iEnemySearchDelay = lastmillis + int(RandomFloat(MinDelay, MaxDelay) * 1000.0f);
     
     dynent *pNewEnemy = NULL, *d = NULL;
     float flDist, flNearestDist = 99999.9f;
     short EnemyVal, BestEnemyVal = -100;
     
     // search for only for monsters when SP mode
     if (m_sp)
     {
          loopv(monsters)
          {
               d = monsters[i]; // Handy shortcut
                        
               if (d->state != CS_ALIVE)
                    continue;
          
               // Check if the enemy is visible
               if(!IsInFOV(d) || !IsVisible(d))
                    continue;
          
               flDist = GetDistance(d->o);
               EnemyVal = 1;
          
               if (flDist < flNearestDist)
               {
                    EnemyVal+=2;
                    flNearestDist = flDist;
               }
               
               if (EnemyVal > BestEnemyVal)
               {
                    pNewEnemy = d;
                    BestEnemyVal = EnemyVal;
               }
          }
     }
     else
     {
          // First loop through all players
          loopv(players)
          {
               d = players[i]; // Handy shortcut
          
               if (!d || isteam(d->team, m_pMyEnt->team) || (d->state != CS_ALIVE))
                    continue;
          
               // Check if the enemy is visible
               if(!IsInFOV(d) || !IsVisible(d))
                    continue;
          
               flDist = GetDistance(d->o);
               EnemyVal = 1;
          
               if (flDist < flNearestDist)
               {
                    EnemyVal+=2;
                    flNearestDist = flDist;
               }
               
               if (EnemyVal > BestEnemyVal)
               {
                    pNewEnemy = d;
                    BestEnemyVal = EnemyVal;
               }
          }
          
          // Then loop through all bots
          loopv(bots)
          {
               d = bots[i]; // Handy shortcut
          
               if (d == m_pMyEnt)
                    continue;
               
               if (!d || isteam(d->team, m_pMyEnt->team) || (d->state != CS_ALIVE))
                    continue;
          
               // Check if the enemy is visible
               if(!IsInFOV(d) || !IsVisible(d))
                    continue;
          
               flDist = GetDistance(d->o);
               EnemyVal = 1;
          
               if (flDist < flNearestDist)
               {
                    EnemyVal+=2;
                    flNearestDist = flDist;
               }
               
               if (EnemyVal > BestEnemyVal)
               {
                    pNewEnemy = d;
                    BestEnemyVal = EnemyVal;
               }
          }
     
          // Then examine the local player
          if (player1 && !isteam(player1->team, m_pMyEnt->team) &&
              (player1->state == CS_ALIVE))
          {
               // Check if the enemy is visible
               if(IsInFOV(player1) && IsVisible(player1))
               {
                    flDist = GetDistance(player1->o);
                    EnemyVal = 1;
          
                    if (flDist < flNearestDist)
                    {
                         EnemyVal+=2;
                         flNearestDist = flDist;
                    }
               
                    if (EnemyVal > BestEnemyVal)
                    {
                         pNewEnemy = player1;
                         BestEnemyVal = EnemyVal;
                    }
               }
          }    
     }
          
     if (pNewEnemy)
     {
          if (!m_pMyEnt->enemy) // Add shoot delay if new enemy is found
          {
               float flMinShootDelay = m_pBotSkill->flMinAttackDelay;
               float flMaxShootDelay = m_pBotSkill->flMaxAttackDelay;
               
               m_iShootDelay = lastmillis + int(RandomFloat(flMinShootDelay,
                                                flMaxShootDelay) * 1000.0f);
          }
     
          m_pMyEnt->enemy = pNewEnemy;          
          return true;
     }

     return false;
}

vec CBot::GetEnemyPos(dynent *d)
{
     // Aim offset idea by botman
     //return d->o; // Disabled for now
     vec o = d->o, offset;
     float flDist = GetDistance(d->o), flScale;
     
     if (m_pMyEnt->gunselect == GUN_RL)
     {
          // Bot is using a rocket launcher, aim at enemy feet?
          if (m_bShootAtFeet && !OUTBORD(m_pMyEnt->enemy->o.x, m_pMyEnt->enemy->o.y))
          {
               // Only do this when enemy is fairly close to the ground
               sqr *s = S(fast_f2nat(m_pMyEnt->enemy->o.x),
                          fast_f2nat(m_pMyEnt->enemy->o.y));
               float z = sheight(s, s, 1.0f);
               if (!SOLID(s) && (m_pMyEnt->enemy->o.z - z) < 8.0f)
               {
                    vec to = o;
                    to.z = z;
                    if (IsVisible(to))
                    {
                         // Target at ground
                         o.z = z;
                    }
               }
          }
          
          if (m_pBotSkill->bCanPredict)
          {
               // How higher the skill, how 'more' the bot predicts
               vec org = o;
               float flPredictTime = RandomFloat(1.25f, 1.7f) / (m_sSkillNr+1);
               o = PredictPos(o, d->vel, flPredictTime);
          }
     }
     else
     {
          if (m_pBotSkill->bCanPredict)
          {
               // How higher the skill, how 'more' the bot predicts
               vec org = o;
               float flPredictTime = RandomFloat(0.8f, 1.2f) / (m_sSkillNr+1);
               o = PredictPos(o, d->vel, flPredictTime);
          }
     }
          
     if (flDist > 60.0f)
          flScale = 1.0;
     else if (flDist > 6.0f)
          flScale = flDist / 60.0;
     else
          flScale = 0.1;

     switch (m_sSkillNr)
     {
     case 0:
          // no offset
          offset.x = 0;  
          offset.y = 0;
          offset.z = 0;
          break;
     case 1:
          // GOOD, offset a little for x, y, and z
          offset.x = RandomFloat(-3, 3) * flScale;
          offset.y = RandomFloat(-3, 3) * flScale;
          offset.z = RandomFloat(-6, 6) * flScale;
          break;
     case 2:
          // FAIR, offset somewhat for x, y, and z
          offset.x = RandomFloat(-8, 8) * flScale;
          offset.y = RandomFloat(-8, 8) * flScale;
          offset.z = RandomFloat(-12, 12) * flScale;
          break;
     case 3:
          // POOR, offset for x, y, and z
          offset.x = RandomFloat(-15, 15) * flScale;
          offset.y = RandomFloat(-15, 15) * flScale;
          offset.z = RandomFloat(-25, 25) * flScale;
          break;
     case 4:
          // BAD, offset lots for x, y, and z
          offset.x = RandomFloat(-20, 20) * flScale;
          offset.y = RandomFloat(-20, 20) * flScale;
          offset.z = RandomFloat(-35, 35) * flScale;
          break;
     }
     
     vadd(o, offset);
     return o;
}
     
void CBot::ShootEnemy()
{
     if (!m_pMyEnt->enemy)
          return;
         
     // Aim to enemy
     vec enemypos = GetEnemyPos(m_pMyEnt->enemy);
     AimToVec(enemypos);
     
     // Time to shoot?
     if (m_iShootDelay < lastmillis)
     //if ((lastmillis-m_pMyEnt->lastaction) >= m_pMyEnt->gunwait)
     {
          // Choose a preferred weapon first
          //if (ChoosePreferredWeapon())
          if (m_pMyEnt->ammo[m_pMyEnt->gunselect])
          {
               // If the bot is using the rifle only shoot if crosshair is near the enemy
               if (m_pMyEnt->gunselect == GUN_RIFLE)
               {
                    float yawtoturn = fabs(m_pMyEnt->yaw - m_pMyEnt->targetyaw);
                    float pitchtoturn = fabs(m_pMyEnt->pitch - m_pMyEnt->targetpitch);
               
                    if ((yawtoturn > 5) || (pitchtoturn > 15)) // UNDONE: Should be skill based
                         return;
               }

               float flDist = GetDistance(enemypos);
               
               // Check if bot is in fire range
               if ((flDist < WeaponInfoTable[m_pMyEnt->gunselect].flMinFireDistance) ||
                   (flDist > WeaponInfoTable[m_pMyEnt->gunselect].flMaxFireDistance))
                    return;
                    
               // Now shoot!
               m_pMyEnt->attacking = true;
               
               // Get the position the bot is aiming at
               vec forward, right, up, dest;
               traceresult_s tr;
               
               AnglesToVectors(GetViewAngles(), forward, right, up);
               
               dest = m_pMyEnt->o;
               vmul(forward, 1000);
               vadd(dest, forward);
               
               TraceLine(m_pMyEnt->o, dest, m_pMyEnt, false, &tr);
               debugbeam(m_pMyEnt->o, tr.end);
               
               // Shoot 
               shoot(m_pMyEnt, tr.end);
               
               // Add shoot delay
               m_iShootDelay = lastmillis + GetShootDelay();
               
               ChoosePreferredWeapon();
          }
     }
#ifndef RELEASE_BUILD     
     else
     {
          char sz[64];
          sprintf(sz, "shootdelay: %d\n", (m_iShootDelay-lastmillis));
          AddDebugText(sz);
     }
#endif     
}

bool CBot::ChoosePreferredWeapon()
{
     TMultiChoice<int> WeaponChoices;
     short sWeaponScore;
     float flDist = GetDistance(m_pMyEnt->enemy->o);
     
     if ((m_iChangeWeaponDelay > lastmillis) && (m_pMyEnt->ammo[m_pMyEnt->gunselect]))
     {
          if ((m_pMyEnt->gunselect != GUN_FIST) || (flDist <= 3.5f))
               return true;
     }
            
     // Choose a weapon
     for(int i=0;i<MAX_WEAPONS;i++)
     {
          // If no ammo for this weapon, skip it
          if (m_pMyEnt->ammo[i] == 0) continue;
          
          sWeaponScore = 5; // Minimal score for a weapon
          
          if ((flDist >= WeaponInfoTable[i].flMinDesiredDistance) &&
              (flDist <= WeaponInfoTable[i].flMaxDesiredDistance))
          {
               // In desired range for this weapon
               sWeaponScore += 5; // Increase score much
          }
          else if ((flDist < WeaponInfoTable[i].flMinFireDistance) ||
                   (flDist > WeaponInfoTable[i].flMaxFireDistance))
               continue; // Wrong distance for this weapon
               
          // The ideal distance would be between the Min and Max desired distance.
          // Score on the difference of the avarage of the Min and Max desired distance.
          float flAvarage = (WeaponInfoTable[i].flMinDesiredDistance +
                             WeaponInfoTable[i].flMaxDesiredDistance) / 2.0f;
          float flIdealDiff = fabs(flDist - flAvarage);
          
          if (flIdealDiff < 0.5f) // Close to ideal distance
               sWeaponScore += 4;
          else if (flIdealDiff <= 1.0f)
               sWeaponScore += 2;
               
          // Now rate the weapon on available ammo...
          if (i!=GUN_FIST)
          {
               // Calculate how much percent of the min desired ammo the bot has
               float flDesiredPercent = (float(m_pMyEnt->ammo[i]) /
                                         float(WeaponInfoTable[i].sMinDesiredAmmo)) *
                                         100.0f;
                                         
               if (flDesiredPercent >= 400.0f)
                    sWeaponScore += 4;
               else if (flDesiredPercent >= 200.0f)
                    sWeaponScore += 3;
               else if (flDesiredPercent >= 100.0f)
                    sWeaponScore += 1;
          }
          
          WeaponChoices.Insert(i, sWeaponScore);
     }   
     
     int WeaponSelect;
     if (WeaponChoices.GetSelection(WeaponSelect))
     {
          m_iChangeWeaponDelay = lastmillis + RandomLong(2000, 8000);
          m_bShootAtFeet = ((WeaponSelect==GUN_RL) &&
                            (RandomLong(1, 100) <=
                             m_pBotSkill->sShootAtFeetWithRLPercent));          
          return SelectGun(WeaponSelect);
     }
     
     return SelectGun(GUN_FIST);
}
     
int CBot::GetShootDelay()
{
     // UNDONE
     if ((m_pMyEnt->gunselect == GUN_FIST) || (m_pMyEnt->gunselect == GUN_CG))
          return m_pMyEnt->gunwait;
     
     float flMinShootDelay = m_pBotSkill->flMinAttackDelay;
     float flMaxShootDelay = m_pBotSkill->flMaxAttackDelay;
     return std::max(m_pMyEnt->gunwait, int(RandomFloat(flMinShootDelay, flMaxShootDelay) * 1000.0f));
}

void CBot::HomeThroughMap()
{
     if (m_bGoToDebugGoal) // For debugging the waypoint navigation
     {
          if (!HeadToGoal())
               ResetWaypointVars();
          else
               AddDebugText("Heading to debug goal...");
     }
     else if (m_pMyEnt->enemy) // Combat navigation
     {
          AddDebugText("has enemy");
          if (!CheckJump())
               DoCombatNav();
     }
     else if (HeadToEnt()) // Heading to an interesting entity(ammo, armour etc)
     {
          AddDebugText("has ent");
          if (!m_pCurrentGoalWaypoint && !CheckJump() && CheckStuck())
               m_iCheckEntsDelay = lastmillis + RandomLong(1000, 2000); // Don't check for
                                                                                                                 // ents a while
                                                                                                                 // when stuck
     }
     else // Normal navigation
     {
          bool bDoNormalNav = true;
          
          AddDebugText("normal nav");

          // Make sure the bot looks straight forward and not up or down
          m_pMyEnt->pitch = 0;
          
          // if it is time to look for a waypoint AND if there are waypoints in this
          // level...
          if ((m_iLookForWaypointTime <= lastmillis) &&  (WaypointClass.m_iWaypointCount >= 1))
          {
               // check if we need to find a waypoint...
               if (CurrentWPIsValid() == false)
               {
                    // find the nearest reachable waypoint
                    waypoint_s *pWP = WaypointClass.GetNearestWaypoint(m_pMyEnt, 10.0f);

                    if (pWP)
                    {
                         SetCurrentWaypoint(pWP);
                         condebug("New nav wp");
                         bDoNormalNav = !HeadToWaypoint();
                    }
               }
               else
               {
                    bDoNormalNav = !HeadToWaypoint();
                    AddDebugText("Using wps for nav");
               }
          }
          
          // If nothing special, do regular (waypointless) navigation
          if(bDoNormalNav) // Regular navigation
          {    
               // Is the bot underwater?
               if (UnderWater(m_pMyEnt->o) && WaterNav())
               {
                    // Bot is under water, navigation happens in WaterNav
               }
               // Time to check the environment?
               else if (m_iCheckEnvDelay < lastmillis)
               {
                    if (!vis(m_vWaterGoal, g_vecZero)) m_vWaterGoal = g_vecZero;

                    // Check for stuck and strafe
                    if (UnderWater(m_pMyEnt->o) || !CheckStuck())
                    {
                         // Only do this when the bot is underwater or when the bot isn't stuck
                         
                         // Check field of view (FOV)
                         CheckFOV();
                    }

               }
                         
               // Check if the bot has to strafe
               CheckStrafe();

	       m_pMyEnt->move = 1;
          }         
     }
}

void CBot::DoCombatNav()
{          
     if (m_iCombatNavTime > lastmillis)
     {
          // If bot has a lower skill and has to turn much, wait
          if ((m_sSkillNr > 2) && (m_fYawToTurn > 90.0f))
          {
               m_pMyEnt->move = 0;
          }
          else
          {
               m_pMyEnt->move = m_iMoveDir;
               m_pMyEnt->strafe = m_iStrafeDir;
          }
          return;
     }
     
     if (m_bCombatJump)
     {
          m_pMyEnt->jumpnext = true;
          m_bCombatJump = false;
          return;
     }
          
     float flDist = GetDistance(m_pMyEnt->enemy->o);

     // Check for nearby items?
     if (((m_iCheckEntsDelay < lastmillis) || m_pTargetEnt) &&
         m_pBotSkill->bCanSearchItemsInCombat)
     {
          m_iCheckEntsDelay = lastmillis + 125;
          bool bSearchItems = false;
          
          if (m_pTargetEnt)
          {
               // Bot has already found an entity, still valid?
               vec v = { m_pTargetEnt->x, m_pTargetEnt->y,
                         S(m_pTargetEnt->x, m_pTargetEnt->y)->floor+m_pMyEnt->eyeheight };
               if ((GetDistance(v) > 25.0f) || !IsVisible(m_pTargetEnt))
                    m_pTargetEnt = NULL;
          }
          
          if (!m_pTargetEnt)
          {
               switch(m_pMyEnt->gunselect)
               {
               case GUN_FIST:
                    bSearchItems = (flDist >= 8.0f);
                    break;
               case GUN_CG:
                    bSearchItems = (m_pMyEnt->ammo[GUN_CG] <= 10);
                    break;
               default:
                    bSearchItems = (m_pMyEnt->ammo[m_pMyEnt->gunselect] <= 5);
                    break;
               }
               
               if (bSearchItems)
                    m_pTargetEnt = SearchForEnts(false, 25.0f, 1.0f);
          }
                    
          if (m_pTargetEnt)
          {
               condebug("Combat ent");
               vec v = { m_pTargetEnt->x, m_pTargetEnt->y,
                         S(m_pTargetEnt->x, m_pTargetEnt->y)->floor+m_pMyEnt->eyeheight };
               vec v2, forward, right, up, cross;
               float flDot;

               AnglesToVectors(GetViewAngles(), forward, right, up);

               v2 = v;
               vsub(v2, m_pMyEnt->o);
               v2.z = 0.0f; // Make 2D
               v2 = Normalize(v2);
               forward.z = 0; // Make 2D
          
               flDot = dotprod(v2 , forward);
               cross = CrossProduct(v2, forward);
               
               bool IsLeft = (cross.z > 0.1f);
               bool IsRight = (cross.z < -0.1f);
               bool IsBehind = (flDot < -0.1f);
               bool IsInFront = (flDot > 0.1f);
               
               debugbeam(m_pMyEnt->o, v);

               EDirection eMoveDir = FORWARD;
                                             
               if (IsInFront && !IsLeft && !IsRight) // ent is straight forward
               {
                    eMoveDir = FORWARD;
                    m_pMyEnt->move = m_iMoveDir = 1; // move forward
                    m_pMyEnt->strafe = m_iStrafeDir = 0; // Don't strafe
               }                                                  
               else if (IsBehind && !IsLeft && !IsRight) // Ent is straight behind bot
               {
                    eMoveDir = BACKWARD;
                    m_pMyEnt->move = m_iMoveDir = -1; // Move backward
                    m_pMyEnt->strafe = m_iStrafeDir = 0; // Don't strafe
               }
               else if (!IsBehind && !IsInFront && IsLeft) // Ent is straight left
               {
                    eMoveDir = LEFT;
                    m_pMyEnt->move = m_iMoveDir = 0; // don't move back or forward
                    m_pMyEnt->strafe = m_iStrafeDir = 1; // Strafe left
               }
               else if (!IsBehind && !IsInFront && IsRight) // Ent is straight right
               {
                    eMoveDir = RIGHT;
                    m_pMyEnt->move = m_iMoveDir = 0; // don't move back or forward
                    m_pMyEnt->strafe = m_iStrafeDir = -1; // Strafe right
               }               
               else if (IsInFront && IsLeft) // Ent is forward-left
               {
                    eMoveDir = FORWARD_LEFT;
                    m_pMyEnt->move = m_iMoveDir = 1; // Move forward
                    m_pMyEnt->strafe = m_iStrafeDir = 1; // Strafe left
               }
               else if (IsInFront && IsRight) // Ent is forward-right
               {
                    eMoveDir = FORWARD_RIGHT;
                    m_pMyEnt->move = m_iMoveDir = 1; // Move forward
                    m_pMyEnt->strafe = m_iStrafeDir = -1; // Strafe right
               }               
               else if (IsBehind && IsLeft) // ent is backward-left
               {
                    eMoveDir = BACKWARD_LEFT;
                    m_pMyEnt->move = m_iMoveDir = -1; // Move backward
                    m_pMyEnt->strafe = m_iStrafeDir = 1; // Strafe left
               }
               else if (IsBehind && IsRight) // ent is backward-right
               {
                    eMoveDir = BACKWARD_RIGHT;
                    m_pMyEnt->move = m_iMoveDir = -1; // Move backward
                    m_pMyEnt->strafe = m_iStrafeDir = -1; // Strafe right
               }
               
               m_iCombatNavTime = lastmillis + RandomLong(125, 250);
               
               // Check if bot needs to jump
               vec from = m_pMyEnt->o;
               from.z -= 1.0f;
               if (!IsVisible(from, eMoveDir, 3.0f, false))
                    m_pMyEnt->jumpnext = true;
               return;
          }
     }
               
     // High skill and enemy is close?
     if ((m_sSkillNr <= 1) && (m_fYawToTurn < 80.0f) && (flDist <= 20.0f))
     {
          // Randomly jump a bit, to avoid some basic firepower ;)
          
          // How lower the distance to the enemy, how higher the chance for a jump
          short sJumpPercent = (100 - ((short)flDist * 4));
          if (RandomLong(1, 100) <= sJumpPercent) // Don't do this more then 20%
          {
               // Choose a nice direction to jump to
               
               // Is the enemy close?
               if ((flDist <= 4.0f) || (m_pMyEnt->gunselect == GUN_FIST))
               {
                    m_pMyEnt->move = m_iMoveDir = 1; // Jump forward
                    m_pMyEnt->strafe = m_iStrafeDir = 0; // Don't strafe
                    m_bCombatJump = true;
               }
               else // else jump to a random direction
               {
                    /* 
                         Directions to choose:
                         - Forward-right
                         - Right
                         - Backward-right
                         - Backward
                         - Backward-left
                         - Left
                         - Forward-left
                         
                    */
                    
                    TMultiChoice<EDirection> JumpDirChoices;
                    short sForwardScore = ((flDist > 8.0f) || (flDist < 4.0f)) ? 20 : 10;
                    short sBackwardScore = (flDist <= 6.0f) ? 20 : 10;
                    short sStrafeScore = (flDist < 6.0f) ? 20 : 10;
                    
                    if (IsVisible(FORWARD_LEFT, 4.0f, false))
                         JumpDirChoices.Insert(FORWARD_LEFT, sForwardScore);
                    if (IsVisible(FORWARD_RIGHT, 4.0f, false))
                         JumpDirChoices.Insert(FORWARD_RIGHT, sForwardScore);
                    if (IsVisible(BACKWARD, 4.0f, false))
                         JumpDirChoices.Insert(BACKWARD, sBackwardScore);
                    if (IsVisible(BACKWARD_LEFT, 4.0f, false))
                         JumpDirChoices.Insert(BACKWARD_LEFT, sBackwardScore);
                    if (IsVisible(BACKWARD_RIGHT, 4.0f, false))
                         JumpDirChoices.Insert(BACKWARD_RIGHT, sBackwardScore);
                    if (IsVisible(LEFT, 4.0f, false))
                         JumpDirChoices.Insert(LEFT, sStrafeScore);
                    if (IsVisible(RIGHT, 4.0f, false))
                         JumpDirChoices.Insert(RIGHT, sStrafeScore);
                    
                    EDirection JumpDir;
                    if (JumpDirChoices.GetSelection(JumpDir))
                    {
                         switch(JumpDir)
                         {
                         case FORWARD_LEFT:
                              m_pMyEnt->move = m_iMoveDir = 1;
                              m_pMyEnt->strafe = m_iStrafeDir = 1;
                              m_bCombatJump = true;
                              break;
                         case FORWARD_RIGHT:
                              m_pMyEnt->move = m_iMoveDir = 1;
                              m_pMyEnt->strafe = m_iStrafeDir = -1;
                              m_bCombatJump = true;
                              break;
                         case BACKWARD:
                              m_pMyEnt->move = m_iMoveDir = -1;
                              m_pMyEnt->strafe = m_iStrafeDir = 0;
                              m_bCombatJump = true;
                              break;
                         case BACKWARD_LEFT:
                              m_pMyEnt->move = m_iMoveDir = -1;
                              m_pMyEnt->strafe = m_iStrafeDir = 1;
                              m_bCombatJump = true;
                              break;
                         case BACKWARD_RIGHT:
                              m_pMyEnt->move = m_iMoveDir = 1;
                              m_pMyEnt->strafe = m_iStrafeDir = -1;
                              m_bCombatJump = true;
                              break;
                         case LEFT:
                              m_pMyEnt->move = m_iMoveDir = 0;
                              m_pMyEnt->strafe = m_iStrafeDir = 1;
                              m_bCombatJump = true;
                              break;
                         case RIGHT:
                              m_pMyEnt->move = m_iMoveDir = 0;
                              m_pMyEnt->strafe = m_iStrafeDir = -1;
                              m_bCombatJump = true;
                              break;
                         }                              
                    }
               }
               
               if (m_bCombatJump)
               {
                    m_iCombatNavTime = lastmillis + RandomLong(125, 250);
                    return;
               }
          }
     }
     
     if (m_pMyEnt->gunselect == GUN_FIST)
          return; // Simply walk towards enemy if 'using' fist
     
     // Out of desired range for current weapon?
     if ((flDist <= WeaponInfoTable[m_pMyEnt->gunselect].flMinDesiredDistance) ||
         (flDist >= WeaponInfoTable[m_pMyEnt->gunselect].flMaxDesiredDistance))
     {
          if (flDist > 8)
               m_pMyEnt->move = m_iMoveDir = 1; // Run forward when not close
          else
               m_pMyEnt->move = m_iMoveDir = -1; // Run backwards when close to enemy
     
          vec src, forward, right, up, dest, MyAngles = GetViewAngles(), o = m_pMyEnt->o;
          traceresult_s tr;
     
          // Is it furthest or farthest? bleh
          float flFurthestDist = 0;
          int bestdir = -1, dir = 0;
          bool moveback = (m_pMyEnt->move == -1);
     
          for(int j=-45;j<=45;j+=45)
          {
               src = MyAngles;
               src.y = WrapYZAngle(src.y + j);
               src.x = 0.0f;
          
               // If we're moving backwards, trace backwards
               if (moveback)
                    src.y = WrapYZAngle(src.y + 180);
          
               AnglesToVectors(src, forward, right, up);
          
               dest = o;
               vmul(forward, 40);
               vadd(dest, forward);
          
               TraceLine(o, dest, m_pMyEnt, false, &tr);
          
               //debugbeam(origin, end);
               flDist = GetDistance(tr.end);
          
               if (flFurthestDist < flDist)
               {
                    flFurthestDist = flDist;
                    bestdir = dir;
               }
               dir++;
          }
     
          switch(bestdir)
          {
          case 0:
               if (moveback)
                    m_pMyEnt->strafe = m_iStrafeDir = -1; // Strafe right
               else
                    m_pMyEnt->strafe = m_iStrafeDir = 1; // Strafe left
          
               m_iCombatNavTime = lastmillis + 500;
               break;
          case 1:
               m_pMyEnt->strafe = m_iStrafeDir = 0; // Don't strafe
               m_iCombatNavTime = lastmillis + 500;
               break;
          case 2:
               if (moveback)
                    m_pMyEnt->strafe = m_iStrafeDir = 1; // Strafe left
               else
                    m_pMyEnt->strafe = m_iStrafeDir = -1; // Strafe right
               
               m_iCombatNavTime = lastmillis + 500;
               break;
          }
     }
     else if (m_pBotSkill->bCircleStrafe) // Circle strafe when in desired range...
     {
          traceresult_s tr;
          vec angles, end, forward, right, up;
          TMultiChoice<EDirection> StrafeDirChoices;
          
          // Check the left side...
          angles = GetViewAngles();
          angles.y = WrapYZAngle(angles.y - 75.0f); // Not 90 degrees because the bot
                                                    // doesn't strafe in a straight line
                                                    // (aims still to enemy).
          
          AnglesToVectors(angles, forward, right, up);
          end = m_pMyEnt->o;
          vmul(forward, 15.0f);
          vadd(end, forward);
          
          TraceLine(m_pMyEnt->o, end, m_pMyEnt, true, &tr);
          StrafeDirChoices.Insert(LEFT, (int)GetDistance(m_pMyEnt->o, tr.end));
          
          // Check the right side...
          angles = GetViewAngles();
          angles.y = WrapYZAngle(angles.y + 75.0f); // Not 90 degrees because the bot
                                                    // doesn't strafe in a straight line
                                                    // (aims still to enemy).
          
          AnglesToVectors(angles, forward, right, up);
          end = m_pMyEnt->o;
          vmul(forward, 15.0f);
          vadd(end, forward);
          
          TraceLine(m_pMyEnt->o, end, m_pMyEnt, true, &tr);
          StrafeDirChoices.Insert(RIGHT, (int)GetDistance(m_pMyEnt->o, tr.end));
     
          EDirection StrafeDir;
          if (StrafeDirChoices.GetSelection(StrafeDir))
          {
               if (StrafeDir == LEFT)
               {
                    m_pMyEnt->strafe = m_iStrafeDir = 1;
                    m_pMyEnt->move = m_iMoveDir = 0;
                    m_iCombatNavTime = lastmillis + RandomLong(1500, 3000);
               }
               else
               {
                    m_pMyEnt->strafe = m_iStrafeDir = -1;
                    m_pMyEnt->move = m_iMoveDir = 0;
                    m_iCombatNavTime = lastmillis + RandomLong(1500, 3000);
               }
          }
     }
     else // Bot can't circle strafe(low skill), just stand still
          ResetMoveSpeed();
}

bool CBot::CheckStuck()
{
     if (m_iStuckCheckDelay >= lastmillis)
          return false;

     if ((!vis(m_vGoal, g_vecZero)) && (GetDistance(m_vGoal) < 2.0f))
          return false;
                    
     bool IsStuck = false;
     
     vec CurPos = m_pMyEnt->o, PrevPos = m_vPrevOrigin;
     CurPos.z = PrevPos.z = 0;


     // Did the bot hardly move the last frame?
     float debug_stuck = GetDistance(CurPos, PrevPos);
     if (debug_stuck <= 0.1f)
     {
          if (m_bStuck)
          {
               if (m_iStuckTime < lastmillis)
                    IsStuck = true;
          }
          else
          {
               m_bStuck = true;
               m_iStuckTime = lastmillis + 1000;
          }
     }
     else
     {
          m_bStuck = false;
          m_iStuckTime = 0;
     }
     
     //printf("Bot %d debug_stuck %1.3f\n", BotManager.GetBotIndex(m_pMyEnt), debug_stuck);

     if (IsStuck)          
     {
#ifndef RELEASE_BUILD     
          char msg[64];
          sprintf(msg, "stuck (%f)", GetDistance(m_vPrevOrigin));
          condebug(msg);
#endif
	  //printf("Bot %d IsStuck\n", BotManager.GetBotIndex(m_pMyEnt));
          
          m_bStuck = false;
          m_iStuckTime = 0;
          
          // Crap bot is stuck, lets just try some random things
                        
          // Check if the bot can turn around
          vec src = GetViewAngles();
          src.x = 0;
          vec forward, left, right, up, dest;
          traceresult_s tr;
          
          AnglesToVectors(src, forward, right, up);
          
          // Check if bot can turn 180 degrees
          dest = m_pMyEnt->o;
          vmul(forward, 3);
          vsub(dest, right);
          
          TraceLine(m_pMyEnt->o, dest, m_pMyEnt, true, &tr);
          debugbeam(m_pMyEnt->o, tr.end);
          
          if (!tr.collided)
          {
               // Bot can turn around, do so
	       //printf("Bot %d turning around, yaw %3.3f\n", BotManager.GetBotIndex(m_pMyEnt), m_pMyEnt->targetyaw);
               m_pMyEnt->targetyaw = WrapYZAngle(m_pMyEnt->yaw + 180);
               m_iStuckCheckDelay = m_iCheckEnvDelay = lastmillis + 500;
	       //printf("Bot %d turning around, yaw %3.3f %3.9f\n", BotManager.GetBotIndex(m_pMyEnt), m_pMyEnt->targetyaw, m_pMyEnt->yaw);
               return true;
          }

          // Check the left side...
          left.x = -right.x; left.y = -right.y; left.z = -right.z;
          dest = m_pMyEnt->o;
          vmul(left, 3);
          vadd(dest, left);
          
          TraceLine(m_pMyEnt->o, dest, m_pMyEnt, true, &tr);
          debugbeam(m_pMyEnt->o, tr.end);
          
          if (!tr.collided)
          {
               // Bot can turn left, do so
	       //printf("Bot %d turning left, yaw %3.3f\n", BotManager.GetBotIndex(m_pMyEnt), m_pMyEnt->targetyaw);
               m_pMyEnt->targetyaw = WrapYZAngle(m_pMyEnt->yaw - 90);
               m_iStuckCheckDelay = m_iCheckEnvDelay = lastmillis + 500;
	       //printf("Bot %d turning left, yaw %3.3f %3.9f\n", BotManager.GetBotIndex(m_pMyEnt), m_pMyEnt->targetyaw, m_pMyEnt->yaw);
               return true;
          }
          
          // Check the right side...     
          dest = m_pMyEnt->o;
          vmul(right, 3);
          vadd(dest, right);
          
          TraceLine(m_pMyEnt->o, dest, m_pMyEnt, true, &tr);
          debugbeam(m_pMyEnt->o, tr.end);
          
          if (!tr.collided)
          {
               // Bot can turn right, do so
	       //printf("Bot %d turning right, yaw %3.3f\n", BotManager.GetBotIndex(m_pMyEnt), m_pMyEnt->targetyaw);
               m_pMyEnt->targetyaw = WrapYZAngle(m_pMyEnt->yaw + 90);
               m_iStuckCheckDelay = m_iCheckEnvDelay = lastmillis + 500;
	       //printf("Bot %d turning right, yaw %3.3f %3.9f\n", BotManager.GetBotIndex(m_pMyEnt), m_pMyEnt->targetyaw, m_pMyEnt->yaw);
               return true;
          }
                    
          // Bleh bot couldn't turn, lets just randomly jump :|
          
          condebug("Randomly jumping...");
	  //conoutf("Randomly jumping...");
          m_pMyEnt->jumpnext = true;
          return true;
     }
     
     return false;
}

// Check if a near wall is blocking and we can jump over it
bool CBot::CheckJump()
{
     bool bHasGoal = (!vis(m_vGoal, g_vecZero));
     float flGoalDist = (bHasGoal) ? GetDistance(m_vGoal) : 0.0f;
     
     if ((bHasGoal) && (flGoalDist < 2.0f))
          return false;
          
     vec src, MyAngles = GetViewAngles(), forward, up, right, dest, start = m_pMyEnt->o;
     traceresult_s tr;
     float flTraceDist = 3.0f;
     
     if (bHasGoal && (flGoalDist < flTraceDist))
          flTraceDist = flGoalDist;

     // Trace a line to see if something blocks our way on eyeheight
     src = MyAngles;
     src.x = 0;     
     AnglesToVectors(src, forward, right, up);
     dest = start;
     vmul(forward, flTraceDist);
     vadd(dest, forward);
     TraceLine(start, dest, m_pMyEnt, false, &tr);
     
     // Something blocks at eye hight?
     if (tr.collided)
     {
          // Check if the bot can jump over it
          src = MyAngles;
          src.x = 0;
     
          AnglesToVectors(src, forward, right, up);
     
          start.z += (JUMP_HEIGHT - 1.0f);
          dest = start;
          vmul(forward, 6);
          vadd(dest, forward);
          
          TraceLine(start, dest, m_pMyEnt, false, &tr);
          //debugbeam(start, end);
     
          if (!tr.collided)
          {
               // Jump
               debugnav("High wall");
               m_pMyEnt->jumpnext = true;
               return true;
          }
     }
     else
     {
          // Check if something is blocking at feet height, so the bot can jump over it
          src = MyAngles;
          src.x = 0;
     
          AnglesToVectors(src, forward, right, up);
     
          start.z -= 1.7f;
          dest = start;
          vmul(forward, flTraceDist);
          vadd(dest, forward);
          
          TraceLine(start, dest, m_pMyEnt, false, &tr);
     
          // Trace was blocked?
          if (tr.collided)
          {
               //debugbeam(start, end);
          
               // Jump
               debugnav("Low wall");
               m_pMyEnt->jumpnext = true;
               return true;
          }
     }
     
     return false; // Bot didn't had to jump(or couldn't)
}
     
bool CBot::CheckStrafe()
{
     if (m_iStrafeTime >= lastmillis)
     {
          m_pMyEnt->strafe = m_iStrafeDir;
          return true;
     }
     
     if (m_iStrafeCheckDelay >= lastmillis)
          return false;
          
     // Check for near walls
     traceresult_s tr;
     vec from = m_pMyEnt->o, to, forward, right, up, dir;
     float flLeftDist = -1.0f, flRightDist = -1.0f;
     bool bStrafe = false;
     short sStrafeDir = 0;
     
     AnglesToVectors(GetViewAngles(), forward, right, up);
     
     // Check for a near left wall
     to = from;
     dir = right;
     vmul(dir, 3.0f);
     vsub(to, dir);
     TraceLine(from, to, m_pMyEnt, false, &tr);
     if (tr.collided)
          flLeftDist = GetDistance(from, tr.end);
     //debugbeam(m_pMyEnt->o, to);
          
     // Check for a near right wall
     to = from;
     dir = right;
     vmul(dir, 3.0f);
     vadd(to, dir);
     TraceLine(from, to, m_pMyEnt, false, &tr);
     if (tr.collided)
          flRightDist = GetDistance(from, tr.end);
     //debugbeam(m_pMyEnt->o, to);
     
     if ((flLeftDist == -1.0f) && (flRightDist == -1.0f))
     {
          from = m_pMyEnt->o;
          vmul(dir, m_pMyEnt->radius);
          vsub(from, dir);
       
          to = from;
          dir = forward;
          vmul(dir, 3.0f);
          vadd(to, dir);
          TraceLine(from, to, m_pMyEnt, false, &tr);
          if (tr.collided)
               flLeftDist = GetDistance(from, tr.end);
          //debugbeam(m_pMyEnt->o, to);
          
          from = m_pMyEnt->o;
          vmul(dir, m_pMyEnt->radius);
          vadd(from, dir);
       
          to = from;
          dir = forward;
          vmul(dir, 3.0f);
          vadd(to, dir);
          TraceLine(from, to, m_pMyEnt, false, &tr);
          if (tr.collided)
               flRightDist = GetDistance(from, tr.end);
          //debugbeam(m_pMyEnt->o, to);
     }
          
     if ((flLeftDist != -1.0f) && (flRightDist != -1.0f))
     {
          if (flLeftDist < flRightDist)
          {
               // Strafe right
               bStrafe = true;
               sStrafeDir = -1;
          }
          else if (flRightDist < flLeftDist)
          {
               // Strafe left
               bStrafe = true;
               sStrafeDir = 1;
          }
          else
          {
               // Randomly choose a strafe direction
               bStrafe = true;
               if (RandomLong(0, 1))
                    sStrafeDir = -1;
               else
                    sStrafeDir = 1;
          }
     }
     else if (flLeftDist != -1.0f)
     {
          // Strafe right
          bStrafe = true;
          sStrafeDir = -1;
     }
     else if (flRightDist != -1.0f)
     {
          // Strafe left
          bStrafe = true;
          sStrafeDir = 1;
     }
     
     if (bStrafe)
     {
          m_pMyEnt->strafe = m_iStrafeDir = sStrafeDir;
          m_iStrafeTime = lastmillis + RandomLong(75, 150);
     }

     return bStrafe;
}

void CBot::CheckFOV()
{
     m_iCheckEnvDelay = lastmillis + RandomLong(125, 250);
     vec MyAngles = GetViewAngles();
     vec src, forward, right, up, dest, best = {0,0,0};
     vec origin = m_pMyEnt->o;
     float flDist, flFurthestDist = 0;
     bool WallLeft = false;
     traceresult_s tr;
     
     //origin.z -= 1.5; // Slightly under eye level
             
     // Scan 90 degrees FOV
     for(int angle=-45;angle<=45;angle+=5)
     {
          src = MyAngles;
          src.y = WrapYZAngle(src.y + angle);
          
          AnglesToVectors(src, forward, right, up);
          
          dest = origin;
          vmul(forward, 40);
          vadd(dest, forward);
          
          TraceLine(origin, dest, m_pMyEnt, false, &tr);
          
          //debugbeam(origin, end);
          flDist = GetDistance(tr.end);
          
          if (flFurthestDist < flDist)
          {
               flFurthestDist = flDist;
               best = tr.end;
          }
     }
     
     if (best.x && best.y && best.z)
     {
          AimToVec(best);
          // Update MyAngles, since their (going to be) change(d)
          MyAngles.x = m_pMyEnt->targetpitch;
          MyAngles.y = m_pMyEnt->targetyaw;
     }
     
     float flNearestHitDist = GetDistance(best);
     
     if (!UnderWater(m_pMyEnt->o) && m_pMyEnt->onfloor)
     {
          // Check if a near wall is blocking and we can jump over it
          if (flNearestHitDist < 4)
          {
               // Check if the bot can jump over it
               src = MyAngles;
               src.x = 0;
     
               AnglesToVectors(src, forward, right, up);
     
               vec start = origin;
               start.z += 2.0f;
               dest = start;
               vmul(forward, 6);
               vadd(dest, forward);
          
               TraceLine(start, dest, m_pMyEnt, false, &tr);
               //debugbeam(start, end);
     
               if (!tr.collided)
               {
                    // Jump
                    debugnav("High wall");
                    m_pMyEnt->jumpnext = true;
                    m_iStrafeCheckDelay = lastmillis + RandomLong(250, 500);
                    return;
               }
          }
          else
          {
               // Check if something is blocking below us, so the bot can jump over it
               src = MyAngles;
               src.x = 0;
     
               AnglesToVectors(src, forward, right, up);
     
               vec start = origin;
               start.z -= 1.7f;
               dest = start;
               vmul(forward, 4);
               vadd(dest, forward);
          
               TraceLine(start, dest, m_pMyEnt, false, &tr);
     
               // Trace was blocked?
               if (tr.collided)
               {
                    //debugbeam(start, end);
          
                    // Jump
                    debugnav("Low wall");
                    m_pMyEnt->jumpnext = true;
                    m_iStrafeCheckDelay = lastmillis + RandomLong(250, 500);
                    return;
               }
          }
          
          // Check if the bot is going to fall...
          src = MyAngles;
          src.x = 0.0f;
          AnglesToVectors(src, forward, right, up);
     
          dest = origin;
          vmul(forward, 3.0f);
          vadd(dest, forward);
     
          TraceLine(origin, dest, m_pMyEnt, false, &tr);
     
          int cx = fast_f2nat(tr.end.x), cy = fast_f2nat(tr.end.y);
          short CubesInWater=0;
          for(int x=cx-1;x<=(cx+1);x++)
          {
               for(int y=cy-1;y<=(cy+1);y++)
               {
                    if (OUTBORD(x, y)) continue;
                    //sqr *s = S(fast_f2nat(x), fast_f2nat(y));
                    //if (!SOLID(s))
                    {
                         vec from = { x, y, m_pMyEnt->o.z };
                         dest = from;
                         dest.z -= 6.0f;
                         TraceLine(from, dest, m_pMyEnt, false, &tr);
                         //vec v = { x, y, sheight(s, s, 1.0f) };
                         bool turn = false;
                         if (UnderWater(tr.end)) CubesInWater++;
                         if (CubesInWater > 2) turn = true; // Always avoid water
                         if (!tr.collided && RandomLong(0, 1))
                              turn = true; // Randomly avoid a fall
          
                         if (turn)
                         {
                              m_pMyEnt->targetyaw = WrapYZAngle(m_pMyEnt->yaw + 180);
                              m_iCheckEnvDelay = m_iStrafeCheckDelay = lastmillis + RandomLong(750, 1500);
                              debugnav("Water or a fall in front");
                              return;
                         }
                    }
               }
          }
     }
     
     // Is the bot about to head a corner?
     if (flNearestHitDist <= 4.0f)
     {
          src = MyAngles;
          src.y = WrapYZAngle(src.y - 45.0f);
          AnglesToVectors(src, forward, right, up);
     
          dest = origin;
          vmul(forward, 4.0f);
          vadd(dest, forward);
          
          TraceLine(origin, dest, m_pMyEnt, false, &tr);
     
          WallLeft = (tr.collided);
     
          src = MyAngles;
          src.y += WrapYZAngle(src.y + 45.0f);
          AnglesToVectors(src, forward, right, up);
     
          dest = origin;
          vmul(forward, 4.0f);
          vadd(dest, forward);
          
          TraceLine(origin, dest, m_pMyEnt, false, &tr);
     
          if (WallLeft && tr.collided)
          {
               // We're about to hit a corner, turn away
               debugnav("Corner");
               m_pMyEnt->targetyaw = WrapYZAngle(m_pMyEnt->yaw + RandomFloat(160.0f, 200.0f));
               m_iCheckEnvDelay = m_iStrafeCheckDelay = lastmillis + RandomLong(750, 1500);
               return;
          }
     }
}

// Called when bot is underwater
bool CBot::WaterNav()
{
     const int iSearchRange = 4;
     
     if (vis(m_vWaterGoal, g_vecZero))
     {
          AddDebugText("WaterNav");
          // Find the nearest and reachable cube which isn't underwater
          
          int cx = fast_f2nat(m_pMyEnt->o.x);
          int cy = fast_f2nat(m_pMyEnt->o.y);
          float flNearestDist = 9999.0f, flDist;
          
          if (OUTBORD(cx, cy)) return false;
                   
          // Check all cubes in range...
          for (int x=(cx-iSearchRange);x<=(cx+iSearchRange);x++)
          {
               for (int y=(cy-iSearchRange);y<=(cy+iSearchRange);y++)
               {
                    sqr *s = S(x, y);
          
                    if (SOLID(s)) continue;
                    if ((x==cx) && (y==cy)) continue;
               
                    vec v = { x, y, sheight(s, s, 1.0f) };
                    
                    if (UnderWater(v)) continue; // Skip, cube is underwater
                    
                    if (hdr.waterlevel < (v.z - 2.0f)) continue; // Cube is too high
                    
                    // Check if the bot 'can fit' on the cube(no near obstacles)
                    bool small = false;
                    for (int a=(x-2);a<=(x+2);a++)
                    {
                         if (small) break;
                         for (int b=(y-2);b<=(y+2);b++)
                         {
                              if ((x==a) && (y==b)) continue;
                              sqr *s2 = S(fast_f2nat(a), fast_f2nat(b));
                              vec v2 = { a, b, sheight(s2, s2, 1.0f) };
                              if (v.z < (v2.z-1-JUMP_HEIGHT))
                              {
                                   small=true;
                                   break;
                              }
                         
                              if ((a >= (x-1)) && (a <= (x+1)) && (b >= (y-1)) && (b <= (y+1)))
                              {
                                   if ((v2.z) < (v.z-2.0f))
                                   {
                                        small = true;
                                        break;
                                   }
                              }
                         
                              traceresult_s tr;
                              TraceLine(v, v2, NULL, false, &tr);
                              if (tr.collided)
                              {
                                   small=true;
                                   break;
                              }
                         }
                         if (small) break;
                    }
                    if (small)
                    {
                         debugbeam(m_pMyEnt->o, v);
                         continue;
                    }
                    
                    // Okay, cube is valid.
                    flDist = GetDistance(v);
                    if (flDist < flNearestDist)
                    {
                         flNearestDist = flDist;
                         m_vWaterGoal = v;
                    }
               }
          }
     }
     
     if (!vis(m_vWaterGoal, g_vecZero))
     {
          AddDebugText("WaterNav");
          //debugbeam(m_pMyEnt->o, m_vWaterGoal);
          vec aim = m_vWaterGoal;
          aim.z += 1.5f; // Aim a bit further up
          AimToVec(aim);
          if ((RandomLong(1, 100) <= 15) && (Get2DDistance(m_vWaterGoal) <= 7.0f))
               m_pMyEnt->jumpnext = true;
          return true;
     }
          
     return false;
}

entity *CBot::SearchForEnts(bool bUseWPs, float flRange, float flMaxHeight)
{
     float flNearestDist = 9999, flDist;
     entity *pNewTargetEnt = NULL;
     bool CheckTeleporters = ((m_iCheckTeleporterDelay < lastmillis) &&
                              (WaypointClass.m_iWaypointCount < 1) &&
                              (m_pMyEnt->enemy == NULL));
     bool CheckJumppads = ((m_iCheckJumppadsDelay < lastmillis) &&
                           (WaypointClass.m_iWaypointCount < 1) &&
                           (m_pMyEnt->enemy == NULL));
     waypoint_s *pWptNearBot = NULL, *pBestWpt = NULL;
     
     if ((WaypointClass.m_iWaypointCount >= 1) && bUseWPs)
          pWptNearBot = WaypointClass.GetNearestWaypoint(m_pMyEnt, 15.0f);

#ifdef WP_FLOOD                    
     if (!pWptNearBot && bUseWPs)
          pWptNearBot = WaypointClass.GetNearestFloodWP(m_pMyEnt, 5.0f);
#endif
          
     loopv(ents)
     {
          entity &e = ents[i];
          vec o = { e.x, e.y, S(e.x, e.y)->floor+player1->eyeheight };
     
          if ((e.type == TELEPORT) && !CheckTeleporters)
               continue;

          if ((e.type == JUMPPAD) && !CheckJumppads)
               continue;
          
          if ((!ents[i].spawned) && (e.type != TELEPORT) && (e.type != JUMPPAD)) continue;
          if (OUTBORD(e.x, e.y)) continue;
          
          bool bInteresting = false;
          switch(e.type)
          {
          case TELEPORT:
          case JUMPPAD:
               bInteresting = true;
               break;
          case I_SHELLS:
               bInteresting = (m_pMyEnt->ammo[1]<itemstats[e.type-I_SHELLS].max);
               break;
          case I_BULLETS:
               bInteresting = (m_pMyEnt->ammo[2]<itemstats[e.type-I_SHELLS].max);
               break;
          case I_ROCKETS:
               bInteresting = (m_pMyEnt->ammo[3]<itemstats[e.type-I_SHELLS].max);
               break;
          case I_ROUNDS:
               bInteresting = (m_pMyEnt->ammo[4]<itemstats[e.type-I_SHELLS].max);
               break;              
          case I_HEALTH:
          case I_BOOST:
               bInteresting = (m_pMyEnt->health<itemstats[e.type-I_SHELLS].max);
               break;
          case I_GREENARMOUR:
               bInteresting = ((m_pMyEnt->armour<itemstats[e.type-I_SHELLS].max) &&
                               (m_pMyEnt->armourtype!=A_YELLOW || m_pMyEnt->armour<=66));
               break;
          case I_YELLOWARMOUR:
               bInteresting = (m_pMyEnt->armour<itemstats[e.type-I_SHELLS].max);
               break;
          case I_QUAD:
               bInteresting = (m_pMyEnt->quadmillis<itemstats[e.type-I_SHELLS].max);
               break;
          }
              
          if (!bInteresting)
              continue; // Not an interesting item, skip
              
          flDist = GetDistance(o);
          
          if ((flDist >= flNearestDist) || (flDist > flRange)) continue;
          
          // If this entity isn't visible check if there is a nearby waypoint
          if (!IsReachable(o, flMaxHeight))//(!IsVisible(o))
          {
               if ((e.type == TELEPORT) || (e.type == JUMPPAD)) continue;
               if (!pWptNearBot) continue;
               
               waypoint_s *pWptNearEnt;
#ifdef WP_FLOOD               
               if (pWptNearBot->iFlags & W_FL_FLOOD)
                    pWptNearEnt = WaypointClass.GetNearestFloodWP(o, 5.0f);
               else
#endif               
                    pWptNearEnt = WaypointClass.GetNearestWaypoint(o, 15.0f);
                    
               if (!pWptNearEnt) continue;
                              
               // Found a valid wp near the bot and the ent,so...lets store it :)
               pBestWpt = pWptNearEnt;
          }
          else
               pBestWpt = NULL; // Reset when a better ent found which doesn't need
                                // waypoints to reach it
                                    
          flNearestDist = flDist;
          pNewTargetEnt = &ents[i];
     }
     
     if (pNewTargetEnt)
     {
          if (pNewTargetEnt->type == JUMPPAD)
               m_iCheckJumppadsDelay = lastmillis + 3500;
          else if (pNewTargetEnt->type == TELEPORT)
               m_iCheckTeleporterDelay = lastmillis + 5000;
          
          // Need waypoints to reach it?
          if (pBestWpt)
          {
               ResetWaypointVars();
               SetCurrentWaypoint(pWptNearBot);
               SetCurrentGoalWaypoint(pBestWpt);
          }
               
          m_vGoal.x = pNewTargetEnt->x;
          m_vGoal.y = pNewTargetEnt->y;
          m_vGoal.z = S(pNewTargetEnt->x, pNewTargetEnt->y)->floor+player1->eyeheight;
     }
          
     return pNewTargetEnt;
}
          
bool CBot::HeadToEnt()
{
     if (m_iCheckEntsDelay > lastmillis)
     {
          m_vGoal = g_vecZero;
          return false;
     }
     
     if (!m_pTargetEnt || vis(m_vGoal, g_vecZero))
          m_pTargetEnt = SearchForEnts(true);
          
     if (m_pTargetEnt)
     {
          vec o = { m_pTargetEnt->x, m_pTargetEnt->y,
                    S(m_pTargetEnt->x, m_pTargetEnt->y)->floor+player1->eyeheight };
          
          if ((m_pTargetEnt->spawned || (m_pTargetEnt->type == TELEPORT) ||
              (m_pTargetEnt->type == JUMPPAD)) && (!UnderWater(m_pMyEnt->o) || !UnderWater(o)))
          {
               bool bIsVisible = false;
               if (m_pCurrentGoalWaypoint)
               {
                    if ((GetDistance(o) <= 20.0f) && IsReachable(o, 1.0f))
                         bIsVisible = true;
                    else if (HeadToGoal())
                    {
                         debugbeam(m_pMyEnt->o, m_pCurrentWaypoint->v_origin);
                         debugbeam(m_pMyEnt->o, m_pCurrentGoalWaypoint->v_origin);
                         AddDebugText("Using WPs for ents");
                         return true;
                    }
               }
               else
                    bIsVisible = IsVisible(o);
                                   
               if (bIsVisible)
               {
                    if (m_pCurrentWaypoint || m_pCurrentGoalWaypoint)
                    {
                         condebug("ent is now visible");
                         ResetWaypointVars();
                    }
                    
                    float flHeightDiff = o.z - m_pMyEnt->o.z;
                    bool bToHigh = false;
                    if (Get2DDistance(o) <= 2.0f)
                    {
                         if (flHeightDiff >= 1.5f)
                         {
                              if (flHeightDiff <= JUMP_HEIGHT)
                              {
#ifndef RELEASE_BUILD                    
                                   char sz[64];
                                   sprintf(sz, "Ent z diff: %f", o.z-m_pMyEnt->o.z);
                                   condebug(sz);
#endif                         
                                   m_pMyEnt->jumpnext = true; // Jump if close to ent and the ent is high
                              }
                              else
                                   bToHigh = true;
                         }
                    }
                    
                    if (!bToHigh)
                    {
                         AimToVec(o);
                         return true;                              
                    }                                        
               }
          }
     }
     else
     {
          m_iCheckEntsDelay = lastmillis + RandomLong(2500, 7500); // Don't think about ents for a while
          return false;
     }
     
     ResetWaypointVars();
     m_vGoal = g_vecZero;
     m_pTargetEnt = NULL;
     m_iCheckEntsDelay = lastmillis + RandomLong(5000, 10000); // Don't think about ents for a while
     return false;
}

bool CBot::IsReachable(vec to, float flMaxHeight)
{
     vec from = m_pMyEnt->o;
     traceresult_s tr;
     float curr_height, last_height;

     float distance = GetDistance(from, to);

     // is the destination close enough?
     //if (distance < REACHABLE_RANGE)
     {
          if (IsVisible(to))
          {
               // Look if bot can 'fit trough'
               vec src = from, forward, right, up;
               AnglesToVectors(GetViewAngles(), forward, right, up);
               
               // Trace from 1 cube to the left
               vec temp = { -right.x, -right.y, -right.z };
               vmul(temp, 1.0f);
               vadd(src, temp);
               if (!::IsVisible(src, to, m_pMyEnt)) return false;

               // Trace from 1 cube to the right
               temp = right;
               vmul(temp, 1.0f);
               vadd(src, temp);
               if (!::IsVisible(src, to, m_pMyEnt)) return false;
                              
               if (UnderWater(from) && UnderWater(to))
               {
                    // No need to worry about heights in water
                    return true;
               }
/*
               if (to.z > (from.z + JUMP_HEIGHT))
               {
                    vec v_new_src = to;
                    vec v_new_dest = to;

                    v_new_dest.z = v_new_dest.z - (JUMP_HEIGHT + 1.0f);

                    // check if we didn't hit anything, if so then it's in mid-air
                    if (::IsVisible(v_new_src, v_new_dest, NULL))
                    {
                         condebug("to is in midair");
                         debugbeam(from, to);
                         return false;  // can't reach this one
                    }
               }
*/

               // check if distance to ground increases more than jump height
               // at points between from and to...

               vec v_temp = to; vsub(v_temp, from);
               vec v_direction = Normalize(v_temp);  // 1 unit long
               vec v_check = from;
               vec v_down = from;

               v_down.z = v_down.z - 100.0f;  // straight down

               TraceLine(v_check, v_down, NULL, false, &tr);

                 // height from ground
               last_height = GetDistance(v_check, tr.end);

               distance = GetDistance(to, v_check);  // distance from goal

               while (distance > 2.0f)
               {
                    // move 2 units closer to the goal
                    v_temp = v_direction;
                    vmul(v_temp, 2.0f);
                    vadd(v_check, v_temp);

                    v_down = v_check;
                    v_down.z = v_down.z - 100.0f;

                    TraceLine(v_check, v_down, NULL, false, &tr);

                    curr_height = GetDistance(v_check, tr.end);

                    // is the difference in the last height and the current height
                    // higher that the jump height?
                    if ((last_height - curr_height) >= flMaxHeight)
                    {
                         // can't get there from here...
                         //condebug("traces failed to to");
                         debugbeam(from, to);
                         return false;
                    }

                    last_height = curr_height;

                    distance = GetDistance(to, v_check);  // distance from goal
               }

               return true;
          }
     }

     return false;
}

void CBot::HearSound(int n, vec *o)
{
     // Has the bot already an enemy?
     if (m_pMyEnt->enemy) return;
     
     // Is the sound not interesting?
     if ((n==S_DIE1) || (n==S_DIE2) || (n==S_DEATHR) || (n==S_DEATHE) || (n==S_DEATHS) ||
         (n==S_DEATHB) || (n==S_DEATHH) || (n==S_DEATHD) || (n==S_PIGGR2))
          return;
     
     int soundvol = m_pBotSkill->iMaxHearVolume - 
                            (int)(GetDistance(*o)*3*m_pBotSkill->iMaxHearVolume/255);
          
     if (soundvol == 0) return;
          
     // Look who made the sound(check for the nearest enemy)
     float flDist, flNearestDist = 3.0f; // Range of 3 units
     dynent *pNearest = NULL;
     
     if (m_sp)
     {
          // Check all monsters
          loopv(monsters)
          {
               if (!monsters[i] || monsters[i]->state != CS_ALIVE) continue;
          
               flDist = GetDistance(*o, monsters[i]->o);
               if ((flDist < flNearestDist) && IsVisible(monsters[i]))
               {
                    pNearest = monsters[i];
                    flNearestDist = flDist;
               }
          }
     }
     else
     {
          // Check all players first
          loopv(players)
          {
               if (!players[i] || (players[i]->state != CS_ALIVE) ||
                   isteam(m_pMyEnt->team, players[i]->team))
                    continue;
          
               flDist = GetDistance(*o, players[i]->o);
               if ((flDist < flNearestDist) && IsVisible(players[i]))
               {
                    pNearest = players[i];
                    flNearestDist = flDist;
               }
          }
     
          // Check all bots
          loopv(bots)
          {
               if (!bots[i] || (bots[i]->state != CS_ALIVE) ||
                   isteam(m_pMyEnt->team, bots[i]->team) || (bots[i] == m_pMyEnt))
                    continue;
          
               flDist = GetDistance(*o, bots[i]->o);
               if ((flDist < flNearestDist) && IsVisible(bots[i]))
               {
                    pNearest = bots[i];
                    flNearestDist = flDist;
               }
          }
          
          // Check local player
          if (player1 && (player1->state == CS_ALIVE) &&
              !isteam(m_pMyEnt->team, player1->team))
          {
               flDist = GetDistance(*o, player1->o);
               if ((flDist < flNearestDist) && IsVisible(player1))
               {
                    pNearest = player1;
                    flNearestDist = flDist;
               }    
          }
     }
     
     if (pNearest)
     {
          m_pMyEnt->enemy = pNearest;
               
          // Add shoot delay
          m_iShootDelay = lastmillis + GetShootDelay();
     }
}

bool CBot::IsInFOV(const vec &o)
{              
     vec v2, forward, right, up;
     float flDot;

     AnglesToVectors(GetViewAngles(), forward, right, up);

     v2 = o;
     vsub(v2, m_pMyEnt->o);
     v2.z = 0.0f; // Make 2D
     v2 = Normalize(v2);
     forward.z = 0; // Make 2D
          
     flDot = dotprod(v2 , forward);

     return(flDot >= 0.5f); // sin2(0.5) == 60 degrees FOV
}
     
// Code of CBot - End
