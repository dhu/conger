//
// C++ Implementation: %{MODULE}
//
// Description: Waypoint class for bots
//
//
// Author: Rick <rickhelmus@gmail.com>, (C) 2004
//

#include "cube.h"

extern float sheight(sqr *s, sqr *t, float z);

vec v_debuggoal = g_vecZero;

CWaypointClass WaypointClass;

// Waypoint class begin

CWaypointClass::CWaypointClass(void) : m_bDrawWaypoints(false), m_bDrawWPPaths(true),
                                       m_bAutoWaypoint(false),  m_bAutoPlacePaths(true), m_bDrawWPText(false),
                                       m_vLastCreatedWP(g_vecZero), m_iWaypointCount(0)
{
     int i;

     for (i=0; i < MAX_WAYPOINTS; i++)
     {
          m_Waypoints[i].ConnectedWPs.DeleteAllNodes();
     }
}

void CWaypointClass::Init()
{
     int i;

     Clear();  // Free previously allocated path memory

     for (i=0; i < MAX_WAYPOINTS; i++)
     {
          m_Waypoints[i].iFlags = 0;
          m_Waypoints[i].v_origin = g_vecZero;
          m_Waypoints[i].ConnectedWPs.DeleteAllNodes();
     }

     m_fPathDrawTime = 0.0;
     m_iWaypointCount = 0;
     m_vLastCreatedWP = g_vecZero;

#ifdef WP_FLOOD
     m_bFlooding = false;
#endif
}

void CWaypointClass::Clear()
{
     int i;

     for (i=0; i < MAX_WAYPOINTS; i++)
     {
          m_Waypoints[i].ConnectedWPs.DeleteAllNodes();
     }
              
#ifdef WP_FLOOD
     m_FloodWPsToCheck.DeleteAllNodes();

     for(int i=0;i<MAX_MAP_GRIDS;i++)
     {
          for(int j=0;j<MAX_MAP_GRIDS;j++)
          {
               while(!m_FloodWPs[i][j].Empty())
               {
                    waypoint_s *p = m_FloodWPs[i][j].Pop();
                    p->ConnectedWPs.DeleteAllNodes();
                    delete p;
               }
          }
     }     
#endif
}

// returns true if waypoints succesfull loaded
bool CWaypointClass::LoadWaypoints(const char *szMapName)
{
     FILE *bfp;
     char szWPFileName[64];
     char filename[256];
     waypoint_header_s header;
     int index, i;
     short int num;
     int path_index;

     strcpy(szWPFileName, szMapName);
     strcat(szWPFileName, ".wpt");

     BotManager.MakeBotFileName(szWPFileName, "waypoints", NULL, filename);

     bfp = fopen(filename, "rb");

     // if file exists, read the waypoint structure from it
     if (bfp != NULL)
     {
          fread(&header, sizeof(header), 1, bfp);
          
          header.szFileType[10] = 0;
          if (strcmp(header.szFileType, "cube_bot") == 0)
          {
               if (header.iWPFileVersion != WAYPOINT_VERSION)
               {
                    conoutf("Wrong waypoint version for cube bot.");
                    fclose(bfp);
                    return false;
               }

               header.szMapName[31] = 0;

               if (strcmp(header.szMapName, szMapName) == 0)
               {
                    Init();  // remove any existing waypoints
                   
                    for (i=0; i < header.iWPCount; i++)
                    {
                         fread(&m_Waypoints[i], sizeof(m_Waypoints[0]), 1, bfp);
                         m_Waypoints[i].ConnectedWPs.Reset();
                         m_iWaypointCount++;
                    }
                    
                    // read and add waypoint paths...
                    for (index=0; index < m_iWaypointCount; index++)
                    {
                         // read the number of paths from this node...
                         fread(&num, sizeof(num), 1, bfp);

                         for (i=0; i < num; i++)
                         {
                              fread(&path_index, sizeof(path_index), 1, bfp);
                              AddPath(&m_Waypoints[index], &m_Waypoints[path_index]);
                         }
                    }                    
               }
               else
               {
                    conoutf("Waypoints aren't for map %s but for map %s", int(szMapName),
                            int(header.szMapName));
                    fclose(bfp);
                    return false;
               }
          }
          else
          {
               conoutf("Waypoint file isn't for cube bot");
               //conoutf("Header FileType: %s", int(header.szFileType));

               fclose(bfp);
               return false;
          }

          fclose(bfp);

          //RouteInit();
     }
     else
     {
          conoutf("Waypoint file %s does not exist", int(filename));
          return false;
     }

     conoutf("Waypoints for map %s loaded", int(szMapName));
     return true;
}

void CWaypointClass::SaveWaypoints()
{
     char filename[256];
     char mapname[64];
     waypoint_header_s header;
     int index, i;
     short int num;
     TLinkedList<waypoint_s *>::node_s *pPath;

     strcpy(header.szFileType, "cube_bot");

     header.iWPFileVersion = WAYPOINT_VERSION;

     header.iWPFileFlags = 0;  // not currently used

     header.iWPCount = m_iWaypointCount;

     memset(header.szMapName, 0, sizeof(header.szMapName));
     strncpy(header.szMapName, getclientmap(), 31);
     header.szMapName[31] = 0;

     strcpy(mapname, getclientmap());
     strcat(mapname, ".wpt");

     BotManager.MakeBotFileName(mapname, "waypoints", NULL, filename);

     FILE *bfp = fopen(filename, "wb");

     // write the waypoint header to the file...
     fwrite(&header, sizeof(header), 1, bfp);
         
     // write the waypoint data to the file...
     for (index=0; index < m_iWaypointCount; index++)
     {
          fwrite(&m_Waypoints[index], sizeof(m_Waypoints[0]), 1, bfp);
     }
         
     // save the waypoint paths...
     for (index=0; index < m_iWaypointCount; index++)
     {
          // count the number of paths from this node...

          pPath = m_Waypoints[index].ConnectedWPs.GetFirst();
          num = m_Waypoints[index].ConnectedWPs.NodeCount();

          fwrite(&num, sizeof(num), 1, bfp);  // write the count

          // now write out each path index...          
          while (pPath != NULL)
          {
               for (i=0;i<m_iWaypointCount;i++)
               {
                    if (&m_Waypoints[i] == pPath->Entry)
                    {
                         fwrite(&i, sizeof(i), 1, bfp);
                         break;
                    }
               }
               pPath = pPath->next;  // go to next node in linked list
          }
     }
     
     fclose(bfp);
}

