//////////////////////////////////////////////////////////////////////////
//////////////////            singular5.cxx          /////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////           PSOPT  Example             ////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////// Title:                 Singular problem          ////////////////
//////// Last modified:         09 January 2009           ////////////////
//////// Reference:             Luus (2002)               ////////////////
//////// (See PSOPT handbook for full reference)          ////////////////
//////////////////////////////////////////////////////////////////////////
////////     Copyright (c) Victor M. Becerra, 2009        ////////////////
//////////////////////////////////////////////////////////////////////////
//////// This is part of the PSOPT software library, which ///////////////
//////// is distributed under the terms of the GNU Lesser ////////////////
//////// General Public License (LGPL)                    ////////////////
//////////////////////////////////////////////////////////////////////////

#include "psopt.h"

  


//////////////////////////////////////////////////////////////////////////
///////////////////  Define the end point (Mayer) cost function //////////
//////////////////////////////////////////////////////////////////////////

adouble endpoint_cost(adouble* initial_states, adouble* final_states, 
                      adouble* parameters,adouble& t0, adouble& tf, 
                      adouble* xad, int iphase, Workspace* workspace)
{
    return 0.0;
} 

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the integrand (Lagrange) cost function  //////
//////////////////////////////////////////////////////////////////////////

adouble integrand_cost(adouble* states, adouble* controls, 
                       adouble* parameters, adouble& time, adouble* xad, 
                       int iphase, Workspace* workspace)
{
   adouble x1 = states[ CINDEX(1) ];
   adouble x2 = states[ CINDEX(2) ];
   adouble x3 = states[ CINDEX(3) ];
   adouble u = controls[ CINDEX(1) ];
   adouble t = time;
   adouble L;

   L =  x1*x1 + x2*x2 + 0.0005*pow(x2+16*t-8.0-0.1*x3*u*u,2.0);

   return L;
} 

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the DAE's ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void dae(adouble* derivatives, adouble* path, adouble* states, 
         adouble* controls, adouble* parameters, adouble& time, 
         adouble* xad, int iphase, Workspace* workspace)
{

   adouble x1 = states[ CINDEX(1) ];
   adouble x2 = states[ CINDEX(2) ];
   adouble x3 = states[ CINDEX(3) ];

   adouble u = controls[ CINDEX(1) ];
   adouble t = time;

   derivatives[ CINDEX(1) ] = x2;
   derivatives[ CINDEX(2) ] = -x3*u + 16*t - 8.0;
   derivatives[ CINDEX(3) ] =  u;

}

////////////////////////////////////////////////////////////////////////////
///////////////////  Define the events function ////////////////////////////
////////////////////////////////////////////////////////////////////////////

void events(adouble* e, adouble* initial_states, adouble* final_states, 
            adouble* parameters,adouble& t0, adouble& tf, adouble* xad, 
            int iphase, Workspace* workspace) 
{
   adouble x10 = initial_states[ CINDEX(1) ];
   adouble x20 = initial_states[ CINDEX(2) ];
   adouble x30 = initial_states[ CINDEX(3) ];

   e[ CINDEX(1) ] = x10;
   e[ CINDEX(2) ] = x20;
   e[ CINDEX(3) ] = x30;


}


///////////////////////////////////////////////////////////////////////////
///////////////////  Define the phase linkages function ///////////////////
///////////////////////////////////////////////////////////////////////////

void linkages( adouble* linkages, adouble* xad, Workspace* workspace)
{

}

////////////////////////////////////////////////////////////////////////////
///////////////////  Define the main routine ///////////////////////////////
////////////////////////////////////////////////////////////////////////////


