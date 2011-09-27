#ifndef GAME_COMMANDER_H
#define GAME_COMMANDER_H

#include <sstream>
#include <NMSTL/internal.h>

#include "netio.h"
#include "cube.h"

// yna
// placeholders for commander mode.

extern netio_state netstate;

extern block sel;
extern bool selset;
extern int selh;


extern void select_items(std::vector<int>& item_indices);
extern void select_players(std::vector<int>& player_indices);
extern void select_bots(std::vector<int>& bot_indices);


bool commandmode = false;
VAR(commanding,0,0,1);

void togglecommander()
{
    if(!editmode && !allowedittoggle()) return;         // must already be in edit mode...
    if(!netstate.game_subs) return;                     // must have a valid subscriber.
    commandmode = !commandmode;
    commanding = commandmode;
}

COMMANDN(commander, togglecommander, ARG_NONE);

/*************************************************
 *
 * Query output rendering
 *
 *************************************************/
typedef std::map< std::string, std::string > results_map;
results_map query_results;

void renderqueries()
{
    if (query_results.size() > 0)
    {
	results_map::iterator results_it = query_results.begin();
	results_map::iterator results_end = query_results.end();

	glPopMatrix();
	glPushMatrix();

	glOrtho(0, VIRTW*3/2, VIRTH*3/2, 0, -1, 1);

	//int query_h = max(2600 - (query_results.size()*70), 2250);
	int query_h = 2600 - (query_results.size()*70);
	for (; results_it != results_end; ++results_it) {
	    std::string result_str = results_it->first + " " + results_it->second;
	    draw_textf("%s", 100, query_h, 2, (int) result_str.c_str());
	    query_h += 70;
	}
    }
}


/************************************************
 *
 * Query handlers
 *
 ************************************************/

void handle_count(std::string query_name, std::string data)
{
    query_results[query_name] =
        NMSTL::to_string( *((int*) (data.c_str() + sizeof(borealis_tuple) + sizeof(int))) );
}


void handle_distance(std::string query_name, std::string data)
{
    double dist = *((double*) (data.c_str() + sizeof(borealis_tuple)));
    std::ostringstream o;
    o << std::setprecision(5) << dist; 
    query_results[query_name] = NMSTL::to_string( o.str() );
}


/************************************************
 *
 * Query helpers.
 *
 ************************************************/

void list_queries()
{
    if (!commanding) {
	conoutf("Not commanding");
	return;
    }

    std::vector<std::string>& query_names = netstate.game_subs->list_queries();

    std::vector<std::string>::iterator queries_it = query_names.begin();
    std::vector<std::string>::iterator queries_end = query_names.end();

    conoutf("Queries:");
    for (; queries_it != queries_end; ++queries_it)
	conoutf(" -- %s", (int) queries_it->c_str());
}

COMMANDN(queries, list_queries, ARG_NONE);

enum { VAL_ITEM = 0, VAL_PLAYER, VAL_REGION, VAL_FRIEND, VAL_ENEMY, VAL_INVALID };


// TODO: change this to a union type.
struct value {
    int type;

    dynent* player;
    int player_index;
    bool player_is_bot;
    std::string player_team;

    entity* item;
    int item_index;

    block sel;
    int selh;
};

typedef void (*handler_fn)(std::string, std::string);

struct query_handle
{
    std::vector< std::pair<std::string, std::string> > box_xmls;
    std::string output_stream_name;
    size_t tuple_size;
    handler_fn handler;
};


bool is_static(value& v)
{
    return (v.type == VAL_ITEM || v.type == VAL_REGION);
}


bool is_team(value& v)
{
    return (v.type == VAL_FRIEND || v.type == VAL_ENEMY);
}

value parse_svalue(std::string v)
{
    value r;

    if (v == "ITEM")
    {
        r.type = VAL_ITEM;
        r.item_index = find_selected_item(1);
        r.item = &(ents[r.item_index]);

    } else if (v == "REGION")
    {
	r.type = VAL_REGION;
	r.sel = sel;
	r.selh = selh;

    } else
    {
	r.type = VAL_INVALID;
    }
    return r;
}

