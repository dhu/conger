#include "StatsMgr.h"

StatsMgr init_sm()
{
    StatsMgr::StatsMgr  sm;
    IDVect    ids;

    ids.push_back( "o1" );
    ids.push_back( "o2" );
    ids.push_back( "o3" );
    sm.add_tsstats( "s1", 4, 1, ids );

    ids.clear();

    ids.push_back( "b1" );
    ids.push_back( "b2" );
    ids.push_back( "b3" );
    sm.add_tsstats( "s2", 5, 2, ids );

    Statistics  &st = sm.get_stats();

    st.get_tsstats( "s1" ).time_stamp = 1;
    st.get_statistics( "s1", "o1" ).add_sample(1);
    st.get_statistics( "s1", "o2" ).add_sample(2);
    st.get_statistics( "s1", "o3" ).add_sample(3);

    st.get_tsstats( "s1" ).time_stamp = 2;
    st.get_statistics( "s1", "o1" ).add_sample(2);
    st.get_statistics( "s1", "o2" ).add_sample(3);
    st.get_statistics( "s1", "o3" ).add_sample(4);

    st.get_tsstats( "s1" ).time_stamp = 3;
    st.get_statistics( "s1", "o1" ).add_sample(5);
    st.get_statistics( "s1", "o2" ).add_sample(6);
    st.get_statistics( "s1", "o3" ).add_sample(7);

    st.get_tsstats( "s1" ).time_stamp = 4;
    st.get_statistics( "s1", "o1" ).add_sample(7);
    st.get_statistics( "s1", "o2" ).add_sample(8);
    st.get_statistics( "s1", "o3" ).add_sample(9);

    st.get_tsstats( "s1" ).time_stamp = 5;
    st.get_statistics( "s1", "o1" ).add_sample(9);
    st.get_statistics( "s1", "o2" ).add_sample(8);
    st.get_statistics( "s1", "o3" ).add_sample(9);

    st.get_tsstats("s2" ).time_stamp = 1;
    st.get_statistics("s2", "b1" ).add_sample(10);
    st.get_statistics("s2", "b2" ).add_sample(20);
    st.get_statistics("s2", "b3" ).add_sample(30);

    st.get_tsstats( "s2" ).time_stamp = 2;
    st.get_statistics( "s2", "b1" ).add_sample(20);
    st.get_statistics( "s2", "b2" ).add_sample(30);
    st.get_statistics( "s2", "b3" ).add_sample(40);

    st.get_tsstats( "s2" ).time_stamp = 3;
    st.get_statistics( "s2", "b1" ).add_sample(50);
    st.get_statistics( "s2", "b2" ).add_sample(60);
    st.get_statistics( "s2", "b3" ).add_sample(70);

    st.get_tsstats( "s2" ).time_stamp = 4;
    st.get_statistics( "s2", "b1" ).add_sample(70);
    st.get_statistics( "s2", "b2" ).add_sample(80);
    st.get_statistics( "s2", "b3" ).add_sample(90);

    st.get_tsstats( "s2" ).time_stamp = 5;
    st.get_statistics( "s2", "b1" ).add_sample(60);
    st.get_statistics( "s2", "b2" ).add_sample(90);
    st.get_statistics( "s2", "b3" ).add_sample(70);

    return( sm );
}


int  main( int  argc, char  **argv )
{
    StatsMgr::StatsMgr  sm = init_sm();
//    cout << sm; // this is currently not yet supported
}
