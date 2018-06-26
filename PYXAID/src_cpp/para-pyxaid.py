from PYXAID import *
import os

# Do the MPI setup and add myproc and nprocs to the parameter list to pass to C++
from mpi4py import MPI

comm = MPI.COMM_WORLD
myproc = comm.Get_rank()
nprocs = comm.Get_size()

hostname = os.uname()[1]

ham_dir = "/homes/daveturner/apps/pyxaid.mpi4py"     # Where res.tar.gz is

scr_dir = "/tmp"
#scr_dir = os.getwcd() + "/scratch"     # No scratch, just use a directory on CWD

# gunzip and untar the hamiltonian directory to scratch, or /tmp on each node
# This is fast and easy to do, but may only help on some file systems
# NOTE: You therefore need res.tar.gz in the Hamiltonian directory ham_dir

t0 = os.times()[4]
for i in range(0,nprocs):
   comm.barrier()
   if myproc == i :
      os.system("mkdir -p " + scr_dir + "/marker.dir" )  # Dummy dir for tests later
      #print("Proc " + str(myproc) + " says hello from " + hostname )
      if not os.path.isdir( scr_dir + "/res" ) :
         print( str(myproc) + " loading res onto " + scr_dir + " on " + hostname )
         os.system( "tar -xzf res.tar.gz -C " + scr_dir )
comm.barrier()
tres = os.times()[4] - t0

params = {}
params["myproc"] = myproc
params["nprocs"] = nprocs

#############################################################################################
# Input section: Here everything can be defined in programable way, not just in strict format
#############################################################################################

# Define general control parameters (file names, directories, etc.)
# Path to Hamiltonians
# These paths must direct to the folder that contains the results of
# the step2 calculations (Ham_ and (optinally) Hprime_ files) and give
# the prefixes and suffixes of the files to read in
#rt = "/homes/daveturner/apps/pyxaid.mpi4py"
#rt = "/tmp"
rt = scr_dir
params["Ham_re_prefix"] = rt+"/res/0_Ham_"
params["Ham_re_suffix"] = "_re"
params["Ham_im_prefix"] = rt+"/res/0_Ham_"
params["Ham_im_suffix"] = "_im"
params["Hprime_x_prefix"] = rt + "/res/0_Hprime_"
params["Hprime_x_suffix"] = "x_re"
params["Hprime_y_prefix"] = rt + "/res/0_Hprime_"
params["Hprime_y_suffix"] = "y_re"
params["Hprime_z_prefix"] = rt + "/res/0_Hprime_"
params["Hprime_z_suffix"] = "z_re"
params["energy_units"] = "Ry"                # This specifies the units of the Hamiltonian matrix elements as they
                                             # are written in Ham_ files. Possible values: "Ry", "eV"

# Set up other simulation parameters:

params["scratch_dir"] = scr_dir


params["read_couplings"] = "batch"           # How to read all input (Ham_ and Hprime_) files. Possible values:
                                             # "batch", "online"

# Simulation type
params["runtype"] = "namd"                   # Type of calculation to perform. Possible values:
                                             # "namd" - to do NA-MD calculations, "no-namd"(or any other) - to
                                             # perform only pre-processing steps - this will create the files with
                                             # the energies of basis states and will output some useful information,
                                             # it may be particularly helpful for preparing your input
params["decoherence"] = 1                    # Do you want to include decoherence via DISH? Possible values:
                                             # 0 - no, 1 - yes
params["is_field"] = 0                       # Do you want to include laser excitation via explicit light-matter
                                             # interaction Hamiltonian? Possible values: 0 - no, 1 - yes

# Integrator parameters
params["elec_dt"] = 1.0                      # Electronic integration time step, fs
params["nucl_dt"] = 1.0                      # Nuclear integration time step, fs (this parameter comes from 
                                             # you x.md.in file)
params["integrator"] = 0                     # Integrator to solve TD-SE. Possible values: 0, 10,11, 2

# NA-MD trajectory and SH control 
params["namdtime"] = 3500                      # Trajectory time, fs
params["num_sh_traj"] = 1000                 # Number of stochastic realizations for each initial condition
params["boltz_flag"] = 1                     # Boltzmann flag (set to 1 anyways)
params["Temp"] = 300.0                       # Temperature of the system
params["alp_bet"] = 0                        # How to treat spin. Possible values: 0 - alpha and beta spins are not
                                             # coupled to each other, 1 - don't care about spins, only orbitals matter

params["debug_flag"] = 0                     # If you want extra output. Possible values: 0, 1, 2, ...
                                             # as the number increases the amount of the output increases too
                                             # Be carefull - it may result in a huge output!

# Parameters of the field (if it is included)
params["field_dir"] = "xyz"                 # Direction of the field. Possible values: "x","y","z","xy","xz","yz","xyz"
params["field_protocol"] = 1                # Envelope function. Possible values: 1 - step function, 2 - saw-tooth
params["field_Tm"] = 25.0                   # Middle of the time interval during which the field is active
params["field_T"] = 25.0                    # The period (duration) of the field pulse
params["field_freq"] = 3.0                  # The frequency of the field radiation = energy of the photons
params["field_freq_units"] = "eV"           # Units of the above quantity. Possible values: "eV", "nm","1/fs","rad/fs"
params["field_fluence"] = 1.0               # Defines the light radiation intensity (fluence), mJ/cm^2