int main(void)
{

////////////////////////////////////////////////////////////////////////////
///////////////////  Declare key structures ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    Alg  algorithm;
    Sol  solution;
    Prob problem;
    MSdata msdata;

////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem name  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.name        		= "Singular control 5";
    problem.outfilename                 = "sing5.txt";

////////////////////////////////////////////////////////////////////////////
////////////  Define problem level constants & do level 1 setup ////////////
////////////////////////////////////////////////////////////////////////////

    problem.nphases   			= 1;
    problem.nlinkages                   = 0;

    psopt_level1_setup(problem);

/////////////////////////////////////////////////////////////////////////////
/////////   Define phase related information & do level 2 setup  ////////////
/////////////////////////////////////////////////////////////////////////////

    problem.phases(1).nstates   		= 3;
    problem.phases(1).ncontrols 		= 1;
    problem.phases(1).nevents   		= 3;
    problem.phases(1).npath                     = 0;
    problem.phases(1).nodes                     = "[80]"; 

    psopt_level2_setup(problem, algorithm);


////////////////////////////////////////////////////////////////////////////
///////////////////  Enter problem bounds information //////////////////////
////////////////////////////////////////////////////////////////////////////


    problem.phases(1).bounds.lower.states(1) = -inf;
    problem.phases(1).bounds.lower.states(2) = -inf;
    problem.phases(1).bounds.lower.states(3) = -inf;


    problem.phases(1).bounds.upper.states(1) = inf;
    problem.phases(1).bounds.upper.states(2) = inf;
    problem.phases(1).bounds.upper.states(3) = inf;

    problem.phases(1).bounds.lower.controls(1) =  0.0;
    problem.phases(1).bounds.upper.controls(1) =  10.0;

    problem.phases(1).bounds.lower.events(1) = 0.0;
    problem.phases(1).bounds.lower.events(2) = -1.0;
    problem.phases(1).bounds.lower.events(3) = -sqrt(5.0);
  
    problem.phases(1).bounds.upper.events(1) = 0.0;
    problem.phases(1).bounds.upper.events(2) = -1.0;
    problem.phases(1).bounds.upper.events(3) = -sqrt(5.0);

    problem.bounds.lower.times = "[0.0,  1.0]";
    problem.bounds.upper.times = "[0.0,  1.0]";
 

////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem functions  ///////////////////////////
////////////////////////////////////////////////////////////////////////////


    problem.integrand_cost 	= &integrand_cost;
    problem.endpoint_cost 	= &endpoint_cost;
    problem.dae 		= &dae;
    problem.events 		= &events;
    problem.linkages		= &linkages;

////////////////////////////////////////////////////////////////////////////
///////////////////  Define & register initial guess ///////////////////////
////////////////////////////////////////////////////////////////////////////

    DMatrix state_guess(3,20), control_guess(1,20), param_guess, time_guess;
   
    state_guess(1,colon())= linspace(0.0, 0.0, 20);
    state_guess(2,colon())= linspace(-1.0, -1.0, 20);
    state_guess(3,colon())= linspace(-sqrt(5.0),-sqrt(5.0), 20);

    control_guess = zeros(1,20);
 
    time_guess = linspace(0.0, 1.0, 20);

    auto_phase_guess(problem, control_guess, state_guess, param_guess, time_guess);

////////////////////////////////////////////////////////////////////////////
///////////////////  Enter algorithm options  //////////////////////////////
////////////////////////////////////////////////////////////////////////////

    algorithm.nlp_iter_max               =  1000;
    algorithm.nlp_tolerance              =  1.e-7;
    algorithm.nlp_method                 = "IPOPT";
    algorithm.scaling                    = "automatic";
    algorithm.derivatives                = "automatic";
    algorithm.defect_scaling             = "jacobian-based";
//    algorithm.collocation_method         = "trapezoidal";
//    algorithm.mesh_refinement            = "automatic";
//    algorithm.ode_tolerance              = 5.e-3	;
//    algorithm.mr_max_iterations          = 3;
   

////////////////////////////////////////////////////////////////////////////
///////////////////  Now call PSOPT to solve the problem   /////////////////
////////////////////////////////////////////////////////////////////////////

    psopt(solution, problem, algorithm);

////////////////////////////////////////////////////////////////////////////
///////////  Extract relevant variables from solution structure   //////////
////////////////////////////////////////////////////////////////////////////


    DMatrix x, u, t, xi, ui, ti;

    x      = solution.get_states_in_phase(1);
    u      = solution.get_controls_in_phase(1);
    t      = solution.get_time_in_phase(1);



////////////////////////////////////////////////////////////////////////////
///////////  Save solution data to files if desired ////////////////////////
////////////////////////////////////////////////////////////////////////////

    x.Save("x.dat");
    u.Save("u.dat");
    t.Save("t.dat");

////////////////////////////////////////////////////////////////////////////
///////////  Plot some results if desired (requires gnuplot) ///////////////
////////////////////////////////////////////////////////////////////////////

    plot(t,x,problem.name,"time (s)", "x");
    plot(t,u,problem.name,"time (s)", "u");
    plot(t,x,problem.name,"time (s)", "x","x1 x2 x3",
                          "pdf","sing5_states.pdf");
    plot(t,u,problem.name,"time (s)", "u","u",
  			   "pdf","sing5_control.pdf");

}

////////////////////////////////////////////////////////////////////////////
///////////////////////      END OF FILE     ///////////////////////////////
////////////////////////////////////////////////////////////////////////////
