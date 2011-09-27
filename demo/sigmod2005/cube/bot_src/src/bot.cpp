//
// C++ Implementation: bot
//
// Description: 
//
// Main bot file
//
// Author:  Rick <rickhelmus@gmail.com>, (C) 2004
//
//
//

#include <string>
#include <map>
#include <vector>
#include <NMSTL/ptr>
#include "tuples.h"

#include "cube.h"

// Added by yna
extern std::map<int,int> stream_seq_nos;
std::map<int, bool> bot_sent_tuple;

bool enable_motion_logging = false;
std::string motion_log_filename;

dvector bots;
dvector pending_bots;
CBotManager BotManager;

struct itemstat { int add, max, sound; };

extern void send_cube_tuple(int, borealis_tuple*);

extern std::vector< std::pair<int, NMSTL::ptr<borealis_tuple> > > client_pending_message;
extern int getclientnum();
extern bool commandmode;
extern int tuplerate;

extern bool intermission;
extern bool listenserv;
extern ENetHost *clienthost;
extern vector<server_entity> sents;
extern int triggertime;
extern itemstat itemstats[];

extern float rad(float x);
extern void normalise(dynent *m, float angle);
extern bool enemylos(dynent *m, vec &v);
extern void spawnstate(dynent *d);
extern void pickup(int n, dynent *d);
extern void respawnself(void);

const vec g_vecZero = {0,0,0};

//Bot class begin

void CBot::Spawn()
{
     // Init all bot variabeles
     m_pMyEnt->eyeheight = 3.2f;
     m_pMyEnt->aboveeye = 0.7f;
     m_pMyEnt->radius = 1.1f;
         
     spawnplayer(m_pMyEnt);
     
     m_pMyEnt->targetyaw = m_pMyEnt->yaw = m_pMyEnt->targetpitch = m_pMyEnt->pitch = 0.0f;
     m_pMyEnt->move = 0;
     m_pMyEnt->enemy = NULL;
     m_pMyEnt->maxspeed = 22.0f;
     m_pMyEnt->health = 100;
     m_pMyEnt->armour = 50;
     m_pMyEnt->pitch = 0;
     m_pMyEnt->roll = 0;
     m_pMyEnt->state = CS_ALIVE;
     m_pMyEnt->anger = 0;
     m_pMyEnt->pBot = this;     
     loopi(NUMGUNS) m_pMyEnt->ammo[i] = 0;
     m_pMyEnt->ammo[GUN_FIST] = 1;
     if(m_noitems)
     {
          m_pMyEnt->gunselect = GUN_RIFLE;
          m_pMyEnt->armour = 0;
          if(m_noitemsrail)
          {
               m_pMyEnt->health = 1;
               m_pMyEnt->ammo[GUN_RIFLE] = 100;
          }
          else
          {
               if(gamemode==12) // eihrul's secret "instafist" mode
               {
                    m_pMyEnt->gunselect = GUN_FIST;
               }
               else
               {
                    m_pMyEnt->health = 256;
                    if(m_tarena)
                    {
                         int gun1 = rnd(4)+1;
                         botbaseammo(m_pMyEnt->gunselect = gun1, m_pMyEnt);
                         for(;;)
                         {
                              int gun2 = rnd(4)+1;
                              if(gun1!=gun2) { botbaseammo(gun2, m_pMyEnt); break; };
                         }
                    }
                    else if(m_arena)    // insta arena
                    {
                         m_pMyEnt->ammo[GUN_RIFLE] = 100;
                    }
                    else // efficiency
                    {
                         loopi(4) botbaseammo(i+1, m_pMyEnt);
                         m_pMyEnt->gunselect = GUN_CG;
                    }
                    m_pMyEnt->ammo[GUN_CG] /= 2;
               }
          }
     }
     else
     {
          m_pMyEnt->ammo[GUN_SG] = 5;
          SelectGun(GUN_SG);
     }
         
     m_iShootDelay = m_iChangeWeaponDelay = 0;
     m_iCheckEnvDelay = 0;
     m_vPrevOrigin = g_vecZero;
     m_iStuckCheckDelay = lastmillis + 250;
     m_bStuck = false;
     m_iStuckTime = 0;
     m_iStrafeTime = m_iStrafeCheckDelay = 0;
     m_iMoveDir = m_iStrafeDir = 0;
     m_iCombatNavTime = 0;
     m_iEnemySearchDelay = 0;
     m_bCombatJump = false;
     m_bShootAtFeet = (RandomLong(1, 100) <= m_pBotSkill->sShootAtFeetWithRLPercent);
        
     m_iLastJumpPad = 0;
     m_pTargetEnt = NULL;
     m_iCheckTeleporterDelay = m_iCheckJumppadsDelay = 0;
     m_iCheckEntsDelay = 0;
     m_iLookForWaypointTime = 0;
        
     m_iAimDelay = 0;
     m_fYawToTurn = m_fPitchToTurn = 0.0f;
     
     m_vGoal = m_vWaterGoal = g_vecZero;
     
     ResetWaypointVars();
}

void CBot::Think()
{
     if (intermission)
          return;
             
     // Bot is dead?
     if (m_pMyEnt->state == CS_DEAD)
     {
          if(lastmillis-m_pMyEnt->lastaction<1200)
          {
               m_pMyEnt->move = 0;
               moveplayer(m_pMyEnt, 1, false);
          }
          else if (!m_arena)
               Spawn();
     
          return;
     }
                              
     CheckItems();
     CheckQuad(curtime);
     
     // Basic AI
     if (!BotManager.IdleBots())
     {               
          // Default bots will run forward
          m_pMyEnt->move = 1;
        
          // Default bots won't strafe
          m_pMyEnt->strafe = 0;
        
          if (!BotManager.BotsShoot() && m_pMyEnt->enemy)
               m_pMyEnt->enemy = NULL;
             
          // Find enemy
          if (BotManager.BotsShoot() && FindEnemy())
          {
               // Shoot at enemy
               ShootEnemy();
          }

          // Navigate        
          HomeThroughMap();
     }
     else
          m_pMyEnt->move = 0;
                  
     // Aim to ideal yaw and pitch
     AimToIdeal();
     
     // Store current location, to see if the bot is stuck
     m_vPrevOrigin = m_pMyEnt->o;
     
     // Don't check for stuck if the bot doesn't want to move
     if (!m_pMyEnt->move && !m_pMyEnt->strafe)
          m_iStuckCheckDelay = max(m_iStuckCheckDelay, (lastmillis+100));
          
     // Move the bot
     moveplayer(m_pMyEnt, 1, false);
     LogBotMotion(); // Added by yna.
     
     // Update bot info on all clients
     //SendBotInfo();
     SendBotTuple(); // Added by yna
}

dynent *CBot::CreateBot(const char *team, const char *skill, const char *name)
{
     dynent *m = newdynent();
     if (!m) return NULL;
     m->pBot = new CBot;
     m->bIsBot = true;
     m->pBot->m_pMyEnt = m;
     m->pBot->m_iLastBotUpdate = 0;
     m->pBot->m_bSendC2SInit = false;

     if (name && *name)
          strn0cpy(m->name, name, 16);
     else
          strn0cpy(m->name, BotManager.GetBotName(), 16);
     
     if (team && *team && strcmp(team, "random"))
          strn0cpy(m->team, team, 5);
     else
          strn0cpy(m->team, BotManager.GetBotTeam(), 5);
     
     if (skill && *skill && strcmp(skill, "random"))
     {
          if (!stricmp(skill, "best"))
               m->pBot->m_sSkillNr = 0;
          else if (!stricmp(skill, "good"))
               m->pBot->m_sSkillNr = 1;
          else if (!stricmp(skill, "medium"))
               m->pBot->m_sSkillNr = 2;
          else if (!stricmp(skill, "worse"))
               m->pBot->m_sSkillNr = 3;
          else if (!stricmp(skill, "bad"))
               m->pBot->m_sSkillNr = 4;
          else
          {
               conoutf("Wrong skill specified. Should be best, good, medium, "
                       "worse or bad");
               conoutf("Using default skill instead...");
               m->pBot->m_sSkillNr = BotManager.m_sBotSkill;
          }
     }
     else // No skill specified, use default
          m->pBot->m_sSkillNr = BotManager.m_sBotSkill;
          
     m->pBot->m_pBotSkill = &BotManager.m_BotSkills[m->pBot->m_sSkillNr];
    
     m->pBot->Spawn();
     pending_bots.add(m);
     return m;     
}

