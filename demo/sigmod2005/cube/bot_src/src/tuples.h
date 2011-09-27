#ifndef TUPLES_H
#define TUPLES_H

#include <time.h>
#include <sys/time.h>

// network messages codes, c2s, c2c, s2c

// Notes:
//   SV_BOTDIS -> SV_CDIS : merged with the use of "isbot" field.
//
//   SV_BOTDIED -> SV_DIED : merged with the use of "client_is_bot"
//                           and "killed_by_bot" fields.
//
//   SV_CLIENT2BOTDMG, SV_BOT2BOTDMG -> SV_DAMAGE :
//     merged with the use of "client_isbot" and "target_isbot" fields.
//
//   SV_BOTFRAGS -> SV_FRAGS : merged with the use of "isbot" field.
//
//   SV_BOTUPDATE -> SV_POS : merged with the use of "isbot" field.
//
//   SV_BOTSOUND -> SV_SOUND : merged with the use of "isbot" field.
//

enum
    {
        SV_INITS2C = 0, SV_INITC2S, SV_POS, SV_TEXT, SV_SOUND, SV_BOTSOUND, SV_CDIS,
        SV_BOTDIS, SV_DIED, SV_BOTDIED, SV_DAMAGE, SV_CLIENT2BOTDMG, SV_BOT2BOTDMG,
        SV_SHOT, SV_FRAGS, SV_BOTFRAGS, SV_TIMEUP, SV_EDITENT, SV_MAPRELOAD, SV_ITEMACC,
        SV_MAPCHANGE, SV_ITEMSPAWN, SV_ITEMPICKUP, SV_DENIED,
        SV_PING, SV_PONG, SV_CLIENTPING, SV_GAMEMODE,
        SV_EDITH, SV_EDITT, SV_EDITS, SV_EDITD, SV_EDITE,
        SV_SENDMAP, SV_RECVMAP, SV_SERVMSG, SV_ITEMLIST,
        SV_ADDBOT, SV_BOTUPDATE, SV_EXT,
    };     




/**
 * Wrapping in a struct to force usage of the kind: TupleType::INSERTION rather
 * INSERTION alone.
 */
struct TupleType
{

public:

    typedef enum
    {

        // Standard types
        INSERTION = 0,
        DELETION,
        REPLACEMENT,
        HISTORICAL,

        // Magda's types used for fault-tolerance
        TENTATIVE,  // Tentative version of INSERTION (tentative deletions/replacements not supported)
        BOUNDARY,   // Tuple acting both as punctuation and hearbeat
        UNDO,       // Tuple indicating that state should be rolled back to undo effects of previous tuples 
        UP_FAILURE, // SUnion signals that it enters failure state
        RECONCILIATION_REQUEST, // SUnion signals that it can now reconcile its state
        RECONCILIATION_DONE,    // SUnion signals that it reconciled its state and caught up with input
        UNDO_REDO_START,        // ConsistencyMngr requests that SUnions reconciles by propagating an UNDO
        // End of Magda's types used for fault-tolerance

        DELIMITER,    //for revisions
        REQUEST,
        NOTIFICATION,
        
        // Default
        UNKNOWN

    } Type;

private:
    Type _type;

public:

    TupleType(Type t = UNKNOWN) : _type(t) {}

    bool operator==(const TupleType& t) const
    {
        return (t._type == _type);
    }

    bool operator!=(const TupleType& t) const
    {
        return (t._type != _type);
    }

};

struct TupleHeader
{

    timeval     tv;          // 0..7 
    timeval     local_tv;    // 8..15
    long        tuple_id;    // 16..19
    TupleType   tuple_type;  // 20..23
    long        rev_id;      // 24..27
    double      quality;     // 28..35
    long        win_spec;    // 36..39
    bool        fake_flag;   // 40
    long        rev_field;   // 41..44
    timeval     tuple_stime; // 45..52 Magda: Timestamp value used for tuple serialization
    // If you add fields here, you must update the HEADER_SIZE constant defined at the beginning of the file

    //  int32       revised_val;// 43..46

    TupleHeader()
    {

        static int counter = 0; // for system-wide unique input tuple id

        tv.tv_sec = 0;  // Aurora will timestamp it (at first node only)
        tv.tv_usec = 0; // Aurora will timestamp it (at first node only)
        local_tv.tv_sec = 0;  // Aurora will timestamp it (at any node)
        local_tv.tv_usec = 0; // Aurora will timestamp it (at any node)
        tuple_id = ++counter;
        tuple_type = TupleType::INSERTION;
        rev_id = -1;
        quality = 0;
        win_spec = -1;
        fake_flag = false;
        rev_field = -1;
        tuple_stime.tv_sec = 0;
        tuple_stime.tv_usec = 0;

        //  revised_val = -1;
    }

} __attribute__((__packed__));


