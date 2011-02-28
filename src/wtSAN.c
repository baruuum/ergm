#include "wtSAN.h"


/*****************
 Note on undirected networks:  For j<k, edge {j,k} should be stored
 as (j,k) rather than (k,j).  In other words, only directed networks
 should have (k,j) with k>j.
*****************/

/*****************
 void WtSAN_wrapper

 Wrapper for a call from R.
*****************/
void WtSAN_wrapper (int *dnumnets, int *nedges,
		    int *heads, int *tails, double *weights, 
		    int *maxpossibleedges,
		    int *dn, int *dflag, int *bipartite, 
		    int *nterms, char **funnames,
		    char **sonames, 
		    char **MHproposaltype, char **MHproposalpackage,
		    double *inputs, double *theta0, double *tau, 
		    int *samplesize, 
		    double *sample, int *burnin, int *interval,  
		    int *newnetworkheads, 
		    int *newnetworktails, 
		    double *newnetworkweights,
		    double *invcov, 
		    int *fVerbose, 
		    int *maxedges)  {
  int directed_flag;
  Vertex n_nodes, nmax, bip;
  Edge n_networks;
  WtNetwork nw[2];
  WtModel *m;
  WtMHproposal MH;
  
  n_nodes = (Vertex)*dn; /* coerce double *dn to type Vertex */
  n_networks = (Edge)*dnumnets; 
  nmax = (Edge)*maxedges; 
  bip = (Vertex)*bipartite; /* coerce double *bipartite to type Vertex */
  
  GetRNGstate();  /* R function enabling uniform RNG */
  
  directed_flag = *dflag;


  m=WtModelInitialize(*funnames, *sonames, &inputs, *nterms);

  /* Form the missing network */
  nw[0]=WtNetworkInitialize(heads, tails, weights, nedges[0],
			    n_nodes, directed_flag, bip, 0);
  /* Form the missing network */
  if (nedges[1]>0) {
   heads += nedges[0];
   tails += nedges[0];
   weights += nedges[0];
   nw[1]=WtNetworkInitialize(heads, tails, weights, nedges[1],
                           n_nodes, directed_flag, bip, 0);
   heads -= nedges[0];
   tails -= nedges[0];
   weights -= nedges[0];
  }

  if (fVerbose)
    Rprintf("Total m->n_stats is %i; total samplesize is %d\n",
             m->n_stats,samplesize);

  WtMH_init(&MH, *MHproposaltype, *MHproposalpackage, inputs, *fVerbose, nw);

  WtSANSample (&MH,
	      theta0, invcov, tau, sample, *samplesize,
	      *burnin, *interval,
	      *fVerbose, nw, m);
  
  WtMH_free(&MH);

  /* record new generated network to pass back to R */
  newnetworkheads[0]=newnetworktails[0]=WtEdgeTree2EdgeList(newnetworkheads+1,newnetworktails+1,newnetworkweights+1,nw,nmax);

  WtModelDestroy(m);
  WtNetworkDestroy(nw);
  if (nedges[1]>0)
    WtNetworkDestroy(&nw[1]);
  PutRNGstate();  /* Disable RNG before returning */
}


/*********************
 void WtSANSample

 Using the parameters contained in the array theta, obtain the
 network statistics for a sample of size samplesize.  burnin is the
 initial number of Markov chain steps before sampling anything
 and interval is the number of MC steps between successive 
 networks in the sample.  Put all the sampled statistics into
 the networkstatistics array. 
*********************/
void WtSANSample (WtMHproposal *MHp,
  double *theta, double *invcov, double *tau, double *networkstatistics, 
  int samplesize, int burnin, 
  int interval, int fVerbose,
  WtNetwork *nwp, WtModel *m) {
  int staken, tottaken, ptottaken;
  int i, j, components, diam;
  
  components = diam = 0;
  
  /*********************
  networkstatistics are modified in groups of m->n_stats, and they
  reflect the CHANGE in the values of the statistics from the
  original (observed) network.  Thus, when we begin, the initial 
  values of the first group of m->n_stats networkstatistics should 
  all be zero
  *********************/
//  for (j=0; j < m->n_stats; j++)
//    networkstatistics[j] = 0.0;

  /*********************
   Burn in step.  While we're at it, use burnin statistics to 
   prepare covariance matrix for Mahalanobis distance calculations 
   in subsequent calls to M-H
   *********************/
  WtSANMetropolisHastings(MHp, theta, invcov, tau, networkstatistics, burnin, &staken,
		      fVerbose, nwp, m);
  
  if (fVerbose){
    Rprintf("Returned from SAN Metropolis-Hastings burnin\n");
  }
  
  if (samplesize>1){
    staken = 0;
    tottaken = 0;
    ptottaken = 0;
    
    /* Now sample networks */
    for (i=1; i < samplesize; i++){
      /* Set current vector of stats equal to previous vector */
      for (j=0; j<m->n_stats; j++){
        networkstatistics[j+m->n_stats] = networkstatistics[j];
      }
      networkstatistics += m->n_stats;
      /* This then adds the change statistics to these values */
      
      WtSANMetropolisHastings (MHp, theta, invcov, tau, networkstatistics, 
		             interval, &staken,
			     fVerbose, nwp, m);
      tottaken += staken;
      if (fVerbose){
        if( ((3*i) % samplesize)==0 && samplesize > 500){
        Rprintf("Sampled %d from SAN Metropolis-Hastings\n", i);}
      }
      
      if( ((3*i) % samplesize)==0 && tottaken == ptottaken){
        ptottaken = tottaken; 
        Rprintf("Warning:  SAN Metropolis-Hastings algorithm has accepted only "
        "%d steps out of a possible %d\n",  ptottaken-tottaken, i); 
      }
    }
    /*********************
    Below is an extremely crude device for letting the user know
    when the chain doesn't accept many of the proposed steps.
    *********************/
    if (fVerbose){
      Rprintf("SAN Metropolis-Hastings accepted %7.3f%% of %d steps.\n",
	      tottaken*100.0/(1.0*interval*samplesize), interval*samplesize); 
    }
  }else{
    if (fVerbose){
      Rprintf("SAN Metropolis-Hastings accepted %7.3f%% of %d steps.\n",
	      staken*100.0/(1.0*burnin), burnin); 
    }
  }
}