void CBot::AimToVec(const vec &o)
{
     m_pMyEnt->targetpitch = atan2(o.z-m_pMyEnt->o.z, GetDistance(o))*180/PI;
     m_pMyEnt->targetyaw = -(float)atan2(o.x - m_pMyEnt->o.x, o.y -
                                         m_pMyEnt->o.y)/PI*180+180;
}

void CBot::AimToIdeal()
{
     if (m_iAimDelay > lastmillis)
          return;

     //printf("Using bot skill %d\n", m_sSkillNr);

     //printf("Calling RandomFloat %3.9f %3.9f\n", m_pBotSkill->flMinAimYOffset, m_pBotSkill->flMaxAimYOffset);

     float MaxXSpeed = RandomFloat(m_pBotSkill->flMinAimXSpeed,
                                   m_pBotSkill->flMaxAimXSpeed);
     float MaxYSpeed = RandomFloat(m_pBotSkill->flMinAimYSpeed,
                                   m_pBotSkill->flMaxAimYSpeed);
     float XOffset = RandomFloat(m_pBotSkill->flMinAimXOffset,
                                 m_pBotSkill->flMaxAimXOffset);
     float YOffset = RandomFloat(m_pBotSkill->flMinAimYOffset,
                                 m_pBotSkill->flMaxAimYOffset);
     float RealXOffset, RealYOffset;
     float AimXSpeed = MaxXSpeed, AimYSpeed = MaxYSpeed;
     float XDiff = fabs(m_pMyEnt->targetpitch - m_pMyEnt->pitch);
     float YDiff = fabs(m_pMyEnt->targetyaw - m_pMyEnt->yaw);

     // How higher the diff, how higher the offsets and aim speed
          
     if (XOffset)
     {
          if (RandomLong(0, 1))
               RealXOffset = XDiff * (XOffset / 100.0f);
          else
               RealXOffset = -(XDiff * (XOffset / 100.0f));
     }
     else
          RealXOffset = 0.0f;

     if (YOffset)
     {
          if (RandomLong(0, 1))
               RealYOffset = YDiff * (YOffset / 100.0f);
          else
               RealYOffset = -(YDiff * (YOffset / 100.0f));
     }
     else
          RealYOffset = 0.0f;

          
     if (XDiff >= 1.0f)
          AimXSpeed = (AimXSpeed * (XDiff / 80.0f)) + (AimXSpeed * 0.25f);
     else
          AimXSpeed *= 0.01f;

     //printf("AimYSpeed my:%3.9f ay:%3.9f yd:%3.9f\n", MaxYSpeed, AimYSpeed, YDiff);

     if (YDiff >= 1.0f)
          AimYSpeed = (AimYSpeed * (YDiff / 70.0f)) + (AimYSpeed * 0.25f);
     else
          AimYSpeed *= 0.015f;

     m_fPitchToTurn = fabs((m_pMyEnt->targetpitch + RealXOffset) - m_pMyEnt->pitch);
     m_fYawToTurn = fabs((m_pMyEnt->targetyaw + RealYOffset) - m_pMyEnt->yaw);
     
     float flIdealPitch = ChangeAngle(AimXSpeed, m_pMyEnt->targetpitch + RealXOffset, m_pMyEnt->pitch);
     float flIdealYaw = ChangeAngle(AimYSpeed, m_pMyEnt->targetyaw + RealYOffset, m_pMyEnt->yaw);
     
//     m_pMyEnt->pitch = WrapXAngle(m_pMyEnt->targetpitch);
//     m_pMyEnt->yaw = WrapYZAngle(m_pMyEnt->targetyaw);
                 
     m_pMyEnt->pitch = WrapXAngle(flIdealPitch);
     m_pMyEnt->yaw = WrapYZAngle(flIdealYaw);

     //printf("AimToIdeal yaw sp:%3.9f ty:%3.9f fid:%3.9f yaw:%3.9f\n", AimYSpeed, m_pMyEnt->targetyaw + RealYOffset, flIdealYaw, m_pMyEnt->yaw);
     
     /* OLD AIM CODE
     m_iAimDelay = lastmillis + RandomLong(10, 25); // UNDONE
     
     normalise(m_pMyEnt, m_pMyEnt->targetyaw);
         if(m_pMyEnt->targetyaw>m_pMyEnt->yaw)             // slowly turn bot towards his target
         {
             m_pMyEnt->yaw += curtime*0.5f;
             if(m_pMyEnt->targetyaw<m_pMyEnt->yaw) m_pMyEnt->yaw = m_pMyEnt->targetyaw;
         }
         else
         {
             m_pMyEnt->yaw -= curtime*0.5f;
          if(m_pMyEnt->targetyaw>m_pMyEnt->yaw) m_pMyEnt->yaw = m_pMyEnt->targetyaw;
         }

        normalise(m_pMyEnt, m_pMyEnt->targetpitch);
         if(m_pMyEnt->targetpitch>m_pMyEnt->pitch)             // slowly turn bot towards his target
         {
             m_pMyEnt->pitch += curtime*0.5f;
             if(m_pMyEnt->targetpitch<m_pMyEnt->pitch) m_pMyEnt->pitch = m_pMyEnt->targetpitch;
         }
         else
         {
             m_pMyEnt->pitch -= curtime*0.5f;
          if(m_pMyEnt->targetpitch>m_pMyEnt->pitch) m_pMyEnt->pitch = m_pMyEnt->targetpitch;
         }
        */
}     

// Function code by Botman
float CBot::ChangeAngle(float speed, float ideal, float current)
{
     float current_180;  // current +/- 180 degrees
     float diff;

     // find the difference in the current and ideal angle
     diff = fabs(current - ideal);
    
     // speed that we can turn during this frame...
     speed = speed * (float(BotManager.m_iFrameTime)/1000.0f);

     // check if difference is less than the max degrees per turn
     if (diff < speed)
          speed = diff;  // just need to turn a little bit (less than max)

     // check if the bot is already facing the idealpitch direction...
     if (diff <= 1.0f)
          return ideal;
          
     if ((current >= 180) && (ideal >= 180))
     {
          if (current > ideal)
               current -= speed;
          else
               current += speed;
     }
     else if ((current >= 180) && (ideal < 180))
     {
          current_180 = current - 180;

          if (current_180 > ideal)
               current += speed;
          else
               current -= speed;
     }
     else if ((current < 180) && (ideal >= 180))
     {
          current_180 = current + 180;
          
          if (current_180 > ideal)
               current += speed;
          else
               current -= speed;
     }
     else  // (current < 180) && (ideal < 180)
     {
          if (current > ideal)
               current -= speed;
          else
               current += speed;
     }


     return current;     
}