void CWaypointClass::Think()
{
#ifdef WP_FLOOD
     //FloodThink();
#endif

     float flDistance, flMinDist;
     vec start, end;
     int i, index = 0;
     
     /*
     loopv(ents)
     {
          entity &e = ents[i];
          if(e.type!=MAPMODEL) continue; // Only check map models for now
               
          // UNDONE: Not very accurate(doesn take ent radius in account)
          vec v;
          makevec(&v, e.x, e.y, e.z);
          {
                    mapmodelinfo &mmi = getmminfo(e.attr2);
                    if(!&mmi || !mmi.h) continue;
                    const float r = mmi.rad;//+(pTracer!=NULL) ? pTracer->radius : 0;
                    { 
                         float hi = (float)(S(e.x, e.y)->floor+mmi.zoff+e.attr3);
                         float lo = hi +mmi.h;
                         linestyle(2.5f, 0xFF, 0xFF, 0x00);
                         line(int(player1->o.x), int(player1->o.y), int(player1->o.z),
                                int(e.x-r), int(e.y-r), int(hi));
                         line(int(player1->o.x), int(player1->o.y), int(player1->o.z),
                                int(e.x+r), int(e.y-r), int(hi));
                         line(int(player1->o.x), int(player1->o.y), int(player1->o.z),
                                int(e.x-r), int(e.y+r), int(hi));
                         line(int(player1->o.x), int(player1->o.y), int(player1->o.z),
                                int(e.x+r), int(e.y+r), int(hi));
                                                         
                         //linestyle(2.5f, 76, 25, 0x00);
                         //line(int(player1->o.x), int(player1->o.y), int(player1->o.z),
                           //     int(e.x), int(e.y), int(lo));                                
                    }
          }
     }*/
              
     if (m_bAutoWaypoint) // is auto waypoint on?
     {
          // find the distance from the last used waypoint
          flDistance = GetDistance(m_vLastCreatedWP, player1->o);

          bool NoLastCreatedWP = ((m_vLastCreatedWP.x == 0) && (m_vLastCreatedWP.y == 0) &&
                                                    (m_vLastCreatedWP.z == 0));
          
          if ((flDistance > 3.0f) || NoLastCreatedWP)
          {
               flMinDist = 9999.0;

               // check that no other reachable waypoints are nearby...
               for (i=0; i < m_iWaypointCount; i++)
               {
                    if (m_Waypoints[i].iFlags & W_FL_DELETED)
                         continue;

                    if (WPIsReachable(player1->o, i))
                    {
                         flDistance = GetDistance(player1->o, m_Waypoints[i].v_origin);

                         if (flDistance < flMinDist)
                              flMinDist = flDistance;
                    }
               }

               // make sure nearest waypoint is far enough away...
               if (flMinDist >= 10.0f)
               {
                    AddWaypoint(player1->o, true);  // place a waypoint here
               }
          }
     }

     flMinDist = 9999.0;

     // Draw info for nearest waypoint?
     if (m_bDrawWPText)
     {
          waypoint_s *nearestwp = WaypointClass.GetNearestWaypoint(player1, 20.0f);
          if (nearestwp)
          {
               char szWPInfo[256];
               sprintf(szWPInfo, "Distance nearest waypoint: %f", GetDistance(player1->o, nearestwp->v_origin));
               AddDebugText(szWPInfo);
                              
               strcpy(szWPInfo, "Flags: ");
               if (nearestwp->iFlags & W_FL_TELEPORT)
                    strcat(szWPInfo, "Teleport ");
               if (nearestwp->iFlags & W_FL_TELEPORTDEST)
                    strcat(szWPInfo, "Teleport destination ");
               if (nearestwp->iFlags & W_FL_JUMP)
                    strcat(szWPInfo, "Jump ");
               
               if (strlen(szWPInfo) == 7)
                    strcat(szWPInfo, "None");
                    
               AddDebugText(szWPInfo);
               
               sprintf(szWPInfo, "Waypoint has %d connections", nearestwp->ConnectedWPs.NodeCount());
               AddDebugText(szWPInfo);
          }
     }
  
     if (m_bDrawWaypoints)  // display the waypoints if turned on...
     {
#ifdef WP_FLOOD
          DrawNearFloodWPs();
#endif
          
          waypoint_s *nearestwp = WaypointClass.GetNearestWaypoint(player1, 20.0f);
                    
          for (i=0; i < m_iWaypointCount; i++)
          {
               if ((m_Waypoints[i].iFlags & W_FL_DELETED) == W_FL_DELETED)
                    continue;

               flDistance = GetDistance(player1->o, m_Waypoints[i].v_origin);

               if (flDistance < 20.0f)
               {
                    if (flDistance < flMinDist)
                    {
                         index = i; // store index of nearest waypoint
                         flMinDist = flDistance;
                    }
                    
                    start = m_Waypoints[i].v_origin;
                    start.z -= 2;
                    end = start;
                    end.z += 3;

                    if (m_Waypoints[i].iFlags & W_FL_JUMP)
                    {
                         // draw a red waypoint
                         linestyle(2.5f, 0xFF, 0x40, 0x40);
                    }                    
                    else if (nearestwp == &m_Waypoints[i])
                    {
                         // draw a green waypoint
                         linestyle(2.5f, 0x40, 0xFF, 0x40);
                    }
                    else
                    {
                         // draw a blue waypoint
                         linestyle(2.5f, 0x40, 0x40, 0xFF);
                    }
                    
                    line(int(start.x), int(start.y), int(start.z), int(end.x), int(end.y), int(end.z));
               }
          }
          
          // check if path waypointing is on...
          if (m_bDrawWPPaths)
          {
               // Draw path from first bot
               if (bots.length() && bots[0] && bots[0]->pBot && bots[0]->pBot->m_pCurrentWaypoint &&
                   bots[0]->pBot->m_pCurrentGoalWaypoint)
               {
                    CBot *pB = bots[0]->pBot;
                    if (!pB->m_bCalculatingAStarPath && !pB->m_AStarNodeList.Empty())
                    {
                         TLinkedList<waypoint_s *>::node_s *pNode = pB->m_AStarNodeList.GetFirst(), *pNext;
                         
                         linestyle(2.5f, 0xFF, 0x40, 0x40);
                         
                         line((int)pB->m_pCurrentWaypoint->v_origin.x, (int)pB->m_pCurrentWaypoint->v_origin.y,
                                (int)pB->m_pCurrentWaypoint->v_origin.z, (int)pNode->Entry->v_origin.x,
                                (int)pNode->Entry->v_origin.y,  (int)pNode->Entry->v_origin.z);
                                
                         while(pNode && pNode->next)
                         {
                              pNext = pNode->next;
                              vec &v1 = pNode->Entry->v_origin;
                              vec &v2 = pNext->Entry->v_origin;
                              
                              line(int(v1.x), int(v1.y), int(v1.z), int(v2.x), int(v2.y), int(v2.z));
                                   
                              pNode = pNode->next;
                         }
                    }
               }
                    
               // check if player is close enough to a waypoint and time to draw path...
               if (flMinDist <= 7)
               {
                    TLinkedList<waypoint_s *>::node_s *pNode = nearestwp->ConnectedWPs.GetFirst();
                    linestyle(2.0f, 0xFF, 0xFF, 0xFF);
                    while(pNode)
                    {
                         vec o = pNode->Entry->v_origin;
                         vec e = nearestwp->v_origin;
               
                         line(int(o.x), int(o.y), int(o.z), int(e.x), int(e.y), int(e.z));
               
                         pNode = pNode->next;
                    }
               }
          }
     }

     //WaypointFlooder.DrawNearFloodWPs();
}

// Add waypoint at location o, returns index of created wp
waypoint_s *CWaypointClass::AddWaypoint(vec o, bool connectwp)
{
     int index = 0;

     if (m_iWaypointCount >= MAX_WAYPOINTS)
     {
          conoutf("Cannont create waypoints, max reached");
          return NULL;
     }

     // find the next available slot for the new waypoint...
     while (index < m_iWaypointCount)
     {
          if (m_Waypoints[index].iFlags & W_FL_DELETED)
               break;

          index++;
     }

     m_Waypoints[index].iFlags = 0;
     m_Waypoints[index].v_origin = o;
     m_vLastCreatedWP = o;

     // increment total number of waypoints if adding at end of array...
     if (index == m_iWaypointCount)
          m_iWaypointCount++;
     
     if (connectwp)
     {
          // calculate all the paths to this new waypoint
          for (int i=0; i < m_iWaypointCount; i++)
          {
               if (i == index)
                    continue;  // skip the waypoint that was just added

               // check if the waypoint is reachable from the new one (one-way)
               if (m_bAutoPlacePaths)
               {
                    if (WPIsReachable(o, i))
                         AddPath(&m_Waypoints[index], &m_Waypoints[i]); // Add a path from a to b
                    if (WPIsReachable(m_Waypoints[i].v_origin, index))
                         AddPath(&m_Waypoints[i], &m_Waypoints[index]); // Add a path from b to a
               }
          }
     }
     
     return &m_Waypoints[index];
}

