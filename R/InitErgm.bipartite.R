#  File R/InitErgm.bipartite.R in package ergm, part of the Statnet suite
#  of packages for network analysis, https://statnet.org .
#
#  This software is distributed under the GPL-3 license.  It is free,
#  open source, and has the attribution requirements (GPL Section 7) at
#  https://statnet.org/attribution
#
#  Copyright 2003-2019 Statnet Commons
#######################################################################
#  See InitErgm.R for a general explanation 
#  of InitErgm functions

# NOTE: a number of undocumented terms from this file have been removed 
# but the terms are retained on the experimental_terms svn branch

###################################### InitErgm TERMS:  A
##########################################################



#########################################################
InitErgmTerm.b1nodematch	<-	function (nw, arglist, ..., version=packageVersion("ergm")) {
  if(version <= as.package_version("3.9.4")){
    ### Check the network and arguments to make sure they are appropriate.
    a <- check.ErgmTerm(nw, arglist, directed = FALSE, bipartite = TRUE,
                varnames 		= c("attrname", "diff", "keep", "beta", "alpha", "byb2attr"), 				
                vartypes 		= c("character", "logical", "numeric", "numeric", "numeric", "character"), 	 
                defaultvalues = list(NULL, FALSE, NULL, 1, 1, NULL), 										 
                required 		= c(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE),
				dep.inform = list(FALSE, FALSE, "levels", FALSE, FALSE, FALSE))
	attrarg <- a$attrname
  }else{
    a <- check.ErgmTerm(nw, arglist, directed = FALSE, bipartite = TRUE,
                varnames 		= c("attr", "diff", "keep", "beta", "alpha", "byb2attr", "levels"), 				
                vartypes 		= c(ERGM_VATTR_SPEC, "logical", "numeric", "numeric", "numeric", ERGM_VATTR_SPEC, ERGM_LEVELS_SPEC), 	 
                defaultvalues = list(NULL, FALSE, NULL, 1, 1, NULL, NULL), 										 
                required 		= c(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE),
				dep.inform = list(FALSE, FALSE, "levels", FALSE, FALSE, FALSE, FALSE)) 								
    attrarg <- a$attr
  }
  ### Process the arguments
  if (!is.numeric(a$beta) || a$beta>1 || a$beta<0)
    ergm_Init_abort("beta argument to b1nodematch must be between 0 and 1 inclusive.")
  if (!is.numeric(a$alpha) || a$alpha>1 || a$alpha<0)
    ergm_Init_abort("alpha argument to b1nodematch must be between 0 and 1 inclusive.")
  
  nodecov <- ergm_get_vattr(attrarg, nw)
  attrname <- attr(nodecov, "name")
  
  b1.len	<-	get.network.attribute(nw, "bipartite")# gives # of b1 nodes
  u 		<-  ergm_attr_levels(a$levels, nodecov[1:b1.len], nw, sort(unique(nodecov[1:b1.len])))		  # gives unique attrnames of b1
  if((!hasName(attr(a,"missing"), "levels") || attr(a,"missing")["levels"]) && !is.null(a$keep)) u <- u[a$keep]
  
  #   Recode to numeric
  nodecov   <- match(nodecov, u, nomatch = length(u)+1)
  # All of the "nomatch" should be given unique IDs so they never match:
  dontmatch <- nodecov == (length(u)+1)
  nodecov[dontmatch] 	<- length(u) + (1:sum(dontmatch))
  ui 					<- seq(along = u)
  
  
   if (!is.null(a$byb2attr)) {										  			
  	b2nodecov 	<-	ergm_get_vattr(a$byb2attr, nw)
  	v 	 		<-  sort(unique(b2nodecov[-(1:b1.len)]))	# unique byb2attrnames of b2 
  	b2attrsize	<-	length(v)	# to get the levels of the byb2attr				
  																		    	
  	#   Recode to numeric														
  	b2nodecov   				<- match(b2nodecov, v, nomatch = length(v)+1)	
  	# All of the "nomatch" should be given unique IDs so they never match:		
  	b2dontmatch 				<- b2nodecov == (length(v)+1)					
  	b2nodecov[b2dontmatch] 		<- length(v) + (1:sum(b2dontmatch))				
  	vi							<- seq(along = v)         						
  	if (length(vi) < 2) {ergm_Init_abort("byb2attr should have at least two levels")}		
  	
  }	else {b2attrsize <- 0}	# to indicate that an b2attr does not exist	
  
  			
  ### Construct the list to return
  if (a$diff) {
  	if(!is.null(a$byb2attr)) {																	
    	coef.names 		<- unlist(lapply(paste("b1nodematch", paste(attrname, collapse="."), u, sep = "."), function(x){paste(x, v, sep = ".")}))																										   	
    	inputs 			<- c(b2attrsize, a$beta, a$alpha, ui, nodecov, vi, b2nodecov)    	
    }else{																						
    	coef.names 		<- paste("b1nodematch", paste(attrname,collapse="."), u, sep=".")		
    	inputs 			<- c(b2attrsize, a$beta, a$alpha, ui, nodecov)				
    }																							
   	 	
  } else {
  	
  	if(!is.null(a$byb2attr)) {																	
    	coef.names 		<- paste("b1nodematch", paste(attrname,collapse="."), v, sep=".")		
    	inputs 			<- c(b2attrsize, a$beta, a$alpha, nodecov, vi, b2nodecov)
    }else{																						
    	coef.names 		<- paste("b1nodematch", paste(attrname,collapse="."), sep=".")		
   		inputs 			<- c(b2attrsize, a$beta, a$alpha, nodecov)												
    }																							
 																					
  }							
  list(name			=  "b1nodematch",                       #name: required
       coef.names 	=  coef.names,                          #coef.names: required
       inputs 		=  inputs
       )
}



