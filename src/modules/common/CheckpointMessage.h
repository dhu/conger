#ifndef CHECKPOINT_MESSAGE_H
#define CHECKPOINT_MESSAGE_H

#include "HAUnit.h"

BOREALIS_NAMESPACE_BEGIN;

class CheckpointMessage
{
  protected:

    string _primary_server;
    
    ptr<HAUnit> _ha_unit;
    
    vector<string> _xml_strings;
    
    ptr<BoxPackage> _box_package;
  
  public:
    
    CheckpointMessage();
    
    CheckpointMessage(string primary_server, ptr<HAUnit> ha_unit);
    
    string get_primary_server();
    
    ptr<HAUnit> get_ha_unit();
    
    void add(string xml_string);
    
    vector<string> get_xml_strings();
    
    ptr<BoxPackage> get_box_package();
    
    string as_string() const;
    
    MedusaID get_backup_server() {return _ha_unit->get_backup_server();};
    
    NMSTL_SIMPLY_SERIALIZABLE(CheckpointMessage, << _primary_server << _ha_unit  << _xml_strings << _box_package );

};

BOREALIS_NAMESPACE_END;

#endif // CHECKPOINT_MESSAGE_H