void CWaypointClass::DeleteWaypoint(dynent *d)
{
     waypoint_s *pWP;

     if (m_iWaypointCount < 1)
          return;

     pWP = GetNearestWaypoint(d, 7.0f);

     if (!pWP)
     {
          conoutf("Error: Couldn´t find near waypoint");
          return;
     }
     
     // delete any paths that lead to this index...
     DeletePath(pWP);
     
     pWP->iFlags = W_FL_DELETED;  // not being used
     pWP->v_origin = g_vecZero;
     pWP->ConnectedWPs.DeleteAllNodes();
}

void CWaypointClass::AddPath(waypoint_s *pWP1, waypoint_s *pWP2)
{
     pWP1->ConnectedWPs.AddNode(pWP2);
}

// Deletes all paths connected to the given waypoint
void CWaypointClass::DeletePath(waypoint_s *pWP)
{
     int index;

     for (index=0; index < m_iWaypointCount; index++)
     {
          m_Waypoints[index].ConnectedWPs.DeleteEntry(pWP);
     }
}

// Deletes path between 2 waypoints(1 way)
void CWaypointClass::DeletePath(waypoint_s *pWP1, waypoint_s *pWP2)
{
     pWP1->ConnectedWPs.DeleteEntry(pWP2);
}

void CWaypointClass::ManuallyCreatePath(dynent *d, int iCmd, bool TwoWay)
{
     static waypoint_s *waypoint1 = NULL;  // initialized to unassigned
     static waypoint_s *waypoint2 = NULL;  // initialized to unassigned

     if (iCmd == 1)  // assign source of path
     {
          waypoint1 = GetNearestWaypoint(d, 7.0f);

          if (!waypoint1)
          {
               conoutf("Error: Couldn't find near waypoint");
               return;
          }

          return;
     }

     if (iCmd == 2)  // assign dest of path and make path
     {
          if (!waypoint1)
          {
               conoutf("Error: First waypoint unset");
               return;
          }
          
          waypoint2 = GetNearestWaypoint(d, 7.0f);

          if (!waypoint2)
          {
               conoutf("Error: Couldn't find near waypoint");
               return;
          }

          AddPath(waypoint1, waypoint2);

          if (TwoWay)
               AddPath(waypoint2, waypoint1);
     }
}

void CWaypointClass::ManuallyDeletePath(dynent *d, int iCmd, bool TwoWay)
{
     static waypoint_s *waypoint1 = NULL;  // initialized to unassigned
     static waypoint_s *waypoint2 = NULL;  // initialized to unassigned

     if (iCmd == 1)  // assign source of path
     {
          waypoint1 = GetNearestWaypoint(d, 7.0f);

          if (!waypoint1)
          {
               conoutf("Error: Couldn't find near waypoint");
               return;
          }

          return;
     }

     if (iCmd == 2)  // assign dest of path and delete path
     {
          if (!waypoint1)
          {
               conoutf("Error: First waypoint unset");
               return;
          }
          
          waypoint2 = GetNearestWaypoint(d, 7.0f);

          if (!waypoint2)
          {
               conoutf("Error: Couldn't find near waypoint");
               return;
          }

          DeletePath(waypoint1, waypoint2);

          if (TwoWay)
               DeletePath(waypoint2, waypoint1);
     }
}

bool CWaypointClass::WPIsReachable(vec o, short sWaypointIndex)
{
     vec from = o, to = m_Waypoints[sWaypointIndex].v_origin;
     traceresult_s tr;
     float curr_height, last_height;

     float distance = GetDistance(from, to);

     // is the destination close enough?
     if (distance < REACHABLE_RANGE)
     {
          if (IsVisible(from, to))
          {                              
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
                         conoutf("to is in midair");
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
                    if ((last_height - curr_height) >= JUMP_HEIGHT)
                    {
                         // can't get there from here...
                         //conoutf("traces failed to to");
                         return false;
                    }

                    last_height = curr_height;

                    distance = GetDistance(to, v_check);  // distance from goal
               }

               return true;
          }
     }

     return false;
/*
     // UNDONE?
     return ((GetDistance(o, m_Waypoints[sWaypointIndex].v_origin) <= 20.0f) &&
             IsVisible(o, m_Waypoints[sWaypointIndex].v_origin, NULL));*/
}

// Returns nearest waypoint from d
waypoint_s *CWaypointClass::GetNearestWaypoint(dynent *d, float Range)
{
     int i;
     waypoint_s *pNearestWP = NULL;
     float distance;
     float min_distance = 9999.0f;

     if (m_iWaypointCount < 1)
          return NULL;

     for (i=0; i < m_iWaypointCount; i++)
     {
          if (m_Waypoints[i].iFlags & W_FL_DELETED)
               continue;  // skip any deleted waypoints

          distance = GetDistance(m_Waypoints[i].v_origin, d->o);

          if ((distance < min_distance) && (distance <= Range))
          {
               if (IsVisible(m_Waypoints[i].v_origin, d->o, d))
               {
                    pNearestWP = &m_Waypoints[i];
                    min_distance = distance;
               }
          }
     }

     return pNearestWP;
}

waypoint_s *CWaypointClass::GetNearestWaypoint(vec v_src, dynent *d, float range)
{
     int index;
     waypoint_s *pNearestWP = NULL;
     float distance;
     float min_distance = 9999.0f;

     if (m_iWaypointCount < 1)
          return NULL;

     for (index=0; index < m_iWaypointCount; index++)
     {
          if (m_Waypoints[index].iFlags & W_FL_DELETED)
               continue;  // skip any deleted waypoints

          distance = GetDistance(m_Waypoints[index].v_origin, d->o);

          if ((distance < min_distance) && (distance <= range))
          {
               if (IsVisible(m_Waypoints[index].v_origin, d->o, d))
               {
                    pNearestWP = &m_Waypoints[index];
                    min_distance = distance;
               }
          }
     }

     return pNearestWP;
}

waypoint_s *CWaypointClass::GetNearestWaypoint(vec v_src, float range)
{
     int index;
     waypoint_s *pNearestWP = NULL;
     float distance;
     float min_distance = 9999.0f;

     if (m_iWaypointCount < 1)
          return NULL;

     for (index=0; index < m_iWaypointCount; index++)
     {
          if (m_Waypoints[index].iFlags & W_FL_DELETED)
               continue;  // skip any deleted waypoints

          distance = GetDistance(m_Waypoints[index].v_origin, v_src);

          if ((distance < min_distance) && (distance <= range))
          {
               if (IsVisible(m_Waypoints[index].v_origin, v_src, NULL))
               {
                    pNearestWP = &m_Waypoints[index];
                    min_distance = distance;
               }
          }
     }

     return pNearestWP;
}