value parse_dvalue(std::string v)
{
    value r;

    std::string player_string = std::string() + "PLAYER";
    std::string bot_string = std::string() + "BOT";

    if ( !v.compare(0, player_string.length(), player_string) )
    {
        int selection_count = 1;
        if ( v.length() > player_string.length() ) {
            std::string count_string = 
                v.substr(player_string.length(), v.length() - player_string.length());
            selection_count = atoi(count_string.c_str());
        }

        r.player_index = find_selected_player(selection_count);
        if ( r.player_index < 0 ) {
            r.type = VAL_INVALID;
            return r;
        }

        r.type = VAL_PLAYER;
        r.player = players[r.player_index];
        r.player_is_bot = false;

    } else if ( !v.compare(0, bot_string.length(), bot_string) )
    {
        int selection_count = 1;
        if ( v.length() > bot_string.length() ) {
            std::string count_string = 
                v.substr(bot_string.length(), v.length() - bot_string.length());
            selection_count = atoi(count_string.c_str());
        }
        
        r.player_index = find_selected_bot(selection_count);
        if ( r.player_index < 0 ) {
            r.type = VAL_INVALID;
            return r;
        }

        r.type = VAL_PLAYER;
        r.player = bots[r.player_index];
        r.player_is_bot = true;

        conoutf("Bot index %d", r.player_index);

    } else
    {
	r.type = VAL_INVALID;
    }

    return r;
}

value parse_value(std::string v)
{
    value r;

    r = parse_dvalue(v);
    if ( r.type == VAL_INVALID) {
        r = parse_svalue(v);
    }

    return r;
}


value parse_rvalue(std::string v, value& w)
{
    value r;

    if (v == "FRIEND") {

        r.player_index = (w.player_is_bot? find_selected_bot(1) : find_selected_player(1));

        if ( r.player_index < 0 ) {
            r.type = VAL_INVALID;
            return r;
        }
        r.type = VAL_FRIEND;
        r.player_team =
            (w.player_is_bot? std::string(bots[r.player_index]->team)
             : std::string(players[r.player_index]->team));

    } else if (v == "ENEMY") {

        int player_index = (w.player_is_bot? find_selected_bot(1) : find_selected_player(1));
        if ( r.player_index < 0 ) {
            r.type = VAL_INVALID;
            return r;
        }
        r.type = VAL_ENEMY;
        r.player_team = 
            (w.player_is_bot? std::string(bots[player_index]->team)
             : std::string(players[player_index]->team));

    } else
    {
	r.type = VAL_INVALID;
    }
    return r;
}


std::string type_to_string(value& v)
{
    if (v.type == VAL_PLAYER) {
        if (v.player_is_bot)
            return std::string("BOT");
        else
            return std::string("PLAYER");
    } else if (v.type == VAL_REGION)
        return std::string("REGION");

    else if (v.type == VAL_ITEM)
        return std::string("ITEM");

    else if (v.type == VAL_FRIEND)
        return std::string("FRIEND");

    else if (v.type == VAL_ENEMY)
        return std::string("ENEMY");

    return std::string("INVALID");
}


/*************************************************
 *
 * Value expressions
 *
 *************************************************/

std::string build_player_select_expr(value& dynamic_val)
{
    std::string expr = std::string() +
	"clientnum == " + NMSTL::to_string(dynamic_val.player_index) +
	" and isbot == " + (dynamic_val.player_is_bot? "1" : "0");

    return expr;
}

std::string build_static_distance_expr(value& static_val)
{
    int static_x, static_y, static_z;
    if (static_val.type == VAL_ITEM)
    {
	static_x = (int)((float)static_val.item->x * DMF);
	static_y = (int)((float)static_val.item->y * DMF);
	static_z = S(static_x, static_y)->floor ;
    } else
    {
	static_x = (int)((float)sel.x * DMF);
	static_y = (int)((float)sel.y * DMF);
	static_z = selh;
    }

    std::string expr = std::string() +
	"sqrt((position_x-" + NMSTL::to_string(static_x) + ")*(position_x-" + NMSTL::to_string(static_x) + ") + " + 
	"(position_y-" + NMSTL::to_string(static_y) + ")*(position_y-" + NMSTL::to_string(static_y) + ") + " +
	"(position_z-" + NMSTL::to_string(static_z) + ")*(position_z-" + NMSTL::to_string(static_z) + "))";

    return expr;
}