void CBot::BotPain(int damage, dynent *d)
{
     if(m_pMyEnt->state!=CS_ALIVE || editmode || intermission) return;
     int ad = damage*(m_pMyEnt->armourtype+1)*20/100; // let armour absorb when possible
     if(ad>m_pMyEnt->armour) ad = m_pMyEnt->armour;
     m_pMyEnt->armour -= ad;
     damage -= ad;
     float droll = damage/0.5f;
     m_pMyEnt->roll += m_pMyEnt->roll>0 ? droll : (m_pMyEnt->roll<0 ? -droll : (rnd(2) ? droll : -droll));  // give player a kick depending on amount of damage
     if((m_pMyEnt->health -= damage)<=0)
     {
          if (player1 == d)
          {
               int frags;
               if(isteam(player1->team, m_pMyEnt->team))
               {
                    frags = -1;
                    conoutf("you fragged a teammate (%s)", (int)m_pMyEnt->name);
               }
               else
               {
                    frags = 1;
                    conoutf("you fragged %s", (int)m_pMyEnt->name);
               }
               //addmsg(1, 2, SV_FRAGS, player1->frags += frags);
	       player1->frags += frags;
	       NMSTL::ptr<cube_frags_tuple> cft = NMSTL::ptr<cube_frags_tuple>(new cube_frags_tuple());
	       cft->clientnum = getclientnum();
	       cft->frags = player1->frags;
	       cft->isbot = 0;
	       client_pending_message.push_back(std::make_pair((int)SV_FRAGS, cft.dynamic_cast_to<borealis_tuple>()));

               //addmsg(1, 4, SV_BOTDIED, BotManager.GetBotIndex(m_pMyEnt), -1, false);
	       NMSTL::ptr<cube_death_tuple> cdt = NMSTL::ptr<cube_death_tuple>(new cube_death_tuple());
	       cdt->clientnum = BotManager.GetBotIndex(m_pMyEnt);
	       cdt->client_isbot = 1;
	       cdt->actor = -1;
	       cdt->actor_isbot = 0;

	       // Note change from SV_BOTDIED to SV_DIED.
	       client_pending_message.push_back(std::make_pair((int)SV_DIED, cdt.dynamic_cast_to<borealis_tuple>()));	       
          }
          else if (d->monsterstate)
               conoutf("%s got killed by %s", (int)m_pMyEnt->name, (int)d->name);
          else
          {                 
               int KillerIndex = -1;
         
               if (d == m_pMyEnt)
               {
                    conoutf("%s suicided", (int)d->name);
                    KillerIndex = BotManager.GetBotIndex(d);

                    //addmsg(1, 3, SV_BOTFRAGS, KillerIndex, --d->frags);
		    NMSTL::ptr<cube_frags_tuple> cft = NMSTL::ptr<cube_frags_tuple>(new cube_frags_tuple());
		    cft->clientnum = KillerIndex;
		    cft->frags = --d->frags;
		    cft->isbot = 1;

		    // Note change from SV_BOTFRAGS to SV_FRAGS
		    client_pending_message.push_back(std::make_pair((int)SV_FRAGS, cft.dynamic_cast_to<borealis_tuple>()));
               }     
               else
               {     
                    if (d->bIsBot)
                    {
                         KillerIndex = BotManager.GetBotIndex(d);
                         //addmsg(1, 3, SV_BOTFRAGS, KillerIndex, ++d->frags);
			 NMSTL::ptr<cube_frags_tuple> cft = NMSTL::ptr<cube_frags_tuple>(new cube_frags_tuple());
			 cft->clientnum = KillerIndex;
			 cft->frags = ++d->frags;
			 cft->isbot = 1;

			 // Note change from SV_BOTFRAGS to SV_FRAGS
			 client_pending_message.push_back(std::make_pair((int)SV_FRAGS, cft.dynamic_cast_to<borealis_tuple>()));
                    }
                    else
                         loopv(players){ if (players[i] == d) { KillerIndex = i; break;} }
                  
                    if(isteam(m_pMyEnt->team, d->team))
                    {
                         conoutf("%s fragged his teammate (%s)", (int)d->name,
                                 (int)m_pMyEnt->name);
                    }
                    else
                    {
                         conoutf("%s fragged %s", (int)d->name, (int)m_pMyEnt->name);
                    }
               }
               //addmsg(1, 4, SV_BOTDIED, BotManager.GetBotIndex(m_pMyEnt), KillerIndex,
               //       d->bIsBot);

	       NMSTL::ptr<cube_death_tuple> cdt = NMSTL::ptr<cube_death_tuple>(new cube_death_tuple());
	       cdt->clientnum = BotManager.GetBotIndex(m_pMyEnt);
	       cdt->client_isbot = 1;
	       cdt->actor = KillerIndex;
	       cdt->actor_isbot = d->bIsBot;

	       // Note change from SV_BOTDIED to SV_DIED.
	       client_pending_message.push_back(std::make_pair((int)SV_DIED, cdt.dynamic_cast_to<borealis_tuple>()));	       
          }

          m_pMyEnt->lifesequence++;
          m_pMyEnt->deaths++;
          m_pMyEnt->attacking = false;
          m_pMyEnt->state = CS_DEAD;
          m_pMyEnt->pitch = 0;
          m_pMyEnt->roll = 60;
          playsound(S_DIE1+rnd(2), &m_pMyEnt->o);
          spawnstate(m_pMyEnt);
          m_pMyEnt->lastaction = lastmillis;
     }
     else
     {
          playsound(S_PAIN6, &m_pMyEnt->o);
     }
}

void CBot::CheckItems()
{
     if(editmode && !commandmode) return;

     loopv(ents)
     {
          entity &e = ents[i];
          if(e.type==NOTUSED) continue;
          if ((e.type != TELEPORT) && (e.type != JUMPPAD))
          {
               if (!e.spawned)
                    continue;
               //if ((i < sents.length()) && (!sents[i].spawned))
               //     continue;
          }
               
          if(OUTBORD(e.x, e.y)) continue;
          vec v = { e.x, e.y, S(e.x, e.y)->floor+m_pMyEnt->eyeheight };
          vdist(dist, t, m_pMyEnt->o, v);
          if(dist<(e.type==TELEPORT ? 4 : 2.5))
          {
	      //if (e.type == TELEPORT) printf("Calling teleport %d\n", i);
	      PickUp(i);
          }
     }
}

void CBot::CheckQuad(int Time)
{
    if(m_pMyEnt->quadmillis && (m_pMyEnt->quadmillis -= Time)<0)
    {
        m_pMyEnt->quadmillis = 0;
        botplaysound(S_PUPOUT, m_pMyEnt);
        //conoutf("quad damage is over");
    }
}

void CBot::PickUp(int n)
{
    int np = 1;

    loopv(players) if(players[i]) np++;
    loopv(bots) if(bots[i]) np++;

    np = np<3 ? 4 : (np>4 ? 2 : 3); // spawn times are dependent on number of players
    int ammo = np*2;

    switch(ents[n].type)
    {
        case I_SHELLS:  AddItem(n, m_pMyEnt->ammo[1], ammo); break;
        case I_BULLETS: AddItem(n, m_pMyEnt->ammo[2], ammo); break;
        case I_ROCKETS: AddItem(n, m_pMyEnt->ammo[3], ammo); break;
        case I_ROUNDS:  AddItem(n, m_pMyEnt->ammo[4], ammo); break;
        case I_HEALTH:  AddItem(n, m_pMyEnt->health,  np*5); break;
        case I_BOOST:   AddItem(n, m_pMyEnt->health,  60);   break;

        case I_GREENARMOUR:
            // (100h/100g only absorbs 166 damage)
            if(m_pMyEnt->armourtype==A_YELLOW && m_pMyEnt->armour>66) break;
            AddItem(n, m_pMyEnt->armour, 20);
            break;

        case I_YELLOWARMOUR:
            AddItem(n, m_pMyEnt->armour, 20);
            break;

        case I_QUAD:
            AddItem(n, m_pMyEnt->quadmillis, 60);
            break;
            
        case CARROT:
            ents[n].spawned = false;
            triggertime = lastmillis;
            trigger(ents[n].attr1, ents[n].attr2, false);  // needs to go over server for multiplayer
            
            // HACK: Reset ent goal if bot was looking for this ent
            if (m_pTargetEnt == &ents[n])
            {
               m_pTargetEnt = NULL;
               m_vGoal = g_vecZero;
               ResetWaypointVars();
               m_iCheckEntsDelay = lastmillis + RandomLong(5000, 10000);
            }        
            
            break;

        case TELEPORT:
        {
            static int lastteleport = 0;
            if(lastmillis-lastteleport<500) break;
            lastteleport = lastmillis;
            teleport(n, m_pMyEnt);
            
            // HACK: Reset ent goal if bot was looking for this ent
            if (m_pTargetEnt == &ents[n])
            {
               m_pTargetEnt = NULL;
               m_vGoal = g_vecZero;
               ResetWaypointVars();
               m_iCheckEntsDelay = lastmillis + RandomLong(5000, 10000);
            }        
            
            break;
        };
        
        case JUMPPAD:
        {
            if(lastmillis-m_iLastJumpPad<300) break;
            m_iLastJumpPad = lastmillis;
            vec v = { (int)(char)ents[n].attr3/10.0f, (int)(char)ents[n].attr2/10.0f, ents[n].attr1/10.0f };
            vadd(m_pMyEnt->vel, v);
            botplaysound(S_JUMPPAD, m_pMyEnt);

            // HACK: Reset ent goal if bot was looking for this ent
            if (m_pTargetEnt == &ents[n])
            {
               m_pTargetEnt = NULL;
               m_vGoal = g_vecZero;
               ResetWaypointVars();
               m_iCheckEntsDelay = lastmillis + RandomLong(5000, 10000);
            }        
                                    
            break;
        };
    };
};