void CWaypointClass::CreateWPsAtTeleporters()
{
     loopv(ents)
     {
          if ((m_iWaypointCount-1) >= MAX_WAYPOINTS)
          {
               conoutf("Couldn't create new waypoints, max reached");
               return;
          }
          
          entity &e = ents[i];
          
          if (e.type != TELEPORT) continue;
          if (OUTBORD(e.x, e.y)) continue;

          vec telepos = { e.x, e.y, S(e.x, e.y)->floor+player1->eyeheight }, teledestpos = g_vecZero;
          
          // Find the teleport destination          
          int n = -1, tag = e.attr1, beenhere = -1;
          for(;;)
          {
               n = findentity(TELEDEST, n+1);
               if(n==beenhere || n<0) { conoutf("no teleport destination for tag %d", tag); continue; };
               if(beenhere<0) beenhere = n;
               if(ents[n].attr2==tag)
               {
                    teledestpos.x = ents[n].x;
                    teledestpos.y = ents[n].y;
                    teledestpos.z = S(ents[n].x, ents[n].y)->floor+player1->eyeheight;
                    break;
               }
          }
          
          if (vis(teledestpos, g_vecZero)) continue;
          
          // Add waypoint at teleporter and teleport destination
          waypoint_s *telewp = AddWaypoint(telepos, false);
          waypoint_s *teledestwp = AddWaypoint(teledestpos, false);
          
          if (telewp && teledestwp)
          {
               // Connect the teleporter waypoint with the teleport-destination waypoint(1 way)
               AddPath(telewp, teledestwp);
               
               // Flag waypoints
               telewp->iFlags = W_FL_TELEPORT;
               teledestwp->iFlags = W_FL_TELEPORTDEST;
          }
     }
}
                         

#ifdef WP_FLOOD

void CWaypointClass::StartFlood()
{
     int count=0, filtered=0, size=0, x, y;
     
     conoutf("Starting flood, this may take a while on large maps....");
     
     // Add wps at teleporters and their destination
     loopv(ents)
     {
          entity &e = ents[i];
          
          if (e.type != TELEPORT) continue;
          if (OUTBORD(e.x, e.y)) continue;

          vec telepos = { e.x, e.y, S(e.x, e.y)->floor+player1->eyeheight }, teledestpos = g_vecZero;
          
          // Find the teleport destination          
          int n = -1, tag = e.attr1, beenhere = -1;
          for(;;)
          {
               n = findentity(TELEDEST, n+1);
               if(n==beenhere || n<0) { conoutf("no teleport destination for tag %d", tag); continue; };
               if(beenhere<0) beenhere = n;
               if(ents[n].attr2==tag)
               {
                    teledestpos.x = ents[n].x;
                    teledestpos.y = ents[n].y;
                    teledestpos.z = S(ents[n].x, ents[n].y)->floor+player1->eyeheight;
                    break;
               }
          }
          
          if (vis(teledestpos, g_vecZero)) continue;
          
          // Add waypoint at teleporter and teleport destination
          waypoint_s *pWP = new waypoint_s;
          pWP->v_origin = telepos;
          pWP->ConnectedWPs.DeleteAllNodes();
          pWP->iFlags |= (W_FL_FLOOD | W_FL_TELEPORT);
               
          short i, j;
          GetFloodWPIndexes(telepos, &i, &j);
          m_FloodWPs[i][j].PushNode(pWP);
          size += sizeof(waypoint_s);               
          count++;
          
          waypoint_s *pWP2 = new waypoint_s;
          pWP2->v_origin = teledestpos;
          pWP2->ConnectedWPs.DeleteAllNodes();
          pWP2->iFlags |= (W_FL_FLOOD | W_FL_TELEPORTDEST);
          
          GetFloodWPIndexes(teledestpos, &i, &j);
          m_FloodWPs[i][j].PushNode(pWP2);
          size += sizeof(waypoint_s);               
          count++;
          
          // Connect the teleporter waypoint with the teleport-destination waypoint(1 way)
          AddPath(pWP, pWP2);
     }
          
     for (x=MINBORD; x<(ssize-MINBORD); x++)
     {
          for (y=MINBORD; y<(ssize-MINBORD); y++)
          {
               sqr *s = S(fast_f2nat(x), fast_f2nat(y));
               if (SOLID(s))
               {
                    continue;
               }
               
               if (fabs(s->ceil - s->floor) < player1->radius)
               {
                    continue;
               }
               
               bool small = false;
               vec from = { x, y, sheight(s, s, 1.0f)+1.0f };
               for (int a=(x-2);a<=(x+2);a++)
               {
                    if (small) break;
                    for (int b=(y-2);b<=(y+2);b++)
                    {
                         if ((x==a) && (y==b)) continue;
                         sqr *s2 = S(fast_f2nat(a), fast_f2nat(b));
                         vec to = { a, b, sheight(s2, s2, 1.0f)+1.0f };
                         if (from.z < (to.z-1-JUMP_HEIGHT))
                         {
                              small=true;
                              break;
                         }
                         
                         if ((a >= (x-1)) && (a <= (x+1)) && (b >= (y-1)) && (b <= (y+1)))
                         {
                              if ((to.z-1.0f) < (from.z-2.0f))
                              {
                                   small = true;
                                   break;
                              }
                         }
                         
                         vec src, dest;
                         traceresult_s tr;
                         //for (float zoffset=-1.0f;zoffset<=1.0f;zoffset++)
                         {
                              src = from, dest = to;
                           //   src.z += zoffset;
                             // dest.z += zoffset;
                              TraceLine(src, dest, NULL, false, &tr);
                              if (tr.collided)
                              {
                                   small=true;
                                   break;
                              }
                         }
                         if (small) break;
                    }
               }
               if (small) continue;
               
               if (GetNearestFloodWP(from, 2.0f, NULL)) continue;
               
               // Add WP
               waypoint_s *pWP = new waypoint_s;
               pWP->v_origin = from;
               pWP->ConnectedWPs.DeleteAllNodes();
               pWP->iFlags |= W_FL_FLOOD;
               
               short i, j;
               GetFloodWPIndexes(from, &i, &j);
               m_FloodWPs[i][j].PushNode(pWP);
               size += sizeof(waypoint_s);               
               count++;
          }
     }
     
     // Now connect nearby waypoints with eachother
     for (x=0;x<MAX_MAP_GRIDS;x++)
     {
          for (y=0;y<MAX_MAP_GRIDS;y++)
          {
               TLinkedList<waypoint_s *>::node_s *pNode = m_FloodWPs[x][y].GetFirst();
               while(pNode)
               {
                    ConnectFloodWP(pNode->Entry);
                    size += sizeof(waypoint_s*);
                    pNode = pNode->next;
               }
          }
     }                   
     
     // Filter all nodes which aren't connected to any other nodes
     for (x=0;x<MAX_MAP_GRIDS;x++)
     {
          for (y=0;y<MAX_MAP_GRIDS;y++)
          {
               TLinkedList<waypoint_s *>::node_s *pNode = m_FloodWPs[x][y].GetFirst(), *pTemp;
               while(pNode)
               {
                    if (pNode->Entry->ConnectedWPs.Empty())
                    {
                         pTemp = pNode;
                         pNode = pNode->next;
                         delete pTemp->Entry;
                         m_FloodWPs[x][y].DeleteNode(pTemp);
                         count--;
                         filtered++;
                         size -= sizeof(waypoint_s);
                         continue;
                    }
                    pNode = pNode->next;
               }
          }
     }                   
         
     size += sizeof(m_FloodWPs);               
     conoutf("Added %d wps", count);
     conoutf("Filtered %d wps", filtered);
     
     char szSize[64];
     sprintf(szSize, "Total size: %.2f Kb", float(size)/1024.0f);
     conoutf(szSize);
}