std::string build_containment_expr(value& region_val)
{
    int f_x, f_y, f_xs, f_ys;
    f_x  = (int)((float)region_val.sel.x * DMF);
    f_y  = (int)((float)region_val.sel.y * DMF);
    f_xs = f_x + ((int)((float)region_val.sel.xs * DMF));
    f_ys = f_y + ((int)((float)region_val.sel.ys * DMF));

    std::string expr = "" +
	NMSTL::to_string(f_x) + " &lt;= position_x and position_x &lt;= " + NMSTL::to_string(f_xs) + " and " +
	NMSTL::to_string(f_y) + " &lt;= position_y and position_y &lt;= " + NMSTL::to_string(f_ys);

    return expr;
}

std::string build_team_expr(value& team_val)
{
    std::string r;
    if ( team_val.type == VAL_FRIEND )
    {
        r = "team_name == '" + team_val.player_team +
            "' and clientnum != " + NMSTL::to_string(team_val.player_index);
    }

    else if ( team_val.type == VAL_ENEMY )
    {
        r = "not(team_name == '" + team_val.player_team + "')";
    }

    return r;
}


/*************************************************
 *
 * Query history
 *
 *************************************************/

typedef std::map<std::string, std::vector<value> > query_value_map;
query_value_map query_values;

void add_query_participants(std::string query_name, std::vector<value>& participants)
{
    query_values[query_name] = participants;
}

void showquery(char* name)
{
    std::string query_name(name);
    query_value_map::iterator qv_it = query_values.find(query_name);
    if (qv_it != query_values.end())
    {
	std::vector<value>::iterator val_it = qv_it->second.begin();
	std::vector<value>::iterator val_end = qv_it->second.end();

	std::vector<int> items;
	std::vector<int> players;
	std::vector<int> bots;

	bool found_region;
	block region;
	int region_height;

	for (; val_it != val_end; ++val_it) {
	    switch(val_it->type)
	    {
            case VAL_ITEM:
                items.push_back(val_it->item_index);
                break;

            case VAL_PLAYER:
                if (val_it->player_is_bot)
                    bots.push_back(val_it->player_index);
                else
                    players.push_back(val_it->player_index);
                break;

            case VAL_REGION:
                found_region = true;
                region = val_it->sel;
                region_height = val_it->selh;
                break;

            case VAL_INVALID:
            default:
                break;
	    }
	}

	clear_selected_items();
	clear_selected_players();
	clear_selected_bots();
	clear_selected_region();

	select_items(items);
	select_players(players);
	select_bots(bots);

	if (found_region)
	    select_region(region, region_height);
    }    
}

COMMAND(showquery, ARG_1STR);

/*************************************************
 *
 * dynamic value selection
 *
 *************************************************/

query_handle
build_filter_query(std::string query_name, std::string id, value& v)
{
    query_handle r;
    std::string filter_expr;

    if ( v.type == VAL_PLAYER )
    {
        filter_expr = build_player_select_expr(v);
    }
    else if ( v.type == VAL_FRIEND
              || v.type == VAL_ENEMY )
    {
        filter_expr = build_team_expr(v);
    }
    else
    {
        printf("Invalid value type: %d\n", v.type);
        return r;
    }


    std::string filter_name = query_name + "-filter" + id;
    std::string deploy_ip = netstate.game_subs->deployment_addr();

    r.output_stream_name = filter_name + "-out";

    std::string query_xml = std::string() +
    "<box name=\"" + filter_name + "\" type=\"filter\" node=\"" + deploy_ip + "\">\n" +
    "  <in stream=\"player_positions\"/>\n" +
    "  <out stream=\"" + r.output_stream_name + "\"/>\n" +
    "  <parameter name=\"expression.0\" value=\"" + filter_expr + "\"/>\n" +
    "</box>\n";

    r.box_xmls.push_back(std::make_pair(filter_name, query_xml));
    r.tuple_size = sizeof(cube_position_tuple);
    r.handler = NULL;

    return r;
}