void CBot::AddItem(int i, int &v, int spawnsec)
{
    //if((i>=sents.length()) || (!sents[i].spawned))
    //      return;

     if(v>=itemstats[ents[i].type-I_SHELLS].max)  // don't pick up if not needed
          return;
             
     itemstat &is = itemstats[ents[i].type-I_SHELLS];
     ents[i].spawned = false;
     v += is.add;
     if(v>is.max) v = is.max;
     botplaysound(is.sound, m_pMyEnt);
     //sents[i].spawned = false;
     //sents[i].spawnsecs = spawnsec;
        
     //addmsg(1, 4, SV_ITEMPICKUP, i, m_classicsp ? 100000 : spawnsec, false);
     NMSTL::ptr<cube_item_pickup_tuple> ct = NMSTL::ptr<cube_item_pickup_tuple>(new cube_item_pickup_tuple());
     ct->clientnum = BotManager.GetBotIndex(m_pMyEnt);
     ct->item_index = i;
     ct->spawn_secs = (m_classicsp ? 100000.0 : (double) spawnsec);
     ct->client_pickup = 0;
     client_pending_message.push_back(std::make_pair((int)SV_ITEMPICKUP, ct.dynamic_cast_to<borealis_tuple>()));

        
     // HACK: Reset ent goal if bot was looking for for this ent
     if (m_pTargetEnt == &ents[i])
     {
          m_pTargetEnt = NULL;
          m_vGoal = g_vecZero;
          ResetWaypointVars();
          m_iCheckEntsDelay = lastmillis + RandomLong(5000, 10000);
     }        
}


// Added by yna
// Assumption: only called by a pending bot.
void CBot::SendBotInitTuple()
{
    if(!m_bSendC2SInit)    // tell other clients who I am
    {
        NMSTL::ptr<cube_add_bot_tuple> cat = NMSTL::ptr<cube_add_bot_tuple>(new cube_add_bot_tuple());
        cat->host_clientnum = getclientnum();
        cat->clientnum = BotManager.GetPendingBotIndex(m_pMyEnt);
        memcpy(cat->player_name, m_pMyEnt->name, 32);
        memcpy(cat->team_name, m_pMyEnt->team, 32);
        cat->lifesequence = m_pMyEnt->lifesequence;
        m_bSendC2SInit = true;
        
        send_cube_tuple(SV_ADDBOT, cat.dynamic_cast_to<borealis_tuple>().get());

        bot_sent_tuple[SV_ADDBOT] = true;
    };

}
// end yna

void CBot::SendBotTuple()
{
     int update_elapse = (tuplerate < 0? 1000 * (-tuplerate) : (tuplerate == 0? 40 : 1000/tuplerate));
     if(lastmillis-m_iLastBotUpdate < update_elapse) return;    // don't update faster than 25fps
     m_iLastBotUpdate = lastmillis;

     NMSTL::ptr<cube_position_tuple> cpt = NMSTL::ptr<cube_position_tuple>(new cube_position_tuple());
     cpt->clientnum  = BotManager.GetBotIndex(m_pMyEnt);
     memset(cpt->team_name, 0, 32);
     memcpy(cpt->team_name, m_pMyEnt->team, strlen(m_pMyEnt->team));
     cpt->position_x = (int)(m_pMyEnt->o.x*DMF);
     cpt->position_y = (int)(m_pMyEnt->o.y*DMF);
     cpt->position_z = (int)(m_pMyEnt->o.z*DMF);
     cpt->yaw        = (int)(m_pMyEnt->yaw*DAF);
     cpt->pitch      = (int)(m_pMyEnt->pitch*DAF);
     cpt->roll       = (int)(m_pMyEnt->roll*DAF);
     cpt->velocity_x = (int)(m_pMyEnt->vel.x*DVF);
     cpt->velocity_y = (int)(m_pMyEnt->vel.y*DVF);
     cpt->velocity_z = (int)(m_pMyEnt->vel.z*DVF);
     // pack rest in 1 byte: strafe:2, move:2, onfloor:1, state:3
     cpt->rest_info  = (m_pMyEnt->strafe&3) | ((m_pMyEnt->move&3)<<2) |
                                             (((int)m_pMyEnt->onfloor)<<4) |
                                              ((editmode ? CS_EDITING :
                                                m_pMyEnt->state)<<5);
     cpt->isbot      = 1;
     cpt->seq_no     = stream_seq_nos[SV_POS];

     // yna -- left here because of LetBotUpdateStats()
     if(!m_bSendC2SInit)    // tell other clients who I am
     {
	 NMSTL::ptr<cube_add_bot_tuple> cat = NMSTL::ptr<cube_add_bot_tuple>(new cube_add_bot_tuple());
	 cat->host_clientnum = getclientnum();
	 cat->clientnum = BotManager.GetBotIndex(m_pMyEnt);
	 memcpy(cat->player_name, m_pMyEnt->name, 32);
	 memcpy(cat->team_name, m_pMyEnt->team, 32);
	 cat->lifesequence = m_pMyEnt->lifesequence;
	 m_bSendC2SInit = true;

	 send_cube_tuple(SV_ADDBOT, cat.dynamic_cast_to<borealis_tuple>().get());

         bot_sent_tuple[SV_ADDBOT] = true;
     };
     

     send_cube_tuple(SV_POS, cpt.dynamic_cast_to<borealis_tuple>().get());
     bot_sent_tuple[SV_POS] = true;
}