##########################################################
InitErgmTerm.b2nodematch	<-	function (nw, arglist, ..., version=packageVersion("ergm")) {
  if(version <= as.package_version("3.9.4")){
    ### Check the network and arguments to make sure they are appropriate.
    a <- check.ErgmTerm(nw, arglist, directed=FALSE, bipartite=TRUE,
                  varnames = c("attrname", "diff", "keep", "beta", "alpha", "byb1attr"),# RPB - 10/03/2012 - added the new arg "byb1attr"
                  vartypes = c("character", "logical", "numeric", "numeric", "numeric", "character"),
                  defaultvalues 	= list(NULL, FALSE, NULL, 1, 1, NULL),
                  required		= c(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE),
				  dep.inform = list(FALSE, FALSE, "levels", FALSE, FALSE, FALSE))
	attrarg <- a$attrname  
  }else{
    ### Check the network and arguments to make sure they are appropriate.
    a <- check.ErgmTerm(nw, arglist, directed=FALSE, bipartite=TRUE,
                  varnames = c("attr", "diff", "keep", "beta", "alpha", "byb1attr", "levels"),# RPB - 10/03/2012 - added the new arg "byb1attr"
                  vartypes = c(ERGM_VATTR_SPEC, "logical", "numeric", "numeric", "numeric", ERGM_VATTR_SPEC, ERGM_LEVELS_SPEC),
                  defaultvalues 	= list(NULL, FALSE, NULL, 1, 1, NULL, NULL),
                  required		= c(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE),
				  dep.inform = list(FALSE, FALSE, "levels", FALSE, FALSE, FALSE, FALSE))
    attrarg <- a$attr
  }
   ### Process the arguments
  if (!is.numeric(a$beta) || a$beta>1 || a$beta<0)
    ergm_Init_abort("beta argument to b2nodematch must be between 0 and 1 inclusive.")
  if (!is.numeric(a$alpha) || a$alpha>1 || a$alpha<0)
    ergm_Init_abort("alpha argument to b2nodematch must be between 0 and 1 inclusive.")
  
  nodecov <- ergm_get_vattr(attrarg, nw)
  attrname <- attr(nodecov, "name")
  
  b1.len 	<-	get.network.attribute(nw, "bipartite")# gives # of b1 nodes
  u 	 	<-  ergm_attr_levels(a$levels, nodecov[-(1:b1.len)], nw, sort(unique(nodecov[-(1:b1.len)])))	  # gives unique attrnames of b2's
   																  # gives unique byb1attrnames of b1's
  if((!hasName(attr(a,"missing"), "levels") || attr(a,"missing")["levels"]) && !is.null(a$keep)) u <- u[a$keep]
																  
  #   Recode to numeric
  nodecov 				<- match(nodecov, u, nomatch = length(u)+1)
  # All of the "nomatch" should be given unique IDs so they never match:
  dontmatch 			<- nodecov ==(length(u)+1)
  nodecov[dontmatch] 	<- length(u) + (1:sum(dontmatch))
  ui 					<- seq(along = u)
  
  if (!is.null(a$byb1attr)) {										# gives unique byb1attrnames of b1's
  	
  	b1nodecov 	<-	ergm_get_vattr(a$byb1attr, nw)												# get byb1attr vals

  	v 	 		<-  sort(unique(b1nodecov[1:b1.len]))# gives unique byb1attrnames of b1's
  	b1attrsize	<-	length(v)	# to get the levels of the byb1attr			
  	
  	#   Recode to numeric														
  	b1nodecov   				<- match(b1nodecov, v, nomatch = length(v)+1)	
  	# All of the "nomatch" should be given unique IDs so they never match:		
  	b1dontmatch 				<- b1nodecov == (length(v)+1)					
  	b1nodecov[b1dontmatch] 		<- length(v) + (1:sum(b1dontmatch))				
  	vi							<- seq(along = v)         						
  	
  	if (length(vi) < 2){ergm_Init_abort("byb1attr should have at least two levels")}		
  
  } else {b1attrsize <- 0}	# to indicate that an b1attr does not exist 	
   
  # if(drop) { # Check for zero statistics, print -Inf messages if applicable	# removed the whole if statement
    # obsstats <- check.ErgmTerm.summarystats(nw, arglist, ...)
    # ew <- extremewarnings(obsstats)
    # u <- u[!ew]
    # ui <- ui[!ew]
  # }
  ### Construct the list to return
  if (a$diff) {
  	if(!is.null(a$byb1attr)) {																	
    	coef.names 		<- unlist(lapply(paste("b2nodematch", paste(attrname, collapse="."), u, sep = "."), function(x){paste(x, v, sep = ".")}))																		
    	inputs 			<- c(b1attrsize, a$beta, a$alpha, ui, nodecov, vi, b1nodecov)
    }else{																						
    	coef.names 		<- paste("b2nodematch", paste(attrname,collapse="."), u, sep=".")		
    	inputs 			<- c(b1attrsize, a$beta, a$alpha, ui, nodecov)				
    }																							
   	
 	
  } else {
  	
  	if(!is.null(a$byb1attr)) {																	
    	coef.names 		<- paste("b2nodematch", paste(attrname,collapse="."), v, sep=".")		
    	inputs 			<- c(b1attrsize, a$beta, a$alpha, nodecov, vi, b1nodecov)	
    }else{																						
    	coef.names 		<- paste("b2nodematch", paste(attrname,collapse="."), sep=".")		
    	inputs 			<- c(b1attrsize, a$beta, a$alpha, nodecov)					
    }																							
    																						
  }	
  list(name="b2nodematch",                               #name: required
       coef.names = coef.names,                          #coef.names: required
       inputs =  inputs
       )
}



###################################### InitErgm TERMS:  B
#########################################################



###################################### InitErgm TERMS:  E
#########################################################


###################################### InitErgm TERMS:  G
#########################################################


###################################### InitErgm TERMS:  M
#########################################################