query_handle
build_filter_player_query(std::string query_name, std::string id, value& dynamic_v)
{
    assert(dynamic_v.type == VAL_PLAYER);
    return build_filter_query(query_name, id, dynamic_v);
}

query_handle
build_filter_team_query(std::string query_name, std::string id, value& team_v)
{
    assert(team_v.type == VAL_FRIEND || team_v.type == VAL_ENEMY);
    return build_filter_query(query_name, id, team_v);
}


/*************************************************
 *
 * contains 
 *
 *************************************************/

query_handle
build_contain_query(std::string query_name, value& region)
{
    assert(region.type == VAL_REGION);

    query_handle r;

    std::string contain_expr = build_containment_expr(region);
    std::string filter_name = query_name + std::string("-rfilter");

    std::string deploy_ip = netstate.game_subs->deployment_addr();

    r.output_stream_name = filter_name + std::string("-out");

    std::string query_xml = std::string() +
    "  <box name=\"" + filter_name + "\" type=\"filter\" node=\"" + deploy_ip + "\">\n" +
    "    <in  stream=\"player_positions\"/>\n" +
    "    <out stream=\"" + r.output_stream_name + "\"/>\n" +
    "    <parameter name=\"expression.0\" value=\"" + contain_expr + "\"/>\n" +
    "  </box>\n";

    r.box_xmls.push_back(std::make_pair(filter_name, query_xml));
    r.tuple_size = sizeof(cube_position_tuple);
    r.handler = NULL;
    return r;
}


/*************************************************
 *
 * distance
 *
 *************************************************/

query_handle
build_single_distance_query(std::string query_name,
			    std::string player_input_stream,
			    value& static_v)
{
    query_handle r;

    std::string compute_dist_expr = build_static_distance_expr(static_v);
    std::string map_name = query_name + "-map";

    std::string deploy_ip = netstate.game_subs->deployment_addr();

    r.output_stream_name = map_name + "-out";

    std::string query_xml = std::string() +
    "<box name=\"" + map_name + "\" type=\"map\" node=\"" + deploy_ip + "\">\n" +
    "  <in  stream=\"" + player_input_stream + "\"/>\n" +
    "  <out stream=\"" + r.output_stream_name + "\"/>\n" +
    "  <parameter name=\"output-field-name.0\" value=\"dist\"/>\n" +
    "  <parameter name=\"expression.0\"        value=\"" + compute_dist_expr + "\"/>\n" +
    "</box>\n";

    r.box_xmls.push_back(std::make_pair(map_name, query_xml));
    r.tuple_size = sizeof(borealis_tuple) + sizeof(double);
    r.handler = handle_distance;
    return r;
}