typedef TupleHeader borealis_tuple;

struct cube_init_tuple : borealis_tuple {
    int       clientnum;
    char    source_ip[16];
    char    player_name[32];
    char    team_name[32];
    int       lifesequence;
    char    toservermap[128];
    int       nextmode;
} __attribute__((__packed__));


struct cube_disconnect_tuple : borealis_tuple {
    int    clientnum;
    int    isbot;
} __attribute__((__packed__));


struct cube_position_tuple : borealis_tuple {
    int      clientnum;
    char     team_name[32];
    float    position_x;
    float    position_y;
    float    position_z;
    float    yaw;
    float    pitch;
    float    roll;
    float    velocity_x;
    float    velocity_y;
    float    velocity_z;
    int      rest_info;
    int      isbot;
    int      seq_no;
} __attribute__((__packed__));


struct cube_inventory_item_tuple : borealis_tuple {
    int   clientnum;
    int   item_index;
    int   total_items;
} __attribute__((__packed__));


struct cube_item_pickup_tuple : borealis_tuple {
    int           clientnum;
    int           item_index;
    double        spawn_secs;
    int           client_pickup;
} __attribute__((__packed__));


struct cube_item_acc_tuple : borealis_tuple {
    int    clientnum;
    int    item_index;
} __attribute__((__packed__));


struct cube_item_spawn_tuple : borealis_tuple {
    int    clientnum;
    int    item_index;
} __attribute__((__packed__));


struct cube_map_change_tuple : borealis_tuple {
    int    clientnum;
    char toservermap[128];
    int    nextmode;
} __attribute__((__packed__));


struct cube_map_reload_tuple : borealis_tuple {
    int    clientnum;
} __attribute__((__packed__));


struct cube_mode_tuple : borealis_tuple {
    int    clientnum;
    int    nextmode;
} __attribute__((__packed__));


struct cube_timeup_tuple : borealis_tuple {
    int    time_remain;
} __attribute__((__packed__));


struct cube_shot_tuple : borealis_tuple {
    int       clientnum;
    int       gun_number;
    float    from_x;
    float    from_y;
    float    from_z;
    float    to_x;
    float    to_y;
    float    to_z;
} __attribute__((__packed__));


struct cube_damage_tuple : borealis_tuple {
    int    clientnum;
    int    target;
    int    damage;
    int    lifesequence;
    int    client_isbot;
    int    target_isbot;
} __attribute__((__packed__));


struct cube_frags_tuple : borealis_tuple {
    int    clientnum;
    int    frags;
    int    isbot;
} __attribute__((__packed__));


struct cube_death_tuple : borealis_tuple {
    int    clientnum;
    int    client_isbot;
    int    actor;
    int    actor_isbot;
} __attribute__((__packed__));


struct cube_sound_tuple : borealis_tuple {
    int    clientnum;
    int    playsound_n;
    int    isbot;
} __attribute__((__packed__));


struct cube_chat_tuple : borealis_tuple {
    int       clientnum;    
    char    chat_text[1024];
} __attribute__((__packed__));


struct cube_servmsg_tuple : borealis_tuple {
    int    text[1024];
} __attribute__((__packed__));


struct cube_ping_tuple : borealis_tuple {
    int    clientnum;
    int    ping_type;
    int    lastmillis;
} __attribute__((__packed__));


struct cube_edit_tuple : borealis_tuple {
    int    edit_type;
    int    amount;
    int    lasttex;
    int    selection_x;
    int    selection_y;
    int    selection_xs;
    int    selection_ys;
    int    selection_type;
} __attribute__((__packed__));


struct cube_edit_ent_tuple : borealis_tuple {
    int    entity_length;
    int    entity_type;
    int    entity_x;
    int    entity_y;
    int    entity_z;
    int    entity_attribute1;
    int    entity_attribute2;
    int    entity_attribute3;
    int    message;
} __attribute__((__packed__));


struct cube_add_bot_tuple : borealis_tuple {
    int     host_clientnum;
    int     clientnum;
    char    player_name[32];
    char    team_name[32];
    int     lifesequence;
} __attribute__((__packed__));


struct cube_bot_init_tuple : borealis_tuple {
    int     host_clientnum;
    int     clientnum;
    int     pending_clientnum;
} __attribute__((__packed__));
#endif
