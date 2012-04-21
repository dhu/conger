/**
 * CongerDiagram.h
 *
 *  Created on: Apr 17, 2012
 *      Author: hellojinjie
 */

#ifndef CONGERDIAGRAM_H_
#define CONGERDIAGRAM_H_

#include <NMSTL/ptr>

#include "Diagram.h"
#include "util.h"

BOREALIS_NAMESPACE_BEGIN

class CongerDiagram : public Diagram
{
public:
    CongerDiagram();
    virtual ~CongerDiagram();

public:
    /// add a single schema to catalog
    ///
    CatalogSchema* add_conger_schema(string schema_name,
            map<string, string> schema_fields);

    /// add a single stream to catalog
    ///
    CatalogStream* add_conger_stream(string stream_name, string schema_name);

    /// add a cql query to catalog, first convert cql query to aurora boxes
    ///
    void add_conger_query(string query_name, map<string, string> query_parameters);

    CatalogBox* add_conger_box(string box_name, map<string, string> box_parameters);

    CatalogSubscription* add_conger_subsribe();
};

BOREALIS_NAMESPACE_END
#endif /* CONGERDIAGRAM_H_ */