/*********************
 void WtSANMetropolisHastings

 In this function, theta is a m->n_stats-vector just as in SANSample,
 but now networkstatistics is merely another m->n_stats-vector because
 this function merely iterates nsteps times through the Markov
 chain, keeping track of the cumulative change statistics along
 the way, then returns, leaving the updated change statistics in
 the networkstatistics vector.  In other words, this function 
 essentially generates a sample of size one
*********************/
void WtSANMetropolisHastings (WtMHproposal *MHp,
			    double *theta, double *invcov, 
			    double *tau, double *networkstatistics,
			    int nsteps, int *staken,
			    int fVerbose,
			    WtNetwork *nwp,
			    WtModel *m) {
  int step, taken;
  int i,j;
  double ip,dif;
  double *deltainvsig, *delta;
  deltainvsig = (double *)malloc( m->n_stats * sizeof(double));
  delta = (double *)malloc( m->n_stats * sizeof(double));
  
//  div=0.0;
//    Rprintf("\n");
//for (i=0; i<m->n_stats; i++){
//  div += (networkstatistics[i])*(networkstatistics[i]);
//  Rprintf("i %d %f\n",i,networkstatistics[i]);
//}

  step = taken = 0;
/*  if (fVerbose)
    Rprintf("Now proposing %d WtMH steps... ", nsteps); */
  while (step < nsteps) {
    MHp->ratio = 1.0;
    (*(MHp->func))(MHp, nwp); /* Call MH function to propose toggles */
    //      Rprintf("Back from proposal; step=%d\n",step);

    WtChangeStats(MHp->ntoggles, MHp->togglehead, MHp->toggletail, MHp->toggleweight, nwp, m);
      
    dif=0.0;
    ip=0.0;
    /* Calculate inner product */
    for (i=0; i<m->n_stats; i++){
     delta[i]=0.0;
     deltainvsig[i]=0.0;
     for (j=0; j<m->n_stats; j++){
      delta[i]+=networkstatistics[j]*invcov[i+(m->n_stats)*j];
      deltainvsig[i]+=(m->workspace[j])*invcov[i+(m->n_stats)*j];
     }
     ip+=deltainvsig[i]*((m->workspace[i])+2.0*networkstatistics[i]);
     dif+=delta[i]*networkstatistics[i];
    }
//Rprintf("i %d j %d ic %f\n",i,j,invcov[i+(m->n_stats)*j]);
//if(i<=1){Rprintf("i %d %f %f\n",i,div,div*div*dif/(tau[i]*asig2[i]));}
//Rprintf(" ip %f dif %f\n",ip,dif);
//Rprintf("ip %f div %f networkstatistics[0] %f networkstatistics[1] %f\n",
//	 ip,div,networkstatistics[0],networkstatistics[1]);
//  Rprintf("ip %f m->workspace[i] %f ns %f asig2[0] %f div %f \n",ip, m->workspace[0],networkstatistics[0],asig2[0],div);
// Rprintf("step %d tau[0] %f tau[1] %f div %f \n",step, tau[0],tau[1],div);
      
    /* if we accept the proposed network */
    if (ip <= 0.0) { 
//  if (ip <= 0.0 || (ip/dif) < 0.001) { 
//  if (div > 0.0 && (ip < 0.0 || unif_rand() < 0.01)) { 
// if (ip <= 0.0 || (ip/dif) < (nsteps-step)*0.001*tau[0]/(1.0*nsteps)) { 
//  if (ip > exp(theta[0])*(m->n_stats)*unif_rand()/(1.0+exp(theta[0])) { 
//  if (ip > tau[0]*(m->n_stats)*unif_rand()) { 
      /* Make proposed toggles (updating timestamps--i.e., for real this time) */
      for (i=0; i < MHp->ntoggles; i++){
        WtSetEdge(MHp->togglehead[i], MHp->toggletail[i], MHp->toggleweight[i], nwp);
      }
      /* record network statistics for posterity */
      for (i = 0; i < m->n_stats; i++){
        networkstatistics[i] += m->workspace[i];
      }
//  div=0.0;
//    for (i=0; i<m->n_stats; i++){
//      div += (networkstatistics[i])*(networkstatistics[i]);
//    }
      taken++;
    }
    step++;
  }
/*  if (fVerbose)
    Rprintf("%d taken (MCMCtimer=%d)\n", taken, nwp->duration_info.MCMCtimer); */

  free(deltainvsig);
  free(delta);

  *staken = taken;
}