void CWaypointClass::ConnectFloodWP(waypoint_s *pWP)
{
     if (!pWP) return;
     
     const float flRange = 4.0f;
     TLinkedList<waypoint_s *>::node_s *pNode;
     short i, j, MinI, MaxI, MinJ, MaxJ, Offset = ceil(flRange / SECTOR_SIZE);
     float flDist;

     GetFloodWPIndexes(pWP->v_origin, &i, &j);
     MinI = i - Offset;
     MaxI = i + Offset;
     MinJ = j - Offset;
     MaxJ = j + Offset;

     if (MinI < 0)
          MinI = 0;
     if (MaxI > MAX_MAP_GRIDS - 1)
          MaxI = MAX_MAP_GRIDS - 1;
     if (MinJ < 0)
          MinJ = 0;
     if (MaxJ > MAX_MAP_GRIDS - 1)
          MaxJ = MAX_MAP_GRIDS - 1;

     for (int x=MinI;x<=MaxI;x++)
     {
          for(int y=MinJ;y<=MaxJ;y++)
          {
               pNode = m_FloodWPs[x][y].GetFirst();

               while(pNode)
               {
                    if (pNode->Entry == pWP)
                    {
                         pNode = pNode->next;
                         continue;
                    }

                    flDist = GetDistance(pWP->v_origin, pNode->Entry->v_origin);
                    if (flDist <= flRange)
                    {
                         if (IsVisible(pWP->v_origin, pNode->Entry->v_origin, NULL))
                         {
                              pWP->ConnectedWPs.AddNode(pNode->Entry);
                         }
                    }

                    pNode = pNode->next;
               }
          }
     }
}

waypoint_s *CWaypointClass::GetNearestFloodWP(vec v_origin, float flRange, waypoint_s *pIgnore)
{
     TLinkedList<waypoint_s *>::node_s *pNode;
     waypoint_s *pNearest = NULL;
     short i, j, MinI, MaxI, MinJ, MaxJ, Offset = ceil(flRange / SECTOR_SIZE);
     float flNearestDist = 9999.99, flDist;

     GetFloodWPIndexes(v_origin, &i, &j);
     MinI = i - Offset;
     MaxI = i + Offset;
     MinJ = j - Offset;
     MaxJ = j + Offset;

     if (MinI < 0)
          MinI = 0;
     if (MaxI > MAX_MAP_GRIDS - 1)
          MaxI = MAX_MAP_GRIDS - 1;
     if (MinJ < 0)
          MinJ = 0;
     if (MaxJ > MAX_MAP_GRIDS - 1)
          MaxJ = MAX_MAP_GRIDS - 1;

     for (int x=MinI;x<=MaxI;x++)
     {
          for(int y=MinJ;y<=MaxJ;y++)
          {
               pNode = m_FloodWPs[x][y].GetFirst();

               while(pNode)
               {
                    if (pNode->Entry == pIgnore)
                    {
                         pNode = pNode->next;
                         continue;
                    }

                    flDist = GetDistance(v_origin, pNode->Entry->v_origin);
                    if ((flDist < flNearestDist) && (flDist <= flRange))
                    {
                         if (IsVisible(v_origin, pNode->Entry->v_origin, NULL))
                         {
                              pNearest = pNode->Entry;
                              flNearestDist = flDist;
                         }
                    }

                    pNode = pNode->next;
               }
          }
     }
     return pNearest;
}

void CWaypointClass::GetFloodWPIndexes(vec v_origin, short *i, short *j)
{
     // Function code by cheesy and PMB
     *i = abs((int)((int)(v_origin.x + ssize) / SECTOR_SIZE));
     *j = abs((int)((int)(v_origin.y + ssize) / SECTOR_SIZE));

     if (*i > MAX_MAP_GRIDS - 1)
          *i = MAX_MAP_GRIDS - 1;
     if (*j > MAX_MAP_GRIDS - 1)
          *j = MAX_MAP_GRIDS - 1;
}

void CWaypointClass::DrawNearFloodWPs()
{
     //if (m_iFloodWPCount < 1)
          //return; // No Flood WP's to draw

     TLinkedList<waypoint_s *>::node_s *pNode;
     waypoint_s *pNearest = NULL;
     short i, j, MinI, MaxI, MinJ, MaxJ, Offset = ceil(15.0f / SECTOR_SIZE);
     float flNearestDist = 9999.99, flDist;
     short DrawWPCount = 0;

     GetFloodWPIndexes(player1->o, &i, &j);
     MinI = i - Offset;
     MaxI = i + Offset;
     MinJ = j - Offset;
     MaxJ = j + Offset;

     if (MinI < 0)
          MinI = 0;
     if (MaxI > MAX_MAP_GRIDS - 1)
          MaxI = MAX_MAP_GRIDS - 1;
     if (MinJ < 0)
          MinJ = 0;
     if (MaxJ > MAX_MAP_GRIDS - 1)
          MaxJ = MAX_MAP_GRIDS - 1;

     for (int x=MinI;x<=MaxI;x++)
     {
          for(int y=MinJ;y<=MaxJ;y++)
          {
               pNode = m_FloodWPs[x][y].GetFirst();

               while(pNode)
               {
                    flDist = GetDistance(player1->o, pNode->Entry->v_origin);
                    if (flDist <= 15)
                    {
                         DrawWPCount++;
                         
                         vec o = pNode->Entry->v_origin;
                         vec e = o;
                         o.z -= 2;
                         e.z += 2;
                         
                         linestyle(2.0f, 0x40, 0x40, 0xFF);
                         line(int(o.x), int(o.y), int(o.z), int(e.x), int(e.y), int(e.z)); 

                         // Draw cube
                         int ix = int(o.x), iy = int(o.y);
                         sqr *s = S(ix, iy);
                         float h1 = sheight(s, s, 1.0f);
                         float h2 = sheight(s, SWS(s,1,0,ssize), 1.0f);
                         float h3 = sheight(s, SWS(s,1,1,ssize), 1.0f);
                         float h4 = sheight(s, SWS(s,0,1,ssize), 1.0f);
                         line(ix,   iy,   h1, ix,   iy+1, h4);
                         line(ix+1, iy,   h2, ix+1, iy+1, h3);
                         line(ix,   iy,   h1, ix+1, iy,   h2);
                         line(ix,   iy+1, h4, ix+1, iy+1, h3);
                         
                         if (flNearestDist > flDist)
                         {
                              flNearestDist = flDist;
                              pNearest = pNode->Entry;
                         }
                    }

                    pNode = pNode->next;
               }
          }
     }

     if (pNearest)
     {
          pNode = pNearest->ConnectedWPs.GetFirst();
          while(pNode)
          {
               vec o = pNode->Entry->v_origin;
               vec e = pNearest->v_origin;
               
               linestyle(2.0f, 0xFF, 0xFF, 0xFF);
               line(int(o.x), int(o.y), int(o.z), int(e.x), int(e.y), int(e.z));
               
               pNode = pNode->next;
          }
     }
}

#endif // WP_FLOOD
// Waypoint class end


// Waypoint commands begin

void addwp(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.AddWaypoint(player1->o, true);
}

COMMAND(addwp, ARG_NONE);

void delwp(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.DeleteWaypoint(player1);
}
     
COMMAND(delwp, ARG_NONE);

void wpvisible(int on)
{
     WaypointClass.SetWaypointsVisible(bool(on));
}

COMMAND(wpvisible, ARG_1INT);

void wpsave(void)
{
     WaypointClass.SaveWaypoints();
}

COMMAND(wpsave, ARG_NONE);

void wpload(void)
{
     WaypointClass.LoadWaypoints(getclientmap());
}

COMMAND(wpload, ARG_NONE);

void autowp(int on)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.SetAutoWaypoint(bool(on));
}

COMMAND(autowp, ARG_1INT);

void wpinfo(int on)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.SetWPInfo(bool(on));
}

COMMAND(wpinfo, ARG_1INT);

void addpath1way1(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.ManuallyCreatePath(player1, 1, false);
}

