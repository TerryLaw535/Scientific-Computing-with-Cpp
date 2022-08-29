#include <iostream>
#include <cassert>
#include <cmath>
#include <fstream>

#include "SymplecticEulerSolver.hpp"

//Specialised constructor
SymplecticEulerSolver::SymplecticEulerSolver ( ODEInterface & anODESystem ,
                          const double initialState,
                          const double initialVelocity,
                          const double initialTime,
                          const double finalTime,
                          const double stepSize,
                          const std::string outputFileName,
                          const int saveGap,
                          const int printGap)
{
    //Initialising protected variables inherited from AbstractODESolver
    mInitialTime=initialTime;
    mFinalTime=finalTime;
    mpODESystem=&anODESystem;
    assert(stepSize>0);//Make sure that the value of step size is bigger than 0
    mStepSize=stepSize;
    mInitialState=initialState;
    mInitialVelocity=initialVelocity;
    mOutputFileName=outputFileName;
    assert(saveGap>=0 && printGap>=0);//Make sure that the value of saveGap and printGap is nonnegative
    mSaveGap=saveGap;
    mPrintGap=printGap;
}

//Solution method
void SymplecticEulerSolver::Solve()
{
    // Print header (inherited from abstract class) in order to provide info about the initial value of t,x and v
    std::string solverName="symplectic Euler method";
    PrintHeader(solverName);

    //Setting the initial value of t,x and v
    double t=mInitialTime;
    double x=mInitialState;
    double v=mInitialVelocity;

    //Define the right-hand side function of (6) f
    double f;

    //Setting the output file
    std::ofstream output_file;

    //Setting the name of the file and open it
    std::string file_name=mOutputFileName;
    output_file.open(file_name,std::ios::app);
    assert(output_file.is_open());

    //Save and print the initial value of t,x and v
    std::cout<<t<<"   "<<x<<"   "<<v<<std::endl;
    output_file<<t<<" "<<x<<" "<<v<<std::endl;

    //Define a iterator n
    int n=0;

    //Start iterations
    while(t<=mFinalTime)
    {
        //Iteration
        mpODESystem->ComputeF(t,x,f);
        v=v+mStepSize*f;
        x=x+mStepSize*v;
        t=t+mStepSize;
        n++;

        //Print the selected elements of t,x and v
        if(n%mPrintGap==0)
        {
            PrintIteration(t,x,v);
        }

        if(n%mSaveGap==0)
        {
            output_file<<t<<" "<<x<<" "<<v<<std::endl;
        }
    }

    //Close file
    output_file.close();
}

//Compute the error of using symplectic Euler method
double SymplecticEulerSolver::ComputeError()
{
    //Define the error and error_new in order to help compute the error
    double error;
    double error_new;

    //Setting the initial value of t,x and v
    double t=mInitialTime;
    double x=mInitialState;
    double v=mInitialVelocity;

    //Define the right-hand side function of (6) f
    double f;

    //Calculate the error at the initial time
    double xExact;
    mpODESystem->ComputeAnalyticSolution(t,xExact);
    error=fabs(x-xExact);

    //Start iterations
    while(t<=mFinalTime)
    {
        //Iteration
        mpODESystem->ComputeF(t,x,f);
        v=v+mStepSize*f;
        x=x+mStepSize*v;
        t=t+mStepSize;
        mpODESystem->ComputeAnalyticSolution(t,xExact);
        error_new=fabs(x-xExact);
        //Find the max error during the iteration
        if(error_new>error)
        {
            error=error_new;
        }
    }

    return error;
}