query_handle
build_pair_distance_query(std::string query_name, value& v1, value& v2)
{
    assert(v1.type == VAL_PLAYER || v2.type == VAL_PLAYER);

    query_handle q1 =
        ( is_team(v1) ?
          build_filter_team_query(query_name, "1", v1) :
          build_filter_player_query(query_name, "1", v1) );

    query_handle q2 =
        ( is_team(v2) ?
          build_filter_team_query(query_name, "2", v2) :
          build_filter_player_query(query_name, "2", v2) );

    query_handle r;

    std::string deploy_ip = netstate.game_subs->deployment_addr();

    std::string join_name = query_name + "join";
    std::string join_out_name = join_name + "-out";
    std::string join_xml = std::string() +
	"<box name=\"" + join_name + "\" type=\"join\" node=\"" + deploy_ip + "\">\n" +
	"  <in  stream=\"" + q1.output_stream_name + "\"/>\n" +
	"  <in  stream=\"" + q2.output_stream_name + "\"/>\n"+ 
	"  <out stream=\"" + join_out_name + "\"/>\n" +
        // Band join predicate
	//"  <parameter name=\"predicate\"             value=\"abs(left.seq_no - right.seq_no) &lt; 10\"/>\n" +
	"  <parameter name=\"predicate\"             value=\"true\"/>\n" +
	"  <parameter name=\"right-buffer-size\"     value=\"1\"/>\n" +
	"  <parameter name=\"right-order-by\"        value=\"VALUES\"/>\n" +
	"  <parameter name=\"right-order-on-field\"  value=\"seq_no\"/>\n" +
	"  <parameter name=\"left-buffer-size\"      value=\"1\"/>\n" +
	"  <parameter name=\"left-order-by\"         value=\"VALUES\"/>\n" +
	"  <parameter name=\"left-order-on-field\"   value=\"seq_no\"/>\n" +
	"  <parameter name=\"out-field-name.0\"      value=\"dist\"/>\n" +
	"  <parameter name=\"out-field.0\"           value=\"" +
	"sqrt((left.position_x-right.position_x)*(left.position_x-right.position_x)+" +
	"(left.position_y-right.position_y)*(left.position_y-right.position_y)+" +
	"(left.position_z-right.position_z)*(left.position_z-right.position_z))" +
	"\"/>\n" +
	"</box>\n";


    copy(q1.box_xmls.begin(), q1.box_xmls.end(), back_inserter(r.box_xmls));
    copy(q2.box_xmls.begin(), q2.box_xmls.end(), back_inserter(r.box_xmls));
    r.box_xmls.push_back(std::make_pair(join_name, join_xml));
    r.output_stream_name = join_out_name;
    r.tuple_size = sizeof(borealis_tuple) + sizeof(double);
    r.handler = handle_distance;
    return r;
}



query_handle build_distance_query(std::string query_name, value& v1, value& v2)
{
    assert(v1.type == VAL_PLAYER || v2.type == VAL_PLAYER);

    query_handle result;

    if ( is_static(v1) || is_static(v2) ) {
	// Create filter, map query.
	value static_v = is_static(v1)? v1 : v2;
	value dynamic_v = is_static(v1)? v2: v1;

	query_handle player_q = 
            ( is_team(dynamic_v)? 
              build_filter_team_query(query_name, std::string(), dynamic_v)
              : build_filter_player_query(query_name, std::string(), dynamic_v) );

	query_handle dist_q = build_single_distance_query(query_name, player_q.output_stream_name, static_v);

    copy(player_q.box_xmls.begin(), player_q.box_xmls.end(), back_inserter(result.box_xmls));
    copy(dist_q.box_xmls.begin(), dist_q.box_xmls.end(), back_inserter(result.box_xmls));
	result.output_stream_name = dist_q.output_stream_name;
	result.tuple_size = dist_q.tuple_size;
	result.handler = handle_distance;

    } else {
	result = build_pair_distance_query(query_name, v1, v2);
    }

    return result;
}


/*************************************************
 *
 * Aggregates
 *
 *************************************************/

handler_fn handle_aggregate(std::string aggregate)
{
    if (aggregate == "count" || aggregate == "count_distinct")

	return handle_count;

    else if (aggregate == "sum" || aggregate == "avg"
	     || aggregate == "max" || aggregate == "min")

	return handle_distance;

    return NULL;

}