COMMAND(addpath1way1, ARG_NONE);

void addpath1way2(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.ManuallyCreatePath(player1, 2, false);
}

COMMAND(addpath1way2, ARG_NONE);

void addpath2way1(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.ManuallyCreatePath(player1, 1, true);
}

COMMAND(addpath2way1, ARG_NONE);

void addpath2way2(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.ManuallyCreatePath(player1, 2, true);
}

COMMAND(addpath2way2, ARG_NONE);

void delpath1way1(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.ManuallyDeletePath(player1, 1, false);
}

COMMAND(delpath1way1, ARG_NONE);

void delpath1way2(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.ManuallyDeletePath(player1, 2, false);
}

COMMAND(delpath1way2, ARG_NONE);

void delpath2way1(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.ManuallyDeletePath(player1, 1, true);
}

COMMAND(delpath2way1, ARG_NONE);

void delpath2way2(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.ManuallyDeletePath(player1, 2, true);
}

COMMAND(delpath2way2, ARG_NONE);

void addtelewps(void)
{
     WaypointClass.SetWaypointsVisible(true);
     WaypointClass.CreateWPsAtTeleporters();
}

COMMAND(addtelewps, ARG_NONE);

void setjumpwp(void)
{
     waypoint_s *wp = WaypointClass.GetNearestWaypoint(player1, 20.0f);
     if (wp)
     {
          WaypointClass.SetWPFlags(wp, W_FL_JUMP);
     }
}

COMMAND(setjumpwp, ARG_NONE);

void unsetjumpwp(void)
{
     waypoint_s *wp = WaypointClass.GetNearestWaypoint(player1, 20.0f);
     if (wp)
     {
          WaypointClass.UnsetWPFlags(wp, W_FL_JUMP);
     }
}

COMMAND(unsetjumpwp, ARG_NONE);

#ifdef WP_FLOOD
void startflood(void)
{
     WaypointClass.StartFlood();
}

COMMAND(startflood, ARG_NONE);
#endif

// Debug functions
#ifdef WP_FLOOD

#ifndef RELEASE_BUILD
void botsheadtome(void)
{
     loopv(bots)
     {
          if (!bots[i] || !bots[i]->pBot) continue;
          
          bots[i]->pBot->GoToDebugGoal(player1->o);
     }
}

COMMAND(botsheadtome, ARG_NONE);

void setdebuggoal(void) { v_debuggoal = player1->o; };
COMMAND(setdebuggoal, ARG_NONE);

void botsheadtodebuggoal(void)
{
     loopv(bots)
     {
          if (!bots[i] || !bots[i]->pBot) continue;
          
          bots[i]->pBot->GoToDebugGoal(v_debuggoal);
     }
}

COMMAND(botsheadtodebuggoal, ARG_NONE);

void countvalidcubes(void)
{
     int count=0;
     int solid=0;
     int tosmall=0;
     
     for (int x=MINBORD; x<(ssize-MINBORD); x++)
     {
          for (int y=MINBORD; y<(ssize-MINBORD); y++)
          {
               sqr *s = S(fast_f2nat(x), fast_f2nat(y));
               if (SOLID(s))
               {
                    solid++;
                    continue;
               }
               
               if (fabs(s->ceil - s->floor) < player1->radius)
               {
                    tosmall++;
                    continue;
               }
               
               bool small = false;
               for (int a=(x-1);a<=(x+1);a++)
               {
                    for (int b=(y-1);b<=(y+1);b++)
                    {
                         if ((x==a) && (y==b)) continue;
                         sqr *s2 = S(fast_f2nat(a), fast_f2nat(b));
                         
                         vec from = { x, y, (s->floor+1) };
                         vec to = { a, b, (s2->floor+1) };
                         traceresult_s tr;
                         TraceLine(from, to, NULL, false, &tr);
                         if (tr.collided)
                         {
                              //tosmall++;
                              small=true;
                         }
                    }
               }
               if (small) { tosmall++; continue; }
               
               count++;
          }
     }
     
     conoutf("Valid cubes: %d", count);
     conoutf("Solid cubes: %d", solid);
     conoutf("To small cubes: %d", tosmall);
}

COMMAND(countvalidcubes, ARG_NONE);

#endif // RELEASE_BUILD

#endif // WP_FLOOD

// End debug functions
// Waypoint commands end


// Bot class begin

bool CBot::FindWaypoint()
{
     waypoint_s *wpselect;
     int index;
     float distance, min_distance[3];
     waypoint_s *min_wp[3];

     for (index=0; index < 3; index++)
     {
          min_distance[index] = 9999.0;
          min_wp[index] = NULL;
     }

     TLinkedList<waypoint_s *>::node_s *pNode = m_pCurrentWaypoint->ConnectedWPs.GetFirst();

     while (pNode)
     {
          // if index is not a current or recent previous waypoint...
          if ((pNode->Entry != m_pCurrentWaypoint) &&
              (pNode->Entry != m_pPrevWaypoints[0]) &&
              (pNode->Entry != m_pPrevWaypoints[1]) &&
              (pNode->Entry != m_pPrevWaypoints[2]) &&
              (pNode->Entry != m_pPrevWaypoints[3]) &&
              (pNode->Entry != m_pPrevWaypoints[4]))
          {
               // find the distance from the bot to this waypoint
               distance = GetDistance(pNode->Entry->v_origin);

               if (distance < min_distance[0])
               {
                    min_distance[2] = min_distance[1];
                    min_wp[2] = min_wp[1];

                    min_distance[1] = min_distance[0];
                    min_wp[1] = min_wp[0];

                    min_distance[0] = distance;
                    min_wp[0] = pNode->Entry;
               }
               else if (distance < min_distance [1])
               {
                    min_distance[2] = min_distance[1];
                    min_wp[2] = min_wp[1];

                    min_distance[1] = distance;
                    min_wp[1] = pNode->Entry;
               }
               else if (distance < min_distance[2])
               {
                    min_distance[2] = distance;
                    min_wp[2] = pNode->Entry;
               }
          }
          pNode = pNode->next;
     }

     wpselect = NULL;

     // about 20% of the time choose a waypoint at random
     // (don't do this any more often than every 10 seconds)

     if ((RandomLong(1, 100) <= 20) && (m_iRandomWaypointTime <= lastmillis))
     {
          m_iRandomWaypointTime = lastmillis + 10000;

          if (min_wp[2])
               index = RandomLong(0, 2);
          else if (min_wp[1])
               index = RandomLong(0, 1);
          else if (min_wp[0])
               index = 0;
          else
               return false;  // no waypoints found!

          wpselect = min_wp[index];
     }
     else
     {
          // use the closest waypoint that has been recently used
          wpselect = min_wp[0];
     }

     if (wpselect)  // was a waypoint found?
     {
          m_pPrevWaypoints[4] = m_pPrevWaypoints[3];
          m_pPrevWaypoints[3] = m_pPrevWaypoints[2];
          m_pPrevWaypoints[2] = m_pPrevWaypoints[1];
          m_pPrevWaypoints[1] = m_pPrevWaypoints[0];
          m_pPrevWaypoints[0] = m_pCurrentWaypoint;

          SetCurrentWaypoint(wpselect);
          return true;
     }

     return false;  // couldn't find a waypoint
}