/*
  void CBot::SendBotInfo()
  {
  if(lastmillis-m_iLastBotUpdate<40) return;    // don't update faster than 25fps
     m_iLastBotUpdate = lastmillis;

     int x = (int)(m_pMyEnt->o.x*DMF);
     int y = (int)(m_pMyEnt->o.y*DMF);
     int z = (int)(m_pMyEnt->o.z*DMF);
     int yaw = (int)(m_pMyEnt->yaw*DAF);
     int pitch = (int)(m_pMyEnt->pitch*DAF);
     int roll = (int)(m_pMyEnt->roll*DAF);
     int velx = (int)(m_pMyEnt->vel.x*DVF);
     int vely = (int)(m_pMyEnt->vel.y*DVF);
     int velz = (int)(m_pMyEnt->vel.z*DVF);
     // pack rest in 1 byte: strafe:2, move:2, onfloor:1, state:3
     int moveflags = (m_pMyEnt->strafe&3) | ((m_pMyEnt->move&3)<<2) |
                                             (((int)m_pMyEnt->onfloor)<<4) |
                                              ((editmode ? CS_EDITING :
                                                m_pMyEnt->state)<<5);
       
     ENetPacket *packet = enet_packet_create (NULL, MAXTRANS, 0);
     uchar *start = packet->data;
     uchar *p = start+2;
        
     putint(p, SV_BOTUPDATE);
     putint(p, BotManager.GetBotIndex(m_pMyEnt));
     putint(p, x); // quantize coordinates to 1/16th of a cube, between 1 and 3 bytes
     putint(p, y);
     putint(p, z);
     putint(p, yaw);
     putint(p, pitch);
     putint(p, roll);
     putint(p, velx);     // quantize to 1/100, almost always 1 byte
     putint(p, vely);
     putint(p, velz);
     // pack rest in 1 byte: strafe:2, move:2, onfloor:1, state:3
     putint(p, moveflags);
     
     if(!m_bSendC2SInit)    // tell other clients who I am
     {
          packet->flags = ENET_PACKET_FLAG_RELIABLE;
          m_bSendC2SInit = true;
          putint(p, SV_ADDBOT);
          putint(p, BotManager.GetBotIndex(m_pMyEnt));
          sendstring(m_pMyEnt->name, p);
          sendstring(m_pMyEnt->team, p);
          putint(p, m_pMyEnt->lifesequence);
     };
     
 
     *(ushort *)start = ENET_HOST_TO_NET_16(p-start);
     enet_packet_resize(packet, p-start);
     incomingdemodata(start, p-start, true);
     if(clienthost)
     {
          enet_host_broadcast(clienthost, 0, packet);
          enet_host_flush(clienthost);
     }
     else
          localclienttoserver(packet);
}
*/

float CBot::GetDistance(const vec &o)
{
     vdist(flDist, vecto, m_pMyEnt->o, o);
     return flDist;
}

float CBot::GetDistance(const vec &v1, const vec &v2)
{
     vdist(flDist, vecto, v1, v2);
     return flDist;
}

float CBot::GetDistance(entity *e)
{
     vec v = { e->x, e->y, e->z };
     vdist(flDist, vecto, m_pMyEnt->o, v);
     return flDist;
}

bool CBot::SelectGun(int Gun)
{
     if (m_pMyEnt->ammo[Gun])
     {
          if (m_pMyEnt->gunselect != Gun)
               botplaysound(S_WEAPLOAD, m_pMyEnt);
               
          m_pMyEnt->gunselect = Gun;
          return true;
     }
     return false;
}

bool CBot::IsVisible(entity *e)
{
     vec v = { e->x, e->y, e->z };
     return ::IsVisible(m_pMyEnt->o, v, m_pMyEnt);
}

bool CBot::IsVisible(vec o, EDirection Dir, float flDist, bool CheckPlayers)
{
     static vec angles, end, forward, right, up;
     static traceresult_s tr;
        
     end = o;
     angles = GetViewAngles();
     
     switch(Dir)
     {
     case FORWARD:
          AnglesToVectors(angles, forward, right, up);
          vmul(forward, flDist);
          vadd(end, forward);
          break;
     case FORWARD_LEFT:
          angles.y = WrapYZAngle(angles.y - 45.0f);
          AnglesToVectors(angles, forward, right, up);
          vmul(forward, flDist);
          vadd(end, forward);
          break;
     case FORWARD_RIGHT:
          angles.y = WrapYZAngle(angles.y + 45.0f);
          AnglesToVectors(angles, forward, right, up);    
          vmul(forward, flDist);
          vadd(end, forward);
          break;
     case BACKWARD:
          AnglesToVectors(angles, forward, right, up);
          vmul(forward, flDist);
          vsub(end, forward);
          break;
     case BACKWARD_LEFT:
          angles.y = WrapYZAngle(angles.y + 45.0f);
          AnglesToVectors(angles, forward, right, up);    
          vmul(forward, flDist);
          vsub(end, forward);
          break;
     case BACKWARD_RIGHT:
          angles.y = WrapYZAngle(angles.y - 45.0f);
          AnglesToVectors(angles, forward, right, up);     
          vmul(forward, flDist);
          vsub(end, forward);
          break;
     case LEFT:
          AnglesToVectors(angles, forward, right, up);
          vmul(right, flDist);
          vsub(end, right);
          break;
     case RIGHT:
          AnglesToVectors(angles, forward, right, up);
          vmul(right, flDist);
          vadd(end, right);
          break;          
     }
     
     TraceLine(o, end, m_pMyEnt, CheckPlayers, &tr);
     
     return !tr.collided;
}

void CBot::LogBotMotion()
{
    if ( BotManager.m_enable_motion_logging ) {
        BotManager.m_motion_log_file << BotManager.GetBotIndex(m_pMyEnt);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->o.x*DMF);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->o.y*DMF);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->o.z*DMF);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->yaw*DAF);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->pitch*DAF);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->roll*DAF);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->vel.x*DVF);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->vel.y*DVF);
        BotManager.m_motion_log_file << (int)(m_pMyEnt->vel.z*DVF);
    }
}

// Bot class end

// Bot manager class begin

void CBotManager::Init()
{
     m_pBotToView = NULL;
     
     //if (!listenserv)
     //     return;

     m_bBotsShoot = true;
     m_bIdleBots = false;
     m_iFrameTime = 0;
     m_iPrevTime = lastmillis;
     m_sBotSkill = 1; // Default all bots have the skill 'Good'
     
     InitSkillData();
     LoadBotNamesFile();
     LoadBotTeamsFile();
     WaypointClass.Init();
     lsrand(time(NULL));
     
     m_enable_motion_logging = enable_motion_logging;
     if ( m_enable_motion_logging ) {
         m_motion_log_file.open(motion_log_filename.c_str());
         if ( !m_motion_log_file ) {
            using namespace NMSTL;
            WARN << "Failed to open motion trace " << motion_log_filename;
            m_enable_motion_logging = false;
         }
     }
}
     
void CBotManager::Think()
{    
     if (m_bInit)
     {
          Init();
          m_bInit = false;
     }

     if (m_pBotToView)
          ViewBot();

     //if (!listenserv)
     //     return;
     
     m_iFrameTime = lastmillis - m_iPrevTime;
     if (m_iFrameTime > 250) m_iFrameTime = 250;
     m_iPrevTime = lastmillis;

     // Added by yna
     // Let pending bots think so they can send their init tuples
     bool all_empty = true;
     loopv(pending_bots)
     {
         if (!pending_bots[i])
             continue;

         if (pending_bots[i]->pBot) {
             pending_bots[i]->pBot->SendBotInitTuple();
             all_empty = false;
         }
     }

     if (all_empty) pending_bots.setsize(0);
     // end yna
                                   
     // Let all bots 'think'
     loopv(bots)
     {
          if (!bots[i])
               continue;

          if (bots[i]->pBot) {
	       //printf("Bot %d thinking\n", i);
               bots[i]->pBot->Think();
	  }


          else
               condebug("Error: pBot == NULL in bot ent\n");
     }

}

void CBotManager::LoadBotNamesFile()
{
     //if (!listenserv)
     //      return;
          
     // Init bot names array first
     for (int i=0;i<100;i++)
          strcpy(m_szBotNames[i], "Bot");
     
     m_sBotNameCount = 0;
     
     // Load bot file
     char szNameFileName[256];
     MakeBotFileName("bot_names.txt", NULL, NULL, szNameFileName);
     FILE *fp = fopen(szNameFileName, "r");
     char szNameBuffer[256];
     int iIndex, iStrIndex;
     
     if (!fp)
     {
          conoutf("Warning: Couldn't load bot names file");
          return;
     }
     
     while (fgets(szNameBuffer, 80, fp) != NULL)
     {
          if (m_sBotNameCount >= 150)
          {
               conoutf("Warning: Max bot names reached(150), ignoring the rest of the"
                       "names");
               break;
          }
          
          short length = strlen(szNameBuffer);

          if (szNameBuffer[length-1] == '\n')
          {
               szNameBuffer[length-1] = 0;  // remove '\n'
               length--;
          }

          iStrIndex = 0;
          while (iStrIndex < length)
          {
               if ((szNameBuffer[iStrIndex] < ' ') || (szNameBuffer[iStrIndex] > '~') ||
                   (szNameBuffer[iStrIndex] == '"'))
               {
                    for (iIndex=iStrIndex; iIndex < length; iIndex++)
                         szNameBuffer[iIndex] = szNameBuffer[iIndex+1];
               }
 
               iStrIndex++;
          }

          if (szNameBuffer[0] != 0)
          {
               if (strlen(szNameBuffer) >= 16)
               {     conoutf("Warning: bot name \"%s\" has to many characters(16 is max)",
                            (int)szNameBuffer);
               }
               strn0cpy(m_szBotNames[m_sBotNameCount], szNameBuffer, 16);
               m_sBotNameCount++;
          }
     }
     fclose(fp);
}
     
