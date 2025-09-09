# Nombre del compilador
CXX = g++

# Opciones de compilación
CXXFLAGS = -Wall -O2 -std=c++17

# Regla por defecto: compilar todos
all: quicksort insertionsort pandasort sort mergesort

# Compilar quicksort
quicksort: algorithms/quicksort.cpp
	$(CXX) $(CXXFLAGS) algorithms/quicksort.cpp -o quicksort

# Ejecutar quicksort
runquicksort: quicksort
	./quicksort

# Compilar insertionsort
insertionsort: algorithms/insertionsort.cpp
	$(CXX) $(CXXFLAGS) algorithms/insertionsort.cpp -o insertionsort

# Ejecutar insertionsort
runinsertionsort: insertionsort
	./insertionsort

# Compilar pandasort
pandasort: algorithms/pandasort.cpp
	$(CXX) $(CXXFLAGS) algorithms/pandasort.cpp -o pandasort

# Ejecutar pandasort
runpandasort: pandasort
	./pandasort

# Compilar sort
sort: algorithms/sort.cpp
	$(CXX) $(CXXFLAGS) algorithms/sort.cpp -o sort

# Ejecutar sort
runsort: sort
	./sort

# Compilar mergesort
mergesort: algorithms/mergesort.cpp
	$(CXX) $(CXXFLAGS) algorithms/mergesort.cpp -o mergesort

# Ejecutar mergesort
runmergesort: mergesort
	./mergesort

# Limpiar archivos generados
clean:
	rm -f quicksort insertionsort pandasort sort mergesort