# Define states:
# Example of indexing convention with Nmin = 5, HOMO = 5, Nmax = 8
# the orbitals indices are consistent with QE (e.g. PP or DOS) indexing, which starts from 1
# [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11] - all computed orbitals
# [ 1, 2, 3, 4, 5, 6]                     - occupied orbitals
#                   [ 7, 8, 9, 10, 11] - unoccupied orbitals
#              [5, 6, 7, 8]            - active space


# Set active space and the basis states

params["active_space"] = [6,7,8,9,10]

params["states"] = []
params["states"].append(["GS",[6,-6,7,-7,8,-8],0.00])  # ground state
params["states"].append(["S1",[6,-6,7,-7,9,-8],0.00])
params["states"].append(["S2",[6,-6,7,-7,10,-8],0.00])
params["states"].append(["S3",[6,-6,9,-7,8,-8],0.00])
params["states"].append(["S4",[6,-6,10,-7,8,-8],0.00])
params["states"].append(["S5",[9,-6,7,-7,8,-8],0.00])
params["states"].append(["S6",[10,-6,7,-7,8,-8],0.00])


# Initial conditions
nmicrost = len(params["states"])
ic = []

i = 0
while i<10:
    j = 0
    while j<nmicrost:
        ic.append([50*i,j])
        j = j + 1
    i = i + 1

params["iconds"] = ic



###################################################################################
# Execution section: Here we actually start the NA-MD calculations and the analysis
###################################################################################

if myproc == 0:
   print params                   # print out all simulation parameters first
   pyxaid_core.info().version()


################################ Run calculations #################################
# Have each proc run namd() for its iconds as split using myproc and nprocs
###################################################################################

t0 = os.times()[4]

pyxaid_core.namd(params)

comm.barrier()
tpyxaid_core = os.times()[4] - t0
if myproc == 0 :
   print( "All procs have finished pyxaid_core.namd(params)" )


###################################################################################
# Below we will be using the average.py module
# Note: If you want to re-run averaging calculations - just comment out the line
# calling namd() functions (or may be some other unnecessary calculations)
###################################################################################

Nstates = len(params["states"])  # Total number of basis states

opt = 12        # Defines the type of the averaging we want to do. Possible values:
                # 1 - average over intial conditions, independnetly for each state
                # 2 - sum the averages for groups of states (calculations with opt=1 must
                # already be done). Can do this for different groups of states without 
                # recomputing initial-conditions averages - they stay the same
                # 12 - do the steps 1 and 2 one after another

# Set groups of states to get the total population as a function of time

MS = []
for i in range(0,Nstates):
    MS.append([i])   # Choose each microstate to contain a single basis config

###################################################################################
# Create a directory for the averages and move non-icond*.txt files from scratch there too
# Some parallel file systems like CEPH simply can't handle millions of files
# in a single directory so tar and gzip the icond*.txt files to the cwd.
###################################################################################
# If you are not using local /tmp on each node, you can reprogram the code below
# to leave the files in the global scratch and manage them outside of the job script.
###################################################################################

t0 = os.times()[4]
tarfile = os.getcwd()+"/spectral_density.tar"
res_dir = os.getcwd()+"/macro"
if myproc == 0:
   os.system("mkdir -p " + res_dir)     # Make res_dir if it is not there

for i in range(0,nprocs):
   comm.barrier()
   if myproc == i and os.path.isdir( scr_dir + "/marker.dir" ) :
      print( str(myproc) + " on " + hostname + " is moving and tarring files to cwd")
      os.system( "mv -f " + scr_dir + "/[mdo][eu]* " + res_dir )
      os.system( "cd " + scr_dir + "; find -name '*Spectral_density.txt' | tar -rf " + tarfile + " -T -" )
      os.system( "rmdir " + scr_dir + "/marker.dir" )

comm.barrier()
if myproc == 0:
   print("Proc 0 is gzipping " + tarfile )
   os.system( "gzip " + tarfile )
tmove = os.times()[4] - t0

                                # This is where the averaged results will be written

# Each proc will do Nstates/nprocs work in average.py
# This is write IO bound so I see ~6x speedup for large jobs which is good enough

t0 = os.times()[4]
average.average(params["namdtime"],Nstates,params["iconds"],opt,MS,res_dir,res_dir,myproc,nprocs)
taverage = os.times()[4] - t0

if myproc == 0 :
   print( "\n                Runtimes from para-pyxaid.py")
   print( str(int(tres)) + " seconds to gunzip and untar res.tar.gz to scratch" )
   print( str(int(tpyxaid_core)) + " seconds to run the pyxaid_core() C++ code" )
   print( str(int(tmove)) + " seconds to tar, gzip and move files from scratch to cwd")
   print( str(int(taverage)) + " seconds for proc 0 to run average.py" )

