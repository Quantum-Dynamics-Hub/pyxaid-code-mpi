# Beocat - module load Boost/1.63.0-foss-2017beocatb-Python-2.7.13
#FLAGS= -fno-for-scope -O2 -fPIC
FLAGS= -fno-for-scope -g -O2 -fPIC -std=c++98
CPP=c++
# BOOST
# UB CCR
#p1=/util/academic/boost/v1.57.0/include/boost
#pl1=/util/academic/boost/v1.57.0/lib
p1=/usr/include
pl1=/usr/lib

# PYTHON
# UB CCR
#p2=/util/academic/python/2.7.6/include/python2.7
#pl2=/util/academic/python/2.7.6/lib
#p2=/usr/include/python2.7
p2=/opt/software/software/Python/2.7.13-iomkl-2017beocatb/include/python2.7
pl2=/usr/lib


I=-I ${p1} -I ${p2}
L=-L ${pl1} -L ${pl2}

all: pyxaid_core.so

mytimer.o: mytimer.cpp mytimer_cpp.h
	${CPP} ${FLAGS} ${I} -c mytimer.cpp

random.o: random.cpp random.h
	${CPP} ${FLAGS} ${I} -c random.cpp

matrix.o: matrix.cpp matrix.h
	${CPP} ${FLAGS} ${I} -c matrix.cpp

state.o: state.cpp state.h
	${CPP} ${FLAGS} ${I} -c state.cpp

io.o: io.cpp io.h
	${CPP} ${FLAGS} ${I} -c io.cpp

InputStructure.o: InputStructure.cpp InputStructure.h
	${CPP} ${FLAGS} ${I} -c InputStructure.cpp

ElectronicStructure.o: ElectronicStructure.cpp ElectronicStructure.h
	${CPP} ${FLAGS} ${I} -c ElectronicStructure.cpp

namd.o: namd.cpp namd.h
	${CPP} ${FLAGS} ${I} -c namd.cpp

namd_export.o: namd_export.cpp namd_export.h
	${CPP} ${FLAGS} ${I} -c namd_export.cpp

aux.o: aux.cpp aux.h
	${CPP} ${FLAGS} ${I} -c aux.cpp

wfc_basic_methods.o: wfc_basic_methods.cpp wfc.h
	${CPP} ${FLAGS} ${I} -c wfc_basic_methods.cpp

wfc_QE_methods.o: wfc_QE_methods.cpp wfc.h aux.cpp
	${CPP} ${FLAGS} ${I} -c wfc_QE_methods.cpp

wfc_functions.o: wfc_functions.cpp wfc.h
	${CPP} ${FLAGS} ${I} -c wfc_functions.cpp

wfc_export.o: wfc_basic_methods.o wfc_QE_methods.o wfc_functions.o wfc_export.cpp
	${CPP} ${FLAGS} ${I} -c wfc_export.cpp

pyxaid_core.o: pyxaid_core.cpp 
	${CPP} ${FLAGS} ${I} -c pyxaid_core.cpp



pyxaid_core.so: pyxaid_core.o wfc_export.o wfc_functions.o wfc_QE_methods.o wfc_basic_methods.o \
        aux.o matrix.o state.o ElectronicStructure.o namd.o namd_export.o InputStructure.o io.o random.o mytimer.o
	${CPP} ${FLAGS} ${I} -shared -o pyxaid_core.so pyxaid_core.o wfc_export.o wfc_functions.o \
        wfc_QE_methods.o wfc_basic_methods.o aux.o matrix.o state.o ElectronicStructure.o namd.o \
        namd_export.o InputStructure.o io.o random.o mytimer.o ${L} -lboost_python
	cp pyxaid_core.so ../.
#        namd_export.o InputStructure.o io.o random.o ${L} -lboost_python-2.7

clean:
	rm *.o
	rm pyxaid_core.so
	rm ../pyxaid_core.so