query_handle
build_aggregate_query(std::string query_name, 
		      std::string aggregate_fn, std::string aggregate_field,
		      query_handle& op, size_t aggregate_fn_size)
{
    std::string deploy_ip = netstate.game_subs->deployment_addr();

    std::string aggregate_name = query_name + aggregate_fn;
    std::string output_stream_name = aggregate_name + std::string("-out");
    std::string query_xml = std::string() +
	"  <box name=\"" + aggregate_name + "\" type=\"aggregate\" node=\"" + deploy_ip + "\">\n" +
	"    <in  stream=\"" + op.output_stream_name + "\"/>\n" +
	"    <out stream=\"" + output_stream_name + "\"/>\n" +
	"    <parameter name=\"aggregate-function.0\"              value=\"" + aggregate_fn + "(" + aggregate_field + ")\"/>\n" +
    "    <parameter name=\"aggregate-function-output-name.0\"  value=\"" + aggregate_field + "\"/>\n" +
	"    <parameter name=\"window-size-by\"                    value=\"VALUES\"/>\n" +
    "    <parameter name=\"window-size\"                       value=\"40\"/>\n" +
	"    <parameter name=\"advance\"                           value=\"1\"/>\n" +
    "    <parameter name=\"order-by\"                          value=\"FIELD\"/>\n" +
    "    <parameter name=\"order-on-field\"                    value=\"seq_no\"/>\n" +
    "    <parameter name=\"timeout\"                           value=\"10.0\"/>\n" +
	"  </box>\n";

    query_handle r;
    copy(op.box_xmls.begin(), op.box_xmls.end(), back_inserter(r.box_xmls));
    r.box_xmls.push_back(std::make_pair(aggregate_name, query_xml));
    r.output_stream_name = output_stream_name;
    r.tuple_size = sizeof(borealis_tuple) + aggregate_fn_size + sizeof(int); // added int for seqno.
    r.handler = handle_aggregate(aggregate_fn);
    return r;
}


query_handle
build_count_query(std::string query_name, query_handle& op)
{
    return build_aggregate_query(query_name, std::string("count"), std::string(), op,
				 sizeof(int));
}

query_handle
build_count_distinct_query(std::string query_name, query_handle& op)
{
    return build_aggregate_query(query_name, std::string("count_distinct"), std::string("clientnum"), op,
				 sizeof(int));
}

query_handle
build_max_query(std::string query_name, query_handle& op)
{
    return build_aggregate_query(query_name, std::string("max"), std::string("dist"), op,
				 sizeof(double));
}

query_handle
build_min_query(std::string query_name, query_handle& op)
{
    return build_aggregate_query(query_name, std::string("min"), std::string("dist"), op,
				 sizeof(double));
}

query_handle
build_avg_query(std::string query_name, query_handle& op)
{
    return build_aggregate_query(query_name, std::string("avg"), std::string("dist"), op,
				 sizeof(double));
}


/*************************************************
 *
 * distance <name> <value> <value>
 *
 ************************************************/

void distance_query(char* name, char* ent1, char* ent2)
{
    value v1 = parse_value(std::string(ent1));
    value v2 = parse_value(std::string(ent2));

    if ( v1.type != VAL_INVALID && v2.type == VAL_INVALID ) {
        v2 = parse_rvalue(std::string(ent2), v1);
    }

    if ( v1.type == VAL_INVALID || v2.type == VAL_INVALID ) {
        conoutf("Invalid entities for distance query:");
        conoutf("  %s type: %s", (int) ent1, (int) type_to_string(v1).c_str());
        conoutf("  %s type: %s", (int) ent2, (int) type_to_string(v2).c_str());
        return;
    }

    std::string query_name(name);

    query_handle r = build_distance_query(query_name, v1, v2);

    bool added = netstate.game_subs->deploy_query(query_name, r.box_xmls,
                        r.output_stream_name, handle_distance, r.tuple_size);

    if ( !added ) {
        using namespace NMSTL;
        DEBUG << "Failed to add " << query_name;
        return;
    }

    std::vector<value> participants;
    participants.push_back(v1);
    participants.push_back(v2);
    add_query_participants(query_name, participants);
}

COMMANDN(distance, distance_query, ARG_3STR);




/*************************************************
 *
 * count <name>
 *
 *************************************************/

void count_query(char* query_name)
{
    if (!selset) {
	conoutf("No region selected");
	return;
    }

    value region_val;
    region_val.type = VAL_REGION;
    region_val.sel = sel;
    region_val.selh = selh;

    query_handle contains_q = build_contain_query(query_name, region_val);
    query_handle count_q = build_count_distinct_query(query_name, contains_q);

    bool added = netstate.game_subs->deploy_query(query_name, count_q.box_xmls,
            count_q.output_stream_name, handle_count, count_q.tuple_size);

    if ( !added ) {
        using namespace NMSTL;
        DEBUG << "Failed to add " << query_name;
        return;
    }

    std::vector<value> participants;
    participants.push_back(region_val);
    add_query_participants(query_name, participants);
}