char *CBotManager::GetBotName()
{
     char *szOutput = NULL;
     TMultiChoice<char *> BotNameChoices;
     short ChoiceVal;
     
     for(int j=0;j<m_sBotNameCount;j++)
     {
          ChoiceVal = 50;
          
          loopv(players)
          {
               if (players[i] && (!stricmp(players[i]->name, m_szBotNames[j])))
                    ChoiceVal -= 10;
          }
          
          loopv(bots)
          {
               if (bots[i] && (!stricmp(bots[i]->name, m_szBotNames[j])))
                    ChoiceVal -= 10;
          }
          
          if (!stricmp(player1->name, m_szBotNames[j]))
               ChoiceVal -= 10;
               
          if (ChoiceVal <= 0)
               ChoiceVal = 1;
          
          BotNameChoices.Insert(m_szBotNames[j], ChoiceVal);
     }
     
     // Couldn't find a selection?
     if (!BotNameChoices.GetSelection(szOutput))
          szOutput = "Bot";
     
     return szOutput;
}
     
void CBotManager::LoadBotTeamsFile()
{
     //if (!listenserv)
     //      return;
          
     // Init bot teams array first
     for (int i=0;i<20;i++)
          strcpy(m_szBotTeams[i], "b0ts");
     
     m_sBotTeamCount = 0;
     
     // Load bot file
     char szNameFileName[256];
     MakeBotFileName("bot_teams.txt", NULL, NULL, szNameFileName);
     FILE *fp = fopen(szNameFileName, "r");
     char szNameBuffer[256];
     int iIndex, iStrIndex;
     
     if (!fp)
     {
          conoutf("Warning: Couldn't load bot teams file");
          return;
     }
     
     while ((m_sBotTeamCount < 20) && (fgets(szNameBuffer, 80, fp) != NULL))
     {
          short length = strlen(szNameBuffer);

          if (szNameBuffer[length-1] == '\n')
          {
               szNameBuffer[length-1] = 0;  // remove '\n'
               length--;
          }

          iStrIndex = 0;
          while (iStrIndex < length)
          {
               if ((szNameBuffer[iStrIndex] < ' ') || (szNameBuffer[iStrIndex] > '~') ||
                   (szNameBuffer[iStrIndex] == '"'))
               {
                    for (iIndex=iStrIndex; iIndex < length; iIndex++)
                         szNameBuffer[iIndex] = szNameBuffer[iIndex+1];
               }
 
               iStrIndex++;
          }

          if (szNameBuffer[0] != 0)
          {
               strn0cpy(m_szBotTeams[m_sBotTeamCount], szNameBuffer, 5);
               m_sBotTeamCount++;
          }
     }
     fclose(fp);
}

char *CBotManager::GetBotTeam()
{
     char *szOutput = NULL;
     TMultiChoice<char *> BotTeamChoices;
     short ChoiceVal;
     
     for(int j=0;j<m_sBotTeamCount;j++)
     {
          ChoiceVal = 50;
          /* UNDONE?
          loopv(players)
          {
               if (players[i] && (!stricmp(players[i]->name, m_szBotNames[j])))
                    ChoiceVal -= 10;
          }
          
          loopv(bots)
          {
               if (bots[i] && (!stricmp(bots[i]->name, m_szBotNames[j])))
                    ChoiceVal -= 10;
          }
          
          if (!stricmp(player1->name, m_szBotNames[j]))
               ChoiceVal -= 10;
               
          if (ChoiceVal <= 0)
               ChoiceVmonsterclearal = 1;*/
          
          BotTeamChoices.Insert(m_szBotTeams[j], ChoiceVal);
     }
     
     // Couldn't find a selection?
     if (!BotTeamChoices.GetSelection(szOutput))
          szOutput = "b0t";
     
     return szOutput;
}

void CBotManager::RenderBots()
{
     static bool drawblue;
     
     loopv(bots)
     {
          drawblue = (m_sp || isteam(player1->team, bots[i]->team));
          if (bots[i] && (bots[i] != m_pBotToView)) 
              renderclient(bots[i], drawblue, "monster/ogro", false, 1.0f, selected_bot(i));
     }
}

void CBotManager::RemoveAllBots()
{
     loopv(bots)
     {
                //addmsg(1, 2, SV_BOTDIS, i);
                
          if (!bots[i])
               continue;
                        
          delete bots[i]->pBot;
                bots[i]->pBot = NULL;
                 zapdynent(bots[i]);
     }
        bots.setsize(0);
}

int CBotManager::GetBotIndex(dynent *m)
{
     loopv(bots)
     {
          if (!bots[i])
               continue;
     
          if (bots[i] == m)
               return i;
     }
     
     return -1;
}

int CBotManager::GetPendingBotIndex(dynent* pb)
{
    loopv(pending_bots)
    {
        if (!pending_bots[i])
            continue;

        if (pending_bots[i] == pb)
            return i;
    }

    return -1;
}

void CBotManager::LetBotsUpdateStats()
{
     loopv(bots) if (bots[i] && bots[i]->pBot) bots[i]->pBot->m_bSendC2SInit = false;
}

void CBotManager::LetBotsHear(int n, vec *loc)
{
     if (bots.length() == 0) return;
     
     if (!loc) return;
          
     loopv(bots)
     {
          if (!bots[i] || !bots[i]->pBot || (bots[i]->state == CS_DEAD)) continue;
          bots[i]->pBot->HearSound(n, loc);
     }
}

void CBotManager::MakeBotFileName(const char *szFileName, const char *szDir1, const char *szDir2, char *szOutput)
{
     char *DirSeperator;

#ifndef __linux__
     DirSeperator = "\\";
     strcpy(szOutput, "bot\\");
#else
     DirSeperator = "/";
     strcpy(szOutput, "bot/");
#endif
     
     if (szDir1)
     {
          strcat(szOutput, szDir1);
          strcat(szOutput, DirSeperator);
     }
     
     if (szDir2)
     {
          strcat(szOutput, szDir2);
          strcat(szOutput, DirSeperator);
     }
     
     strcat(szOutput, szFileName);
}

