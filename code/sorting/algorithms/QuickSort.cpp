#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

// === Configura aquí el directorio de entrada y salida ===
const fs::path INPUT_DIR  = "code/sorting/data/array_input";
const fs::path OUTPUT_DIR = "code/sorting/data/array_output";

// Function to swap two elements
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to generate a random pivot index
int generateRandomPivot(int low, int high) {
    srand(time(NULL));
    return low + rand() % (high - low + 1);
}

// Function to perform QuickSort
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pivotIndex = generateRandomPivot(low, high);
        int pivotValue = arr[pivotIndex];

        // Swap the pivot element with the last element
        swap(&arr[pivotIndex], &arr[high]);

        int i = low - 1;

        for (int j = low; j < high; j++) {
            if (arr[j] < pivotValue) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }

        // Swap the pivot element back to its final position
        swap(&arr[i+1], &arr[high]);

        // Recursively sort the left and right subarrays
        quickSort(arr, low, i);
        quickSort(arr, i+2, high);
    }
}

// ---- Procesar un archivo con QuickSort y guardar en OUTPUT_DIR ----
void procesarArchivo(const fs::path& inputPath, const fs::path& outDir) {
    ifstream in(inputPath);

    vector<int> arr;
    int x;
    while (in >> x) arr.push_back(x);
    in.close();

    size_t memBytes = arr.size() * sizeof(int);
    cout << "Procesando " << inputPath.filename().string()
         << " | n=" << arr.size()
         << " | ~mem=" << (memBytes / (1024.0*1024.0)) << " MB\n";

    auto start = chrono::high_resolution_clock::now();
    quickSort(arr.data(), 0, (int)arr.size() - 1);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // Construir nombre de salida: <nombre_entrada_sin_ext>_out.txt en otro directorio
    fs::path outName = inputPath.stem().string() + "_out" + inputPath.extension().string();
    fs::path outPath = outDir / outName;

    ofstream out(outPath);
    for (int v : arr) out << v << ' ';
    out << '\n';
    out.close();

    cout << "✅ " << outPath.string() << " | tiempo=" << elapsed.count() << " s\n";
}

int main() {
    // Semilla del RNG UNA sola vez
    srand(static_cast<unsigned>(time(nullptr)));

    // Asegura que el directorio de salida exista
    fs::create_directories(OUTPUT_DIR);

    // Recolecta archivos .txt de INPUT_DIR
    vector<fs::directory_entry> files;
    for (const auto& entry : fs::directory_iterator(INPUT_DIR)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            files.push_back(entry);
        }
    }

    // Ordena: primero por tamaño (menor a mayor), luego por fecha (más viejo a más nuevo)
    sort(files.begin(), files.end(),
        [](const fs::directory_entry& a, const fs::directory_entry& b) {
            auto sa = fs::file_size(a.path());
            auto sb = fs::file_size(b.path());
            if (sa == sb) return fs::last_write_time(a) < fs::last_write_time(b);
            return sa < sb;
        });

    auto globalStart = chrono::high_resolution_clock::now();
    for (const auto& e : files) procesarArchivo(e.path(), OUTPUT_DIR);
    auto globalEnd = chrono::high_resolution_clock::now();
    cout << "\nTiempo total: "
         << chrono::duration<double>(globalEnd - globalStart).count()
         << " s\n";

    return 0;
}

// https://www.geeksforgeeks.org/dsa/quicksort-using-random-pivoting/