COMMANDN(count, count_query, ARG_1STR);



/*************************************************
 *
 * query <name> <query>
 *
 *************************************************/

/*************************************************
 * Game expression grammar:
 *
 * svalue := region | item
 *
 * dvalue := player | bot
 *
 * value := dvalue | svalue
 *
 * rvalue := friend | enemy
 *
 * op :=
 *     distance <dvalue> <value>
 *   | distance <dvalue> <rvalue>
 *   | contents region
 *
 * field := clientnum | dist
 *
 * aggregate := 
 *     sum <field> <op>
 *   | min <field> <op>
 *   | max <field> <op>
 *   | count <field> <op>
 *   | count_distinct <field> <op>
 *   | avg <field> <op>
 *
 * expr := aggregate | op
 *
 **************************************************/
typedef std::vector<std::string> tokens;
typedef std::vector<std::string>::iterator token_iterator;
token_iterator last_token;
std::string current_query_name;

std::string next_token(token_iterator& token_it)
{
    assert(token_it != last_token);
    return *token_it++;
}

bool match_aggregate(token_iterator& token_it)
{
    std::string aggregate = *token_it;
    return ( aggregate == "count"
	     || aggregate == "count_distinct"
	     || aggregate == "sum"
	     || aggregate == "avg"
	     || aggregate == "min"
	     || aggregate == "max" );
}

bool validate_aggregate(std::string& aggregate, std::string& field)
{
    return ( (aggregate == "count" && field.empty())
	     || (aggregate == "count_distinct" && field == "clientnum")
	     || ((aggregate == "max" || aggregate == "min" || aggregate == "avg") && field == "dist") );
}

bool match_op(token_iterator& token_it)
{
    std::string op = *token_it;
    return ( op == "distance"
	     || op == "contents" );
}

size_t get_aggregate_size(std::string aggregate)
{
    size_t r;

    if (aggregate == "count" || aggregate == "count_distinct")

	r = sizeof(int);

    else if (aggregate == "sum" || aggregate == "avg"
	     || aggregate == "max" || aggregate == "min")

	r = sizeof(double);

    else

	r = sizeof(int);

    return r;
}

query_handle parse_op(token_iterator& token_it)
{
    query_handle r;

    std::string op = next_token(token_it);
    if (op == "distance")
    {
        std::string v1_s = next_token(token_it);
        std::string v2_s = next_token(token_it);

        value v1 = parse_dvalue(v1_s);
        value v2 = parse_value(v2_s);

        if ( v1.type != VAL_INVALID && v2.type == VAL_INVALID )
            v2 = parse_rvalue(v2_s, v1);

        if ( v1.type == VAL_INVALID || v2.type == VAL_INVALID ) {
            conoutf("Invalid entities for distance query.");
            conoutf("  %s type: %s", (int) v1_s.c_str(), (int) type_to_string(v1).c_str());
            conoutf("  %s type: %s", (int) v2_s.c_str(), (int) type_to_string(v2).c_str());
            return r;
        }            

        r = build_distance_query(current_query_name, v1, v2);
    }
    else if (op == "contents") {
	value v1 = parse_value(next_token(token_it));

	r = build_contain_query(current_query_name, v1);
    }
    else
	conoutf("Invalid operator %s", (int) op.c_str());

    return r;
}

query_handle parse_aggregate(token_iterator& token_it)
{
    query_handle r;

    std::string aggregate = next_token(token_it);
    std::string field = next_token(token_it);

    if (validate_aggregate(aggregate, field)) {
	query_handle op = parse_op(token_it);
	r = build_aggregate_query(current_query_name, aggregate, field, op,
				  sizeof(borealis_tuple) + get_aggregate_size(aggregate));
    }

    return r;
}