void CBotManager::InitSkillData()
{
     // Best skill
     m_BotSkills[0].flMinReactionDelay = 0.015f;
     m_BotSkills[0].flMaxReactionDelay = 0.035f;
     m_BotSkills[0].flMinAimXOffset = 15.0f;
     m_BotSkills[0].flMaxAimXOffset = 20.0f;
     m_BotSkills[0].flMinAimYOffset = 10.0f;
     m_BotSkills[0].flMaxAimYOffset = 15.0f;
     m_BotSkills[0].flMinAimXSpeed = 330.0f;
     m_BotSkills[0].flMaxAimXSpeed = 355.0f;
     m_BotSkills[0].flMinAimYSpeed = 400.0f;
     m_BotSkills[0].flMaxAimYSpeed = 450.0f;
     m_BotSkills[0].flMinAttackDelay = 0.1f;
     m_BotSkills[0].flMaxAttackDelay = 0.4f;
     m_BotSkills[0].flMinEnemySearchDelay = 0.09f;
     m_BotSkills[0].flMaxEnemySearchDelay = 0.12f;
     m_BotSkills[0].sShootAtFeetWithRLPercent = 85;
     m_BotSkills[0].bCanPredict = true;
     m_BotSkills[0].iMaxHearVolume = 75;
     m_BotSkills[0].bCircleStrafe = true;
     m_BotSkills[0].bCanSearchItemsInCombat = true;

     // Good skill
     m_BotSkills[1].flMinReactionDelay = 0.035f;
     m_BotSkills[1].flMaxReactionDelay = 0.045f;
     m_BotSkills[1].flMinAimXOffset = 20.0f;
     m_BotSkills[1].flMaxAimXOffset = 25.0f;
     m_BotSkills[1].flMinAimYOffset = 15.0f;
     m_BotSkills[1].flMaxAimYOffset = 20.0f;
     m_BotSkills[1].flMinAimXSpeed = 250.0f;
     m_BotSkills[1].flMaxAimXSpeed = 265.0f;
     m_BotSkills[1].flMinAimYSpeed = 260.0f;
     m_BotSkills[1].flMaxAimYSpeed = 285.0f;
     m_BotSkills[1].flMinAttackDelay = 0.3f;
     m_BotSkills[1].flMaxAttackDelay = 0.6f;
     m_BotSkills[1].flMinEnemySearchDelay = 0.12f;
     m_BotSkills[1].flMaxEnemySearchDelay = 0.17f;
     m_BotSkills[1].sShootAtFeetWithRLPercent = 60;
     m_BotSkills[1].bCanPredict = true;
     m_BotSkills[1].iMaxHearVolume = 60;
     m_BotSkills[1].bCircleStrafe = true;
     m_BotSkills[1].bCanSearchItemsInCombat = true;

     // Medium skill
     m_BotSkills[2].flMinReactionDelay = 0.075f;
     m_BotSkills[2].flMaxReactionDelay = 0.010f;
     m_BotSkills[2].flMinAimXOffset = 25.0f;
     m_BotSkills[2].flMaxAimXOffset = 30.0f;
     m_BotSkills[2].flMinAimYOffset = 20.0f;
     m_BotSkills[2].flMaxAimYOffset = 25.0f;
     m_BotSkills[2].flMinAimXSpeed = 190.0f;
     m_BotSkills[2].flMaxAimXSpeed = 125.0f;
     m_BotSkills[2].flMinAimYSpeed = 210.0f;
     m_BotSkills[2].flMaxAimYSpeed = 240.0f;
     m_BotSkills[2].flMinAttackDelay = 0.75f;
     m_BotSkills[2].flMaxAttackDelay = 1.0f;
     m_BotSkills[2].flMinEnemySearchDelay = 0.18f;
     m_BotSkills[2].flMaxEnemySearchDelay = 0.22f;
     m_BotSkills[2].sShootAtFeetWithRLPercent = 25;
     m_BotSkills[2].bCanPredict = false;
     m_BotSkills[2].iMaxHearVolume = 45;
     m_BotSkills[2].bCircleStrafe = true;
     m_BotSkills[2].bCanSearchItemsInCombat = false;

     // Worse skill
     m_BotSkills[3].flMinReactionDelay = 0.15f;
     m_BotSkills[3].flMaxReactionDelay = 0.20f;
     m_BotSkills[3].flMinAimXOffset = 30.0f;
     m_BotSkills[3].flMaxAimXOffset = 35.0f;
     m_BotSkills[3].flMinAimYOffset = 25.0f;
     m_BotSkills[3].flMaxAimYOffset = 30.0f;
     m_BotSkills[3].flMinAimXSpeed = 155.0f;
     m_BotSkills[3].flMaxAimXSpeed = 170.0f;
     m_BotSkills[3].flMinAimYSpeed = 160.0f;
     m_BotSkills[3].flMaxAimYSpeed = 210.0f;
     m_BotSkills[3].flMinAttackDelay = 1.2f;
     m_BotSkills[3].flMaxAttackDelay = 1.6f;
     m_BotSkills[3].flMinEnemySearchDelay = 0.25f;
     m_BotSkills[3].flMaxEnemySearchDelay = 0.30f;
     m_BotSkills[3].sShootAtFeetWithRLPercent = 10;
     m_BotSkills[3].bCanPredict = false;
     m_BotSkills[3].iMaxHearVolume = 30;
     m_BotSkills[3].bCircleStrafe = false;
     m_BotSkills[3].bCanSearchItemsInCombat = false;

     // Bad skill
     m_BotSkills[4].flMinReactionDelay = 0.30f;
     m_BotSkills[4].flMaxReactionDelay = 0.50f;
     m_BotSkills[4].flMinAimXOffset = 35.0f;
     m_BotSkills[4].flMaxAimXOffset = 40.0f;
     m_BotSkills[4].flMinAimYOffset = 30.0f;
     m_BotSkills[4].flMaxAimYOffset = 35.0f;
     m_BotSkills[4].flMinAimXSpeed = 45.0f;
     m_BotSkills[4].flMaxAimXSpeed = 60.0f;
     m_BotSkills[4].flMinAimYSpeed = 125.0f;
     m_BotSkills[4].flMaxAimYSpeed = 180.0f;
     m_BotSkills[4].flMinAttackDelay = 1.5f;
     m_BotSkills[4].flMaxAttackDelay = 2.0f;
     m_BotSkills[4].flMinEnemySearchDelay = 0.30f;
     m_BotSkills[4].flMaxEnemySearchDelay = 0.36f;
     m_BotSkills[4].sShootAtFeetWithRLPercent = 0;
     m_BotSkills[4].bCanPredict = false;
     m_BotSkills[4].iMaxHearVolume = 15;
     m_BotSkills[4].bCircleStrafe = false;
     m_BotSkills[4].bCanSearchItemsInCombat = false;
}

void CBotManager::ChangeBotSkill(short Skill, dynent *bot)
{
     static char *SkillNames[5] = { "best", "good", "medium", "worse", "bad" };
     
     if (bot && bot->pBot)
     {
          // Only change skill of a single bot
          bot->pBot->m_pBotSkill = &m_BotSkills[Skill];
          bot->pBot->m_sSkillNr = Skill;
          conoutf("Skill of %s is now %s", (int)bot->name, (int)SkillNames[Skill]);
          return;
     }
     
     // Change skill of all bots
     loopv(bots)
     {
          if (!bots[i] || !bots[i]->pBot) continue;
          
          bots[i]->pBot->m_pBotSkill = &m_BotSkills[Skill];
          bots[i]->pBot->m_sSkillNr = Skill;
     }
     
     // Change default bot skill
     m_sBotSkill = Skill;
     
     conoutf("Skill of all bots is now %s", (int)SkillNames[Skill]);
}

void CBotManager::ViewBot()
{   
     // Check if this bot is still in game
     bool bFound = false;
     loopv(bots)
     {
          if (bots[i] == m_pBotToView)
          {
               bFound = true;
               break;
          }
     }
     
     if (!bFound)
     {
          DisableBotView();
          return;
     }
     
     player1->state = CS_DEAD; // Fake dead
     
     player1->o = m_pBotToView->o;
     player1->o.z += 1.0f;
     player1->yaw = m_pBotToView->yaw;
     player1->pitch = m_pBotToView->pitch;
     player1->roll = m_pBotToView->roll;
     player1->radius = 0; // Don't collide
}

void CBotManager::DisableBotView()
{
     m_pBotToView = NULL;
     respawnself();
     player1->radius = 1.1f;
}
     
// Bot manager class end

void addbot(char *arg1, char *arg2, char *arg3)
{
     // UNDONE: Needs to be decided who can and who can't add bots(voting?)
     //if (listenserv)
     //{

    conoutf("Creating bot...\n");

    dynent *b = CBot::CreateBot(arg1, arg2, arg3);
    if (b)
        conoutf("connected: %s", (int)b->name);

    // Increase amount of monsters in dmsp mode...:)
    if (m_dmsp)
    {
        monstercountgrow();

    }

     //}
     //else
     //          conoutf("Only the host can create bots");
}

COMMAND(addbot, ARG_3STR);

