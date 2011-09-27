#ifndef  WRITEREGION_H
#define  WRITEREGION_H

#include  "RegionalCatalog.h"

BOREALIS_NAMESPACE_BEGIN

class WriteRegion
{
  public:

    ///  Dump a regional catalog to a file as Xml.
    ///
    static void  write_region(RegionalCatalog  &Catalog,
                                       string   file);


    ///  Format a regional catalog to a string as Xml.
    ///
    static string  form_region(RegionalCatalog  &Catalog);


  private:

    ///  Format schemas in a regional catalog as an Xml string.
    ///
    static string  form_schema(RegionalCatalog  &catalog);


    ///  Format streams in a regional catalog as an Xml string.
    ///
    static string  form_stream(RegionalCatalog  &Catalog);


    /// Format subscriptions to outputs in a regional catalog as an Xml string.
    ///
    static string  form_subscribe(RegionalCatalog  &Catalog);


    ///  Get all queries in a regional catalog as an Xml string.
    ///
    static string  form_query(RegionalCatalog  &Catalog);


    ///  Format connection point views in a regional catalog as an Xml string.
    ///
    static string  form_view(RegionalCatalog  &Catalog);
};


BOREALIS_NAMESPACE_END

#endif                        // WRITEREGION_H
