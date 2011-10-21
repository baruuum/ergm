#ifndef WTCHANGESTATS_H
#define WTCHANGESTATS_H

#include "wtedgetree.h"
#include "wtchangestat.h"

/********************  changestats:   A    ***********/
WtD_CHANGESTAT_FN(d_atleast);

/********************  changestats:   C    ***********/
WtD_CHANGESTAT_FN(d_cyclicalweighs_max); WtS_CHANGESTAT_FN(s_cyclicalweights_max);
WtD_CHANGESTAT_FN(d_cyclicalweighs_sum); WtS_CHANGESTAT_FN(s_cyclicalweights_sum);
WtD_CHANGESTAT_FN(d_cyclicalweights_threshold); WtS_CHANGESTAT_FN(s_cyclicalweights_threshold);

/********************  changestats:   E    ***********/
WtD_CHANGESTAT_FN(d_edgecov_nonzero); WtD_CHANGESTAT_FN(d_edgecov_sum);

/********************  changestats:   G    ***********/
WtD_CHANGESTAT_FN(d_greaterthan);

/********************  changestats:   I    ***********/
WtD_CHANGESTAT_FN(d_ininterval);

/********************  changestats:   M    ***********/
WtD_CHANGESTAT_FN(d_mutual_wt_product);
WtD_CHANGESTAT_FN(d_mutual_wt_geom_mean);
WtD_CHANGESTAT_FN(d_mutual_wt_min); 
WtD_CHANGESTAT_FN(d_mutual_wt_nabsdiff);
WtD_CHANGESTAT_FN(d_mutual_wt_threshold);

/********************  changestats:   N    ***********/
WtD_CHANGESTAT_FN(d_nodecov_nonzero);
WtD_CHANGESTAT_FN(d_nodecov_sum);
WtD_CHANGESTAT_FN(d_nodefactor_nonzero);
WtD_CHANGESTAT_FN(d_nodefactor_sum);
WtD_CHANGESTAT_FN(d_nodeifactor_nonzero);
WtD_CHANGESTAT_FN(d_nodeifactor_sum);
WtD_CHANGESTAT_FN(d_nodeofactor_nonzero);
WtD_CHANGESTAT_FN(d_nodeofactor_sum);
WtD_CHANGESTAT_FN(d_nonzero);
WtD_CHANGESTAT_FN(d_nsumlogfactorial);

/********************  changestats:   S    ***********/
WtD_CHANGESTAT_FN(d_sum);
WtD_CHANGESTAT_FN(d_sum_pow);

/********************  changestats:   T    ***********/
WtD_CHANGESTAT_FN(d_transitiveweights_threshold); WtS_CHANGESTAT_FN(s_transitiveweights_threshold);
WtD_CHANGESTAT_FN(d_transitiveweighs_max); WtS_CHANGESTAT_FN(s_transitiveweights_max);
WtD_CHANGESTAT_FN(d_transitiveweighs_sum); WtS_CHANGESTAT_FN(s_transitiveweights_sum);

              
#endif