void botsshoot(int Shoot)
{
     //if (!listenserv) return;
     if (Shoot)
     {
          BotManager.SetBotsShoot(true);
          conoutf("Bots will shoot");
     }
     else
     {
          BotManager.SetBotsShoot(false);
          conoutf("Bots won't shoot");
     }
}

COMMAND(botsshoot, ARG_1INT);
          
void idlebots(int Idle)
{
     //if (!listenserv) return;
     if (Idle)
     {
          BotManager.SetIdleBots(true);
          conoutf("Bots are idle");
     }
     else
     {
          BotManager.SetIdleBots(false);
          conoutf("Bots aren't idle");
     }
}

COMMAND(idlebots, ARG_1INT);

void drawbeamtobots()
{
     loopv(bots)
     {
          if (bots[i])
               particle_trail(1, 500, player1->o, bots[i]->o);
     }
}

COMMAND(drawbeamtobots, ARG_NONE);

void kickbot(const char *szName)
{
     //if (!listenserv || !szName || !(*szName))
     //      return;
          
     int iBotInd = -1;
     loopv(bots)
     {
          if (!bots[i]) continue;
          
          if (!strcmp(bots[i]->name, szName))
          {
               iBotInd = i;
               break;
          }
     }
     
     if (iBotInd != -1)
     {
          //addmsg(1, 2, SV_BOTDIS, iBotInd);
	  NMSTL::ptr<cube_disconnect_tuple> cdt = NMSTL::ptr<cube_disconnect_tuple>(new cube_disconnect_tuple());
	  cdt->clientnum = iBotInd;
	  cdt->isbot = 1;

          // Note change to SV_CDIS
	  client_pending_message.push_back(std::make_pair((int)SV_CDIS, cdt.dynamic_cast_to<borealis_tuple>()));
          
          conoutf("bot %s disconnected", (int)(bots[iBotInd]->name[0] ? bots[iBotInd]->name : "[incompatible client]"));
          delete bots[iBotInd]->pBot;
                 zapdynent(bots[iBotInd]);
          bots.remove(iBotInd);
     }
}

COMMAND(kickbot, ARG_1STR);

void kickallbots(void)
{
     //if (!listenserv) return;
     
     loopv(bots)
     {
          if (bots[i] && bots[i]->pBot)
          {
	      // change to SV_CDIS
	      //addmsg(1, 2, SV_BOTDIS, i);
	      NMSTL::ptr<cube_disconnect_tuple> cdt = NMSTL::ptr<cube_disconnect_tuple>(new cube_disconnect_tuple());
	      cdt->clientnum = i;
	      cdt->isbot = 1;

	      // Note change to SV_CDIS
	      client_pending_message.push_back(std::make_pair((int)SV_CDIS, cdt.dynamic_cast_to<borealis_tuple>()));
          
	      conoutf("bot %s disconnected",(int)(bots[i]->name[0] ?
						  bots[i]->name : "[incompatible client]"));
	      delete bots[i]->pBot;
	      zapdynent(bots[i]);
          }
     }
     
     bots.setsize(0);
}

COMMAND(kickallbots, ARG_NONE);

void togglegrap()
{
     if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GrabMode(0))
          SDL_WM_GrabInput(SDL_GRAB_ON);
     else
          SDL_WM_GrabInput(SDL_GrabMode(0));
}

COMMAND(togglegrap, ARG_NONE);

void togglebotview(char *bot)
{         
     if (BotManager.m_pBotToView)
          BotManager.DisableBotView();
     else if (bot && *bot)
     {
          loopv(bots)
          {
               if (!bots[i]) continue;
               
               if (!strcmp(bots[i]->name, bot))
               {
                    BotManager.EnableBotView(bots[i]);
                    break;
               }
          }
     }
}

COMMAND(togglebotview, ARG_1STR);

void botskill(char *bot, char *skill)
{
     //if (!skill || !(*skill) || !listenserv)
     //      return;
     
     short SkillNr;
     
     if (!stricmp(skill, "best"))
          SkillNr = 0;
     else if (!stricmp(skill, "good"))
          SkillNr = 1;
     else if (!stricmp(skill, "medium"))
          SkillNr = 2;
     else if (!stricmp(skill, "worse"))
          SkillNr = 3;
     else if (!stricmp(skill, "bad"))
          SkillNr = 4;
     else
     {
          conoutf("Wrong skill specified. Should be best, good, medium, worse or bad");
          return;
     }
     
     if (bot)
     {
          loopv(bots)
          {
               if (bots[i] && !strcmp(bots[i]->name, bot))
               {
                    BotManager.ChangeBotSkill(SkillNr, bots[i]);
                    break;
               }
          }
     }
     else
          BotManager.ChangeBotSkill(SkillNr);
}

COMMAND(botskill, ARG_2STR);

void botskillall(char *skill)
{
     botskill(NULL, skill);
}

COMMAND(botskillall, ARG_1STR);

#ifndef RELEASE_BUILD

void telebot(void)
{
     vec dest = player1->o, forward, right, up;
     vec angles = { player1->pitch, player1->yaw, player1->roll };
     traceresult_s tr;
     
     AnglesToVectors(angles, forward, right, up);
     vmul(forward, 4.0f);
     vadd(dest, forward);
     
     TraceLine(player1->o, dest, player1, true, &tr);
     
     if (!tr.collided)
     {
          // Get the first bot
          loopv(bots)
          {
               if (!bots[i] || !bots[i]->pBot) continue;
               bots[i]->o = tr.end;
               break;
          }
     }
}

COMMAND(telebot, ARG_NONE);

void testvisible(int Dir)
{
     static vec angles, end, forward, right, up;
     static traceresult_s tr;
     
     float flDist = 20.0f;
        
     end = player1->o;
     makevec(&angles, 0.0f, player1->yaw, player1->roll);
     
     EDirection eDir = EDirection(Dir);
     
     switch(eDir)
     {
     case FORWARD:
          AnglesToVectors(angles, forward, right, up);
          vmul(forward, flDist);
          vadd(end, forward);
          break;
     case FORWARD_LEFT:
          angles.y = WrapYZAngle(angles.y - 45.0f);
          AnglesToVectors(angles, forward, right, up);
          vmul(forward, flDist);
          vadd(end, forward);
          break;
     case FORWARD_RIGHT:
          angles.y = WrapYZAngle(angles.y + 45.0f);
          AnglesToVectors(angles, forward, right, up);    
          vmul(forward, flDist);
          vadd(end, forward);
          break;
     case BACKWARD:
          AnglesToVectors(angles, forward, right, up);
          vmul(forward, flDist);
          vsub(end, forward);
          break;
     case BACKWARD_LEFT:
          angles.y = WrapYZAngle(angles.y + 45.0f);
          AnglesToVectors(angles, forward, right, up);    
          vmul(forward, flDist);
          vsub(end, forward);
          break;
     case BACKWARD_RIGHT:
          angles.y = WrapYZAngle(angles.y - 45.0f);
          AnglesToVectors(angles, forward, right, up);     
          vmul(forward, flDist);
          vsub(end, forward);
          break;
     case LEFT:
          AnglesToVectors(angles, forward, right, up);
          vmul(right, flDist);
          vsub(end, right);
          break;
     case RIGHT:
          AnglesToVectors(angles, forward, right, up);
          vmul(right, flDist);
          vadd(end, right);
          break;          
     case UP:
          AnglesToVectors(angles, forward, right, up);
          vmul(up, flDist);
          vadd(end, up);
          break;          
     case DOWN:
          AnglesToVectors(angles, forward, right, up);
          vmul(up, flDist);
          vsub(end, up);     
     }
     
     TraceLine(player1->o, end, player1, true, &tr);
     
     debugbeam(player1->o, tr.end);
     char sz[250];
     sprintf(sz, "dist: %f; hit: %d", GetDistance(player1->o, tr.end), tr.collided);
     condebug(sz);
}

COMMAND(testvisible, ARG_1INT);
               
#endif