bool CBot::HeadToWaypoint()
{
     if (!m_pCurrentWaypoint)
          return false; // Can't head to waypoint

     bool Touching = false;
     float WPDist = GetDistance(m_pCurrentWaypoint->v_origin);

#ifndef RELEASE_BUILD
     if (m_pCurrentGoalWaypoint && vis(m_vGoal, g_vecZero))
          conoutf("Warning: m_vGoal unset");
#endif

     // did the bot run past the waypoint? (prevent the loop-the-loop problem)
     if ((m_fPrevWaypointDistance > 1.0) && (WPDist > m_fPrevWaypointDistance) &&
         (WPDist <= 5.0f))
          Touching = true;
     // bot needs to be close for jump waypoints
     else if (m_pCurrentWaypoint->iFlags & W_FL_JUMP)
          Touching = (WPDist <= 1.5f);
     // are we close enough to a target waypoint...
     else if (WPDist <= 3.0f)
     {
          if (!m_pCurrentGoalWaypoint || (m_pCurrentWaypoint != m_pCurrentGoalWaypoint) ||
               IsVisible(m_vGoal) || (WPDist <= 1.5f))
               Touching = true;
          // If the bot has a goal check if he can see his next wp
          if (m_pCurrentGoalWaypoint && (m_pCurrentGoalWaypoint != m_pCurrentWaypoint) &&
             !m_AStarNodeList.Empty() && (WPDist >= 1.0f) &&
             !IsVisible(m_AStarNodeList.GetFirst()->Entry->v_origin))
               Touching = false;
     }
     
     // save current distance as previous
     m_fPrevWaypointDistance = WPDist;

     // Reached the waypoint?
     if (Touching)
     {
          // Reached a jump waypoint?
          if (m_pCurrentWaypoint->iFlags & W_FL_JUMP)
               m_pMyEnt->jumpnext = true;
                              
          m_fPrevWaypointDistance = 0.0;

          // Does the bot have a goal?
          if (m_pCurrentGoalWaypoint)
          {               
               if (m_pCurrentWaypoint != m_pCurrentGoalWaypoint)
               {
                    if (m_AStarNodeList.Empty())
                    {
                         if (!AStar())
                         {
                              // Bot is calculating a new path, just stand still for now
                              ResetMoveSpeed();
                              m_iWaypointTime += 200;
                              return true;
                         }
                         else
                         {
                              if (m_AStarNodeList.Empty())
                              {
                                   //m_UnreachableNodes.PushNode(unreachable_node_s(m_pCurrentGoalWaypoint, gpGlobals->time));
                                   return false; // Couldn't get a new wp to go to
                              }
                         }
                    }

                    m_pCurrentWaypoint = m_AStarNodeList.Pop();

                    if (!IsVisible(m_pCurrentWaypoint->v_origin)) 
                        //(!(m_pCurrentWaypoint->iFlags & W_FL_TELEPORT)))
                    {
                         //m_UnreachableNodes.PushNode(unreachable_node_s(m_pCurrentGoalWaypoint, gpGlobals->time));
                         condebug("Next WP not visible");
                         return false;
                    }

                    /* UNDONE?
                    waypoint_s *pNextWP = GetNextFloodWP();
                    int Counter = 2;
                    while(pNextFloodWP && Counter)
                    {
                         if (IsVisible(pNextFloodWP->v_origin))
                              m_pCurrentWaypoint = m_AStarNodeList.Pop();
                         else
                              break;

                         Counter--;
                         pNextWP = GetNextFloodWP();
                    }*/

                    SetCurrentWaypoint(m_pCurrentWaypoint);
               }
               else
               {
                    // Bot reached the goal waypoint but couldn't reach the goal itself
                    // (otherwise we wouldn't be in this function)
                    //m_UnreachableNodes.PushNode(unreachable_node_s(m_pCurrentGoalWaypoint, gpGlobals->time));
                    return false;
               }
          }
          else // else just find a random near wp
          {
               short index = 4;
               bool status;

               // try to find the next waypoint
               while (((status = FindWaypoint()) == false) && (index > 0))
               {
                    // if waypoint not found, clear oldest previous index and try again

                    m_pPrevWaypoints[index] = NULL;
                    index--;
               }

               if (status == false)
               {
                    ResetWaypointVars();
                    condebug("Couldn't find new random waypoint");
                    return false;
               }
          }
          m_iWaypointHeadPauseTime = lastmillis + 75;
          m_iWaypointTime += 75;          
     }

     // keep turning towards the waypoint...
     if (m_pCurrentWaypoint->iFlags & W_FL_FLOOD)
     {
          vec aim = m_pCurrentWaypoint->v_origin;
          aim.x+=0.5f;
          aim.y+=0.5f;
          AimToVec(aim);
     }
     else
          AimToVec(m_pCurrentWaypoint->v_origin);

     if (m_fYawToTurn <= 25.0f)
          m_iWaypointHeadLastTurnLessTime = lastmillis;

     // Bot had to turn much for a while?
     if ((m_iWaypointHeadLastTurnLessTime > 0) &&
         (m_iWaypointHeadLastTurnLessTime < (lastmillis - 1000)))
     {
          m_iWaypointHeadPauseTime = lastmillis + 200;
          m_iWaypointTime += 200;
     }

     if (m_iWaypointHeadPauseTime >= lastmillis)
     {
          m_pMyEnt->move = 0;
          //conoutf("Pause in HeadToWaypoint()");
     }

     return true;
}

// returns true when done or failure
bool CBot::HeadToGoal()
{
     // Does the bot has a goal(waypoint)?
     if (m_pCurrentGoalWaypoint)
     {
          if (ReachedGoalWP())
          {
               return false;
          }
          else
          {
               if (m_bCalculatingAStarPath)
               {
                    // Bot is calculating a new path, just stand still for now
                    ResetMoveSpeed();
                    m_iWaypointTime += 200;

                    // Done calculating the path?
                    if (AStar())
                    {
                         m_bCalculatingAStarPath = false;
                         if (m_AStarNodeList.Empty())
                         {
                              //m_UnreachableNodes.PushNode(unreachable_node_s(m_pCurrentGoalWaypoint, gpGlobals->time));
                              return false; // Couldn't find a path
                         }
                         else
                              SetCurrentWaypoint(m_AStarNodeList.Pop());
                    }
                    else
                         return true; // else just wait a little bit longer
               }
          
               if (!CurrentWPIsValid())
               {
                    // Current waypoint isn't reachable, search new one
                    waypoint_s *pWP = NULL;

#ifdef WP_FLOOD                                        
                    if (m_pCurrentGoalWaypoint->iFlags & W_FL_FLOOD)
                         pWP = WaypointClass.GetNearestFloodWP(m_pMyEnt, 5.0f);
                    else
#endif                    
                         pWP = WaypointClass.GetNearestWaypoint(m_pMyEnt, 15.0f);

                    if (!pWP || (pWP == m_pCurrentWaypoint))
                         return false;
                                        
                    SetCurrentWaypoint(pWP);
                    if (AStar() && m_AStarNodeList.Empty()) return false;
               }
               
               if (GetDistance(m_pCurrentWaypoint->v_origin) >= 3.0f)
               {
                    vec from = m_pMyEnt->o, to, forward, up, right;
                    from.z -= 1.0f;
                    to = from;
                    AnglesToVectors(GetViewAngles(), forward, right, up);
                    vmul(forward, 3.0f);
                    vadd(to, forward);
                    debugbeam(from, to);
                    if (!::IsVisible(from, to))
                         m_pMyEnt->jumpnext = true;
               }
          }
     }
     else
          return false;
          
     return HeadToWaypoint();
}