query_handle parse_expr(token_iterator& token_it)
{
    query_handle r;

    if (match_aggregate(token_it))

	r = parse_aggregate(token_it);

    else if (match_op(token_it))

	r = parse_op(token_it);

    return r;
}

tokens create_tokens(char* query_str)
{
    char* tok = strtok(query_str, " \t");
    tokens t;
    while (tok) {
	t.push_back(std::string(tok));
	tok = strtok(NULL, " \t");
    }

    return t;
}

void query(char* name, char* query)
{
    std::string query_name(name);
    tokens toks = create_tokens(query);

    // Set up parsing state.
    current_query_name = query_name;
    last_token = toks.end();

    token_iterator first_token = toks.begin();
    query_handle q = parse_expr(first_token);

    if( q.box_xmls.size() == 0 ) {
    	conoutf("Failed to parse query %s", (int)query);
    	return;
    }
    
    bool added = netstate.game_subs->deploy_query(query_name, q.box_xmls,
                            q.output_stream_name, q.handler, q.tuple_size);

    if ( !added ) {
        using namespace NMSTL;
        DEBUG << "Failed to add " << query_name;
    }
}

COMMAND(query, ARG_2STR);

void mypos()
{
    { using namespace NMSTL;
        DEBUG << "Position: " << player1->o.x << ", "
            << player1->o.y << ", " << player1->o.z;

        DEBUG << "Position DMF: " << player1->o.x*DMF << ", "
            << player1->o.y*DMF << ", " << player1->o.z*DMF;
    }
}

COMMAND(mypos, ARG_NONE);


/**************************************************
 * Automatic query generation.
 **************************************************/
extern int selectpos(int x, int xs, int y, int ys);

int num_region_queries = 0;

void build_count_region_queries(int num_queries)
{
    int max_pos = ssize-(2*MINBORD);
    if ( max_pos == 0 )
       max_pos = 1;

    for (int i = 1; i <= num_queries; ++i)
    {
        int x = MINBORD + (rand() % max_pos);
        int xs = x + (x <= max_pos? 1 : (rand() % (max_pos - x)));
        int y = MINBORD + (rand() % max_pos);
        int ys = y + (y <= max_pos? 1 : (rand() % (max_pos-y)));

        selectpos(x, xs, y, ys);
        std::string query_name = "count-" + NMSTL::to_string(i + num_region_queries);
        count_query((char*)query_name.c_str());
    }

    num_region_queries += num_queries;
    clear_selected_region();
}

COMMANDN(gencount, build_count_region_queries, ARG_1INT);

int num_distance_queries = 0;

void build_distance_queries(int num_queries)
{
    int valid_bots = 0;
    loopv(bots) {
        if (bots[i]) valid_bots++;
    }

    int queries_to_create = min(num_queries, valid_bots);
    int queries_created = 0;
    unsigned int last_bot = 0;

    std::string value_str = "BOT1";
    std::string rvalue_str = "FRIEND";

    for (unsigned int i = 0; i < queries_to_create; ++i)
    {
        for (unsigned int j = last_bot + 1; j < bots.length(); ++j) {
            if (bots[j]) {
                std::vector<int> selection;
                selection.push_back(j);
                select_bots(selection);
                conoutf("Selecting bot %d", j);
                int bot_index = find_selected_bot(1);
                conoutf("Verifying bot selection with %d", bot_index);

                //std::string query_name = std::string(bots[j]->name, 5) + "-frnd"; 
                std::string query_name = std::string("distance-") + NMSTL::to_string(num_distance_queries);

                transform( query_name.begin(), query_name.end(),
                           query_name.begin(), (int(*)(int))tolower );

                if (!netstate.game_subs->find_query(query_name))
                {
                    distance_query( (char*) query_name.c_str(),
                                    (char*) value_str.c_str(),
                                    (char*) rvalue_str.c_str());
                    last_bot = j;
                    ++queries_created;
                    ++num_distance_queries;
                    break;
                }
            }
        }
    }

    conoutf("Created %d distance queries", queries_created);

    clear_selected_bots();
}

COMMANDN(gendistance, build_distance_queries, ARG_1INT);

#endif