// return true when done calculating
bool CBot::AStar()
{
     if (!m_pCurrentGoalWaypoint || !m_pCurrentWaypoint)
     {
          m_bCalculatingAStarPath = false;
          return true;
     }

     // Ideas by PMB :
     // * Make locals static to speed up a bit
     // * MaxCycles per frame and make it fps dependent

     static int iMaxCycles;
     static int iCurrentCycles;
     static float newg;
     static waypoint_s *n, *n2;
     static TLinkedList<waypoint_s *>::node_s *pPath = NULL;

     iMaxCycles = BotManager.m_iFrameTime;
     //conoutf("MaxCycles: %d", iMaxCycles);
     iCurrentCycles = 0;

     if (!m_bCalculatingAStarPath)
     {
          m_pCurrentWaypoint->g = 0.0f;
          m_pCurrentWaypoint->f = m_pCurrentWaypoint->g + GetDistance(m_pCurrentGoalWaypoint->v_origin);
          m_pCurrentWaypoint->pParent = NULL;
          m_AStarOpenList.Clear();
          m_AStarClosedList.DeleteAllNodes();
          m_AStarOpenList.AddEntry(m_pCurrentWaypoint, m_pCurrentWaypoint->f);
          
          m_AStarNodeList.DeleteAllNodes();
     }

     while(!m_AStarOpenList.Empty())
     {
          if (iCurrentCycles >= iMaxCycles)
          {
               m_bCalculatingAStarPath = true;
               return false;
          }

          n = m_AStarOpenList.Pop();

          // Done with calculating
          if (n == m_pCurrentGoalWaypoint)
          {
               while(n)
               {
                    m_AStarNodeList.PushNode(n);
                    n = n->pParent;
               }

               m_AStarOpenList.Clear();
               m_AStarClosedList.DeleteAllNodes();

               m_bCalculatingAStarPath = false;
               return true;
          }

          pPath = n->ConnectedWPs.GetFirst();
          while(pPath)
          {
               n2 = pPath->Entry;

               newg = AStarCost(n, n2);

               if ((n2->g <= newg) &&
                    ((m_AStarClosedList.IsInList(n2) || m_AStarOpenList.IsInList(n2, n2->f))))
               {
                    pPath = pPath->next;
                    continue;
               }
               
               n2->pParent = n;
               n2->g = newg;
               n2->f = n2->g + GetDistance(n2->v_origin, m_pCurrentGoalWaypoint->v_origin);

               m_AStarClosedList.DeleteEntry(n2);

               if (!m_AStarOpenList.IsInList(n2, n2->f))
               {
                    m_AStarOpenList.AddEntry(n2, n2->f);
               }
               pPath = pPath->next;
          }
          
          m_AStarClosedList.PushNode(n);
          iCurrentCycles++;
     }

     // Failed making path  
     condebug("Path failed");
     m_AStarOpenList.Clear();
     m_AStarClosedList.DeleteAllNodes();     
     m_bCalculatingAStarPath = false;
     return true;
}

float CBot::AStarCost(waypoint_s *pWP1, waypoint_s *pWP2)
{
	float flCost = 1.0f;
/* UNDONE
	// Is there a player near this WP and is he near the bot?
	CBasePlayer *pPlayer;
	float flBotToPlayerDist, flNodeToPlayerDist;
	for(byte b=1;b<=gpGlobals->maxClients;b++)
	{
		pPlayer = (CBasePlayer *)UTIL_PlayerByIndex(b);

		if (pPlayer == this)
			continue;

		if (UTIL_IsValidPlayer(pPlayer) == false)
			continue;

		flBotToPlayerDist = GetDistance(pPlayer);
		flNodeToPlayerDist = UTIL_Distance(pPlayer->pev->origin, pWP2->v_origin);

		// Is this player near the new node and is the player fairly close to the bot
		if ((flNodeToPlayerDist < 200.0f) && (flBotToPlayerDist < 1250.0f))
		{
			if (pPlayer->FVisible(pWP2->v_origin))
				flCost += ((1250.0f - flBotToPlayerDist) * 0.01f) + ((200.0f - flNodeToPlayerDist) * 0.01f);
          }
     }
*/
     
     // Check nearby cubes...
     int x = int(pWP2->v_origin.x);
     int y = int(pWP2->v_origin.y);
     
     for (int a=(x-3);a<=(x+3);a++)
     {
          for (int b=(y-3);b<=(y+3);b++)
          {
               if ((x==a) && (y==b)) continue;
               sqr *s = S(fast_f2nat(a), fast_f2nat(b));
               vec to = { a, b, sheight(s, s, 1.0f) };
               
               // See if there is a obstacle(cube or mapmodel) nearby          
               traceresult_s tr;
               TraceLine(pWP2->v_origin, to, NULL, false, &tr);
               if (tr.collided)
                    flCost*=1.2f;
               
               vec from = to;
               to.z -= (JUMP_HEIGHT - 1.0f);
               TraceLine(from, to, NULL, false, &tr);
               if (!tr.collided)
                    flCost *= 1.2f;
          }
     }

     return flCost;
}

void CBot::ResetWaypointVars()
{
     m_iWaypointTime = 0;
     m_pCurrentWaypoint = NULL;
     m_pCurrentGoalWaypoint = NULL;
     m_pPrevWaypoints[0] = NULL;
     m_pPrevWaypoints[1] = NULL;
     m_pPrevWaypoints[2] = NULL;
     m_pPrevWaypoints[3] = NULL;
     m_pPrevWaypoints[4] = NULL;
     m_fPrevWaypointDistance = 0;
     m_iWaypointHeadLastTurnLessTime = 0;
     m_iWaypointHeadPauseTime = 0;
     m_bCalculatingAStarPath = false;
     m_AStarNodeList.DeleteAllNodes();
     m_AStarOpenList.Clear();
     m_AStarClosedList.DeleteAllNodes();
     m_bGoToDebugGoal = false;
}

void CBot::SetCurrentWaypoint(waypoint_s *pWP)
{
     m_pCurrentWaypoint = pWP;
     m_iWaypointTime = lastmillis;
}

bool CBot::CurrentWPIsValid()
{
     if (!m_pCurrentWaypoint)
     {
          condebug("Invalid WP: Is NULL");
          return false;
     }
     
     // check if the bot has been trying to get to this waypoint for a while...
     if ((m_iWaypointTime + 5000) < lastmillis)
     {   
          condebug("Invalid WP: time over");
          return false;
     }

#ifndef RELEASE_BUILD     
     if (!IsVisible(m_pCurrentWaypoint->v_origin))
          condebug("Invalid WP: Not visible");
#endif
          
     return (IsVisible(m_pCurrentWaypoint->v_origin));
}

bool CBot::ReachedGoalWP()
{
     if ((!m_pCurrentWaypoint) || (!m_pCurrentGoalWaypoint))
          return false;

     if (m_pCurrentWaypoint != m_pCurrentGoalWaypoint)
          return false;

     return ((GetDistance(m_pCurrentGoalWaypoint->v_origin) <= 3.0f) &&
             (IsVisible(m_pCurrentGoalWaypoint->v_origin)));
}

#ifdef WP_FLOOD
void CBot::GoToDebugGoal(vec o)
{
     ResetWaypointVars();
     
     waypoint_s *wp = WaypointClass.GetNearestWaypoint(m_pMyEnt, 20.0f);
     waypoint_s *goalwp = WaypointClass.GetNearestWaypoint(player1, 20.0f);
     
     if (!wp || !goalwp)
     {
          wp = WaypointClass.GetNearestFloodWP(m_pMyEnt, 5.0f);
          goalwp = WaypointClass.GetNearestFloodWP(player1, 5.0f);
     }
     if (!wp || !goalwp) return;
          
     SetCurrentWaypoint(wp);
     SetCurrentGoalWaypoint(goalwp);
     m_vGoal = o;
     m_bGoToDebugGoal = true;
}
#endif

// Bot class end
